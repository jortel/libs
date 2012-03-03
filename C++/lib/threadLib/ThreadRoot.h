///////////////////////////////////////////////////////////////////////
//
//  File/Class: ThreadRoot
// Description: The Thread root object.
//
// Author: Jeff Ortel, 04/05/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef THREADROOT_H
#define THREADROOT_H

class Thread;

class ThreadRoot
{
public:
   ThreadRoot();
   virtual ~ThreadRoot();
   virtual void operator()(void*);

   int setThread(Thread*);
   const Thread *getThread() const;

protected:
   Thread *thread;
};

inline
int ThreadRoot::setThread(Thread *value)
{
   int result = 0;
   if(thread == 0)
   {
      thread = value;
      result = 1;
   }

   return result;
}

inline
const Thread *ThreadRoot::getThread() const
{
   return thread;
}

#endif // THREADROOT_H
