///////////////////////////////////////////////////////////////////////
//
//  File/Class: IpSocket
// Description: Provides and IP socket.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef IPSCOKET_H
#define IPSCOKET_H

#include <netLib/Socket.h>
#include <netinet/in.h>

class IpAddress;

class IpSocket : public Socket
{
protected:
   IpSocket();
   IpSocket(int openFd);
   IpSocket(int openFd, const IpSocket& s);
   IpSocket(const IpSocket& s);

   virtual void connect(const IpAddress&);
   virtual void bind(const IpAddress&);
   virtual void getName(sockaddr_in&);
   virtual void getPeer(sockaddr_in&);

   // attributes
   sockaddr_in nearEnd;
   sockaddr_in farEnd;
};

#endif //IPSCOKET_H
