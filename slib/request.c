#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "request.h"

request *get_request(const int conn_fd) {
    char req_buf[REQ_BUF_SIZE];
    if (recv(conn_fd, req_buf, REQ_BUF_SIZE, 0) < 0)
        return NULL;

    request *req = _initialize_request();
    if (req == NULL)
        return NULL;

    req->conn_fd = conn_fd;
    if (!_parse_request(req_buf, req))
        return NULL;

    return req;
}

request *parse_request(const char *req_buf, const int conn_fd) {
    request *req = _initialize_request();
    if (req == NULL)
        return NULL;

    if (!_parse_request(req_buf, req))
        return NULL;
    return req;
}

void close_request(request *req) {
    if (req->conn_fd != -1) {
        close(req->conn_fd);
        req->conn_fd = -1;
    }
    _free_request(req);
}

const char *get_request_header(const request *req, const char *header_key, char *header_val) {
    if (req == NULL || header_key == NULL)
        return NULL;

    const char *_header_val = NULL;
    if ((_header_val = g_hash_table_lookup(req->header_htab, header_key)) != NULL) {
        if (header_val != NULL)
            strcpy(header_val, _header_val);
        return _header_val;
    }

    return NULL;
}

request *_initialize_request() {
    request *req = malloc(sizeof(request));
    if (req == NULL)
        return NULL;

    req->conn_fd = -1;
    req->http_method = NULL;
    req->url = NULL;
    req->http_ver = NULL;
    if ((req->header_htab =
             g_hash_table_new_full(g_str_hash, g_str_equal, _req_header_htab_key_destroy,
                                   _req_header_htab_value_destroy)) == NULL)
        return NULL;

    return req;
}

int _parse_request(const char *req_buf, request *req) {
    if (req_buf == NULL)
        return 0;
    if (req == NULL)
        return 0;

    char *req_buff = strdup(req_buf);

    req->http_method = strdup(strtok(req_buff, " "));
    req->url = strdup(strtok(NULL, " "));
    req->http_ver = strdup(strtok(NULL, "\r"));

    char *header_key, *header_val;
    char *key, *value;

    while ((header_key = strtok(NULL, ":")) != NULL && (header_val = strtok(NULL, "\r")) != NULL) {
        key = strdup(header_key + 1);
        value = strdup(header_val + 1);
        g_hash_table_insert(req->header_htab, key, value);
    }

    free(req_buff);
    return 1;
}

void _free_request(request *req) {
    if (req == NULL)
        return;

    if (req->header_htab != NULL) {
        g_hash_table_destroy(req->header_htab);
        req->header_htab = NULL;
    }
    if (req->http_method != NULL) {
        free(req->http_method);
        req->http_method = NULL;
    }
    if (req->url != NULL) {
        free(req->url);
        req->url = NULL;
    }
    if (req->http_ver != NULL) {
        free(req->http_ver);
        req->http_ver = NULL;
    }

    free(req);
    req = NULL;
}

void _req_header_htab_key_destroy(gpointer data) {
    free(data);
    data = NULL;
}

void _req_header_htab_value_destroy(gpointer data) {
    free(data);
    data = NULL;
}

void _print_header(char *key, char *value, gpointer user_data) {
    printf(">> %s = %s\n", key, value);
}

// int main() {
//     char req_buf[REQ_BUF_SIZE] = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection:
//     keep-alive"
//                                 "\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests:
//                                 1\r\n" "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X
//                                 11_2_0) " "AppleWebKit/537.36 (KHTML, like Gecko)
//                                 Chrome/87.0.4280.88 " "Safari/537.36\r\nAccept:
//                                 text/html,application/xhtml+xml,"
//                                 "application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,"
//                                 "application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none"
//                                 "\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r"
//                                 "\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate,
//                                 br"
//                                 "\r\nAccept-Language: en-US,en;q=0.9\r\n\r\n";
//     printf("%s\n", req_buf);

//     request *req = NULL;
//     if((req = parse_request(req_buf, -1)) == NULL) return 1;

//     printf("> %s\t%s\t%s\n", req->http_method, req->url, req->http_ver);
//     g_hash_table_foreach(req->header_htab, (GHFunc) _print_header, NULL);

//     return 0;
// }
