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
/* $XConsortium: text.c /main/5 1996/03/25 13:13:06 ageorge $ */
/*									*
 *  text.c                                                              *
 *   Contains all the text initializationf for the Calulator            *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <stdio.h>
#include <string.h>
#include "calctool.h"
#include "ds_common.h"

extern struct button buttons[] ;           /* Calculator button values. */
extern struct button mode_buttons[] ;      /* Special "mode" buttons. */
extern struct menu cmenus[] ;              /* Calculator menus. */
extern struct menu_entry menu_entries[] ;  /* All the menu strings. */

extern Vars v ;                 /* Calctool variables and options. */

void init_mess             P(()) ;

/*  The following are all the strings used by the dtcalc program.
 *  They are initialized in init_text() to the local language equivalents.
 */

char *base_str[MAXBASES] ;
char *calc_res[MAXRESOURCES] ;
char *cmdstr[MAXCMDS] ;
char *dtype_str[MAXDISPMODES] ;
char *lstrs[MAXLABELS] ;
char *mess[MAXMESS] ;
char *mode_str[MAXMODES] ;
char *mpstrs[MAXMPERRS] ;
char *opts[MAXOPTS] ;
char *pstrs[MAXPSTRS] ;
char *ttype_str[MAXTRIGMODES] ;
char *ustrs[MAXUSAGE] ;
char *vstrs[MAXVMESS] ;


void
init_cmdline_opts(void)      /* Initialize command line option strings. */
{
  cmdstr[(int) CMD_ACC]      = "-a" ;
  cmdstr[(int) CMD_MODE]      = "-m" ;
  cmdstr[(int) CMD_BASE]      = "-b" ;
  cmdstr[(int) CMD_TRIG]      = "-trig" ;
  cmdstr[(int) CMD_DISP]      = "-notation" ;
  cmdstr[(int) CMD_MENU_BAR]      = "-no_menu_bar" ;
  cmdstr[(int) CMD_SESSION]      = "-session" ;
}


void
init_text(void)   /* Setup text strings depending upon language. */
{
  int i ;

  init_mess();
/*  Some notes for translators:
 *
 *  Text in the label strings below, should be left justified. It will
 *  automatically be centre justified in the buttons, but various
 *  calculations depend on the text starting on the left.
 *
 *  The original English V3 version was written with fixed (upto) four
 *  character button labels. I've tried to extend this to allow any sized
 *  labels, however the following conditions apply:
 *
 *  - If the label won't fit in the button, then as much as possible will be
 *    shown, with the last character being '>'. If you enlarge the calculator,
 *    probably more of the label will show.
 *
 *  - If the last character of the label is a '.', and this is not the first
 *    character of the label (ie, the numeric point label), then dtcalc
 *    knows that this signifies that this label should have two more '.'
 *    characters appended to it (ie, "Mem." becomes "Mem...").
 *
 *  - If a dtcalc button has a menu item glyph associated with it, the
 *    width of this glyph is taken into consideration, and the amount of
 *    the label that can be displayed is adjusted accordingly.
 */

  i = 0 ;
  read_str(&buttons[i++].str, LGET("")) ;                          /* Row 1.*/
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 5, "Functions"))) ; 
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 6, "Constants"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 7, "Keys"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 8, "Int"))) ;     /* Row 2. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 9, "Frac"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 10, "Abs"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 11, "Exp"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 12, "Accuracy"))) ; /* Row 3.*/
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 13, "Store"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 14, "Recall"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 15, "Exchange"))) ;

  read_str(&buttons[i++].str, LGET("")) ;      /* Row 4. */
  read_str(&buttons[i++].str, LGET("")) ;
  read_str(&buttons[i++].str, LGET("")) ;
  read_str(&buttons[i++].str, LGET("")) ;

  read_str(&buttons[i++].str, LGET("")) ;      /* Row 5. */
  read_str(&buttons[i++].str, LGET("")) ;
  read_str(&buttons[i++].str, LGET("")) ;
  read_str(&buttons[i++].str, LGET("")) ;

  read_str(&buttons[i++].str, LGET("")) ;      /* Row 6. */
  read_str(&buttons[i++].str, LGET("")) ;
  read_str(&buttons[i++].str, LGET("")) ;
  read_str(&buttons[i++].str, LGET("")) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 16, "1/x"))) ;    /* Row 7. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 17, "x^2"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 19, "Sqrt"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 18, "%"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 20, "("))) ;      /* Row 8. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 21, ")"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 22, "Bsp"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 23, "Clr"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 24, "D"))) ;      /* Row 9. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 25, "E"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 26, "F"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 27, "+/-"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 28, "A"))) ;      /* Row 10. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 29, "B"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 30, "C"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 31, "X"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 32, "7"))) ;      /* Row 11. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 33, "8"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 34, "9"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 35, "/"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 36, "4"))) ;      /* Row 12. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 37, "5"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 38, "6"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 39, "-"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 40, "1"))) ;      /* Row 13. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 41, "2"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 42, "3"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 43, "+"))) ;

  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 44, "0"))) ;      /* Row 14. */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 45, "."))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 46, "="))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 47, "Quit"))) ;

  /* the three extra defs */
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 48, "Base"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 49, "Disp"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 50, "Mode"))) ;
  read_str(&buttons[i++].str, LGET(GETMESSAGE(3, 51, "Trig"))) ;

  i = 0 ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 52, "F - Funcs"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 53, "# - Const"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 54, "k - Keys"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 55, "I - Int"))) ; 

