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
/* $XConsortium: qalib.c /main/4 1996/10/01 16:27:40 drk $ */
/*************************************************************
*  Module Name        :  qalib.c 
*  Original Author    :  Panacom 
*  Revision Date      :  10/21/92 
*  Revision Author    :  Randall Robinson 
*  Data Files         :  None
*
*  Abstract           :  This file contains the global variable definitions, 
                         and the common function definitions used between
                         all (most) test.
*                        
*                        Many variable and functions definitions have been 
*                        left in place that were originally used by Panacom, 
*                        and are no longer necessary. 
***************************************************************/
#ifndef _QALIB_INCLUDE
#define _QALIB_INCLUDE

#include "include_files.h"

#define __nu            "\0"
#define __eq            "\005"
#define __ak            "\006"
#define __lf            "\n"
#define __cr            "\015"
#define __d1            "\021"
#define __d2            "\022"
#define __d3            "\023"

#define __buf_len       4096
#define __ack_time_out  10

/*  not used functionally */
typedef struct __TIME {
    short Hund, Sec, Min, Hour;
} __TIME;

/*
typedef struct __INS8250 {
    short Thr, Rbr, Ier, Iir, Lcr, Mcr, Lsr, Msr;
} __INS8250;
*/
/*
typedef union __REGISTERS {
    struct {
        short Ax, Bx, Cx, Dx, Bp, Si, Di, Ds, Es, Flags;
    } U1;
    struct {
        uchar Al, Ah, Bl, Bh, Cl, Ch, Dl, Dh;
    } U2;
} __REGISTERS;
*/

/* not used functionally  */
typedef enum {
    _B110, _B150, _B300, _B600, _B1200, _B2400, _B4800, _B9600, _B19200, _B38400
} __BAUD_TYPE;


/* not used functionally  */
typedef struct __DATACOM_REC {
    __BAUD_TYPE Baud;  
    uchar Parity, Data_Bits, Stop_Bits;
    Char Intrig, Interm, Hand_Shake;
    double Time_Out;
    boolean Echo, Xoffed;
    short Xcount, Ea_Count;
} __DATACOM_REC;

/* not used functionally  */
typedef __DATACOM_REC __DATACOM_TYPE[4];
typedef uchar __BUFFER_TYPE[__buf_len + 1];
typedef __BUFFER_TYPE __BUFFERS[4];
typedef short __POINTERS[4];


/* These are needed */
/* global variables */
int fd_test = -1;               /* file descriptor for /dev/tty  */
FILE *f_test = (FILE *)NULL;    /* file pointer to /dev/tty      */
int __DEBUG = 0;                /* set from command by   "-D:Y   */

struct termios termio_orig, termio_test;    /* for /dev/tty      */


typedef char __C_BUFFER[BUFSIZ];
__C_BUFFER buffer;  


/* ******   In the john dir: these flags not needed. Config.c file
 ********   controls these parameters, but they are needed in the
 ********   other 3 dirs. 
*/
int page_mode= 0;
int block_mode=0;
int autolf_mode=0;
int DC1_hndshk=1, DC2_hndshk = 0, NO_hndskh=0;
int inh_hndshk=0, inh_DC2=1;


Char Input_Terminator[4];    /* used */
Char Input_Trigger[4];       /* not used yet functionally, but still used
                                in some functions as a variable.  RCR */

Static FILE *__Message_File;                   /* for the .LOG file */
Static boolean __Group_Not_Running;    /* used to have something to do with
                                          option -D                         */
Static Char __File_Name[256], __Command_Line[256];
Static Char __Message_File_NAME[_FNSIZE];
Static boolean __Manual, __Log_Every_Thing, __Log_Msg;


/* function declarations */
extern void logln(); 
void timeout ();
void cleanup ();
char *vis();
char *in();


/* not used functionally */
Static uchar __default_test_term = 1, __default_ref_term = 0,
             __default_aux_a = 0, __default_aux_b = 0;
/*
Static uchar __com_jump_table[4][6] = {
    { 0x2e, 0xff, 0x16, 0, 0, 1 },
    { 0x2e, 0xff, 0x16, 0, 0, 2 },
    { 0x2e, 0xff, 0x16, 0, 0, 3 },
    { 0x2e, 0xff, 0x16, 0, 0, 4 }
};
*/

/*
Static __INS8250 __rs232[4] = {
    { 0x3f8, 0x3f8, 0x3f9, 0x3fa, 0x3fb, 0x3fc, 0x3fd, 0x3fe },
    { 0x2f8, 0x2f8, 0x2f9, 0x2fa, 0x2fb, 0x2fc, 0x2fd, 0x2fe },
    { 0x3e8, 0x3e8, 0x3e9, 0x3ea, 0x3eb, 0x3ec, 0x3ed, 0x3ee },
    { 0x2e8, 0x2e8, 0x2e9, 0x2ea, 0x2eb, 0x2ec, 0x2ed, 0x2ee }
};
*/

