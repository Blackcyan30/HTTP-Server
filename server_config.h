#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "client_session.h"

void run_server(int port);
int create_listening_socket(int port);
void accept_client(int epfd, int listenfd);
void process_client_request(client_session_t* client_info);

#endif