/* CTL('f') */
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 56, "^f - Frac"))) ; 

/* CTL('u') */
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 57, "^u - Abs"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 58, "E - Exp"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 59, "A - Acc"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 60, "S - Sto"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 61, "R - Rcl"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 62, "X - Xchg"))) ;

  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;

  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;

  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;
  read_str(&buttons[i++].str2, LGET("")) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 63, "r - 1/x"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 64, "@ - x^2"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 66, "s - Sqrt"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 65, "%"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 67, "("))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 68, ")"))) ;

/* CTL('h') */
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 69, "^h - Bsp"))) ;

/* del */
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 70, "del - Clr"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 71, "d"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 72, "e"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 73, "f"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 74, "C - +/-"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 75, "a"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 76, "b"))) ; 
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 77, "c"))) ; 
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 78, "x"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 79, "7"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 80, "8"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 81, "9"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 82, "/"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 83, "4"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 84, "5"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 85, "6"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 86, "-"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 87, "1"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 88, "2"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 89, "3"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 90, "+"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 91, "0"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 92, "."))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 93, "="))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 94, "q - Quit"))) ;

  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 95, "B"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 96, "D"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 97, "M"))) ;
  read_str(&buttons[i++].str2, LGET(GETMESSAGE(3, 98, "T"))) ;

  i = 0 ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 452, "F"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 453, "#"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 454, "k"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 455, "I"))) ; 

/* CTL('f') */
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 456, "\006"))) ; 

/* CTL('u') */
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 457, "\025"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 458, "E"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 459, "A"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 460, "S"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 461, "R"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 462, "X"))) ;

  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;

  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;

  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;
  get_key_val(&buttons[i++].value, LGET("")) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 463, "r"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 464, "@"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 466, "s"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 465, "%"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 67, "("))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 68, ")"))) ;

/* CTL('h') */
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 467, "\010"))) ;

/* del */
#ifdef hpux
  if(v->keybdID == HIL_ITF_KBD)
     get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 513, "\013"))) ;
  else
     get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 468, "\177"))) ;
#else
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 468, "\177"))) ;
#endif


  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 469, "d"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 470, "e"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 471, "f"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 472, "C"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 473, "a"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 474, "b"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 475, "c"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 476, "x"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 79, "7"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 80, "8"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 81, "9"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 82, "/"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 83, "4"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 84, "5"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 85, "6"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 86, "-"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 87, "1"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 88, "2"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 89, "3"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 90, "+"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 91, "0"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 92, "."))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 93, "="))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 477, "q"))) ;

  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 95, "B"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 96, "D"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 97, "M"))) ;
  get_key_val(&buttons[i++].value, LGET(GETMESSAGE(3, 98, "T"))) ;

  calc_res[(int) R_ACCURACY] = DGET("accuracy") ;
  calc_res[(int) R_BASE]     = DGET("base") ;
  calc_res[(int) R_DISPLAY]  = DGET("display") ;
  calc_res[(int) R_MODE]     = DGET("mode") ;
  calc_res[(int) R_REGS]     = DGET("showRegisters") ;
  calc_res[(int) R_TRIG]     = DGET("trigType") ;
  calc_res[(int) R_BEEP]     = DGET("beep") ;
  calc_res[(int) R_ICON]     = DGET("iconify") ;
  calc_res[(int) R_WORKSPACE]  = DGET("workspace") ;
  calc_res[(int) R_WIDTH]    = DGET("width") ;
  calc_res[(int) R_HEIGHT]   = DGET("height") ;
  calc_res[(int) R_X]        = DGET("x") ;
  calc_res[(int) R_Y]        = DGET("y") ;
  calc_res[(int) R_DISPLAYED]  = DGET("displayed") ;
  calc_res[(int) R_REG0]     = DGET("register0") ;
  calc_res[(int) R_REG1]     = DGET("register1") ;
  calc_res[(int) R_REG2]     = DGET("register2") ;
  calc_res[(int) R_REG3]     = DGET("register3") ;
  calc_res[(int) R_REG4]     = DGET("register4") ;
  calc_res[(int) R_REG5]     = DGET("register5") ;
  calc_res[(int) R_REG6]     = DGET("register6") ;
  calc_res[(int) R_REG7]     = DGET("register7") ;
  calc_res[(int) R_REG8]     = DGET("register8") ;
  calc_res[(int) R_REG9]     = DGET("register9") ;
  calc_res[(int) R_FREG0]    = DGET("fregister0") ;
  calc_res[(int) R_FREG1]    = DGET("fregister1") ;
  calc_res[(int) R_FREG2]    = DGET("fregister2") ;
  calc_res[(int) R_FREG3]    = DGET("fregister3") ;
  calc_res[(int) R_FREG4]    = DGET("fregister4") ;
  calc_res[(int) R_FREG5]    = DGET("fregister5") ;
  calc_res[(int) R_MENUBAR]  = DGET("postMenuBar") ;
  calc_res[(int) R_KEYS]     = DGET("keys") ;

  STRCPY(v->con_names[0], LGET(GETMESSAGE(3, 119, "kilometers per hour <=> miles per hour."))) ;
  STRCPY(v->con_names[1], LGET(GETMESSAGE(3, 120, "square root of 2."))) ;
  STRCPY(v->con_names[2], LGET(GETMESSAGE(3, 121, "e."))) ;
  STRCPY(v->con_names[3], LGET(GETMESSAGE(3, 122, "pi."))) ;
  STRCPY(v->con_names[4], LGET(GETMESSAGE(3, 123, "centimeters <=> inch."))) ;
  STRCPY(v->con_names[5], LGET(GETMESSAGE(3, 124, "degrees in a radian."))) ;
  STRCPY(v->con_names[6], LGET(GETMESSAGE(3, 125, "2 ^ 20."))) ;
  STRCPY(v->con_names[7], LGET(GETMESSAGE(3, 126, "grams <=> ounce."))) ;
  STRCPY(v->con_names[8], LGET(GETMESSAGE(3, 127, "kilojoules <=> British thermal units."))) ;
  STRCPY(v->con_names[9], LGET(GETMESSAGE(3, 128, "cubic centimeters <=> cubic inches."))) ;

