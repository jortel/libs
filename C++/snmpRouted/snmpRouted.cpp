///////////////////////////////////////////////////////////////////////
//
//  File/Class: snmpRouted (main)
// Description: 
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include <netLib/UdpSocket.h>
#include <netLib/IpAddress.h>
#include <netLib/Unistd.h>
#include <netLib/Syslog.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream.h>
#include <string.h>
#include <errno.h>

// constants
static const int MaxRequest      = 510; // 500 outstanding requests
static const int RequestTimeout  = 30;  // 30 seconds, 1 response packet
static const int InterPktTimeout = 3;   // 3 seconds, inter-packet


///////////////////////////////////////////////////////////////////////////////
// SnmpRouted
///////////////////////////////////////////////////////////////////////////////

class SnmpRouted
{
public:
   static void sigChild(int);
   SnmpRouted(const IpAddress&, const IpAddress&, bool daemon);
   ~SnmpRouted();
   void run();

private:
   void serverInit();
   int setup(fd_set&);
   void routeRequest();
   void routeReply(int fd);
   void purgeRequestTable();

   struct Request
   {
      Request()
         :dest(0), expTime(0){};
      IpAddress origAddr;
      UdpSocket *dest;
      time_t expTime;
   };

   // attributes
   UdpSocket listener;
   IpAddress destAddr;
   Request reqTable[MaxRequest];
   char dataGram[0xFFFF];
};

void SnmpRouted::sigChild(int)
{
   int stat;
   pid_t pid;
   while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
   {
      syslog(LOG_INFO, "child pid=%d, terminated", pid);
   }

   signal(SIGCHLD, sigChild);
}

SnmpRouted::SnmpRouted(const IpAddress& sAddr, const IpAddress& dAddr, bool daemon)
   :destAddr(dAddr)
{
   memset(reqTable, 0, sizeof(reqTable));
   if(daemon) serverInit();
   listener.bind(sAddr);
}

SnmpRouted::~SnmpRouted()
{
   for(int i = 0; i < MaxRequest; i++)
   {
      Request& req = reqTable[i];
      delete req.dest;
      req.dest = 0;
   }
}

void SnmpRouted::serverInit()
{
   signal(SIGCHLD, sigChild);

   pid_t pid = fork();
   switch(pid)
   {
      case -1:
         syslog(LOG_ERR, "fork() failed \"%s\"", strerror(errno));
         exit(0);
         break;
      case 0: // child
         setsid();
         {
            rlimit newLimit;
            newLimit.rlim_cur = MaxRequest-1;
            newLimit.rlim_max = MaxRequest-1;
            if(setrlimit(RLIMIT_NOFILE, &newLimit) == -1)
            {
               syslog(LOG_ERR, "setrlimit(maxfd=%d) failed \"%s\"",
                  newLimit.rlim_max, strerror(errno));
            }
         }
         break;
      default: // parent
         exit(0);
   }
}

int SnmpRouted::setup(fd_set& set)
{
   FD_ZERO(&set);
   int maxfd = listener.getfd();
   FD_SET(maxfd, &set);
   for(int i = 0; i < MaxRequest; i++)
   {
      UdpSocket *dest = reqTable[i].dest;
      if(dest)
      {
         FD_SET(dest->getfd(), &set);
         if(i > maxfd) maxfd = i;
      }
   }

   return maxfd+1;
}

void SnmpRouted::routeRequest()
{
   try
   {
      Request req;
      req.dest = new UdpSocket(destAddr);
      req.expTime = time(0)+RequestTimeout;
      if(req.dest)
      {
         req.dest->connect();
         int nbytes = listener.bytesToRead(sizeof(dataGram));
         listener.read(dataGram, nbytes);
         req.origAddr = listener.getSender();
         req.dest->write(dataGram, nbytes);
         int fd = req.dest->getfd();
         reqTable[fd] = req;
      }
   }
   catch(NetErr& e)
   {
      syslog(LOG_ERR, "Request not routed: %s", e.errm);
   }
}

void SnmpRouted::routeReply(int fd)
{
   Request& req = reqTable[fd];

   try
   {
      if(req.dest)
      {
         UdpSocket originator(req.origAddr);
         int nbytes = req.dest->bytesToRead(sizeof(dataGram));
         req.dest->read(dataGram, nbytes);
         originator.write(dataGram, nbytes);
         req.expTime = time(0)+InterPktTimeout;
      }
   }
   catch(NetErr& e)
   {
      syslog(LOG_ERR, "Request not routed: %s", e.errm);
      req.expTime = 0;
      delete req.dest;
      req.dest = 0;
   }
}

void SnmpRouted::purgeRequestTable()
{
   time_t cTime = time(0);
   for(int i = 0; i < MaxRequest; i++)
   {
      Request& req = reqTable[i];
      if((req.expTime) && (cTime > req.expTime))
      {
         req.expTime = 0;
         delete req.dest;
         req.dest = 0;
      }
   }
}

void SnmpRouted::run()
{
   timeval tm;
   fd_set rdset;
   int numfd, maxfd;
   int reqfd = listener.getfd();
   tm.tv_sec = 1;
   tm.tv_usec = 0;

   while(1)
   {
      maxfd = setup(rdset);
      numfd = select(maxfd, &rdset, 0, 0, &tm);
      for(int fd = maxfd; ((numfd > 0) && (fd >= 0)); fd--)
      {
         if(FD_ISSET(fd, &rdset))
         {
            if(fd == reqfd)
               routeRequest();
            else
               routeReply(fd);
         }
      }

      purgeRequestTable();
   }
}

///////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////

static void showUsage()
{
   cout << endl;
   cout << "usage: snmpRouted [ -h ] [ -i interface (optional)] ";
   cout << "[ -p port ] [ -d host:port ]" << endl;
   cout << endl;
}

int main(int argc, char** argv)
{
   bool daemon = true;
   char *ip = 0, *dest = 0;
   int opt, port = 0;
   while((opt = getopt(argc, argv, "hi:p:d:f")) != -1)
   {
      switch(opt)
      {
         case 'i':
            ip = strdup(optarg);
            break;
         case 'p':
            port = atoi(optarg);
            break;
         case 'd':
            dest = strdup(optarg);
            break;
         case 'h':
            showUsage();
            return 0;
            break;
         case 'f':
            daemon = false;
            break;
         case '?':
            showUsage();
            return 0;
            break;
      }
   }

   if((! port) || (! dest))
   {
      showUsage();
      return 1;
   }

   syslog(LOG_INFO, "Routing: %s:%d to: %s", (ip ? ip : "ALL"), port, dest);

   try
   {
      IpAddress ipAddr(ip, port);
      IpAddress destAddr(dest);
      SnmpRouted server(ipAddr, destAddr, daemon);
      syslog(LOG_INFO, "Started");
      server.run();
   }
   catch(NetErr& e)
   {
      syslog(LOG_ERR, "Exception %s", e.errm);
   }

   return 0;
}
