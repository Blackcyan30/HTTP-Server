#ifndef HTTP_DEBUG_H
#define HTTP_DEBUG_H

#include<stdio.h>


void print_request(const char* request, size_t request_size) {
    printf("\n\nDebug Output: Raw Request Content:\n\n");

    for (size_t i = 0; i < request_size; i++) {
        if (request[i] == '\0') {
            printf("\\0");
            break;
        } else if (request[i] == '\r') {
            printf("\\r");
        } else if (request[i] == '\n') {
            printf("\\n\n");
        } else {
            putchar(request[i]);
        }
    }
    printf("\n\nEnd of Debug Output\n\n");
}

#endif