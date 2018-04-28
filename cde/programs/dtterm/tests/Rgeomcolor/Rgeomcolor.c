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
/* $XConsortium: Rgeomcolor.c /main/3 1995/10/31 11:51:45 rswiston $ */
#include <stdio.h>
#include <math.h>
#include  "synvar.h"
#include  "common.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];


#define BLANK    ' '

void TestGeomFgBg(WinName, String, TestNum)
char *WinName, *String;
int TestNum;
{
  char Str[IMAGE_FILE_LEN];
     PrintTermString(WinName, String);
     sprintf(Str, "%srgeomcolor%d", IMAGE_DIR, TestNum);
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
/*10*/ " -ms red ",
/*11*/ " -ms blue "
};

static char *ResourceArr[] = {
/*0*/ "geometry 40x20 foreground green background red",
/*1*/ "geometry 60x15 foreground red background green",
/*2*/ "geometry 80x10 foreground green background yellow",
/*3*/ "geometry 80x40 foreground blue background cyan",
/*4*/ "geometry 20x40 foreground white background black",
/*5*/ "reverseVideo True",
/*6*/ "reverseVideo On",
/*7*/ "reverseVideo False",
/*8*/ "backgroundIsSelect True",
/*9*/ "backgroundIsSelect False",
/*10*/ "pointerColor red",
/*11*/ "pointerColor blue"
};

static char *LogAction[] = {
"Testing Resource Option: geometry 40x20 foreground green background red",
"Testing Resource Option: geometry 60x15 foreground red background green",
"Testing Resource Option: geometry 80x10 foreground green background yellow",
"Testing Resource Option: geometry 80x40 foreground blue background cyan",
"Testing Resource Option: geometry 20x40 foreground white background black",
"Testing Resource Option: reverseVideo True",
"Testing Resource Option: reverseVideo On",
"Testing Resource Option: reverseVideo False",
"Testing Resource Option: backgroundIsSelect True",
"Testing Resource Option: backgroundIsSelect False",
"Testing Resource Option: pointerColor red",
"Testing Resource Option: pointerColor blue"
};

#define ArrCount    (int) (sizeof(ResourceArr) / sizeof(char *))

int MakeResourceFile(ResArr)
char *ResArr;
{
  FILE *ResFile;
  int i, j, Len;
  char Resource[50], Value[50];
    
    if ((ResFile = fopen("res", "w")) == NULL)
       {LogError("Resource File Creation fail"); return(-1);}
    for (i=0; i < 50; i++) Resource[i] = BLANK;
    for (i=0; i < 50; i++) Value[i] = BLANK;
    Len = strlen(ResArr); i=0;
    while (i < Len) {
      j = 0;
      while (ResArr[i] != BLANK) Resource[j++] = ResArr[i++];
      Resource[j] = NULLCHAR;
      i++; j = 0;
      while ((ResArr[i] != BLANK) && (ResArr[i] != NULLCHAR))
            Value[j++] = ResArr[i++];
      Value[j] = NULLCHAR;
      i++;
      fprintf(ResFile, "%s*%s: %s\n", TERM_EMU, Resource, Value);
    }
    fclose(ResFile);
}

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
fprintf(TermLog, "**************************************************\n");
LogTime();
fprintf(TermLog, "TestName: <%s> STARTS\n", argv[0]);
#endif 
     InitTest(argc, argv);
     for (i=0; i < ArrCount; i++) {
       CheckCapsLock();
       ExecCommand("xrdb -load xrdb.out");
       MakeResourceFile(ResourceArr[i]);
       CheckCapsLock();
       strcpy(Command, "xrdb -merge res; ");
       strcat(Command, TERM_EMU);
       ExecCommand(Command);
       if (WaitWinMap("TermWin") < 0) continue;
       AssignWinName("TermWin", TERM_EMU);
       TestGeomFgBg("TermWin", OptionArr[i], i+1);
       WaitWinUnMap("TermWin", 60L);
     }
     CheckCapsLock();
     ExecCommand("xrdb -load xrdb.out");
     CloseTest(False);
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

