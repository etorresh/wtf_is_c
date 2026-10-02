/*
 * Struct padding and reading past a char array.
 *
 * Question: how does the compiler lay out a struct, and what does
 * printf("%s") do with a char array that has no terminating '\0'?
 *
 * - Both arrays in Neighbours hold 17 characters and no '\0'. printf keeps
 *   reading until it finds a zero byte, so printing `first` also prints
 *   `second`, which sits right after it in memory. That's an out-of-bounds
 *   read: undefined behaviour, on purpose.
 * - Padded holds 17 + 4 bytes of data, but sizeof reports 24. The int has to
 *   start on a 4-byte boundary, so the compiler adds 3 bytes after the array.
 */
#include <stdio.h>

struct Neighbours {
    char first[17];
    char second[17];
};

struct Padded {
    char text[17];
    int number;
};

int main(void) {
    struct Neighbours n = {
        .first = {'h', 'e', 'l', 'l', ' ', 'y', 'e', 'a', 'h', ' ', 'b', 'r',
                  'o', 't', 'h', 'e', 'r'},
        .second = {'h', 'e', 'l', 'l', ' ', 'j', 'e', 'a', 'h', ' ', 'b', 'r',
                   'o', 't', 'h', 'e', 'r'},
    };
    printf("first:  %s\n", n.first);
    printf("second: %s\n", n.second);

    struct Padded p = {.text = "hello yeah broth", .number = 5};
    printf("sizeof(int): %zu\n", sizeof(int));
    printf("sizeof(struct Padded): %zu (17 + 4 + 3 bytes of padding)\n",
           sizeof(p));
    return 0;
}
