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
/* $XConsortium: edittest.c /main/3 1995/10/31 11:57:08 rswiston $ */
#include <stdio.h>
#include  "synvar.h"

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

void FillHalfScrLnNumbers(Lines, Cols)
int Lines, Cols;
{
  int i, j;
  char Str[10];
    HomeUp(); ClearScreen();
    for (i=0; i < Lines; i++)  {
      sprintf(Str, "%x", (i % 15) +1);
      for (j=0; j < Cols/2; j++) WRITETEST(Str); 
      if (i != (Lines-1)) NextLine();
    }
}

void InsertTest(Lines, Cols)
int Lines, Cols;
{
  int i, Count=5; char Str[100];

    for (i=0; i < Count; i++) NextLine();
#ifdef SLOW
sleep(5);
#endif
    InsertLines(1);
#ifdef SLOW
sleep(5);
#endif
    sprintf(Str,"CameDown by %d Lines and inserted this line", Count);
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
    Count = 10;
    for (i=0; i < Count; i++) NextLine();
    InsertLines(2);
    sprintf(Str,"CameDown by %d Lines and inserted two lines  ", Count);
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
}

void IndexTest(Lines)
int Lines;
{
  int i; char Str[100];
    HomeUp();
    for (i=0; i < Lines; i++) Index();
    InsertLines(1);
#ifdef SLOW
sleep(5);
#endif
    sprintf(Str,"Screen Should have scrolled by a line; let me scroll two more");
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
    Index(); Index();
#ifdef SLOW
sleep(5);
#endif
}

void ReverseIndexTest(Lines, Cols)
int Lines, Cols;
{
  int i, Count; char Str[100];
    for (i=0; i < Lines; i++) ReverseIndex(); 
#ifdef SLOW
sleep(5);
#endif
    Count = 1;
    for (i=0; i < Count; i++) ReverseIndex(); 
    InsertLines(1); CursorBack(Cols);
    sprintf(Str,"Screen should have scrolled down by  3 lines;");
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
    Count = 2;
    ReverseIndex();
    CursorBack(Cols);
    sprintf(Str,"Let me scroll %d more ", Count);
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
    for (i=0; i < Count; i++) ReverseIndex(); 
#ifdef SLOW
sleep(5);
#endif
}

void DeleteLnTest(Lines, Cols)
int Lines, Cols;
{
  int i, Count=10; char Str[100];
sleep(2);
FLUSHTEST();
    CursorBack(Cols);
sleep(5);
FLUSHTEST();
    CursorUp(Count);
sleep(5);
FLUSHTEST();
    DeleteLines(2); 
sleep(5);
FLUSHTEST();
    InsertLines(1);
    sprintf(Str,"CameUp by %d Lines and deleted two lines  ", Count);
    WRITETEST(Str);
sleep(5);
FLUSHTEST();
#ifdef SLOW
sleep(5);
#endif
    CursorBack(Cols);
    Count = 5;
    CursorUp(Count);
    DeleteLines(1); 
    InsertLines(1);
    sprintf(Str,"CameUp by %d Lines and deleted a line  ", Count);
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
}

void DeleteCharTest()
{
  int i, Count=3; char Str[100];
    CursorUp(Count);
    CursorBack(20);
    DeleteChars(10); NextLine();
    InsertLines(1);
    sprintf(Str, "Came Up %d lines and deleted ten charactes; ", Count);
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
    Count = 2;
    CursorBack(20);
    CursorUp(Count);
    DeleteChars(90); NextLine();
    InsertLines(1);
    sprintf(Str,"Came Up %d lines and tried to delete 90 chars", Count);
    WRITETEST(Str);
#ifdef SLOW
sleep(5);
#endif
}


void InsertCharTest(Lines, Cols)
int Lines, Cols;
{
  int i, Count;
    FillScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
    CursorForward(Cols/2);
    for (i=0; i < Lines; i++) {
      CursorBack(1); DeleteChars(i*2+1); 
#ifdef SLOW
      FLUSHTEST(); sleep(1); 
#endif
      InsertBlanks(i*2+1);  CursorDown(1); 
#ifdef SLOW
     FLUSHTEST(); sleep(1); 
#endif
    }
}

void InsertReplTest(Lines, Cols)
int Lines, Cols;
{
  int i;
    FillHalfScrLnNumbers(Lines, Cols);
    FLUSHTEST(); 
    HomeUp();
    for (i=0; i < (Lines/2); i++) {
      CursorForward(2*i+1);
      SetInsertMode();
      WRITETEST("Now In Insert Mode");
      NextLine(); CursorBack(Cols);
      CursorForward(2*i+1);
      SetReplaceMode();
      WRITETEST("Now In Repalce Mode");
      if (i != ((Lines/2) -1)) {NextLine(); CursorBack(Cols);}
    }
    FLUSHTEST();
}

#define FILLSCR \
sleep(5); \
    FillScrLnNumbers(Lines, Cols); \
    FLUSHTEST();  \
sleep(5);


void TestAllEdit(WinName)
char *WinName;
{
  int i,j,k, l, Wrap;
  int Lines, Cols, TabSpace, TabCount;
  char Str[IMAGE_FILE_LEN];

    START(1, 0, 0, 0,0);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); exit(-1);}
    ClearScreen(); 
    GetWinSize(&Lines, &Cols); 
#ifdef DEBUG
fprintf(TermLog, "WINDOW Size Cols: %d Lines: %d \n", Cols, Lines);
SAVELOG;
#endif
    FILLSCR;
    HomeUp();
    InsertTest(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sedit1", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    FILLSCR;
    IndexTest(Lines);
#ifdef SYNLIB
    sprintf(Str, "%sedit2", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    FILLSCR;
    DeleteLnTest(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sedit3", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    FILLSCR;
    DeleteCharTest();
#ifdef SYNLIB
    sprintf(Str, "%sedit4", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    FILLSCR;
    ReverseIndexTest(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sedit5", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    FILLSCR;
    InsertCharTest(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sedit6", IMAGE_DIR); 
    MatchWindows(WinName, Str); 
#endif
    FILLSCR;
    InsertReplTest(Lines, Cols);
#ifdef SYNLIB
    sprintf(Str, "%sedit7", IMAGE_DIR); 
    MatchWindows(WinName, Str); 
#endif
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
     TestAllEdit("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

