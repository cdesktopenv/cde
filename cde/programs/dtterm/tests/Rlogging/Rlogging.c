/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: Rlogging.c /main/3 1995/10/31 11:52:32 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];

static char *LogAction[] = { 
/*0*/ "Testing Resourse Option:logging True",
/*1*/ "Testing Resourse Option:logging True but in unwritable directory",
/*2*/ "Testing Resourse Option:logging True logFile Term.Log",
/*3*/ "Testing Resourse Option:logging False logFile Term.Log",
};


static char *ResourceArr[] = {
/*0*/ "logging True",
/*1*/ "logging True ",
/*2*/ "logging True logFile Term.Log",
/*3*/ "logging False logFile Term.Log",
};

int MakeResourceFile(ResArr)
char *ResArr;
{
  FILE *ResFile;
  int i, j, Len;
  char Resource[50], Value[50];
    
    if ((ResFile = fopen("res", "w")) == NULL)
       {LogError("Resource File Creation fail"); return(-1);}
    for (i=0; i < 50; i++) Resource[i] = BLANK;
    for (i=0; i < 50; i++) Value[i] = BLANK;
    Len = strlen(ResArr); i=0;
    while (i < Len) {
      j = 0;
      while (ResArr[i] != BLANK) Resource[j++] = ResArr[i++];
      Resource[j] = NULLCHAR;
      i++; j = 0;
      while ((ResArr[i] != BLANK) && (ResArr[i] != NULLCHAR))
            Value[j++] = ResArr[i++];
      Value[j] = NULLCHAR;
      i++;
      fprintf(ResFile, "%s*%s: %s\n", TERM_EMU, Resource, Value);
    }
    fclose(ResFile);
}


#define ArrCount    (int) (sizeof(ResourceArr) / sizeof(char *))

main(argc, argv)
int argc;
char *argv[];
{
   SynStatus Result;
   char Command[NEED_LEN];
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
     ExecCommand("mkdir tmplog");
     for (i=0; i < ArrCount; i++) {
       CheckCapsLock();
       ExecCommand("xrdb -load xrdb.out");
       MakeResourceFile(ResourceArr[i]);
       CheckCapsLock();
       ExecCommand("xrdb -merge res"); sleep(5);
       CheckCapsLock();
       ExecCommand("mv  DTtermLog* tmplog"); sleep(1);
       ExecCommand("mv Term.Log tmplog"); sleep(1);
       if (i == 1) { /* try in un writable directory */
          ExecCommand("mkdir unwrite"); sleep(1);
          ExecCommand("chmod 555 unwrite"); sleep(1);
          ExecCommand("cd  unwrite"); sleep(1);
       }
       LogError(LogAction[i]);
       strcpy(Command, TERM_EMU);
       CheckCapsLock();
       ExecCommand(Command); sleep(1);
       if (WaitWinMap("TermWin") < 0) continue;
       AssignWinName("TermWin", TERM_EMU);
       JustExit("TermWin");
       WaitWinUnMap("TermWin", 10L);
       switch (i) {
         case 0: 
           ExecCommand("test-l"); break;
         case 1: 
           ExecCommand("test+l"); break;
         case 2: 
           ExecCommand("test-lf"); break;
         case 3: ExecCommand("test+l"); break;
       }
       if (i == 1) {
          ExecCommand("cd ..");
          ExecCommand("rmdir unwrite");
       }
sleep(2);
     }
     ExecCommand("mv  tmplog/DTtermLog* ."); sleep(1);
     ExecCommand("mv tmplog/Term.Log ."); sleep(1);
     ExecCommand("rmdir tmplog");
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

