///////////////////////////////////////////////////////////////////////
//
//  File/Class: UdpSocket
// Description: Provides a UDP/IP socket object.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <netLib/IpSocket.h>
#include <netLib/IpAddress.h>
#include <netinet/in.h>

class UdpSocket : public IpSocket
{
public:
   UdpSocket();
   UdpSocket(const IpAddress& ipAddr);

   void bind();
   void bind(const IpAddress& ipAddr);
   void connect();
   void connect(const IpAddress& ipAddr);

   int read(char *p, int nbytes);
   int write(const char *p, int nbytes);
   int send(const char *p, int nbytes);
   int recv(char *p, int nbytes);
   const IpAddress& getSender() const;

private:
   // attributes
   bool connected;
   IpAddress sender;
};

inline
int UdpSocket::read(char *p, int nbytes)
{
   return recv(p, nbytes);
}

inline
int UdpSocket::write(const char *p, int nbytes)
{
   return send(p, nbytes);
}

inline
const IpAddress& UdpSocket::getSender() const
{
   return sender;
}

#endif //UDPSOCKET_H
