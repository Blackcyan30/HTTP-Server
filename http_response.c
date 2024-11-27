/// @file http_response.c

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "http_response.h"
#include "http_parser.h"
#include "constants.h"
#include "http_errors.h"
#include "http_method_handler.h"
#include <sys/epoll.h>

void generate_response(const char* method, const char* path, client_session_t* client_info) {
    if (strstr(client_info->request, "\r\n\r\n") == NULL) {
        raise_http_error(BAD_REQUEST, client_info);
        return;
    }

    if (strcmp(method, "GET") == 0) {
       handle_get(path, client_info);
    } else if (strcmp(method, "POST") == 0) {
        handle_post(path, client_info);
    } else {
        raise_http_error(BAD_REQUEST, client_info);
    }
}

/// @brief 
/// @param clientfd 
/// @param buf 
/// @param size 
static void send_data(int clientfd, char buf[], int size) {
    ssize_t amt, total = 0;

    do {
        amt = send(clientfd, buf + total, size - total, 0);
        if (amt < 0) break;
        total += amt;
    } while (total < size);
}

void Send(client_session_t* client_info) {
    if (client_info->body_chunking_enabled) {
        char buffer[BMAX];
        size_t remaining_bytes = client_info->file_size - client_info->bytes_sent;
        size_t to_read = (remaining_bytes > BMAX) ? BMAX : remaining_bytes;

        ssize_t bytes_read = read(client_info->file_fd, buffer, to_read);
        
        // Send header only if it is the first chunk.
        if (client_info->bytes_sent == 0) {
            send_data(client_info->fd, client_info->header, client_info->HSIZE);
        }

        if (bytes_read > 0) {
            send_data(client_info->fd, buffer, bytes_read);
            client_info->bytes_sent += bytes_read;

            // Check if this is the last chunk
            if (client_info->bytes_sent >= client_info->file_size) {
                close(client_info->file_fd); // Close the file descriptor
                epoll_ctl(client_info->epfd, EPOLL_CTL_DEL, client_info->fd, NULL); // Remove from epoll interest list
                close(client_info->fd); // Close the socket
                free(client_info); // Free the client session memory
            }
        } else {
            // Handle read error or EOF
            close(client_info->file_fd); // Close the file descriptor
            epoll_ctl(client_info->epfd, EPOLL_CTL_DEL, client_info->fd, NULL); // Remove from epoll interest list
            close(client_info->fd); // Close the socket
            free(client_info); // Free the client session memory
        }
    } else {
        // Normal response (non-chunked)
        send_data(client_info->fd, client_info->header, client_info->HSIZE);
        send_data(client_info->fd, client_info->body, client_info->BSIZE);

        epoll_ctl(client_info->epfd, EPOLL_CTL_DEL, client_info->fd, NULL); // Remove from epoll interest list
    }
}
