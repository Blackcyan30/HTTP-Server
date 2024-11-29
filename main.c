/// @file hw5.c
/// @brief Entry point for the HTTP server application.
/// @details This file contains the main function which initializes and starts the HTTP server. The server listens on the specified port and handles incoming HTTP requests.

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

/**
 * @brief Entry point for the HTTP server application.
 * @details This function initializes the server and starts it on the specified port. It asserts that the correct number of command-line arguments are provided.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments. The first argument is the program name, and the second argument is the port number.
 * @return Returns 0 on successful execution.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
int main(int argc, char * argv[])
{
    assert(argc == 2);
    int port = atoi(argv[1]);
    run_server(port);

    return 0;
}