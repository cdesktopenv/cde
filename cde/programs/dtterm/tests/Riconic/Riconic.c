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
/* $XConsortium: Riconic.c /main/3 1995/10/31 11:52:08 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

#define BLANK    ' '

char LogStr[200];


void TestIconify(WinName, String, TestNum)
char *WinName, *String;
int TestNum;
{
  char Str[IMAGE_FILE_LEN];
    sprintf(Str, "%sriconic%d", IMAGE_DIR, TestNum);
    MatchWindows(WinName, Str);
    Deiconify(WinName);
    if (WaitWinMap(WinName) < 0) LogError("Deiconify did not work");
    PrintTermString(WinName, String);
    sprintf(Str, "%sriconic%dA", IMAGE_DIR, TestNum);
    MatchWindows(WinName, Str);
    CloseTerm(WinName);
}

void TestIconifyMap(WinName, String, TestNum)
char *WinName, *String;
int TestNum;
{
  char Str[IMAGE_FILE_LEN];
    sprintf(Str, "%sriconic%d", IMAGE_DIR, TestNum);
    MatchWindows(WinName, Str);
    if (TestNum == 3) {
       if (WaitWinMap(WinName) < 0) LogError("mapOnOutput: True did not work");
       PrintTermString(WinName, String);
       sprintf(Str, "%sriconic%dA", IMAGE_DIR, TestNum);
       MatchWindows(WinName, Str);
       PrintTermString(WinName, "1");
       CloseTerm(WinName);
    }
    else {
       if (WaitWinMap(WinName) == 0) LogError("mapOnOutput:False did not work");
       else {
         Deiconify(WinName); 
         if (WaitWinMap(WinName) < 0) LogError("Deiconify did not work");
         CloseTerm(WinName);
       }
    }
}


static char *OptionArr[] = {
/*0*/ " -iconic ",
/*1*/ " -iconic -xrm 'dtterm*iconName: Test' ",
/*2*/ " -iconic -map -xrm 'dtterm*mapOnOutputDelay: 5' -e wait5write",
/*3*/ " -iconic +map -xrm 'dtterm*mapOnOutputDelay: 5' -e wait5write"
};

static char *ResourceArr[] = {
/*0*/ "iconic True",
/*1*/ "iconic True iconName test",
/*2*/ "iconic True mapOnOutput True mapOnOutputDelay 5",
/*3*/ "iconic True mapOnOutput False mapOnOutputDelay 5"
};

static char *LogAction[] = {
"Testing Option  iconic True ",
"Testing Option  iconic True iconName Test",
"Testing Option  iconic True mapOnOutput True mapOnOutputDelay 5",
"Testing Option  iconic True mapOnOutput False mapOnOutputDelay 5"
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

#define ArrCount    (int) (sizeof(OptionArr) / sizeof(char *))

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
     for (i=0; i < ArrCount; i++) {
       CheckCapsLock();
       ExecCommand("xrdb -load xrdb.out");
       MakeResourceFile(ResourceArr[i]);
       CheckCapsLock();
       strcpy(Command, "xrdb -merge res; ");
       strcat(Command, TERM_EMU);
       if (i > 1) strcat(Command, " -e wait5write");
       ExecCommand(Command);
       if (WaitWinMap("TermWin") < 0) continue;
       AssignWinName("TermWin", TERM_EMU);
       if (i <= 1) TestIconify("TermWin", OptionArr[i], i+1);
       else TestIconifyMap("TermWin", OptionArr[i], i+1);
       WaitWinUnMap("TermWin", 60L);
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

