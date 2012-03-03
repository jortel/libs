///////////////////////////////////////////////////////////////////////
//
//  File/Class: TcpSocket
// Description: Provides a TCP/IP Socket.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/TcpSocket.h"
#include "netLib/IpAddress.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

TcpSocket::TcpSocket()
{
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if(fd == -1)
   {
      throw Err("socket(AF_INET, SOCK_STREAM)",
         __FILE__, __LINE__);
   }
}

TcpSocket::TcpSocket(const TcpSocket& s)
 :IpSocket(s)
{
}

TcpSocket::TcpSocket(int openFd)
 :IpSocket(openFd)
{
}

TcpSocket::TcpSocket(int openFd, const TcpSocket& s)
 :IpSocket(openFd, s)
{
}

void TcpSocket::bind(const IpAddress& ipAddr)
{
   nearEnd = (sockaddr_in&)ipAddr;
   while(1)
   {
      if(::bind(fd, (sockaddr*)&nearEnd, sizeof(nearEnd)) == -1)
      {
         if(errno != EINTR)
            throw Err("bind", __FILE__, __LINE__);
      }
      else
         break;
   }
   getName(nearEnd);
}

int TcpSocket::write(const char *p, int nbytes)
{
   int wr, bytesToWrite = nbytes;
   int bytesWritten = 0;

   while(bytesToWrite)
   {
      wr = ::write(fd, p, bytesToWrite);
      if(wr == -1)
      {
         if(errno != EINTR)
            throw Err("write", __FILE__, __LINE__);
      }
      else
      {
         bytesWritten += wr;
         bytesToWrite -= wr;
         p += wr;
      }
   }

   return bytesWritten;
}

int TcpSocket::read(char *p, int nbytes)
{
   int rr, bytesToRead = nbytes;
   int bytesRead = 0;

   while(bytesToRead)
   {
      rr = ::read(fd, p, bytesToRead);
      switch(rr)
      {
         case 0:
            throw Disconnect("read", __FILE__, __LINE__);
            break;
         case -1:
            if(errno != EINTR)
            {
               throw Err("read", __FILE__, __LINE__);
            }
            break;
         default:
            bytesRead += rr;
            bytesToRead -= rr;
            p += rr;
            break;
      }
   }

   return bytesRead;
}

void TcpSocket::listen(int backlog)
{
   while(1)
   {
      if(::listen(fd, backlog) == -1)
      {
         if(errno != EINTR)
            throw Err("listen", __FILE__, __LINE__);
      }
      else
         break;
   }
}

TcpSocket* TcpSocket::accept()
{
   while(1)
   {
      int nConn = ::accept(fd, 0, 0);
      if(nConn == -1)
      {
         if(errno != errno)
            throw Err("accept", __FILE__, __LINE__);
      }
      else
      {
         TcpSocket *newSocket = new TcpSocket(nConn, *this);
         ::close(nConn);
         return newSocket;
      }
   }

   return 0;
}

void TcpSocket::connect(const IpAddress& ipAddr)
{
   farEnd = (sockaddr_in&)ipAddr;
   while(1)
   {
      if(::connect(fd, ipAddr, ipAddr.getLength()) == -1)
      {
         if(errno != EINTR)
            throw Err("connect", __FILE__, __LINE__);
      }
      else
         break;
   }
}

bool TcpSocket::setNoDelay(bool on_off)
{
   int op = (on_off ? 1 : 0);
   int opLen = sizeof(op);
   return (setsockopt(fd,
                      IPPROTO_TCP,
                      TCP_NODELAY,
                      (char*)&op,
                      opLen) != -1);
}

bool TcpSocket::setLinger(int seconds)
{
   linger op;
   op.l_onoff = seconds;
   op.l_linger = seconds;
   int opLen = sizeof(op);
   return (setsockopt(fd,
                      SOL_SOCKET,
                      SO_LINGER,
                      (char*)&op,
                      opLen) != -1);   
}
