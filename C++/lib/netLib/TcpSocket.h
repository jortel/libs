///////////////////////////////////////////////////////////////////////
//
//  File/Class: TcpSocket
// Description: Provides a TCP/IP Socket.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "netLib/IpSocket.h"
#include <netinet/in.h>

class IpAddress;

class TcpSocket : public IpSocket
{
public:
   TcpSocket();
   TcpSocket(const TcpSocket& s);
   TcpSocket(int openFd);
   TcpSocket(int openFd, const TcpSocket& s);

   class Err;
   class Disconnect;

   void bind(const IpAddress& ipAddr);
   void listen(int backlog = 5);
   TcpSocket* accept();
   void connect(const IpAddress& ipAddr);
   int read(char *p, int nbytes);
   int write(const char *p, int nbytes);
   bool setNoDelay(bool);
   bool setLinger(int seconds);
};


class TcpSocket::Err : public SocketErr
{
public:
   Err(const char* context,
       const char* fileLocation,
       int lineLocation)
   :SocketErr(context, fileLocation, lineLocation)
    {};
};

class TcpSocket::Disconnect : public TcpSocket::Err
{
public:
   Disconnect(const char* context,
              const char* fileLocation,
              int lineLocation)
   :Err(context, fileLocation, lineLocation)
    {};
};

#endif //TCPSOCKET_H