/* not needed functionally, currently just as place holders. */
Static __DATACOM_REC __default_datacom_rec = {
    _B9600, 0, 8, 1, '\021', '\015', 'E', 60000.0, false, false, 0, 0
};
Static short __com_int_addr = 0, __data_segment = 0;
Static uchar Test_Com=0, Ref_Com, Aux_A_Com, Aux_B_Com;
Static __DATACOM_TYPE __Data_Com;
Static boolean __Refpres, __Aux_A_Pres, __Aux_B_Pres;


/*
Static uchar __Rs_Error;
Static __BUFFERS __The_Buffs;
Static __POINTERS __Heads, __Tails;      
Static short __Oldcs[4], __Oldip[4];
Static __REGISTERS __Regs; 
Static boolean __Initialized, __Rs_Time_Out;
*/


/* These are needed */
Char **P_argv;                   /* pascal global **argv */
int P_argc=0;                          /* pascal gloabal argc  */
int P_ioresult;
Char STRTMP[256];
Char HAND_SHAKE;


/* **********  start of function definitions   ************/


Static void
PAUSE(double Sec)
{
  sleep( (unsigned int)Sec);
}


Static void
SETSTRLEN(Char *S, short E)
{
    S[(long)((int)E)] = '\0';
}


Static void
STRAPPEND(Char *S1, Char *S2)
{
    if ((int) strlen(S1) + (int) strlen(S2) <= 255)
        strcat(S1, S2);
}


Static void
STRDELETE(Char *S, short P, short N)
{
   MODULE_ERR("STRDELETE not supported");
   cleanup(0);
   return;

}
/* STRINSERT and strinsert both not supported yet, Do not understand
   the intent of them.                 
*/
Static void
STRINSERT(Char *S1, Char *S2, short P)
{
  MODULE_ERR("STRINSERT not supported yet");
  cleanup(0);
  return;

}

/* Starting at "start_pos" from string "input", copy the results into
   "Result".            
*/
Static Char *
strsub(Char *Result, Char *input, short start_pos, short len)
{
  Char STR1[256];
  Char *tmp;
  int i;
  
  for(i=1; i <= start_pos; i++)
     input++;
  tmp= input;
  strcpy(Result, tmp);
  return Result;
}

/* Do not understand the intent behind STRMOVE. NOT supported yet. */
Static void
STRMOVE(short N, Char *S1_, short P1, Char *S2, short P2)
{

    MODULE_ERR("STRMOVE not supported yet");
    cleanup(0);
    return;

}


Static Char *
STRRPT(Char *Result, Char *S, short N)
{
    Char St[256];
    short I;

    *St = '\0';
    for (I = 1; I <= N; I++)
        strcat(St, S);
    return strcpy(Result, St);
}


Static short
STRLEN(Char *S)
{
    return ((short) strlen(S));
}


/* this function only supports returning position of one char (not
   a string).  "num" is the occurrence of the character's position to
   return.                                                         */
Static short
strpos2(Char *S1, Char *S2, short num)
{
   Char STR1[256];
   short found = 0;
   short done = 0;
   short index = 0;
   
   strcpy(STR1,S1);

   while(!done) {
     if(STR1[index] == S2[0]) {
       found++;
     }
     if(found == num)
       done = 1;
     else
       index++;
   }

   return index;
}
    

Static short
STRPOS(Char *S1, Char *S2)
{
    return strpos2(S2, S1, 1);
}


Static Char *
STRLTRIM(Char *Result, Char *S_)
{
    Char S[256];
    boolean Done;
    Char *tmp;
    strcpy(S, S_);
    Done = false;
    if (*S == '\0')
        return strcpy(Result, S);
    if (S[0] != ' ')
        return strcpy(Result, S);
    do {
    /*    strdelete((void *)S, 1, 1);  */
    /* remove the first blank          */
    /*    strcpy(S,strchar(S,S[1]));  This is not working */
        tmp = S + sizeof(Char);
        strcpy(S,tmp);
        if (*S != '\0') {
            if (S[0] != ' ')
                Done = true;
        }
        else {
            Done = true;
        }
    } while (!Done);
    return strcpy(Result, S);
}

