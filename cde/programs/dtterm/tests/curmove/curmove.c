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
/* $XConsortium: curmove.c /main/3 1995/10/31 11:56:15 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];

#define InitArr(Arr, NumCols, NumLines) \
{ \
    for (i=0; i < NumCols; i++) \
      for (j=0; j < NumLines; j++) Arr[i][j] = 0; \
}

void TestCurMove(WinName)
char *WinName;
{

  int i,j, Count=0, Position, Arr[200][100]; 
  int CurrX, CurrY, PrevX, PrevY, GotX, GotY;
  int Relative=1, Wrap=0, NumLines, NumCols;
  char Str[IMAGE_FILE_LEN];

    START(1,0,0,0,0);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); DONE(); return;} 
    GetWinSize(&NumLines, &NumCols);
#ifdef DEBUG
sprintf(LogStr, "WINDOW Size Cols: %d Lines: %d \n", NumCols, NumLines);
LogError(LogStr);
#endif
    ClearScreen();  InitArr(Arr, NumCols, NumLines);   
    PrevX = 0; PrevY = 0;
    while (1) {
      CurrX = (int) (rand() % NumCols);
      CurrY = (int) (rand() % NumLines);
      if (Arr[CurrX][CurrY] == 1) continue;
      if ((CurrX == PrevX) && (CurrY == PrevY))  continue;
      if (Relative == 1) RelativeGoToXY(PrevX, PrevY, CurrX, CurrY);
      else if (Relative == 2) AbsoluteGoToXY(CurrX+1, CurrY+1);
      else AbsoluteGoToXY_HVP(CurrX+1, CurrY+1);
      Arr[CurrX][CurrY] = 1; 
      if (CurrX != (NumCols - 1)) 
         {WRITETEST("0"); CursorBack(1);}
      else {
        WRITETEST("0");
        if (Wrap == 1) AbsoluteGoToXY(CurrX+1, CurrY+1);
      }
      GetCursorPosn(&GotX, &GotY);
      if ((CurrX != (GotX - 1)) || (CurrY != (GotY -1))) {
            if (Relative == 1) printf("Relative :");
            else printf("Absolute:");
            sprintf(LogStr, "Expeted X Y: %3d  %3d Got X Y: %3d  %3d \n",
                  CurrX+1, CurrY+1, GotX, GotY);
            printf("%s", LogStr); LogError(LogStr);
            DONE(); return;
      }
#ifdef DEBUG
sprintf(LogStr, "CurrX: %3d CurrY: %3d GotX: %3d GotY: %3d \n",
                  CurrX+1, CurrY+1, GotX, GotY);
LogError(LogStr);
#endif
      PrevX= CurrX; PrevY = CurrY;
      Count++; 
      if (Count >= ( NumLines *NumCols))  {
#ifdef SYNLIB
         sprintf(Str, "%scurmove%d", IMAGE_DIR, Relative);
         MatchWindows(WinName, Str);
#endif
         if ((Relative == 1) || (Relative == 2)) {
            ClearScreen(); InitArr(Arr, NumCols, NumLines); 
            Relative += 1; Count = 0;
         }
         else break;
      }
    }
    DONE();
}

#define    STARLEN      120

void TestCurColLine(WinName)
char *WinName;
{
  int i, Cols, Lines;
  char Str[STARLEN];
    START(1,0,0,0,0);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); DONE(); return;} 
    GetWinSize(&Lines, &Cols);
    ClearScreen(); HomeUp();
    CursorDown(Lines); 
    CursorPrevLine(Lines/2 + 1);
    CursorColumn(Cols/10);
    for (i=0; i < (STARLEN/2); i++) Str[i] = '*';
    Str[STARLEN/2] = NULLCHAR;
    WRITETEST(Str);
    NextLine();
    CursorColumn(Cols/10);
    WRITETEST("*");
    CursorColumn(((Cols/10) + (STARLEN/2) - 1));
    WRITETEST("*");
    NextLine();
    CursorColumn(Cols/10);
    WRITETEST(Str);
    CursorPrevLine(1);
    CursorColumn((Cols/10) + (Cols/20));
    WRITETEST("DO YOU SEE A BOX USING * IN THE CENTER ENCLOSING THIS");
#ifdef SYNLIB
    sprintf(Str, "%scurmove4", IMAGE_DIR);
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
     TestCurMove("TermWin");
     TestCurColLine("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

