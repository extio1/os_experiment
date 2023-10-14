PROG_1=unithread_sigdelivery
PROG_2=multithread_sigdelivery

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${PROG_1} ${PROG_2}

${PROG_1}:
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${PROG_1}.c ${LIBS} -o ${PROG_1}

${PROG_2}:
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${PROG_2}.c ${LIBS} -o ${PROG_2}

clean:
	rm -f ${PROG_1} ${PROG_2}