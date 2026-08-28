#include <poll.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

int main() {
    int fds[2]; // fds[0] = read end, fds[1] = write end
    if (pipe(fds) == -1) {
        return 1;
    };
    printf("pipe: %i\n", fds[0]);   // the first given fd is always 3 as 0-2 are
                                    // are assigned to stdin, stdout, and stderror
                                    // https://en.wikipedia.org/wiki/File_descriptor

    struct pollfd pfd = {.fd = fds[0], .events = POLLIN};

    poll(&pfd, 1, 0);
    printf("Initially: %s\n", (pfd.revents & POLLIN) ? "READY" : "NOT READY");

    ssize_t bytes_writen = write(fds[1], "A", 1);
    if (bytes_writen < 0) {
       return 1;
    }

    poll(&pfd, 1, 0);
    printf("After write: %s\n", (pfd.revents & POLLIN) ? "READY" : "NOT READY");
}
