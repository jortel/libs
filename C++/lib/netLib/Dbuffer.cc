///////////////////////////////////////////////////////////////////////
//
//  File/Class: Dbuffer
// Description: Provides a dynamic buffer.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/Dbuffer.h"
#include "netLib/Syslog.h"
#include <string.h>
#include <errno.h>

Dbuffer::Dbuffer()
 :size(0),
  len(0),
  content(0)
{
}

Dbuffer::~Dbuffer()
{
   clear();
}

void Dbuffer::clear()
{
   len = 0;
   size = 0;
   delete[] content;
   content = 0;
}

char *Dbuffer::reallocate()
{
   size += 1024;
   char *tmp = new char[size];
   if(tmp)
   {
      memcpy(tmp, content, len);
      delete[] content;
      content = tmp;
      return content;
   }
   size = 0; len = 0;
   syslog(LOG_ERR, "New %d failed, %s", size, strerror(errno));
   return 0;
}

char *Dbuffer::reallocate(long n)
{
   char *result = content;
   while(size < n)
   {
      result = reallocate();
   }

   return result;
}

void Dbuffer::append(const char *p, int s)
{
   if((p == content) || (reallocate(size+s) == 0))
   {
      return;
   }

   memcpy(&content[len], p, s);
   len += s;
}

void Dbuffer::prepend(const char *p, int s)
{
   if((p == content) || (reallocate(size+s) == 0))
   {
      return;
   }

   char *tmp = new char[size];
   if(tmp)
   {
      memcpy(tmp, content, len);
      memcpy(content, p, s);
      memcpy(&content[s], tmp, len);
      delete[] tmp;
      len += s;
   }
   else
   {
      syslog(LOG_ERR, "New %d failed, %s",
      size, strerror(errno));
   }
}

char Dbuffer::end() const
{
   if((content) && (len))
   {
      return content[len-1];
   }
   else
      return 0;
}

int Dbuffer::length() const
{
   return len;
}

Dbuffer::operator char*()
{
   char *result = "";

   if(reallocate(len))
   {
      content[len] = '\0';
      result = content;
   }

   return result;
}
