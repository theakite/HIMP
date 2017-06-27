#Makefile to build BACnet4Linux
CC      = gcc
BASEDIR = .
#CFLAGS  = -Wall -I.
# -g for debugging with gdb
#CFLAGS  = -Wall -I. -g
# -O2 optimize for better performance
CFLAGS  = -Wall -I. -g -O2

TARGET = bacnet_rp

SRCS = bacnet_rp.c net.c bacnet_text.c invoke_id.c bacdcode.c \
	    signal_handler.c \
	    bacnet_object.c bacnet_device.c \
	    send_npdu.c send_read_property.c \
	    send_whois.c \
	    send_bip.c packet.c ethernet.c \
	    receive_apdu.c receive_readproperty.c \
	    receive_npdu.c receive_readpropertyACK.c receive_COV.c \
	    receive_iam.c receive_bip.c \
	    debug.c pdu.c reject.c \
	    keylist.c dstring.c dbuffer.c bigendian.c \
	    version.c

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


