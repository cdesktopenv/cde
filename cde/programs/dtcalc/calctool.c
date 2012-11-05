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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: calctool.c /main/9 1996/09/25 11:28:16 rswiston $ */
/*									*
 *  calctool.c                                                          *
 *   Contains the none user interface portion of the Desktop            *
 *   Calculator.                                                        *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/param.h>
#include <pwd.h>
#include <math.h>
#include "patchlevel.h"
#include "calctool.h"
#include "ds_common.h"

#ifdef sun

/* Copied from math.h */
struct exception {
        int type;
        char *name;
        double arg1;
        double arg2;
        double retval;
};

#endif

time_t time() ;

double max_fix[4] = {
          6.871947674e+10, 3.245185537e+32,
          1.000000000e+36, 2.230074520e+43
} ;
double min_fix0[4] = {
          2.500000000e-1, 3.750000000e-1,
          1.000000000e-1, 4.375000000e-1
} ;
double min_fix1[4] = {
          1.250000000e-1, 4.687500000e-2,
          1.000000000e-2, 2.734375000e-2
} ;
double min_fix2[4] = {
          6.250000000e-2, 5.859375000e-3,
          1.000000000e-3, 1.708984375e-3
} ;
double min_fix3[4] = {
          3.125000000e-2, 7.324218750e-4,
          1.000000000e-4, 1.068115234e-4
} ;
double min_fix4[4] = {
          1.562500000e-2, 9.155273437e-5,
          1.000000000e-5, 6.675720215e-6
} ;
double min_fix5[4] = {
          7.812500000e-3, 1.144409180e-5,
          1.000000000e-6, 4.172325134e-7
} ;
double min_fix6[4] = {
          6.906250000e-3, 1.430511475e-6,
          1.000000000e-7, 2.607703209e-8
} ;
double min_fix7[4] = {
          1.953125000e-3, 1.788139343e-7,
          1.000000000e-8, 1.629814506e-9
} ;
double min_fix8[4] = {
          9.765625000e-4, 2.235174179e-8,
          1.000000000e-9, 1.018634066e-10
} ;
double min_fix9[4] = {
          4.882812500e-4, 2.793967724e-9,
          1.000000000e-10, 6.366462912e-12
} ;

extern char *base_str[] ;       /* Strings for each base value. */
extern char *cmdstr[] ;         /* Strings for each command line option. */
extern char *dtype_str[] ;      /* Strings for each display mode value. */
extern char *lstrs[] ;          /* Labels for various Motif items. */
extern char *mess[] ;           /* Message strings. */
extern char *mode_str[] ;       /* Strings for each mode value. */
extern char *opts[] ;           /* Command line option strings. */
extern char *ttype_str[] ;      /* Strings for each trig type value. */
extern char *ustrs[] ;          /* Usage message strings. */
extern char *vstrs[] ;          /* Various strings. */

char digits[] = "0123456789ABCDEF" ;
int basevals[4] = { 2, 8, 10, 16 } ;

int left_pos[BCOLS]  = { 3, 2, 1, 0 } ;  /* Left positions. */
int right_pos[BCOLS] = { 0, 1, 2, 3 } ;  /* "Right" positions. */

/* Valid keys when an error condition has occured. */
/*                            MEM  KEYS clr     clr     QUIT REDRAW */
char validkeys[MAXVKEYS]  = { 'm', 'k', '\177', '\013', 'q', '\f' } ;

Vars v ;            /* Calctool variables and options. */

struct menu_entry menu_entries[MAXENTRIES] ;

struct menu cmenus[MAXMENUS] = {                 /* Calculator menus. */
/*      title     total index defval                           */
  { (char *) NULL, 10,    0,    2  /* 2 places */    },    /* ACC */
  { (char *) NULL,  4,   20,    2  /* Decimal  */    },    /* BASE TYPE */
  { (char *) NULL, 10,    0,    0  /* Con. 0   */    },    /* CON */
  { (char *) NULL, 10,   10,    0  /* Reg. 0   */    },    /* EXCH */
  { (char *) NULL, 10,    0,    0  /* Fun. 0   */    },    /* FUN */
  { (char *) NULL,  4,   30,    0  /* Basic    */    },    /* MODE */
  { (char *) NULL,  3,   24,    1  /* Fixed    */    },    /* NUM TYPE */
  { (char *) NULL, 10,   10,    0  /* Reg. 0   */    },    /* RCL */
  { (char *) NULL, 10,   10,    0  /* Reg. 0   */    },    /* STO */
  { (char *) NULL,  3,   27,    0  /* Degrees  */    },    /* TRIG TYPE */
} ;

/*  This table shows the keyboard values that are currently being used:
 *
 *           | a b c d e f g h i j k l m n o p q r s t u v w x y z
 *-------------+--------------------------------------------------
 *  Control: | a   c d   f   h i     l m         r s t u       y
 *  Lower:   | a b c d e f   h i   k   m n   p q r s     v   x y
 *  Upper:   | A B C D E F G           M N   P Q R S T       X
 *  Numeric: | 0 1 2 3 4 5 6 7 8 9
 *  Other:   | @ . + - * / = % ( ) # < > [ ] { } | & ~ ^ ? ! \177
 *----------------------------------------------------------------
 */

/* Calculator button values. */

struct button buttons[TITEMS] = {
/*     str       str2      value opdisp   menutype   resname  func */

/* Row 1. */
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_FUN,    "fun",    do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_CON,    "con",    do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "keys",   do_keys    },

/* Row 2. */
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "int",    do_portion },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "frac",   do_portion },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "abs",    do_portion },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "exp",    do_expno   },

/* Row 3. */
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_ACC,    "acc",    do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_STO,    "sto",    do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_RCL,    "rcl",    do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_EXCH,   "exch",   do_pending },

/* Row 4. */
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },

/* Row 5. */ 
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },

/* Row 6. */ 
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "blank",  do_none },

/* Row 7. */
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "recip",   do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "square",  do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "sqrt",    do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "percent", do_calc   },

/* Row 8. */
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "lparen", do_paren   },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "rparen", do_paren   },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "bsp",    do_delete  },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "clr",    do_clear   },

/* Row 9. */
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "numd",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "nume",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "numf",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "chs",    do_immed   },

/* Row 10. */
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "numa",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "numb",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "numc",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "mul",    do_calc    },

/* Row 11. */
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num7",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num8",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num9",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "div",    do_calc    },

/* Row 12. */
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num4",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num5",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num6",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "sub",    do_calc    },

/* Row 13. */
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num1",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num2",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num3",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "add",    do_calc    },

/* Row 14. */
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "num0",   do_number  },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "point",  do_point   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "equals", do_calc    },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "quit",   do_frame   },

{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_BASE,   "base",   do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NUM,    "disp",   do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_MODE,   "mode",   do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_TRIG,   "trig",   do_pending },
} ; 

struct button mode_buttons[MAXMODES * MODEKEYS] = {
/*     str       str2      value opdisp   menutype   resname  func */

/* Financial. */
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "term",   do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "rate",   do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "pv",     do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "pmt",    do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "fv",     do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "ppy",    do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "clrreg", do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "ctrm",   do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "ddb",    do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "sln",    do_business},
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "syd",    do_business},
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },

