#if !defined(HTTP_RESPONSE_H)
#define HTTP_RESPONSE_H

void generate_response(const char* method, const char* path);

// make the send have the method and the reponse to send.
void Send(int clientfd);

#endif