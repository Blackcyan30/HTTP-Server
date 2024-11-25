#ifndef HTTP_METHOD_HANDLER_H
#define HTTP_METHOD_HANDLER_H
#include <stdbool.h>
#include "client_session.h"

typedef struct {
    bool body_chunking_enabled;
    int file_fd;
    size_t file_size;
    size_t bytes_sent;

} response_manager_t;

void handle_get(const char* path, client_session_t* client_info);
void handle_post(const char* path, client_session_t* client_info);

#endif