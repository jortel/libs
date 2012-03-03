##############################################################
##############################################################

LIBPATH   = -L. -L../lib
INCPATH   = -I. -I../lib
LIBS      = -lNet -lxnet
GNUCC     = g++ -g -Wall
SUN4CC    = CC -DPREANSI97
COMPILER  = $(GNUCC)
AR        = ar

#
# Pre ANSI/ISO 1997 compilers add: -DPREANSI97
#

CC = $(COMPILER) $(INCPATH) $(LIBPATH)

APPS = snmpRouted

##############################################################
# Targets
##############################################################

all:  $(APPS)

clean:
	/bin/rm -f *.o $(APPS)

snmpRouted:	snmpRouted.o
	$(CC) snmpRouted.cpp -o $@ $(LIBS)

# tools

send:	send.o
	$(CC) send.cpp -o $@ $(LIBS)


##############################################################
# suffixes
##############################################################

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) -c $<
