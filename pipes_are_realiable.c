#include <stdio.h>
#include <unistd.h>

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
            // printf("+ STA of WRITER iter: %i\n", iteration);
            char write_buff[1024];
            long bytes_written = write(fds[1], write_buff, sizeof(write_buff));
            printf("+ bytes_written: %ld\n", bytes_written);
            // printf("+ END of WRITER iter: %i\n", iteration);
            iteration++;
        }
    } else {
        close(fds[1]);

        int iteration = 0;
        while (1) {
            sleep(1);
            // printf("- STA of READER iter: %i\n", iteration);
            char read_buff[1024];
            long bytes_read = read(fds[0], read_buff, sizeof(read_buff));
            printf("- bytes_read: %ld\n", bytes_read);
            // printf("- END of READER iter: %i\n", iteration);
            iteration ++;
        }
    }

    return 0;
}
