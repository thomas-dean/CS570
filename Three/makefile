# Makefile for Program 3 (Readers and Writers) CS570

CC = gcc
CFLAGS = -g -o -Wall -Wpointer-arith -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -D_REENTRANT

# The first target in this makefile is 'test', so 'make' will run the test
# of your p3 with parameter '66'.  Since p3 is listed as a dependency, the
# test will first make sure p3 is up to date before running the test.
# If you only want to compile rather than run the test, use 'make p3' .
# You can try a test with a different seed (6) with 'make test2', and of
# course, you can [after compiling] just run p3 with whatever seed you wish.

p3:	p3main.o p3helper.o
	${CC} -o p3 p3main.o p3helper.o -pthread

test:	p3
	@echo Now running p3 with a seed of 66:
	./p3 0 66

p3main.o:	p3.h

p3helper.o:	p3.h

clean:
	rm -f p3 p3main.o p3helper.o

test2:	p3
	./p3 1 6

tags:		p3helper.c p3main.c p3.h CHK.h
		exctags -R --fields=+S --c-kinds=+defgmstux

lint:
		clang-tidy50 '-checks=*' p3helper.c --
