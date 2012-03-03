///////////////////////////////////////////////////////////////////////
//
//  File/Class: rdaSetd (main)
// Description: Provides the server for remote daSet/daGet.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "ChildProcess.h"
#include "netLib/Dbuffer.h"
#include <netLib/TcpSocket.h>
#include <netLib/IpAddress.h>
#include <netLib/Unistd.h>
#include <netLib/Syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

// constants
static const int MaxChild = 300;


///////////////////////////////////////////////////////////////////////////////
// RDaSetd
///////////////////////////////////////////////////////////////////////////////

class RDaSetd
{
public:
   static void sigChild(int);
   RDaSetd(const IpAddress&, bool daemon);
   ~RDaSetd();
   void run();
   void setApp(const char*);

private:
   void serverInit();
   void inetInit(const IpAddress&);
   void destroyChild(int child);
   void multiplexData(TcpSocket& socket);
   void handleConnection(TcpSocket *newConnect);
   int setup(TcpSocket&, fd_set&);
   void submit();
   void forwardData(TcpSocket& socket);
   void forwardData(TcpSocket& socket, int child);

   // attributes
   TcpSocket listener;
   const char *app;
   ChildProcess *children[MaxChild];
   Dbuffer sBuffer;
};

void RDaSetd::sigChild(int)
{
   int stat;
   pid_t pid;
   while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
   {
      syslog(LOG_INFO, "child pid=%d, terminated", pid);
   }

   signal(SIGCHLD, sigChild);
}

RDaSetd::RDaSetd(const IpAddress& addr, bool daemon)
 :app(0)
{
   memset(children, 0, sizeof(children));
   if(daemon) serverInit();
   inetInit(addr);
}

RDaSetd::~RDaSetd()
{
}

void RDaSetd::setApp(const char* aValue)
{
   app = aValue;
}

void RDaSetd::serverInit()
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
            newLimit.rlim_cur = MaxChild*2;
            newLimit.rlim_max = MaxChild*2;
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

void RDaSetd::inetInit(const IpAddress& addr)
{
   listener.bind(addr);
   listener.listen();
};

void RDaSetd::destroyChild(int child)
{
   delete children[child];
   children[child] = 0;
}

int RDaSetd::setup(TcpSocket& socket, fd_set& set)
{
   FD_ZERO(&set);
   int maxfd = socket.getfd();
   FD_SET(maxfd, &set);
   for(int i = 0; i < MaxChild; i++)
   {
      if(children[i])
      {
         FD_SET(i, &set);
         if(i > maxfd) maxfd = i;
      }
   }

   return maxfd+1;
}

void RDaSetd::submit()
{
   sBuffer.prepend(app, strlen(app));
   ChildProcess *child = new ChildProcess(sBuffer);
   if(child)
   {
      int idx = child->execute();
      if(idx > -1)
         children[idx] = child;
   }

   sBuffer.clear();
}

void RDaSetd::forwardData(TcpSocket& socket)
{
   static char buffer[1024];
   memset(buffer, 0, sizeof(buffer));
   int nbytes = socket.bytesToRead(sizeof(buffer));
   if(nbytes == 0)
      throw TcpSocket::Disconnect("read", __FILE__, __LINE__);

   socket.read(buffer, nbytes);
   for(int start = 0, end = 0; end < nbytes; end++)
   {
      if((end == nbytes) || (buffer[end] == '\n'))
      {
         sBuffer.append(&buffer[start], (end-start));
         start = end+1;
         if(buffer[end] == '\n')
            submit();
      }
   }
}

void RDaSetd::forwardData(TcpSocket& socket, int child)
{
   static char buffer[1024];
   memset(buffer, 0, sizeof(buffer));
   int nbytes = children[child]->bytesToRead(sizeof(buffer));
   if(children[child]->readStdout(buffer, nbytes))
   {
      socket.write(buffer, nbytes);
      return;
   }
   buffer[0] = 0x04;
   socket.write(buffer, 1);
   destroyChild(child);
}

void RDaSetd::multiplexData(TcpSocket& socket)
{
   fd_set rdset;
   int numfd, maxfd;
   int clientfd = socket.getfd();

   while(1)
   {
      maxfd = setup(socket, rdset);
      numfd = select(maxfd, &rdset, 0, 0, 0);
      for(int fd = maxfd; ((numfd > 0) && (fd >= 0)); fd--)
      {
         if(FD_ISSET(fd, &rdset))
         {
            if(fd == clientfd)
               forwardData(socket);
            else
               forwardData(socket, fd);
         }
      }
   }
}

void RDaSetd::handleConnection(TcpSocket *newConnect)
{
   if(newConnect)
   {
      syslog(LOG_INFO, "Connection accepted (fd=%d)",
         newConnect->getfd());

      newConnect->setLinger(0);
      newConnect->setNoDelay(true);
      pid_t pid = fork();
      switch(pid)
      {
         case -1:
            syslog(LOG_ERR, "fork() failed \"%s\"", strerror(errno));
            break;
         case 0: // child
            listener.close();
            multiplexData(*newConnect);
            exit(0);
            break;
         default: // parent
            break;
      }
      newConnect->close();
      delete newConnect;
   }
}

void RDaSetd::run()
{
   while(1)
   {
      handleConnection(listener.accept());
   }
}

///////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////

static void showUsage()
{
   cout << endl;
   cout << "usage: rdaSetd [ -h ] [ -a address (optional)] ";
   cout << "[ -p port ] [ -e program ]" << endl;
   cout << endl;
}

int main(int argc, char** argv)
{
   char *program = 0, *ip = 0;
   int opt, port = 0;
   bool daemon = true;
   while((opt = getopt(argc, argv, "a:hp:e:f")) != -1)
   {
      switch(opt)
      {
         case 'p':
            port = atoi(optarg);
            break;
         case 'a':
            ip = strdup(optarg);
            break;
         case 'e':
            program = new char[strlen(optarg)+2];
            if(program)
               sprintf(program, "%s ", optarg);
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

   if((port == 0) || (program == 0))
   {
      showUsage();
      return 1;
   }

   syslog(LOG_INFO, "Started addr=%s port=%d exec=\"%s\"",
      (ip ? ip : "ALL"), port, program);

   try
   {
      IpAddress addr(ip, port);
      RDaSetd server(addr, daemon);
      server.setApp(program);
      syslog(LOG_INFO, "Started");
      server.run();
   }
   catch(TcpSocket::Disconnect& e)
   {
      syslog(LOG_ERR, "Connection terminated");
   }
   catch(NetErr& e)
   {
      syslog(LOG_ERR, "Exception %s", e.errm);
   }

   return 0;
}
