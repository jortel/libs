#ifndef SOCKETEVENTMANAGER_H
#define SOCKETEVENTMANAGER_H

class Socket;

class SocketEventManager
{
public:
   SocketEventManager(int numSocket);
   ~SocketEventManager();

   enum Event
   {
      ReadyToRead  = 0x01,
      ReadyToWrite = 0x02,
      Connect      = 0x04,
      Disconnect   = 0x08,
      All          = 0xFF
   };

   void setDelegate(SocketEventHandler*);

   void bind(Socket*, const Event&);
   void unbind(Socket*, const Event&);

private:
   struct EventArray
   {
      EventArray()
         :socket(0), eventMask(0){};
      Socket *socket;
      int eventMask;
   };
      
   EventArray *eventArray;
   SocketEventHandler *delegate;
};

#endif //SOCKETEVENTMANAGER_H
