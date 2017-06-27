#Makefile to build unit tests
CC      = gcc
CFLAGS  = -Wall -I. -g -DTEST -DTEST_BACNET_TEXT

TARGET = bacnet_text

SRCS = bacnet_text.c ctest.c

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
