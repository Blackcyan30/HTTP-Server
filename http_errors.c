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
        "Content-Length: 13\r\n"
        "\r\n"
    );
    *BSIZE = snprintf(body, BMAX, "404 Not Found");
}

void raise_http_error(int error_code, int* HSIZE, int* BSIZE, char* header, char* body) {
    switch(error_code) {
        case 400:
            bad_request(HSIZE, BSIZE, header, body);
            break;
        case 413:
            request_entity_too_large(HSIZE, BSIZE, header, body);
            break;
        case 404:
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