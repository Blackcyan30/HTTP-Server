#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "http_response.h"
#include "http_parser.h"
#include "constants.h"
#include "http_errors.h"


extern int HSIZE;
extern int BSIZE;
extern char request[RMAX+1];
extern char header[HMAX];
extern char body[BMAX];


/// @brief 
/// @param method 
/// @param path 
/// @return 
void generate_response(const char* method, const char* path) {
    if (strstr(request, "\r\n\r\n") == NULL) {
        // printf("Here in invalid req \n");
        raise_http_error(400, &HSIZE, &BSIZE, header, body);
        return;
    }

    if (strcmp(method, "GET") == 0 && strcmp(path, "/ping") == 0) {
        // printf("Here in ping\n");
        // Setting header for /ping.
        HSIZE = snprintf(header, HMAX,
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 4\r\n"
            "\r\n"
        );
        
        // Setting body for /ping
        BSIZE = snprintf(body, BMAX, "pong");
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/echo") == 0) {
        char headers[HMAX];
        
        if (parse_headers(request, headers, HMAX) < 0) {
            raise_http_error(400, &HSIZE, &BSIZE, header, body);
            return;
        }
        if (strlen(headers) > HMAX) {
            raise_http_error(413, &HSIZE, &BSIZE, header, body);
            return;
        }

        // Setting header to send
        HSIZE = snprintf(header, HMAX, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: %zu\r\n\r\n",
            strlen(headers)
        );

        // Setting body to send
        BSIZE = snprintf(body, BMAX, "%s", headers);
    } else {
        raise_http_error(404, &HSIZE, &BSIZE, header, body);
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
        // printf("Sent %zd bytes, total sent: %zd out of %d\n", amt, total, size);
    } while (total < size);
}

/// @brief 
/// @param clientfd 
void Send(int clientfd) {
    // printf("Sending header:\n%s", header);
    send_data(clientfd, header, HSIZE);
    // printf("Sending body:\n%s", body);
    send_data(clientfd, body, BSIZE);
    // printf("Body sent\n");
}

