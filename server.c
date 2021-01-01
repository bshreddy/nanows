#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<stdbool.h>
#include<pthread.h>
#include<signal.h>

#include "mimetypes.h"

#define BACKLOG 16
#define BUF_SIZE 2048
#define REQ_BUF_SIZE 8192
#define HTTP_METHOD_BUF_SIZE 10
#define HTTP_VER_BUF_SIZE 10
#define URL_BUF_SIZE 2048

#define SERVER_NAME "ElServe/0.1"
#define SITE_DIR "sites"
#define DEFAULT_PAGE "/index.html"

void* handleRequest(void*);
void closeRequest(void*, int);
void parseRequest(char*, char*, char*, char*);

void stop_server();

int tcp_socket = -1;

int main() {
    atexit(stop_server);
    signal(SIGINT, exit);
    signal(SIGTSTP, exit);

    int conn_fd = -1, on = 1;

    if((tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <  0) {
        printf("Unable to get IPv4 TCP Socket using socket(): %s\n", strerror(errno));
        return -1;
    }

    setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t server_addr_size = sizeof(server_addr);

    if(bind(tcp_socket, (struct sockaddr*) &server_addr, server_addr_size) < 0) {
        printf("Unable to bind socket to server address: %s\n", strerror(errno));
        return -1;
    }

    if(listen(tcp_socket, BACKLOG) < 0) {
        printf("Unable to listen to socket: %s\n", strerror(errno));
        return -1;
    }

    if(!create_mime_table()) {
        printf("Cannot load MIME Types. Check if 'mimetypes.conf' file is present in pwd.\n");
        return -1;
    }
    
    char req[REQ_BUF_SIZE], res[BUF_SIZE];

    printf("Server Started...\nListening on http://0.0.0.0:8080\n"
            "Press Ctrl+C to exit.\n\n");

    while(true) {
        if((conn_fd = accept(tcp_socket, (struct sockaddr*) &server_addr,
                                &server_addr_size)) < 0) {
            printf("Unable to accept new connection: %s\n", strerror(errno));
            break;
        }

        pthread_t tid;
        int *new_conn_fd = (int*)malloc(sizeof(conn_fd));
        *new_conn_fd = conn_fd;
        if(pthread_create(&tid, NULL, handleRequest, (void*) new_conn_fd) < 0) {
            printf("Unable to create new thread: %s\n", strerror(errno));
            close(conn_fd);
            continue;
        }

        if(pthread_detach(tid) != 0) {
            printf("Unable to detach new thread: %s\n", strerror(errno));
            continue;
        }
    }

    return 0;
}

void stop_server() {
    printf("\nShutting down server.....\n");
    if(tcp_socket != -1) close(tcp_socket);
    tcp_socket = -1;
    destroy_mime_table();
}

// TODO: report errors
void* handleRequest(void *new_conn_fd) {
    int conn_fd = *((int*) new_conn_fd);
    int file_fd = -1, read_size, send_size;
    char req[REQ_BUF_SIZE], res[BUF_SIZE], url[URL_BUF_SIZE], file_path[URL_BUF_SIZE+8];
    char http_ver[HTTP_VER_BUF_SIZE], http_method[HTTP_METHOD_BUF_SIZE];

    if(recv(conn_fd, req, REQ_BUF_SIZE, 0) < 0) {
        printf("Unable to read message: %s\n", strerror(errno));
        closeRequest(new_conn_fd, file_fd);
    }

    parseRequest(req, http_method, url, http_ver);
    if(strcmp(url, "/") == 0) strcpy(url, DEFAULT_PAGE);
    printf("> (%s) (%s) (%s)\n", http_method, url, http_ver);

    sprintf(file_path, "%s%s", SITE_DIR, url);
    
    if((file_fd = open(file_path, O_RDONLY)) < 0) {
        printf("Unable to open file (%s): %s\n", file_path, strerror(errno));
        // TODO: Send 404 Not Found
        closeRequest(new_conn_fd, file_fd);
    }

    // TODO: create send header
    sprintf(res, "%s 200 OK\nServer: %s\nContent-Type: %s\r\n\n",
            http_ver, SERVER_NAME, get_mimetype_for_url(url, NULL));

    if(send(conn_fd, res, strlen(res), 0) < 0) {
        printf("Unable to send message: %s\n", strerror(errno));
        closeRequest(new_conn_fd, file_fd);
    }

    // TODO: create send data
    while((read_size = read(file_fd, res, BUF_SIZE)) > 0) {
        send_size = send(conn_fd, res, read_size, 0);
        
        if(send_size != read_size) {
            printf("Unable to send data: %s\n", strerror(errno));
            closeRequest(new_conn_fd, file_fd);
        }
    }

    closeRequest(new_conn_fd, file_fd);
    return NULL;
}

// TODO: Parse Headers as Dictonary/Hash Table
void parseRequest(char *req, char* http_method, char* url, char* http_ver) {
    req = strtok(req, "\r\n");

    strcpy(http_method, strtok(req, " "));
    strcpy(url, strtok(NULL, " "));
    strcpy(http_ver, strtok(NULL, " "));
}

void closeRequest(void *new_conn_fd, int file_fd) {
    close(*((int*) new_conn_fd));
    if(file_fd != -1) close(file_fd);
    free(new_conn_fd);
    pthread_exit(0);
}