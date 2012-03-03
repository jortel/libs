///////////////////////////////////////////////////////////////////////
//
//  File/Class: ChildProcess
// Description: Provides Unix child process fork/exec with piped
//              Stdin, stdout, stderr.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "ChildProcess.h"
#include <netLib/Unistd.h>
#include <netLib/Syslog.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


ChildProcess::ChildProcess(const char *name)
 :app(name),
  _stdin(-1),
  _stdout(-1),
  _stderr(-1)
{
   syslog(LOG_ERR, "ex:[%s]", name);
   signal(SIGPIPE, SIG_IGN);
   memset(argv, 0, sizeof(argv));
   getargs();
}

ChildProcess::~ChildProcess()
{
   if(_stdin != -1)
      close(_stdin);
   if(_stdout != -1)
      close(_stdout);
   if(_stderr != -1)
      close(_stderr);

   for(int i = 0; i < 1000; i++)
   {
      if(argv[i])
         delete[] argv[i];
      else
         break;
   }
}

char *ChildProcess::allocArg(int arg, int size)
{
   argv[arg] = new char[size];
   if(!argv[arg])
   {
      syslog(LOG_ERR, "New[%d], failed", size);
      return 0;
   }

   memset(argv[arg], 0, size);
   return argv[arg];
}

inline
static void appendArg(char *arg, int& w, const char& c)
{
   if((c > 31) && (c < 127))
   {
      arg[w] = c;
      w++;
   }
}

void ChildProcess::getargs()
{
   int arg = 0, w = 0;
   int argSize = strlen(app)+1;
   enum { Clear,
             WtArg,
             Sentinel,
             SingleQuote,
             DoubleQuote } state = Clear;

   if(!allocArg(0, argSize))
      return;

   for(int i = 0; ((arg < 1000) && (app[i])); i++)
   {
      switch(state)
      {
         case Clear:
            switch(app[i])
            {
               case ' ':
               case '\t':
                  state = WtArg;
                  break;
               case '"':
                  state = DoubleQuote;
                  break;
               case '\'':
                  state = SingleQuote;
                  break;
               case 0x1F:
                  state = Sentinel;
                  break;
               default:
                  appendArg(argv[arg], w, app[i]);
                  break;
            }
            break;
         case WtArg:
            switch(app[i])
            {
               case ' ':
               case '\t':
                  break;
               default:
                  i--;
                  state = Clear;
                  arg++; w = 0;
                  if(!allocArg(arg, argSize))
                     return;
            }
            break;
         case Sentinel:
            switch(app[i])
            {
               case 0x1F:
                  state = Clear;
                  break;
               default:                                                                                             
                  appendArg(argv[arg], w, app[i]);
                  break;
            }
            break;
         case SingleQuote:
            switch(app[i])
            {
               case '\'':
                  state = Clear;
                  break;
               default:
                  appendArg(argv[arg], w, app[i]);
                  break;
            }
            break;
         case DoubleQuote:
            switch(app[i])
            {
               case '"':
                  state = Clear;
                  break;
               default:
                  appendArg(argv[arg], w, app[i]);
                  break;
            }
            break;
      }
   }
}

int ChildProcess::execute()
{
   int pipeIn[2];
   int pipeOut[2];
   static const int ParentEnd = 0;
   static const int ChildEnd = 1;

   if(pipe(pipeIn) == -1)
   {
      syslog(LOG_ERR, "pipe() failed \"%s\"", strerror(errno));
      return -1;
   }
   if(pipe(pipeOut) == -1)
   {
      close(pipeIn[0]);
      close(pipeIn[1]);
      syslog(LOG_ERR, "pipe() failed \"%s\"", strerror(errno));
      return -1;
   }

   pid_t pid = fork();
   switch(pid)
   {
      case -1:
         syslog(LOG_ERR, "fork() failed \"%s\"", strerror(errno));
         return -1;
      case 0:
         close(0);
         Dup(pipeOut[ChildEnd]);
         close(1);
         close(2);
         Dup(pipeIn[ChildEnd]);
         Dup(pipeIn[ChildEnd]);
         close(pipeIn[0]);
         close(pipeIn[1]);
         close(pipeOut[0]);
         close(pipeOut[1]);
         if(execvp(argv[0], argv) == -1)
            syslog(LOG_ERR, "execvp() failed \"%s\"", strerror(errno));
         exit(1);
         break;
      default:
         waitpid(pid, 0, WNOHANG);
         _stdin   = pipeOut[ParentEnd];
         _stdout = pipeIn[ParentEnd];
         _stderr = pipeIn[ParentEnd];
         close(pipeOut[ChildEnd]);
         close(pipeIn[ChildEnd]);
         break;
   }

   return _stdout;
}

int ChildProcess::bytesToRead(int limit) const
{
   int result = 0;
   ioctl(_stdout, FIONREAD, &result);
   return (result > limit ? limit : result);
}

bool ChildProcess::writeStdin(const char *p, int nbytes)
{
   if(_stdin != -1)
      return (write(_stdin, p, nbytes) == nbytes);
   else
      return false;
}

bool ChildProcess::readStdout(char *p, int nbytes) const
{
   if((nbytes) && (_stdout != -1))
      return (read(_stdout, p, nbytes) == nbytes);
   else
      return false;
}

bool ChildProcess::readStderr(char *p, int nbytes) const
{
   if((nbytes) && (_stderr != -1))
      return (read(_stderr, p, nbytes) == nbytes);
   else
      return false;
}
