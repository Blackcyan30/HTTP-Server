/// @file storage.c
/// @brief Contains functions for managing storage.
/// @details This file includes functions to initialize, save, read, clear, and free storage, as well as to get the total memory usage.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "storage.h"
#include "network_utils.h"



#define MAX_CLIENT_STORAGE_SIZE 1024
#define SERVER_MEMORY_LIMIT (MAX_CLIENT_STORAGE_SIZE * 1000)

static size_t total_allocated_memory = 0;

/**
 * @brief Initializes the storage.
 * @details This function allocates memory for the storage and initializes its length to 0. It also updates the total allocated memory.
 * @return Returns a pointer to the initialized storage.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
storage_t* storage_init() {
    storage_t* storage = (storage_t*)Malloc(sizeof(storage_t));

    storage->data = Malloc(MAX_CLIENT_STORAGE_SIZE);

    storage->length = 0;
    total_allocated_memory += MAX_CLIENT_STORAGE_SIZE;
    return storage;
}

/**
 * @brief Saves data to the storage.
 * @details This function copies the provided data to the storage buffer and updates the length of the storage.
 * @param storage Pointer to the storage.
 * @param data The data to be saved.
 * @param length The length of the data.
 * @return Returns 0 on success, or -1 if the data exceeds the storage capacity.
 * @note Time complexity: O(n) where n is the length of the data. Space complexity: O(1).
 */
int storage_save(storage_t* storage, const char* data, size_t length) {
    if (length > MAX_CLIENT_STORAGE_SIZE) {
        printf("Data exceeds storage capacity\n");
        return -1;
    }

    memcpy(storage->data, data, length);
    storage->length = length;
    return 0;
}

/**
 * @brief Reads data from the storage.
 * @details This function copies the data from the storage buffer to the provided buffer and returns the number of bytes copied.
 * @param storage Pointer to the storage.
 * @param buffer A buffer to store the data read from the storage.
 * @param buffer_size The size of the buffer.
 * @return Returns the number of bytes copied.
 * @note Time complexity: O(n) where n is the length of the data. Space complexity: O(1).
 */
ssize_t storage_read(storage_t* storage, char* buffer, size_t buffer_size) {
    if (storage->length == 0) {
        printf("No data in storage\n");
        return -1;
    }

    size_t bytes_to_copy = (storage->length < buffer_size) ? storage->length : buffer_size;
    memcpy(buffer, storage->data, bytes_to_copy);
    return bytes_to_copy;
}

/**
 * @brief Clears the storage.
 * @details This function sets the storage length to 0 and clears the data in the storage buffer.
 * @param storage Pointer to the storage.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
void storage_clear(storage_t* storage) {
    if (storage && storage->data) {
        memset(storage->data, 0, storage->length);
        storage->length = 0;
    }
}

/**
 * @brief Frees the storage.
 * @details This function frees the memory allocated for the storage and updates the total allocated memory.
 * @param storage Pointer to the storage.
 * @return This function does not return a value.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
void storage_free(storage_t* storage) {
    printf("Here in free storage\n");
    if (storage) {
        if (storage->data) {
            free(storage->data);
            total_allocated_memory -= MAX_CLIENT_STORAGE_SIZE;
        }
        free(storage);
    }
}

/**
 * @brief Gets the total memory usage.
 * @details This function returns the total allocated memory.
 * @return Returns the total allocated memory.
 * @note Time complexity: O(1). Space complexity: O(1).
 */
size_t storage_get_memory_usage() {
    return total_allocated_memory;
}