/* Keyboard equivalents for the dtcalc menu entries. */

          i = 0 ;
/* 00 */  get_key_val(&menu_entries[i++].val, LGET("0")) ;  /* ACC */
/* 01 */  get_key_val(&menu_entries[i++].val, LGET("1")) ;
/* 02 */  get_key_val(&menu_entries[i++].val, LGET("2")) ;
/* 03 */  get_key_val(&menu_entries[i++].val, LGET("3")) ;
/* 04 */  get_key_val(&menu_entries[i++].val, LGET("4")) ;
/* 05 */  get_key_val(&menu_entries[i++].val, LGET("5")) ;
/* 06 */  get_key_val(&menu_entries[i++].val, LGET("6")) ;
/* 07 */  get_key_val(&menu_entries[i++].val, LGET("7")) ;
/* 08 */  get_key_val(&menu_entries[i++].val, LGET("8")) ;
/* 09 */  get_key_val(&menu_entries[i++].val, LGET("9")) ;
/* 10 */  get_key_val(&menu_entries[i++].val, LGET("0")) ;  /* EXCH, RCL, STO */
/* 11 */  get_key_val(&menu_entries[i++].val, LGET("1")) ;
/* 12 */  get_key_val(&menu_entries[i++].val, LGET("2")) ;
/* 13 */  get_key_val(&menu_entries[i++].val, LGET("3")) ;
/* 14 */  get_key_val(&menu_entries[i++].val, LGET("4")) ;
/* 15 */  get_key_val(&menu_entries[i++].val, LGET("5")) ;
/* 16 */  get_key_val(&menu_entries[i++].val, LGET("6")) ;
/* 17 */  get_key_val(&menu_entries[i++].val, LGET("7")) ;
/* 18 */  get_key_val(&menu_entries[i++].val, LGET("8")) ;
/* 19 */  get_key_val(&menu_entries[i++].val, LGET("9")) ;
/* 35 */  get_key_val(&menu_entries[i++].val, LGET("b")) ;  /* BASE */
/* 36 */  get_key_val(&menu_entries[i++].val, LGET("o")) ;
/* 37 */  get_key_val(&menu_entries[i++].val, LGET("d")) ;
/* 38 */  get_key_val(&menu_entries[i++].val, LGET("h")) ;
/* 39 */  get_key_val(&menu_entries[i++].val, LGET("e")) ;  /* Display type. */
/* 40 */  get_key_val(&menu_entries[i++].val, LGET("f")) ;
/* 41 */  get_key_val(&menu_entries[i++].val, LGET("s")) ;
/* 42 */  get_key_val(&menu_entries[i++].val, LGET("d")) ;  /* Trig. type. */
/* 43 */  get_key_val(&menu_entries[i++].val, LGET("g")) ;
/* 44 */  get_key_val(&menu_entries[i++].val, LGET("r")) ;
/* 45 */  get_key_val(&menu_entries[i++].val, LGET("b")) ;  /* MODE */
/* 46 */  get_key_val(&menu_entries[i++].val, LGET("f")) ;
/* 47 */  get_key_val(&menu_entries[i++].val, LGET("l")) ;
/* 48 */  get_key_val(&menu_entries[i++].val, LGET("s")) ;
/* 49 */  get_key_val(&menu_entries[i++].val, LGET("\020")) ;

  i = 0 ;
/* Financial */
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 208, "Term"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 209, "i%Yr"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 210, "Pv"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 211, "Pmt"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 212, "Fv"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 515, "P/YR"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 213, "CLRfregs"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 214, "Ctrm"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 215, "Ddb"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 216, "Sln"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 217, "Syd"))) ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;

/* Logical. */
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 218, "<"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 219, ">"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 220, "&16"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 221, "&32"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 222, "Or"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 223, "And"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 224, "Not"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 225, "Xor"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 226, "Xnor"))) ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;

/* Scientific. */
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 228, "Inv"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 227, "Hyp"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 229, "e^x"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 230, "10^x"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 231, "y^x"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 232, "x!"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 233, "Cos"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 234, "Sin"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 235, "Tan"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 236, "Ln"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 237, "Log"))) ;
  read_str(&mode_buttons[i++].str, LGET(GETMESSAGE(3, 238, "Rand"))) ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;
  read_str(&mode_buttons[i++].str, " ") ;

/* Keyboard equivalents for the three dtcalc mode window buttons. */

  i = 0 ;
