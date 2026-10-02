/*
 * poll on a single pipe.
 *
 * Questions: what does poll report before and after a pipe has data, and why
 * is the first file descriptor a program opens always 3?
 *
 * 0, 1 and 2 are already taken by stdin, stdout and stderr, and the kernel
 * hands out the lowest free number, so the pipe's read end is 3 and its write
 * end is 4. poll_1000.c scales this up to 1,000 pipes.
 */
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fds[2]; // fds[0] = read end, fds[1] = write end
    if (pipe(fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    printf("read end: %d, write end: %d\n", fds[0], fds[1]);

    struct pollfd pfd = {.fd = fds[0], .events = POLLIN};

    poll(&pfd, 1, 0);
    printf("before write: %s\n", (pfd.revents & POLLIN) ? "READY" : "NOT READY");

    if (write(fds[1], "A", 1) == -1) {
        perror("write");
        return EXIT_FAILURE;
    }

    poll(&pfd, 1, 0);
    printf("after write:  %s\n", (pfd.revents & POLLIN) ? "READY" : "NOT READY");
    return EXIT_SUCCESS;
}
