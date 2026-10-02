# wtf_is_c

Small C experiments on what happens underneath: the heap, the stack, file descriptors, pipes, and waiting on many pipes at once. Each file asks one question and answers it by running. I wrote them while learning C on Linux.

## Build and run

```sh
make                 # builds every experiment into build/
./build/poll
```

Needs Linux, gcc or clang, and glibc 2.36 or newer (for `arc4random_uniform`). The 1,000-pipe experiments open about 2,000 file descriptors, so raise the limit first if your shell's is 1,024: `ulimit -n 4096`.

Several experiments invoke undefined behaviour or crash on purpose, since that's what they test. The table says which. Results are from gcc at `-O0`; optimisation can change what undefined behaviour does.

## Experiments

| File | Question | What happens |
|---|---|---|
| `struct_padding.c` | How is a struct laid out, and what does `printf` do with a char array that has no `'\0'`? | 21 bytes of data take 24 (3 bytes of padding before the `int`). `printf` keeps reading into the next array. Out-of-bounds read, on purpose. |
| `string_or_array.c` | Why can a char array be changed but a string literal can't? | The literal lives in read-only `.rodata`; the array is a copy on the stack. |
| `size_of_arr.c` | What's `sizeof` of an array after it's passed to a function, and what's left of a local after its function returns? | 20 becomes 8 once the array decays to a pointer. gcc returns `NULL` instead of the dangling address, so reading it segfaults. **Crashes on purpose.** |
| `divide_by_zero.c` | What does integer division by zero do? | A constant `5 / 0` is compiled away. A zero the compiler can't see raises `SIGFPE` on x86-64. **Crashes on purpose.** |
| `realloc.c` | What's in fresh `malloc` memory, does shrinking with `realloc` move the block, and how far past it can you read? | Zeros (fresh pages from the kernel), the same block, and reads work until the end of the heap that `sbrk(0)` reports. **Crashes on purpose.** |
| `malloc_padding.c` | How much memory does glibc map for the first `malloc`? | The request plus a 128 KB pad. I first blamed tcache for a constant 33-page heap; it was the first `printf` allocating stdout's buffer. Notes at the bottom of the file. **Crashes on purpose.** |
| `wtf_are_pipes.c` | Does `pipe()` overwrite the array you pass it? | Yes: `{67, 69}` becomes `{3, 4}`, and what goes in one end comes out the other. |
| `pipes_are_reliable.c` | What happens when a writer is faster than the reader? | The writer blocks on the full 64 KB pipe and wakes once a 4 KB page is free. Nothing is lost. Runs until Ctrl+C. |
| `poll.c`, `epoll.c` | What do `poll` and `epoll` report before and after a pipe has data? | Not ready, then ready. The first descriptor is 3 because 0 to 2 are stdin, stdout and stderr. |
| `poll_1000.c`, `epoll_1000.c` | How do `poll` and `epoll` compare when waiting on 1,000 pipes? | See below. |

## poll vs. epoll across 1,000 pipes

The parent forks 1,000 children, each with its own pipe. Every tick, each child writes to its pipe with some chance, and the parent waits on all 1,000 read ends. The shared setup is in `pipes_1000.h`, so the two programs differ only in how they wait:

- `poll` gets the whole list of 1,000 on every call. The kernel checks all of them, and then the parent scans all of them again to find the ready ones.
- `epoll` registers the 1,000 once. Each `epoll_wait` returns only the ready ones.

```sh
./build/poll_1000                          # defaults: 10 s, a few messages, printed as they arrive
./build/poll_1000  -s 10 -c 100 -t 10 -q   # ~1,000 messages a second, summary only
./build/epoll_1000 -s 10 -c 100 -t 10 -q
```

`-s` is seconds, `-c` the chance out of 10,000 that a child writes on a tick, `-t` the tick in milliseconds, and `-q` turns off per-message printing, which would skew the timing.

Parent CPU time (user + system, from `getrusage`), median of 3 runs of 10 seconds each:

| Load | `poll` CPU per wakeup | `epoll` CPU per wakeup | `poll` CPU in 10 s | `epoll` CPU in 10 s |
|---|---|---|---|---|
| ~1,000 messages a second | 137 µs | 4.9 µs | 1.27 s | 0.05 s |
| ~10,000 messages a second | 104 µs | 4.2 µs | 6.43 s | 0.32 s |

Each `poll` wakeup costs about 25 to 30 times more, because its cost follows the 1,000 pipes being watched, while `epoll`'s follows the few that are ready. At 10,000 messages a second, `poll` keeps about 64% of a core busy just waiting; `epoll` uses about 3%. `poll`'s cost per wakeup drops at the higher load only because each wakeup finds more pipes ready (1.6 messages per wakeup instead of 1.1), so the scan is shared.

Measured on an AMD Ryzen 5 5600X, Linux 7.2, glibc 2.44, gcc 16, `-O0`.

## Notes

[`notes.md`](notes.md) has smaller things I picked up along the way: the stages of compilation and a few tools.
