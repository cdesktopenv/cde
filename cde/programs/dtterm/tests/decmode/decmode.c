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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: decmode.c /main/3 1995/10/31 11:56:41 rswiston $ */
#include <stdio.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];

void FillScrLnNumbers(Lines, Cols)
int Lines, Cols;
{
  int i, j;
  char Str[10];
    HomeUp(); ClearScreen();
    for (i=0; i < Lines; i++)  {
      sprintf(Str, "%x", (i % 15) +1);
      for (j=0; j < Cols; j++) WRITETEST(Str); 
    }
}

void TestScrollRegn(Lines, Cols, ScrollTop, ScrollBottom, WinName, ImageFile)
int Lines, Cols, ScrollTop, ScrollBottom;
char *WinName, *ImageFile;
{
  char Str[IMAGE_FILE_LEN];
    ClearScreen();
    SetScrollRegn(ScrollTop, ScrollBottom);
    SetOriginMode(DECModeSet); /* To Origin Mode */
    FillScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
#ifdef SYNLIB
    sprintf(Str, "%sA", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    SetOriginMode(DECModeSave);  /* Save the mode set (Origin Mode) */
    SetOriginMode(DECModeReset); /* Set the mode to cursor mode */
    ClearScreen();
    FillScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
#ifdef SYNLIB
    sprintf(Str, "%sB", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    ClearScreen();
    SetOriginMode(DECModeRestore);  /* Restore the saved mode (Origin Mode)*/
    FillScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
#ifdef SYNLIB
    sprintf(Str, "%sC", ImageFile);
    MatchWindows(WinName, Str); 
#endif
}


void TestReverseWrap(Lines, Cols, WinName, ImageFile)
int Lines, Cols;
char *WinName, *ImageFile;
{
  char Str[IMAGE_FILE_LEN];
    SetScrollRegn(1, Lines);
    SetOriginMode(DECModeReset);
    SetWrapMode(DECModeSet);
    SetWrapMode(DECModeSave);
    FillScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
    SetReverseWrap(DECModeSet);
    SetReverseWrap(DECModeSave);
    CursorBack((Lines*Cols) / 2);
    InsertBlanks(Cols);
#ifdef SYNLIB
    sprintf(Str, "%sA", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    SetReverseWrap(DECModeReset);
    SetWrapMode(DECModeReset);
    CursorForward(Cols*Lines);
    CursorBack((Lines*Cols));
sleep(10);
#ifdef SYNLIB
    sprintf(Str, "%sB", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    SetWrapMode(DECModeRestore);
    FillScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
    SetReverseWrap(DECModeRestore);
    BackSpace(((Lines*Cols) / 2) + 1);
    InsertBlanks(Cols);
#ifdef SYNLIB
    sprintf(Str, "%sC", ImageFile);
    MatchWindows(WinName, Str); 
#endif
}

void TestReverseVideo(Lines, Cols, WinName, ImageFile)
int Lines, Cols;
char *WinName, *ImageFile;
{
  char Str[IMAGE_FILE_LEN];
    SetOriginMode(DECModeReset);
    SetWrapMode(DECModeSet);
    FillScrLnNumbers(Lines, Cols);
    SetVideoType(DECModeSet);
    SetVideoType(DECModeSave);
#ifdef SYNLIB
    sprintf(Str, "%sA", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    SetVideoType(DECModeReset);
    FillScrLnNumbers(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sB", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    SetVideoType(DECModeRestore);
    FillScrLnNumbers(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sC", ImageFile);
    MatchWindows(WinName, Str); 
#endif
    SetVideoType(DECModeReset);
}

void WriteTwiceCols(Cols)
int Cols;
{
  int i;
    for (i=0; i < (2*Cols); i++) 
      if (i <= Cols) WRITETEST("a");
      else WRITETEST("b");
}

void TestDECMode(WinName)
char *WinName;
{
  int Lines, Cols, GotX, GotY;
  char Str1[IMAGE_FILE_LEN], Str2[IMAGE_FILE_LEN];

    START(1, 0, 0, 0,0);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); exit(-1);}
    ClearScreen(); 
    GetWinSize(&Lines, &Cols); 
#ifdef DEBUG
fprintf(TermLog, "WINDOW Size Cols: %d Lines: %d \n", Cols, Lines);
SAVELOG;
#endif
    sprintf(Str1, "%sdecmode1", IMAGE_DIR);
    TestScrollRegn(Lines, Cols, 5, 15, WinName, Str1);
    sprintf(Str1, "%sdecmode2", IMAGE_DIR);
    TestScrollRegn(Lines, Cols, 3, 23, WinName, Str1);
    SetWrapMode(DECModeReset);
    ClearScreen();
    WriteTwiceCols(Cols);
    GetCursorPosn(&GotX, &GotY);
    if (GotX != Cols) LogError("WrapMode Off failed");
    else LogError("WrapMode Off Success");
    NextLine();
    WriteTwiceCols(Cols);
    GetCursorPosn(&GotX, &GotY);
    if (GotX != Cols) LogError("WrapMode Off failed");
    else LogError("WrapMode Off Success");
    sprintf(Str1, "%sdecmode3", IMAGE_DIR);
    TestReverseWrap(Lines, Cols, WinName, Str1);
    sprintf(Str1, "%sdecmode4", IMAGE_DIR);
    TestReverseVideo(Lines, Cols, WinName, Str1);
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
     TestDECMode("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