/* Logical. */
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "lshift", do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "rshift", do_pending },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "and16",  do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "and32",  do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "or",     do_calc    },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "and",    do_calc    },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "not",    do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "xor",    do_calc    },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "xnor",   do_calc    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },

/* Scientific. */
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "inv",    do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "hyp",    do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "etox",   do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "tentox", do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_SET,   M_NONE,   "ytox",   do_calc    },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "xfact",  do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "cos",    do_trig    },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "sin",    do_trig    },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "tan",    do_trig    },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "ln",     do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "log",    do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_CLEAR, M_NONE,   "rand",   do_immed   },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
{ (char *)NULL, (char *)NULL, 0, OP_NOP,   M_NONE,   "",       do_none    },
} ;


int
char_val(chr)
char chr ;
{
       if (chr >= '0' && chr <= '9') return(chr - '0') ;
  else if (chr >= 'a' && chr <= 'f') return(chr - 'a' + 10) ;
  else if (chr >= 'A' && chr <= 'F') return(chr - 'A' + 10) ;
  else return(-1) ;
}


void
clear_display()
{
  int i ;
 
  v->pointed = 0 ;
  v->toclear = 1 ;
  v->defState = 1 ;
  i = 0 ;
  mpcim(&i, v->MPdisp_val) ;
  STRCPY(v->display, make_number(v->MPdisp_val, FALSE)) ;
  set_item(DISPLAYITEM, v->display) ;
  
  v->hyperbolic = 0 ;
  v->inverse    = 0 ;
  v->show_paren = 0 ;
  v->opsptr     = 0 ;            /* Clear parentheses stacks. */
  v->numsptr    = 0 ;
  set_item(HYPITEM, "    ") ;
  set_item(INVITEM, "    ") ;
}


char *
convert(line)              /* Convert .dtcalcrc line to ascii values. */
char *line ;               /* Input line to be converted. */
{
  static char output[MAXLINE] ;   /* Converted output record. */
  int ctrl = 0       ;     /* Set if we are processing a control character. */
  int i ;                  /* Position within input line. */
  int len ;
  int n = 0 ;              /* Position within output line. */
 
  len = strlen(line) ;
  for (i = 0; i < len; i++)
    {
           if (line[i] == ' ') continue ;
      else if (line[i] == '\\') ctrl = 1 ;
      else if (ctrl)
        {
          output[n++] = CTL(line[i]) ;
          ctrl = 0 ;
        }
      else output[n++] = line[i] ;
    }
  output[n] = '\0' ;
  return(output) ;
}


void
do_dtcalc(argc, argv)
int argc ;
char **argv ;
{
  char *ptr ;

  v->progname = argv[0] ;     /* Save programs name. */
  v->appname  = NULL ;
  init_cmdline_opts() ;       /* Initialise command line option strings. */

  if ((ptr = strrchr(argv[0], '/')) != NULL)
    read_str(&v->appname, ptr+1) ;
  else read_str(&v->appname, argv[0]) ;

  init_text() ;               /* Setup text strings depending upon language. */
  init_vars() ;               /* Setup default values for variables. */
  key_init() ;                /* Determine numeric function keys. */
  load_resources() ;          /* Get resources from various places. */
  read_resources() ;          /* Read resources from merged database. */
  get_options(argc, argv) ;   /* Get command line arguments. */
  if(application_args.session != NULL)
  {
      RestoreSession(); 
  }
  read_rcfiles() ;            /* Read .dtcalcrc's files. */
  init_graphics() ;
  make_frames() ;             /* Create dtcalc window frames. */

  v->shelf      = NULL ;      /* No selection for shelf initially. */
  v->noparens   = 0 ;         /* No unmatched brackets initially. */
  v->opsptr     = 0 ;         /* Nothing on the parentheses op stack. */
  v->numsptr    = 0 ;         /* Nothing on the parenthese numeric stack. */
  v->pending    = 0 ;         /* No initial pending command. */
  if(application_args.session == NULL)
     v->tstate     = 0 ;      /* Button values displayed first. */
  v->hyperbolic = 0 ;         /* Normal trig functions initially. */
  v->inverse    = 0 ;         /* No inverse functions initially. */

  srand48((long) time((time_t *) 0)) ;   /* Seed random number generator. */

  make_items() ;              /* Create server images and fir frames. */
  if(v->display[0] == 0)
     do_clear() ;                /* Initialise and clear display. */

  if (v->rstate == TRUE)      /* Show the memory register window? */
    {
      make_registers(MEM) ;
      if (!v->iconic) win_display(FCP_REG, TRUE) ;
    }
  if (!v->iconic) win_display(FCP_MODE, TRUE) ;
  show_display(v->MPdisp_val) ;     /* Output in correct display mode. */
  save_cmdline(argc, argv) ;        /* Setup dtcalc command line. */
  start_tool() ;                    /* Display the calculator. */
}


/* Dtcalc's customised math library error-handling routine. */

void
doerr(errmes)
char *errmes ;
{
  if (!v->started) return ;
  STRCPY(v->display, errmes) ;
  set_item(DISPLAYITEM, v->display) ;
  v->error = 1 ;
  beep() ;
  set_item(OPITEM, vstrs[(int) V_CLR]) ;
}


int
get_bool_resource(rtype, boolval)   /* Get boolean resource from database. */
enum res_type rtype ;
int *boolval ;
{
  char *val, tempstr[MAXLINE] ;
  int len, n ;
 
  if ((val = get_resource(rtype)) == NULL) return(0) ;
  STRCPY(tempstr, val) ;
  len = strlen(tempstr) ;
  for (n = 0; n < len; n++)
    if (isupper(tempstr[n])) tempstr[n] = tolower(tempstr[n]) ;
  if (EQUAL(tempstr, vstrs[(int) V_TRUE])) *boolval = TRUE ;
  else                                     *boolval = FALSE ;
  return(1) ;
}


/*  Get button index for given character value, setting curwin,
 *  row and column appropriately. Note that if the value isn't found,
 *  then a value of TITEMS is returned. This is "chucked out" by
 *  process_item as being invalid.
 *
 *  XXX: This routine can be improved by using a hash lookup table.
 */
 
int
get_index(ch)
char ch ;
{
  int n ;
 
  for (n = 0; n < TITEMS; n++) {
    if (ch == buttons[n].value)  
       break ;
  }
  if (n < TITEMS) 
     v->curwin = FCP_KEY ;
  else
  {
     return(TITEMS) ;
  }
  v->row = n / MAXCOLS ;
  v->column = n - (v->row * MAXCOLS) ;
  return(n) ;
}


int
get_int_resource(rtype, intval)   /* Get integer resource from database. */
enum res_type rtype ;
int *intval ;
{
  char *val ;

  if ((val = get_resource(rtype)) == NULL) return(0) ;
  *intval = atoi(val) ;
  return(1) ;
}


/* Get keyboard equivalent from first character of localised string. */

void
get_key_val(val, str)
char *val, *str ;
{
  *val = str[0] ;
}


