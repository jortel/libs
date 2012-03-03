///////////////////////////////////////////////////////////////////////
//
//  File/Class: EmWorkshopSrv
// Description: The Remote EM Workshop server.
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef EMWORKSHOPSRV_H
#define EMWORKSHOPSRV_H

#include <netLib/TcpSocket.h>
#include <stdio.h>

class EmWorkshopSrv
{
public:
   // constructor
   EmWorkshopSrv(const char *arg0, bool needLogin);
   ~EmWorkshopSrv();

   // server methods
   void run();

   // command methods
   void setmode();
   void help();
   void read();
   void write();
   void pwd();
   void cd();
   void ctCmd();
   void binCmd();
   void exit();
   void openLog();

private:
   // private methods
   void traceLog(const char*, const char*);
   void traceLog(const char*, const char*, int);
   void traceTime();
   void initArgs();
   void login();
   void detectGuestLogin();
   void detectRootLogin();
   void validatePasswd();
   void setUserId();
   void split();
   int getLine();
   void exec();
   void exec(char *path);
   void prepend(char*);
   void insert(unsigned int, char*);
   void send(int, const char*);
   void setview(const char*);
   void setupEnvironment(const char*);
   void logArgs();

   enum Mode {
      Normal,
      Echo
   };

   struct CmdDef {
      char *tag;
      void(EmWorkshopSrv::*fn)();
      const char *usage;
   };

   typedef CmdDef* Role;

   // constants
   static CmdDef guest[];
   static CmdDef authenticated[];
   static const int MaxArg = 100;

   // attributes
   Mode mode;          // session mode
   Role role;          // current user role;
   TcpSocket socket;   // socket
   const char *arg0;   // program argv[0] 
   bool needLogin;     // need login flag
   char input[10240];  // generic input buffer
   int argc;           // command argc
   char *argv[MaxArg]; // command argv
   FILE *ssnLog;       // session log
};

inline
void EmWorkshopSrv::prepend(char* arg)
{
   insert(0, arg);
}

#endif // EMWORKSHOPSRV_H
