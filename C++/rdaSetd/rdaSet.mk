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

APPS = rdaSetd rdaSet

##############################################################
# Targets
##############################################################

all:  $(APPS)

clean:
	/bin/rm -f *.o $(APPS)

rdaSetd:	rdaSetd.o ChildProcess.o
	$(CC) rdaSetd.o ChildProcess.o -o $@ $(LIBS)

ChildProcess.o:	ChildProcess.cpp ChildProcess.h
	$(CC) -c $<

rdaSet:	rdaSet.cpp
	$(CC) rdaSet.cpp -o $@ $(LIBS)

# testing

daSet:	daSet.cpp
	$(CC) daSet.cpp -o $@

##############################################################
# suffixes
##############################################################

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) -c $<
