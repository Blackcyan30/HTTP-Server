#include <string.h>
#include <stdio.h>
#include "http_parser.h"

/// @brief 
/// @param request 
/// @param method 
/// @param method_size 
/// @param path 
/// @param path_size 
/// @return 
int parse_request(const char* request, char* method, size_t method_size, char* path, size_t path_size) {
    printf("Request: %s\n", request);
    printf("Here in parse_request\n");
    const char* method_end = strchr(request, ' ');
    printf("%p\n", method_end);
    if (!method_end) return -1;
    printf("Method valid\n");
    size_t method_len = method_end - request;
    // If there is not enough space to store the http method safely.
    if (method_len >= method_size) return -1;
    printf("Method len is valid\n");
    strncpy(method, request, method_len);
    method[method_len] = '\0';

    const char* path_start = method_end + 1;
    const char* path_end = strchr(path_start, ' ');
    if (!path_end) return -1;
    printf("path_end is valid\n");

    size_t path_len = path_end - path_start;
    if (path_len >= path_size) return -1;
    strncpy(path, path_start, path_len);
    path[path_len] = '\0';
    printf("Here returning from parse_request with 0\n");
    return 0;
}

int parse_headers(const char* request, char* headers, size_t headers_size) {
    // Extracting a pointer to the start of the header. +2 since \r\n are counted as 2 chars.
    const char* header_start = strstr(request, "\r\n") + 2;
    if (!header_start) return -1;

    const char* header_end = strstr(header_start, "\r\n\r\n");
    if (!header_end) return -1;

    size_t header_len = header_end - header_start;
    strncpy(headers, header_start, header_len);
    headers[header_len] = '\0';
    
    return 0;
}