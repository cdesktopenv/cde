/* $XConsortium: keypad.c /main/3 1995/10/31 11:58:09 rswiston $ */
#include <stdio.h>
#include  "synvar.h"

#ifdef LOG
FILE *TermLog;
#define SAVELOG  fclose(TermLog); TermLog = fopen("term.log", "a");
#endif

char LogStr[200];


void TestKeypad(WinName)
char *WinName;
{
  int i, Lines, Cols;
  char Str[IMAGE_FILE_LEN];

    START(1, 0, 0, 0, 1);
    if (CheckTermStatus() == -1)
       {printf("terminal emulator malfunctioning\n"); exit(-1);}
    ClearScreen(); 
    GetWinSize(&Lines, &Cols); HomeUp();
    for (i=0; i < 2; i++) {
      SetNormalKeyPad();
      PressKeypadKeys();
      NextLine();
      SetApplnKeyPad();
      PressKeypadKeys();
      NextLine();
    }
#ifdef SYNLIB
    sprintf(Str, "%skeypad", IMAGE_DIR);
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
     TestKeypad("TermWin");
#ifdef SYNLIB
     CloseTest(False);
#endif
#ifdef LOG
fprintf(TermLog, "TestName: <%s> ENDS\n", argv[0]);
fclose(TermLog);
#endif
}

