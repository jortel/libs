///////////////////////////////////////////////////////////////////////
//
//  File/Class: NetErr
// Description: The network object exception base class.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef NETERR_H
#define NETERR_H

struct NetErr
{
   NetErr(const char* context,
          const char *fileLocation,
          int lineLocation);

   // attributes
   const char* _context;
   const char* _fileLocation;
   int _lineLocation;
   char errm[1024];
};

#endif //NETERR_H
