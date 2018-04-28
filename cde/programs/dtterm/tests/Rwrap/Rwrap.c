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
/* $XConsortium: Rwrap.c /main/3 1995/10/31 11:55:31 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];


#define BLANK    ' '

void TestWrap(WinName, String, TestNum)
char *WinName, *String;
int TestNum;
{
  char Str[IMAGE_FILE_LEN];
  int i;
     PressKeyNtimes(WinName, "C", 100);
     for (i=0; i < 75; i++) 
       PressBackSpace(WinName);
     sprintf(Str, "%srwrap%d", IMAGE_DIR, TestNum);
     MatchWindows(WinName, Str);
     CloseTerm(WinName);
}

static char *ResourceArr[] = {
/*0*/ "autoWrap True reverseWrap True ",
/*1*/ "autoWrap False reverseWrap False ",
/*2*/ "autoWrap True reverseWrap False "
};

static char *LogAction[] = {
"Testing Resource Option: autoWrap True reverseWrap True ",
"Testing Resource Option:autoWrap False reverseWrap False ",
"Testing Resource Option:autoWrap True reverseWrap False "
};

#define ArrCount    (int) (sizeof(ResourceArr) / sizeof(char *))

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
fprintf(TermLog, "**************************************************\n");
LogTime();
fprintf(TermLog, "TestName: <%s> STARTS\n", argv[0]);
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
       ExecCommand(Command);
       if (WaitWinMap("TermWin") < 0) continue;
       AssignWinName("TermWin", TERM_EMU);
       TestWrap("TermWin", "Checking Pointer Resources ", i+1);
       WaitWinUnMap("TermWin", 60L);
     }
     CheckCapsLock();
     ExecCommand("xrdb -load xrdb.out");
     CloseTest(False);
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