/* Financial */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 239, "T - Term"))) ;
/* CTL('r') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 240, "^r - i%Yr"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 241, "p - Pv"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 242, "P - Pmt"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 243, "v - Fv"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 516, "y - P/YR"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 244, "L - Clrgs"))) ;
/* CTL('t') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 245, "^t - Ctrm"))) ;
/* CTL('d') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 246, "^d - Ddb")));
/* CTL('s') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 247, "^s - Sln")));
/* CTL('y') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 248, "^y - Syd")));
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;

/* Logical. */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 249, "<"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 250, ">"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 251, "[ - &16"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 252, "] - &32"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 253, "| - Or"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 254, "& - And"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 255, "~ - Not"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 256, "^ - Xor"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 257, "n - Xnor"))) ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;

/* Scientific. */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 259, "i - Inv"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 258, "h - Hyp"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 260, "{ - e^x"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 261, "} - 10^x"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 262, "y - y^x"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 263, "! - x!"))) ;
/* CTL('c') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 264, "^c - Cos")));
/* CTL('s') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 265, "^s - Sin")));
/* CTL('t') */
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 266, "^t - Tan")));
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 267, "N - Ln"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 268, "G - Log"))) ;
  read_str(&mode_buttons[i++].str2, LGET(GETMESSAGE(3, 269, "? - Rand"))) ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;
  read_str(&mode_buttons[i++].str2, " ") ;

  i = 0 ;
/* Financial */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 480, "T"))) ;
/* CTL('r') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 481, "\022"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 482, "p"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 483, "P"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 484, "v"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(5, 517, "y"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 485, "L"))) ;
/* CTL('t') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 486, "\024"))) ;
/* CTL('d') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 487, "\004"))) ;
/* CTL('s') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 488, "\023"))) ;
/* CTL('y') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 489, "\031"))) ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;

/* Logical. */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 249, "<"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 250, ">"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 490, "["))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 491, "]"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 492, "|"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 493, "&"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 494, "~"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 495, "^"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 496, "n"))) ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;

/* Scientific. */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 501, "i"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 500, "h"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 502, "{"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 503, "}"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 504, "y"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 505, "!"))) ;
/* CTL('c') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 506, "\003"))) ;     
/* CTL('s') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 507, "\023"))) ;
/* CTL('t') */
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 508, "\024"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 509, "N"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 510, "G"))) ;
  get_key_val(&mode_buttons[i++].value, LGET(GETMESSAGE(3, 511, "?"))) ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;
  get_key_val(&mode_buttons[i++].value, " ") ;

}

void
init_mess(void)   /* Setup text strings depending upon language. */
{
  int i ;

  base_str[(int) BIN] = XtNewString(LGET(GETMESSAGE(2, 1, "Bin"))) ;  /* Base values. */
  base_str[(int) OCT] = XtNewString(LGET(GETMESSAGE(2, 2, "Oct"))) ;
  base_str[(int) DEC] = XtNewString(LGET(GETMESSAGE(2, 3, "Dec"))) ;
  base_str[(int) HEX] = XtNewString(LGET(GETMESSAGE(2, 4, "Hex"))) ;

  i = 0 ;
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 107, "Accuracy"))) ;         /* ACC */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 108, "Numeric base"))) ;  /* BASE TYPE */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 109, "Constants"))) ;        /* CON */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 110, "Exchange"))) ;         /* EXCH */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 111, "Functions"))) ;        /* FUN */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 113, "Mode"))) ;             /* MODE */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 114, "Display type"))) ;   /* NUM TYPE */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 115, "Retrieve"))) ;         /* RCL */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 117, "Store"))) ;            /* STO */
  cmenus[i++].title = XtNewString(LGET(GETMESSAGE(3, 118, "Trigonometric type"))) ;/* TRIG TYPE */

/* Display mode. */
  dtype_str[(int) ENG] = XtNewString(LGET(GETMESSAGE(2, 5, "ENG"))) ;
  dtype_str[(int) FIX] = XtNewString(LGET(GETMESSAGE(2, 6, "FIX"))) ;
  dtype_str[(int) SCI] = XtNewString(LGET(GETMESSAGE(2, 7, "SCI"))) ;

/* Labels for various Motif items. */
  lstrs[(int) L_CONNO]   = XtNewString(LGET(GETMESSAGE(3, 132, "Constant no:"))) ;
  lstrs[(int) L_FUNNO]   = XtNewString(LGET(GETMESSAGE(3, 133, "Function no:"))) ;
  lstrs[(int) L_LCALC]   = XtNewString(LGET(GETMESSAGE(3, 134, "calculator"))) ;
  lstrs[(int) L_NEWCON]  = XtNewString(LGET(GETMESSAGE(3, 135, "New Constant"))) ;
  lstrs[(int) L_NEWFUN]  = XtNewString(LGET(GETMESSAGE(3, 136, "New Function"))) ;
  lstrs[(int) L_UCALC]   = XtNewString(LGET(GETMESSAGE(2, 12, "Calculator"))) ;
  lstrs[(int) L_MEMT]    = XtNewString(LGET(GETMESSAGE(3, 137, "Memory Registers"))) ;
  lstrs[(int) L_FINMEMT] = XtNewString(LGET(GETMESSAGE(3, 518, "Financial Registers"))) ;
  lstrs[(int) L_PROPT]   = XtNewString(LGET(GETMESSAGE(3, 139, "Calculator properties"))) ;
  lstrs[(int) L_DESC]    = XtNewString(LGET(GETMESSAGE(3, 140, "Description:"))) ;
  lstrs[(int) L_VALUE]   = XtNewString(LGET(GETMESSAGE(3, 141, "Value:"))) ;
  lstrs[(int) L_CONB]    = XtNewString(LGET(GETMESSAGE(3, 142, "Enter Constant"))) ;
  lstrs[(int) L_FUNB]    = XtNewString(LGET(GETMESSAGE(3, 143, "Enter Function"))) ;

