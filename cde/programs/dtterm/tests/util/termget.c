/* $XConsortium: termget.c /main/3 1995/10/31 12:03:12 rswiston $ */
#include <stdio.h>
#include <fcntl.h>
#include <sys/termios.h>

FILE *Ptr;

LogError(Str)
char *Str;
{
    fprintf(Ptr, "%s \n", Str);
}

main()
{
  int FilePtr, i, Error=0;
  struct termios termio_orig;

    Ptr = fopen("term.log", "a");
    if ((FilePtr = open("/dev/tty", O_RDWR)) < 0) 
       {LogError("Could not open tty "); exit(-1);}
    if(tcgetattr(FilePtr, &termio_orig) < 0) 
       {LogError("tcgetattr failed "); exit(-1);}
    if (termio_orig.c_cc[VINTR] != '!') 
       {LogError("intr not set"); Error = 1;}
    if (termio_orig.c_cc[VQUIT] != '@') 
       {LogError("quit not set"); Error = 1;}
    if (termio_orig.c_cc[VERASE] != '#') 
       {LogError("erase not set"); Error = 1;}
    if (termio_orig.c_cc[VKILL] != '$') 
       {LogError("kill not set"); Error = 1;}
    if (termio_orig.c_cc[VEOF] != '%') 
       {LogError("eof not set"); Error = 1;}
    if (termio_orig.c_cc[VEOL] != '^') 
       {LogError("eol not set"); Error = 1;}
    if (termio_orig.c_cc[VSWTCH] != '&') 
       {LogError("swtch not set"); Error = 1;}
    if (termio_orig.c_cc[VSTART] != '*') 
       {LogError("start not set"); Error = 1;}
    if (termio_orig.c_cc[VSTOP] != '(') 
       {LogError("stop not set"); Error = 1;}
    if (termio_orig.c_cc[VSUSP] != ')') 
       {LogError("susp not set"); Error = 1;}
    if (Error == 1) LogError("Test Failed");
    else LogError("Test Passed");
    fclose(Ptr);
}

