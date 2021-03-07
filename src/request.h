#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define __USE_GNU 1
#include<search.h>

#define REQ_BUF_SIZE 8192
#define REQ_HEADER_HTABLE_SIZE 32

typedef struct request {
    int conn_fd;
    char* http_method;
    char* url;
    char* http_ver;
    struct hsearch_data *header_htab;
} request;

request* get_request(int);
request* parse_request(const char *);
void close_request(request*);

/* Internal Helper Functions */
request* _initialize_request();
request* _parse_request(const char*, request*);
void _free_request(request*);

/* Useless methods */
void print_request(request*);

request* get_request(int conn_fd) {
    char req_buf[REQ_BUF_SIZE];
    if(recv(conn_fd, req_buf, REQ_BUF_SIZE, 0) < 0) {
        printf("Unable to read message: %s\n", strerror(errno));
        return NULL;
    }

    request *req = _initialize_request();
    req->conn_fd = conn_fd;
    _parse_request(req_buf, req);
}

request* parse_request(const char *req_buf) {
    return _parse_request(req_buf, NULL);
}

void close_request(request *req) {
    if(req->conn_fd != -1) close(req->conn_fd);
    _free_request(req);
}

/* Internal Helper Functions */
request* _initialize_request() {
    request *req = malloc(sizeof(request));
    req->conn_fd = -1;
    req->http_method = NULL;
    req->url = NULL;
    req->http_ver = NULL;
    req->header_htab = malloc(sizeof(struct hsearch_data));
}

request* _parse_request(const char *req_buf, request *req) {
    char buf[REQ_BUF_SIZE] = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9\r\n\r\n";
    char *req_buff;
    ENTRY entry, *ent;

    if(req == NULL)  req = _initialize_request();
    if(req_buf == NULL) req_buff = strdup(buf);
    else req_buff = strdup(req_buf);

    req->http_method = strdup(strtok(req_buff, " "));
    req->url = strdup(strtok(NULL, " "));
    req->http_ver = strdup(strtok(NULL, "\r"));
    
    if(!hcreate_r(REQ_HEADER_HTABLE_SIZE, req->header_htab)) {
        printf("Can't create hash table: %s\n", strerror(errno));
        return NULL;
    }
    
    char *header_key, *header_val;
    while((header_key = strtok(NULL, ":")) != NULL && (header_val = strtok(NULL, "\r")) != NULL) {
        entry.key = strdup(header_key+1);
        entry.data = strdup(header_val+1);

        if(!hsearch_r(entry, ENTER, &ent, req->header_htab)) {
            printf("Can't insert entry into hash table: %s\n", strerror(errno));
            return NULL;
        }
    }

    free(req_buff);
    return req;
}

void _free_request(request *req) {
    if(req == NULL) return;
    
    hdestroy_r(req->header_htab);

    if(req->http_method != NULL) free(req->http_method);
    if(req->url != NULL) free(req->url);
    if(req->http_ver != NULL) free(req->http_ver);
    if(req->header_htab != NULL) free(req->header_htab);
    free(req);
}

/* Useless methods */
void print_request(request *req) {
    ENTRY entry, *ent;
    printf("> (%s) (%s) (%s)\n", req->http_method, req->url, req->http_ver);

    entry.key = "User-Agent";
    if(hsearch_r(entry, FIND, &ent, req->header_htab)) {
        printf(">> %s\n", (char*) ent->data);
    } else {
        printf(">> %s\n", strerror(errno));
    }
}