Static Char *
STRRTRIM(Char *Result, Char *S_)
{
    Char S[256];
    boolean Done;

    strcpy(S, S_);
    Done = false;
    if (*S == '\0')
        return strcpy(Result, S);
    if (S[(int) strlen(S) - 1] != ' ')
        return strcpy(Result, S);
    do {
     /*    strdelete((void *)S, strlen(S), 1);  */
     /*   remove trailing blank                 */
        S[(int) strlen(S) - 1]='\0';
        if (*S != '\0') {
            if (S[(int) strlen(S) - 1] != ' ')
                Done = true;
        }
        else {
            Done = true;
        }
    } while (!Done);
    return strcpy(Result, S);
}


Static Char *
UPCASE_STRING(Char *Result, Char *S_)
{
    Char S[256];
    short I, FORLIM;

    strcpy(S, S_);
    FORLIM = (short) strlen(S);
    for (I = 0; I < FORLIM; I++)
        S[I] = toupper(S[I]);
    return strcpy(Result, S);
}


Static void
FIX_STRING(Char *S)
{
    Char STR1[256], STR2[256], STR3[256];

    strcpy(S, UPCASE_STRING(STR1, STRRTRIM(STR2, STRLTRIM(STR3, S))));
}


Static Char *
TOHEXSTR(Char *Result, short Num)
{
    static Char the_chars[16] = "0123456789ABCDEF";
    short I;

    Result[4] = '\0';
    for (I = 0; I <= 3; I++)
        Result[3 - I] = the_chars[(((unsigned)Num) >> (I * 4)) & 0xf];
    return Result;
}


Static void
SUBTIME(__TIME Time2, __TIME Time1, __TIME *Time_Out)
{
    if (Time1.Hund < Time2.Hund) {
        Time1.Sec--;
        Time1.Hund += 100;
    }
    Time_Out->Hund = Time1.Hund - Time2.Hund;
    if (Time1.Sec < Time2.Sec) {
        Time1.Min--;
        Time1.Sec += 60;
    }
    Time_Out->Sec = Time1.Sec - Time2.Sec;
    if (Time1.Min < Time2.Min) {
        Time1.Hour--;
        Time1.Min += 60;
    }
    Time_Out->Min = Time1.Min - Time2.Min;
    Time_Out->Hour = Time1.Hour - Time2.Hour;
}


Static void
GET_TIME(__TIME *Cur_Time)
{ 
   ;
}


Static void
COM_INT_ROUTINE(void)
{
    uchar Com_Num, In_Char;
    boolean Use_It;
;
}


Static void
RS232_CLEANUP(uchar Com)
{
;
}


/*Static */void
DONE(void)
{
    (void) cleanup(0);

}


Static void
__ERROR(short Num, short Where)
{
    Char STR2[256];

    printf("\nProgram terminated before completed.\n");
    printf("Program counter is $%s.  Error Number is %d.\n",
           TOHEXSTR(STR2, Where), Num);
    DONE();
}


int
MODULE_ERR(Char *S)
{
   perror(S);
   if(fprintf(__Message_File, "*** Fatal Error ***  %s\n", S) < 0) {
     perror("MODULE_ERR: fprintf: ");
     DONE();
   }
   DONE();
   return 1;

}


Static void
SET_DATA_COM(__BAUD_TYPE Speed, uchar Parity, uchar Stop, uchar Data_Bits,
             uchar Com)
{

;
}

/* Only supporting test terminal (no reference terminal).  If support
   reference terminal, must pass in correct file descriptor. Now,
   var "Com" is not used. Only left in to keep from having to make
   changes.                                                           */
Static void
FLUSH_BUFFER_COM(short Com)
{

    if(tcflush(fd_test,TCIOFLUSH) < 0) {
       MODULE_ERR("FLUSH_BUFFER_COM: tcflush:"); 
       DONE();
    } 
}

/* No refernece terminal supported, only 1 (test) terminal to flush. */
Static void
FLUSH_BUFFERS(void)
{
  FLUSH_BUFFER_COM(1);
   
}


Static short
READY(uchar Com, short Time_Out)
{
;

}


#define enq_count       80
#define time_out_const  32000

/*
Static void
RS232_OUT(Char Param, uchar Com)
{

;

}
*/

#undef enq_count
#undef time_out_const

/* Only test terminal supported. */
Static void
__WRITE_BOTH(Char Ch)
{
   Char STR1[256];

   sprintf(STR1,"%s",Ch);
   WRITETEST(STR1);

}

/*
Static void
RS232_INIT(uchar Com)
{
;

}
*/

