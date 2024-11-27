#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "constants.h"

typedef struct {
    int fd;
    int epfd;
    size_t bytes_sent;
    int file_fd;
    size_t file_size;
    bool body_chunking_enabled;
    char request[RMAX];
    ssize_t request_size;
    char header[HMAX];
    int HSIZE;
    char body[BMAX];
    int BSIZE;
} client_session_t;

#endif