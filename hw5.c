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

char request[RMAX+1];

int HSIZE = 0;
char header[HMAX];

int BSIZE = 0;
char body[BMAX];

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

void handle_connection(int clientfd) {
    // Clearing the request buffer to remove data from previous calls.
    memset(request, 0x00, sizeof(request));
    // printf("here in handle_connection");
    // Reading the client's request.
    ssize_t bytes_read = Read(clientfd, request, RMAX);
    printf("Request: %s\n", request);
     if (bytes_read <= 0) {
        printf("No request data received\n"); 
        raise_http_error(400, &HSIZE, &BSIZE, header, body);
        Send(clientfd);
        close(clientfd);
        return;
    }
    request[bytes_read] = '\0';
    
    // printf("Recieved request:\n%s\n", request);

    // Parsing http method and path. 
    char method[1024];
    char path[1024];
    printf("Request: %s\n", request);
    if (parse_request(request, method, 1024, path, 1024) < 0){
        printf("parse_requst failed\n");
        raise_http_error(400, &HSIZE, &BSIZE,  header, body);
        Send(clientfd);
        close(clientfd);
        return;
    }

    // printf("Parsed method: %s, path: %s\n", method, path);
    // Generating response
    printf("Method: %s\n Path: %s", method, path);
    generate_response(method, path);

    // Sending response;
    Send(clientfd);

    close(clientfd);
}

void run_server(int port) {
    int listenfd = initialize_server(port);

    printf("Server if running on port: %d\n", port);

    while (1) {
        int clientfd = Accept(listenfd);
        printf("Accepted Connection\n");
        handle_connection(clientfd);
    }

    close(listenfd);
}



int main(int argc, char * argv[])
{
    assert(argc == 2);
    int port = atoi(argv[1]);
    run_server(port);

    return 0;
}
