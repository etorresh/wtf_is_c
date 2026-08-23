// gcc size_of_arr.c -o size_of_arr && ./size_of_arr
#include <stdio.h>

unsigned long scoped_size_of(int *arr) {
    return sizeof(arr);
}

int *pass_after_stack_unwind() {
    int x = 5;
    return &x;
}

int main() {
    int x[5];
    printf("%lu \n", sizeof(x));
    printf("%lu \n", scoped_size_of(x));


    int *pointer_to_freed_stack = pass_after_stack_unwind();
    printf("%i\n", *pointer_to_freed_stack);


    return 0;
}
