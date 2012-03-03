///////////////////////////////////////////////////////////////////////
//
//  File/Class: ThreadErr
// Description: The Thread lib exception base class.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef THREADERR_H
#define THREADERR_H

struct ThreadErr
{
   ThreadErr(int errn,
             const char* context,
             const char *fileLocation,
             int lineLocation);

   // attributes
   int errn;
   const char* context;
   const char* fileLocation;
   int lineLocation;
   char errm[1024];
};

#endif //THREADERR_H
