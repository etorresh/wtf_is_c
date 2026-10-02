/*
 * Shared setup for poll_1000.c and epoll_1000.c.
 *
 * The parent forks 1,000 children, each with its own pipe back to the parent.
 * Every tick, each child rolls a number from 0 to 9,999 and writes it to its
 * pipe if it's under the chance. The parent listens on all 1,000 read ends
 * for a fixed time, then stops the children and reports how much CPU time it
 * spent. The two programs differ only in how the parent waits.
 *
 * Usage: ./poll_1000 [-s seconds] [-c chance out of 10,000] [-t tick ms] [-q]
 *
 * Defaults: a tick a second and a 7 in 10,000 chance. (1 - 0.0007)^1000 is
 * about 0.5, so there's roughly a 50% chance that one of the 1,000 children
 * writes in a given second.
 */
#ifndef PIPES_1000_H
#define PIPES_1000_H

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define CHILDREN 1000

struct config {
    int seconds;  // how long the parent listens
    int chance;   // out of 10,000, per child per tick
    int tick_ms;  // how often each child rolls
    bool quiet;   // don't print each message (printing skews the CPU time)
};

struct children {
    pid_t pids[CHILDREN];
    int read_fds[CHILDREN];
};

static struct config parse_args(int argc, char **argv) {
    struct config c = {.seconds = 10, .chance = 7, .tick_ms = 1000};
    int opt;
    while ((opt = getopt(argc, argv, "s:c:t:q")) != -1) {
        switch (opt) {
        case 's': c.seconds = atoi(optarg); break;
        case 'c': c.chance = atoi(optarg); break;
        case 't': c.tick_ms = atoi(optarg); break;
        case 'q': c.quiet = true; break;
        default: c.seconds = 0; // fall through to the usage message
        }
    }
    if (c.seconds <= 0 || c.chance < 0 || c.chance > 10000 || c.tick_ms <= 0) {
        fprintf(stderr, "usage: %s [-s seconds] [-c chance out of 10000] "
                        "[-t tick ms] [-q]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return c;
}

// Never returns. Keeps only this child's write end, moved to fd 3, so each
// child doesn't hold the other 1,999 pipe ends open.
static void child_loop(int write_fd, struct config c) {
    if (dup2(write_fd, 3) == -1) {
        perror("dup2");
        _exit(EXIT_FAILURE);
    }
    close_range(4, ~0U, 0);

    struct timespec tick = {c.tick_ms / 1000, (c.tick_ms % 1000) * 1000000L};
    for (;;) {
        int roll = arc4random_uniform(10000);
        if (roll < c.chance && write(3, &roll, sizeof(roll)) == -1) {
            perror("write");
            _exit(EXIT_FAILURE);
        }
        nanosleep(&tick, NULL);
    }
}

static void stop_children(struct children *kids, int count) {
    for (int i = 0; i < count; i++) {
        kill(kids->pids[i], SIGTERM);
    }
    for (int i = 0; i < count; i++) {
        waitpid(kids->pids[i], NULL, 0);
    }
}

// Leaves the parent with only the 1,000 read ends.
static void spawn_children(struct children *kids, struct config c) {
    int fds[CHILDREN][2];
    for (int i = 0; i < CHILDREN; i++) {
        if (pipe(fds[i]) == -1) {
            perror("pipe (2,000 descriptors needed; try ulimit -n 4096)");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            stop_children(kids, i);
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            child_loop(fds[i][1], c);
        }
        kids->pids[i] = pid;
    }
    for (int i = 0; i < CHILDREN; i++) {
        close(fds[i][1]);
        kids->read_fds[i] = fds[i][0];
    }
    if (!c.quiet) {
        printf("parent: I am groot and I created %d processes\n", CHILDREN);
    }
}

static double now_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

// User + system CPU time used by this process so far.
static double cpu_ms(void) {
    struct rusage u;
    getrusage(RUSAGE_SELF, &u);
    return (u.ru_utime.tv_sec + u.ru_stime.tv_sec) * 1000.0 +
           (u.ru_utime.tv_usec + u.ru_stime.tv_usec) / 1000.0;
}

static void report(const char *name, long wakeups, long messages, double cpu) {
    printf("%s: %ld wakeups, %ld messages, %.1f ms CPU, %.2f us CPU per "
           "wakeup\n", name, wakeups, messages, cpu,
           wakeups ? cpu * 1000.0 / wakeups : 0.0);
}

#endif
