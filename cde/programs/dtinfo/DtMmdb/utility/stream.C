// $XConsortium: stream.C /main/4 1996/08/21 15:55:05 drk $

#include <stdio.h>
#include <stdarg.h>

#include "utility/c_stream.h"

char*  form(const char* fmt ...)
{
   static char buf[1024];

   va_list args;

   va_start(args, fmt);

   (void) vsprintf(buf, fmt, args);
               
   va_end(args);

   return buf;
}

