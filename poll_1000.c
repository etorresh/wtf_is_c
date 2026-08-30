#define _DEFAULT_SOURCE // this allows arc4random_uniform
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

// 0.07%
// roughly y where (1 - y)^1000 = 0.5
// (around 50% chance of writing across 1000 attempts)
const int NUMERATOR = 7;
const int DENOMINATOR = 10000;

int main() {
    int pipes_fds[1000][2];
    for (int i = 0; i < 1000; i++) {
        if (pipe(pipes_fds[i]) < 0) {
            printf("Error creating pipe #%i\n", i);
            return 1;
        }
    }

    int process_count = 0;
    int process_i = -1;
    for (int i = 0; i < 1000; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            printf("Error forking\n");
            return 1;
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
        struct pollfd fds[1000];
        for (int i = 0; i < 1000; i++) {
            fds[i] = (struct pollfd) {
                .fd = pipes_fds[i][0],
                .events = POLLIN,
            };
        }
        while(1) {
            int event_count = poll(fds, sizeof(fds) / sizeof(fds[0]), -1);
            if (event_count == 0) {
                fprintf(stderr, "poll timed out which shouldn't be possible as poll(timeout = -1)\n");
                return EXIT_FAILURE;
            } else if (event_count < 0) {
                printf("error: %d", errno);
                return EXIT_FAILURE;
            }

            for (int i = 0; i < 1000; i++) {
                if (fds[i].revents & POLLIN) {
                    int message;
                    ssize_t bytes_read = read(fds[i].fd, &message, sizeof(message));
                    if (bytes_read <= 0) {
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        continue;
                    }
                    printf("parent: child %i spoke to me thanks to %i because %i < %i\n", i, message, message, NUMERATOR);
                }
            }
        }

    } else {
        for (int i = 0; i < 1000; i++) {
            close(pipes_fds[i][0]);
            if (process_i != i) {
                close(pipes_fds[i][1]);
            }
        }
        int my_pipe_write_fd = pipes_fds[process_i][1];

        while(1) {
            int random_value = arc4random_uniform(DENOMINATOR);
            if (random_value < NUMERATOR) {
                ssize_t bytes_written = write(my_pipe_write_fd, &random_value, sizeof(int));
                if (bytes_written < 0) {
                    fprintf(stderr, "Error writing bytes\n");
                }
                // printf("child #%i: I'm writing\n", process_i);
            }
            sleep(1);
        }
    }
}
