#define _GNU_SOURCE // allows arc4random_uniform, and close_range
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include<sys/epoll.h>

// 0.07%
// roughly y where (1 - y)^1000 = 0.5
// (around 50% chance of writing across 1000 attempts)
const int NUMERATOR = 7;
const int DENOMINATOR = 10000;

int main() {
    int pipes_fds[1000][2];
    for (int i = 0; i < 1000; i++) {
        if (pipe(pipes_fds[i]) == -1) {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }

    int process_count = 0;
    int process_i = -1;
    for (int i = 0; i < 1000; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            process_i = i;
            break;
        }
        process_count += 1;
    }

    if (process_i == -1) { // is root
        for (int i = 0; i < 1000; i++) {
            close(pipes_fds[i][1]);
        }

        printf("parent: I am groot and I created #%i processes\n", process_count);
        int epollfd = epoll_create1(0);
        for (int i = 0; i< 1000; i++) {
            struct epoll_event ev = {
                .data.u32= i,
                .events = EPOLLIN,
            };
            epoll_ctl(epollfd, EPOLL_CTL_ADD, pipes_fds[i][0], &ev);
        }

        while(1) {
            struct epoll_event events[1000];
            int event_count = epoll_wait(epollfd, &events[0], 1000, -1);
            if (event_count < 0) {
                perror("epoll_wait");
                return EXIT_FAILURE;
            }

            for (int i = 0; i < event_count; i++) {
                int child_id = events[i].data.u32;
                int fd_to_read = pipes_fds[child_id][0];
                int message;
                ssize_t bytes_read = read(fd_to_read, &message, sizeof(message));
                if (bytes_read <= 0) {
                    // it looks like epoll_ctl is unecessary as this is the only
                    // reference to the fd, and  the kernel will automatically
                    // unregister it from all epoll instances when closing
                    // epoll_ctl(epollfd, EPOLL_CTL_DEL, fd_to_read, NULL);
                    close(fd_to_read);
                    continue;
                }
                printf("parent: child %i spoke to me thanks to %i because %i < %i\n", child_id, message, message, NUMERATOR);
            }
        }

    } else {
        int my_pipe_write_fd = dup2(pipes_fds[process_i][1], 3);
        if (my_pipe_write_fd < 0) {
            perror("dup2");
        }
        close_range(4, ~0U, 0);

        while(1) {
            int random_value = arc4random_uniform(DENOMINATOR);
            if (random_value < NUMERATOR) {
                ssize_t bytes_written = write(my_pipe_write_fd, &random_value, sizeof(int));
                if (bytes_written == -1) {
                    perror("write");
                }
            }
            sleep(1);
        }
    }
}
