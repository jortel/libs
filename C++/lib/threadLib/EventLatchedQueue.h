///////////////////////////////////////////////////////////////////////
//
//  File/Class: EventLatchedQueue
// Description: An event latched queue object
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTLATCHEDQUEUE_H
#define EVENTLATCHEDQUEUE_H

#include "threadLib/Mutex.h"
#include <pthread.h>

class UnsafeEventLatchedQueue
{
protected:
   UnsafeEventLatchedQueue();
   virtual ~UnsafeEventLatchedQueue();

   void push(void*);
   void *pop();

private:
   void waitOnEvent();

   struct Node {
      Node()
        :next(0), prev(0), data(0)
        {};
      Node *next;
      Node *prev;
      void *data;
   };

   pthread_cond_t event;
   Node headSentinel;
   Node tailSentinel;
   UnsafeMutex mutex;
   int length;
};

template <class T>
class EventLatchedQueue : public UnsafeEventLatchedQueue
{
public:
   EventLatchedQueue();
   ~EventLatchedQueue();

   void push(T*);
   T *pop();
};

template <class T>
inline
EventLatchedQueue<T>::EventLatchedQueue()
{
}

template <class T>
inline
EventLatchedQueue<T>::~EventLatchedQueue()
{
}

template <class T>
inline
void EventLatchedQueue<T>::push(T *t)
{
   UnsafeEventLatchedQueue::push(t);
}

template <class T>
inline
T *EventLatchedQueue<T>::pop()
{
   return (T*)UnsafeEventLatchedQueue::pop();
}

#endif //EVENTLATCHEDQUEUE_H