Static void
INIT_DEFAULTS(int Echo)
{

/* These can be removed, they are not used functionally.  Only to keep
   some having to change some function parameters.
*/
char Device[30];
    Test_Com = __default_test_term;
    Ref_Com = __default_ref_term;
    Aux_A_Com = __default_aux_a;
    Aux_B_Com = __default_aux_b;
    __Refpres = (__default_ref_term != 0);
    __Aux_A_Pres = (__default_aux_a != 0);
    __Aux_B_Pres = (__default_aux_b != 0);
    __Data_Com[0] = __default_datacom_rec;
    __Data_Com[1] = __default_datacom_rec;
    __Data_Com[2] = __default_datacom_rec;
    __Data_Com[3] = __default_datacom_rec;
    /* __Rs_Time_Out = false;               */
/* ****************************************************************/ 

    /* Open /dev/tty and obtain file descriptor and FILE pointer. */
    if((fd_test = open("/dev/tty", O_RDWR)) < 0) {
      (void) perror("INIT_DEFAULTS: /dev/tty open:");
      (void) exit(1);
    }

    f_test = fdopen(fd_test, "w"); 
    (void) setvbuf(f_test, (char *) 0, _IONBF, 0);

    /* get struct termio for later restoration and use for setting defaults */
    if(tcgetattr(fd_test, &termio_orig) < 0) {
      (void) perror("INIT_DEFAULTS: tcgetattr:");
      (void) exit(1);
    }
    
    /* Default settings determined from PANACOM tests, and translated to
       HP_UX environment. Along with default settings using struct termios. */

/* Canonical Input is OFF  */

    termio_test = termio_orig;
    termio_test.c_iflag &= ~(ICRNL | IUCLC | IXANY);
    termio_test.c_iflag |= (IXON | IXOFF);  
    termio_test.c_lflag &= ~(ICANON | ECHO);
    termio_test.c_cflag |= (CS8 | CREAD);
    termio_test.c_cc[VMIN] = 1;    
    termio_test.c_cc[VTIME] = 0;  
  
   
/* Canonical Input is ON  */
/*
   termio_test = termio_orig;
   termio_test.c_iflag |= (IXON | IXOFF);
   termio_test.c_lflag |= (ICANON);
   termio_test.c_lflag &= ~(ECHO);
   termio_test.c_cflag |= CS8; 
   termio_test.c_cc[VINTR] = _POSIX_VDISABLE;
   termio_test.c_cc[VQUIT] = _POSIX_VDISABLE;
   termio_test.c_cc[VERASE] = _POSIX_VDISABLE;
   termio_test.c_cc[VKILL] = _POSIX_VDISABLE;
   termio_test.c_cc[VEOF] = _POSIX_VDISABLE;
*/
     


/* turn on signal handlers... */
    (void) signal(SIGHUP, cleanup);
    (void) signal(SIGINT, cleanup);
    (void) signal(SIGQUIT, cleanup);
    (void) signal(SIGTERM, cleanup);

    /* set a 2 minute timeout... */
    (void) signal(SIGALRM, timeout);
    /* (void) alarm(240);  */


    /* set new terminal control attributes */
    if(tcsetattr(fd_test,TCSADRAIN,&termio_test) < 0) {
      (void) perror("tcsetattr New");
      (void) exit(1);
    }

    /* default terminal settings  */
    SPEEDTEST(B9600);
    PARITYTEST("4");                  /* 8 bit no parity */
    HANDSHAKE_COM("X");               /* IXON and IXOFF  */
    if (Echo) ECHO_COM("ON");         /* echoing on     */
    else ECHO_COM("OFF");         /* echoing off     */

}


Local boolean
CHECK_COM(uchar Test)
{
   return true;

}


Static boolean
PORT_ASSIGNMENTS_OK(uchar Test, uchar Ref, uchar Aux_A, uchar Aux_B)
{
    boolean Ok;
    
    Ok = true;       /* don't need */
    return Ok;

}


/* Local variables for START: */
struct LOC_START {
    short I, J;
} ;

/* Local variables for GET_PARAMS: */
struct LOC_GET_PARAMS {
    struct LOC_START *LINK;
} ;


/* gets name of program to use for log file name */ 
Local Char *
GET_PROG_NAME(Char *Result, struct LOC_GET_PARAMS *LINK)
{
    short Addres, I, J;
    Char Path[256];

    /* Just using argc and argv to return program name. */
    return strcpy(Result,P_argv[0]);
}


/* during pasrsing of command line. Sets the value for the 4 different
   Com ports.  Not functionally needed anymore
*/
Local boolean
PUT_NUM(uchar *Default, Char *Buffer, struct LOC_GET_PARAMS *LINK)
{
    boolean Ok;

    Ok = ((int) strlen(Buffer) == 4);
    if (!Ok)
        return Ok;
    if (Buffer[3] >= '0' && Buffer[3] <= '4')
        *Default = Buffer[3] - '0';
    else
        Ok = false;
    return Ok;
}

