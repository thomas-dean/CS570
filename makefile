# A Makefile for Program 0, CS570

PROGRAM = p0
CC = gcc
CFLAGS = -g

${PROGRAM}:	getword.o p0.o
		${CC} -o ${PROGRAM} getword.o ${PROGRAM}.o

p1.o:		getword.h

getword.o:	getword.h

clean:
		rm -f *.o ${PROGRAM}

lint:
		/opt/SUNWspro/bin/lint getword.c ${PROGRAM}.c
