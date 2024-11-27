/// @file http_method_handler.c

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "constants.h"
#include "http_errors.h"
#include "http_parser.h"
#include "storage.h"
#include "network_utils.h"
#include "http_method_handler.h"

storage_t* server_storage = NULL;

static void handle_ping(client_session_t* client_info);
static void handle_echo(client_session_t* client_info);
static void handle_read(client_session_t* client_info);
static void handle_write(client_session_t* client_info);
static void handle_common_get(const char* path, client_session_t* client_info);
static void set_header(size_t content_length, client_session_t* client_info);

void handle_get(const char* path, client_session_t* client_info) {
    if (strcmp(path, "/ping") == 0) {
        handle_ping(client_info);
    } else if (strcmp(path, "/echo") == 0) {
        handle_echo(client_info);
    } else if (strcmp(path, "/read") == 0) {
        handle_read(client_info);
    } else {
        handle_common_get(path, client_info);
    }
}

void handle_post(const char* path, client_session_t* client_info) {
    if (strcmp(path, "/write") == 0) {
        handle_write(client_info);
    } else {
        raise_http_error(BAD_REQUEST, client_info);
    }
}

static void handle_ping(client_session_t* client_info) {
    // Setting header for /ping.
    client_info->HSIZE = snprintf(client_info->header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 4\r\n"
        "\r\n"
    );

    // Setting body for /ping
    client_info->BSIZE = snprintf(client_info->body, BMAX + 1, "pong");
}

static void handle_echo(client_session_t* client_info) {
    char header_recieved[HMAX];
    int status = parse_headers(client_info->request, header_recieved, HMAX);

    if (status == -1) {
        raise_http_error(BAD_REQUEST, client_info);
        return;
    }

    if (status == -2) {
        raise_http_error(ENTITY_TOO_LARGE, client_info);
        return;
    }

    // Setting header to send
    client_info->HSIZE = snprintf(client_info->header, HMAX, 
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n\r\n",
        (size_t)strlen(header_recieved)
    );

    // Setting body to send
    client_info->BSIZE = snprintf(client_info->body, BMAX + 1, "%s", header_recieved);
}

static void handle_write(client_session_t* client_info) {
    int content_length = extract_content_length(client_info->request);

    size_t to_store_len = content_length;
    if (content_length < 0) {
        raise_http_error(BAD_REQUEST, client_info);
        return;
    }

    if (content_length > BMAX) {
        raise_http_error(ENTITY_TOO_LARGE, client_info);
        return;
    }

    char body_recieved[BMAX + 1];
    int body_length = parse_body(client_info->request, client_info->request_size, body_recieved, BMAX);

    // Means error in parsing body
    if (body_length < content_length) {

        if (body_length == -1) {
            raise_http_error(BAD_REQUEST, client_info);
            return;
        }

        if (body_length == -2) {
            if (!server_storage) {
                server_storage = storage_init();
            }
            parse_body_upto(client_info->request, body_recieved, to_store_len);
            
            if (storage_save(server_storage, body_recieved, content_length) < 0) {
                raise_http_error(ENTITY_TOO_LARGE, client_info);
                return;
            }

            client_info->HSIZE = snprintf(client_info->header, HMAX,
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: %u\r\n"
                "\r\n",
                content_length
            );
   
            client_info->BSIZE = storage_read(server_storage, client_info->body, BMAX);
            return;
        }
    }

    if (!server_storage) {
        server_storage = storage_init();
    }

    if (storage_save(server_storage, body_recieved, to_store_len) < 0) {
        raise_http_error(ENTITY_TOO_LARGE, client_info);
        return;
    }

    client_info->HSIZE = snprintf(client_info->header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %u\r\n"
        "\r\n",
        content_length
    );

    memcpy(client_info->body, body_recieved, content_length);
    client_info->BSIZE = content_length;
}

static void handle_read(client_session_t* client_info) {
    if (!server_storage || server_storage->length == 0) {
        client_info->HSIZE = snprintf(client_info->header, HMAX,
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: %zu\r\n"
            "\r\n",
            strlen("<empty>")
        );

        client_info->BSIZE = snprintf(client_info->body, BMAX + 1, 
            "<empty>"
        );

        return;
    }

    client_info->HSIZE = snprintf(client_info->header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        server_storage->length
    );

    client_info->BSIZE = storage_read(server_storage, client_info->body, BMAX);
}

static void handle_common_get(const char* path, client_session_t* client_info) {
    const char* filepath = path + 1;

    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        raise_http_error(NOT_FOUND, client_info);
        return;
    }


    struct stat file_stat;
    if (fstat(file_fd, &file_stat) < 0 || !S_ISREG(file_stat.st_mode)) {
        close(file_fd);
        raise_http_error(NOT_FOUND, client_info);
        return;
    }

    size_t file_size = file_stat.st_size;
    set_header(file_size, client_info);

    if (file_size > BMAX) {
        client_info->body_chunking_enabled = true;
        client_info->file_fd = file_fd;
        client_info->file_size = file_size;
        client_info->bytes_sent = 0;
        return;
    }
    
    char* read_buff = Malloc(file_size);
    ssize_t bytes_read = Read(file_fd, read_buff, file_size);
    close(file_fd);

    memcpy(client_info->body, read_buff, file_size);
    client_info->BSIZE = file_size;

    free(read_buff);
}

static void set_header(size_t content_length, client_session_t* client_info) {
    client_info->HSIZE = snprintf(client_info->header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        content_length
    );
}
