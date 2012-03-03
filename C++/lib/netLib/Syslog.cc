#include "netLib/Syslog.h"
#include <strings.h>
#include <stdio.h>

Syslog::Syslog(const char *ident)
   :pty(LOG_INFO)
{
   memset(buffer, 0, sizeof(buffer));
   openlog(ident, LOG_PID|LOG_NDELAY, LOG_USER);
}

Syslog::~Syslog()
{
   closelog();
}

Syslog& Syslog::operator<<(const char* p)
{
   strcat(buffer, p);
   return *this;
}

Syslog& Syslog::operator<<(char c)
{
   int end = strlen(buffer);
   buffer[end++] = c;
   buffer[end] = 0;
   return *this;
}

Syslog& Syslog::operator<<(short n)
{
   char tmp[30];
   sprintf(tmp, "%d", n);
   strcat(buffer, tmp);
   return *this;
}

Syslog& Syslog::operator<<(int n)
{
   char tmp[30];
   sprintf(tmp, "%d", n);
   strcat(buffer, tmp);
   return *this;
}

Syslog& Syslog::operator<<(long n)
{
   char tmp[30];
   sprintf(tmp, "%ld", n);
   strcat(buffer, tmp);
   return *this;
}

void Syslog::flush(const char* p)
{
   syslog(pty, buffer, 0);
   memset(buffer, 0, sizeof(buffer));
}
