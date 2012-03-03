///////////////////////////////////////////////////////////////////////
//
//  File/Class: IpAddress
// Description: Provides an IP address that performs DNS host name lookup
//              and other address and port translations.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/IpAddress.h"
#include "netLib/NetErr.h"
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

static in_addr_t translate(const char* ipAddr)
{
   if(ipAddr)
   {
      hostent *entry = gethostbyname(ipAddr);
      if(entry)
      {
         in_addr_t inetAddr;
         memcpy(&inetAddr, entry->h_addr_list[0], entry->h_length);
         return htonl(inetAddr);
      }
      else
         throw NetErr("gethostbyname", __FILE__, __LINE__);
   }

   return INADDR_ANY;
}

void IpAddress::init(const char* ipAddr, int port)
{
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(translate(ipAddr));
   addr.sin_port = htons(port);
}

IpAddress::IpAddress()
{
   init(0, 0);
}

IpAddress::IpAddress(const char* ipAndPort)
{
   int port = 0;
   char *ipAddr = strdup(ipAndPort);
   if(ipAddr)
   {
      for(int i = 0; ipAddr[i]; i++)
      {
         if(ipAddr[i] == ':')
         {
            ipAddr[i++] = '\0';
            port = atoi(&ipAddr[i]);
            break;
         }
      }
      init(ipAddr, port);
      free(ipAddr);
   }
   else
      init(0, 0);
}

IpAddress::IpAddress(const char* ipAddr, int port)
{
   init(ipAddr, port);
}

IpAddress::IpAddress(const sockaddr_in& a)
 :addr(a)
{
}