/* Message strings. */
  mess[(int) MESS_PARAM] = XtNewString(MGET(GETMESSAGE(3, 148, "%s\n"))) ;
  mess[(int) MESS_CON]   = XtNewString(MGET(GETMESSAGE(3, 149, "%s %1d already exists."))) ;
          i = 0 ;
/* ACC */
/* 00 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 153, "0 radix places"))) ;
/* 01 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 154, "1 radix places"))) ;
/* 02 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 155, "2 radix places"))) ;
/* 03 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 156, "3 radix places"))) ;
/* 04 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 157, "4 radix places"))) ;
/* 05 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 158, "5 radix places"))) ;
/* 06 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 159, "6 radix places"))) ;
/* 07 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 160, "7 radix places"))) ;
/* 08 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 161, "8 radix places"))) ;
/* 09 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 162, "9 radix places"))) ;

/* EXCH, RCL, STO */
/* 10 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 163, "Register 0"))) ;
/* 11 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 164, "Register 1"))) ;
/* 12 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 165, "Register 2"))) ;
/* 13 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 166, "Register 3"))) ;
/* 14 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 167, "Register 4"))) ;
/* 15 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 168, "Register 5"))) ;
/* 16 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 169, "Register 6"))) ;
/* 17 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 170, "Register 7"))) ;
/* 18 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 171, "Register 8"))) ;
/* 19 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 172, "Register 9"))) ;

/* BASE */
/* 20 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 188, "Binary"))) ;      
/* 21 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 189, "Octal"))) ;
/* 22 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 190, "Decimal"))) ;
/* 23 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 191, "Hexadecimal"))) ;

/* Display type. */
/* 24 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 192, "Engineering"))) ; 
/* 25 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 193, "Fixed point"))) ;
/* 26 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 194, "Scientific"))) ;

/* Trig. type. */
/* 27 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 195, "Degrees"))) ;     
/* 28 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 196, "Gradients"))) ;
/* 29 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 197, "Radians"))) ;

/* MODE */
/* 30 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 198, "Basic"))) ;       
/* 31 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 199, "Financial"))) ;
/* 32 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 200, "Logical"))) ;
/* 33 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 201, "Scientific"))) ;

/* 34 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 203, "Term:"))) ;
/* 35 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 204, "i%Yr:"))) ;
/* 36 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 205, "Present Value:"))) ;
/* 37 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 206, "Payment:"))) ;
/* 38 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 207, "Future Value:"))) ;
/* 39 */  menu_entries[i++].str = XtNewString(LGET(GETMESSAGE(3, 514, "Payments/Year:"))) ;

  mode_str[(int) FINANCIAL]  = XtNewString(LGET(GETMESSAGE(3, 270, "FINANCIAL"))) ;
  mode_str[(int) LOGICAL]    = XtNewString(LGET(GETMESSAGE(3, 271, "LOGICAL"))) ;
  mode_str[(int) SCIENTIFIC] = XtNewString(LGET(GETMESSAGE(3, 272, "SCIENTIFIC"))) ;

