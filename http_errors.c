/// @file http_errors.c
/// @brief Contains functions for handling HTTP errors.
/// @details This file includes functions to generate HTTP error responses for various error codes.

#include <stdio.h>
#include <string.h>
#include "http_errors.h"
#include "constants.h"

/**
 * @brief Generates a 400 Bad Request response.
 * @details This function sets the HTTP response header to indicate a 400 Bad Request error.
 * @param client_info Pointer to the client session information.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
static void bad_request(client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX,
        "HTTP/1.1 400 Bad Request\r\n"
        "\r\n"
    );
    client_info->BSIZE = 0;
}

/**
 * @brief Generates a 413 Request Entity Too Large response.
 * @details This function sets the HTTP response header to indicate a 413 Request Entity Too Large error.
 * @param client_info Pointer to the client session information.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
static void request_entity_too_large(client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX, 
        "HTTP/1.1 413 Request Entity Too Large\r\n"
        "\r\n"
    );
    client_info->BSIZE = 0;
}

/**
 * @brief Generates a 404 Not Found response.
 * @details This function sets the HTTP response header to indicate a 404 Not Found error.
 * @param client_info Pointer to the client session information.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
static void request_not_found(client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX, 
        "HTTP/1.1 404 Not Found\r\n"
        "\r\n"
    );
    client_info->BSIZE = 0;
}

/**
 * @brief Raises an HTTP error response.
 * @details This function generates an HTTP error response based on the provided error code. It sets the appropriate response header and body for the error.
 * @param error_code The HTTP error code.
 * @param client_info Pointer to the client session information.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
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