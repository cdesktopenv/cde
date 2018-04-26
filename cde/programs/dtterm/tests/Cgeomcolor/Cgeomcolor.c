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
/* $XConsortium: Cgeomcolor.c /main/3 1995/10/31 11:47:36 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];


void TestGeomFgBg(WinName, String, TestNum)
char *WinName, *String;
int TestNum;
{
  char Str[IMAGE_FILE_LEN];
     PrintTermString(WinName, String);
     sprintf(Str, "%sgeomcolor%d", IMAGE_DIR, TestNum);
     MatchWindows(WinName, Str);
     CloseTerm(WinName);
}
static char *OptionArr[] = { 
/*0*/ " -geometry 40x20 -fg green -bg red ", 
/*1*/ " -geometry 60x15 -fg red  -bg green ",
/*2*/ " -geometry 80x10 -fg green -bg yellow ",
/*3*/ " -geometry 80x40 -foreground blue -background cyan ",
/*4*/ " -geometry 20x40 -foreground white -background black ",
/*5*/ " -rv",
/*6*/ " -reverse",
/*7*/ " +rv",
/*8*/ " -bs ",
/*9*/ " +bs ",
/*10*/ " -ms red -cr blue", 
/*11*/ " -ms blue -cr green"
};

static char *LogAction[] = { 
/*0*/ "Testing Option -geometry 40x20 -fg green -bg red ", 
/*1*/ "Testing Option -geometry 60x15 -fg red  -bg green ",
/*2*/ "Testing Option -geometry 80x10 -fg green -bg yellow ",
/*3*/ "Testing Option -geometry 80x40 -foreground blue -background cyan ",
/*4*/ "Testing Option -geometry 20x40 -foreground white -background black ",
/*5*/ "Testing Option -rv",
/*6*/ "Testing Option -reverse",
/*7*/ "Testing Option +rv",
/*8*/ "Testing Option -bs ",
/*9*/ "Testing Option +bs ",
/*10*/ "Testing Option -ms red -cr blue", 
/*11*/ "Testing Option -ms blue -cr green"
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
       TestGeomFgBg("TermWin", OptionArr[i], i+1);
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

