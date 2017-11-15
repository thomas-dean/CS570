# A Makefile for Program 2, CS570

PROGRAM = p2
CC = clang
CFLAGS = -march=native -O2 -Os -Wall -Wextra -Weverything -pedantic

${PROGRAM}:	builtins.o exec.o redir.o parse.o getword.o ${PROGRAM}.o
		${CC} -o ${PROGRAM} builtins.o exec.o redir.o parse.o getword.o ${PROGRAM}.o

${PROGRAM}.o:	parse.h exec.h

parse.o:	getword.o parse.h

redir.o:	redir.h

exec.o:		parse.h redir.h exec.h builtins.h

builtins.o:	builtins.h parse.h

getword.o:	getword.h

clean:
		rm -f *.o *.core ${PROGRAM}

tags:		builtins.c exec.c redir.c getword.c parse.c ${PROGRAM}.c makefile getword.h parse.h redir.h exec.h builtins.h
		exctags -R --fields=+S --c-kinds=+defgmstux

lint:
		clang-tidy50 '-header-filter=.*' '-checks=*' *.c --
