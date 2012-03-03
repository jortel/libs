///////////////////////////////////////////////////////////////////////
//
//  File/Class: emswd (main)
// Description:
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "EmWorkshopDaemon.h"
#include "EmWorkshopSrv.h"
#include "EmWorkshopCrypt.h"
#include <netLib/IpAddress.h>
#include <netLib/Unistd.h>
#include <netLib/Syslog.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <iostream.h>

static void showUsage()
{
   cout << endl;
   cout << "usage: emwsd -p <port> [ -a address ] [ -f ] [ -e ]" << endl;
   cout << endl;
}

int main(int argc, char** argv)
{
   int opt = 0;
   char *ip = 0;
   int port = 0;
   char *pwd = 0;
   char *epwd = 0;
   bool daemon = true;
   bool needLogin = true;

   while((opt = getopt(argc, argv, "nhfp:a:e:d:")) != -1)
   {
      switch(opt)
      {
         case 'h':
            showUsage();
            return 0;
            break;
         case 'p':
            port = atoi(optarg);
            break;
         case 'a':
            ip = strdup(optarg);
            break;
         case 'f':
            daemon = false;
            break;
         case 'n':
            needLogin = false;
            break;
         case 'e':
            pwd = strdup(optarg);
            break;
         case 'd':
            epwd = strdup(optarg);
            break;
         case '?':
            showUsage();
            return 0;
            break;
      }
   }

   // encode password
   if(pwd)
   {
      EmWorkshopCrypt crypt;
      cout << "encode: " << pwd;
      crypt.encode(pwd);
      cout << " [" << pwd << "]" << endl;
      return 0;
   }
   // decode password
   if(epwd) 
   {  
      EmWorkshopCrypt crypt; 
      cout << "decode: " << epwd;
      crypt.decode(epwd);
      cout << " [" << epwd << "]" << endl;
      return 0;
   } 

   // validate path
   if(argv[0][0] != '/')
   {
      cerr << "must execute using full path name" << endl;
      exit(1);
   }

   // server mode

   if(port == 0)
   {
      syslog(LOG_INFO,
             "Started (server) login=%s",
              (needLogin ? "yes" : "no"));

      EmWorkshopSrv server(argv[0], needLogin);
      server.run();
      return 0;
   }

   // daemon mode

   syslog(LOG_INFO,
          "Started (daemon) addr=%s port=%d",
           (ip ? ip : "ALL"),
           port);

   try
   {
      IpAddress addr(ip, port);
      EmWorkshopDaemon dmn(addr, argv[0], daemon);
      syslog(LOG_INFO, "Started");
      dmn.run();
   }
   catch(TcpSocket::Disconnect& e)
   {
      syslog(LOG_ERR, "Connection terminated");
   }
   catch(NetErr& e)
   {
      syslog(LOG_ERR, "Exception %s", e.errm);
   }

   return 0;
}
