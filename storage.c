/// @file storage.c

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "storage.h"
#include "network_utils.h"



#define MAX_CLIENT_STORAGE_SIZE 1024
#define SERVER_MEMORY_LIMIT (MAX_CLIENT_STORAGE_SIZE * 1000)

static size_t total_allocated_memory = 0;

storage_t* storage_init() {
    storage_t* storage = (storage_t*)Malloc(sizeof(storage_t));

    storage->data = Malloc(MAX_CLIENT_STORAGE_SIZE);

    storage->length = 0;
    total_allocated_memory += MAX_CLIENT_STORAGE_SIZE;
    return storage;
}

int storage_save(storage_t* storage, const char* data, size_t length) {
    if (length > MAX_CLIENT_STORAGE_SIZE) {
        printf("Data exceeds storage capacity\n");
        return -1;
    }

    memcpy(storage->data, data, length);
    storage->length = length;
    return 0;
}

ssize_t storage_read(storage_t* storage, char* buffer, size_t buffer_size) {
    if (storage->length == 0) {
        printf("No data in storage\n");
        return -1;
    }

    size_t bytes_to_copy = (storage->length < buffer_size) ? storage->length : buffer_size;
    memcpy(buffer, storage->data, bytes_to_copy);
    return bytes_to_copy;
}

void storage_clear(storage_t* storage) {
    if (storage && storage->data) {
        memset(storage->data, 0, storage->length);
        storage->length = 0;
    }
}

void storage_free(storage_t* storage) {
    if (storage) {
        if (storage->data) {
            free(storage->data);
            total_allocated_memory -= MAX_CLIENT_STORAGE_SIZE;
        }
        free(storage);
    }
}

size_t storage_get_memory_usage() {
    return total_allocated_memory;
}