void
get_label(n)
int n ;
{
  int val ;
  char *temp;

  val = buttons[n].value ;
  if (v->tstate)
     temp = buttons[n].str2;
  else 
     temp = buttons[n].str;

  if(temp != NULL)
     STRCPY(v->pstr, temp) ;
  else
     STRCPY(v->pstr, "");
}


void
get_options(argc, argv)      /* Extract command line options. */
int argc ;
char *argv[] ;
{
  char next[MAXLINE] ;       /* The next command line parameter. */
  char strval[MAXLINE] ;
  char *msg;
  int i, len;

  INC ;
  while (argc > 0)
    {
      if (argv[0][0] == '-' || argv[0][0] == '+')
        {
          switch (argv[0][1])
            {
              case 'D' : v->MPdebug = TRUE ;   /* MP debug info. to stderr. */
                         break ;
              case 'a' : INC ;
                         getparam(next, argv, opts[(int) O_ACCVAL]) ;
                         v->accuracy = atoi(next) ;
                         if (v->accuracy < 0 || v->accuracy > 9)
                           {
                             msg = (char *) XtMalloc(strlen(
                                                   opts[(int) O_ACCRANGE]) + 3);
                             sprintf(msg, "%s", opts[(int) O_ACCRANGE]);
                             _DtSimpleError (v->appname, DtWarning, NULL, msg);
                             XtFree(msg);
                             v->accuracy = 2 ;
                           }
                         break ;
              case 'm' : INC ;
                         msg = (char *) XtMalloc(strlen(opts[(int) O_MODE])+
                                                             strlen(next) + 3);
                         sprintf(msg, opts[(int) O_MODE], next);
                         getparam(next, argv, msg) ;
                         XtFree(msg);
                         STRCPY(strval, next) ;
                         len = strlen(strval) ;
                         for (i = 0; i < len; i++)
                         {
                           if (islower(strval[i])) 
                              strval[i] = toupper(strval[i]) ;
                         }
                         if(strcmp(strval, "FINANCIAL") == 0)
                             v->modetype = FINANCIAL ;
                         else if(strcmp(strval, "LOGICAL") == 0)
                             v->modetype = LOGICAL ;
                         else if(strcmp(strval, "SCIENTIFIC") == 0)
                             v->modetype = SCIENTIFIC ;
                         else
                           {
                             msg = (char *) XtMalloc(strlen(opts[(int) O_MODE])+
                                                    strlen(next) + 3);
                             sprintf(msg, opts[(int) O_MODE], next);
                             _DtSimpleError (v->appname, DtWarning, NULL, msg);
                             XtFree(msg);
                             v->modetype = SCIENTIFIC ;
                           }
                         break ;
              case 'b' : INC ;
                         getparam(next, argv, opts[(int) O_BASE]) ;
                         STRCPY(strval, next) ;
                         len = strlen(strval) ;
                         for (i = 0; i < len; i++)
                         {
                           if (islower(strval[i]))
                              strval[i] = toupper(strval[i]) ;
                         }
                         if(strncmp(strval, "BIN", 3) == 0)
                             v->base = BIN ;
                         else if(strncmp(strval, "OCT", 3) == 0)
                             v->base = OCT ;
                         else if(strncmp(strval, "DEC", 3) == 0)
                             v->base = DEC ;
                         else if(strncmp(strval, "HEX", 3) == 0)
                             v->base = HEX ;
                         else
                           {
                             msg = (char *) XtMalloc(strlen(
                                                    opts[(int) O_BASE]) + 3);
                             sprintf(msg, "%s", opts[(int) O_BASE]);
                             _DtSimpleError (v->appname, DtWarning, NULL, msg);
                             XtFree(msg);
                             v->base = DEC ;
                           }
                         break ;
              case 'n' : if(strcmp(&argv[0][1], "notation") == 0) 
                         {
                            INC ;
                            msg = (char *) XtMalloc(strlen(
                                                 opts[(int) O_DISPLAY]) + 
                                                 strlen(next) + 3);
                            sprintf(msg, opts[(int) O_DISPLAY], next);
                            getparam(next, argv, msg) ;
                            XtFree(msg);
                            STRCPY(strval, next) ;
                            len = strlen(strval) ;
                            for (i = 0; i < len; i++)
                            {
                              if (islower(strval[i]))
                                 strval[i] = toupper(strval[i]) ;
                            }
                            if(strncmp(strval, "FIX", 3) == 0)
                                v->dtype = FIX ;
                            else if(strncmp(strval, "ENG", 3) == 0)
                                v->dtype = ENG ;
                            else if(strncmp(strval, "SCI", 3) == 0)
                                v->dtype = SCI ;
                            else
                              {
                                msg = (char *) XtMalloc(strlen(
                                                    opts[(int) O_DISPLAY]) + 
                                                    strlen(next) + 3);
                                sprintf(msg, opts[(int) O_DISPLAY], next);
                                _DtSimpleError (v->appname, DtWarning, NULL, msg);
                                XtFree(msg);
                                v->dtype = FIX ;
                              }
                            break ;
                         }
                         else if(strcmp(&argv[0][1], "no_menu_bar") == 0)
                         {
                            INC ;
                            application_args.menuBar = False;
                            break ;
                         }
              case 't' : if(strcmp(&argv[0][1], "trig") == 0)
                         {
                            INC ;
                            msg = (char *) XtMalloc(strlen(
                                                    opts[(int) O_TRIG]) + 
                                                    strlen(next) + 3);
                            sprintf(msg, opts[(int) O_TRIG], next);
                            getparam(next, argv, msg) ;
                            XtFree(msg);
                            STRCPY(strval, next) ;
                            len = strlen(strval) ;
                            for (i = 0; i < len; i++)
                            {
                              if (islower(strval[i]))
                                 strval[i] = toupper(strval[i]) ;
                            }
                            if(strncmp(strval, "DEG", 3) == 0)
                                v->ttype = DEG ;
                            else if(strncmp(strval, "RAD", 3) == 0)
                                v->ttype = RAD ;
                            else if(strncmp(strval, "GRAD", 4) == 0)
                                v->ttype = GRAD ;
                            else
                              {
                                msg = (char *) XtMalloc(strlen(
                                                    opts[(int) O_TRIG]) + 
                                                    strlen(next) + 3);
                                sprintf(msg, opts[(int) O_TRIG], next);
                                _DtSimpleError (v->appname, DtWarning, NULL, msg);
                                XtFree(msg);
                                v->ttype = DEG ;
                              }
                            break ;
                         }
              case 's' : if(strcmp(&argv[0][1], "session") == 0)
                         {
                            INC ;
                            getparam(next, argv, opts[(int) O_SESSION]) ;
                            application_args.session = XtNewString(next);
                            break ;
                         }
              case '?' :
              case 'v' : usage(v->progname) ;
                         break ;
              default  :
              toolarg  :             /* Pick up generic tool arguments. */
                         usage(v->progname) ;
            }
          INC ;
        }
      else INC ;
    }
}


void
getparam(s, argv, errmes)
char *s, *argv[], *errmes ;
{
  char *msg;

  if (*argv != NULL && argv[0][0] != '-') STRCPY(s, *argv) ;
  else
    { 
      msg = (char *) XtMalloc(strlen(mess[(int) MESS_PARAM]) + 
                              strlen(errmes) + 3);
      sprintf(msg, mess[(int) MESS_PARAM], errmes);
      _DtSimpleError (v->appname, DtError, NULL, msg);
      FPRINTF(stderr, mess[(int) MESS_PARAM], errmes) ;
      exit(1) ;
    }
}


void
get_rcfile(name)          /* Read .dtcalcrc file. */
char *name ;
{
  char line[MAXLINE] ;    /* Current line from the .dtcalcrc file. */
  char tmp[MAXLINE] ;     /* Used to extract definitions. */
  double cval ;           /* Current constant value being converted. */
  int i ;                 /* Index to constant or function array. */
  int isval ;             /* Set to 'c' or 'f' for convertable line. */
  int len, n ;            
  FILE *rcfd ;            /* File descriptor for dtcalc rc file. */
 
  if ((rcfd = fopen(name, "r")) == NULL) return ;
 
/*  Process the .dtcalcrc file. There are currently four types of
 *  records to look for:
 *
 *  1) Those starting with a hash in the first column are comments.
 *
 *  2) Lines starting with 'c' or 'C' in the first column are
 *     definitions for constants. The cC is followed by a digit in
 *     the range 0-9, then a space. This is followed by a number
 *     in fixed or scientific notation. Following this is an optional
 *     comment, which if found, will be used in the popup menu for
 *     the constants. If the comment is present, there must be at
 *     least one space between this and the preceding number.
 *
 *  3) Those starting with a 'f' or a 'F' in the first column are
 *     definitions for functions. The fF is followed by a digit in
 *     the range 0-9, then a space. This is followed by a function
 *     definition. Following this is an optional comment, which if
 *     found, will be used in the popup menu for the functions.
 *     If the comment is present, there must be at least one space
 *     between this and the preceding function definition.
 *
 *  4) Lines starting with a 'r' or a 'R' in the first column are
 *     definitions for the initial contents of the calculators
 *     memory registers. The rR is followed by a digit in the
 *     range 0-9, then a space. This is followed by a number in
 *     fixed or scientific notation. The rest of the line is ignored.
 *
 *  All other lines are ignored.
 *
 *  Two other things to note. There should be no embedded spaces in
 *  the function definitions, and whenever a backslash is found, that
 *  and the following character signify a control character, for
 *  example \g would be ascii 7.
 */

  while (fgets(line, MAXLINE, rcfd) != NULL)
    {
      isval = 0 ;
           if (line[0] == 'c' || line[0] == 'C') isval = 'c' ;
      else if (line[0] == 'f' || line[0] == 'F') isval = 'f' ;
      else if (line[0] == 'r' || line[0] == 'R') isval = 'r' ;
      if (isval)
        if (line[1] >= '0' && line[1] <= '9' && line[2] == ' ')
          {
            i = char_val(line[1]) ;
            if (isval == 'c')
              {  
                n = sscanf(&line[3], "%lf", &cval) ;
                if (n == 1) 
                {
                   if(line[3] == '-')
                   {
                      MPstr_to_num(&line[4], DEC, v->MPcon_vals[i]) ;
                      mpneg(v->MPcon_vals[i], v->MPcon_vals[i]) ;
                   }
                   else
                      MPstr_to_num(&line[3], DEC, v->MPcon_vals[i]) ;
                }
              }  
            else if (isval == 'f')
              {
                SSCANF(&line[3], "%s", tmp) ;
                STRCPY(v->fun_vals[i], convert(tmp)) ;
              }  
            else if (isval == 'r')
              {  
                n = sscanf(&line[3], "%lf", &cval) ;
                if (n == 1) MPstr_to_num(&line[3], DEC, v->MPmvals[i]) ;
                continue ;
              }  
            len = strlen(line) ;
            for (n = 3; n < len; n++)
              if (line[n] == ' ' || line[n] == '\n')
                {
                  while (line[n] == ' ') n++ ;
                  line[strlen(line)-1] = '\0' ;
                  if (isval == 'c')
                    {
                      STRCPY(tmp, make_number(v->MPcon_vals[i], TRUE)) ;
                      SPRINTF(v->con_names[i], "%1d: %s [%s]",
                              i, tmp, &line[n]) ;
                    }
                  else
                    SPRINTF(v->fun_names[i], "%1d: %s [%s]",
                            i, tmp, &line[n]) ;
                  break ;
                }
          }
    }
  FCLOSE(rcfd) ;
}


int
get_str_resource(rtype, strval)   /* Get a string resource from database. */
enum res_type rtype ;
char *strval ;
{
  char *val ;
  int i, len ;

  if ((val = get_resource(rtype)) == NULL) return(0) ;
  STRCPY(strval, val) ;
  len = strlen(strval) ;
  if(rtype != R_TRIG && rtype != R_DISPLAY)
  {
     for (i = 0; i < len; i++)
       if (islower(strval[i])) strval[i] = toupper(strval[i]) ;
  }
  return(1) ;
}


void
grey_buttons(base)     /* Grey out numeric buttons depending upon base. */
enum base_type base ;
{
  char val ;
  int column, dim, i, n, row ;

  for (i = 0; i < 16; i++)
    {
      val = digits[i] ;
      if (isupper(val)) val = tolower(val) ;
      for (n = 0; n < TITEMS; n++)
        if (val == buttons[n].value) break ;
      row = n / MAXCOLS ;
      column = n - (row * MAXCOLS) ;

      if (i < basevals[(int) base]) dim = FALSE ;
      else                          dim = TRUE ;
      grey_button(row, column, dim) ;
    }
}


void
handle_menu_selection(n, item)   /* Process right button menu selection. */
int n, item ;
{
  if (item != -1)
    {
      if (IS_KEY(v->pending, KEY_LPAR))     /* Are we inside parentheses? */
        {
          v->current = buttons[n].value ;
          do_paren() ;
          v->current = item ;
          do_paren() ;
        }
      else
        { 
          save_pending_values(buttons[n].value) ;
          v->current = item ;
          v->ismenu = 1 ;       /* To prevent grey buttons being redrawn. */
          do_pending() ;
          v->ismenu = 0 ;
        }
    }
}


