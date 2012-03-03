#include <IpAddress.h>
#include <UdpSocket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>

int main(int argc, char **argv)
{
   if(argc < 3)
   {
      cout << "usage: send address port# <message>" << endl;
      return 1;
   }

   try
   {
      IpAddress ipAddr(argv[1], atoi(argv[2]));
      UdpSocket socketA(ipAddr);
      char reply[0xFFFF];
      memset(reply, 0, sizeof(reply));
      socketA.send(argv[3], strlen(argv[3]));
      //socketA.recv(reply, strlen(argv[3]));
      //cout << reply << endl;
   }
   catch(const NetErr&)
   {
      cout << "NetErr - terminated" << endl;
   }

   return 0;
}
