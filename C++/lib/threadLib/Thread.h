///////////////////////////////////////////////////////////////////////
//
//  File/Class: Thread
// Description: The POSIX thread object
//
// Author: Jeff Ortel, 04/05/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef CYBERWERX_THREAD_H
#define CYBERWERX_THREAD_H

#include "threadLib/Mutex.h"
#include <pthread.h>

class ThreadRoot;

class Thread
{
public:
   // types
   enum Type
   {
      Bound,  // system (bound to LWP)
      Unbound // process (unbound to LWP)
   };

   // class methods
   static Thread *getInstance();
   static void yield();

   Thread(ThreadRoot&, Type = Bound);
   virtual ~Thread();

   // operations
   void *wait() const;
   Type getType() const;

   // operators
   operator pthread_t*();

protected:
   virtual void start();

private:
   // class methods
   static void *threadMain(void*);

   static const int MaxThread = 1000;

   // class attributes
   static Thread *list[MaxThread]; // extent list
   static Mutex listMutex;         // extent list mutex

   // attributes
   pthread_t tid;          // thread ID
   ThreadRoot& threadRoot; // thread root object
   Type type;              // thread type
};


inline
Thread::operator pthread_t*()
{
   return &tid;
}

inline
Thread::Type Thread::getType() const
{
   return type;
}

#endif // CYBERWERX_THREAD_H
