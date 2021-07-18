#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "request.h"
#include "response.h"

response *create_response(const int conn_fd) {
    response *res = _initialize_response();
    if (res == NULL)
        return NULL;

    res->conn_fd = dup(conn_fd);
    return res;

    return NULL;
}

response *create_response_from_request(const request *req) {
    response *res = create_response(req->conn_fd);
    if (res == NULL)
        return NULL;

    res->http_ver = strdup(req->http_ver);
    return res;

    return NULL;
}

const char *get_response_header(const response *res, const char *header_key, char *header_val) {
    if (res == NULL || header_key == NULL)
        return NULL;

    const char *_header_val = NULL;
    if ((_header_val = g_hash_table_lookup(res->header_htab, header_key)) != NULL) {
        if (header_val != NULL)
            strcpy(header_val, _header_val);
        return _header_val;
    }

    return NULL;
}

const char *set_response_header(const response *res, const char *header_key,
                                const char *header_val) {
    if (res == NULL || header_key == NULL || header_val == NULL)
        return NULL;

    if (g_hash_table_lookup(res->header_htab, header_key) == NULL) {
        char *key = strdup(header_key);
        char *val = strdup(header_val);

        g_hash_table_insert(res->header_htab, key, val);
        return g_hash_table_lookup(res->header_htab, key);
    }

    return NULL;
}

// TODO: Only 1 send() call
ssize_t send_response_header(const response *res) {
    char buf[RES_HEADER_BUF_SIZE];
    ssize_t buf_size = 0, total_buf_size = 0;

    // HTTP Version and Status Code needs to be set.
    if (res->http_ver == NULL || res->status_code == NULL)
        return 0;

    // Sending first line of response head
    sprintf(buf, "%s %s\r\n", res->http_ver, res->status_code);
    buf_size = strlen(buf);
    if (send(res->conn_fd, buf, buf_size, 0) != buf_size)
        return total_buf_size;
    total_buf_size += buf_size;

    // TODO: Send response headers
    GHashTableIter iter;
    gpointer header_key, header_value;
    g_hash_table_iter_init(&iter, res->header_htab);

    while (g_hash_table_iter_next(&iter, &header_key, &header_value)) {
        sprintf(buf, "%s: %s\r\n", (char *)header_key, (char *)header_value);
        buf_size = strlen(buf);
        if (send(res->conn_fd, buf, buf_size, 0) != buf_size)
            return total_buf_size;
        total_buf_size += buf_size;
    }

    // Sending last line of response head
    strcpy(buf, "\r\n");
    buf_size = strlen(buf);
    if (send(res->conn_fd, buf, buf_size, 0) != buf_size)
        return total_buf_size;
    total_buf_size += buf_size;

    return total_buf_size;
}

ssize_t send_response_file(const response *res, FILE *file) {
    ssize_t total_buf_size = 0;
    size_t buf_size = 0, send_size = 0;
    char buf[RES_BUF_SIZE];

    while ((buf_size = fread(buf, 1, RES_BUF_SIZE, file)) > 0) {
        send_size = send(res->conn_fd, buf, buf_size, 0);
        if (send_size != buf_size)
            return 0;
        total_buf_size += send_size;
    }

    return total_buf_size;
}

ssize_t send_response(const response *res, const char *buf, ssize_t buf_size) {
    if (buf_size == -1)
        buf_size = strlen(buf);
    return send(res->conn_fd, buf, buf_size, 0);
}

void close_response(response *res) {
    if (res->conn_fd != -1) {
        close(res->conn_fd);
        res->conn_fd = -1;
    }
    _free_response(res);
}

response *_initialize_response() {
    response *res = malloc(sizeof(response));
    if (res == NULL)
        return NULL;

    res->conn_fd = -1;
    res->http_ver = NULL;
    res->status_code = NULL;
    if ((res->header_htab =
             g_hash_table_new_full(g_str_hash, g_str_equal, _res_header_htab_key_destroy,
                                   _res_header_htab_value_destroy)) == NULL)
        return NULL;

    return res;
}

void _free_response(response *res) {
    if (res == NULL)
        return;

    if (res->header_htab != NULL) {
        g_hash_table_destroy(res->header_htab);
        res->header_htab = NULL;
    }
    if (res->status_code != NULL) {
        free(res->status_code);
        res->status_code = NULL;
    }
    if (res->http_ver != NULL) {
        free(res->http_ver);
        res->http_ver = NULL;
    }

    free(res);
    res = NULL;
}

void _res_header_htab_key_destroy(gpointer data) {
    free(data);
    data = NULL;
}

void _res_header_htab_value_destroy(gpointer data) {
    free(data);
    data = NULL;
}
