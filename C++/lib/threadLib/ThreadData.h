///////////////////////////////////////////////////////////////////////
//
//  File/Class: ThreadData
// Description: Thread specific data object.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef THREADDATA_H
#define THREADDATA_H

#include <pthread.h>

class UnsafeThreadData
{
protected:
   UnsafeThreadData();
   virtual ~UnsafeThreadData();
   void* get() const;
   void set(void*);
private:
   pthread_key_t key;
};

template <class T>
class ThreadData : public UnsafeThreadData
{
public:
   ThreadData();
   T *get() const;
   void set(T*);
   void set(T&);
};

// inlines

template <class T>
inline
ThreadData<T>::ThreadData()
{
}

template <class T>
inline
T *ThreadData<T>::get() const
{
   return (T*)UnsafeThreadData::get();
}

template <class T>
inline
void ThreadData<T>::set(T *value)
{
   UnsafeThreadData::set(value);
}

template <class T>
inline
void ThreadData<T>::set(T& value)
{
   UnsafeThreadData::set(&value);
}

#endif // THREADDATA_H
