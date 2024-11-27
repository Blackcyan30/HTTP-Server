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
#include "client_session.h"
#include "server_config.h"

int main(int argc, char * argv[])
{
    assert(argc == 2);
    int port = atoi(argv[1]);
    run_server(port);

    return 0;
}