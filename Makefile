# Usage:
# make          # same as `make compile`
# make compile  # compile all binaries
# make check    # builds tests and runs them
# make test     # runs all built tests from bin/tests
# make clean    # remove ALL binaries and object files

.PHONY = compile clean

CC = gcc

GLIB_CCFLAGS := $(shell pkg-config --cflags glib-2.0)
GLIB_LLFLAGS := $(shell pkg-config --libs glib-2.0)

CHECK_CCFLAGS := $(shell pkg-config --cflags check)
CHECK_LLFLAGS := $(shell pkg-config --libs check)

CCFLAGS = -I include ${GLIB_CCFLAGS}
SO_CCFLAGS = ${CCFLAGS} -shared -fPIC -c
TESTS_CCFLAGS = ${CCFLAGS} ${CHECK_CCFLAGS}

LLFLAGS = -pthread -lm -L lib $(LIBS:lib/lib%.so=-l%) ${GLIB_LLFLAGS}
TESTS_LLFLAGS = ${LLFLAGS} ${CHECK_LLFLAGS}

SLIBS := $(wildcard slib/*.c)
LIBS := $(SLIBS:slib/%.c=lib/lib%.so)

SRCS := $(wildcard src/*.c)
BINS := $(SRCS:src/%.c=bin/%)

TESTS := $(wildcard tests/*.c)
TESTS_BINS := $(TESTS:tests/%.c=bin/tests/%)

lib/lib%.so: slib/%.c --dir-lib
	${CC} ${SO_CCFLAGS} -o $@ $<

bin/%: src/%.c --dir-bin
	${CC} ${CCFLAGS} ${LLFLAGS} -o $@ $<

bin/tests/%: tests/%.c --dir-bin-tests
	${CC} ${TESTS_CCFLAGS} ${TESTS_LLFLAGS} -o $@ $<

compile: --compile-libs --compile-bins

check: --remove-old-tests --compile-tests --run-tests

test: --run-tests

clean:
	rm -rf lib/*.so
	rm -rf bin/*
	@echo "Cleaned Library Files and Binaries\n"

--compile-libs: ${LIBS}
	@echo "Compiled Library Files\n"

--compile-bins: ${BINS}
	@echo "Compiled Binaries\n"

--remove-old-tests:
	rm -rf bin/tests/*

--compile-tests: ${TESTS_BINS}
	@echo "Compiled Tests\n"

--run-tests: ${TESTS_BINS}
	@echo "Starting Tests\n"
	@for file in $^ ; do $${file} ; done
	@echo "\nTests Done\n"

--dir-%:
	mkdir -p $(subst -,/,$(@:--dir-%=%))
	@echo "Created Dir: $(subst -,/,$(@:--dir-%=%))\n"
