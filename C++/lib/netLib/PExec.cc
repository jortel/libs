///////////////////////////////////////////////////////////////////////
//
//  File/Class: PExec
// Description: Provides Unix child process fork/exec with piped
//              Stdin, stdout, stderr.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "netLib/PExec.h"
#include "netLib/Unistd.h"
#include "netLib/Syslog.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>


PExec::PExec(int argc, char **argv)
 :pid(-1),
  status(1),
  argc(argc),
  argv(argv),
  input(-1),
  output(-1)
{
   argv[argc] = 0;
   signal(SIGPIPE, SIG_IGN);
}

PExec::~PExec()
{
   getStatus();
   if(input != -1)
      close(input);
   if(output != -1)
      close(output);
}

int PExec::execute()
{
   int pipeIn[2];
   int pipeOut[2];
   static const int ParentEnd = 0;
   static const int ChildEnd = 1;

   if(argc < 2)
   {
      syslog(LOG_ERR, "insufficient args (<2)");
      return -1;
   }

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

   pid = fork();
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
         if(execvp(argv[0], &argv[1]) == -1)
         {
            char *errm = strerror(errno);
            fprintf(stderr, "%s\n", errm);
            syslog(LOG_ERR, "execv() failed \"%s\"", errm);
         }
         exit(1);
         break;
      default:
         input = pipeOut[ParentEnd];
         output = pipeIn[ParentEnd];
         close(pipeOut[ChildEnd]);
         close(pipeIn[ChildEnd]);
         break;
   }

   return output;
}

int PExec::getStatus()
{
   if(pid != -1)
   {
      waitpid(pid, &status, WNOHANG);
      pid = -1;
   }

   return status;
}
