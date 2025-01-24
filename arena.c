#ifndef ARENA_C
#define ARENA_C

#include <stdlib.h>

/**
 * Simple statically-sized arena allocator.
 *
 * Usage:
 *
 * 1. Create an Arena struct
 * 2. Initialize the arena with the desired size: Arena_init
 * 3. Allocate memory from the arena with Arena_alloc
 * 4. If desired, reset the arena for reuse with Arena_reset
 * 5. Finally, free the allocated memory with Arena_free
 */
typedef struct arena_t {
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
    return arena->data + offset;
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

/**
 * Growable arena allocator.
 *
 * This allocator can grow (but not shrink) dynamically by allocating pages of memory as needed.
 * Is uses the simple allocator under the hood and maintains a list of them to find a free memory
 * block and automatically adds new pages as needed. Allocation is therefore slightly more expensive.
 */
typedef struct growable_arena_t {
    size_t page_size;
    size_t pages;
    Arena *base_arena;
} GrowableArena;

/**
 * Initialize a growable arena.
 * @param arena An empty GrowableArena struct
 * @param page_size The size of a single page in bytes
 * @return true for success, false if allocation failed
 */
bool GrowableArena_init(GrowableArena *arena, const size_t page_size) {
    if (arena == NULL) return false;

    Arena* base = malloc(sizeof(Arena));
    if (!base) return false;
    Arena_init(base, page_size);
    arena->page_size = page_size;
    arena->base_arena = base;
    arena->pages = 1;
    return true;
}

/**
 * Allocate memory from the growable arena.
 * @param arena The growable arena
 * @param size No. of bytes to allocate
 * @return Pointer to the memory block or NULL if there was not enough memory left
 */
void *GrowableArena_alloc(GrowableArena *arena, const size_t size) {
    if (arena == NULL) return NULL;
    size_t current_page = 0;

    for (;;) {
        // Find a free memory segment
        for (size_t i = current_page; i < arena->pages; ++i) {
            void* mem = Arena_alloc((arena->base_arena)+i, size);
            if (mem) return mem;
        }

        // Still here, meaning we have not found a free memory block yet. Allocate a new page.
        Arena* base = realloc(arena->base_arena, sizeof(Arena)*(arena->pages+1));
        if (!base) return false;

        if (!Arena_init(base+current_page+1, arena->page_size)) return false;
        arena->base_arena = base;
        arena->pages++;
        current_page++;
    }
}

/**
 * Resets all the arena pages to zero, meaning the memory is fully available.
 * @param arena The growable arena to reset
 */
void GrowableArena_reset(GrowableArena *arena) {
    for (size_t i = 0; i < arena->pages; ++i)
        Arena_reset((arena->base_arena)+i);
}

/**
 * Release the memory allocated by the arena. After this call, the arena cannot be used anymore.
 * @param arena The arena to free.
 */
void GrowableArena_free(GrowableArena *arena) {
    if (arena != NULL) {
        for (size_t i = 0; i < arena->pages; ++i)
            Arena_free(arena->base_arena);

        free(arena->base_arena);
        arena->base_arena = NULL;
    }
}

/**
 * Returns the amount of available bytes left for allocation.
 * @param arena
 * @return Number of bytes available.
 */
size_t GrowableArena_remaining(const GrowableArena *arena) {
    if (arena == NULL) return 0;

    size_t capacity = 0;
    for (size_t i = 0; i < arena->pages; ++i)
        capacity += Arena_remaining((arena->base_arena)+i);

    return capacity;
}

#endif
