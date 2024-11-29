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

/// @file network_utils.c
/// @brief Contains utility functions for network operations.
/// @details This file includes wrapper functions for common network operations such as creating a socket, binding, and listening on a socket.

/**
 * @brief Wrapper function for socket creation.
 * @details This function creates a socket and handles errors if the socket creation fails.
 * @param namespace The family name of the protocol to follow, e.g., AF_INET.
 * @param style The type of socket to be created, e.g., SOCK_STREAM.
 * @param protocol The protocol to be used with the socket, e.g., IPPROTO_TCP.
 * @return Returns the file descriptor of the created socket.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
int Socket(int namespace, int style, int protocol) {
    int sockfd = socket(namespace, style, protocol);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

/**
 * @brief Wrapper function for binding a socket.
 * @details This function binds a socket to an address and handles errors if the binding fails.
 * @param sockfd The file descriptor of the socket.
 * @param addr A pointer to the address structure to bind the socket to.
 * @param addrlen The length of the address structure.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
void Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Wrapper function for listening on a socket.
 * @details This function marks the socket as a passive socket that will be used to accept incoming connection requests.
 * @param sockfd The file descriptor of the socket.
 * @param backlog The maximum length of the queue of pending connections.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
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

/**
 * @brief Wrapper function for accepting a new client connection.
 * @details This function accepts a new client connection and handles errors if the acceptance fails.
 * @param listenfd The file descriptor of the listening socket.
 * @return Returns the file descriptor of the accepted client connection.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
int Accept(int listenfd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(struct sockaddr_in);
    
    memset(&client_addr, 0x00, sizeof(client_addr));

    int clientfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_len);
    if (clientfd < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    return clientfd;
}

/**
 * @brief Wrapper function for receiving data from a socket.
 * @details This function receives data from a socket and handles errors if the receive operation fails.
 * @param sockfd The file descriptor of the socket.
 * @param buffer A pointer to the buffer where the received data will be stored.
 * @param length The length of the buffer.
 * @param flags Flags to be used with the receive operation.
 * @return Returns the number of bytes received.
 * @note Time complexity: O(n) where n is the number of bytes received. Space complexity: O(1).
 */
ssize_t Recv(int sockfd, void* buffer, size_t length, int flags) {
    ssize_t bytes_recieved = recv(sockfd, buffer, length, flags);

    if (bytes_recieved < 0) {
        printf("Recv Failed.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return bytes_recieved;
}

/**
 * @brief Wrapper function for allocating memory.
 * @details This function allocates memory and handles errors if the allocation fails.
 * @param size The size of the memory to be allocated.
 * @return Returns a pointer to the allocated memory.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
void* Malloc(size_t size) {
    void* ptr = malloc(size);

    if (!ptr) {
        printf("Failed to allocate memory\n");
        free(ptr);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

/**
 * @brief Wrapper function for adding, modifying, or removing file descriptors from an epoll instance.
 * @details This function adds, modifies, or removes file descriptors from an epoll instance and handles errors if the operation fails.
 * @param epfd The file descriptor of the epoll instance.
 * @param op The operation to be performed, e.g., EPOLL_CTL_ADD.
 * @param fd The file descriptor to be added, modified, or removed.
 * @param event A pointer to the epoll event structure.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
void Epoll_ctl(int epfd, int op, int fd, struct epoll_event* event) {
    int status = epoll_ctl(epfd, op, fd, event);

    if (status < 0) {
        printf("epoll_ctl filed\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Wrapper function for reading data from a file descriptor.
 * @details This function reads data from a file descriptor and handles errors if the read operation fails.
 * @param fd The file descriptor to read from.
 * @param buffer A pointer to the buffer where the read data will be stored.
 * @param count The number of bytes to read.
 * @return Returns the number of bytes read.
 * @note Time complexity: O(n) where n is the number of bytes read. Space complexity: O(1).
 */
ssize_t Read(int fd, void* buffer, size_t count) {
    ssize_t bytes_read = read(fd, buffer, count);
    if (bytes_read < 0) {
        perror("Read failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    return bytes_read;
}