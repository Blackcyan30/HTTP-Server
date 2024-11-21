#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <sys/socket.h>

int Socket(int namespace, int style, int protocol);
void Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
void configure_socket(int sockfd);
int Accept(int listenfd);
ssize_t Read(int fd, void* buffer, size_t count);
ssize_t Recv(int sockfd, void* buffer, size_t length, int flags);
void* Malloc(size_t size);

#endif