/* Used to grab string that will be place in global var: __Command_Line.
   This is associated with option: -C. 
*/
Local void
GET_COMMAND_LINE(short *I, Char *Current_, struct LOC_GET_PARAMS *LINK)
{
    Char Current[256];
    Char STR1[256];
    short FORLIM;

    strcpy(Current, Current_);
    strcpy(Current, strsub(STR1, Current, strpos2(Current, "\"", 1) + 1,
                           (int) strlen(Current)));
    sprintf(STR1, "%.*s", strpos2(Current, "\"", 1) - 1, Current);
    strcpy(Current, STR1);
    if (Current[0] == ' ')
        (*I)++;
    FORLIM = (int) strlen(Current);
    for (LINK->LINK->J = 2; LINK->LINK->J <= FORLIM; LINK->LINK->J++) {
        if (Current[LINK->LINK->J - 1] == ' ' && Current[LINK->LINK->J - 2] != ' ')
            (*I)++;
    }
    strcpy(__Command_Line, Current);
}

/* Parses command line, looking at each option and doing the right thing. */
Local void
GET_PARAMS(struct LOC_START *LINK)
{
    struct LOC_GET_PARAMS V;
    Char Buffer[256], Current[256];

    
    boolean Bad_Params;
    Char STR1[256];
    Char STR2[256];

    V.LINK = LINK;

    __Group_Not_Running = true;
#if 0
    sprintf(__File_Name, "%s.LOG", GET_PROG_NAME(STR1, &V));
#endif
   
    *__Command_Line = '\0';   
    
    LINK->I = 1;
    Bad_Params = false;
    while (LINK->I < P_argc && !Bad_Params) {
        strcpy(Buffer, P_argv[LINK->I]);
        if ((char) Buffer[0] != '-' || (int) strlen(Buffer) < 2) {
            Bad_Params = true;
            break;
        }
        switch (toupper(Buffer[1])) {

          case 'T':
            if (!PUT_NUM(&__default_test_term, Buffer, &V))
                Bad_Params = true;
            break;

          case 'R':
            if (!PUT_NUM(&__default_ref_term, Buffer, &V))
                Bad_Params = true;
            break;

          case 'A':
            if (!PUT_NUM(&__default_aux_a, Buffer, &V))
                Bad_Params = true;
            break;

          case 'B':
            if (!PUT_NUM(&__default_aux_b, Buffer, &V))
                Bad_Params = true;
            break;

          case 'C':
              GET_COMMAND_LINE(&LINK->I, P_argv[LINK->I], &V);
            break;

          case 'M':
            if (strlen(Buffer) == 4) {
                if (toupper(Buffer[3]) == 'Y') {
                    __Manual = true;
                }
                else if (toupper(Buffer[3]) != 'Y')
                    Bad_Params = true;
            }
            else {
                Bad_Params = true;
            }
            break;

          case 'E':
            __Log_Every_Thing = true;
            __Log_Msg = true;
            break;

          case 'L':
            __Log_Msg = true;
            if ((int) strlen(Buffer) > 3)
                strsub(__File_Name, Buffer, 4, strlen(Buffer));
            break;

          case 'D':
         /*   __Group_Not_Running = false;  */
         /*  Option 'D' is being switched to debug mode. */
            
            __DEBUG = 1;
            break;

          default:
            Bad_Params = true;
            break;
        }
        LINK->I++;
    }
    if (Bad_Params)
        MODULE_ERR("Bad parameter line.  Please see manual for information");
    if (!PORT_ASSIGNMENTS_OK(__default_test_term, __default_ref_term,
                             __default_aux_a, __default_aux_b))
        MODULE_ERR("Bad Port assignments in command line. See manual");
    sprintf(__Command_Line, "NM%s", strcpy(STR2, __Command_Line));
    if (!__Manual)
        __Command_Line[1] = 'A';
    if (__Refpres)
        __Command_Line[0] = 'R';
}

/* Initialize routine. Every program call this.  */
/* The 4 com ports are not functionally needed.  */
/*Static */void
START(uchar Test, uchar Ref, uchar Aux1, uchar Aux2, int Echo)
{


    struct LOC_START V;
    _PROCEDURE TEMP;

    __Refpres = false;
    __Aux_A_Pres = false;
    __Aux_B_Pres = false;
    if (!PORT_ASSIGNMENTS_OK(Test, Ref, Aux1, Aux2))
        MODULE_ERR("Invalid ports specified in START.  See manual for details");
    __Manual = false;

    
    __Log_Msg = false;
    __Log_Every_Thing = false;

    /* set system variables based upon input from command line options. */    
    GET_PARAMS(&V);
    if (__Log_Msg) {
        strcpy(__Message_File_NAME, __File_Name);
        if (__Message_File != NULL)
            __Message_File = freopen(__Message_File_NAME, "w", __Message_File);
        else
            __Message_File = fopen(__Message_File_NAME, "w");
        _SETIO(__Message_File != NULL, FileNotFound);
        if (P_ioresult != 0)
            MODULE_ERR("Illegal log file name specified OR not DOS 3.x");
    }
    INIT_DEFAULTS(Echo);
}


