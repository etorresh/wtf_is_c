/*
 * Integer division by zero.
 *
 * Question: what actually happens when you divide an int by zero?
 *
 * It's undefined behaviour, so the compiler may assume it never happens.
 * `5 / 0` with an unused result is simply removed: gcc warns and emits no
 * division at all. Reading the divisor through a volatile forces a real
 * division at run time, and on x86-64 the CPU traps: the process dies with
 * SIGFPE ("Floating point exception", even though no floating point is
 * involved). On ARM64 the same division returns 0 instead of trapping.
 */
#include <stdio.h>

int main(void) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
#pragma GCC diagnostic ignored "-Wunused-value"
    5 / 0; // removed by the compiler, so nothing happens
#pragma GCC diagnostic pop

    volatile int zero = 0;
    printf("dividing by a zero the compiler can't see...\n");
    fflush(stdout);
    int result = 5 / zero; // SIGFPE on x86-64
    printf("never printed on x86-64: %d\n", result);
    return 0;
}
