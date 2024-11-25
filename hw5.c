/// @file hw5.c


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "network_utils.h"
#include "http_parser.h"
#include "http_response.h"
#include "constants.h"
#include "http_errors.h"
#include "http_debug.h"
#include "client_session.h"

char request[RMAX+1];
ssize_t request_size;
int HSIZE = 0;
char header[HMAX];
int BSIZE = 0;
char body[BMAX];


int initialize_server(int port);
void accept_new_connection(int epfd, int listenfd);
// void handle_client_request(int epfd, client_session_t *client_info);
void run_server(int port);
// void print_request(const char* request, size_t request_size);


int main(int argc, char * argv[])
{
    assert(argc == 2);
    int port = atoi(argv[1]);
    run_server(port);

    return 0;
}


void run_server(int port) {
    int listenfd = initialize_server(port);
    int epfd = epoll_create1(0);
    printf("Server if running on port: %d\n", port);
    
    struct epoll_event event, events[MAX_EVENTS];
    memset(&event, 0x00, sizeof(event));

    event.events = EPOLLIN;
    event.data.fd = listenfd;

    Epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
    

    while (1) {
        int num_events = epoll_wait(epfd, events, MAX_EVENTS, TIME_OUT);

        for (int i = 0; i < num_events; i++) {
            if (events[i].events == EPOLLIN) {
                if (events[i].data.fd == listenfd) {
                    accept_new_connection(epfd, listenfd);
                } else {
                    client_session_t* client_info = (client_session_t*) events[i].data.ptr;
                    handle_client_request(epfd, client_info);
                }
            } else if (events[i].events == EPOLLOUT) {
                client_session_t* client_info = (client_session_t*) events[i].data.ptr;
            }
        }
    }
    close(listenfd);
}

int initialize_server(int port) {
    // Creating the socket and setting socket option.
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    configure_socket(listenfd);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));

    Bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    Listen(listenfd, BACKLOG);

    return listenfd;
}



void accept_new_connection(int epfd, int listenfd) {
    int clientfd = Accept(listenfd);

    client_session_t* client_info = Malloc(sizeof(client_session_t));
    memset(client_info, 0x00, sizeof(client_session_t));

    client_info->fd = clientfd;
    client_info->epfd = epfd;

    struct epoll_event event;
    memset(&event, 0, sizeof(event));

    event.events = EPOLLIN;
    event.data.ptr = client_info;

    Epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &event);
}

void handle_client_request(int epfd, client_session_t* client_info) {
    memset(request, 0, sizeof(request));

    ssize_t bytes_read = Recv(client_info->fd, request, RMAX, 0);

    if (bytes_read <= 0) {
        close(client_info->fd);
        free(client_info);
        return;
    }

    request[bytes_read] = '\0';
    char method[1024], path[1024];
    if (parse_request(request, method, 1024, path, 1024) < 0) {
        raise_http_error(BAD_REQUEST, &HSIZE, &BSIZE, header, body);
        Send(client_info);
        close(client_info->fd);
        free(client_info);
        return;
    }

    generate_response(method, path, client_info);

    if (client_info->body_chunking_enabled) {
        struct epoll_event event;
        memset(&event, 0, sizeof(event));

        event.events = EPOLLOUT;
        event.data.ptr = client_info;
        Epoll_ctl(epfd, EPOLL_CTL_ADD, client_info->fd, &event);
    } else {
        Send(client_info);
        close(client_info->fd);
        free(client_info);
    }
}



// void handle_connection(int clientfd) {
//     // Clearing the request buffer to remove data from previous calls.
//     memset(request, 0x00, sizeof(request));
//     // printf("here in handle_connection");
//     // Reading the client's request.
//     ssize_t bytes_read = Recv(clientfd, request, RMAX, 0);
//     request_size = bytes_read;
//     // printf("Request: %s\n", request);

//     if (bytes_read == RMAX) {
//         printf("Read bytes equal to RMAX\n");
//         char peek[1];
//         ssize_t peek_bytes = Recv(clientfd, peek, 1, MSG_PEEK);

//         if (peek_bytes > 0) {
//             raise_http_error(413,  &HSIZE, &BSIZE, header, body);
//             Send(clientfd);
//             close(clientfd);
//             return;
//         }
//     }

//     if (bytes_read <= 0) {
//         close(clientfd);
//         return;
//     }

    
//     request[bytes_read] = '\0';

//     // print_request(request, bytes_read);
    
//     // printf("\n\nRecieved request:\n%s\n\n", request);

//     // Parsing http method and path. 
//     char method[1024];
//     char path[1024];
//     // printf("Request: %s\n", request);
//     if (parse_request(request, method, 1024, path, 1024) < 0){
//         printf("parse_requst failed\n");
//         raise_http_error(BAD_REQUEST, &HSIZE, &BSIZE,  header, body);
//         Send(clientfd);
//         close(clientfd);
//         return;
//     }

//     // printf("Parsed method: %s, path: %s\n", method, path);
//     // Generating response
//     // printf("Method: %s\n Path: %s", method, path);
//     generate_response(method, path);

//     // Sending response;
//     Send(clientfd);

//     close(clientfd);
// }