/* prints to xxx.LOG file all data read and written thru READ_COM and 
   WRITE_COM.  Triggered by option -E, __Log_EveryThing.
*/
Static void
LOG_IT(char *S, short Howmany, Char Which_Way)
{
    short I, Count;
    Char Who[256];
    Char S1[3];
    boolean Ok;
    Char STR1[256];

    Count = 1;
   
    strcpy(Who,"Terminal"); 
    switch (Which_Way) {

      case 'R':
        strcat(Who, " ==> HOST");
        break;

      case 'W':
        sprintf(Who, "HOST ==> %s", strcpy(STR1, Who));
        break;

      default:
        MODULE_ERR("Bad call to Log_it.  Bad \"which_way\"");
        break;
    }
    
 
    _SETIO(fprintf(__Message_File, "%s\n", Who) >= 0, FileWriteError);
    Ok = (P_ioresult == 0);
    
    if(fprintf(__Message_File, "%s\n",vis(S)) < 0) {
      perror("fprintf __Message_File");
    } 


}

/* have not seen a use for yet */
#define display_row     25
#define line_length     79


Local Char
READ_CHAR(void)
{

;

}


Static boolean
YESNOKEY(Char *Prompt_, boolean Default)
{
;

}

#undef display_row
#undef line_length


Static short
ASCIIINT(Char *S, short *Er)
{
    short I;

    I = 0;
    if ((int) strlen(S) > 10) {
        *Er = 0;
        return I;
    }
    *Er = (sscanf(S, "%hd", &I) == 0);
    if (*Er == 0)
        *Er = 2;
    else
        *Er = 1;
    return I;
}


Static Char *
INTASCII(Char *Result, short Num)
{
    Char St[256];

    sprintf(St, "%d", Num);
    return strcpy(Result, St);
}


Static char * 
READ_COM(int fd, char *terminator)
{
    char *c;
    char STR1[BUFSIZ];
   

    /* Based upon settings of strap G and H determine Handshake scheme. */
    /* Currently, on support 3 versions of the handshake scheme.        */
#if 0
    if(!inh_hndshk && inh_DC2)
       WRITETEST("\021");        /* default, DC1 handshake  */

    else if(inh_hndshk && !inh_DC2) {  /* DC1-DC2-DC1 handshake */
          ;

    }
    else if(inh_hndshk && inh_DC2)
       ;                               /* NO hand shake */
    else
       MODULE_ERR("ERROR, illegal combination of hand shake parameters."); 
#endif


    /* Determine termination character for read statements.              */
    /* Currently just work with auto line feed or not.                   */
/*
    if(autolf_mode)
      terminator="\n";
    else
      terminator="\r";
*/


    /* Read in data form /dev/tty. Input_Terminator is set globally */
    /* Read til get terminating char.                               */
    c=in(fd_test,STR1, sizeof(STR1), terminator);
    if(autolf_mode)
        STR1[(int) strlen(STR1) - 2] = '\0';   /* remove "CR and new line  */         
    else
        STR1[(int) strlen(STR1) - 1] = '\0';   /* just remove terminating char */
    return(STR1);
}

/* Reference terminal not supported. */
Static char * 
READREF()
{
;
}


/*Static */char * 
READTEST(char *Terminator)
{  
    char *tmp;
    char STR[BUFSIZ];
    
    tmp = READ_COM(fd_test, Terminator);
    strcpy(STR,tmp);

    if(__Log_Every_Thing)
      LOG_IT(STR, sizeof(STR),'R');
    return STR;  
}


Static short
STRCOMP(Char *S1, Char *S2)
{
    short I, J;
    boolean Still_The_Same;

    if (!strcmp(S1, S2)) {
        return 0;
    }
    else {
        I = (int) strlen(S1);
        if ((int) strlen(S2) < I)
            I = (int) strlen(S2);
        J = 1;
        Still_The_Same = true;
        while (J <= I && Still_The_Same) {
            if (S1[J - 1] != S2[J - 1])
                Still_The_Same = false;
            else
                J++;
        }
        return J;
    }
}

/* set termio flag ECHO based on user input from test script */
/* Variable "Com" is not used, it is left in to keep from modifying */
/* most tests.                                                       */

/* Currently, this function only supports a test terminal (no reference
   terminal). If reference terminal is to be supported, the correct 
   termios structure will have to be passed in.    
*/


/* ECHO_COM  used to return void, but to keep compiler happy, I
   changed to return type of int.
   Error msg was: Inconsistent type declaration: ECHO_COM          */
