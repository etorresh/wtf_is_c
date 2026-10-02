/*
 * epoll on a single pipe.
 *
 * Question: what does epoll report before and after a pipe has data?
 *
 * Register the read end, ask epoll_wait for ready descriptors without
 * blocking (timeout 0), write one byte, then ask again. The first call returns
 * 0 ready descriptors and the second returns 1. epoll_1000.c scales this up to
 * 1,000 pipes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(void) {
    int fds[2]; // fds[0] = read end, fds[1] = write end
    if (pipe(fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return EXIT_FAILURE;
    }

    struct epoll_event ev = {.events = EPOLLIN, .data.fd = fds[0]};
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fds[0], &ev) == -1) {
        perror("epoll_ctl");
        return EXIT_FAILURE;
    }

    struct epoll_event ready;
    int count = epoll_wait(epoll_fd, &ready, 1, 0);
    printf("before write: %d ready\n", count);

    char data = '1';
    if (write(fds[1], &data, 1) == -1) {
        perror("write");
        return EXIT_FAILURE;
    }

    count = epoll_wait(epoll_fd, &ready, 1, 0);
    printf("after write:  %d ready (fd %d)\n", count, ready.data.fd);
    return EXIT_SUCCESS;
}
