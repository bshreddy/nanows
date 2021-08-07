/**
 * @file
 * @brief
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

#define BACKLOG 16
#define SERVER_NAME "ElServe/2.0"
#define FILE_PATH_BUF_SIZE 1024

void start_server();
void stop_server();
void setup_socket();
void *handle_request(void *);
void clean_request(FILE *, request *, response *);

#endif
