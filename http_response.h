#if !defined(HTTP_RESPONSE_H)
#define HTTP_RESPONSE_H

#include "client_session.h"

void generate_response(const char* method, const char* path, client_session_t* client_info);

// make the send have the method and the reponse to send.
void Send(int clientfd);

void send_chunked_response(int epfd, client_session_t* client_info);

#endif