#Makefile to build unit tests
CC      = gcc
BASEDIR = .
CFLAGS  = -Wall -I. -g -DTEST -DTEST_DBUFFER

TARGET = dbuffer

SRCS = dbuffer.c ctest.c

OBJS = ${SRCS:.c=.o}

all: ${TARGET}
 
${TARGET}: ${OBJS}
	${CC} -o $@ ${OBJS} 

.c.o:
	${CC} -c ${CFLAGS} $*.c
	
depend:
	rm -f .depend
	${CC} -MM ${CFLAGS} *.c >> .depend
	
clean:
	rm -rf core ${OBJS} ${TARGET} *.bak

include: .depend
