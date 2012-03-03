///////////////////////////////////////////////////////////////////////
//
//  File/Class: DBuffer
// Description: Provides a dynamic buffer.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef _DBUFFER_H
#define _DBUFFER_H

class Dbuffer
{
public:
   Dbuffer();
   ~Dbuffer();
   void clear();
   void append(const char *p, int s);
   void prepend(const char *p, int s);
   char end() const;
   int length() const;
   operator char*();

private:
   char *reallocate();
   char *reallocate(long);
   // attrubutes
   int size;
   int len;
   char *content;
};

#endif //_DBUFFER_H
