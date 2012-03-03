///////////////////////////////////////////////////////////////////////
//
//  File/Class: QueuedConsumer
// Description: An event latched ThreadRoot object that reads and
//              process messages from a message queue.
//
// Author: Jeff Ortel, 04/03/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef QUEUEDCONSUMER_H
#define QUEUEDCONSUMER_H

#include "threadLib/ThreadRoot.h"
#include "threadLib/EventLatchedQueue.h"

template <class T>
class QueuedConsumer : public ThreadRoot
{
public:
   QueuedConsumer();
   ~QueuedConsumer();
   void operator()(void*);

   void send(T*);
   void send(T&);
   virtual void onMessageReceived(T*) = 0;

private:
   EventLatchedQueue<T> messageQueue;
};


// inlines

template <class T>
inline
QueuedConsumer<T>::QueuedConsumer()
{
}

template <class T>
inline
QueuedConsumer<T>::~QueuedConsumer()
{
}

template <class T>
inline
void QueuedConsumer<T>::operator()(void*)
{
   while(1)
   {
      onMessageReceived(messageQueue.pop());
   }
}

template <class T>
inline
void QueuedConsumer<T>::send(T *msg)
{
   messageQueue.push(msg);
}

template <class T>
inline
void QueuedConsumer<T>::send(T& msg)
{
   messageQueue.push(&msg);
}

#endif //QUEUEDCONSUMER_H