/* MP errors (visible with the -E command line option. */

  mpstrs[(int) MP_ADD2A] = XtNewString(LGET(GETMESSAGE(3, 273, "*** SIGN NOT 0, +1 OR -1 IN MPADD2 CALL.\n"))) ;
  mpstrs[(int) MP_ADD2B] = XtNewString(LGET(GETMESSAGE(3, 274, "POSSIBLE OVERWRITING PROBLEM ***\n"))) ;
  mpstrs[(int) MP_PART1] = XtNewString(LGET(GETMESSAGE(3, 275, "*** N .LE. 1 IN CALL TO MPART1 ***\n"))) ;
  mpstrs[(int) MP_ASIN]  = XtNewString(LGET(GETMESSAGE(3, 276, "*** ABS(X) .GT. 1 IN CALL TO MPASIN ***\n"))) ;
  mpstrs[(int) MP_ATAN]  = XtNewString(LGET(GETMESSAGE(3, 277, "*** ERROR OCCURRED IN MPATAN, RESULT INCORRECT ***\n"))) ;
  mpstrs[(int) MP_CHKC]  = XtNewString(MGET(GETMESSAGE(3, 278, "*** B = %d ILLEGAL IN CALL TO MPCHK.\n"))) ;
  mpstrs[(int) MP_CHKD]  = XtNewString(LGET(GETMESSAGE(3, 279, "PERHAPS NOT SET BEFORE CALL TO AN MP ROUTINE ***\n"))) ;
  mpstrs[(int) MP_CHKE]  = XtNewString(MGET(GETMESSAGE(3, 280, "*** T = %d ILLEGAL IN CALL TO MPCHK.\n"))) ;
  mpstrs[(int) MP_CHKF]  = XtNewString(LGET(GETMESSAGE(3, 279, "PERHAPS NOT SET BEFORE CALL TO AN MP ROUTINE ***\n"))) ;
  mpstrs[(int) MP_CHKG]  = XtNewString(LGET(GETMESSAGE(3, 282, "*** M .LE. T IN CALL TO MPCHK.\n"))) ;
  mpstrs[(int) MP_CHKH]  = XtNewString(LGET(GETMESSAGE(3, 279, "PERHAPS NOT SET BEFORE CALL TO AN MP ROUTINE ***\n"))) ;
  mpstrs[(int) MP_CHKI]  = XtNewString(LGET(GETMESSAGE(3, 284, "*** B TOO LARGE IN CALL TO MPCHK ***\n"))) ;
  mpstrs[(int) MP_CHKJ]  = XtNewString(LGET(GETMESSAGE(3, 285, "*** MXR TOO SMALL OR NOT SET TO DIM(R) BEFORE CALL "))) ;
  mpstrs[(int) MP_CHKL]  = XtNewString(LGET(GETMESSAGE(3, 286, "TO AN MP ROUTINE ***\n"))) ;
  mpstrs[(int) MP_CHKM]  = XtNewString(MGET(GETMESSAGE(3, 287, "*** MXR SHOULD BE AT LEAST %d*T + %d = %d  ***\n"))) ;
  mpstrs[(int) MP_CHKN]  = XtNewString(MGET(GETMESSAGE(3, 288, "*** ACTUALLY MXR = %d, AND T = %d  ***\n"))) ;
  mpstrs[(int) MP_CMD]   = XtNewString(LGET(GETMESSAGE(3, 289, "*** FLOATING-POINT OVER/UNDER-FLOW IN MPCMD ***\n"))) ;
  mpstrs[(int) MP_CMR]   = XtNewString(LGET(GETMESSAGE(3, 290, "*** FLOATING-POINT OVER/UNDER-FLOW IN MPCMR ***\n"))) ;
  mpstrs[(int) MP_CQM]   = XtNewString(LGET(GETMESSAGE(3, 291, "*** J = 0 IN CALL TO MPCQM ***\n"))) ;
  mpstrs[(int) MP_DIVA]  = XtNewString(LGET(GETMESSAGE(3, 292, "*** ATTEMPTED DIVISION BY ZERO IN CALL TO MPDIV ***\n"))) ;
  mpstrs[(int) MP_DIVB]  = XtNewString(LGET(GETMESSAGE(3, 293, "*** OVERFLOW OCCURRED IN MPDIV ***\n"))) ;
  mpstrs[(int) MP_DIVIA] = XtNewString(LGET(GETMESSAGE(3, 294, "*** ATTEMPTED DIVISION BY ZERO IN CALL TO MPDIVI ***\n"))) ;
  mpstrs[(int) MP_DIVIB] = XtNewString(LGET(GETMESSAGE(3, 295, "*** INTEGER OVERFLOW IN MPDIVI, B TOO LARGE ***\n"))) ;
  mpstrs[(int) MP_EXPA]  = XtNewString(LGET(GETMESSAGE(3, 296, "*** OVERFLOW IN SUBROUTINE MPEXP ***\n"))) ;
  mpstrs[(int) MP_EXPB]  = XtNewString(LGET(GETMESSAGE(3, 297, "*** ERROR OCCURRED IN MPEXP, RESULT INCORRECT ***\n"))) ;
  mpstrs[(int) MP_EXP1]  = XtNewString(LGET(GETMESSAGE(3, 298, "*** ABS(X) NOT LESS THAN 1 IN CALL TO MPEXP1 ***\n"))) ;
  mpstrs[(int) MP_LNA]   = XtNewString(LGET(GETMESSAGE(3, 299, "*** X NONPOSITIVE IN CALL TO MPLN ***\n"))) ;
  mpstrs[(int) MP_LNB]   = XtNewString(LGET(GETMESSAGE(3, 300, "*** ERROR IN MPLN, ITERATION NOT CONVERGING ***\n"))) ;
  mpstrs[(int) MP_LNSA]  = XtNewString(LGET(GETMESSAGE(3, 301, "*** ABS(X) .GE. 1/B IN CALL TO MPLNS ***\n"))) ;
  mpstrs[(int) MP_LNSB]  = XtNewString(LGET(GETMESSAGE(3, 302, "*** ERROR OCCURRED IN MPLNS.\n"))) ;
  mpstrs[(int) MP_LNSC]  = XtNewString(LGET(GETMESSAGE(3, 303, "NEWTON ITERATION NOT CONVERGING PROPERLY ***\n"))) ;
  mpstrs[(int) MP_MULA]  = XtNewString(LGET(GETMESSAGE(3, 304, "*** INTEGER OVERFLOW IN MPMUL, B TOO LARGE ***\n"))) ;
  mpstrs[(int) MP_MULB]  = XtNewString(LGET(GETMESSAGE(3, 305, "*** ILLEGAL BASE B DIGIT IN CALL TO MPMUL.\n"))) ;
  mpstrs[(int) MP_MULC]  = XtNewString(LGET(GETMESSAGE(3, 306, "POSSIBLE OVERWRITING PROBLEM ***\n"))) ;
  mpstrs[(int) MP_MUL2A] = XtNewString(LGET(GETMESSAGE(3, 307, "*** OVERFLOW OCCURRED IN MPMUL2 ***\n"))) ;
  mpstrs[(int) MP_MUL2B] = XtNewString(LGET(GETMESSAGE(3, 308, "*** INTEGER OVERFLOW IN MPMUL2, B TOO LARGE ***\n"))) ;
  mpstrs[(int) MP_MULQ]  = XtNewString(LGET(GETMESSAGE(3, 309, "*** ATTEMPTED DIVISION BY ZERO IN MPMULQ ***\n"))) ;
  mpstrs[(int) MP_NZRA]  = XtNewString(LGET(GETMESSAGE(3, 310, "*** SIGN NOT 0, +1 OR -1 IN CALL TO MPNZR.\n"))) ;
  mpstrs[(int) MP_NZRB]  = XtNewString(LGET(GETMESSAGE(3, 311, "POSSIBLE OVERWRITING PROBLEM ***\n"))) ;
  mpstrs[(int) MP_NZRC]  = XtNewString(LGET(GETMESSAGE(3, 312, "*** OVERFLOW OCCURRED IN MPNZR ***\n"))) ;
  mpstrs[(int) MP_OVFL]  = XtNewString(LGET(GETMESSAGE(3, 313, "*** CALL TO MPOVFL, MP OVERFLOW OCCURRED ***\n"))) ;
  mpstrs[(int) MP_PI]    = XtNewString(LGET(GETMESSAGE(3, 314, "*** ERROR OCCURRED IN MPPI, RESULT INCORRECT ***\n"))) ;
  mpstrs[(int) MP_PWRA]  = XtNewString(LGET(GETMESSAGE(3, 315, "*** ATTEMPT TO RAISE ZERO TO NEGATIVE POWER IN\n"))) ;
  mpstrs[(int) MP_PWRB]  = XtNewString(LGET(GETMESSAGE(3, 316, "CALL TO SUBROUTINE MPPWR ***\n"))) ;
  mpstrs[(int) MP_PWR2A] = XtNewString(LGET(GETMESSAGE(3, 317, "*** X NEGATIVE IN CALL TO MPPWR2 ***\n"))) ;
  mpstrs[(int) MP_PWR2B] = XtNewString(LGET(GETMESSAGE(3, 318, "*** X ZERO AND Y NONPOSITIVE IN CALL TO MPPWR2 ***\n"))) ;
  mpstrs[(int) MP_RECA]  = XtNewString(LGET(GETMESSAGE(3, 319, "*** ATTEMPTED DIVISION BY ZERO IN CALL TO MPREC ***\n"))) ;
  mpstrs[(int) MP_RECB]  = XtNewString(LGET(GETMESSAGE(3, 320, "*** ERROR OCCURRED IN MPREC, NEWTON ITERATION\n"))) ;
  mpstrs[(int) MP_RECC]  = XtNewString(LGET(GETMESSAGE(3, 321, "NOT CONVERGING PROPERLY ***\n"))) ;
  mpstrs[(int) MP_RECD]  = XtNewString(LGET(GETMESSAGE(3, 322, "*** OVERFLOW OCCURRED IN MPREC ***\n"))) ;
  mpstrs[(int) MP_ROOTA] = XtNewString(LGET(GETMESSAGE(3, 323, "*** N = 0 IN CALL TO MPROOT ***\n"))) ;
  mpstrs[(int) MP_ROOTB] = XtNewString(LGET(GETMESSAGE(3, 324, "*** ABS(N) TOO LARGE IN CALL TO MPROOT ***\n"))) ;
  mpstrs[(int) MP_ROOTC] = XtNewString(LGET(GETMESSAGE(3, 325, "*** X = 0 AND N NEGATIVE IN CALL TO MPROOT ***\n"))) ;
  mpstrs[(int) MP_ROOTD] = XtNewString(LGET(GETMESSAGE(3, 326, "*** X NEGATIVE AND N EVEN IN CALL TO MPROOT ***\n"))) ;
  mpstrs[(int) MP_ROOTE] = XtNewString(LGET(GETMESSAGE(3, 327, "*** ERROR OCCURRED IN MPROOT, NEWTON ITERATION\n"))) ;
  mpstrs[(int) MP_ROOTF] = XtNewString(LGET(GETMESSAGE(3, 328, "NOT CONVERGING PROPERLY ***\n"))) ;
  mpstrs[(int) MP_SETB]  = XtNewString(LGET(GETMESSAGE(3, 329, "*** IDECPL .LE. 0 IN CALL TO MPSET ***\n"))) ;
  mpstrs[(int) MP_SETC]  = XtNewString(LGET(GETMESSAGE(3, 330, "ITMAX2 TOO SMALL IN CALL TO MPSET ***\n"))) ;
  mpstrs[(int) MP_SETD]  = XtNewString(LGET(GETMESSAGE(3, 331, "*** INCREASE ITMAX2 AND DIMENSIONS OF MP ARRAYS \n"))) ;
  mpstrs[(int) MP_SETE]  = XtNewString(MGET(GETMESSAGE(3, 332, "TO AT LEAST %d ***\n"))) ;
  mpstrs[(int) MP_SIN]   = XtNewString(LGET(GETMESSAGE(3, 333, "*** ERROR OCCURRED IN MPSIN, RESULT INCORRECT ***\n"))) ;
  mpstrs[(int) MP_SIN1]  = XtNewString(LGET(GETMESSAGE(3, 334, "*** ABS(X) .GT. 1 IN CALL TO MPSIN1 ***\n"))) ;
  mpstrs[(int) MP_SQRT]  = XtNewString(LGET(GETMESSAGE(3, 335, "*** X NEGATIVE IN CALL TO SUBROUTINE MPSQRT ***\n"))) ;
  mpstrs[(int) MP_TAN]   = XtNewString(LGET(GETMESSAGE(3, 336, "*** TAN(X) TOO LARGE IN CALL TO MPTAN ***\n"))) ;

