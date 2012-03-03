///////////////////////////////////////////////////////////////////////
//
//  File/Class: ThreadErr
// Description: The Thread lib exception base class.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "threadLib/ThreadErr.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

ThreadErr::ThreadErr(int errn,
                     const char* context,
                     const char *fileLocation,
                     int lineLocation)
 :errn(errn),
  context(context),
  fileLocation(fileLocation),
  lineLocation(lineLocation)
{
   sprintf(errm,
           "%s:%d %s failed %s%s",
            fileLocation,
            lineLocation,
            context,
            (errn ? ", " : ""),
            (errn ? strerror(errn) : ""));
}
