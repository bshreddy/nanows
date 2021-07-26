/**
 * @file include/response.h
 * @brief Function Prototypes for creating, sending and handling network responses.
 *
 * This file contains the function prototypes to create response for a network request,
 * manage response headers and send response back to the client. It also contains prototypes for
 * internal function to handle response creation and memory management.
 *
 * Implemented in slib/response.c
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#ifndef _RESPONSE_H
#define _RESPONSE_H 1

#ifndef RES_BUF_SIZE
#define RES_BUF_SIZE 8192
#endif

#ifndef RES_HEADER_BUF_SIZE
#define RES_HEADER_BUF_SIZE 1024
#endif

#include <glib.h>
#include <stdio.h>

#include "request.h"

typedef struct response {
    int conn_fd;
    char *http_ver;
    char *status_code;
    GHashTable *header_htab;
} response;

response *create_response(const int);
response *create_response_from_request(const request *);
const char *get_response_header(const response *, const char *, char *);
const char *set_response_header(const response *, const char *, const char *);
ssize_t send_response_header(const response *);
ssize_t send_response_file(const response *, FILE *);
ssize_t send_response(const response *, const char *, ssize_t);
void close_response(response *);

// Internal Helper Functions
response *_initialize_response();
void _free_response(response *);
void _res_header_htab_key_destroy(gpointer);
void _res_header_htab_value_destroy(gpointer);
#endif
