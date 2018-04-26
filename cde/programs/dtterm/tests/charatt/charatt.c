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
/* $XConsortium: charatt.c /main/3 1995/10/31 11:55:53 rswiston $ */
#include <stdio.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];

#define Normal        0
#define Bold          1
#define Underscore    2
#define Inverse       3
#define Blink         4

static char *DispAttArr[] = {"Normal","Bold", "Underscore", "Inverse", "Blink"};

static SetDispAttr(WhichAttr, NewLine, BlinkOn)
int WhichAttr;
Bool NewLine, BlinkOn;
{
    switch (WhichAttr) {
      case Normal: 
        SetDispNormal();  break;
      case Bold:
        SetDispBold();  break;
      case Underscore:
        SetDispUnderscore();  break;
      case Inverse:
        SetDispInverse();  break;
    }
    if (BlinkOn == True) SetDispBlink(); 
    WRITETEST(DispAttArr[WhichAttr]);
    if (NewLine == True) {
       NextLine();
       SetDispNormal(); 
    }
}


void TestDispAtt(WinName)
char *WinName;
{
  int i, j, k, Lines, Cols;
  char Str[IMAGE_FILE_LEN];
  Bool BlinkOn;

    ClearScreen(); 
    for (i=0; i < 2; i++) {
      if (i == 1) BlinkOn = True;
      else BlinkOn = False; 
      SetDispAttr(Normal, True, BlinkOn);
      SetDispAttr(Bold, True, BlinkOn);
      SetDispAttr(Underscore, True, BlinkOn);
      SetDispAttr(Inverse, True, BlinkOn);
      SetDispAttr(Bold, False, BlinkOn);
      SetDispAttr(Underscore, True, BlinkOn);
      SetDispAttr(Bold, False, BlinkOn);
      SetDispAttr(Inverse, True, BlinkOn);
      SetDispAttr(Bold, False, BlinkOn); 
      SetDispAttr(Underscore, False, BlinkOn);
      SetDispAttr(Inverse, True, BlinkOn);
      SetDispAttr(Underscore, False, BlinkOn);
      SetDispAttr(Inverse, True, BlinkOn);
    }
#ifdef SYNLIB
    sprintf(Str, "%scharatt1", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    SetDispNormal();
FLUSHTEST();
}

void TestCursor(WinName)
char *WinName;
{
  char Str[IMAGE_FILE_LEN];
    SetScrollRegn(5, 15);
    SetDispBold(); 
    SetDispInverse(); 
    SetDispUnderscore();
    SetOriginMode(DECModeSet); /* To Origin Mode */
    SetWrapMode(DECModeSet);
    AbsoluteGoToXY(46, 14);
    SaveCursor();
    SetDispNormal();
    SetOriginMode(DECModeReset); 
    SetWrapMode(DECModeReset);
    ClearScreen();
    WRITETEST("Now In Normal Char Att; Cursor Mode; in Line 1; with no wrap and on restoring cursor");
    NextLine();
    WRITETEST("You did not see full line because of no wrap;On Restoring Cursor");
    RestoreCursor();
    WRITETEST("I Should start at (46,14) with char attribute Bold Inverse Underline, Origin mode set, scrolling region is 5,15 with wrap mode on");
#ifdef SYNLIB
    sprintf(Str, "%scharatt2", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
}

void ResetTerm(Cols)
int Cols;
{
    SetDispNormal();
    SetOriginMode(DECModeReset); 
    SetScrollRegn(1, Cols);
}

main(argc, argv)
int argc;
char *argv[];
{
     
  int NumLines, NumCols;
#ifdef LOG
if ((TermLog = fopen("term.log", "a")) == NULL) {
   if ((TermLog = fopen("term.log", "w")) == NULL)
      {printf("Logfile could not be opened \n"); exit(-1);}
}
fprintf(TermLog, "**************************************************\n");
LogTime();
fprintf(TermLog, "TestName: <%s> STARTS\n", argv[0]);
#endif 
    START(1,0,0,0,0);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); DONE(); return;} 
    GetWinSize(&NumLines, &NumCols);
#ifdef SYNLIB
     InitTest(argc, argv);
     AssignWinName("TermWin", TERM_EMU);
#endif
     TestDispAtt("TermWin");
#ifdef SLOW
sleep(10);
#endif
     TestCursor("TermWin");
     ResetTerm(NumCols);
#ifdef SYNLIB
     CloseTest(False);
#endif
    DONE();
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

