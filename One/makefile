# A Makefile for Program 0, CS570

PROGRAM = p1
TESTPROG = getword_test
CC = clang
CFLAGS = -march=native -O2 -Os -Wall -Wextra -pedantic

${PROGRAM}:	getword.o ${PROGRAM}.o
		${CC} -o ${PROGRAM} getword.o ${PROGRAM}.o

${TESTPROG}:	getword.o ${TESTPROG}.o
		${CC} -o ${TESTPROG} getword.o ${TESTPROG}.o

${TESTPROG}.o:	getword.h

${PROGRAM}.o:	getword.h

getword.o:	getword.h

clean:
		rm -f *.o *.core ${PROGRAM} ${TESTPROG}

test:		${TESTPROG}
		./${TESTPROG}

tags:		getword.c ${PROGRAM}.c makefile getword.h ${TESTPROG}.c
		exctags -R --fields=+S --c-kinds=+defgmstux

lint:
		clang-tidy39 '-header-filter=.*' '-checks=*' *.c --
