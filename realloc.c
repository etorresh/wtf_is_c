/*
 * What malloc and realloc actually hand you.
 *
 * Questions: what's in fresh malloc memory, does shrinking with realloc move
 * the block, and how far past a 16-byte allocation can you read before it
 * crashes?
 *
 * Findings on glibc (x86-64):
 * - A fresh malloc from a new heap reads as zeros. That's because the pages
 *   come straight from the kernel, which zeroes them; malloc itself promises
 *   nothing, and reused memory can hold old data.
 * - Shrinking 16 bytes to 6 keeps the same block (realloc returns the same
 *   pointer), so the old pointer still "works". Using it is still undefined
 *   behaviour, since realloc may move the block.
 * - Reading far past the 16 bytes doesn't crash: the whole 33-page heap that
 *   glibc mapped (see malloc_padding.c) belongs to the process. The read only
 *   segfaults past the end of the heap, which sbrk(0) reports.
 */
#define _DEFAULT_SOURCE // sbrk
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__GNUC__) && !defined(__clang__) // these warnings are gcc-only
#pragma GCC diagnostic ignored "-Wuse-after-free"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

int main(void) {
    int *arr = malloc(4 * sizeof(int));
    if (arr == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    printf("fresh malloc:   ");
    for (int i = 0; i < 4; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    for (int i = 0; i < 4; i++) {
        arr[i] = 10 + i;
    }

    int *shrunk = realloc(arr, 6);
    if (shrunk == NULL) {
        perror("realloc");
        return EXIT_FAILURE;
    }
    printf("realloc moved the block: %s\n", shrunk == arr ? "no" : "yes");
    printf("*shrunk = %d, read through the old pointer = %d\n", *shrunk, *arr);

    // Past the allocation but inside the heap: garbage or zeros, no crash.
    char *bytes = (char *)shrunk;
    char *heap_end = sbrk(0);
    ptrdiff_t last = heap_end - bytes - 1;
    printf("byte 16:       %d\n", bytes[16]);
    printf("byte 16,380:   %d\n", bytes[16380]);
    printf("byte %td: %d (last byte of the heap)\n", last, bytes[last]);
    printf("byte %td: about to read one past the heap...\n", last + 1);
    fflush(stdout);
    printf("%d\n", bytes[last + 1]); // segfaults
    return 0;
}
