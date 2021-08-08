/**
 * @file include/request.h
 * @brief Function Prototypes for accepting, parsing and handling network requests.
 *
 * This file contains the function prototypes to accept and receive requests, parse the request,
 * and handle network requests. It also contains internal functions to handle request parsing and
 * memory management.
 *
 * Implemented in slib/request.c
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#ifndef _REQUEST_H
#define _REQUEST_H 1

/**
 * @brief Defines the max size of a request buffer.
 */
#ifndef REQ_BUF_SIZE
#define REQ_BUF_SIZE 8192
#endif

#include <glib.h>

/**
 * @struct request
 * @brief Defines a request structure.
 *
 * This structure defines a request structure. It is used to store the connection file descriptor,
 * HTTP Method, URL, HTTP Version, and the request headers.
 *
 * @see get_request
 * @see parse_request
 * @see get_request_header
 * @see close_request
 *
 * @property int request::conn_fd
 * @brief The file descriptor of the connection used to receive data from the connection.
 *
 * @property char* request::http_method
 * @brief The HTTP method of the request, supports only `GET` for now.
 *
 * @property char* request::url
 * @brief The URL of the request, doesn't include the protocol and domain. (e.g. `/index.html`)
 *
 * @property char* request::http_ver
 * @brief The HTTP version of the request. (e.g. `HTTP/1.1`)
 *
 * @property GHashTable* request::header_htab
 * @brief The hash table of the request headers.
 */
typedef struct request {
    int conn_fd;
    char *http_method;
    char *url;
    char *http_ver;
    GHashTable *header_htab;
} request;

/**
 * @brief Receives an accepted connection, parses the request, and returns the request.
 *
 * This function receives (reads) an accepted connection and calls `parse_request` to parse the
 * request. The request struct returned by `parse_request` is then returned. This function must be
 * called after a connection is accepted, i.e. after `accept` is called. Therefore, the file
 * descriptor returned from `accept` must be passed to this function and not the file descriptor
 * used to accept the connection, i.e. file descriptor passed to `accept`.
 *
 * For, more info about `accept()`, see POSIX socket function docs.
 *
 * @param conn_fd The file descriptor of the accepted connection, i.e the file descriptor returned
 * by `accept`.
 * @return On success, pointer to a request struct is returned. On failure, `NULL` is returned.
 *
 * @see parse_request
 */
request *get_request(const int);

/**
 * @brief Parses the request buffer and returns the request struct.
 *
 * This function accepts request data and file descriptor of the accepted connection, and parses
 * the request. Initially, `_initialize_request` is called to initialize the request struct. Then
 * `_parse_request` is called to parse the request. The request struct is then populated with the
 * data parsed from the request. The same request struct is returned. If an error occurs, `NULL`
 * is returned.
 *
 * @param req_buf The buffer containing the request data (read using `recv()`).
 * @param conn_fd The file descriptor of the accepted connection, i.e the file descriptor returned
 * by `accept`.
 * @return On success, pointer to a request struct is returned. On failure, `NULL` is returned.
 *
 * @see _initialize_request
 * @see _parse_request
 */
request *parse_request(const char *, const int);

/**
 * @brief Gets the value of a request header for a given key.
 *
 * If `header_key` is found in header table, the value is copied into `header_val` and the same is
 * returned. If the key is not found or an error occurs, `NULL` is returned and `header_val` is
 * not modified.
 *
 * `header_val` can be `NULL`, in which case, the function simply returns the value.
 *
 * @param req The request struct.
 * @param header_key The key of the header.
 * @param header_val Pointer to a string to store the value of the header.
 * @return On success, returns a pointer to the header value. On failure, returns `NULL`.
 */
const char *get_request_header(const request *, const char *, char *);

/**
 * @brief Closes the request connection and frees the request struct.
 *
 * All accepted request connections need to be closed to complete the response. Even if the response
 * connection is closed, the browser will still be waiting for a response, if request is not closed.
 * Therefore, this function, along with `close_response`, must be called to complete the
 * request-response cycle. This function calls `_free_request` to free the request struct.
 *
 * @param req The request to be closed and freed.
 * @return void
 *
 * @see close_response
 * @see _free_request
 */
void close_request(request *);

// ==============================
// Internal Helper Functions
// ==============================

/**
 * @private
 * @brief Allocates memory for a request struct and initializes it to default values.
 *
 * Default values are:
 *     - conn_fd = -1
 *     - http_method = `NULL`
 *     - url = `NULL`
 *     - http_ver = `NULL`
 *     - header_htab = pointer to a newly allocated `GHashTable`
 *
 * @return On success, pointer to a newly allocated request struct is returned. On failure, `NULL`
 * is returned.
 */
request *_initialize_request();

/**
 * @private
 * @brief Helper function to parse the request buffer and populate the request struct.
 *
 * Request data is parsed and populated into the request struct. For example, lets take a normal
 * request:
 * ```
 *   GET /index.html HTTP/1.1
 *   Host: localhost
 *   Accept: text/html
 *   Accept-Encoding: gzip
 *   Accept-Language: en-US
 *   User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36
 *       (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36
 *   Connection: keep-alive
 *   Upgrade-Insecure-Requests: 1
 *   Cache-Control: max-age=0
 * ```
 *
 * The request struct will be populated as follows:
 *     - conn_fd = `file descriptor of accepted connection, set in parse_request()`
 *     - http_method = `GET`
 *     - url = `/index.html`
 *     - http_ver = `HTTP/1.1`
 *     - header_htab = `GHashTable` containing the following key-value pairs:
 *         - `Host`: `localhost`
 *         - `Accept`: `text/html`
 *         - `Accept-Encoding`: `gzip`
 *         - `Accept-Language`: `en-US`
 *         - `User-Agent`: `Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36
 * (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36`
 *         - `Connection`: `keep-alive`
 *         - `Upgrade-Insecure-Requests`: `1`
 *         - `Cache-Control`: `max-age=0`
 *
 * @param req_buf The buffer containing the request data (read using `recv()`).
 * @param req The request struct to store the parsed request data.
 * @return On success, `1` is returned. On failure, `0` is returned.
 */
int _parse_request(const char *, request *);

/**
 * @private
 * @brief Helper function to free the request struct.
 *
 * This function frees the memory allocated for the request struct. This includes freeing the
 * memory allocated for GHashTable using `g_hash_table_destroy()` and freeing the memory allocated
 * to `http_method`, `url`, `http_ver` using `free()`. Once the request struct is freed, `req`
 * parameter is set to `NULL`. If a `NULL` pointer is passed to this function, function does
 * nothing.
 *
 * @param req The request struct to be freed and set to `NULL`.
 * @return void
 */
void _free_request(request *);

/**
 * @private
 * @brief Releases the memory allocated for the given key, called automatically by
 * `g_hash_table_destroy()`.
 *
 * @param data Pointer to key in the hash table.
 * @return void
 */
void _req_header_htab_key_destroy(gpointer);

/**
 * @private
 * @brief Releases the memory allocated for the given value, called automatically by
 * `g_hash_table_destroy()`.
 *
 * @param data Pointer to value in the hash table.
 * @return void
 */
void _req_header_htab_value_destroy(gpointer);
#endif
