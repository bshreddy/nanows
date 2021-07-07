#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "mimetypes.h"
#include "request.h"
#include "response.h"

#define BACKLOG 16
#define DEFAULT_HOST "0.0.0.0"
#define DEFAULT_PORT 8080
#define SERVER_NAME "ElServe/2.0"
#define SITE_DIR "site"
#define DEFAULT_PAGE "/index.html"

void stop_server();
void setup_socket();
void* handle_request(void*);

int tcp_socket = -1;

int main(int argc, char *argv[]) {
    int conn_fd = -1;
    pthread_t tid;

    // Managing Process Lifecycle
    atexit(stop_server);
    signal(SIGINT, exit);
    signal(SIGTSTP, exit);

    // Setup
    setup_socket();

    printf("Server Started...\nListening on http://%s:%d\nPress Ctrl+C to exit.\n\n", 
            DEFAULT_HOST, DEFAULT_PORT);
    
    while(true) {
        if((conn_fd = accept(tcp_socket, NULL, NULL)) < 0) {
            perror("Unable to accept new connection");
            continue;
        }

        int *new_conn_fd = (int*) malloc(sizeof(int));
        *new_conn_fd = conn_fd;
        if(pthread_create(&tid, NULL, handle_request, (void *) new_conn_fd) < 0) {
            perror("Unable to create new thread");
            close(conn_fd);
            free(new_conn_fd);
            continue;
        }

        if(pthread_detach(tid) != 0) {
            perror("Unable to detach new thread");
            continue;
        }
    }
    
    return 0;
}

void stop_server() {
    printf("\nShutting down server.....\n");
    if(tcp_socket != -1) close(tcp_socket);
    tcp_socket = -1;
}

void setup_socket() {
    if((tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Unable to get IPv4 TCP Socket using socket()");
        exit(-1);
    }
    
    bool on = true;
    setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, DEFAULT_HOST, &(server_addr.sin_addr));
    socklen_t server_addr_len = sizeof(server_addr);

    if(bind(tcp_socket, (struct sockaddr *) &server_addr, server_addr_len) < 0) {
        perror("Unable to bind socket to server address");
        exit(-1);
    }

    if(listen(tcp_socket, BACKLOG) < 0) {
        perror("Unable to listen to socket");
        exit(-1);
    }
}

void* handle_request(void* new_conn_fd) {
    int conn_fd = *((int *) new_conn_fd);
    free(new_conn_fd);

    request *req = get_request(conn_fd);

    return 0;
}
