# Builds every experiment into build/. Several of them crash or run forever on
# purpose; the README says which.
CC ?= cc
CFLAGS ?= -std=gnu17 -Wall -Wextra -O0 -g

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=build/%)

all: $(BINS)

build/%: %.c $(wildcard *.h) | build
	$(CC) $(CFLAGS) $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean
