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
/* $XConsortium: motif.h /main/4 1995/11/01 12:42:00 rswiston $ */
/*                                                                      *
 *  motif.h                                                             *
 *   Contains the user interface headers for the Desktop                *
 *   Calculator.                                                        *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

typedef struct Xobject {            /* Motif/Xlib graphics object. */
  Pixmap icon, iconmask ;
  Widget menubar, mainWin, kFrame;
  Widget kframe, mframe[MAXMODES], rframe, frframe, Aframe, CFframe, Pframe ;
  Widget textForm, textFrame;
  Widget kbuttons[BROWS][BCOLS];
  Widget Apanel, Api_butOK, Api_butClose, Api_butHelp ;
  Widget kkeyboard, kpanel, modeline, modeFrame, notice ;
  Widget menus[MAXMENUS], modevals[MAXITEMS], registers[MAXREGS];
  Widget fregisters[FINREGS], fregistersvals[FINREGS];
  Widget Papply, Pdefaults, Ppanel, Preset ;
  Widget CFpanel, CFpi_butOK, CFpi_butClose, CFpi_butHelp ;
  Widget popupMenu, asciiConv, enterFun, enterConst, Close;
  Widget helpDialog;               /* the help dialog box .....*/
  Widget errParent, errDialog;
  Widget baseWidgArry[4], numWidgArry[3], ttypeWidgArry[3];

  struct cW_struct *Pappearance, *Pdisplay;
  struct tW_struct *Api_text, *CFpi_cftext, *CFpi_dtext, *CFpi_vtext ;

  enum menu_type CFtype ;

  Atom clipboard ;
  Atom length_atom ;
  Display *dpy ;                    /* Display ids of dtcalc frames. */
  Window root ;

  XtAppContext app ;
  XEvent *event ;                   /* Current X event being processed. */
  XrmDatabase dtcalcDB ;            /* Dtcalc resources database. */
  XrmDatabase rDB ;                 /* Combined resources database. */
  char *home ;                      /* Pointer to user's home directory. */
  char *iconfont ;                  /* Font to use for icon label. */
  char *cfval ;                     /* Constant/Function number. */
  char *dval ;                      /* Constant/Function description. */
  char *vval ;                      /* Constant/Function value. */

  short helpMapped;

/* Data for holding information about the server's keyboard mapping. */

  int kcmin ;                       /* Minimum keycode. */
  int kcmax ;                       /* Maximum keycode. */
  int keysyms_per_key ;             /* Keysyms per keycode. */
  unsigned char *kparray ;          /* Array indicating if key is on keypad. */

  enum menu_type mtype ;
  int cfexists ;                    /* Constant/function already exists? */
  int cfno ;                        /* Current constant/function number. */ 
  int mrec[MAXMENUS] ;
  int screen ;                      /* Default graphics display screen. */
} XObject ;

typedef struct Xobject *XVars ;

XVars X ;

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif
#define CALC_ICON_NAME     CDE_INSTALLATION_TOP "/appconfig/icons/C/Dtcalc.l.pm"
#define CALC_ICON_NAME_BM  CDE_INSTALLATION_TOP "/appconfig/icons/C/Dtcalc.l.bm"

#define HELP_HELP_MENU                  710

#define HELP_INTRODUCTION               711
#define HELP_TABLEOFCONTENTS            712
#define HELP_TASKS                      713
#define HELP_REFERENCE                  714
#define HELP_ITEM                       715
#define HELP_USING                      716
#define HELP_VERSION                    717

#define HELP_ASCII                      718
#define HELP_CONSTANT                   719
#define HELP_FUNCTION                   720

#define HELP_DISPLAY                    721
#define HELP_MODELINE                   722
#define HELP_MODE                       723
#define HELP_BASE                       724
#define HELP_NOTATION                   725
#define HELP_TRIG                       726
#define HELP_MENUBAR                    727

#define HELP_USING_HELP_VOLUME          "Help4Help"
#define HELP_VOLUME                     "Calculator"

#define HELP_HELP_MENU_STR              "helpMenu"
#define HELP_INTRODUCTION_STR           "_HOMETOPIC"
#define HELP_TABLEOFCONTENTS_STR        "TableOfContents"
#define HELP_TASKS_STR                  "Tasks"
#define HELP_REFERENCE_STR              "Reference"
#define HELP_ITEM_STR                   "onItem"
#define HELP_USING_STR                  "_HOMETOPIC"
#define HELP_VERSION_STR                "_COPYRIGHT"

#define HELP_ASCII_STR			"asciiHelp"
#define HELP_CONSTANT_STR		"constantHelp"
#define HELP_FUNCTION_STR		"functionHelp"

#define HELP_DISPLAY_STR		"display"
#define HELP_MODELINE_STR 		"modeline"
#define HELP_MENUBAR_STR 		"popup"

#define HELP_BLANK_STR			"blank"
#define HELP_ACC_STR			"calcAccuracy"
#define HELP_LSHIFT_STR			"shiftleft"
#define HELP_RSHIFT_STR			"shiftright"
#define HELP_TRUNC16_STR		"trunc16"
#define HELP_TRUNC32_STR		"trunc32"
#define HELP_INT_STR			"ipercentYr"
#define HELP_PAYPYR_STR			"PperYR"
#define HELP_ETOX_STR			"e2thex"
#define HELP_TENTOX_STR			"ten2thex"
#define HELP_YTOX_STR			"y2thex"
#define HELP_XFACT_STR			"xfactorial"
#define HELP_RECIP_STR			"reciprocalx"
#define HELP_SQUARE_STR			"xsquared"
#define HELP_SQRT_STR			"calcSqrt" 
#define HELP_PERCENT_STR		"calcpercent"
#define HELP_LPAREN_STR			"leftparen"
#define HELP_RPAREN_STR			"rightparen"
#define HELP_CHGSIGN_STR		"plusorminus"
#define HELP_TIMES_STR			"times"
#define HELP_SEVEN_STR			"seven"
#define HELP_EIGHT_STR			"eight"
#define HELP_NINE_STR			"nine"
#define HELP_DIVIDE_STR			"calcdivide"
#define HELP_FOUR_STR			"four"
#define HELP_FIVE_STR			"five"
#define HELP_SIX_STR			"six"
#define HELP_MINUS_STR			"minus"
#define HELP_ONE_STR			"one"
#define HELP_TWO_STR			"two"
#define HELP_THREE_STR			"three"
#define HELP_PLUS_STR			"plus"
#define HELP_ZERO_STR			"zero"
#define HELP_DECIMAL_STR		"period"
#define HELP_EQUAL_STR			"equal"

#define NORMAL_RESTORE  0
#define HOME_RESTORE    1
#define CURRENT_RESTORE    2

