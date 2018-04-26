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
/* $XConsortium: erase.c /main/3 1995/10/31 11:57:36 rswiston $ */
#include <stdio.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];

void FillScreen(Lines, Cols, Ch)
int Lines, Cols;
char Ch;
{
  int i, j; char Str[10];
   ClearScreen();
   sprintf(Str, "%c", Ch);
   for (i=0; i < Lines; i++) 
     for (j=0; j < Cols; j++) 
       WRITETEST(Str);
}

void EraseLnTest(Lines, Cols, WinName)
int Lines, Cols;
char *WinName;
{
  int i; char Str[IMAGE_FILE_LEN];
    FillScreen(Lines, Cols, 'X');
    FLUSHTEST();
    HomeUp();
    for (i=0; i < Lines; i++) {
      CursorForward(1); EraseLnFromCur(); CursorDown(1); 
#ifdef SLOW
      sleep(1); FLUSHTEST();
#endif
    }
#ifdef SYNLIB
    sprintf(Str, "%serase1", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    HomeUp();
    FillScreen(Lines, Cols, 'M');
    FLUSHTEST();
    HomeUp();
    for (i=0; i < Lines; i++) {
      CursorForward(1); EraseLnToCur(); CursorDown(1); 
#ifdef SLOW
      sleep(1); FLUSHTEST();
#endif
    }
#ifdef SYNLIB
    sprintf(Str, "%serase2", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    HomeUp();
    FillScreen(Lines, Cols, 'M');
    FLUSHTEST();
    HomeUp();
    for (i=0; i < (Lines/2); i++) {
      CursorForward(2); EraseLnAll(); CursorDown(2); 
#ifdef SLOW
      sleep(1); FLUSHTEST();
#endif
    }
#ifdef SYNLIB
    sprintf(Str, "%serase3", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
}

#define MoveBy    5

void EraseScrTest(Lines, Cols, WinName)
int Lines, Cols;
char *WinName;
{
  int i, Count;
  char Str[IMAGE_FILE_LEN];
    FillScreen(Lines, Cols, 'R');
    FLUSHTEST();
    Count = Lines / MoveBy;
    for (i=0; i < Count; i++) {
      CursorBack(((Cols-MoveBy) /Count)); CursorUp(MoveBy); 
      EraseScrFromCur(); 
#ifdef SLOW
      sleep(2); FLUSHTEST();
#endif
    }
    sprintf(Str, "%serase4", IMAGE_DIR);
#ifdef SYNLIB
    MatchWindows(WinName, Str);
#endif
    ClearScreen();
    FillScreen(Lines, Cols, 'M');
    FLUSHTEST();
    HomeUp();
    for (i=0; i < Count; i++) {
      CursorForward(((Cols-MoveBy) /Count)); CursorDown(MoveBy); 
      EraseScrToCur(); 
#ifdef SLOW
      sleep(2); FLUSHTEST();
#endif
    }
#ifdef SYNLIB
    sprintf(Str, "%serase5", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
}


void EraseCharTest(Lines, Cols, WinName)
int Lines, Cols;
char *WinName;
{
  int i, Count;
  char Str[IMAGE_FILE_LEN];
    FillScreen(Lines, Cols, 'R');
    FLUSHTEST();
    HomeUp();
    CursorForward(Cols/2);
    for (i=0; i < Lines; i++) {
      CursorBack(1); EraseChars(i*2+1); CursorDown(1); 
#ifdef SLOW
      sleep(1); FLUSHTEST();
#endif
    }
#ifdef SYNLIB
    sprintf(Str, "%serase6", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
}

void TestAllErase(WinName)
char *WinName;
{
  int i,j,k, l, Wrap;
  int Lines, Cols, TabSpace, TabCount;
  char Str[IMAGE_FILE_LEN];

    START(1, 0, 0, 0, 0);
/*
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); exit(-1);}
*/
    ClearScreen(); 
    GetWinSize(&Lines, &Cols); 
#ifdef DEBUG
fprintf(TermLog, "WINDOW Size Cols: %d Lines: %d \n", Cols, Lines);
SAVELOG;
#endif
    EraseLnTest(Lines, Cols, WinName);
    EraseScrTest(Lines, Cols, WinName);
    EraseCharTest(Lines, Cols, WinName);
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
     TestAllErase("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

