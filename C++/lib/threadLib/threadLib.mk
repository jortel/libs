##############################################################
##############################################################

LIBPATH   = -L.
INCPATH   = -I..
GNUCC     = g++ -g -Wall
SUN4CC    = CC -DPREANSI97
COMPILER  = $(GNUCC)
AR        = ar

#
# Pre ANSI/ISO 1997 compilers add: -DPREANSI97
#

CC = $(COMPILER) $(INCPATH) $(LIBPATH)

THREADLIBOBJ = Thread.o ThreadRoot.o ThreadErr.o ThreadData.o \
               Lock.o Mutex.o QueuedConsumer.o EventLatchedQueue.o

##############################################################
# Targets
##############################################################

test:	test.cc libThread.a
	$(CC) test.cc -o test -lThread -lpthread -lposix4

libThread.a:	$(THREADLIBOBJ)
	$(AR) -r $@ $(THREADLIBOBJ)

install: libThread.a
	cp libThread.a ../

clean:
	/bin/rm -f *.o *.a

##############################################################
# libNet
##############################################################

Thread.o:	Thread.cc Thread.h
	$(CC) -c $<

##############################################################
# suffixes
##############################################################

.SUFFIXES: .cc .o

.cc.o:
	$(CC) -c $<
