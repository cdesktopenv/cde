/* $XConsortium: Rtm.c /main/3 1995/10/31 11:54:45 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];



static char *OptionArr[] = { 
/*0*/ " -tm 'intr ! quit @ erase # kill $ eof % eol ^ swtch & start * stop ( susp ) dsusp _'"                                 /* Normal option */
};


static char *LogAction[] = { 
/*0*/ "Testing Resource Option -tm "             
};

static char *ResourceArr[] = {
"intr ! quit @ erase # kill $ eof % eol ^ swtch & start * stop ( susp ) dsusp _"
};

int MakeResourceFile(ResArr)
char *ResArr;
{
  FILE *ResFile;
  int i, j, Len;
  char Resource[50], Value[50];
    
    if ((ResFile = fopen("res", "w")) == NULL)
       {LogError("Resource File Creation fail"); return(-1);}
    fprintf(ResFile, "%s*%s: %s\n", TERM_EMU, "ttyModes", ResourceArr[0]);
    fclose(ResFile);
}


#define ArrCount    (int) (sizeof(ResourceArr) / sizeof(char *))

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
     for (i=0; i < ArrCount; i++) {
       CheckCapsLock();
       ExecCommand("xrdb -load xrdb.out");
       MakeResourceFile(ResourceArr[i]);
       LogError(LogAction[i]);
       strcpy(Command, "xrdb -merge res; ");
       strcat(Command, TERM_EMU);
       CheckCapsLock();
       ExecCommand(Command); sleep(2);
       if (WaitWinMap("TermWin") < 0) continue;
       ExecCommand("termget"); 
       CloseTerm("TermWin");
       WaitWinUnMap("TermWin", 10L);
sleep(2);
     }
     CheckCapsLock();
     ExecCommand("xrdb -load xrdb.out");
     CloseTest(False);
#ifdef LOG
sprintf(Command, "TestName: <%s> ENDS\n", argv[0]);
LogError(Command);
LogError("****************************************************************************");
fclose(TermLog);
#endif
}

