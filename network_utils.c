/// @file network_utils.c


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "network_utils.h"

/// @brief Wrapper function for socket
/// @param namespace family name of protocol to follow eg. AF_INET
/// @param style 
/// @param protocol 
/// @return File descriptor to the socket that has been created.
int Socket(int namespace, int style, int protocol) {
    int sockfd = socket(namespace, style, protocol);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

/// @brief Wrapper function for bind
/// @param sockfd 
/// @param addr 
/// @param addrlen 
void Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/// @brief Wrapper for listen
/// @param sockfd 
/// @param backlog 
void Listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/// @brief 
/// @param sockfd 
void configure_socket(int sockfd) {
    int optval = -1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Set socket options failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/// @brief 
/// @param listenfd 
/// @return 
int Accept(int listenfd) {
    struct sockaddr_in client_addr;
    socklen_t client_len;
    
    memset(&client_addr, 0x00, sizeof(client_addr));

    int clientfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_len);

    if (clientfd < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    return clientfd;
}

ssize_t Recv(int sockfd, void* buffer, size_t length, int flags) {
    ssize_t bytes_recieved = recv(sockfd, buffer, length, flags);

    if (bytes_recieved < 0) {
        printf("Recv Failed.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return bytes_recieved;
}

void* Malloc(size_t size) {
    void* ptr = malloc(size);

    if (!ptr) {
        printf("Failed to allocate memory\n");
        free(ptr);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void Epoll_ctl(int epfd, int op, int fd, struct epoll_event* event) {
    int status = epoll_ctl(epfd, op, fd, event);

    if (status < 0) {
        print("epoll_ctl filed\n");
        exit(EXIT_FAILURE);
    }
}

// ssize_t Read(int fd, void* buffer, size_t count) {
//     ssize_t bytes_read = read(fd, buffer, count);
//     if (bytes_read < 0) {
//         perror("Read failed");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }
//     printf("Read %zd bytes\n", bytes_read);

//     return bytes_read;
// }