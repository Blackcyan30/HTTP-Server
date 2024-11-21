/// @file http_errors.c

#include <stdio.h>
#include <string.h>
#include "http_errors.h"
#include "constants.h"

static void bad_request(int* HSIZE, int* BSIZE, char* header, char* body) {
    *HSIZE = snprintf(header, HMAX,
        "HTTP/1.1 400 Bad Request\r\n"
        "\r\n"
    );
    *BSIZE = 0;
}

static void request_entity_too_large(int* HSIZE, int* BSIZE, char* header, char* body) {
    *HSIZE = snprintf(header, HMAX, 
        "HTTP/1.1 413 Request Entity Too Large\r\n"
        "\r\n"
    );
    *BSIZE = 0;
}

static void request_not_found(int* HSIZE, int* BSIZE, char* header, char* body) {
    *HSIZE = snprintf(header, HMAX, 
        "HTTP/1.1 404 Not Found\r\n"
        "\r\n"
    );
    *BSIZE = 0;
}

void raise_http_error(int error_code, int* HSIZE, int* BSIZE, char* header, char* body) {
    switch(error_code) {
        case BAD_REQUEST:
            bad_request(HSIZE, BSIZE, header, body);
            break;
        case ENTITY_TOO_LARGE:
            request_entity_too_large(HSIZE, BSIZE, header, body);
            break;
        case NOT_FOUND:
            request_not_found(HSIZE, BSIZE, header, body);
            break;
        default:
            *HSIZE = snprintf(header, HMAX,
                "HTTP/1.1 500 Internal Server Error \r\n"
                "Content-Length: 0\r\n"
                "\r\n"
            );
            *BSIZE = 0;
            break;
    }
}