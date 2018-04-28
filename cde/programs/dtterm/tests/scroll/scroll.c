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
/* $XConsortium: scroll.c /main/3 1995/10/31 11:58:41 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];

void TestScrolling(WinName)
char *WinName;
{

  int i, NumLines, NumCols;
  char Str[200];

    START(1,0,0,0,0);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); DONE(); return;} 
    GetWinSize(&NumLines, &NumCols);
#ifdef DEBUG
sprintf(LogStr, "WINDOW Size Cols: %d Lines: %d \n", NumCols, NumLines);
LogError(LogStr);
#endif
    ClearScreen();  
    SetScrollType(DECModeSet);
    SetScrollType(DECModeSave);
    for (i=0; i < (20*NumLines); i++) {
      sprintf(Str, "%s%d", "It's in smooth Scroll Mode. is it right", i);
      WRITETEST(Str);
      NextLine();
    }
    ClearScreen();
sleep(2);
    SetScrollType(DECModeReset);
    for (i=0; i < (20*NumLines); i++) {
      sprintf(Str, "%s%d", "It's in Jump Scroll Mode. is it right", i);
      WRITETEST(Str);
      NextLine();
    }
    SetScrollType(DECModeRestore);
    ClearScreen();
sleep(2);
    for (i=0; i < (20*NumLines); i++) {
      sprintf(Str, "%s%d", "It's in smooth Scroll Mode. is it right", i);
      WRITETEST(Str);
      NextLine();
    }
    DONE();
}

main(argc, argv)
int argc;
char *argv[];
{
     
#ifdef LOG
if ((TermLog = fopen("term.log", "a")) == NULL) {
   if ((TermLog = fopen("term.log", "w")) == NULL)
      {printf("Logfile could not be opened \n"); exit(-1);}
}
fprintf(TermLog, "**************************************************\n");
LogTime();
fprintf(TermLog, "TestName: <%s> STARTS\n", argv[0]);
#endif 
#ifdef SYNLIB
     InitTest(argc, argv);
     AssignWinName("TermWin", TERM_EMU);
#endif
     TestScrolling("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

