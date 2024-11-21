#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>

typedef struct {
    char* data;
    size_t length;
} storage_t;

// Initialize per-client storage
storage_t* storage_init();

// Save data to storage
int storage_save(storage_t* storage, const char* data, size_t length);

// Read data from storage
ssize_t storage_read(storage_t* storage, char* buffer, size_t buffer_size);

// Clear storage data
void storage_clear(storage_t* storage);

// Free storage
void storage_free(storage_t* storage);

// Get total memory allocated across all clients
size_t storage_get_memory_usage();

#endif