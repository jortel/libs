#include "threadLib/Thread.h"
#include "threadLib/ThreadErr.h"
#include "threadLib/ThreadRoot.h"
#include "threadLib/ThreadData.h"
#include "threadLib/Mutex.h"
#include "threadLib/QueuedConsumer.h"
#include <stdio.h>
#include <unistd.h>

class TestRoot : public ThreadRoot
{
public:
   void operator()(void*);
};

void TestRoot::operator()(void*)
{
   for(int i = 0; i < 25; i++)
   {
      printf("Thread 0x%x running ....\n", (uint)thread);
      sleep(1);
   }
}

class TestQueuedConsumer : public QueuedConsumer<char>
{
public:
   void onMessageReceived(char *c);
};

void TestQueuedConsumer::onMessageReceived(char *c)
{
   if(c)
      printf("Msg: [%c] rcvd thread=0x%x\n", *c, (uint)thread);
   else
      printf("Msg: [%ld] rcvd thread=0x%x\n", (long)c, (uint)thread);

   delete c;
}

int main(void)
{
   int ecode = 0;
   pthread_mutex_t m;
   ecode = pthread_mutex_init(&m, NULL);
   ecode = pthread_mutex_lock(&m);
   ecode = pthread_mutex_trylock(&m);

   ThreadData<int> td;
   td.set(new int(33));
   printf("thread data %d\n", *td.get());

   printf("Starting thread(s)\n");

   try
   {
      TestRoot root1;
      Thread t1(root1);
      printf("Thread #1 started\n");

      TestRoot root2;
      Thread t2(root2);
      printf("Thread #2 started\n");

      TestQueuedConsumer root3;
      Thread t3(root3);
      printf("Thread #3 started\n");
      sleep(1);

      for(int i = 0; i < 100; i++)
      for(char c = 'a'; c < 'z'; c++)
      {
         printf("Send %c\n", c);
         root3.send(new char(c));
         if(i % 10 == 0)
            Thread::yield();
      }

      sleep(2);
      printf("Send B\n");
      root3.send(new char('B'));

      printf("main waiting ....\n");
      t1.wait();
      t2.wait();
      t3.wait();
   }
   catch(ThreadErr& e)
   {
      printf("Exception: %s\n", e.errm);
   }

   return 0;
}
