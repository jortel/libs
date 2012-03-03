///////////////////////////////////////////////////////////////////////
//
//  File/Class: Lock
// Description: Provides semiphore based locking (MUTEX)
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include <threadLib/Lock.h>
#include <sys/ipc.h>
#include <sys/sem.h>

Lock::Lock(key_t key)
   :semid(-1)
{
   semid = semget(key, 1, 0);
   if(semid == -1)
   {
      semid = semget(key, 1, (IPC_CREAT|0666));
   }
}

Lock::~Lock()
{
}

bool Lock::acquire()
{
   static sembuf sb[2] =
   { { 0, 0, 0 },
     { 0, 1, SEM_UNDO } };

   return (semop(semid, &sb[0], 2) != -1);
}

bool Lock::release()
{
   static sembuf sb[1] =
   { { 0, -1, (SEM_UNDO|IPC_NOWAIT) } };

   return (semop(semid, &sb[0], 1) != -1);
}



AutoLock::AutoLock(Lock *aLock)
 :locked(0),
  lock(aLock)
{
   if(lock)
      locked = lock->acquire();
}

AutoLock::~AutoLock()
{
   if(locked)
   {
      lock->release();
      locked = false;
   }
}
