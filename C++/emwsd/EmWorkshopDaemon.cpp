///////////////////////////////////////////////////////////////////////
//
//  File/Class: EmWorkshopDaemon
// Description: The Remote EM Workshop deamon
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "EmWorkshopDaemon.h"
#include <netLib/TcpSocket.h>
#include <netLib/IpAddress.h>
#include <netLib/Unistd.h>
#include <netLib/Syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

void EmWorkshopDaemon::sigChild(int)
{
   int stat;
   pid_t pid;
   while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
   {
      syslog(LOG_INFO, "child pid=%d, terminated", pid);
   }

   signal(SIGCHLD, sigChild);
}

EmWorkshopDaemon::EmWorkshopDaemon(const IpAddress& addr,
                                   const char *arg0,
                                   bool daemon)
 :arg0(arg0)
{
   if(daemon) serverInit();
   inetInit(addr);
}

EmWorkshopDaemon::~EmWorkshopDaemon()
{
}

void EmWorkshopDaemon::run()
{
   while(1)
   {
      handleConnection(listener.accept());
   }
}

void EmWorkshopDaemon::serverInit()
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
         break;
      default: // parent
         exit(0);
   }
}

void EmWorkshopDaemon::inetInit(const IpAddress& addr)
{
   listener.bind(addr);
   listener.listen();
}

void EmWorkshopDaemon::handleConnection(TcpSocket *newConnect)
{
   if(newConnect)
   {
      syslog(LOG_INFO,
             "Connection accepted (fd=%d)",
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
            launchServer(*newConnect);
            exit(0);
            break;
         default: // parent
            break;
      }
      newConnect->close();
      delete newConnect;
   }
}

void EmWorkshopDaemon::launchServer(TcpSocket& socket)
{
   umask(0002);
   putenv("SHELL=/bin/sh");

   close(0);
   close(1);
   close(2);
   dup(socket);
   dup(socket);
   dup(socket);

   execl(arg0, arg0, 0);
}
