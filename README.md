# c-tools

This repo contains a few essential tools for writing safer C.

## arena.c

A simple arena allocator `Arena`.

```c
bool Arena_init(Arena *arena, const size_t capacity);
void *Arena_alloc(Arena *arena, const size_t size);
void Arena_reset(Arena *arena);
void Arena_free(Arena *arena);
size_t Arena_remaining(const Arena *arena);
```

A growable arena allocator `GrowableArena`. It allocates memory in blocks (page_size) as needed.

```c
bool GrowableArena_init(GrowableArena *arena, const size_t page_size);
void *GrowableArena_alloc(GrowableArena *arena, const size_t size);
void GrowableArena_reset(GrowableArena *arena);
void GrowableArena_free(GrowableArena *arena);
size_t GrowableArena_remaining(const GrowableArena *arena);
```

All allocators have the same API, except for the specific `_init` routine.

Usage pattern:

```c
// Allocate an arena on the stack
GrowableArena arena = {0};

// Initialize the arena
if (!GrowableArena_init(&arena)) return -1;

// Request memory
char *my_string = GrowableArena_alloc(&arena, 500);

// Finally, free it
GrowableArena_free(&arena);
```

## LICENSE

Released under ISC, see [LICENSE](LICENSE).