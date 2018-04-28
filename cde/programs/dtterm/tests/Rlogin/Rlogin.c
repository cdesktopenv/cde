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
/* $XConsortium: Rlogin.c /main/3 1995/10/31 11:52:58 rswiston $ */
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
/*0*/ "Testing Resource Option: loginShell True",
/*1*/ "Testing Resource Option: loginShell False",
/*2*/ "Testing Resource Option: loginShell on",
/*3*/ "Testing Resource Option: loginShell off",
};


static char *ResourceArr[] = {
/*0*/ "loginShell True",
/*1*/ "loginShell False",
/*2*/ "loginShell on",
/*2*/ "loginShell off",
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
   char Command[NEED_LEN], *Shell, *Path, *CurDir;
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
     CurDir = getenv("PWD");
     ExecCommand("PATH=.:../bin:../util:/usr/bin/X11:/bin:/opt/dt/bin"); 
     for (i=0; i < ArrCount; i++) {
       CheckCapsLock();
       ExecCommand("xrdb -load xrdb.out");
       MakeResourceFile(ResourceArr[i]);
       CheckCapsLock();
       LogError(LogAction[i]);
       strcpy(Command, "xrdb -merge res; ");
       strcat(Command, TERM_EMU);
       ExecCommand(Command); sleep(2);
       if (WaitWinMap("TermWin") < 0) continue;
       strcpy(Command, CurDir); strcat(Command, "/../util/");
       switch (i) {
         case 0: 
           strcat(Command, "test-ls");
           ExecCommand(Command); break;
         case 1: 
           strcat(Command, "test+ls");
           ExecCommand(Command); break;
         case 2: 
           strcat(Command, "test-ls");
           ExecCommand(Command); break;
         case 3: 
           strcat(Command, "test+ls");
           ExecCommand(Command); break;
       }
       CloseTerm("TermWin");
       WaitWinUnMap("TermWin", 10L);
sleep(2);
     }
     strcpy(Command, "PATH="); 
     strcat(Command, Path);
     ExecCommand(Command);
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

