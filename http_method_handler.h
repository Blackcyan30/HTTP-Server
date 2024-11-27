#ifndef HTTP_METHOD_HANDLER_H
#define HTTP_METHOD_HANDLER_H
#include <stdbool.h>
#include "client_session.h"

void handle_get(const char* path, client_session_t* client_info);
void handle_post(const char* path, client_session_t* client_info);

#endif