///////////////////////////////////////////////////////////////////////
//
//  File/Class: rdaSet (main)
// Description: remote daSet application
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include <netLib/TcpSocket.h>
#include <netLib/IpAddress.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

static void showUsage()
{
   cout << endl;
   cout << "usage: rdaSet [ host ] [ port ] [ ... ]" << endl;
   cout << endl;
}

int main(int argc, char** argv)
{
   if(argc < 4)
   {
      showUsage();
      return 1;
   }

   try
   {
      IpAddress addr(argv[1], atoi(argv[2]));
      TcpSocket socket;
      socket.connect(addr);
      socket.setNoDelay(true);
      socket.setLinger(0);
      cout << "connected" << endl;
      char sentinel = 0x1F;
      for(int i = 3; i < argc; i++)
      {
         socket.write(&sentinel, 1);
         socket.write(argv[i], strlen(argv[i]));
         socket.write(&sentinel, 1);
         if(i < argc) socket.write(" ", 1);
      }
      socket.write("\n", 1);
      char c;
      while(1)
      {
         socket.read(&c, 1);
         if(c == 0x04)
            break;
         else
            putchar(c);
      }
   }
   catch(TcpSocket::Disconnect&)
   {
      cout << "disconnected" << '\a' << endl;
   }
   catch(NetErr& e)
   {
      cout << "Exception: " << e.errm << endl;
      cout << "terminated" << endl;
      return 1;
   }

   return 0;
}