int
ECHO_COM(char *S_)
{   
    if(!strcmp(S_,"ON"))
      termio_test.c_lflag |= ECHO;
    else
      termio_test.c_lflag &= ~ECHO;

   
    /* set new terminal control attributes */
    if(tcsetattr(fd_test,TCSADRAIN,&termio_test) < 0) {
      (void) perror("tcsetattr: ECHO ");
      (void) exit(1);
    } 

}


Static void
INTERM_COM(uchar Com, Char *S)
{
   strcpy(Input_Terminator, S);

}


Static void
INTRIG_COM(uchar Com, Char *S)
{
    strcpy(Input_Trigger,S);

}


void
_ECHO(char *S__)
{
    uchar I=0;       /* "I" is a dummy var */ 
    ECHO_COM(S__);   
 
}


Static void
INTERM(Char *S)
{
    short I=0;
    INTERM_COM(I,S);

/*   not set up for more than 1 terminal
    for (I = 1; I <= 4; I++)
        INTERM_COM(I, S);
*/
}


Static void
INTRIG(Char *S)
{
    short I=0;
    INTRIG_COM(I,S);

/*   not set up for more than 1 terminal
    for (I = 1; I <= 4; I++)
        INTRIG_COM(I, S);
*/
}


Static void
LOGMSG(Char *S)
{
    if (__Log_Msg) {  /*$I-*/
        _SETIO(fprintf(__Message_File, "%s\n", S) >= 0, FileWriteError);
        if (P_ioresult != 0)
            MODULE_ERR("Error writing to Message File. Disk may be full");
    }
    puts(S);
}


Local boolean
GET_PARITY(Char *S_, uchar Com)
{
;
}


Static void
PARITY_COM(struct termios *term, Char *Par)
{
  switch (Par[0]) {

      case '0':
        term->c_cflag |= (PARENB | PARODD);
        break;

      case '1':
        term->c_cflag |= PARENB;
        term->c_cflag &= ~(PARODD);
        break;

      case '2':
        term->c_cflag |= PARENB;
        term->c_cflag &= ~(PARODD);
        break;

      case '3':
        term->c_cflag |= (PARENB | PARODD);
        break;

      case '4':
        term->c_cflag &= ~(PARENB);
        break;
    }
    if (Par[0] == '4')
        term->c_cflag |= CS8;
    else
        term->c_cflag |= CS7;

    /* set new terminal control attributes */
    if(tcsetattr(fd_test,TCSADRAIN,&termio_test) < 0) {
      (void) perror("tcsetattr: attempting to set parity and data bits.");
    }

}


Static void
PARITYREF(Char *Par)
{
;
}


/* PARITYTEST used to return void, but to keep compiler happy, I
   changed to return type of int.
   Error msg was: Inconsistent type declaration: PARITYTEST        */
int
PARITYTEST(Char *Par)
{
    PARITY_COM(&termio_test, Par);

    return 1;
}


Static void
GET_BAUD(Char *S_, uchar Com, short *Code)
{
;

}


Static void
SPEED_COM(struct termios *term, speed_t sp)
{

    if(cfsetospeed(term,sp) < 0) {
      (void) perror("cfsetospeed: illegal baud rate");
    }
    if(cfsetispeed(term,sp) < 0) {
      (void) perror("cfsetispeed: illegal baud rate");
    } 

    /* set new terminal control attributes */
    if(tcsetattr(fd_test,TCSADRAIN,&termio_test)) {
      (void) perror("tcsetattr: attempting to set baud rate");
    }

}


Static void
SPEEDREF(speed_t S)
{ 
;

}


/* SPEEDTEST used to return void, but to keep compiler happy, I
   changed to return type of int.
   Error msg was: Inconsistent type declaration: SPEEDTEST         */
int
SPEEDTEST(speed_t S)
{
   
   SPEED_COM(&termio_test,S);
   return 1;
 
}


Static void
TIMEOUT_COM(uchar Com, Char *S)
{
;

}


Static void
TIMEOUT(Char *S)
{
;

}


Static void
WRITE_COM(FILE *f, Char *S)
{
    if(fputs(S,f) < 0) {
      MODULE_ERR("WRITE ERROR,fputs");
      perror("fputs");
     exit(1);
    }

}


Static void
WRITEREF(Char *S_)
{
;  /* refernece terminal not supported */

}

/* WRITETEST used to return void, but to keep compiler happy, I
   changed to return type of int. 
   Error msg was: Inconsistent type declaration: WRITETEST         */
int 
WRITETEST(Char *S_)
{
    WRITE_COM(f_test, S_);
    if(__Log_Every_Thing)
      LOG_IT(S_,sizeof(buffer),'W');
    return 1;
}


