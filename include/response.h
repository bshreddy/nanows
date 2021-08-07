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
 *
 * @bug Respose headers cannot be modified or removed as of now. This is a known issue and will be
 * fixed in the future.
 * @bug `send_response_head()`: Function doesn't return error code.
 * @bug `send_response_file()`: No easy way to confirm that the file was sent sucessfully. Known
 * issue and will be fixed in the future.
 */

#ifndef _RESPONSE_H
#define _RESPONSE_H 1

/**
 * @brief Defines the max size of a response buffer.
 */
#ifndef RES_BUF_SIZE
#define RES_BUF_SIZE 8192
#endif

/**
 * @brief Defines the max size of buffer for individual response headers.
 */
#ifndef RES_HEADER_BUF_SIZE
#define RES_HEADER_BUF_SIZE 1024
#endif

#include <glib.h>

#include "request.h"

/**
 * @struct response
 * @brief Defines a rresponse structure.
 *
 * This structure defines a response structure. It is used to store the connection file descriptor,
 * HTTP Version, Status Code, and the response headers.
 *
 * @see create_response
 * @see create_response_from_request
 * @see get_response_header
 * @see set_response_header
 * @see send_response_head
 * @see send_response_file
 * @see send_response
 * @see close_response
 *
 * @property int response::conn_fd
 * @brief The file descriptor of the connection that will be used to send the response.
 *
 * @property char* response::http_ver
 * @brief The HTTP version of the response. (e.g. `HTTP/1.1`)
 *
 * @property char* response::status_code
 * @brief The status code of the response. (e.g. `200 OK`)
 *
 * @property GHashTable* response::header_htab
 * @brief The hash table of the response headers.
 */
typedef struct response {
    int conn_fd;
    char *http_ver;
    char *status_code;
    GHashTable *header_htab;
} response;

/**
 * @brief Creates a response struct, duplicates `conn_fd` and returns a pointer to the response
 * struct.
 *
 * This function allocates memory for a response struct by calling `_initialize_response`,
 * duplicates `conn_fd` using `dup()` and returns a pointer to the response struct. `conn_fd` is
 * duplicated to prevent from user closing the original connection file descriptor and leaving the
 * response struct in an invalid state. (e.g. `conn_fd` is closed, but the response struct is still
 * in use by the user, this causes errors when the user tries to send the response back to the
 * client).
 *
 * For, more info about `dup()`, see docs/man pages for `unistd.h`.
 *
 * @param conn_fd The file descriptor of the connection that will be used to send the response.
 * @return On success, a pointer to the response struct is returned. On failure, `NULL` is returned.
 *
 * @see _initialize_response
 */
response *create_response(const int);

/**
 * @brief Similar to `create_response()`, but takes a request struct as an argument and copies
 * `http_ver` from the request struct.
 *
 * This function is similar to `create_response()`, and uses `create_response()` internally, except
 * that it takes a request struct as an argument and also copies `http_ver` from the request struct.
 * `http_ver` is a freshly allocated string and is independent of `request:http_ver`. It is adviced
 * to use `create_response_from_request()` instead of `create_response()` if you are using a request
 * struct.
 *
 * @param req The request struct that will be used to create the response.
 * @return On success, a pointer to the response struct is returned. On failure, `NULL` is returned.
 *
 * @see create_response
 */
response *create_response_from_request(const request *);

/**
 * @brief Gets the value of the response header for the given key.
 *
 * If `header_key` is found in header table, the value is copied into `header_val` and the same
 * is returned. If the key is not found or an error occurs, `NULL` is returned and `header_val` is
 * not modified.
 *
 * `header_val` can be `NULL`, in which case, the function simply returns the value.
 *
 * @param res The response struct.
 * @param header_key The key of the header.
 * @param header_val Pointer to a string to store the value of the header.
 * @return On success, returns a pointer to the header value. On failure, returns `NULL`.
 */
const char *get_response_header(const response *, const char *, char *);

/**
 * @brief Sets the value of the response header for the given key.
 *
 * If `header_key` is not found in header table, `header_key` is added to the header table with
 * value `header_val`. If `header_key` is found in header table, `NULL` is returned and value in
 * header table is not modified. If an error occurs, `NULL` is returned and header table is not
 * modified.
 *
 * `header_key` and `header_val` are duplicated using `strdup()` before storing in header table,
 * this is done to prevent from user freeing the original values and leaving dandling pointers in
 * the header table. The freshly allocated strings are freed interally and the user should not free
 * them.
 *
 * @param res The response struct.
 * @param header_key The key for the header.
 * @param header_val Header value to be set.
 * @return On success, returns `header_val` that was stored. On failure, returns `NULL`.
 */
