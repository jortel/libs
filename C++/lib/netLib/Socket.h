///////////////////////////////////////////////////////////////////////
//
//  File/Class: Socket
// Description: Provides a socket base class.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

class Socket
{
public:
   virtual ~Socket();

   operator int() const;
   Socket& operator=(const Socket& s);
   int getfd() const;
   int bytesToRead(int limit) const;
   virtual void close();
   virtual int read(char *p, int nbytes);
   virtual int write(const char *p, int nbytes);

protected:
   Socket();
   Socket(int openFd);
   Socket(const Socket& s);

   // attributes
   int fd;
};

inline
Socket::operator int() const
{
   return fd;
}

inline
int Socket::getfd() const
{
   return fd;
}


#include "netLib/NetErr.h"
struct SocketErr : public NetErr
{
   SocketErr(const char* context,
             const char* fileLocation,
             int lineLocation)
     :NetErr(context, fileLocation, lineLocation)
     {};
};


#endif //SOCKET_H
