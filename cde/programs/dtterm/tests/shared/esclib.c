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
/* $XConsortium: esclib.c /main/3 1995/10/31 12:00:21 rswiston $ */
#include <stdio.h>
#include "escbase.h"
#include "common.h"

extern char *READTEST();

#define  ESC_STR_LEN      20

void HomeUp()
{
    WRITETEST(HOME_UP);
}

void ClearScreen()
{
  char Str[ESC_STR_LEN];
    sprintf(Str, "%s%s", HOME_UP, CLEAR_DISP);
    WRITETEST(Str);
}

void CursorUp(HowMuch)
int HowMuch;
{
  char Str[10];
   if (HowMuch == 0) return;
   sprintf(Str, CURSOR_N_UP, HowMuch);
   WRITETEST(Str);
}

void CursorDown(HowMuch)
int HowMuch;
{
  char Str[10];
   if (HowMuch == 0) return;
   sprintf(Str, CURSOR_N_DOWN, HowMuch);
   WRITETEST(Str);
}

void CursorForward(HowMuch)
int HowMuch;
{
  char Str[10];
   if (HowMuch == 0) return;
   sprintf(Str, CURSOR_N_FORW, HowMuch);
   WRITETEST(Str);
}

void CursorBack(HowMuch)
int HowMuch;
{
  char Str[10];
   if (HowMuch == 0) return;
   sprintf(Str, CURSOR_N_BACK, HowMuch);
   WRITETEST(Str);
}

void CursorPrevLine(HowMuch)
int HowMuch;
{
  char Str[10];
   if (HowMuch == 0) return;
   sprintf(Str, CURSOR_N_PREV_LINE, HowMuch);
   WRITETEST(Str);
}

void CursorColumn(HowMuch)
int HowMuch;
{
  char Str[10];
   if (HowMuch == 0) return;
   sprintf(Str, CURSOR_N_COLUMN, HowMuch);
   WRITETEST(Str);
}

void GetCursorPosn(X, Y)
int *X, *Y;
    /* expected ESC sequence is of the form Esc[n1;n2R */
{
  char  *Ptr, Line[5], Col[5];
  int i, j;
    WRITETEST(CUR_POSN_STAT);
    Ptr = READTEST("R");
    i = 2; j = 0;  
    while (Ptr[i] != ';') {Line[j] = Ptr[i]; i++; j++;}
    Line[j++] = NULLCHAR;
    i++; j = 0; 
    while (Ptr[i] != 'R') {Col[j] = Ptr[i]; i++; j++;}
    Col[j++] = NULLCHAR;
    *Y = atoi(Line); *X = atoi(Col);
}

int CheckTermStatus()
    /* return 1 if term is in proper status else 0 */
{
  char  *Ptr;
    WRITETEST(TERMINAL_STAT);
    Ptr = READTEST("n");
    if (strcmp(Ptr, TERMINAL_GOOD) == 0) return(0);
    else return(-1);
}

void AbsoluteGoToXY(X, Y)
int X, Y;
{
  char Str[ESC_STR_LEN];
    sprintf(Str, GOTO_X_Y, Y, X);
    WRITETEST(Str);
}

void AbsoluteGoToXY_HVP(X, Y)
int X, Y;
{
  char Str[ESC_STR_LEN];
    sprintf(Str, GOTO_X_Y_HVP, Y, X);
    WRITETEST(Str);
}

void RelativeGoToXY(CurrX, CurrY, X, Y)
int CurrX, CurrY, X, Y;
{
    if (X < CurrX) CursorBack((CurrX - X));
    else CursorForward((X - CurrX));
    if (Y < CurrY) CursorUp((CurrY - Y));
    else CursorDown((Y - CurrY));
}

void Index()
{
    WRITETEST(INDEX); 
}


void ReverseIndex()
{
    WRITETEST(REVERSEINDEX); 
}


void NextLine()
{
    WRITETEST(NEXTLINE); 
}

void InsertLines(Howmany)
int Howmany;
{
  char Str[10];
   if (Howmany == 0) return;
   sprintf(Str, INSERT_N_LINE, Howmany);
   WRITETEST(Str);
}

void InsertBlanks(Howmany)
int Howmany;
{
  char Str[10];
   if (Howmany == 0) return;
   sprintf(Str, INSERT_N_BLANK, Howmany);
   WRITETEST(Str);
}

void DeleteLines(Howmany)
int Howmany;
{
  char Str[10];
   if (Howmany == 0) return;
   sprintf(Str, DELETE_N_LINE, Howmany);
   WRITETEST(Str);
}


void DeleteChars(Howmany)
int Howmany;
{
  char Str[10];
   if (Howmany == 0) return;
   sprintf(Str, DELETE_N_CHAR, Howmany);
   WRITETEST(Str);
}

void SetInsertMode()
{
    WRITETEST(INSERT_MODE);
}


void SetReplaceMode()
{
    WRITETEST(REPLACE_MODE);
}

void GetWinSize(Lines, Cols)
int *Lines, *Cols;
{
    AbsoluteGoToXY(999, 999);
    GetCursorPosn(Cols, Lines);
}

/*** TabFunctions  ***/

void ClearAllTabs()
{
    WRITETEST(CLEAR_ALL_TABS);
}

void ClearCurTab()
{
    WRITETEST(CLEAR_CUR_TAB);
}

