#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "arena.c"

int test_arena(void) {
    printf("Allocating arena of size 1024\n");
    Arena arena = {0};
    if (!Arena_init(&arena, 1024)) {
        assert(false && "Arena init failed");
    };

    assert(Arena_remaining(&arena) == 1024 && "Amount of available space should be 1024");

    printf("Allocating 1023 bytes\n");
    const void* data1 = Arena_alloc(&arena, 1023);
    assert(data1 != NULL && "Arena alloc 1 failed");
    assert(Arena_remaining(&arena) == 1 && "Amount of available space should be 1");

    printf("Allocating 2 bytes, this should fail\n");
    const void* data2 = Arena_alloc(&arena, 2);
    assert(data2 == NULL && "Arena alloc 2 should have failed");
    assert(Arena_remaining(&arena) == 1 && "Amount of available space should be 1");

    printf("Allocating 1 byte, this should not fail\n");
    const void* data3 = Arena_alloc(&arena, 1);
    assert(data3 != NULL && "Arena alloc 3 should have not failed");
    assert(Arena_remaining(&arena) == 0 && "Amount of available space should be 0");

    printf("Resetting arena\n");
    Arena_reset(&arena);
    assert(Arena_remaining(&arena) == 1024 && "Amount of available space should be 1024");

    printf("Allocating 1024 bytes\n");
    const void* data4 = Arena_alloc(&arena, 1024);
    assert(data4 != NULL && "Arena alloc 4 failed");
    assert(Arena_remaining(&arena) == 0 && "Amount of available space should be 0");

    printf("Deallocating arena\n");
    Arena_free(&arena);
    assert(arena.data == NULL && "Arena.data should be NULL after free");

    printf("Deallocating arena again\n");
    Arena_free(&arena);
    assert(arena.data == NULL && "Arena.data should be NULL after free");

    return 0;
}