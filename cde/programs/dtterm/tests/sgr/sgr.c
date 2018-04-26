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
/* $XConsortium: sgr.c /main/3 1995/10/31 11:59:14 rswiston $ */
#include <stdio.h>

#define CSI            "\033["

#define PRIMARY        0
#define BOLD           1
#define UNDERSCORE     2
#define SLOWBLINK      5
#define REVERSE        7
#define NORMAL         22
#define NOT_UNDERLINE  24
#define NOT_BLINKING   25
#define POSITIVE_IMAGE 27


#define DISP_BLACK     30
#define DISP_RED       31
#define DISP_GREEN     32
#define DISP_YELLOW    33
#define DISP_BLUE      34
#define DISP_MAGENTA   35
#define DISP_CYAN      36
#define DISP_WHITE     37

#define BACK_BLACK     40
#define BACK_RED       41
#define BACK_GREEN     42
#define BACK_YELLOW    43
#define BACK_BLUE      44
#define BACK_MAGENTA   45
#define BACK_CYAN      46
#define BACK_WHITE     47

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];
void TestSGR()
{
  char EscStr[20], WriteStr[200], AttStr[30], DispStr[30], BackStr[30];
  int i, j, k, Attribute, DispColor, BackColor, Count=0;

    for (i=0; i < 8; i++) {
      switch (i) {
        case 0: 
          Attribute = BOLD; strcpy(AttStr, "BOLD "); break;
        case 1: 
          Attribute = UNDERSCORE; strcpy(AttStr, "UNDERSCORE "); break;
        case 2: 
          Attribute = SLOWBLINK; strcpy(AttStr, "SLOWBLINK "); break;
        case 3: 
          Attribute = REVERSE; strcpy(AttStr, "REVERSE "); break;
        case 4: 
          Attribute = NORMAL; strcpy(AttStr, "NORMAL "); break;
        case 5: 
          Attribute = NOT_UNDERLINE; strcpy(AttStr, "NOT_UNDERLINE "); break;
        case 6: 
          Attribute = NOT_BLINKING; strcpy(AttStr, "NOT_BLINKING "); break;
        case 7: 
          Attribute = POSITIVE_IMAGE; strcpy(AttStr, "POSITIVE_IMAGE "); break;
       }
       for (j=0; j < 8; j++) {
         switch (j) {
           case 0: 
             DispColor = DISP_BLACK; strcpy(DispStr, "DISP_BLACK "); break;
           case 1: 
             DispColor = DISP_RED; strcpy(DispStr, "DISP_RED "); break;
           case 2: 
             DispColor = DISP_GREEN; strcpy(DispStr, "DISP_GREEN "); break;
           case 3: 
             DispColor = DISP_YELLOW; strcpy(DispStr, "DISP_YELLOW "); break;
           case 4: 
             DispColor = DISP_BLUE; strcpy(DispStr, "DISP_BLUE "); break;
           case 5: 
             DispColor = DISP_MAGENTA; strcpy(DispStr, "DISP_MAGENTA "); break;
           case 6: 
             DispColor = DISP_CYAN; strcpy(DispStr, "DISP_CYAN "); break;
           case 7: 
             DispColor = DISP_WHITE; strcpy(DispStr, "DISP_WHITE "); break;
         }
         for (k=0; k < 8; k++) {
           switch (k) {
             case 0: 
               BackColor = BACK_BLACK; strcpy(BackStr, "BACK_BLACK "); break;
             case 1: 
               BackColor = BACK_RED; strcpy(BackStr, "BACK_RED "); break;
             case 2: 
               BackColor = BACK_GREEN; strcpy(BackStr, "BACK_GREEN "); break;
             case 3: 
               BackColor = BACK_YELLOW; strcpy(BackStr, "BACK_YELLOW "); break;
             case 4: 
               BackColor = BACK_BLUE; strcpy(BackStr, "BACK_BLUE "); break;
             case 5: 
               BackColor = BACK_MAGENTA; strcpy(BackStr, "BACK_MAGENTA "); break;
             case 6: 
               BackColor = BACK_CYAN; strcpy(BackStr, "BACK_CYAN "); break;
             case 7: 
               BackColor = BACK_WHITE; strcpy(BackStr, "BACK_WHITE "); break;
           }
           sprintf(EscStr, "%s%d;%d;%dm", CSI, Attribute, DispColor, BackColor);
           WRITETEST(EscStr);
           sprintf(WriteStr, "%2d;%2d;%2d ", Attribute, DispColor, BackColor);
           WRITETEST(WriteStr);
           sprintf(EscStr, "%s0m", CSI);
           WRITETEST(EscStr);
         }
       }
    }
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
#ifdef DEBUG
sprintf(LogStr, "WINDOW Size Cols: %d Lines: %d \n", NumCols, NumLines);
LogError(LogStr);
#endif
    HomeUp();
    TestSGR();
    DONE();
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}
