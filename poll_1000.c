#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>

// 0.07%
// roughly y where x^1000 = 0.5 and y = 1 - (1 - x)^1000
// (around 50% chance of writing across 1000 attempts)
const int NUMERATOR = 7;
const int DENOMINATOR = 10000;

int main() {
    int pipes_fds[1000][2];
    for (int i = 0; i < 1000; i++) {
        if (pipe(pipes_fds[i]) < 0) {
            printf("Error creating pipe #%i", i);
            return 1;
        }
    }

    int process_count = 0;
    int process_i = -1;
    for (int i = 0; i < 1000; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            printf("Error forking");
            return 1;
        } else if (pid == 0) {
            process_i = i;
            break;
        }
        process_count += 1;
    }

    if (process_i == -1) { // is root
        printf("I am groot and I created #%i processes\n", process_count);
    } else {
        int *my_pipe = pipes_fds[process_i];
        unsigned int seed = process_i + 1;
        while(1) {
            int random_value = rand_r(&seed);
            if (random_value % DENOMINATOR < NUMERATOR) {
                printf("Process #%i is writing\n", process_i);
            }
            sleep(1);
        }
    }

    while(wait(NULL) > 0);  // Block parent to make sure I can see the children's stdout
                            // and that they're killed on interrupt
}
