///////////////////////////////////////////////////////////////////////
//
//  File/Class: ThreadData
// Description: Thread specific data object.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "threadLib/ThreadData.h"

static void tsdDestructor(void *p)
{
   delete (char*)p;
}

UnsafeThreadData::UnsafeThreadData()
 :key(0)
{
   pthread_key_create(&key, tsdDestructor);
}

UnsafeThreadData::~UnsafeThreadData()
{
   pthread_key_delete(key);
}

void *UnsafeThreadData::get() const
{
   return pthread_getspecific(key);
}

void UnsafeThreadData::set(void *value)
{
   pthread_setspecific(key, value);
}
