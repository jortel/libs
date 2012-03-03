##############################################################
##############################################################

LIBPATH   = -L./
INCPATH   = -I../
GNUCC     = g++ -g -Wall
SUN4CC    = CC -DPREANSI97
COMPILER  = $(GNUCC)
AR        = ar

#
# Pre ANSI/ISO 1997 compilers add: -DPREANSI97
#

CC = $(COMPILER) $(INCPATH) $(LIBPATH)

NETLIBOJB = NetErr.o Dbuffer.o PExec.o Unistd.o IpAddress.o \
            Socket.o IpSocket.o UdpSocket.o TcpSocket.o

##############################################################
# Targets
##############################################################

libNet.a:	$(NETLIBOJB)
	$(AR) -r $@ $(NETLIBOJB)

install: libNet.a
	cp libNet.a ../

clean:
	/bin/rm -f *.o *.a

##############################################################
# libNet
##############################################################

NetErr.o:	NetErr.cc NetErr.h
	$(CC) -c $<

Dbuffer.o:	Dbuffer.cc Dbuffer.h
	$(CC) -c $<

PExec.o:	PExec.cc PExec.h
	$(CC) -c $<

Unistd.o:	Unistd.cc Unistd.h
	$(CC) -c $<

IpAddress.o:	IpAddress.cc IpAddress.h
	$(CC) -c $<

Socket.o:	Socket.cc Socket.h NetErr.h
	$(CC) -c $<

IpSocket.o:	IpSocket.cc IpSocket.h Socket.h
	$(CC) -c $<

UdpSocket.o:	UdpSocket.cc UdpSocket.h IpSocket.h
	$(CC) -c $<

TcpSocket.o:	TcpSocket.cc TcpSocket.h IpSocket.h
	$(CC) -c $<

##############################################################
# suffixes
##############################################################

.SUFFIXES: .cc .o

.cc.o:
	$(CC) -c $<