void SetTab()
{
    WRITETEST(SET_TAB);
}


void EraseLnFromCur()
{
    WRITETEST(ERASE_LN_FROM_CUR);
}


void EraseLnToCur()
{
    WRITETEST(ERASE_LN_TO_CUR);
}

void EraseLnAll()
{
    WRITETEST(ERASE_LN_ALL);
}


void EraseScrFromCur()
{
    WRITETEST(ERASE_FROM_CUR);
}


void EraseScrToCur()
{
    WRITETEST(ERASE_TO_CUR);
}

void EraseScrAll()
{
    WRITETEST(ERASE_ALL);
}

void EraseChars(Howmany)
int Howmany;
{
  char Str[10];
   if (Howmany == 0) return;
   sprintf(Str, ERASE_N_CHAR, Howmany);
   WRITETEST(Str);
}


void SetApplnKeyPad()
{
    WRITETEST(APPLN_KEYPAD);
}

void SetNormalKeyPad()
{
    WRITETEST(NORMAL_KEYPAD);
}

void SetDispNormal()
{
    WRITETEST(NORMAL);
}

void SetDispBold()
{
    WRITETEST(BOLD);
}

void SetDispUnderscore()
{
    WRITETEST(UNDERSCORE);
}

void SetDispBlink()
{
    WRITETEST(BLINK);
}

void SetDispInverse()
{
    WRITETEST(INVERSE);
}

void SaveCursor()
{
    WRITETEST(SAVE_CURSOR);
}


void RestoreCursor()
{
    WRITETEST(RESTORE_CURSOR);
}

void SetScrollRegn(Top, Bottom)
int Top, Bottom;
{
  char Str[ESC_STR_LEN];
    sprintf(Str, SET_SCROLL_REGN, Top, Bottom);
    WRITETEST(Str);
}


void SetCursorKeys(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(APPL_CUR_KEYS); break;
      case DECModeReset:
        WRITETEST(NORMAL_CUR_KEYS); break;
      case DECModeSave:
        WRITETEST(SAVE_CUR_KEYS); break;
      case DECModeRestore:
        WRITETEST(REST_CUR_KEYS); break;
    }
}

void SetColumnWidth(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(COLUMN_80); break;
      case DECModeReset:
        WRITETEST(COLUMN_132); break;
      case DECModeSave:
        WRITETEST(SAVE_COLUMN_TYPE); break;
      case DECModeRestore:
        WRITETEST(REST_COLUMN_TYPE); break;
    }
}

void SetScrollType(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(SMOOTH_SCROLL); break;
      case DECModeReset:
        WRITETEST(JUMP_SCROLL); break;
      case DECModeSave:
        WRITETEST(SAVE_SCROLL_TYPE); break;
      case DECModeRestore:
        WRITETEST(REST_SCROLL_TYPE); break;
    }
}

void SetVideoType(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(REVERSE_VIDEO); break;
      case DECModeReset:
        WRITETEST(NORMAL_VIDEO); break;
      case DECModeSave:
        WRITETEST(SAVE_VIDEO_TYPE); break;
      case DECModeRestore:
        WRITETEST(REST_VIDEO_TYPE); break;
    }
}

void SetOriginMode(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(ORIGIN_MODE); break;
      case DECModeReset:
        WRITETEST(CURSOR_MODE); break;
      case DECModeSave:
        WRITETEST(SAVE_SCREEN_MODE); break;
      case DECModeRestore:
        WRITETEST(REST_SCREEN_MODE); break;
    }
}

void SetWrapMode(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(WRAP_MODE); break;
      case DECModeReset:
        WRITETEST(NO_WRAP_MODE); break;
      case DECModeSave:
        WRITETEST(SAVE_WRAP_MODE); break;
      case DECModeRestore:
        WRITETEST(REST_WRAP_MODE); break;
    }
}

void SetMarginBell(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(MARGIN_BELL); break;
      case DECModeReset:
        WRITETEST(NO_MARGIN_BELL); break;
      case DECModeSave:
        WRITETEST(SAVE_MARGIN_BELL_TYPE); break;
      case DECModeRestore:
        WRITETEST(REST_MARGIN_BELL_TYPE); break;
    }
}

void SetReverseWrap(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(REVERSE_WRAP); break;
      case DECModeReset:
        WRITETEST(NO_REVERSE_WRAP); break;
      case DECModeSave:
        WRITETEST(SAVE_REVERSE_WRAP_TYPE); break;
      case DECModeRestore:
        WRITETEST(REST_REVERSE_WRAP_TYPE); break;
    }
}

void SetLoggingMode(DECMode)
DECModeType DECMode;
{
    switch(DECMode) {
      case DECModeSet:
        WRITETEST(START_LOGGING); break;
      case DECModeReset:
        WRITETEST(STOP_LOGGING); break;
      case DECModeSave:
        WRITETEST(SAVE_LOGGING_TYPE); break;
      case DECModeRestore:
        WRITETEST(REST_LOGGING_TYPE); break;
    }
}

#define  BACKSPACE        010

void BackSpace(HowMany)
int HowMany;
{
  int i; char Str[20];
    for (i=0; i < HowMany-1; i++) 
      {sprintf(Str, "%c", BACKSPACE); WRITETEST(Str);}
}
