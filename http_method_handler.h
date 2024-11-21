#ifndef HTTP_METHOD_HANDLER_H
#define HTTP_METHOD_HANDLER_H
#include <stdbool.h>

typedef struct {
    bool body_chunking_enabled;
    int file_fd;
    size_t file_size;
    size_t bytes_sent;

} response_manager_t;

void handle_get(const char* path);
void handle_post(const char* path);

#endif