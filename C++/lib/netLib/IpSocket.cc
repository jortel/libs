///////////////////////////////////////////////////////////////////////
//
//  File/Class: IpSocket
// Description: Provides and IP socket.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/IpSocket.h"
#include <string.h>

IpSocket::IpSocket()
{
   memset(&nearEnd, 0, sizeof(nearEnd));
   memset(&farEnd, 0, sizeof(farEnd));
}

IpSocket::IpSocket(int openFd)
 :Socket(openFd)
{
   memset(&nearEnd, 0, sizeof(nearEnd));
   memset(&farEnd, 0, sizeof(farEnd));
}

IpSocket::IpSocket(int openFd, const IpSocket& s)
 :Socket(openFd),
  nearEnd(s.nearEnd),
  farEnd(s.farEnd)
{
}

IpSocket::IpSocket(const IpSocket& s)
 :Socket(s),
  nearEnd(s.nearEnd),
  farEnd(s.farEnd)
{
}

void IpSocket::connect(const IpAddress&)
{
}

void IpSocket::bind(const IpAddress&)
{
}

void IpSocket::getName(sockaddr_in& addr)
{
   int len = sizeof(addr);
   if(getsockname(fd, &(sockaddr&)addr, &len) == -1)
   {
      throw SocketErr("getsockname", __FILE__, __LINE__);
   }
}

void IpSocket::getPeer(sockaddr_in& addr)
{
   int len = sizeof(addr);
   if(getpeername(fd, &(sockaddr&)addr, &len) == -1)
   {
      throw SocketErr("getpeername", __FILE__, __LINE__);
   }
}
