/*
 * poll() across 1,000 pipes. Setup and options are in pipes_1000.h.
 *
 * poll() takes the whole list on every call: the kernel copies in and checks
 * all 1,000 entries each time, and then the parent scans all 1,000 again to
 * find the few that are ready. The cost of a wakeup grows with the number of
 * descriptors watched, not the number that are ready. Compare with
 * epoll_1000.c.
 */
#define _GNU_SOURCE // arc4random_uniform and close_range
#include <errno.h>
#include <poll.h>

#include "pipes_1000.h"

int main(int argc, char **argv) {
    struct config c = parse_args(argc, argv);
    struct children kids;
    spawn_children(&kids, c);

    struct pollfd fds[CHILDREN];
    for (int i = 0; i < CHILDREN; i++) {
        fds[i] = (struct pollfd){.fd = kids.read_fds[i], .events = POLLIN};
    }

    long wakeups = 0, messages = 0;
    double cpu_start = cpu_ms();
    double deadline = now_ms() + c.seconds * 1000.0;
    for (;;) {
        int timeout = (int)(deadline - now_ms());
        if (timeout <= 0) {
            break;
        }
        int ready = poll(fds, CHILDREN, timeout);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            stop_children(&kids, CHILDREN);
            return EXIT_FAILURE;
        }
        if (ready == 0) {
            break; // timed out: the run is over
        }
        wakeups++;

        for (int i = 0; i < CHILDREN; i++) {
            if (!(fds[i].revents & POLLIN)) {
                continue;
            }
            int roll;
            if (read(fds[i].fd, &roll, sizeof(roll)) != sizeof(roll)) {
                close(fds[i].fd);
                fds[i].fd = -1; // poll skips negative descriptors
                continue;
            }
            messages++;
            if (!c.quiet) {
                printf("parent: child %d rolled %d (< %d)\n", i, roll, c.chance);
            }
        }
    }
    double cpu = cpu_ms() - cpu_start;

    stop_children(&kids, CHILDREN);
    report("poll", wakeups, messages, cpu);
    return EXIT_SUCCESS;
}
