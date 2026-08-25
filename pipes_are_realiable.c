#include <stdio.h>
#include <unistd.h>

// Fast writer blocks on a full pipe and wakes as the slow reader frees buffer space. At least one page has to be freed for the writer to wake up
int main() {
    int fds[2];
    if (pipe(fds) == -1) {
        return 1;
    }

    int process_id = fork();
    if (process_id == -1) {
        return 1;
    }

    if (process_id == 0) {
        close(fds[0]);

        int iteration = 0;
        while(1) {
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
            iteration ++;
        }
    }

    return 0;
}
