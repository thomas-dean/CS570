# A Makefile for Program 2, CS570

PROGRAM = p2
CC = clang
CFLAGS = -march=native -O2 -Os -Wall -Wextra -Weverything -pedantic

${PROGRAM}:	exec.o redir.o parse.o getword.o ${PROGRAM}.o
		${CC} -o ${PROGRAM} exec.o redir.o parse.o getword.o ${PROGRAM}.o

${PROGRAM}.o:	parse.h exec.h

parse.o:	getword.o parse.h

redir.o:	redir.h

exec.o:		parse.h redir.h exec.h

getword.o:	getword.h

clean:
		rm -f *.o *.core ${PROGRAM}

tags:		exec.c redir.c getword.c parse.c ${PROGRAM}.c makefile getword.h parse.h redir.h exec.h
		exctags -R --fields=+S --c-kinds=+defgmstux

lint:
		clang-tidy39 '-header-filter=.*' '-checks=*' *.c --
