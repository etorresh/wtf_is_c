/*
 * Array decay and pointers to a finished stack frame.
 *
 * Questions: what does sizeof report for an array once it's passed to a
 * function, and what happens if you keep a pointer to a local variable after
 * its function returns?
 *
 * - sizeof(x) in main is 20 (5 ints). Inside scoped_size_of the array has
 *   decayed to a pointer, so sizeof reports 8, the size of an int *.
 * - Returning the address of a local is undefined behaviour. I expected to
 *   read a stale 5 from the old stack frame. Instead gcc (even at -O0)
 *   replaces the returned address with NULL, so dereferencing it segfaults.
 */
#include <stdio.h>

size_t scoped_size_of(int *arr) { return sizeof(arr); }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
int *pass_after_stack_unwind(void) {
    int x = 5;
    return &x;
}
#pragma GCC diagnostic pop

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // print everything before the crash

    int x[5];
    printf("sizeof in main:           %zu\n", sizeof(x));
    printf("sizeof after decay:       %zu\n", scoped_size_of(x));

    int *dangling = pass_after_stack_unwind();
    printf("returned local address:   %p\n", (void *)dangling);
    printf("value: %d\n", *dangling); // segfaults: gcc returned NULL
    return 0;
}
