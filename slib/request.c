#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#define __USE_GNU 1
#include<search.h>

#include "request.h"

request* get_request(const int conn_fd) {
    char req_buf[REQ_BUF_SIZE];
    if(recv(conn_fd, req_buf, REQ_BUF_SIZE, 0) < 0) return NULL;

    request *req = _initialize_request();
    if(req == NULL) return NULL;

    req->conn_fd = conn_fd;
    if(!_parse_request(req_buf, req)) return NULL;
    
    return req;
}

request* parse_request(const char *req_buf, const int conn_fd) {
    request *req = _initialize_request();
    if(req == NULL) return NULL;

    if(!_parse_request(req_buf, req)) return NULL;
    return req;
}

void close_request(request *req) {
    if(req->conn_fd != -1) close(req->conn_fd);
    _free_request(req);    
}

const char* get_request_header(const request *req, const char *header_key, char *header_val) {
    if(req == NULL || header_key == NULL) return NULL;

    ENTRY entry, *ent;
    entry.key = strdup(header_key);
    if(hsearch_r(entry, FIND, &ent, req->header_htab)) {
        if(header_val != NULL) strcpy(header_val, (char *)(ent->data));
        return (const char*)(ent->data);
    }

    return NULL;
}

request* _initialize_request() {
    request *req = malloc(sizeof(request));
    if(req == NULL) return NULL;

    req->conn_fd = -1;
    req->http_method = NULL;
    req->url = NULL;
    req->http_ver = NULL;
    req->header_htab = malloc(sizeof(struct hsearch_data));
    return req;
}

int _parse_request(const char *req_buf, request *req) {
    if(req_buf == NULL) return 0;
    if(req == NULL) return 0;
    
    char *req_buff = strdup(req_buf);
    
    req->http_method = strdup(strtok(req_buff, " "));
    req->url = strdup(strtok(NULL, " "));
    req->http_ver = strdup(strtok(NULL, "\r"));

    if(!hcreate_r(REQ_HEADER_HTABLE_SIZE, req->header_htab)) return 0;

    char *header_key, *header_val;
    ENTRY entry, *ent;

    while((header_key = strtok(NULL, ":")) != NULL && (header_val = strtok(NULL, "\r")) != NULL) {
        entry.key = strdup(header_key + 1);
        entry.data = strdup(header_val + 1);

        if(!hsearch_r(entry, ENTER, &ent, req->header_htab)) return 0;
    }

    free(req_buff);
    return 1;
}

void _free_request(request *req) {
    if(req->header_htab != NULL) {
        hdestroy_r(req->header_htab);
        req->header_htab = NULL;
    }

    free(req->http_method);
    free(req->url);
    free(req->http_ver);
    free(req);
}

// int main() {
//     char req_buf[REQ_BUF_SIZE] = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive"
//                                 "\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\n"
//                                 "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_0) "
//                                 "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 "
//                                 "Safari/537.36\r\nAccept: text/html,application/xhtml+xml,"
//                                 "application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,"
//                                 "application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none"
//                                 "\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r"
//                                 "\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br"
//                                 "\r\nAccept-Language: en-US,en;q=0.9\r\n\r\n";

//     request *req = parse_request(req_buf, -1);

//     printf("%s\n", req_buf);
//     printf("> %s\t%s\t%s\n", req->http_method, req->url, req->http_ver);
//     printf(">> %s\n", get_request_header(req, "User-Agent", NULL));

//     return 0;
// }
