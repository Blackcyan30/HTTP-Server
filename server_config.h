#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "client_session.h"

/// @file server_config.h
/// @brief Contains function declarations for server configuration and client handling.
/// @details This header file includes the declarations of functions used to create a listening socket, accept client connections, process client requests, and run the server.

/**
 * @brief Runs the server on the specified port.
 * @details This function initializes the server, creates an epoll instance, and enters an event loop to handle incoming connections and client requests.
 * @param port The port number on which the server will listen for incoming connections.
 * @return This function does not return a value.
 * @note Time complexity: O(n) where n is the number of events. Space complexity: O(1).
 */
void run_server(int port);

/**
 * @brief Creates a listening socket on the specified port.
 * @details This function creates a socket, configures it, binds it to the specified port, and starts listening for incoming connections.
 * @param port The port number on which the server will listen for incoming connections.
 * @return Returns the file descriptor of the listening socket.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
int create_listening_socket(int port);

/**
 * @brief Accepts a new client connection.
 * @details This function accepts a new client connection and adds it to the epoll instance for monitoring.
 * @param epfd The epoll file descriptor.
 * @param listenfd The file descriptor of the listening socket.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
void accept_client(int epfd, int listenfd);

/**
 * @brief Processes a client request.
 * @details This function receives data from the client, processes the request, and sends the appropriate response.
 * @param client_info Pointer to the client session information.
 * @return This function does not return a value.
 * @note Time complexity: O(n) where n is the size of the request. Space complexity: O(1).
 */
void process_client_request(client_session_t* client_info);

#endif