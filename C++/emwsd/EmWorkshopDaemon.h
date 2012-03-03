///////////////////////////////////////////////////////////////////////
//
//  File/Class: EmWorkshopDaemon
// Description: The Remote EM Workshop deamon
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef EMWORKSHOPDAEMON_H
#define EMWORKSHOPDAEMON_H

#include <netLib/TcpSocket.h>

class EmWorkshopDaemon
{
public:
   static void sigChild(int);
   EmWorkshopDaemon(const IpAddress&, const char *client, bool daemon);
   ~EmWorkshopDaemon();
   void run();

private:
   void serverInit();
   void inetInit(const IpAddress&);
   void handleConnection(TcpSocket *newConnect);
   void launchServer(TcpSocket& socket);

   // attributes
   TcpSocket listener;
   const char *arg0;
};

#endif // EMWORKSHOPDAEMON_H
