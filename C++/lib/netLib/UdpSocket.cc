///////////////////////////////////////////////////////////////////////
//
//  File/Class: UdpSocket
// Description: Provides a UDP/IP socket object.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/UdpSocket.h"
#include "netLib/IpAddress.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


UdpSocket::UdpSocket()
   :connected(0)
{
   fd = socket(AF_INET, SOCK_DGRAM, 0);
   if(fd == -1)
   {
      throw SocketErr("socket(AF_INET, SOCK_DGRAM)",
         __FILE__, __LINE__);
   }
}

UdpSocket::UdpSocket(const IpAddress& ipAddr)
   :connected(0)
{
   farEnd = (sockaddr_in&)ipAddr;
   fd = socket(AF_INET, SOCK_DGRAM, 0);
   if(fd == -1)
   {
      throw SocketErr("socket(AF_INET, SOCK_DGRAM)",
         __FILE__, __LINE__);
   }
}

void UdpSocket::bind()
{
   bind(nearEnd);
}

void UdpSocket::bind(const IpAddress& ipAddr)
{
   nearEnd = (sockaddr_in&)ipAddr;
   while(1)
   {
      if(::bind(fd, (sockaddr*)&nearEnd, sizeof(nearEnd)) == -1)
      {
         if(errno != EINTR)
            throw SocketErr("bind", __FILE__, __LINE__);
      }
      else
         break;
   }

   getName(nearEnd);
}

void UdpSocket::connect()
{
   connect(farEnd);
}

void UdpSocket::connect(const IpAddress& ipAddr)
{
   farEnd = (sockaddr_in&)ipAddr;
   while(1)
   {
      if(::connect(fd, ipAddr, ipAddr.getLength()) == -1)
      {
         if(errno != EINTR)
            throw SocketErr("connect", __FILE__, __LINE__);
      }
      else
         break;
   }
   connected = true;
}

int UdpSocket::send(const char *p, int nbytes)
{
   int wr, bytesToWrite = nbytes;
   int bytesWritten = 0;

   while(bytesToWrite)
   {
      if(connected)
         wr = ::write(fd, p, bytesToWrite);
      else
         wr = ::sendto(fd, p, bytesToWrite, 0,
                        (sockaddr*)&farEnd, sizeof(farEnd));

      if(wr == -1)
      {
         if(errno != EINTR)
            throw SocketErr("sendto", __FILE__, __LINE__);
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

int UdpSocket::recv(char *p, int nbytes)
{
   int fSize = sizeof(farEnd);
   int rr, bytesToRead = nbytes;
   int bytesRead = 0;

   while(bytesToRead)
   {
      if(connected)
         rr = ::read(fd, p, bytesToRead);
      else
         rr = ::recvfrom(fd, p, bytesToRead, 0, (sockaddr*)&farEnd, &fSize);

      if(rr == -1)
      {
         if(errno != EINTR)
            throw SocketErr("recvfrom", __FILE__, __LINE__);
      }
      else
      {
         bytesRead += rr;
         bytesToRead -= rr;
         p += rr;
      }
   }

   sender = farEnd;
   return bytesRead;
}
