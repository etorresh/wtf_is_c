/*
 * Heap padding experiment
 * Demonstrates how glibc allocates the initial heap. By allocating a chunk of
 * memory and walking past it until segment fault occurs we can find how much
 * memory the OS actually mapped
 */

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

extern char _end;
int main() {
  long page_size = sysconf(_SC_PAGESIZE);
  printf("System page size: %lu\n", page_size);

  /*
   * Request an amount of memory smaller than the 128KB padding
   * So this can be 1 byte, 1 page or 32 pages. As long as it's <= 32 the
   * total pages owned by the process at the end will be the same.
   */
  int pages_requested = 10;
  char *ptr = malloc(pages_requested * page_size);

  printf("Walking the heap until segmentation fault:\n");
  int i = 0;
  while (1) {
    unsigned long j = i * page_size;
    // Reading ptr[j] to avoid the compiler optimizing the call away
    printf("The process owns %i page(s) | ptr[%lu] = %i\n", i + 1, j, ptr[j]);
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
 * which ends un being tcache request < 4KB which rounds up to 4KB, + 128KB = 33
 * pages. While I strongly suspect tcache is the reason behind this fixed heap
 * size behavior a fun future experiment would be to disable tcache before the
 * first malloc, and see if the math reflects the user's actual request size,
 * and also digging deeper into tcache's source code to see what it actually
 * does
 */
