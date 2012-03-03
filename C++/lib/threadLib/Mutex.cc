///////////////////////////////////////////////////////////////////////
//
//  File/Class: Mutex
// Description: The Mutual Exclusion (MUTEX) object.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "threadLib/Mutex.h"
#include "threadLib/Thread.h"

// class attributes
UnsafeMutex Mutex::mutex;

// instance methods
UnsafeMutex::UnsafeMutex()
{
   pthread_mutex_init(&id, NULL);
}

UnsafeMutex::~UnsafeMutex()
{
   release();
}

void UnsafeMutex::acquire()
{
   pthread_mutex_lock(&id);
}

void UnsafeMutex::release()
{
   pthread_mutex_unlock(&id);
}

// -- Mutex

Mutex::Mutex()
 :tid(0),
  depth(0)
{
}

inline
bool Mutex::isAcquired() const
{
   bool result = 0;
   mutex.acquire();
   result = (tid == pthread_self());
   mutex.release();
   return result;
}
  
void Mutex::acquire()
{
   pthread_t thisThread = pthread_self();
   mutex.acquire();
   if((depth > 0) && (tid == thisThread))
   {
      depth++;
      mutex.release();
      return;
   }

   UnsafeMutex::acquire();
   depth = 1;
   tid = thisThread;
   mutex.release();
}

void Mutex::release()
{
   mutex.acquire();
   pthread_t thisThread = pthread_self();
   if(tid == thisThread)
   {
      if(depth) depth--;
      if(depth == 0)
      {
         tid = 0;
         UnsafeMutex::release();
      }
   }

   mutex.release();
}
