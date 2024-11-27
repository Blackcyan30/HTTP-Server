/// @file server_config.c

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
#include "client_session.h"
#include "server_config.h"

int create_listening_socket(int port) {
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

void accept_client(int epfd, int listenfd) {
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

void process_client_request(client_session_t* client_info) {
    memset(client_info->request, 0, sizeof(client_info->request));

    ssize_t bytes_recieved = Recv(client_info->fd, client_info->request, RMAX, 0);

    if (bytes_recieved <= 0) {
        close(client_info->fd);
        free(client_info);
        return;
    }

    // Upadting the request size and request buffer.
    client_info->request_size = bytes_recieved;
    client_info->request[bytes_recieved] = '\0';

    char method[1024], path[1024];
    if (parse_request(client_info->request, method, 1024, path, 1024) < 0) {
        raise_http_error(BAD_REQUEST, client_info);

        Send(client_info);
        close(client_info->fd);
        free(client_info);
        return;
    }

    generate_response(method, path, client_info);

    if (client_info->body_chunking_enabled) {
        Epoll_ctl(client_info->epfd, EPOLL_CTL_DEL, client_info->fd, NULL);

        struct epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = EPOLLOUT;
        event.data.ptr = (void*)client_info;
        Epoll_ctl(client_info->epfd, EPOLL_CTL_ADD, client_info->fd, &event);
    } else {
        Send(client_info);
        close(client_info->fd);
        free(client_info);
    }

}

void run_server(int port) {
    int listenfd = create_listening_socket(port);

    /**
     * epoll_create1() system call creates a new epoll instance and returns a file descriptor referring to that instance.
     * 
     * The epoll instance is used to monitor multiple file descriptors to see if I/O is possible on any of them.
     * 
     * Internally, epoll_create1() allocates an event table in kernel memory to keep track of the file descriptors 
     * and the events of interest for each file descriptor.
     * 
     * The returned file descriptor can be used with epoll_ctl() to add, modify, or remove file descriptors from the epoll instance,
     * and with epoll_wait() to wait for events on the monitored file descriptors.
     * 
     * Purpose:
     * - Efficiently monitor multiple file descriptors to see if I/O is possible on any of them.
     * - Useful in applications that need to handle many simultaneous connections, such as network servers.
     */
    int epfd = epoll_create1(0);
    // printf("Server is running on port: %d\n", port);

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
                    accept_client(epfd, listenfd);
                } else {
                    client_session_t* client_info = (client_session_t*) events[i].data.ptr;
                    process_client_request(client_info);
                }
            } else if (events[i].events == EPOLLOUT) {
                client_session_t* client_info = (client_session_t*) events[i].data.ptr;
                Send(client_info);
            }
        }
    }
    close(listenfd);
}