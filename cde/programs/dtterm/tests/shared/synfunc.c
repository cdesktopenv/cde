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
/* $XConsortium: synfunc.c /main/3 1995/10/31 12:01:30 rswiston $ */
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include  "synvar.h"


int errno;



#ifdef LOG
extern FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

static TestData test;
extern char LogStr[200];

void LogTime()
{
  time_t Time;
#ifdef LOG
    time(&Time);
    fprintf(TermLog, "%s", ctime(&Time)); 
    SAVELOG;
#endif
}

void LogError(Str)
char *Str;
{
#ifdef LOG
  fprintf(TermLog, "%s\n", Str); 
  SAVELOG;
#endif
}


char *MemAllocate(Size)
int Size;
{
   char *P;
     if (Size <= 0) {
        sprintf(LogStr, "Invalid Size for memory allocation %d ", Size); 
        LogError(LogStr); return(NULL);
     }
     P = (char *) calloc(Size, 1);
     if (P == NULL) {
        sprintf(LogStr, "No memory for allocation");
        LogError(LogStr); return(NULL);
     }
     return(P);
}

void MemRelease(P)
char *P;
{
    if (P == NULL) {LogError("Invalid memory address for release");return;}
    free(P);
}

int FileExists(FileName)
char *FileName;
{
    if (fopen(FileName, "r") != NULL) return(0);
    return((int) errno);
}


void InitTest(argc, argv)
int argc;
char *argv[];
{
    test.display = SynOpenDisplay(NULL);
    SynParseCommandOptions(argc, argv);
    SynNameWindow(test.display, DefaultRootWindow(test.display), "WinRoot");
#if 0
    SynSetDoubleClickDelay(MULTI_CLICK_DELAY);
#endif
    test.waitTime = WIN_WAIT_TIME;
}

CloseTest(exitDialog)
int exitDialog;
{
   Window Win;
   SynStatus Result;
      SynCloseDisplay(test.display);    
}

void MatchWindows(WinName, ImageFile)
char *WinName, *ImageFile;
{
  int Result;
  SynStatus SynResult;
  char *Expected, *Actual;

#ifdef SLOW
sleep(10);
#endif
    if ((Result = FileExists(ImageFile)) != 0) {
       sprintf(LogStr, "ImageFile <%s> does not exist", ImageFile);
#if 0
       if (Result ==  ENOENT) 
          sprintf(LogStr, "ImageFile <%s> does not exist", ImageFile);
       else if (Result == EACCES)  
         sprintf(LogStr, "ImageFile <%s> permission problem ", ImageFile);
       else LogError(LogStr, "ImageFile <%s> Unknown error ", ImageFile);
#endif
       LogError(LogStr);
    }
    Expected = MemAllocate((int) (strlen(ImageFile) + strlen(EXPECTED_SUFFIX)));
    Actual = MemAllocate((int) (strlen(ImageFile) + strlen(ACTUAL_SUFFIX)));
    strcpy(Expected, ImageFile); strcat(Expected, EXPECTED_SUFFIX);
    strcpy(Actual, ImageFile); strcat(Actual, ACTUAL_SUFFIX);
    if ((SynResult = SynCompareWindowImage(test.display, WinName, ImageFile,
                        Expected, Actual)) != SYN_SUCCESS) {
       sprintf(LogStr,"ImageComparison Failed ImageFile <%s> \
          Expected <%s> Actual <%s>", ImageFile, Expected, Actual);
       LogError(LogStr);
    }
    else {
      sprintf(LogStr, "ImageComparison Success \n");
      LogError(LogStr);
    }
}


int AssignWinName(WinName, TitleName)
char *WinName, *TitleName;
{
   SynStatus Result;
   Window Win, *SelList;
   int SelCount;
     Result = SynNameWindowByProperty(test.display, WinName, TitleName, 
                                       "WM_CLASS", &SelList, &SelCount);
     if (Result == SYN_SUCCESS)
        {sprintf(LogStr, "SynSucc SelCount %d ", SelCount); LogError(LogStr);}
     else if (Result == SYN_NAME_COLLISION)
        {sprintf(LogStr, "Window Collision "); LogError(LogStr);}
     else if (Result == BAD_SYN_WINDOW)
        {sprintf(LogStr, "SynFail");  LogError(LogStr); return(-1);}
     return(0);
}


void PressKeypadKeys()
{
    SynClickKey(test.display, "<KP_0>");
    SynClickKey(test.display, "<KP_1>");
    SynClickKey(test.display, "<KP_2>");
    SynClickKey(test.display, "<KP_3>");
    SynClickKey(test.display, "<KP_4>");
    SynClickKey(test.display, "<KP_5>");
    SynClickKey(test.display, "<KP_6>");
    SynClickKey(test.display, "<KP_7>");
    SynClickKey(test.display, "<KP_8>");
    SynClickKey(test.display, "<KP_9>");
    SynClickKey(test.display, "<KP_Multiply>");
    SynClickKey(test.display, "<KP_Divide>");
    SynClickKey(test.display, "<KP_Add>");
    SynClickKey(test.display, "<KP_Subtract>");
    SynClickKey(test.display, "<KP_Separator>");
    SynClickKey(test.display, "<KP_Tab>");
    SynClickKey(test.display, "<KP_Decimal>");
    SynClickKey(test.display, "<KP_Enter>");
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
}

