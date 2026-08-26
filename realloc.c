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
        *(arr+i) = i;
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
    // in that line I asked for 4 * 4 = 16 bytes and that got our process a 4KB page assigned
    // so this should crash instead of give me garbage
    printf("%i\n", arr[4095 + 1]);
    // since I have a single malloc here, this means that malloc must ask for more than a single page
    //
    // TO DO: next session I'll use gdb and mallopt M_TOP_PAD to find the padding requested, so I can find the exact value when it segfaults
    // afterwards understand why realloc works, I would think the original pointer is not guranteed to work if there's not enough
    // available contiguous memory, but it seems it is. It might not be the be the case if I call malloc again
}
