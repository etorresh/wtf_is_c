/*
 * Pipes don't drop data.
 *
 * Question: what happens when a fast writer fills a pipe faster than a slow
 * reader empties it?
 *
 * The writer blocks on a full pipe (64 KB by default on Linux) and wakes up as
 * the reader frees space; nothing is lost or overwritten. At least one page
 * (4 KB) has to be freed before the writer wakes up. Runs until Ctrl+C.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fds[2];
    if (pipe(fds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t process_id = fork();
    if (process_id == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (process_id == 0) {
        close(fds[0]);

        int iteration = 0;
        while (1) {
            char write_buff[1024];
            long bytes_written = write(fds[1], write_buff, sizeof(write_buff));
            printf("+ bytes_written: %ld iter: %i\n", bytes_written, iteration);
            iteration++;
        }
    } else {
        close(fds[1]);

        int iteration = 0;
        while (1) {
            sleep(1);
            char read_buff[1024];
            long bytes_read = read(fds[0], read_buff, sizeof(read_buff));
            printf("- bytes_read: %ld iter: %i\n", bytes_read, iteration);
            iteration++;
        }
    }

    return 0;
}
