/// @file http_parser.c
/// @brief Contains functions for parsing HTTP requests.
/// @details This file includes functions to parse HTTP request lines and headers.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "http_parser.h"
#include "constants.h"

/**
 * @brief Parses the HTTP request line.
 * @details This function extracts the HTTP method and path from the request line.
 * It uses `strchr` to find the spaces that separate the method, path, and HTTP version.
 * The method and path are then copied into the provided buffers.
 * @param request The HTTP request line.
 * @param method A buffer to store the extracted HTTP method.
 * @param method_size The size of the method buffer.
 * @param path A buffer to store the extracted path.
 * @param path_size The size of the path buffer.
 * @return Returns 0 on success, or -1 on failure.
 * @note Time complexity: O(n) where n is the length of the request line. Space complexity: O(1).
 */
int parse_request(const char* request, char* method, size_t method_size, char* path, size_t path_size) {

    const char* method_end = strchr(request, ' ');

    if (!method_end) return -1;

    size_t method_len = method_end - request;
    // If there is not enough space to store the http method safely.
    if (method_len >= method_size) return -1;

    strncpy(method, request, method_len);
    method[method_len] = '\0';

    const char* path_start = method_end + 1;
    const char* path_end = strchr(path_start, ' ');
    if (!path_end) return -1;

    size_t path_len = path_end - path_start;
    if (path_len >= path_size) return -1;
    strncpy(path, path_start, path_len);
    path[path_len] = '\0';
    return 0;
}

/**
 * @brief Parses the HTTP headers.
 * @details This function extracts the headers from the HTTP request.
 * It uses `strstr` to find the start and end of the headers, which are delimited by `\r\n` and `\r\n\r\n` respectively.
 * The headers are then copied into the provided buffer.
 * @param request The HTTP request containing the headers.
 * @param header_recieved A buffer to store the extracted headers.
 * @param headers_size The size of the headers buffer.
 * @return Returns 0 on success, or -1 on failure.
 * @note Time complexity: O(n) where n is the length of the request. Space complexity: O(1).
 */
int parse_headers(const char* request, char* header_recieved, size_t headers_size) {
    // Extracting a pointer to the start of the header. +2 since \r\n are counted as 2 chars.
    const char* header_start = strstr(request, "\r\n") + 2;
    if (!header_start) return -1;
    
    const char* header_end = strstr(header_start, "\r\n\r\n");
    if (!header_end) return -1;

    size_t header_len = header_end - header_start;
    if (header_len > HMAX) return -2;

    strncpy(header_recieved, header_start, header_len);
    header_recieved[header_len] = '\0';
    
    return 0;
}

/**
 * @brief Parses the HTTP body.
 * @details This function extracts the body from the HTTP request. 
 * It uses `strstr` to find the start of the body, which is delimited by `\r\n\r\n`. 
 * The body is then copied into the provided buffer. 
 * The function also ensures that the body size does not exceed the buffer size.
 * @param request The HTTP request containing the body.
 * @param request_size The size of the HTTP request.
 * @param body_recieved A buffer to store the extracted body.
 * @param body_size The size of the body buffer.
 * @return Returns the length of the extracted body on success, or -1 on failure.
 * @note Time complexity: O(n) where n is the length of the request. Space complexity: O(1).
 */
int parse_body(const char* request, ssize_t request_size, char* body_recieved, size_t body_size) {
    // +4 to skip the carraige return.
    char* body_start = strstr(request, "\r\n\r\n") + 4;
    if (!body_start) return -1;

    size_t parsed_b_length = request_size - (body_start - request);
    if (parsed_b_length >= body_size) return -2;

    memcpy(body_recieved, body_start, parsed_b_length);
    body_recieved[parsed_b_length] = '\0';

    return (int)parsed_b_length;
}

/**
 * @brief Extracts the Content-Length from the HTTP request.
 * @details This function extracts the Content-Length value from the HTTP request headers. 
 * It uses `strstr` to find the "Content-Length:" header and then converts the subsequent characters to an integer.
 * @param request The HTTP request containing the headers.
 * @return Returns the Content-Length value on success, or -1 on failure.
 * @note Time complexity: O(n) where n is the length of the request. Space complexity: O(1).
 */
int extract_content_length(const char* request) {
    const char* content_length = strstr(request, "Content-Length:");
    if (!content_length) return -1;

    content_length += 15;

    return atoi(content_length);
}

/**
 * @brief Parses the HTTP body up to a specified length.
 * @details This function extracts the body from the HTTP request up to the specified length. It uses `strstr` to find the start of the body, which is delimited by `\r\n\r\n`. The body is then copied into the provided buffer up to the specified length.
 * @param request The HTTP request containing the body.
 * @param body_recieved A buffer to store the extracted body.
 * @param length_to_copy The length of the body to copy.
 * @return This function does not return a value.
 * @note Time complexity: O(n) where n is the length to copy. Space complexity: O(1).
 */
void parse_body_upto(const char* request, char* body_recieved, size_t length_to_copy) {
    char* body_start = strstr(request, "\r\n\r\n") + 4;

    strncpy(body_recieved, body_start, length_to_copy);
    body_recieved[length_to_copy] = '\0';
}