/// @file https_parser.c

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "http_parser.h"
#include "constants.h"

/// @brief 
/// @param request 
/// @param method 
/// @param method_size 
/// @param path 
/// @param path_size 
/// @return 
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

/// @brief 
/// @param request 
/// @param headers 
/// @param headers_size 
/// @return 
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

/// @brief 
/// @param request 
/// @param body_recieved 
/// @param body_size 
/// @return 
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

/// @brief 
/// @param request 
/// @return 
int extract_content_length(const char* request) {
    const char* content_length = strstr(request, "Content-Length:");
    if (!content_length) return -1;

    content_length += 15;

    return atoi(content_length);
}

void parse_body_upto(const char* request, char* body_recieved, size_t length_to_copy) {
    char* body_start = strstr(request, "\r\n\r\n") + 4;

    strncpy(body_recieved, body_start, length_to_copy);
    body_recieved[length_to_copy] = '\0';
}