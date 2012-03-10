/* $XConsortium: Clogin.c /main/3 1995/10/31 11:48:57 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];



static char *OptionArr[] = { 
/*0*/ " -ls " ,                                /* Normal option */
/*1*/ " -xrm 'dtterm*loginShell: off'  -ls ", 
/*2*/ " -xrm 'dtterm*loginShell: on'  +ls ",
};


static char *LogAction[] = { 
/*0*/ "Testing Option -ls ",             
/*1*/ "Testing Option -xrm 'dtterm*loginShell: off'  -ls ",   
/*2*/ "Testing Option -xrm 'dtterm*loginShell: on'  +ls "
};

#define ArrCount    (int) (sizeof(OptionArr) / sizeof(char *))

main(argc, argv)
int argc;
char *argv[];
{
   SynStatus Result;
   char Command[NEED_LEN], *Shell, *Path;
   int i;
     
#ifdef LOG
if ((TermLog = fopen("term.log", "a")) == NULL) {
   if ((TermLog = fopen("term.log", "w")) == NULL)
      {printf("Logfile could not be opened \n"); exit(-1);}
}
LogError("****************************************************************************");
LogTime();
sprintf(Command, "TestName: <%s> STARTS\n", argv[0]);
LogError(Command);
#endif 
     InitTest(argc, argv);
     CheckCapsLock();
     Path = getenv("PATH");
     ExecCommand("PATH=/clone/vels/cdetest/bin:/usr/bin/X11:/bin:/opt/dt/bin"); 
     for (i=0; i < ArrCount; i++) {
       strcpy(Command, TERM_EMU);  
       strcat(Command, OptionArr[i]);
       CheckCapsLock();
       LogError(LogAction[i]);
       ExecCommand(Command); sleep(2);
       if (WaitWinMap("TermWin") < 0) continue;
       switch (i) {
         case 0: 
           ExecCommand("test-ls"); break;
         case 1: 
           ExecCommand("test-ls"); break;
         case 2: 
           ExecCommand("test+ls"); break;
       }
       CloseTerm("TermWin");
       WaitWinUnMap("TermWin", 10L);
sleep(2);
     }
     strcpy(Command, "PATH="); 
     strcat(Command, Path);
     ExecCommand(Command);
     CloseTest(False);
#ifdef LOG
sprintf(Command, "TestName: <%s> ENDS\n", argv[0]);
LogError(Command);
LogError("****************************************************************************");
fclose(TermLog);
#endif
}

