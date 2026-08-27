#include <stdio.h>
#include <stdlib.h>
int main() {
    int *arr = malloc(4 * sizeof(int));
    if (arr == NULL) {
        return 1;
    }

    // check what's there before init
    for (int i = 0; i < 4; i++) {
        printf("arr[%i] = %i\n", i, arr[i]);
    }
    printf("\n");

    // set arr = {0, 1, 2, 3}
    for (int i = 0; i < 4; i++) {
        *(arr + i) = i;
    }
    for (int i = 0; i < 4; i++) {
        printf("arr[%i] = %i\n", i, arr[i]);
    }
    printf("\n");

    int *ptr = realloc(arr, 6);
    printf("*arr = %i\n", *arr);
    printf("*ptr = %i\n\n", *ptr);

    *arr = 10;
    printf("*arr = %i\n", *arr);
    printf("*ptr = %i\n\n", *ptr);

    // malloc means that we have a full page assigned to this process
    // so this should give me garbage but not segfault
    printf("%i\n", arr[6]);
    printf("%i\n", arr[1024]);
    printf("%i\n", arr[2048]);
    printf("%i\n", arr[4095]);

    // I predirect this will segfault
    // int *arr = malloc(4 * sizeof(int));
    // in that line I asked for 4 * 4 = 16 bytes and that got our process a 4KB
    // page assigned so this should crash instead of give me garbage
    printf("%i\n\n", arr[4095 + 1]); // my theory was incorrect and it lead to
                                     // exploration at view_reuse_pool.c

    int iteration = 0;
    char *char_ptr = (char *)arr;
    while (1) {
        printf("");
        iteration++;
    }
}
