#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
   pid_t pid = fork();
   if(pid == 0)
   {
      sleep(2);
      fprintf(stdout, "\ndaSet:started\n");
      for(int i = 0; i < argc; i++)
      {
         printf("[%s] ", argv[i]);
         fflush(stdout);
      }
      fprintf(stderr, "\ndaSet:finished\n");
   }
   else
      exit(0);

   return 0;
}