void
init_vars()    /* Setup default values for various variables. */
{
  int acc, i, n, size ;

  v->accuracy   = 2 ;        /* Initial accuracy. */
  v->base       = DEC ;      /* Initial base. */
  v->dtype      = FIX ;      /* Initial number display mode. */
  v->ttype      = DEG ;      /* Initial trigonometric type. */
  v->modetype   = SCIENTIFIC;    /* Initial calculator mode. */
  v->rstate     = 0 ;        /* No memory register frame display initially. */
  v->frstate    = 0 ;        /* No fin. memory register frame display 
                                initially. */
  v->iconic     = FALSE ;    /* Calctool not iconic by default. */
  v->MPdebug    = FALSE ;    /* No debug info by default. */
  v->MPerrors   = TRUE ;               /* No error information. */
  acc           = MAX_DIGITS + 12 ;     /* MP internal accuracy. */
  size          = MP_SIZE ;
  mpset(&acc, &size, &size) ;

  v->hasicon     = FALSE ;        /* Use standard dtcalc icon by default. */
  v->beep        = TRUE ;         /* Beep on error by default. */
  v->error       = 0 ;            /* No calculator error initially. */
  v->key_exp     = 0 ;            /* Not entering an exponent number. */
  v->pending_op  = 0 ;            /* No pending arithmetic operation. */
  v->titleline   = NULL ;         /* No User supplied title line. */
  v->display[0]  = 0 ;         

  v->x  = 0;
  v->x  = 0;
  v->width  = 0;
  v->height = 0; 

  v->workspaces = NULL; 

  read_str(&v->iconlabel, lstrs[(int) L_LCALC]) ;  /* Default icon label. */

  MPstr_to_num("0.621", DEC, v->MPcon_vals[0]) ;  /* kms/hr <=> miles/hr. */
  MPstr_to_num("1.4142135623", DEC, v->MPcon_vals[1]) ;  /* square root of 2 */
  MPstr_to_num("2.7182818284", DEC, v->MPcon_vals[2]) ;  /* e */
  MPstr_to_num("3.1415926535", DEC, v->MPcon_vals[3]) ;  /* pi */
  MPstr_to_num("2.54",         DEC, v->MPcon_vals[4]) ;  /* cms <=> inch. */
  MPstr_to_num("57.295779513", DEC, v->MPcon_vals[5]) ;  /* degrees/radian. */
  MPstr_to_num("1048576.0",    DEC, v->MPcon_vals[6]) ;  /* 2 ^ 20. */
  MPstr_to_num("0.0353", DEC, v->MPcon_vals[7]) ;  /* grams <=> ounce. */
  MPstr_to_num("0.948",  DEC, v->MPcon_vals[8]) ;  /* Kjoules <=> BTU's. */
  MPstr_to_num("0.0610", DEC, v->MPcon_vals[9]) ;  /* cms3 <=> inches3. */

  n = 0 ;
  for (i = 0; i < MAXREGS; i++) mpcim(&n, v->MPmvals[i]) ;
  for (i = 0; i < FINREGS; i++)  
     v->MPfvals[i] = (double)n;
  v->MPfvals[FINREGS - 1] = (double)12;
}


void
initialise()
{
  int i ;

  v->error         = 0 ;           /* Currently no display error. */
  v->cur_op        = '?' ;         /* No arithmetic operator defined yet. */
  v->old_cal_value = '?' ;
  i = 0 ;
  mpcim(&i, v->MPresult) ;         /* No previous result yet. */
  mpcim(&i, v->MPlast_input) ;
}


char *
make_eng_sci(MPnumber)      /* Convert engineering or scientific number. */
int *MPnumber ;
{
  char fixed[MAX_DIGITS+1], *optr ;
  int MP1[MP_SIZE], MPatmp[MP_SIZE], MPval[MP_SIZE] ;
  int MP1base[MP_SIZE], MP3base[MP_SIZE], MP10base[MP_SIZE] ;
  int i, dval, len, n ;
  int MPmant[MP_SIZE] ;        /* Mantissa. */
  int ddig ;                   /* Number of digits in exponent. */
  int eng = 0 ;                /* Set if this is an engineering number. */
  int exp = 0 ;                /* Exponent */
 
  if (v->dtype == ENG) eng = 1 ;
  optr = v->snum ;
  mpabs(MPnumber, MPval) ;
  n = 0 ;
  mpcim(&n, MP1) ;
  if (mplt(MPnumber, MP1)) *optr++ = '-' ;
  mpstr(MPval, MPmant) ;
 
  mpcim(&basevals[(int) v->base], MP1base) ;
  n = 3 ;
  mppwr(MP1base, &n, MP3base) ;
 
  n = 10 ;
  mppwr(MP1base, &n, MP10base) ;
 
  n = 1 ;
  mpcim(&n, MP1) ;
  mpdiv(MP1, MP10base, MPatmp) ;
 
  n = 0 ;
  mpcim(&n, MP1) ;
  if (!mpeq(MPmant, MP1))
    {
      while (mpge(MPmant, MP10base))
        {
          exp += 10 ;
          mpmul(MPmant, MPatmp, MPmant) ;
        }

      while ((!eng &&  mpge(MPmant, MP1base)) ||
              (eng && (mpge(MPmant, MP3base) || exp % 3 != 0)))
        {
          exp += 1 ;
          mpdiv(MPmant, MP1base, MPmant) ;
        }

      while (mplt(MPmant, MPatmp))
        {
          exp -= 10 ;
          mpmul(MPmant, MP10base, MPmant) ;
        }

      n = 1 ;
      mpcim(&n, MP1) ;
      while (mplt(MPmant, MP1) || (eng && exp % 3 != 0))
        {
          exp -= 1 ;
          mpmul(MPmant, MP1base, MPmant) ;
        }
    }    

  STRCPY(fixed, make_fixed(MPmant, MAX_DIGITS-6)) ;
  len = strlen(fixed) ;
  for (i = 0; i < len; i++) *optr++ = fixed[i] ;

  *optr++ = 'e' ;

  if (exp < 0)
    {
      exp = -exp ;
      *optr++ = '-' ;
    }
  else *optr++ = '+' ;

  MPstr_to_num("0.5", DEC, MP1) ;
  mpaddi(MP1, &exp, MPval) ;
  n = 1 ;
  mpcim(&n, MP1) ;
  for (ddig = 0; mpge(MPval, MP1); ddig++)
    mpdiv(MPval, MP1base, MPval) ;

  if (ddig == 0) *optr++ = '0' ;

  while (ddig-- > 0)
    {
      mpmul(MPval, MP1base, MPval) ;
      mpcmi(MPval, &dval) ;
      *optr++ = digits[dval] ;
      dval = -dval ;
      mpaddi(MPval, &dval, MPval) ;
    }
  *optr++    = '\0' ;
  v->toclear = 1 ;
  v->pointed = 0 ;
  return(v->snum) ;
}


char *
make_fixed(MPnumber, cmax)     /* Convert MP number to fixed number string. */
int *MPnumber ;
int cmax ;                     /* Maximum characters to generate. */
{
  char *optr ;
  int MP1base[MP_SIZE], MP1[MP_SIZE], MP2[MP_SIZE], MPval[MP_SIZE] ;
  int ndig ;                   /* Total number of digits to generate. */
  int ddig ;                   /* Number of digits to left of . */
  int dval, n ;
 
  optr = v->fnum ;
  mpabs(MPnumber, MPval) ;
  n = 0 ;
  mpcim(&n, MP1) ;
  if (mplt(MPnumber, MP1)) *optr++ = '-' ;
 
  mpcim(&basevals[(int) v->base], MP1base) ;
 
  mppwr(MP1base, &v->accuracy, MP1) ;
  MPstr_to_num("0.5", DEC, MP2) ;
  mpdiv(MP2, MP1, MP1) ;
  mpadd(MPval, MP1, MPval) ;
 
  n = 1 ;
  mpcim(&n, MP2) ;
  if (mplt(MPval, MP2))
    {
      ddig = 0 ;
      *optr++ = '0' ;
      cmax-- ;
    }
  else
    for (ddig = 0; mpge(MPval, MP2); ddig++)
      mpdiv(MPval, MP1base, MPval) ;

  ndig = MIN(ddig + v->accuracy, --cmax) ;

  while (ndig-- > 0)
    {
      if (ddig-- == 0) *optr++ = '.' ;
      mpmul(MPval, MP1base, MPval) ;
      mpcmi(MPval, &dval) ;
      *optr++ = digits[dval] ;
      dval = -dval ;
      mpaddi(MPval, &dval, MPval) ;
    }
  *optr++    = '\0' ;
  v->toclear = 1 ;
  v->pointed = 0 ;
  return(v->fnum) ;
}


