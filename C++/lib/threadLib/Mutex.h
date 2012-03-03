///////////////////////////////////////////////////////////////////////
//
//  File/Class: Mutex
// Description: The Mutual Exclusion (MUTEX) object.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef CYBERWERX_MUTEX_H
#define CYBERWERX_MUTEX_H

#include <pthread.h>

class UnsafeMutex
{
public:
   UnsafeMutex();
   virtual ~UnsafeMutex();
   virtual void acquire();
   virtual void release();

   operator pthread_mutex_t*();

protected:
   pthread_mutex_t id;
};

// -- Mutex

class Mutex : public UnsafeMutex
{
public:
   Mutex();
   void acquire();
   void release();
   bool isAcquired() const;

protected:
   static UnsafeMutex mutex;
   pthread_t tid;
   int depth;
};

// -- AutoMutex

class AutoMutex
{
public:
   AutoMutex(Mutex& m)
     :mutex(m)
      { mutex.acquire(); }
   ~AutoMutex()
      { mutex.release(); }
private:
   Mutex mutex;
};


// inlines

inline
UnsafeMutex::operator pthread_mutex_t*()
{
   return &id;
}

#endif // CYBERWERX_MUTEX_H
