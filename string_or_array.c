/*
 * String literal vs. char array.
 *
 * Question: why can `char arr[] = "World"` be changed but `char *ptr =
 * "Hello"` can't?
 *
 * The pointer points at the literal itself, which lives in read-only memory
 * (.rodata), so writing through it is undefined behaviour and usually
 * segfaults. The array is a copy of the literal on the stack, so it's ours to
 * change.
 */
#include <stdio.h>

int main(void) {
    char *ptr = "Hello";
    // ptr[1] = 'a'; // uncomment to segfault: .rodata is read-only
    printf("%s\n", ptr);

    char arr[] = "World";
    arr[1] = 'e';
    printf("%s\n", arr);
    return 0;
}
