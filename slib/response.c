#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#define __USE_GNU 1
#include<search.h>

#include "request.h"
#include "response.h"

response* create_response(const int conn_fd) {
    response *res = _initialize_response();
    if(res == NULL) return NULL;

    res->conn_fd = dup(conn_fd);
    return res;
}

response* create_response_from_request(const request *req) {
    response *res = create_response(req->conn_fd);
    if(res == NULL) return NULL;

    res->http_ver = strdup(req->http_ver);
    return res;
}

ssize_t send_response_head(const response *res) {
    char buf[1024];
    ssize_t buf_size = 0, total_buf_size = 0;

    // Sending first line of response head
    sprintf(res, "%s %s\n", res->http_ver, res->status_code);
    buf_size = strlen(buf);
    if(send(res->conn_fd, buf, buf_size, 0) != buf_size) return total_buf_size;
    total_buf_size += buf_size;

    // TODO: Send response headers
    

    // Sending last line of response head
    strcpy("\r\n", buf);
    buf_size = strlen(buf);
    if(send(res->conn_fd, buf, buf_size, 0) != buf_size) return total_buf_size;
    total_buf_size += buf_size;

    return total_buf_size;
}

int send_response(const response *res, const char *buf) {
    
}

const char* get_response_header(const response *res, const char *header_key, char *header_val) {
    if(res == NULL || header_key == NULL) return NULL;

    ENTRY entry, *ent;
    entry.key = strdup(header_key);
    if(hsearch_r(entry, FIND, &ent, res->header_htab)) {
        if(header_val != NULL) strcpy(header_val, (char *) ent->data);
        return (const char *)(ent->data);
    }
    
    return NULL;
}

const char* set_response_header(const response* res, const char *header_key, char *header_val) {
    if(res == NULL || header_key == NULL || header_val == NULL) return NULL;

    ENTRY entry, *ent;
    entry.key = strdup(header_key);
    entry.data = strdup(header_val);

    if(hsearch_r(entry, ENTER, &ent, res->header_htab)) {
        strcpy(header_val, (char *) ent->data);
        return (const char *)(ent->data);
    }

    return NULL;
}

void close_response(response *res) {
    if(res->conn_fd != -1) close(res->conn_fd);
    _free_response(res);
}

response* _initialize_response() {
    response *res = malloc(sizeof(response));
    if(res == NULL) return NULL;
    if(!hcreate_r(REQ_HEADER_HTABLE_SIZE, res->header_htab)) return NULL;

    res->conn_fd = -1;
    res->http_ver = NULL;
    res->status_code = NULL;
    res->header_htab = malloc(sizeof(struct hsearch_data));
    return res;
}

void _free_response(response *res) {
    if(res->header_htab != NULL) {
        hdestroy_r(res->header_htab);
        res->header_htab = NULL;
    }
    
    free(res->status_code);
    free(res->http_ver);
    free(res);
}
