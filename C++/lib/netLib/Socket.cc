///////////////////////////////////////////////////////////////////////
//
//  File/Class: Socket
// Description: Provides a socket base class.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/Socket.h"
#include "netLib/Unistd.h"
#include <sys/filio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>

Socket::Socket()
 :fd(-1)
{
}

Socket::Socket(const Socket& s)
 :fd(-1)
{
   if(s.fd != -1)
   {
      fd = Dup(s.fd);
   }
}

Socket::Socket(int openFd)
 :fd(-1)
{
   if(openFd != -1)
   {
      fd = Dup(openFd);
   }
}

Socket::~Socket()
{
   close();
}

Socket& Socket::operator=(const Socket& s)
{
   if(s.fd != -1)
   {
      fd = Dup(s.fd);
   }

   return *this;
}

void Socket::close()
{
   if(fd != -1)
   {
      ::close(fd);
      fd = -1;
   }
}

int Socket::bytesToRead(int limit) const
{
   int result = 0;
   ioctl(fd, FIONREAD, &result);
   return (result > limit ? limit : result);
}

int Socket::read(char*, int)
{
   return 0;
}

int Socket::write(const char*, int)
{
   return 0;
}
