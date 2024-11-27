#if !defined(HTTP_RESPONSE_H)
#define HTTP_RESPONSE_H

#include "client_session.h"

void generate_response(const char* method, const char* path, client_session_t* client_info);
void Send(client_session_t* client_info);

#endif