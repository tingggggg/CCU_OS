SHELL = /bin/bash
CC = gcc
CFLAGS = -pthread
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all: ${EXE}

%:	%.c
	${CC} ${CFLAGS} -O3 $@.c -o $@
	${CC} ${CFLAGS} -g  $@.c -o $@-g

clean:
	rm ${EXE} *-g