/* Command line options. */

  opts[(int) O_ACCVAL]   = XtNewString(LGET(GETMESSAGE(3, 337, "Calculator: -a needs accuracy value of 0-9."))) ;
  opts[(int) O_ACCRANGE] = XtNewString(MGET(GETMESSAGE(3, 338, "Calculator: accuracy should be in the range 0-9\nWill set accuracy to the default: 2.\n")));
  opts[(int) O_BASE]     = XtNewString(MGET(GETMESSAGE(3, 339, "Calculator: base should be binary, octal, decimal or hexadecimal.\nWill set base to the default: decimal.\n"))) ;
  opts[(int) O_DISPLAY]  = XtNewString(MGET(GETMESSAGE(3, 340, "Calculator: invalid display mode [%s]\n"))) ;
  opts[(int) O_MODE]     = XtNewString(MGET(GETMESSAGE(3, 341, "Calculator: invalid mode [%s]\nWill set mode to its default: scientific\n"))) ;
  opts[(int) O_TRIG]     = XtNewString(MGET(GETMESSAGE(3, 342, "Calculator: invalid trig. mode [%s]\n"))) ;
  opts[(int) O_SESSION]     = XtNewString(MGET(GETMESSAGE(3, 343, "Calculator: invalid session file.\n"))) ;

  pstrs[(int) P_CHAR]   = XtNewString(LGET(GETMESSAGE(3, 351, "Character:"))) ;
  pstrs[(int) P_ASCIIT] = XtNewString(LGET(GETMESSAGE(3, 352, "Get ASCII"))) ;