void PrintTermString(WinName, String)
char *WinName, *String;
{
   int i;
     SynMovePointer(test.display, WinName, "LocTerm1");
     for (i=0; i < 5; i++) {
       SynPrintString(test.display, String);
       SynPrintString(test.display, "    ");
     }
     SynFlushInputBuffer(test.display);
}

void ExecCommand(Command)
char *Command;
{
sleep(5);
    SynMovePointer(test.display, "WinRoot", "LocTerm");
    SynPressAndHoldKey(test.display, "<Control_L>");
    SynPrintString(test.display, "C");
    SynReleaseKey(test.display, "<Control_L>");
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
/* Above things are auxillary things and assumes intr set as ^C */
    SynPrintString(test.display, Command);
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
}

void ExecWinCommand(WinName, Command)
char *WinName, *Command;
{
    SynMovePointer(test.display, WinName, "LocTerm1");
    SynPressAndHoldKey(test.display, "<Control_L>");
    SynPrintString(test.display, "C");
    SynReleaseKey(test.display, "<Control_L>");
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
/* Above things are auxillary things and assumes intr set as ^C */
    SynPrintString(test.display, Command);
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
}

void PressPrevKey(WinName)
char *WinName;
{
    SynClickKey(test.display, "<Prior>");
    SynFlushInputBuffer(test.display);
}

void PressCtrlGKey(WinName)
char *WinName;
{
    SynPressAndHoldKey(test.display, "<Control_L>");
    SynPrintString(test.display, "G");
    SynReleaseKey(test.display, "<Control_L>");
    SynFlushInputBuffer(test.display);
}

void PressKeyNtimes(WinName, Key, N)
char *WinName, *Key;
int N;
{
  int i;
    for (i=0; i < N; i++)
      SynPrintString(test.display, Key);
    SynFlushInputBuffer(test.display);
}

void PressBackSpace(WinName)
char *WinName;
{
    SynClickKey(test.display, "<BackSpace>");
    SynFlushInputBuffer(test.display);
}

void CloseTerm(WinName)
char *WinName;
{
    SynMovePointer(test.display, WinName, "LocTerm1");
    SynPressAndHoldKey(test.display, "<Control_L>");
    SynPrintString(test.display, "C");
    SynReleaseKey(test.display, "<Control_L>");
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
    SynPrintString(test.display, "exit");
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
}

void JustExit(WinName)
char *WinName;
{
    SynMovePointer(test.display, WinName, "LocTerm1");
    SynPrintString(test.display, "exit");
    SynClickKey(test.display, "<Return>");
    SynFlushInputBuffer(test.display);
}

int WaitWinMap(WinName)
char *WinName;
{
  SynStatus Result;
    Result = SynWaitWindowMap(test.display, WinName, WIN_WAIT_TIME);
    if (Result == SYN_TIME_OUT) 
       {LogError("term window did not come up"); return(-1);}
    return(0);
}

void WaitWinUnMap(WinName, WaitTime)
char *WinName;
long WaitTime;
{
  SynStatus Result;
    Result = SynWaitWindowUnmap(test.display, WinName, WaitTime);
    if (Result == BAD_SYN_WINDOW) LogError("Named  window does not exist");
}

void CheckCapsLock()
{
  int X, Y, X1, Y1;
  unsigned int Mask;
  Window A, B;
    if (XQueryPointer(test.display, DefaultRootWindow((test.display)),
                        &A, &B, &X, &Y, &X1, &Y1, &Mask) == True) {
       if ((Mask&LockMask) == LockMask) {
          SynClickKey(test.display, "<Caps_Lock>");
          SynFlushInputBuffer(test.display);
       }
    }
    else LogError("QueryPointer failed ");
}

void Deiconify(WinName)
char *WinName;
{
     SynDoubleClickButton(test.display, "Button1", WinName, "LocTerm1");
     SynWaitWindowMap(test.display, NULL, WIN_WAIT_TIME);
     SynWaitWindowUnmap(test.display, NULL,WIN_WAIT_TIME);
     SynWaitWindowUnmap(test.display, WinName, WIN_WAIT_TIME);
}

void TestMenu(WinName)
char *WinName;
{
    SynMovePointer(test.display, WinName, "file");
    SynClickButton(test.display, "Button1", WinName, "file");
    SynMovePointer(test.display, WinName, "file");
/*
    SynClickButton(test.display, "Button1", WinName, "exit");
*/
sleep(5);
}
