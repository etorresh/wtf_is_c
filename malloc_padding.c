/*
 * Heap padding experiment
 * Demonstrates how glibc allocates the initial heap. By allocating a chunk of
 * memory and walking past it until a segmentation fault (on purpose) we can
 * find how much memory the OS actually mapped.
 *
 * Usage: ./malloc_padding [pages] [-u]
 *   pages  how many pages to malloc (default 10)
 *   -u     make stdout unbuffered first, so printf never calls malloc
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int pages_requested = argc > 1 ? atoi(argv[1]) : 10;
    if (argc > 2 && strcmp(argv[2], "-u") == 0) {
        setvbuf(stdout, NULL, _IONBF, 0);
    }

    long page_size = sysconf(_SC_PAGESIZE);
    printf("System page size: %ld\n", page_size);

    /*
     * Request an amount of memory smaller than the 128KB padding
     * So this can be 1 byte, 1 page or 32 pages. As long as it's <= 32 the
     * total pages owned by the process at the end will be the same.
     */
    char *ptr = malloc(pages_requested * page_size);
    if (ptr == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    printf("Walking the heap until segmentation fault:\n");
    int i = 0;
    while (1) {
        unsigned long j = i * page_size;
        // Reading ptr[j] to avoid the compiler optimizing the call away
        printf("The process owns %i page(s) | ptr[%lu] = %i\n", i + 1, j,
               ptr[j]);
        i++;
    }
    return 0;
}

/*
 * Notes:
 * Whether we request 1 byte, 1 page, or 10 pages, this loop always crashes at
 * exactly page 34. The heap is always locked at 33 pages (135,168 bytes). it
 * looks like glibc defaults to a 128 KB (32-page) cushion called M_TOP_PAD. If
 * the padding is 32 pages, why do we get 33? And why doesn't requesting 10
 * pages result in a 42-page heap? It looks like in modern glibc, your first
 * malloc() is intercepted by tcache, which requires an internal allocation of
 * less than a page. This internal tcache setup triggers the first allocation
 * which ends up being tcache request < 4KB which rounds up to 4KB, + 128KB = 33
 * pages. While I strongly suspect tcache is the reason behind this fixed heap
 * size behavior a fun future experiment would be to disable tcache before the
 * first malloc, and see if the math reflects the user's actual request size,
 * and also digging deeper into tcache's source code to see what it actually
 * does
 *
 * Follow-up: I ran that experiment, and tcache wasn't it. Disabling tcache
 * (GLIBC_TUNABLES=glibc.malloc.tcache_count=0) still gives 33 pages. The
 * first malloc was printf's: the first printf allocates stdout's buffer, so
 * the heap gets set up then (1 page + 32 pages of M_TOP_PAD = 33), and any
 * later request up to 32 pages fits inside the pad. With stdout unbuffered
 * (-u), printf never mallocs, and the heap follows the request:
 *
 *   request    buffered stdout    unbuffered (-u)
 *   1 page     33 pages           34 pages
 *   10 pages   33 pages           43 pages (10 + 1 + 32)
 *
 * The extra page is the 16-byte chunk header: malloc's pointer starts 16
 * bytes into the heap, so 10 pages of data plus the header need 11 pages.
 * Same with tcache disabled, so tcache plays no part here.
 */
