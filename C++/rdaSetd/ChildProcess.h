///////////////////////////////////////////////////////////////////////
//
//  File/Class: ChildProcess
// Description: Provides Unix child process fork/execvp with piped
//              stdin, stdout, stderr.
//
// Author: Jeff Ortel, 07/01/2000 (Cyberwerx, LLC)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

class ChildProcess
{
public:
   ChildProcess(const char *name);
   ~ChildProcess();

   int execute();
   int bytesToRead(int limit) const;
   bool writeStdin(const char *p, int nbytes);
   bool readStdout(char *p, int nbytes) const;
   bool readStderr(char *p, int nbytes) const;

private:
   char *allocArg(int arg, int size);
   void getargs();
   // attributes
   int argc;
   char *argv[1000];
   const char *app;
   int _stdin;
   int _stdout;
   int _stderr;
};

#endif //CHILDPROCESS_H
