#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>
int main() {
    // does pipe() overwrite?
    int pipe_1[2] = {67, 69};
    printf("pipe_read: %i pipe_write: %i\n", pipe_1[0], pipe_1[1]);
    pipe(pipe_1);
    printf("pipe_read: %i pipe_write: %i\n", pipe_1[0],
           pipe_1[1]); // yes pipe() does overwrite __pipedes
    char buf[] = "Hello";
    const char *test = "Hello";
    ssize_t bytes_written = write(pipe_1[1], buf, 6);
    if (bytes_written == -1 || bytes_written != 6) {
        perror("write failed");
        return -1;
    };
    struct pollfd x = {.fd = 0};
    // poll(struct pollfd *fds, nfds_t nfds, int timeout);
    struct test {
        int x;
    };
    struct test test_instance = {.x = 5};
    struct test *p = malloc(sizeof(struct test));
    (*p).x = 6; // (*p).x = p->
}
