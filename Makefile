# Usage:
# make 			# same as `make compile`
# make compile	# compile all binaries
# make clean	# remove ALL binaries and object files

.PHONY = compile clean

CC = gcc

GLIB_CCFLAGS := $(shell pkg-config --cflags glib-2.0)
GLIB_LLFLAGS := $(shell pkg-config --libs glib-2.0)

CCFLAGS = -I include ${GLIB_CCFLAGS}
SO_CCFLAGS = ${CCFLAGS} -shared -fPIC -c
LLFLAGS = -pthread -L lib $(LIBS:lib/lib%.so=-l%) ${GLIB_LLFLAGS} -lm

SLIBS := $(wildcard slib/*.c)
LIBS := $(SLIBS:slib/%.c=lib/lib%.so)

SRCS := $(wildcard src/*.c)
BINS := $(SRCS:src/%.c=bin/%)

lib/lib%.so: slib/%.c --dir-lib
	${CC} ${SO_CCFLAGS} -o $@ $<

bin/%: src/%.c --dir-bin
	${CC} ${CCFLAGS} ${LLFLAGS} -o $@ $<

compile: --compile-libs --compile-bins

clean: 
	rm -rf lib/*.so
	rm -rf bin/*
	@echo "Cleaned Library Files and Binaries\n"

--compile-libs: ${LIBS}
	@echo "Compiled Library Files\n"

--compile-bins: ${BINS}
	@echo "Compiled Binaries\n"

--dir-%:
	mkdir -p $(@:--dir-%=%)
	@echo "Created Dir: $(@:--dir-%=%)\n"
