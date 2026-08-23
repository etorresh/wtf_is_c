#include <stdio.h>
#include <poll.h>
#include <sys/poll.h>
#include <unistd.h>

int main() {
    int fds[2]; // fds[0] = read end, fds[1] = write end
    if (pipe(fds) == -1) {
        printf("oh no\n");
        return 1;
    };
    printf("pipe: %i\n", fds[0]);

    struct pollfd pfd = {
        .fd = fds[0],
        .events = POLLIN
    };

    poll(&pfd, 1, 0);
    printf("Initially: %s\n", (pfd.revents & POLLIN) ? "READY" : "NOT READY");

    write(fds[1], "A", 1);
    poll(&pfd, 1, 0);
    printf("After write: %s\n", (pfd.revents & POLLIN) ? "READY" : "NOT READY");
}