void
make_items()
{
  set_item(DISPLAYITEM, v->display) ;
  set_item(OPITEM,      "    ") ;
  set_item(HYPITEM,     "    ") ;
  set_item(INVITEM,     "    ") ;
}


char *
make_number(MPnumber, mkFix)     /* Convert MP number to character string. */
int *MPnumber ;
BOOLEAN mkFix ;
{
  double number, val ;

/*  NOTE: make_number can currently set v->error when converting to a double.
 *        This is to provide the same look&feel as V3 even though dtcalc
 *        now does internal arithmetic to "infinite" precision.
 *
 *  XXX:  Needs to be improved. Shouldn't need to convert to a double in
 *        order to do these tests.
 */  
     
  mpcmd(MPnumber, &number) ;
  val = fabs(number) ;
  if (v->error) return(vstrs[(int) V_ERROR]) ;
  if (v->dtype == ENG || v->dtype == SCI ||
       v->dtype == FIX && val != 0.0 && (val > max_fix[(int) v->base]))
     return(make_eng_sci(MPnumber)) ;
  else if (v->dtype == FIX && val != 0.0 && mkFix)
  {
     if(v->accuracy == 0)
     {
        if(val <= min_fix0[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 1)
     {
        if(val <= min_fix1[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 2)
     {
        if(val <= min_fix2[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 3)
     {
        if(val <= min_fix3[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 4)
     {
        if(val <= min_fix4[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 5)
     {
        if(val <= min_fix5[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 6)
     {
        if(val <= min_fix6[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 7)
     {
        if(val <= min_fix7[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 8)
     {
        if(val <= min_fix8[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
     else if(v->accuracy == 9)
     {
        if(val <= min_fix9[(int) v->base])
           return(make_eng_sci(MPnumber)) ;
        else
           return(make_fixed(MPnumber, MAX_DIGITS)) ;
     }
  }
  else 
     return(make_fixed(MPnumber, MAX_DIGITS)) ;
}


/*ARGSUSED*/
int
matherr(exc)        /* Default math library exception handling routine. */
struct exception *exc ;
{
#if 0
  char msg[100];
  
  if (exc) {
	  strcpy(msg, exc->name);
	  strcat(msg, ": ");
	  if(exc->type == DOMAIN)
	     strcat(msg, "DOMAIN ");
	  else if(exc->type == SING)
	     strcat(msg, "SING ");
	  else if(exc->type == OVERFLOW)
	     strcat(msg, "OVERFLOW ");
	  else if(exc->type == UNDERFLOW)
	     strcat(msg, "UNDERFLOW ");
	  else if(exc->type == TLOSS)
	     strcat(msg, "TLOSS ");
	  else if(exc->type == PLOSS)
	     strcat(msg, "PLOSS ");
	  
	  strcat(msg, vstrs[(int) V_ERROR]);

	  _DtSimpleError (v->appname, DtWarning, NULL, msg);
  }
#endif

  doerr(vstrs[(int) V_ERROR]) ;
  return(1) ;                     /* Value ignored. */
}


void
MPstr_to_num(str, base, MPval)    /* Convert string into an MP number. */
char *str ;
enum base_type base ;
int *MPval ;
{
  char   *optr ;
  int MP1[MP_SIZE], MP2[MP_SIZE], MPbase[MP_SIZE] ;
  int    i, inum ;
  int    neg      = 0 ;
  int    exp      = 0 ;
  int    exp_sign = 1 ;

  i = 0 ;
  mpcim(&i, MPval) ;
  mpcim(&basevals[(int) base], MPbase) ;
  optr = str ;
  while (*optr == ' ') optr++ ;
  if(*optr == '-')
  {
     /* negative number */
     optr++ ;
     neg = 1;
  }
  while ((inum = char_val(*optr)) >= 0)
    {
      mpmul(MPval, MPbase, MPval) ;
      mpaddi(MPval, &inum, MPval) ;
      optr++ ;
    }

  if (*optr == '.')
    for (i = 1; (inum = char_val(*++optr)) >= 0; i++)
      {  
        mppwr(MPbase, &i, MP1) ;
        mpcim(&inum, MP2) ;
        mpdiv(MP2, MP1, MP1) ;
        mpadd(MPval, MP1, MPval) ;
      }
 
  while (*optr == ' ') optr++ ;

  if (*optr != '\0')
    {
      if (*optr == '-') exp_sign = -1 ;

      while ((inum = char_val(*++optr)) >= 0)
        exp = exp * basevals[(int) base] + inum ;
    }
  exp *= exp_sign ;

  if (v->key_exp || exp_sign == -1) 
    {
      mppwr(MPbase, &exp, MP1) ;
      mpmul(MPval, MP1, MPval) ;
    }
  if( neg == 1)
  {
      i = -1 ;
      mpcim(&i, MP1) ;
      mpmul(MPval, MP1, MPval) ;
  }
}


void
paren_disp(c)   /* Append the latest parenthesis char to the display item. */
char c ;
{
  int i, n ;

/*  If the character is a Delete, clear the whole line, and exit parenthesis
 *  processing.
 *
 *  If the character is a Back Space, remove the last character. If the last
 *  character was a left parenthesis, decrement the parentheses count. If
 *  the parentheses count is zero, exit parenthesis processing.
 *
 *  If the character is a control character (not Ctrl-h), then append ^(char).
 *
 *  Otherwise just append the character.
 */
 
  n = strlen(v->display) ;
  if (IS_KEY(c, KEY_CLR))             /* Is it a Delete character? */
    {
      v->noparens = v->pending = v->opsptr = v->numsptr = 0 ;
      v->cur_op = '?' ;
      set_item(OPITEM, "") ;
      i = 0 ;
      mpcim(&i, v->MPdisp_val) ;
      show_display(v->MPdisp_val) ;
      set_base(v->base);
      set_numtype(v->dtype);
      set_option_menu((int) TTYPEITEM, (int)v->ttype);
      make_modewin() ;
      v->curwin = FCP_KEY ;
      v->defState = 1;
      return ;
    }
  else if (IS_KEY(c, KEY_BSP))        /* Is is a Back Space character? */
    {
      if (!n) return ;
      if (v->display[n-1] == '(')
        {
          v->noparens-- ;
          if (!v->noparens)
            {
              v->pending = v->opsptr = v->numsptr = 0 ;
              v->cur_op = '?' ;
              set_item(OPITEM, "") ;
              show_display(v->MPdisp_val) ;
              return ;
            }
        }
      v->display[n-1] = '\0' ;
    }
  else if (c <= CTL('z'))             /* Is it a control character? */
    {
      if (n < MAXLINE-2)
        {
          v->display[n]   = '^' ;
          v->display[n+1] = c + 96 ;
          v->display[n+2] = '\0' ;
        }
    }    
  else                                /* It must be an ordinary character. */
    {
      if (n < MAXLINE-1)
        {
          v->display[n]   = c ;
          v->display[n+1] = '\0' ;
        }
    }    

  n = (n < MAX_DIGITS) ? 0 : n - MAX_DIGITS ;
  v->show_paren = 1 ;       /* Hack to get set_item to really display it. */
  set_item(DISPLAYITEM, &v->display[n]) ;
  v->show_paren = 0 ;
}


void
process_event(type)       /* Process this event. */
int type ;
{
  int ival ;

  switch (type)
    {
      case KEYBOARD_DOWN   : if (v->pending)
                               {
                                 v->current = v->cur_ch ;
                                 do_pending() ;
                               }
                             else
                               { 
                                 ival = get_index(v->cur_ch) ;
                                 if (ival < TITEMS - EXTRA) {
                                    if ((v->modetype == FINANCIAL && 
                                                                 ival == 23) ||
                                        (v->modetype == LOGICAL && (
                                                    ival == 21 || ival == 22 || 
                                                                 ival == 23)))
                                       break;
                                    else
                                       /* go arm the button */
                                       draw_button(ival, v->curwin, v->row, 
                                                   v->column, TRUE) ;
                                 }
                                 process_item(ival) ;
                               }
                             break ;

      case KEYBOARD_UP     : ival = get_index(v->cur_ch) ;
                             if (ival < TITEMS - EXTRA) {
                                 if ((v->modetype == FINANCIAL && 
                                                              ival == 23) ||
                                     (v->modetype == LOGICAL && (
                                                 ival == 21 || ival == 22 ||
                                                              ival == 23)))
                                    break;
                                 else
                                    /* go disarm the button */
                                    draw_button(ival, v->curwin, v->row, 
                                                v->column, FALSE) ;
                             }
                             /* go disarm the button */
                             draw_button(ival, v->curwin, v->row, 
                                                v->column, FALSE) ;
                             break ;
 
    }
}


void
process_item(n)
int n ;
{
  int i,isvalid ;

  if (n < 0 || n >= TITEMS)
    {
      beep() ;
      v->error = TRUE;
    }

  v->current = buttons[n].value ;
  if (v->current == '*') v->current = 'x' ;      /* Reassign "extra" values. */
  if (v->current == '\015') v->current = '=' ;
  if (v->current == 'Q') v->current = 'q' ;

  if (v->error)
    {
      isvalid = 0 ;                    /* Must press a valid key first. */
      for (i = 0; i < MAXVKEYS; i++)
        if (v->current == validkeys[i]) isvalid = 1 ;
      if (v->pending == '?') isvalid = 1 ;
      if (!isvalid) return ;
      v->error = 0 ;
    }

  if (v->pending)
    {
      if (v->pending_win == FCP_KEY) (*buttons[v->pending_n].func)() ;
      else (*mode_buttons[MODEKEYS * ((int) v->pending_mode - 1) +
            v->pending_n].func)() ;
      return ;
    }
  switch (buttons[n].opdisp)
    {
      case OP_SET   : 
         if (v->current == 'T') 
         {
             if (v->modetype == SCIENTIFIC)
                set_item(OPITEM, buttons[n].str) ;
             else
                v->pending = 0;
         }
         else
            set_item(OPITEM, buttons[n].str) ;
         break ;
      case OP_CLEAR : 
         if (v->error) 
            set_item(OPITEM, vstrs[(int) V_CLR]) ;
         else 
            set_item(OPITEM, "") ;
    }
  (*buttons[n].func)() ;

  if(strcmp(buttons[n].resname, "clr") != 0)
      v->defState = 0;
}


/* Process a portion of the parentheses stack. */

void
process_stack(startop, startnum, n)
int startop ;         /* Initial position in the operand stack. */
int startnum ;        /* Initial position in the numeric stack. */
int n ;               /* Number of items to process. */
{
  char sdisp[MAXLINE] ;     /* Used to save display contents. */
  int i ;
  int nptr ;                /* Pointer to next number from numeric stack. */

  STRCPY(sdisp, v->display) ;  /* Save current display. */
  nptr = startnum ;
  v->pending = 0 ;
  v->cur_op = '?' ;            /* Current operation is initially undefined. */
  for (i = 0; i < n; i++)
    {
      if (v->opstack[startop + i] == -1)
        {
          mpstr(v->MPnumstack[nptr++], v->MPdisp_val) ;
        }
      else
        { 
          v->cur_ch = v->opstack[startop + i] ;
          if (v->cur_ch == '^')                    /* Control character? */
            {
              i++ ;
              v->cur_ch = CTL(v->opstack[startop + i]) ;
            }
          if (v->pending)
            {
              v->current = v->cur_ch ;
              do_pending() ;
            }
          else process_item(get_index(v->cur_ch)) ;
        }
    }    
  v->numsptr = startnum ;
  push_num(v->MPdisp_val) ;
  v->opsptr = startop - 1 ;
  push_op(-1) ;
  save_pending_values(KEY_LPAR) ;
  STRCPY(v->display, sdisp) ;  /* Restore current display. */
}


void
process_str(str, mtype)
char *str ;
enum menu_type mtype ;
{
  int i, len ;
  char save[80];

  len = strlen(str) ;
  STRCPY(save, v->display) ;
  STRCPY(v->display, " ") ;
  set_item(DISPLAYITEM, v->display);
  for (i = 0 ; i < len; i++)
    {    
      if(str[i] == '*')
         str[i] = 'x';
      if (v->error) 
      {
        if(mtype == M_FUN)
        {
           STRCPY(v->display, save);
           set_item(DISPLAYITEM, v->display);
           v->error = 0;
        }
        return ;
      }
      if (v->pending)
        {
          v->current = str[i] ;
          do_pending() ;
        }
      else 
        switch(v->base)
        {
           case DEC:
              if(str[i] == 'a' ||
                 str[i] == 'b' ||
                 str[i] == 'c' ||
                 str[i] == 'd' ||
                 str[i] == 'e' ||
                 str[i] == 'f')
              {
                 v->error = True;
                 beep();
                 break;
              }
           default:
              process_item(get_index(str[i])) ;
        }
    }
}


void
read_rcfiles()   /* Read .dtcalcrc's from home and current directories. */
{
  char *home ;                  /* Pathname for users home directory. */
  char name[MAXPATHLEN + 50] ;          /* Full name of users .dtcalcrc file. */
  char pathname[MAXPATHLEN + 5] ;   /* Current working directory. */
  char tmp[MAXLINE] ;           /* For temporary constant string creation. */
  int n ;
  struct passwd *entry ;

  for (n = 0; n < MAXREGS; n++)
    {
      STRCPY(tmp, make_number(v->MPcon_vals[n], FALSE)) ;
      SPRINTF(name, "%1d: %s [%s]", n, tmp, v->con_names[n]) ;

      STRCPY(v->con_names[n], name) ;
      STRCPY(v->fun_vals[n], "") ;    /* Initially empty function strings. */
    }

  if ((home = getenv("HOME")) == NULL)
    {
      if ((entry = getpwuid(getuid())) == NULL) return ;
      home = entry->pw_dir ;
    }
  SPRINTF(name, "%s/%s", home, RCNAME) ;
  get_rcfile(name) ;      /* Read .dtcalcrc from users home directory. */
 
  SPRINTF(name, "%s/%s", getcwd(pathname, MAXPATHLEN+1), RCNAME) ;
  get_rcfile(name) ;      /* Read .dtcalcrc file from current directory. */
}


void
show_display(MPval)
int *MPval ;
{
  if (!v->error)
    {
      STRCPY(v->display, make_number(MPval, TRUE)) ;
      set_item(DISPLAYITEM, v->display) ;
    }
}


void
usage(progname)
char *progname ;
{
  FPRINTF(stderr, ustrs[(int) USAGE1], PATCHLEVEL) ;
  FPRINTF(stderr, "%s", ustrs[(int) USAGE2]) ;
  FPRINTF(stderr, "%s", ustrs[(int) USAGE3]) ;
  exit(1) ;
}

void
write_rcfile(mtype, exists, cfno, val, comment)
enum menu_type mtype ;
int exists, cfno ;
char *val, *comment ;
{
  char *home ;                  /* Pathname for users home directory. */
  char pathname[MAXPATHLEN] ;   /* Current working directory. */
  char rcname[MAXPATHLEN] ;     /* Full name of users .dtcalcrc file. */
  char str[MAXLINE] ;           /* Temporary buffer. */
  char sval[3] ;                /* Used for string comparisons. */
  char tmp_filename[MAXLINE] ;  /* Used to construct temp filename. */
  int rcexists ;                /* Set to 1, if .dtcalcrc file exists. */
  FILE *rcfd ;                  /* File descriptor for .dtcalcrc file. */
  FILE *tmpfd ;                 /* File descriptor for new temp .dtcalcrc. */
  struct passwd *entry ;        /* The user's /etc/passwd entry. */

  rcexists = 0 ;
  SPRINTF(rcname, "%s/%s", getcwd(pathname, MAXPATHLEN), RCNAME) ;
  if (access(rcname, F_OK) == 0) rcexists = 1 ;
  else
    { 
      if ((home = getenv("HOME")) == NULL)
        {
          if ((entry = getpwuid(getuid())) == NULL) return ;
          home = entry->pw_dir ;
        }
      SPRINTF(rcname, "%s/%s", home, RCNAME) ;
      if (access(rcname, F_OK) == 0) rcexists = 1 ;
    }
  STRCPY(tmp_filename, "/tmp/.dtcalcrcXXXXXX") ;
  MKTEMP(tmp_filename) ;
  if ((tmpfd = fopen(tmp_filename, "w+")) == NULL) return ;

  if (rcexists)
    {
      rcfd = fopen(rcname, "r") ;
      SPRINTF(sval, " %1d", cfno) ;
      while (fgets(str, MAXLINE, rcfd))
        {
          if (exists)
            {
              switch (mtype)
                {
                  case M_CON : sval[0] = 'c' ;
                               if (!strncmp(str, sval, 2)) FPUTS("#", tmpfd) ;
                               sval[0] = 'C' ;
                               if (!strncmp(str, sval, 2)) FPUTS("#", tmpfd) ;
                               break ;
                  case M_FUN : sval[0] = 'f' ;
                               if (!strncmp(str, sval, 2)) FPUTS("#", tmpfd) ;
                               sval[0] = 'F' ;
                               if (!strncmp(str, sval, 2)) FPUTS("#", tmpfd) ;
                }
            }    
          FPRINTF(tmpfd, "%s", str) ;
        }
      FCLOSE(rcfd) ;
    }

  switch (mtype)
    {
      case M_CON : FPRINTF(tmpfd, "\nC%1d %s %s\n", cfno, val, comment) ;
                   break ;
      case M_FUN : 
               if(strcmp(val, "") != 0)
                  FPRINTF(tmpfd, "\nF%1d %s %s\n", cfno, val, comment) ;
    }
  UNLINK(rcname) ;
  rcfd = fopen(rcname, "w") ;
  REWIND(tmpfd) ;
  while (fgets(str, MAXLINE, tmpfd)) FPRINTF(rcfd, "%s", str) ;
  FCLOSE(rcfd) ;
  FCLOSE(tmpfd);
  UNLINK(tmp_filename) ;
}


void
write_resources(filename)
char *filename;
{
  char intval[5] ;
  int MPtemp[MP_SIZE];

  SPRINTF(intval, "%d", v->accuracy) ;
  put_resource(R_ACCURACY, intval) ;
  put_resource(R_DISPLAYED, v->display) ;
  put_resource(R_BASE,     base_str[(int) v->base]) ;
  put_resource(R_DISPLAY,  dtype_str[(int) v->dtype]) ;
  put_resource(R_MODE,     mode_str[(int) v->modetype]) ;
  put_resource(R_TRIG,     ttype_str[(int) v->ttype]) ;
  put_resource(R_REGS,     set_bool(v->rstate     == TRUE)) ;

  put_resource(R_REG0,     make_number(v->MPmvals[0], FALSE)) ;
  put_resource(R_REG1,     make_number(v->MPmvals[1], FALSE)) ;
  put_resource(R_REG2,     make_number(v->MPmvals[2], FALSE)) ;
  put_resource(R_REG3,     make_number(v->MPmvals[3], FALSE)) ;
  put_resource(R_REG4,     make_number(v->MPmvals[4], FALSE)) ;
  put_resource(R_REG5,     make_number(v->MPmvals[5], FALSE)) ;
  put_resource(R_REG6,     make_number(v->MPmvals[6], FALSE)) ;
  put_resource(R_REG7,     make_number(v->MPmvals[7], FALSE)) ;
  put_resource(R_REG8,     make_number(v->MPmvals[8], FALSE)) ;
  put_resource(R_REG9,     make_number(v->MPmvals[9], FALSE)) ;

  mpcdm(&(v->MPfvals[0]), MPtemp);
  put_resource(R_FREG0,     make_number(MPtemp, FALSE)) ;
  mpcdm(&(v->MPfvals[1]), MPtemp);
  put_resource(R_FREG1,     make_number(MPtemp, FALSE)) ;
  mpcdm(&(v->MPfvals[2]), MPtemp);
  put_resource(R_FREG2,     make_number(MPtemp, FALSE)) ;
  mpcdm(&(v->MPfvals[3]), MPtemp);
  put_resource(R_FREG3,     make_number(MPtemp, FALSE)) ;
  mpcdm(&(v->MPfvals[4]), MPtemp);
  put_resource(R_FREG4,     make_number(MPtemp, FALSE)) ;
  mpcdm(&(v->MPfvals[5]), MPtemp);
  put_resource(R_FREG5,     make_number(MPtemp, FALSE)) ;

  save_resources(filename) ;
}