/* HANDSHAKE_COM used to return void, but to keep compiler happy, I
   changed to return type of int.
   Error msg was: Inconsistent type declaration: HANDSHAKE_COM       */
/* Not used functionally */
int
HANDSHAKE_COM(char *S)
/*  HANDSHAKE_COM(uchar Com, char *S) */
{
    if (toupper(S[0]) == 'N' || toupper(S[0]) == 'B' ||
         toupper(S[0]) == 'X' || toupper(S[0]) == 'E')
        HAND_SHAKE = toupper(S[0]);
    else
        MODULE_ERR("Bad handshaking driver type in HandShake_Com");

    return 1;

}


Static void
IOCONFIG(Char *S)
{
;

}


Static void
DATACOMM_COM(uchar Com, Char *Baud, Char *Time_Out, Char *Hand,
             Char *Echo_Var, Char *Interm_Var, Char *Intrig_Var)
{
;

}


Static void
DATACOMM(Char *Baud, Char *Time_Out, Char *Hand, Char *Echo_Var,
         Char *Interm_Var, Char *Intrig_Var, boolean Refpres)
{
;

}


Static void
INITINFO(Char *S)
{
    strcpy(S, __Command_Line);
}


Static void
__setstrlen(Char *s, short l)
{
    s[l] = '\0';
}


void ding()
{

/* if (debug)
        (void) fputs(">> ding!!\n", log);
*/
    return;
}

void timeout(int sig)
{
    /* reset the signal handler... */
    if (sig)
        (void) signal(sig, timeout);

    (void) fprintf(stderr, "timeout!\n");
    (void) cleanup();
}


/* Shutdown orderly and restore terminal back to original condition. */
void cleanup(int sign)
{
    /* reset the signal handler... */
    if (sign)
        (void) signal(sign, cleanup);
    (void)fflush(__Message_File);
    
    /* restore terminal terimo... */
    if (fd_test >= 0) {
      if(tcsetattr(fd_test,TCSAFLUSH,&termio_orig) < 0) {
        (void) perror("tcsetattr Cleanup");
        return;
      }
    }


 #if 0
   /* restore terminal configuration... */
    if (f_test) {
        (void) fputs("\033X", f_test);  /* turn off format mode.. */
        (void) fputs("\033&k0B", f_test);    /* turn off block mode.  */
        (void) fputs("\033&k0A", f_test);    /* turn off autolf mode. */
    }
 #endif 

    return;
}


/* Reads from stream until the terminating char is found.  */
char *
in(int fd, char *buffer, int bufsiz, char *term)
{
    static char holdbuffer[BUFSIZ];
    static int holdbufsiz = 0;
    char *c;
    int i;

    c = buffer;
    /* copy over the hold buffer... */

#ifdef NOCODE                /* not required */
    if (holdbufsiz > 0) {
        /* copy it over... */
        (void) memcpy(buffer, holdbuffer, holdbufsiz);

        /* set it's length... */
        i = holdbufsiz;

        /* clear it... */
        holdbufsiz = 0;
    } else {
        i = 0;
    }
#endif
    
    i=0;
    while (bufsiz > 0) {
        for (; i > 0; i--, c++, bufsiz--) {
            /* look for a valid terminalter... */
            if (strchr(term, *c)) {
                /* found a terminator... */
                /* move over 1 character... */
                (void) c++;
                (void) i--;

                /* copy the rest of the string into the hold buffer... */
                if (i > 0) {
                    (void) memcpy(holdbuffer, c, i);
                    holdbufsiz = i;
                }

                /* null out the next character... */
                *c = '\0';

                /* and return string... */
                strcat(buffer, term);
                return(buffer);
            }
        }
        if ((i = read(fd, c, bufsiz)) < 0) {
            (void) perror("read");
            (void) cleanup(0);
        }

    }
}





/*  prints out all invisible and visible chars */
char
*vis(char *buf)  
{
 static char visbuffer[BUFSIZ];
    char *c;

    for (c = visbuffer; *buf; ) {
        if (*buf < 0x20) {
            *c++ = '^';
            *c++ = '@' + *buf++;
        } else if (*buf == '\\') {
            *c++ = *buf;
            *c++ = *buf++;
        } else if (*buf < 0x7f) {
            *c++ = *buf++;
        } else {
            *c++ = '\\';
            *c++ = '\0' + (*((unsigned char *) buf) >> 6) & 07;
            *c++ = '\0' + (*((unsigned char *) buf) >> 3) & 07;
            *c++ = '\0' + (*((unsigned char *) buf++) >> 0) & 07;
        }
    }

    /* null out end of buffer... */
    *c = '\0';

    return(visbuffer);
}



void FLUSHTEST()
{
   fflush(f_test);
}

#endif
