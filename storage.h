#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>
#include <unistd.h>

typedef struct {
    char* data;
    size_t length;
} storage_t;

// Initialize per-client storage
storage_t* storage_init();
int storage_save(storage_t* storage, const char* data, size_t length);
ssize_t storage_read(storage_t* storage, char* buffer, size_t buffer_size);
void storage_clear(storage_t* storage);
void storage_free(storage_t* storage);
size_t storage_get_memory_usage();

#endif