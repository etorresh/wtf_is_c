/*
 * epoll across 1,000 pipes. Setup and options are in pipes_1000.h.
 *
 * The 1,000 read ends are registered once with epoll_ctl. After that,
 * epoll_wait returns only the descriptors that are ready, so the cost of a
 * wakeup follows the number of ready pipes, not the 1,000 being watched.
 * Compare with poll_1000.c.
 */
#define _GNU_SOURCE // arc4random_uniform and close_range
#include <errno.h>
#include <sys/epoll.h>

#include "pipes_1000.h"

int main(int argc, char **argv) {
    struct config c = parse_args(argc, argv);
    struct children kids;
    spawn_children(&kids, c);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        stop_children(&kids, CHILDREN);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < CHILDREN; i++) {
        struct epoll_event ev = {.events = EPOLLIN, .data.u32 = i};
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, kids.read_fds[i], &ev) == -1) {
            perror("epoll_ctl");
            stop_children(&kids, CHILDREN);
            return EXIT_FAILURE;
        }
    }

    long wakeups = 0, messages = 0;
    static struct epoll_event events[CHILDREN];
    double cpu_start = cpu_ms();
    double deadline = now_ms() + c.seconds * 1000.0;
    for (;;) {
        int timeout = (int)(deadline - now_ms());
        if (timeout <= 0) {
            break;
        }
        int ready = epoll_wait(epoll_fd, events, CHILDREN, timeout);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("epoll_wait");
            stop_children(&kids, CHILDREN);
            return EXIT_FAILURE;
        }
        if (ready == 0) {
            break; // timed out: the run is over
        }
        wakeups++;

        for (int i = 0; i < ready; i++) {
            int child = events[i].data.u32;
            int roll;
            if (read(kids.read_fds[child], &roll, sizeof(roll)) != sizeof(roll)) {
                // Closing the last reference to a descriptor removes it from
                // every epoll instance, so no EPOLL_CTL_DEL is needed.
                close(kids.read_fds[child]);
                continue;
            }
            messages++;
            if (!c.quiet) {
                printf("parent: child %d rolled %d (< %d)\n", child, roll,
                       c.chance);
            }
        }
    }
    double cpu = cpu_ms() - cpu_start;

    stop_children(&kids, CHILDREN);
    report("epoll", wakeups, messages, cpu);
    return EXIT_SUCCESS;
}
