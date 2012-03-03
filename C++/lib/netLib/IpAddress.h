///////////////////////////////////////////////////////////////////////
//
//  File/Class: IpAddress
// Description: Provides an IP address that performs DNS host name lookup
//              and other address and port translations.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>

class sockaddr;

class IpAddress
{
public:
   IpAddress();
   IpAddress(const char*);
   IpAddress(const char*, int);
   IpAddress(const sockaddr_in&);
   operator sockaddr*() const;
   operator sockaddr&() const;
   IpAddress& operator=(const sockaddr_in&);
   int getLength() const;
private:
   void init(const char*, int);
   // attributes
   sockaddr_in addr;
};

inline
IpAddress::operator sockaddr*() const
{
   return (sockaddr*)&addr;
}

inline
IpAddress::operator sockaddr&() const
{
   return (sockaddr&)addr;
}

inline
IpAddress& IpAddress::operator=(const sockaddr_in& rhs)
{
   addr = rhs;
   return *this;
}

inline
int IpAddress::getLength() const
{
   return sizeof(addr);
}

#endif //IPADDRESS_H
