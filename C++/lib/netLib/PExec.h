///////////////////////////////////////////////////////////////////////
//
//  File/Class: PExec
// Description: Provides Unix child process fork/execvp with piped
//              stdin, stdout, stderr.
//
// Author: Jeff Ortel, 04/08/2002 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef PEXEC_H
#define PEXEC_H

#include <sys/types.h>

class PExec
{
public:
   // constructor
   PExec(int argc, char **argv);
   ~PExec();

   // methods
   int execute();
   int getStatus();
   int getStdin() const;
   int getStdout() const;

private:
   // attributes
   pid_t pid;
   int status;
   int argc;
   char **argv;
   int input;
   int output;
};

// inlines

inline
int PExec::getStdin() const
{
   return input;
}

inline
int PExec::getStdout() const
{
   return output;
}

#endif //PEXEC_H
