/*
 * First look at pipes.
 *
 * Question: does pipe() overwrite the array you pass it, and what goes in
 * one end and out the other?
 *
 * The array starts as {67, 69}; after pipe() it holds two new file
 * descriptors (3 and 4). Writing "Hello" to the write end and reading the
 * read end gets the same 6 bytes back, including the '\0'.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fds[2] = {67, 69};
    printf("before pipe(): {%d, %d}\n", fds[0], fds[1]);
    if (pipe(fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    printf("after pipe():  {%d, %d}\n", fds[0], fds[1]);

    char message[] = "Hello";
    if (write(fds[1], message, sizeof(message)) != (ssize_t)sizeof(message)) {
        perror("write");
        return EXIT_FAILURE;
    }

    char received[sizeof(message)];
    if (read(fds[0], received, sizeof(received)) != (ssize_t)sizeof(received)) {
        perror("read");
        return EXIT_FAILURE;
    }
    printf("read back: %s\n", received);
    return EXIT_SUCCESS;
}
