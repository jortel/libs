///////////////////////////////////////////////////////////////////////
//
//  File/Class: EmWorkshopCrypt
// Description: The Remote EM Workshop server.
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef EMWORKSHOPCRYPT_H
#define EMWORKSHOPCRYPT_H

class EmWorkshopCrypt
{
public:
   EmWorkshopCrypt();
   void encode(char *pwd) const;
   void decode(char *pwd) const;

private:
   char find(char c) const;
   int random(int seed) const;

   // attributes
   char map[128];
};

#endif // EMWORKSHOPCRYPT_H
