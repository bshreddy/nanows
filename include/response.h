#ifndef _RESPONSE_H
#define _RESPONSE_H 1

#ifndef RES_BUF_SIZE
    #define RES_BUF_SIZE 8192
#endif

#ifndef RES_HEADER_HTABLE_SIZE
    #define RES_HEADER_HTABLE_SIZE 32
#endif

typedef struct response {
    int conn_fd;
    char* http_ver;
    char* status_code;
    struct hsearch_data *header_htab;
} response;

response* create_response(const int);
response* create_response_from_request(const request*);
const char* get_response_header(const response*, const char*, char*);
const char* set_response_header(const response*, const char*, char*);
ssize_t send_response_head(const response*);
ssize_t send_response(const response*, const char*);
void close_response(response*);

response* _initialize_response();
void _free_response(response*);
#endif
