///////////////////////////////////////////////////////////////////////
//
//  File/Class: EmWorkshopCrypt
// Description: The Remote EM Workshop server.
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "EmWorkshopCrypt.h"
#include <strings.h>
#include <stdlib.h>
#include <iostream.h>

static const int Seed = 94593;

EmWorkshopCrypt::EmWorkshopCrypt()
{
   int rnum = Seed;
   memset(map, 32, sizeof(map));
   for(int i = 45; i <= 122; i++) map[i] = i;
   for(int i = 45; i <= 122; i++)
   {
      rnum = random(rnum);
      int index = (abs(rnum % 78) + 45);
      char tmp= map[i];
      map[i] = map[index];
      map[index] = tmp;
   }
}

void EmWorkshopCrypt::encode(char *pwd) const
{
   for(int i = 0; pwd[i]; i++)
   {
      pwd[i] = (char)map[pwd[i]];
   }
}

void EmWorkshopCrypt::decode(char *pwd) const
{
   for(int i = 0; pwd[i]; i++)
   {
      pwd[i] = find(pwd[i]);
   }
}

char EmWorkshopCrypt::find(char c) const
{
   for(int i = 45; i <= 122; i++)
   {
      if(map[i] == c) return i;
   }

   return c;
}

int EmWorkshopCrypt::random(int seed) const
{
   return (seed * 9301 + 49297) % 233280;
}
