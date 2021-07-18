#ifndef _REQUEST_H
#define _REQUEST_H 1

#ifndef REQ_BUF_SIZE
#define REQ_BUF_SIZE 8192
#endif

#ifndef REQ_HEADER_HTABLE_SIZE
#define REQ_HEADER_HTABLE_SIZE 32
#endif

#include <glib.h>

typedef struct request {
    int conn_fd;
    char *http_method;
    char *url;
    char *http_ver;
    GHashTable *header_htab;
} request;

request *get_request(const int);
request *parse_request(const char *, const int);
const char *get_request_header(const request *, const char *, char *);
void close_request(request *);

// Internal Helper Functions
request *_initialize_request();
int _parse_request(const char *, request *);
void _free_request(request *);
void _req_header_htab_key_destroy(gpointer);
void _req_header_htab_value_destroy(gpointer);
void _print_header(char *, char *, gpointer);
#endif
