///////////////////////////////////////////////////////////////////////
//
//  File/Class: NetErr
// Description: The network object exception base class.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/NetErr.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

NetErr::NetErr(const char* context,
               const char *fileLocation,
               int lineLocation)
 :_context(context),
  _fileLocation(fileLocation),
  _lineLocation(lineLocation)
{
   sprintf(errm,
           "%s:%d %s failed %s%s",
            _fileLocation,
            _lineLocation,
            _context,
            (errno ? ", " : ""),
            (errno ? strerror(errno) : ""));
}
