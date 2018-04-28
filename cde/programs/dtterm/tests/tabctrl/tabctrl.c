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
/* $XConsortium: tabctrl.c /main/3 1995/10/31 12:02:15 rswiston $ */
#include <stdio.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];


#define  BELL             007
#define  BACKSPACE        010
#define  HORI_TAB         011
#define  NEWLINE          012
#define  VERTI_TAB        013
#define  NEWPAGE          014
#define  CARRI_RET        015



void TestTabCtrl(WinName)
char *WinName;
{
  int i,j,k, l, Lines, Cols, TabSpace, TabCount;
  char Str[IMAGE_FILE_LEN];

    START(1, 0, 0, 0, 0);
    TabSpace = 5; /* how to take parameter */
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); exit(-1);}
    ClearScreen(); 
    GetWinSize(&Lines, &Cols); 
#ifdef DEBUG
fprintf(TermLog, "WINDOW Size Cols: %d Lines: %d \n", Cols, Lines);
SAVELOG;
#endif
    ClearAllTabs(); 
    HomeUp(); 
    SetTab(); 
    TabCount = (Cols / TabSpace);
    if ((Cols % TabSpace) == 0) TabCount -= 1;
    for (i=0; i < TabCount; i++)
      {CursorForward(TabSpace); SetTab();}
    HomeUp();
    for (k=0; k < Lines; k++) {
       for (j=0; j < TabCount; j++) {
         sprintf(Str, "*%c", HORI_TAB); WRITETEST(Str); 
#ifdef SLOW
sleep(1);
#endif
         for (l=0; l < TabSpace-1; l++) {
           sprintf(Str, "%c", BACKSPACE); WRITETEST(Str);
#ifdef SLOW
sleep(1);
#endif
         } 
         for (l=0; l < TabSpace-1; l++) 
           {sprintf(Str, "-");WRITETEST(Str);} 
       }
       sprintf(Str, "*");WRITETEST(Str); 
       for (l=0; l < ((Cols-1) % TabSpace); l++) 
         {sprintf(Str, "-"); WRITETEST(Str);} 
       if (k < (Lines -1))  {
          switch ((k % 3)) {
            case 0: sprintf(Str, "%c", NEWLINE); WRITETEST(Str); break;
            case 1: sprintf(Str, "%c%c", CARRI_RET, VERTI_TAB); WRITETEST(Str);
                    break;
            case 2: sprintf(Str, "%c%c", CARRI_RET, NEWPAGE); WRITETEST(Str);
                    break;
          }
       }
    }
#ifdef SYNLIB
    sprintf(Str, "%stabctrl1", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    HomeUp();
    for (j=0; j < TabCount; j++) 
      {sprintf(Str, "%c", HORI_TAB); WRITETEST(Str); ClearCurTab();}
    HomeUp(); ClearScreen();
    for (k=0; k < Lines; k++) {
      sprintf(Str, "*%c", HORI_TAB); WRITETEST(Str);
      sprintf(Str, "%c", NEWLINE); WRITETEST(Str); 
    }
#ifdef SYNLIB
    sprintf(Str, "%stabctrl2", IMAGE_DIR);
    MatchWindows(WinName, Str);
#endif
    HomeUp(); TabSpace = 8;
    TabCount = (Cols / TabSpace);
    if ((Cols % TabSpace) == 0) TabCount -= 1;
    for (i=0; i < TabCount; i++)
      {CursorForward(TabSpace); SetTab();}
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
     TestTabCtrl("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

