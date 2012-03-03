///////////////////////////////////////////////////////////////////////
//
//  File/Class: Unistd header
// Description: Provides Unix std defines and ops that log to syslog.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/Unistd.h"
#include "netLib/Syslog.h"
#include <string.h>
#include <errno.h>

int Dup(int fd)
{
   int result = dup(fd);
   if(result == -1)
   {
      syslog(LOG_ERR, "dup(%d) failed, \"%s\"", fd, strerror(errno));
   }
   return result;
}
