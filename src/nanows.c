/**
 * @file src/nanows.c
 * @brief Web Server main source file and entry point.
 *
 * Implements the main function of ***Nano Web Server.*** It is a simple, multi-threaded, web server
 * written using POSIX Sockets. Uses a process-driven approach and creates a new thread for each
 * request. This web server can only host simple static webpages with no server-size processing. As
 * of v2, it does not conform to any HTTP protocol. This is a learning project and is not intended
 * to be used in a production environment.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "server.h"

/**
 * @brief The main function of the web server.
 */
int main(int argc, char *argv[]) {
    // Managing Process Lifecycle
    atexit(stop_server);
    signal(SIGINT, exit);
    signal(SIGTSTP, exit);

    // Start Server
    start_server();

    return 0;
}