const char *set_response_header(const response *, const char *, const char *);

/**
 * @brief Sends the response head (Start line and headers) to the client.
 *
 * This function sends the response  start line, followed by the all the response headers set in
 * header table for the response `res` and an  empty line to separate the response head from the
 * response body. This function must be called before sending the response body (i.e.,
 * `send_response_file()` or `send_response()`).)
 *
 * @param res The response struct.
 * @return Returns the number of bytes sent. If an error occurs, returns the number of bytes that
 * were sent before the error occurred.
 */
ssize_t send_response_head(const response *);

/**
 * @brief Sends data in a `FILE` stream to the client as response body.
 *
 * This function sends the response body from the `FILE` stream `file` to the client. The file
 * is read using (`fread()` as chunks of size `RES_BUF_SIZE` and sent to the client. If the number
 * of bytes read from the file doesn't match the number of bytes sent to the client, it is
 * considered an error and the function returns total number of bytes that were sent before the
 * error occurred.
 *
 * The function doesn't send response head (Start line and headers) to the client, it is assumed
 * that the client has already received the response head. The file stream is assumed to be
 * opened in `"rb"` mode and is not closed by this function after sending the response.
 *
 * For now, the only way to make sure the file has been sucessfully sent to the client is to check
 * the returned number of bytes with the actual byte size of the file. This is a known limitation
 * and will be fixed in the future.
 *
 * @param res The response struct.
 * @param file The `FILE` stream to be sent as response body to the client.
 * @return Returns the number of bytes sent. If an error occurs, returns the number of bytes that
 * were sent before the error occurred.
 */
ssize_t send_response_file(const response *, FILE *);

/**
 * @brief Sends first `buf_size` bytes in `buf` to the client as response body.
 *
 * If `buf_size` is given as '-1', the function sends the entire buffer `buf` to the client as
 * response body (i.e. until `\0` is reached). On success, the function returns the number of
 * bytes sent. On failure, returns `-1`.
 *
 * @param res The response struct.
 * @param buf The buffer to be sent as response body to the client.
 * @param buf_size The number of bytes to be sent (-1 if the entire buffer `buf` is to be sent).
 * @return On success, returns the number of bytes sent. On failure, returns `-1`.
 */
ssize_t send_response(const response *, const char *, ssize_t);

/**
 * @brief Closes the response connection and frees the response struct.
 *
 * All response connections needed to be closed after sending the response. Even if the response
 * connection is closed, the browser will still wait for resquest connection to be closed.
 * Therefore, this function, along with `close_request`, must be called to complete the
 * request-response cycle. This function calls `_free_response` to free the request struct.
 *
 * @param req The response to be closed and freed.
 * @return void
 *
 * @see close_request
 * @see _free_response
 */
void close_response(response *);

// ==============================
// Internal Helper Functions
// ==============================

/**
 * @brief Allocates memory for a response struct and initializes it to default values.
 *
 * Default values are:
 *     - conn_fd = -1
 *     - http_ver = `NULL`
 *     - status_code = `NULL`
 *     - header_htab = pointer to a newly allocated `GHashTable`
 *
 * @return On success, pointer to a newly allocated request struct is returned. On failure, `NULL`
 * is returned.
 */
response *_initialize_response();

/**
 * @brief Helper function to free the response struct.
 *
 * This function frees the memory allocated for the response struct. This includes freeing the
 * memory allocated for GHashTable using `g_hash_table_destroy()` and freeing the memory allocated
 * to `http_ver` and `status_code` using `free()`. Once the request struct is freed, `res` parameter
 * is set to `NULL`. If a `NULL` pointer is passed to this function, function does nothing.
 *
 * @param res The response struct to be freed and set to `NULL`.
 * @return void
 */
void _free_response(response *);

/**
 * @brief Releases the memory allocated for the given key, called automatically by
 * `g_hash_table_destroy()`.
 *
 * @param data Pointer to key in the hash table.
 * @return void
 */
void _res_header_htab_key_destroy(gpointer);

/**
 * @brief Releases the memory allocated for the given value, called automatically by
 * `g_hash_table_destroy()`.
 *
 * @param data Pointer to value in the hash table.
 * @return void
 */
void _res_header_htab_value_destroy(gpointer);
#endif
