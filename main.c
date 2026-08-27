// gcc -Wall -Wextra -Werror main.c -o main && ./main
#include <stdio.h>

struct PleaseOrderMyMemory {
    char c_sized[17];
    char c_arr[17];
};

struct SizeTest {
    char c_arr[17];
    int i;
};
int main() {
    // char my_favourite_char = 'a';
    // printf("%c%c\n", 'a', 'b');
    // printf("Hello World\n");
    // fprintf(stdout, "Hello World\n");
    // char* c_pointer = "hell yeah brother";
    // printf("%s\n", c_pointer);
    // let's fill this with garbage
    // char garbage[1024];
    // for (int i = 0; i < 1024; i++) garbage[i] = 'X';
    // char c_arr[] = {72 + 26 + 6, 'e', 'l', 'l', ' ', 'y', 'e', 'a', 'h', ' ',
    // 'b', 'r', 'o', 't', 'h', 'e', }; printf("%s\n", c_arr); char c_sized[16]
    // = {72 + 26 + 6, 'e', 'l', 'l', ' ', 'j', 'e', 'a', 'h', ' ', 'b', 'r',
    // 'o', 't', 'h', 'e' }; printf("%s\n", c_sized);
    struct PleaseOrderMyMemory ordered_mem = {
        .c_sized = {'h', 'e', 'l', 'l', ' ', 'y', 'e', 'a', 'h', ' ', 'b', 'r',
                    'o', 't', 'h', 'e', 'r'},
        .c_arr = {'h', 'e', 'l', 'l', ' ', 'j', 'e', 'a', 'h', ' ', 'b', 'r',
                  'o', 't', 'h', 'e', 'r'},
    };
    printf("%s\n", ordered_mem.c_sized);
    printf("%s\n", ordered_mem.c_arr);
    printf("size of int: %lu\n", sizeof(int));
    struct SizeTest size_test = {
        .c_arr = "hello yeah broth",
        .i = 5,
    };
    printf("size of size_test: %lu\n", sizeof(size_test));
}
