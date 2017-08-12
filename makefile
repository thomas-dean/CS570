# A Makefile for Program 0, CS570

PROGRAM = p0
TESTPROG = getword_test
CC = gcc
CFLAGS = -g

${PROGRAM}:	getword.o p0.o
		${CC} -o ${PROGRAM} getword.o ${PROGRAM}.o

${TESTPROG}:	getword.o ${TESTPROG}.o
		${CC} -o ${TESTPROG} getword.o ${TESTPROG}.o

${TESTPROG}.o:	getword.h

p0.o:		getword.h

getword.o:	getword.h

clean:
		rm -f *.o *.core ${PROGRAM} ${TESTPROG}

test:		${TESTPROG}
		./${TESTPROG}

lint:
		/opt/SUNWspro/bin/lint getword.c ${PROGRAM}.c
