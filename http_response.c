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

/**
 * @brief Sends an HTTP response.
 * @details This function constructs and sends an HTTP response based on the provided parameters.
 * @param param1 The socket file descriptor to send the response to.
 * @param param2 The HTTP status code to be sent in the response.
 * @return void
 * @note Time complexity: O(1). Space complexity: O(1).
 */
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

/**
 * @file http_response.c
 * @brief Handles HTTP responses.
 * @details file contains functions and definitions for creating and managing
 * HTTP responses.
 * @param param1 The first parameter, typically used for specifying the response code.
 * @param param2 The second parameter, typically used for specifying the response body.
 * @return Returns an integer indicating success (0) or failure (non-zero).
 * @note Time complexity: O(1). Space complexity: O(1).
 */
static void send_data(int clientfd, char buf[], int size) {
    ssize_t amt, total = 0;

    do {
        amt = send(clientfd, buf + total, size - total, 0);
        if (amt < 0) break;
        total += amt;
    } while (total < size);
}

/**
 * @brief Sends the HTTP response to the client.
 * @details This function handles both chunked and non-chunked responses. For chunked responses, it reads data from the file in chunks and sends it to the client. For non-chunked responses, it sends the header and body directly.
 * @param client_info Pointer to the client session information.
 * @return This function does not return a value.
 * @note Time complexity: O(n) where n is the size of the response. Space complexity: O(1).
 */
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
