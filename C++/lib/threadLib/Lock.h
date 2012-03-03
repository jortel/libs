///////////////////////////////////////////////////////////////////////
//
//  File/Class: Lock
// Description: Provides semiphore based locking (MUTEX)
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef LOCK_H
#define LOCK_H

#include <sys/types.h>

class Lock
{
public:
   Lock(key_t key);
   ~Lock();
   bool acquire();
   bool release();

protected:
   int semid;
};



class AutoLock
{
public:
   AutoLock(Lock*);
   ~AutoLock();
   bool isLocked() const;

private:
   bool locked;
   Lock *lock;
};

inline
bool AutoLock::isLocked() const
{
   return locked;
}

#endif //LOCK_H