/* Trig. type values. */
  ttype_str[(int) DEG]      = XtNewString(LGET(GETMESSAGE(3, 353, "Deg"))) ;
  ttype_str[(int) GRAD]     = XtNewString(LGET(GETMESSAGE(3, 354, "Grad"))) ;
  ttype_str[(int) RAD]      = XtNewString(LGET(GETMESSAGE(3, 355, "Rad"))) ;

/* Usage message. */
  ustrs[(int) USAGE1] = XtNewString(MGET(GETMESSAGE(3, 356, "Calculator (dtcalc) version 1.%1d\n\n"))) ;
  ustrs[(int) USAGE2] = XtNewString(MGET(GETMESSAGE(3, 357, "Usage: dtcalc [-a accuracy] [-m mode] [-b base] [-no_menu_bar] [-?]\n"))) ;
  ustrs[(int) USAGE3] = XtNewString(LGET(GETMESSAGE(3, 358, "\t [-notation display_notation] [-trig trignometric_type] [-session session_file]\n"))) ;

/* Various strings. */
  vstrs[(int) V_CANCEL]   = XtNewString(LGET(GETMESSAGE(3, 361, "Cancel"))) ;
  vstrs[(int) V_CONFIRM]  = XtNewString(LGET(GETMESSAGE(3, 362, "Confirm"))) ;
  vstrs[(int) V_CONTINUE] = XtNewString(LGET(GETMESSAGE(3, 363, "Continue"))) ;
  vstrs[(int) V_CONWNAME] = XtNewString(LGET(GETMESSAGE(3, 142, "Enter Constant"))) ;
  vstrs[(int) V_ERROR]    = XtNewString(LGET(GETMESSAGE(3, 364, "Error"))) ;
  vstrs[(int) V_FUNWNAME] = XtNewString(LGET(GETMESSAGE(3, 143, "Enter Function"))) ;
  vstrs[(int) V_CLR]      = XtNewString(LGET(GETMESSAGE(3, 370, "CLR"))) ;
  vstrs[(int) V_HYP]      = XtNewString(LGET(GETMESSAGE(3, 371, "HYP"))) ;
  vstrs[(int) V_INV]      = XtNewString(LGET(GETMESSAGE(3, 372, "INV"))) ;
  vstrs[(int) V_INVCON]   = XtNewString(LGET(GETMESSAGE(3, 373, "Invalid constant value"))) ;
  vstrs[(int) V_LCON]     = XtNewString(LGET(GETMESSAGE(3, 374, "Invalid constant number."))) ;
  vstrs[(int) V_LFUN]     = XtNewString(LGET(GETMESSAGE(3, 375, "Invalid function number."))) ;
  vstrs[(int) V_NOCHANGE] = XtNewString(LGET(GETMESSAGE(3, 377, "Constant not changed."))) ;
  vstrs[(int) V_NUMSTACK] = XtNewString(LGET(GETMESSAGE(3, 378, "Numeric stack error"))) ;
  vstrs[(int) V_OPSTACK]  = XtNewString(LGET(GETMESSAGE(3, 379, "Operand stack error"))) ;
  vstrs[(int) V_OWRITE]   = XtNewString(LGET(GETMESSAGE(3, 380, "Okay to overwrite?"))) ;
  vstrs[(int) V_RANGE]    = XtNewString(LGET(GETMESSAGE(3, 381, "Must be in the range 0 - 9"))) ;
  vstrs[(int) V_TRUE]     = LGET("true") ;
  vstrs[(int) V_UCON]     = XtNewString(LGET(GETMESSAGE(3, 383, "Constant"))) ;
  vstrs[(int) V_UFUN]     = XtNewString(LGET(GETMESSAGE(3, 384, "Function"))) ;
  vstrs[(int) V_NORSAVE]  = XtNewString(LGET(GETMESSAGE(3, 385, "Unable to save defaults.\n"))) ;

}
