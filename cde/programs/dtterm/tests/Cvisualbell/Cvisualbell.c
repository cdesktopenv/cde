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
/* $XConsortium: Cvisualbell.c /main/3 1995/10/31 11:50:50 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];


void TestVisualbell(WinName, TestNum)
char *WinName;
int TestNum;
{
  char Str[IMAGE_FILE_LEN];
  int i;
    for (i=0; i < 4; i++) 
      PressCtrlGKey(WinName);
    if (TestNum > 2) 
       PrintTermString(WinName, "You should have heard beep");
    else 
       PrintTermString(WinName, "You should have seen screen flashing");
sleep(5);
    CloseTerm(WinName);
}

static char *OptionArr[] = { 
/*0*/ " -vb ",
/*1*/ " -xrm 'visualBell: False' -vb",
/*2*/ " +vb ",
/*3*/ " -xrm 'visualBell: True' +vb"
};

static char *LogAction[] = { 
/*0*/ " Testing Option: -vb ",
/*1*/ " Testing Option: -xrm 'visualBell: False' -vb",
/*2*/ " Testing Option: +vb ",
/*3*/ " Testing Option: -xrm 'visualBell: True' +vb"
};

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
       strcpy(Command, TERM_EMU);  
       strcat(Command, OptionArr[i]);
       CheckCapsLock();
       LogError(LogAction[i]);
       ExecCommand(Command);
       if (WaitWinMap("TermWin") < 0) continue;
       AssignWinName("TermWin", TERM_EMU);
       TestVisualbell("TermWin", i+1);
       WaitWinUnMap("TermWin", 60L);
     }
     CloseTest(False);
#ifdef LOG
sprintf(Command, "TestName: <%s> ENDS\n", argv[0]);
LogError(Command);
LogError("****************************************************************************");
fclose(TermLog);
#endif
}

