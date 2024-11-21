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

extern int HSIZE;
extern int BSIZE;
extern char request[RMAX+1];
extern char header[HMAX];
extern char body[BMAX];

response_manager_t response_manager;
storage_t* server_storage = NULL;

static void handle_ping();
static void handle_echo();
static void handle_read();
static void handle_write();
static void handle_common_get(const char* path);
static void set_header(size_t content_length);
// static void set_body(char* content, size_t content_length);

void handle_get(const char* path) {
    if (strcmp(path, "/ping") == 0) {
        handle_ping();
    } else if (strcmp(path, "/echo") == 0) {
        handle_echo();
    } else if (strcmp(path, "/read") == 0) {
        handle_read();
    } else {
        handle_common_get(path);
    }
}

void handle_post(const char* path) {
    if (strcmp(path, "/write") == 0) {
        handle_write();
    } else {
        raise_http_error(BAD_REQUEST, &HSIZE, &BSIZE, header, body);
    }
}


static void handle_ping() {
    // Setting header for /ping.
    HSIZE = snprintf(header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 4\r\n"
        "\r\n"
    );

    // Setting body for /ping
    BSIZE = snprintf(body, BMAX + 1, "pong");
}

static void handle_echo() {
    char header_recieved[HMAX];
    int status = parse_headers(request, header_recieved, HMAX);

    if (status == -1) {
        raise_http_error(400, &HSIZE, &BSIZE, header, body);
        return;
    }

    if (status == -2) {
        raise_http_error(413, &HSIZE, &BSIZE, header, body);
        return;
    }

    // Setting header to send
    HSIZE = snprintf(header, HMAX, 
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n\r\n",
        (size_t)strlen(header_recieved)
    );

    // Setting body to send
    BSIZE = snprintf(body, BMAX + 1, "%s", header_recieved);
}

static void handle_write() {
    int content_length = extract_content_length(request);
    size_t to_store_len = content_length;
    if (content_length < 0) {
        // printf("here\n");
        raise_http_error(400, &HSIZE, &BSIZE, header, body);
        return;
    }

    if (content_length > BMAX) {
        raise_http_error(ENTITY_TOO_LARGE, &HSIZE, &BSIZE, header, body);
        return;
    }

    char body_recieved[BMAX + 1];
    int body_length = parse_body(request, body_recieved, BMAX);

    // Means error in parsing body
    if (body_length < content_length) {
        if (body_length == -1) {
            raise_http_error(BAD_REQUEST, &HSIZE, &BSIZE, header, body);
            return;
        }

        if (body_length == -2) {
            if (!server_storage) {
                server_storage = storage_init();
            }
            parse_body_upto(request, body_recieved, to_store_len);
            
            if (storage_save(server_storage, body_recieved, content_length) < 0) {
                raise_http_error(ENTITY_TOO_LARGE, &HSIZE, &BSIZE, header, body);
                return;
            }

            HSIZE = snprintf(header, HMAX,
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: %u\r\n"
                "\r\n",
                content_length
            );
   
            BSIZE = storage_read(server_storage, body, BMAX);
            return;
        }
    }

    if (!server_storage) {
        server_storage = storage_init();
    }
    if (storage_save(server_storage, body_recieved, to_store_len) < 0) {
        raise_http_error(ENTITY_TOO_LARGE, &HSIZE, &BSIZE, header, body);
        return;
    }

    HSIZE = snprintf(header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %u\r\n"
        "\r\n",
        content_length
    );
    memcpy(body, body_recieved, content_length);
    BSIZE = content_length;
}

static void handle_read() {
    if (!server_storage || server_storage->length == 0) {
        HSIZE = snprintf(header, HMAX,
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: %zu\r\n"
            "\r\n",
            strlen("<empty>")
        );

        BSIZE = snprintf(body, BMAX + 1, 
            "<empty>"
        );

        printf("\nHeader is:\n%s\nBody is:%s\n", header, body);
        return;
    }

    HSIZE = snprintf(header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        server_storage->length
    );

    BSIZE = storage_read(server_storage, body, BMAX);
}

static void handle_common_get(const char* path) {
    printf("here\n");
    const char* filepath = path + 1;

    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        printf("here\n");
        raise_http_error(NOT_FOUND, &HSIZE, &BSIZE, header, body);
        return;
    }


    struct stat file_stat;
    if (fstat(file_fd, &file_stat) < 0 || !S_ISREG(file_stat.st_mode)) {
        close(file_fd);
        printf("\nFile not found\n");
        raise_http_error(NOT_FOUND, &HSIZE, &BSIZE, header, body);
        return;
    }

    size_t file_size = file_stat.st_size;
    // printf("\nFile size:%zu\n", file_size);
    set_header(file_size);

    if (file_size > BMAX) {
        memset(&response_manager, 0x00, sizeof(response_manager));
        response_manager.body_chunking_enabled = true;
        response_manager.file_fd = file_fd;
        response_manager.bytes_sent = 0;
        response_manager.file_size = file_size;
        return;
    }
    // if (file_size > BMAX) {
    //     printf("here\n");
    //     close(file_fd);
    //     raise_http_error(INTERNAL_SERVER_ERROR, &HSIZE, &BSIZE, header, body);
    //     return;
    // }
    
    char* read_buff = Malloc(file_size);

    ssize_t bytes_read = read(file_fd, read_buff, file_size);
    // printf("\nFile contents:\n%s\n", read_buff);
    close(file_fd);
    memcpy(body, read_buff, file_size);
    BSIZE = file_size;

    free(read_buff);
}



static void set_header(size_t content_length) {
    HSIZE = snprintf(header, HMAX,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        content_length
    );
}

// static int set_body(char* content, size_t content_length) {

// }