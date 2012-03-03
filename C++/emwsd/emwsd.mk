##############################################################
##############################################################

LIBPATH   = -L. -L../lib/
INCPATH   = -I. -I../lib
LIBS      = -lNet -lxnet
GNUCC     = g++ -s -Wall
SUN4CC    = CC -DPREANSI97
COMPILER  = $(GNUCC)
AR        = ar

#
# Pre ANSI/ISO 1997 compilers add: -DPREANSI97
#

CC = $(COMPILER) $(INCPATH) $(LIBPATH)

APPS = emwsd
OBJS = EmWorkshopDaemon.o EmWorkshopSrv.o EmWorkshopCrypt.o

##############################################################
# Targets
##############################################################

all:  $(APPS)

clean:
	/bin/rm -f *.o $(APPS)

emwsd:	emwsd.o $(OBJS)
	$(CC) emwsd.o $(OBJS) -o $@ $(LIBS)

##############################################################
# suffixes
##############################################################

.SUFFIXES: .cpp .cc .o

.cc.o:
	$(CC) -c $<
.cpp.o:
	$(CC) -c $<
