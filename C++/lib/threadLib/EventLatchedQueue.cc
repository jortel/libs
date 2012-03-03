///////////////////////////////////////////////////////////////////////
//
//  File/Class: EventLatchedQueue
// Description: An event latched queue object
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "threadLib/EventLatchedQueue.h"

UnsafeEventLatchedQueue::UnsafeEventLatchedQueue()
 :length(0)
{
   pthread_cond_init(&event, NULL);
   headSentinel.next = &tailSentinel;
   tailSentinel.prev = &headSentinel;
}

UnsafeEventLatchedQueue::~UnsafeEventLatchedQueue()
{
   mutex.acquire();
   while(pop());
   mutex.release();
}

void UnsafeEventLatchedQueue::push(void *data)
{
   mutex.acquire();
   Node *node = new Node;
   node->data = data;
   node->prev = tailSentinel.prev;
   node->next = &tailSentinel;
   tailSentinel.prev->next = node;
   tailSentinel.prev = node;
   length++;
   pthread_cond_broadcast(&event);
   mutex.release();
}

void UnsafeEventLatchedQueue::waitOnEvent()
{
   pthread_cond_wait(&event, mutex);
}

void *UnsafeEventLatchedQueue::pop()
{
   void *result = 0;

   mutex.acquire();
   if(length == 0)
      waitOnEvent();

   Node *node = headSentinel.next;
   if((node) && (node != &tailSentinel))
   {
      result = node->data;
      node->next->prev = &headSentinel;
      headSentinel.next = node->next;
      delete node; node = 0;
   }

   if(length) length--;
   mutex.release();
   return result;
}
