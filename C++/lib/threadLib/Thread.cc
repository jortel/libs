///////////////////////////////////////////////////////////////////////
//
//  File/Class: Thread
// Description: The POSIX thread object.
//
// Author: Jeff Ortel, 04/05/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "threadLib/Thread.h"
#include "threadLib/ThreadRoot.h"
#include "threadLib/ThreadErr.h"
#include <sched.h>

// class attributes
Mutex Thread::listMutex;
Thread *Thread::list[MaxThread];

// class methods
Thread *Thread::getInstance()
{
   Thread *result = 0;
   pthread_t tid = pthread_self();
   listMutex.acquire();
   if(tid < MaxThread)
      result = list[tid];

   listMutex.release();
   return result;
}

void Thread::yield()
{
   sched_yield();
}

void *Thread::threadMain(void *arg)
{
   if(arg)
   {
      Thread *threadObject = (Thread*)arg;
      pthread_t tid = pthread_self();
      if(tid < MaxThread)
      {
         listMutex.acquire();
         list[tid] = threadObject;
         listMutex.release();
      }

      threadObject->start();
   }

   return arg;
}

Thread::Thread(ThreadRoot& root, Type type)
 :tid(0),
  threadRoot(root),
  type(type)
{
   int errn = 0;

   // set the thread root
   if(!threadRoot.setThread(this))
      throw ThreadErr(0, "setroot", __FILE__, __LINE__);

   // set attributes
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   if(type == Bound)
   {
      errn = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
      if(errn)
         throw ThreadErr(errn, "setscope", __FILE__, __LINE__);
   }

   // create the thread
   errn = pthread_create(&tid, &attr, threadMain, this);
   if(errn)
      throw ThreadErr(errn, "create", __FILE__, __LINE__);
}

Thread::~Thread()
{
   threadRoot.setThread(0);
   pthread_cancel(tid);
}

void Thread::start()
{
   threadRoot(this);
}

void *Thread::wait() const
{
   void *status = 0;
   int errn = pthread_join(tid, &status);
   if(errn)
      throw ThreadErr(errn, "join", __FILE__, __LINE__);

   return status;
}
