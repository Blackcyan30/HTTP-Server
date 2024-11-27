/// @file http_errors.c

#include <stdio.h>
#include <string.h>
#include "http_errors.h"
#include "constants.h"

static void bad_request(client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX,
        "HTTP/1.1 400 Bad Request\r\n"
        "\r\n"
    );
    client_info->BSIZE = 0;
}

static void request_entity_too_large(client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX, 
        "HTTP/1.1 413 Request Entity Too Large\r\n"
        "\r\n"
    );
    client_info->BSIZE = 0;
}

static void request_not_found(client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX, 
        "HTTP/1.1 404 Not Found\r\n"
        "\r\n"
    );
    client_info->BSIZE = 0;
}

void raise_http_error(int error_code, client_session_t* client_info) {
    switch(error_code) {
        case BAD_REQUEST:
            bad_request(client_info);
            break;
        case ENTITY_TOO_LARGE:
            request_entity_too_large(client_info);
            break;
        case NOT_FOUND:
            request_not_found(client_info);
            break;
        default:
            client_info->HSIZE = snprintf(client_info->header, HMAX,
                "HTTP/1.1 500 Internal Server Error \r\n"
                "Content-Length: 0\r\n"
                "\r\n"
            );
            client_info->BSIZE = 0;
            break;
    }
}