#include <stdio.h>
int main() {
    char *ptr = "Hello";
    // ptr[1] = 'c'; // UB usually segfaults as ptr points to .rodata
    printf("%s\n", ptr);


    char arr[] = "World";
    arr[1] = 'e';
    printf("%s\n", arr);
    return 0;
}
