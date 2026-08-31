// TO DO: rewrite after I implement epoll_1000.c as I'll have a better understanding of epoll
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<sys/epoll.h>
int main() {
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return EXIT_FAILURE;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.fd = pipe_fd[0],
    };

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd[0], &ev) < 0) {
        perror("epoll_ctl");
        return EXIT_FAILURE;
    }

    int nfds = epoll_wait(epoll_fd, &ev, 1, 0);
    printf("nfds = %i\n", nfds);

    char data = '1';
    write(pipe_fd[1], &data, 1);

    nfds = epoll_wait(epoll_fd, &ev, 10, 0);
    printf("nfds = %i\n", nfds);

    return EXIT_SUCCESS;
}
