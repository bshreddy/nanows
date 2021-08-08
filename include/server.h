/**
 * @file include/server.h
 * @brief Function prototypes for the server functions.
 *
 * This file contains function prototypes for loading configuration, starting and stopping the
 * server, memory management and for handling the server's main loop.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#ifndef _SERVER_H
#define _SERVER_H 1

#include "config.h"
#include "mimetypes.h"
#include "request.h"
#include "response.h"

/**
 * @brief Defines the maximum legnth for the queue of pending connnections, passed to `listen()`
 * function.
 *
 * For more Info about `backlog` parameter in `listen()` function, please refer to POSIX Sockets
 * Docs.
 */
#define BACKLOG 16

/**
 * @brief Defines the value for value for `Server` response header.
 *
 * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Server
 */
#define SERVER_NAME "ElServe/2.0"

/**
 * @brief Defines the maximum length of path for website root directory.
 *
 * In config file, the length of value for key defined by `SITE_DIR_CONF_KEY` macro (default:
 * `site_root_dir`) should not exceed this value.
 *
 * @see SITE_DIR_CONF_KEY
 */
#define FILE_PATH_BUF_SIZE 1024

/**
 * @brief Loads the config, sets up the server and starts the main loop.
 *
 * This function is the entry point for the server. It loads the config, sets up the server socket
 * and enters the main loop. This function never returns unless an error occurs or signalled by
 * OS. The server main loop runs on main thread and creates a new thread for each connection.
 * Requests are handled by the `handle_request()` function on the newly created thread.
 *
 * @return Never returns unless an error occurs or signalled by OS.
 * @see handle_request()
 */
void start_server();

/**
 * @brief Stops the server and free all the resources.
 *
 * This function should be called when the server is no longer needed. It stops the server, closes
 * the server socket, unloads config and mime types hash table and frees all the resources.
 *
 * This function must be called before the program exits to ensure all the resources are freed.
 * Recommended to call this function from `atexit()` function and as signal handler using `signal()`
 * function.
 *
 * If `stop_server()` is called before `start_server()`, it does nothing.
 *
 * @return void
 */
void stop_server();

/**
 * @brief Sets up the server's POSIX socket.
 *
 * Creates a TCP socket and binds it to the host and port specified in the config file. On success,
 * it sets `tcp_socket` file descriptor to the socket created and returns. On failure, it exits
 * with exit code -1.
 *
 * @param
 * @return void
 */
void setup_socket();

/**
 * @brief Handles the client requests.
 *
 * This function is called by the main loop on a new thread for each connection. It handles the
 * client requests and sends the response back to the client. The file descriptor for the client
 * is passed as an pointer to the function by `pthread_create()`. The thread is detached after it
 * is created. So, the resources must be freed before the thread exits.
 *
 * If any error occurs, returns a non-zero value. On success, returns 0. This return value is
 * currently unused.
 *
 * @param new_conn_fd Pointer to the file descriptor of the new connection.
 * @return void
 */
void *handle_request(void *);

/**
 * @brief Closes file stream, requests and response objects and free memory allocated for them.
 *
 * If `file` or `req` or `resp` is `NULL`, then no action is taken. This helps in cases where
 * only one or two of the objects are allocated and needed to be freed.
 *
 * @param file File stream to be closed.
 * @param req Request object to be closed.
 * @param res Response object to be closed.
 * @return void
 */
void clean_request(FILE *, request *, response *);
#endif
