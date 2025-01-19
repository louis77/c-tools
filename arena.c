#ifndef ARENA_C
#define ARENA_C

#include <stdlib.h>

/**
 * Simple Arena Allocator
 *
 * Usage:
 *
 * 1. Create an Arena struct
 * 2. Initialize the arena with the desired size: Arena_init
 * 3. Allocate memory from the arena with Arena_alloc
 * 4. If desired, reset the arena for reuse with Arena_reset
 * 5. Finally, free the allocated memory with Arena_free
 */
typedef struct {
    void * data;
    size_t capacity;
    size_t next_offset;
} Arena;

/**
 * Initialize an arena.
 * @param arena An empty arena struct
 * @param capacity No. of bytes to allocate
 * @return true for success, false if allocation failed
 */
bool Arena_init(Arena *arena, const size_t capacity) {
    if (arena == NULL) return false;
    void *data = malloc(capacity);
    if (data == NULL) return false;
    arena->data = data;
    arena->capacity = capacity;
    arena->next_offset = 0;
    return true;
}

/**
 * Allocate memory from the arena.
 * @param arena The arena
 * @param size No. of bytes to allocate in the arena
 * @return Pointer to the memory block or NULL if there was not enough memory left
 */
void *Arena_alloc(Arena *arena, const size_t size) {
    if (arena == NULL) return NULL;
    if (arena->next_offset + size > arena->capacity) return NULL;
    const size_t offset = arena->next_offset;
    arena->next_offset = offset + size;
    return arena->data+offset;
}

/**
 * Resets the arena to zero, meaning the memory block is fully available.
 * @param arena The arena to reset
 */
void Arena_reset(Arena *arena) {
    if (arena == NULL) return;
    arena->next_offset = 0;
}

/**
 * Release the memory allocated by the arena. After this call, the arena cannot be used anymore.
 * @param arena The arena to free.
 */
void Arena_free(Arena *arena) {
    if (arena != NULL && arena->data != NULL) {
        free(arena->data);
        arena->data = NULL;
    }
}

/**
 * Returns the amount of available bytes left for allocation.
 * @param arena
 * @return Number of bytes available.
 */
size_t Arena_remaining(const Arena *arena) {
    if (arena == NULL) return 0;
    return arena->capacity - arena->next_offset;
}

#endif