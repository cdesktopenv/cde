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
/* $XConsortium: functions.c /main/6 1996/09/25 09:36:28 mustafa $ */
/*									*
 *  functions.c                                                         *
 *   Contains the many of the functions (i.e. do_*) which actually do   *
 *   (at least start) the calculations.                                 *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "calctool.h"

extern char *base_str[] ;       /* Strings for each base value. */
extern char *dtype_str[] ;      /* Strings for each display mode value. */
extern char *mode_str[] ;       /* Strings for each mode value. */
extern char *ttype_str[] ;      /* Strings for each trig type value. */
extern char *vstrs[] ;          /* Various strings. */

extern struct button buttons[] ;           /* Calculator button values. */
extern struct button mode_buttons[] ;      /* Special "mode" buttons. */
extern struct menu_entry menu_entries[] ;  /* All the menu strings. */

extern Vars v ;                 /* Calctool variables and options. */

double mods[] = { 1.0, 1.0e-1, 1.0e-2, 1.0e-3, 1.0e-4,
                  1.0e-5, 1.0e-6, 1.0e-7, 1.0e-8, 1.0e-9,
                  1.0e-10, 1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14,
                  1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19 };

static void compute_i(double *target);
static int count_sign_changes(double *cf, int count);


void
do_accuracy(void)     /* Set display accuracy. */
{
  int i ;

  for (i = ACC_START; i <= ACC_END; i++)
    if (v->current == menu_entries[i].val)
      {
        v->accuracy = char_val(v->current) ;
        make_registers(MEM) ;
        make_registers(FIN) ;
        return ;
      }
}


void
do_ascii(void)        /* Convert ASCII value. */
{
  show_ascii_frame() ;
}


void
do_base(void)    /* Change the current base setting. */
{
       if (v->current == BASE_BIN) v->base = BIN ;
  else if (v->current == BASE_OCT) v->base = OCT ;
  else if (v->current == BASE_DEC) v->base = DEC ;
  else if (v->current == BASE_HEX) v->base = HEX ;
  else return ;

  set_base(v->base) ;
}

void
set_base(enum base_type base)
{
  grey_buttons(v->base) ;
  show_display(v->MPdisp_val) ;
  set_option_menu((int) BASEITEM, (int)v->base);
  v->pending = 0 ;
  if (v->rstate) make_registers(MEM) ;
  if (v->frstate) make_registers(FIN) ;
}

void
do_business(void)     /* Perform special business mode calculations. */
{
  Boolean need_show = TRUE;
  char *display_number = NULL;
  int MPbv[MP_SIZE], MP1[MP_SIZE], MP2[MP_SIZE], MP3[MP_SIZE], MP4[MP_SIZE] ;
  int i, len, val, accSav ;
  double result, w;

  if (IS_KEY(v->current, KEY_CTRM))
    {
/*  Cterm - FMEM1 = int (periodic interest rate).
 *          FMEM2 = Pv  (present value).
 *          FMEM4 = Fv  (future value).
 *
 *          RESULT = log(FMEM4 / FMEM2) / log(1 + FMEM1)
 */
      if(v->MPfvals[1] == 0.0 || v->MPfvals[2] == 0.0 || v->MPfvals[4] == 0.0)
      {
           char *errorMsg, *tmp;

           /* want to undraw the button first */
           draw_button(19, 0, 4, 3, FALSE);
           errorMsg = GETMESSAGE(5, 7, "Can't calculate 'Compound Term'\nwithout a non zero interest rate,\na non zero Present value, and\na non zero Future value.\n");
           tmp = XtNewString(errorMsg);
           ErrorDialog(tmp);
           XtFree(tmp);
      }
      else
      {
         v->error = False;
         result = log(v->MPfvals[4] / v->MPfvals[2]) / log(1.0 + (v->MPfvals[1] / 1200));
         if(!v->error)
         {
            mpcdm(&result, v->MPdisp_val) ;
            make_registers(FIN) ;
            v->funstate = 1;
         }
      }
    }
  else if (IS_KEY(v->current, KEY_DDB))
    {

/*  Ddb   - MEM0 = cost    (amount paid for asset).
 *          MEM1 = salvage (value of asset at end of its life).
 *          MEM2 = life    (useful life of the asset).
 *          MEM3 = period  (time period for depreciation allowance).
 *
 *          bv = 0.0 ;
 *          for (i = 0; i < MEM3; i++)
 *            {
 *              VAL = ((MEM0 - bv) * 2) / MEM2
 *              bv += VAL
 *            }
 *          RESULT = VAL
 */

      i = 0 ;
      mpcim(&i, MPbv) ;
      mpcmi(v->MPmvals[3], &len) ;
      for (i = 0; i < len; i++)
        {
          mpsub(v->MPmvals[0], MPbv, MP1) ;
          val = 2 ;
          mpmuli(MP1, &val, MP2) ;
          mpdiv(MP2, v->MPmvals[2], v->MPdisp_val) ;
          mpstr(MPbv, MP1) ;
          mpadd(MP1, v->MPdisp_val, MPbv) ;
        }
    }
  else if (IS_KEY(v->current, KEY_FV))
    {

/*  Fv    - FMEM3 = pmt (periodic payment).
 *          FMEM1 = int (periodic interest rate).
 *          FMEM2 = Pv  (present value).
 *          FMEM0 = n   (number of periods).
 *
 */
      if(v->MPfvals[0] == 0.0 || v->MPfvals[1] == 0.0 ||
             (v->MPfvals[2] == 0.0 && v->MPfvals[3] == 0.0) || v->funstate == 0)
      {
          if(v->funstate == 1)
          {
             v->funstate = 0;
             doerr(GETMESSAGE(5, 2, "ERROR: No Solution"));
             return;
          }
          else
             /* set FV register */
             mpcmd(v->MPdisp_val, &(v->MPfvals[4]));
      }
      else
      {
         if ((w = 1.0 + v->MPfvals[1] / (v->MPfvals[5] * 100.0)) == 1.0)
             result = -(v->MPfvals[2] + v->MPfvals[0] * v->MPfvals[3]);
         else
             result =  -(v->MPfvals[2] * pow(w, v->MPfvals[0]) +
                       v->MPfvals[3] * (pow(w, v->MPfvals[0]) - 1.0) *
                       pow(w, 0.0) / (w - 1.0));
         mpcdm(&result, v->MPdisp_val) ;
      }
      if(strcmp(v->display, GETMESSAGE(3, 364, "Error")) != 0)
         mpcmd(v->MPdisp_val, &(v->MPfvals[4]));
      make_registers(FIN) ;
      v->funstate = 1;
    }
  else if (IS_KEY(v->current, KEY_PMT))
    {

/*  Pmt   - FMEM0 = prin (principal).
 *          FMEM1 = int  (periodic interest rate).
 *          FMEM2 = n    (term).
 *
 *          RESULT = FMEM0 * (FMEM1 / (1 - pow(FMEM1 + 1, -1 * FMEM2)))
 */

      if(v->MPfvals[0] == 0.0 || v->MPfvals[1] == 0.0 ||
           (v->MPfvals[2] == 0.0 && v->MPfvals[4] == 0.0) || v->funstate == 0)
      {
          if(v->funstate == 1)
          {
             v->funstate = 0;
             doerr(GETMESSAGE(5, 2, "ERROR: No Solution"));
             return;
          }
          else
             /* set Payment register */
             mpcmd(v->MPdisp_val, &(v->MPfvals[3]));
      }
      else
      {

         if ((w = 1.0 + v->MPfvals[1] / (v->MPfvals[5] * 100.0)) == 1.0)
             result = -(v->MPfvals[4] + v->MPfvals[2]) / v->MPfvals[0];
         else
             result = -(v->MPfvals[2] * pow(w, v->MPfvals[0]) +
                        v->MPfvals[4]) * (w - 1.0) /
                        ((pow(w, v->MPfvals[0]) - 1.0) * pow(w, 0.0));
         mpcdm(&result, v->MPdisp_val) ;
      }
      if(strcmp(v->display, GETMESSAGE(3, 364, "Error")) != 0)
         mpcmd(v->MPdisp_val, &(v->MPfvals[3]));
      make_registers(FIN) ;
      v->funstate = 1;
    }
  else if (IS_KEY(v->current, KEY_PV))
    {

/*  Pv    - FMEM0 = pmt (periodic payment).
 *          FMEM1 = int (periodic interest rate).
 *          FMEM2 = n   (term).
 *
 *          RESULT = FMEM0 * (1 - pow(1 + FMEM1, -1 * FMEM2)) / FMEM1
 */

      if(v->MPfvals[0] == 0.0 || v->MPfvals[1] == 0.0 || v->MPfvals[3] == 0.0 ||                v->funstate == 0)
      {
          if(v->funstate == 1)
          {
             v->funstate = 0;
             doerr(GETMESSAGE(5, 2, "ERROR: No Solution"));
             return;
          }
          else
             /* set PV register */
             mpcmd(v->MPdisp_val, &(v->MPfvals[2]));
      }
      else
      {
         if ((w = 1.0 + v->MPfvals[1] / (v->MPfvals[5] * 100.0)) == 1.0)
             result = -(v->MPfvals[4] + v->MPfvals[0] * v->MPfvals[3]);
         else
             result =  -(v->MPfvals[4] / pow(w, v->MPfvals[0]) +
                         v->MPfvals[3] * (pow(w, v->MPfvals[0]) - 1.0) *
                         pow(w, 0.0 - v->MPfvals[0]) / (w - 1.0));
         mpcdm(&result, v->MPdisp_val) ;
      }
      if(strcmp(v->display, GETMESSAGE(3, 364, "Error")) != 0)
         mpcmd(v->MPdisp_val, &(v->MPfvals[2]));
      make_registers(FIN) ;
      v->funstate = 1;
    }
  else if (IS_KEY(v->current, KEY_RATE))
    {
/*  Rate  - MEM0 = fv (future value).
 *          MEM1 = pv (present value).
 *          MEM2 = n  (term).
 *
 *          RESULT = pow(MEM0 / MEM1, 1 / MEM2) - 1
 */
      if(v->MPfvals[0] == 0.0 || (v->MPfvals[2] == 0.0 && v->MPfvals[3] == 0.0)
                              || (v->MPfvals[3] == 0.0 && v->MPfvals[4] == 0.0)
                              || v->funstate == 0)
      {
          if(v->funstate == 1)
          {
             v->funstate = 0;
             doerr(GETMESSAGE(5, 2, "ERROR: No Solution"));
             return;
          }
          else
          {
             accSav = v->accuracy;
             v->accuracy = 2;
             display_number = make_number(v->MPdisp_val, FALSE);
             MPstr_to_num(display_number, DEC, v->MPdisp_val);
             /* set RATE register */
             mpcmd(v->MPdisp_val, &(v->MPfvals[1]));
             v->accuracy = accSav;
          }
      }
      else
      {
          compute_i(&(v->MPfvals[1]));
          mpcdm(&(v->MPfvals[1]), v->MPdisp_val);
          accSav = v->accuracy;
          v->accuracy = 2;
          display_number = make_number(v->MPdisp_val, FALSE);
          MPstr_to_num(display_number, DEC, v->MPdisp_val);
          v->accuracy = accSav;
      }

      if(!v->error)
          make_registers(FIN) ;
      v->funstate = 1;

      STRCPY(v->display, display_number);
      set_item(DISPLAYITEM, v->display);
      need_show = FALSE;
    }
  else if (IS_KEY(v->current, KEY_SLN))
    {

/*  Sln   - MEM0 = cost    (cost of the asset).
 *          MEM1 = salvage (salvage value of the asset).
 *          MEM2 = life    (useful life of the asset).
 *
 *          RESULT = (MEM0 - MEM1) / MEM2
 */

      mpsub(v->MPmvals[0], v->MPmvals[1], MP1) ;
      mpdiv(MP1, v->MPmvals[2], v->MPdisp_val) ;
    }
  else if (IS_KEY(v->current, KEY_SYD))
    {

/*  Syd   - MEM0 = cost    (cost of the asset).
 *          MEM1 = salvage (salvage value of the asset).
 *          MEM2 = life    (useful life of the asset).
 *          MEM3 = period  (period for which depreciation is computed).
 *
 *          RESULT = ((MEM0 - MEM1) * (MEM2 - MEM3 + 1)) /
 *                   (MEM2 * (MEM2 + 1) / 2)
 */

      mpsub(v->MPmvals[2], v->MPmvals[3], MP2) ;
      val = 1 ;
      mpaddi(MP2, &val, MP3) ;
      mpaddi(v->MPmvals[2], &val, MP2) ;
      mpmul(v->MPmvals[2], MP2, MP4) ;
      val = 2 ;
      mpcim(&val, MP2) ;
      mpdiv(MP4, MP2, MP1) ;
      mpdiv(MP3, MP1, MP2) ;
      mpsub(v->MPmvals[0], v->MPmvals[1], MP1) ;
      mpmul(MP1, MP2, v->MPdisp_val) ;
    }
  else if (IS_KEY(v->current, KEY_TERM))
    {

/*  Term  - FMEM0 = pmt (periodic payment).
 *          FMEM1 = fv  (future value).
 *          FMEM2 = int (periodic interest rate).
 *
 *          RESULT = log(1 + (FMEM1 * FMEM2 / FMEM0)) / log(1 + FMEM2)
 */

      if(v->MPfvals[1] == 0.0 || (v->MPfvals[2] == 0.0 && v->MPfvals[4] == 0)
                              || v->MPfvals[3] == 0.0 || v->funstate == 0)
      {
          if(v->funstate == 1)
          {
             v->funstate = 0;
             doerr(GETMESSAGE(5, 2, "ERROR: No Solution"));
             return;
          }
          else
             /* set Term register */
             mpcmd(v->MPdisp_val, &(v->MPfvals[0]));
      }
      else
      {
         if ((w = 1.0 + v->MPfvals[1] / (v->MPfvals[5] * 100.0)) == 1.0)
             result = -(v->MPfvals[4] + v->MPfvals[2]) / v->MPfvals[3];
         else
         {
             double wdb = pow(w, 0.0);

             result = log((v->MPfvals[3] * wdb / (w - 1.0) - v->MPfvals[4]) /
                          (v->MPfvals[2] * pow(w, 0.0) + v->MPfvals[3] * wdb /
                          (w - 1.0))) / log(w);
         }

         if(strcmp(v->display, GETMESSAGE(3, 364, "Error")) != 0)
            mpcdm(&result, v->MPdisp_val) ;
      }
      if(strcmp(v->display, GETMESSAGE(3, 364, "Error")) != 0)
         mpcmd(v->MPdisp_val, &(v->MPfvals[0]));
      make_registers(FIN) ;
      v->funstate = 1;
    }
  else if (IS_KEY(v->current, KEY_PYR))
    {
      mpcmd(v->MPdisp_val, &(v->MPfvals[5]));
      result = do_round(v->MPfvals[5], 0);
      if (result < 1.0)
         v->MPfvals[5] = 1.0;
      else
         v->MPfvals[5] = result;
      make_registers(FIN) ;
      v->funstate = 1;
    }
  else if (IS_KEY(v->current, KEY_FCLR))
    {
       int zero = 0;

       mpcim(&zero, MP1) ;

       /* clear Term register */
       mpcmd(MP1, &(v->MPfvals[0])) ;

       /* clear %/YR register */
       mpcmd(MP1, &(v->MPfvals[1])) ;

       /* clear PV register */
       mpcmd(MP1, &(v->MPfvals[2])) ;

       /* clear Payment register */
       mpcmd(MP1, &(v->MPfvals[3])) ;

       /* clear FV register */
       mpcmd(MP1, &(v->MPfvals[4])) ;

       zero = 12;
       mpcim(&zero, MP1) ;
       mpcmd(MP1, &(v->MPfvals[5])) ;

       make_registers(FIN);
    }

  if (need_show == TRUE)
      show_display(v->MPdisp_val) ;

  return;
}


void
do_calc(void)      /* Perform arithmetic calculation and display result. */
{
  double dval, dres ;
  int MP1[MP_SIZE] ;

  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

  if (!(v->opsptr && !v->show_paren)) {  /* Don't do if processing parens. */
    if (IS_KEY(v->current, KEY_EQ) && IS_KEY(v->old_cal_value, KEY_EQ)) {
      if (v->new_input) {
         mpstr(v->MPdisp_val, v->MPresult) ;
      } else {
         mpstr(v->MPlast_input, v->MPdisp_val) ;
      }
    }
  }

  if (!IS_KEY(v->current, KEY_EQ) && IS_KEY(v->old_cal_value, KEY_EQ))
    v->cur_op = '?' ;

  if (IS_KEY(v->cur_op, KEY_COS) ||                           /* Cos */
      IS_KEY(v->cur_op, KEY_SIN) ||                           /* Sin */
      IS_KEY(v->cur_op, KEY_TAN) ||                           /* Tan */
      v->cur_op == '?')                                 /* Undefined */
    mpstr(v->MPdisp_val, v->MPresult) ;

  else if (IS_KEY(v->cur_op, KEY_ADD))                  /* Addition */
    mpadd(v->MPresult, v->MPdisp_val, v->MPresult) ;

  else if (IS_KEY(v->cur_op, KEY_SUB))                  /* Subtraction. */
    mpsub(v->MPresult, v->MPdisp_val, v->MPresult) ;

  else if (v->cur_op == '*' ||
           IS_KEY(v->cur_op, KEY_MUL))                  /* Multiplication */
    mpmul(v->MPresult, v->MPdisp_val, v->MPresult) ;

  else if (IS_KEY(v->cur_op, KEY_DIV))                  /* Division. */
    mpdiv(v->MPresult, v->MPdisp_val, v->MPresult) ;

  else if (IS_KEY(v->cur_op, KEY_PER))                  /* % */
    {
      mpmul(v->MPresult, v->MPdisp_val, v->MPresult) ;
      MPstr_to_num("0.01", DEC, MP1) ;
      mpmul(v->MPresult, MP1, v->MPresult) ;
    }

  else if (IS_KEY(v->cur_op, KEY_YTOX))                 /* y^x */
    mppwr2(v->MPresult, v->MPdisp_val, v->MPresult) ;

  else if (IS_KEY(v->cur_op, KEY_AND))                  /* And */
    {
      mpcmd(v->MPresult, &dres) ;
      mpcmd(v->MPdisp_val, &dval) ;
      dres = setbool((BOOLEAN)(ibool(dres) & ibool(dval))) ;
      mpcdm(&dres, v->MPresult) ;
    }

  else if (IS_KEY(v->cur_op, KEY_OR))                   /* Or */
    {
      mpcmd(v->MPresult, &dres) ;
      mpcmd(v->MPdisp_val, &dval) ;
      dres = setbool((BOOLEAN)(ibool(dres) | ibool(dval))) ;
      mpcdm(&dres, v->MPresult) ;
    }

  else if (IS_KEY(v->cur_op, KEY_XOR))                  /* Xor */
    {
      mpcmd(v->MPresult, &dres) ;
      mpcmd(v->MPdisp_val, &dval) ;
      dres = setbool((BOOLEAN)(ibool(dres) ^ ibool(dval))) ;
      mpcdm(&dres, v->MPresult) ;
    }

  else if (IS_KEY(v->cur_op, KEY_XNOR))                 /* Xnor */
    {
      mpcmd(v->MPresult, &dres) ;
      mpcmd(v->MPdisp_val, &dval) ;
      dres = setbool((BOOLEAN)(~ibool(dres) ^ ibool(dval))) ;
      mpcdm(&dres, v->MPresult) ;
    }

  else if (IS_KEY(v->cur_op, KEY_EQ)) /* do nothing. */ ;   /* Equals */

  show_display(v->MPresult) ;

  if (!(IS_KEY(v->current, KEY_EQ) && IS_KEY(v->old_cal_value, KEY_EQ)))
    mpstr(v->MPdisp_val, v->MPlast_input) ;

  mpstr(v->MPresult, v->MPdisp_val) ;

  v->cur_op = v->current ;

  v->old_cal_value = v->current ;
  v->new_input     = v->key_exp = 0 ;
}


void
do_clear(void)       /* Clear the calculator display and re-initialise. */
{
  clear_display() ;
  if (v->error) set_item(DISPLAYITEM, "") ;
  initialise() ;
}


void
do_constant(void)
{
  if (v->current >= '0' && v->current <= '9')
    {
      mpstr(v->MPcon_vals[char_val(v->current)], v->MPdisp_val) ;
      show_display(v->MPdisp_val) ;
    }
}


void
do_delete(void)     /* Remove the last numeric character typed. */
{
  if (strlen(v->display))
    v->display[strlen(v->display)-1] = '\0' ;

   /*  If we were entering a scientific number, and we have backspaced over
    *  the exponent sign, then this reverts to entering a fixed point number.
    */

  if (v->key_exp && !(strchr(v->display, '+')))
    {
      v->key_exp = 0 ;
      v->display[strlen(v->display)-1] = '\0' ;
      set_item(OPITEM, "") ;
    }

   /* If we've backspaced over the numeric point, clear the pointed flag. */

  if (v->pointed && !(strchr(v->display, '.'))) v->pointed = 0 ;

  if(strcmp(v->display, "") == 0)
     do_clear();

  set_item(DISPLAYITEM, v->display) ;
  MPstr_to_num(v->display, v->base, v->MPdisp_val) ;
}


void
do_exchange(void)         /* Exchange display with memory register. */
{
  int i, MPtemp[MP_SIZE] ;

  for (i = MEM_START; i <= MEM_END; i++)
    if (v->current == menu_entries[i].val)
      {
        mpstr(v->MPdisp_val, MPtemp) ;
        mpstr(v->MPmvals[char_val(v->current)], v->MPdisp_val) ;
        mpstr(MPtemp, v->MPmvals[char_val(v->current)]) ;
        make_registers(MEM) ;
        return ;
      }
}


void
do_expno(void)           /* Get exponential number. */
{
  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

  v->pointed = (strchr(v->display, '.') != NULL) ;
  if (!v->new_input)
    {
      STRCPY(v->display, "1.0 +") ;
      v->new_input = v->pointed = 1 ;
    }
  else if (!v->pointed)
    {
      STRNCAT(v->display, ". +", 3) ;
      v->pointed = 1 ;
    }
  else if (!v->key_exp) STRNCAT(v->display, " +", 2) ;
  v->toclear = 0 ;
  v->key_exp = 1 ;
  v->exp_posn = strchr(v->display, '+') ;
  set_item(DISPLAYITEM, v->display) ;
  MPstr_to_num(v->display, v->base, v->MPdisp_val) ;
}


void
do_factorial(int *MPval, int *MPres)     /* Calculate the factorial of MPval. */
{
  double val ;
  int i, MPa[MP_SIZE], MP1[MP_SIZE], MP2[MP_SIZE] ;

/*  NOTE: do_factorial, on each iteration of the loop, will attempt to
 *        convert the current result to a double. If v->error is set,
 *        then we've overflowed. This is to provide the same look&feel
 *        as V3.
 *
 *  XXX:  Needs to be improved. Shouldn't need to convert to a double in
 *        order to check this.
 */

  mpstr(MPval, MPa) ;
  mpcmim(MPval, MP1) ;
  i = 0 ;
  mpcim(&i, MP2) ;
  if (mpeq(MPval, MP1) &&  mpge(MPval, MP2))  /* Only positive integers. */
    {
      i = 1 ;
      if (mpeq(MP1, MP2))                     /* Special case for 0! */
        {
          mpcim(&i, MPres) ;
          return ;
        }
      mpcim(&i, MPa) ;
      mpcmi(MP1, &i) ;
      if (!i) matherr((struct exception *) NULL) ;
      else
        while (i > 0)
          {
            mpmuli(MPa, &i, MPa) ;
            mpcmd(MPa, &val) ;
            if (v->error) break ;
            i-- ;
          }
    }
  else matherr((struct exception *) NULL) ;
  mpstr(MPa, MPres) ;
}


void
do_frame(void)    /* Exit dtcalc. */
{
  exit(0) ;
}

void
do_function(void)      /* Perform a user defined function. */
{
  enum fcp_type scurwin ;
  int fno, scolumn, srow ;

  srow = v->row ;
  scolumn = v->column ;
  scurwin = v->curwin ;
  v->pending = 0 ;
  if (v->current >= '0' && v->current <= '9')
    {
      fno = char_val(v->current) ;
      if(strcmp(v->fun_vals[fno], "") != 0)
         process_str(v->fun_vals[fno], M_FUN) ;
    }
  v->curwin = scurwin ;
  v->row = srow ;
  v->column = scolumn ;
}


void
do_immed(void)
{
  double dval, dval2 ;
  int i, MP1[MP_SIZE], MP2[MP_SIZE] ;

  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

 if (IS_KEY(v->current, KEY_HYP))          /* Hyp */
    {
      v->hyperbolic = !v->hyperbolic ;
      set_item(HYPITEM, (v->hyperbolic) ? vstrs[(int) V_HYP]
                                        : "    ") ;
    }

  else if (IS_KEY(v->current, KEY_INV))          /* Inv */
    {
      v->inverse = !v->inverse ;
      set_item(INVITEM, (v->inverse) ? vstrs[(int) V_INV]
                                     : "    ") ;
    }

  else if (IS_KEY(v->current, KEY_32))           /* &32 */
    {
      mpcmd(v->MPdisp_val, &dval) ;
      dval2 = ibool2(dval);
      if(dval2 == 0)
         doerr(GETMESSAGE(5, 6, "ERR:Num too large for operation"));
      else
      {
         dval = setbool((BOOLEAN)dval2) ;
         mpcdm(&dval, v->MPdisp_val) ;
      }
    }

  else if (IS_KEY(v->current, KEY_16))           /* &16 */
    {
      mpcmd(v->MPdisp_val, &dval) ;
      dval2 = ibool2(dval);
      if(dval2 == 0)
         doerr(GETMESSAGE(5, 6, "ERR:Num too large for operation"));
      else
      {
         dval = setbool((BOOLEAN)(ibool(dval2) & 0xffff)) ;
         mpcdm(&dval, v->MPdisp_val) ;
      }
    }

  else if (IS_KEY(v->current, KEY_ETOX))         /* e^x */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpexp(MP1, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_TTOX))         /* 10^x */
    {
      i = 10 ;
      mpcim(&i, MP1) ;
      mppwr2(MP1, v->MPdisp_val, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_LN))           /* Ln */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpln(MP1, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_LOG))          /* Log */
    {
      mplog10(v->MPdisp_val, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_RAND))         /* Rand */
    {
      dval = drand48() ;
      mpcdm(&dval, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_SQRT))         /* Sqrt */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpsqrt(MP1, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_NOT))          /* Not */
    {
      mpcmd(v->MPdisp_val, &dval) ;
      dval = setbool((BOOLEAN)~ibool(dval)) ;
      mpcdm(&dval, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_REC))          /* 1/x */
    {
      i = 1 ;
      mpcim(&i, MP1) ;
      mpstr(v->MPdisp_val, MP2) ;
      mpdiv(MP1, MP2, v->MPdisp_val) ;
    }
  else if (IS_KEY(v->current, KEY_FACT))         /* x! */
    {
      do_factorial(v->MPdisp_val, MP1) ;
      mpstr(MP1, v->MPdisp_val) ;
    }
  else if (IS_KEY(v->current, KEY_SQR))          /* x^2 */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpmul(MP1, MP1, v->MPdisp_val) ;
    }

  else if (IS_KEY(v->current, KEY_CHS))          /* +/- */
    {
      if (v->key_exp)
        {
          if (*v->exp_posn == '+') *v->exp_posn = '-' ;
          else                     *v->exp_posn = '+' ;
          set_item(DISPLAYITEM, v->display) ;
          MPstr_to_num(v->display, v->base, v->MPdisp_val) ;
          v->key_exp = 0 ;
        }
      else
      {
         mpneg(v->MPdisp_val, v->MPdisp_val) ;
         mpstr(v->MPdisp_val, v->MPlast_input) ;
      }
    }
  show_display(v->MPdisp_val) ;
}


void
do_keys(void)      /* Display/undisplay the dtcalc key values. */
{
  v->tstate = !v->tstate ;
  redraw_buttons() ;
}

void
do_mode(void)                  /* Set special calculator mode. */
{
       if (v->current == MODE_FIN) v->modetype = FINANCIAL ;
  else if (v->current == MODE_LOG) v->modetype = LOGICAL ;
  else if (v->current == MODE_SCI) v->modetype = SCIENTIFIC ;

  make_modewin() ;
  v->curwin = FCP_KEY ;
}


void
do_none(void)       /* Null routine for empty buttons. */
{
}


void
do_number(void)
{
  char nextchar ;
  int len, n ;
  static int maxvals[4] = { 1, 7, 9, 15 } ;

  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

  nextchar = v->current ;
  n = v->current - '0' ;
  if (v->base == HEX && v->current >= 'a' && v->current <= 'f')
    {
      nextchar -= 32 ;             /* Convert to uppercase hex digit. */
      n = v->current - 'a' + 10 ;
    }
  if (n > maxvals[(int) v->base])
    {
      beep() ;
      return ;
    }

  if (v->toclear)
    {
      SPRINTF(v->display, "%c", nextchar) ;
      v->toclear = 0 ;
    }
  else
    {
      len = strlen(v->display) ;
      if (len < MAX_DIGITS)
        {
          v->display[len] = nextchar ;
          v->display[len+1] = '\0' ;
        }
      else
        beep() ;
    }
  set_item(DISPLAYITEM, v->display) ;
  MPstr_to_num(v->display, v->base, v->MPdisp_val) ;
  v->new_input = 1 ;
}


void
do_numtype(void)    /* Set number type (engineering, fixed or scientific). */
{
       if (v->current == DISP_ENG) v->dtype = ENG ;
  else if (v->current == DISP_FIX) v->dtype = FIX ;
  else if (v->current == DISP_SCI) v->dtype = SCI ;
  else return ;

  set_numtype(v->dtype);
}

void
set_numtype(enum num_type dtype)
{
  v->pending = 0 ;
  show_display(v->MPdisp_val) ;
  set_option_menu((int) NUMITEM, (int)v->dtype);
  if (v->rstate) make_registers(MEM) ;
  if (v->frstate) make_registers(FIN) ;
}

void
do_paren(void)
{
  char *ptr ;
  double tmpdb;

  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

/*  Check to see if this is the first outstanding parenthesis. If so, and
 *  their is a current operation already defined, then add the current
 *  operation to the parenthesis expression being displayed.
 *  Increment parentheses count, and add the open paren to the expression.
 */

  if (IS_KEY(v->current, KEY_LPAR))
    {
      if (v->noparens == 0)
      {
          /* if not in default state, put the operand between the display
             value and the paren, else just put the paren */
          if(!v->defState)
          {
            /* there is no paren, and there is no current operand ... Let's
               make the current operand into a "x" */
            if(v->cur_op == '?')
            {
               v->current = 'x';
               do_calc();
            }
 
             /* if the current op is an '=' and the result in the display is 
                zero, we want to ignore the display */
             if(v->cur_op == '=')
             {
                mpcmd(v->MPdisp_val, &tmpdb);
                if(tmpdb == 0.0)
                {
                   v->cur_op = '?';
                   STRCPY(v->display, "") ;
                   set_item(DISPLAYITEM, v->display) ;
                }
                else
                {
                   v->current = 'x';
                   do_calc();
                   v->current = '(';
                   paren_disp(v->cur_op) ;
                }
             }
             else
             {
                v->current = '(';
                paren_disp(v->cur_op) ;
             }
          }
          else
          {
             STRCPY(v->display, "") ;
             set_item(DISPLAYITEM, v->display) ;
         }
      }
      else
      {
         int len = strlen(v->display);

         if(v->display[len - 1] >= '0' && v->display[len - 1] <= '9')
            paren_disp(v->cur_op) ;
      }

      v->pending = v->current ;
      v->noparens++ ;
    }

/*  If we haven't had any left brackets yet, and this is a right bracket,
 *  then just ignore it.
 *  Decrement the bracket count. If the count is zero, then process the
 *  parenthesis expression.
 */

  else if (IS_KEY(v->current, KEY_RPAR))
    {
      if (!v->noparens) return ;
      v->noparens-- ;
      if (!v->noparens)
        {
          v->toclear = 1;
          paren_disp(v->current) ;
          ptr = v->display ;
          while (*ptr != '(') ptr++ ;
          while (*ptr != '\0') process_parens(*ptr++) ;
          return ;
        }
    }
  paren_disp(v->current) ;
}

void
do_pending(void)
{

  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

/*  Certain pending operations which are half completed, force the numeric
 *  keypad to be reshown (assuming they already aren't).
 *
 *  Con, Exch, Fun, Sto, Rcl and Acc    show buttons 0 - 9.
 *  < and >                             show buttons 0 - f.
 */

  if (!v->ismenu)
    {
      if (IS_KEY(v->current, KEY_CON)  ||      /* Con. */
          IS_KEY(v->current, KEY_EXCH) ||      /* Exch. */
          IS_KEY(v->current, KEY_FUN)  ||      /* Fun. */
          IS_KEY(v->current, KEY_STO)  ||      /* Sto. */
          IS_KEY(v->current, KEY_RCL)  ||      /* Rcl. */
          IS_KEY(v->current, KEY_ACC))         /* Acc. */
        grey_buttons(DEC) ;
      if (IS_KEY(v->current, KEY_LSFT) ||
          IS_KEY(v->current, KEY_RSFT))
        grey_buttons(HEX) ;
     }

       if (IS_KEY(v->pending, KEY_BASE)) do_base() ;         /* Base */
  else if (IS_KEY(v->pending, KEY_DISP)) do_numtype() ;      /* Disp */
  else if (IS_KEY(v->pending, KEY_TRIG)) do_trigtype() ;     /* Trig */
  else if (IS_KEY(v->pending, KEY_CON))  do_constant() ;     /* Con */
  else if (IS_KEY(v->pending, KEY_EXCH)) do_exchange() ;     /* Exch */
  else if (IS_KEY(v->pending, KEY_FUN))  do_function() ;     /* Fun */
  else if (IS_KEY(v->pending, KEY_STO) ||                    /* Sto */
           IS_KEY(v->pending, KEY_RCL))                      /* Rcl */
    {
      do_sto_rcl() ;
      if (IS_KEY(v->pending_op, KEY_ADD) ||
          IS_KEY(v->pending_op, KEY_SUB) ||
          IS_KEY(v->pending_op, KEY_MUL) ||
          IS_KEY(v->pending_op, KEY_DIV)) return ;
    }
  else if (IS_KEY(v->pending, KEY_LSFT) ||                   /* < */
           IS_KEY(v->pending, KEY_RSFT)) do_shift() ;        /* > */
  else if (IS_KEY(v->pending, KEY_ACC))  do_accuracy() ;     /* Acc */
  else if (IS_KEY(v->pending, KEY_MODE)) do_mode() ;         /* Mode */
  else if (IS_KEY(v->pending, KEY_LPAR))                     /* ( */
    {
      do_paren() ;
      return ;
    }
  else if (!v->pending)
    {
      save_pending_values(v->current) ;
      v->pending_op = KEY_EQ ;
      return ;
    }

  show_display(v->MPdisp_val) ;
  if (v->error) set_item(OPITEM, vstrs[(int) V_CLR]) ;
  else set_item(OPITEM, "") ;         /* Redisplay pending op. (if any). */

  v->pending = 0 ;
  if (!v->ismenu)
    grey_buttons(v->base) ;  /* Just show numeric keys for current base. */
}


void
do_point(void)                   /* Handle numeric point. */
{
  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

  if (!v->pointed)
    {
      if (v->toclear)
        {
          STRCPY(v->display, ".") ;
          v->toclear = 0 ;
        }
      else STRNCAT(v->display, ".", 1) ;
      v->pointed = 1 ;
    }
  else
    beep() ;
  set_item(DISPLAYITEM, v->display) ;
  MPstr_to_num(v->display, v->base, v->MPdisp_val) ;
}


void
do_portion(void)
{
  int MP1[MP_SIZE] ;

  /* the financial state is false - last key was not a fin. key */
  v->funstate = 0;

       if (IS_KEY(v->current, KEY_ABS))                      /* Abs */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpabs(MP1, v->MPdisp_val) ;
    }
  else if (IS_KEY(v->current, KEY_FRAC))                     /* Frac */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpcmf(MP1, v->MPdisp_val) ;
    }
  else if (IS_KEY(v->current, KEY_INT))                      /* Int */
    {
      mpstr(v->MPdisp_val, MP1) ;
      mpcmim(MP1, v->MPdisp_val) ;
    }
  show_display(v->MPdisp_val) ;
}


void
do_shift(void)     /* Perform bitwise shift on display value. */
{
  int MPtemp[MP_SIZE], shift ;
  BOOLEAN temp ;
  double dval ;

  shift = char_val(v->current) ;
  if(strcmp(v->snum, v->display) != 0)
  {
     MPstr_to_num(v->display, v->base, MPtemp) ;
     mpcmd(MPtemp, &dval) ;
  }
  else
     mpcmd(v->MPdisp_val, &dval) ;
  temp = ibool(dval) ;

       if (IS_KEY(v->pending, KEY_LSFT)) temp = temp << shift ;
  else if (IS_KEY(v->pending, KEY_RSFT)) temp = temp >> shift ;

  dval = setbool((BOOLEAN)temp) ;
  mpcdm(&dval, v->MPdisp_val) ;
  show_display(v->MPdisp_val) ;
  mpstr(v->MPdisp_val, v->MPlast_input) ;
  return ;
}


void
do_sto_rcl(void)     /* Save/restore value to/from memory register. */
{
  int i, MPn[MP_SIZE], n ;

  for (i = MEM_START; i <= MEM_END; i++)
    if (v->current == menu_entries[i].val)
      {
        if (IS_KEY(v->pending, KEY_RCL))                        /* Rcl */
          {
            mpstr(v->MPmvals[char_val(v->current)], v->MPdisp_val) ;
            v->new_input = 1 ;
          }
        else if (IS_KEY(v->pending, KEY_STO))                   /* Sto */
          {
            n = char_val(v->current) ;

                 if (IS_KEY(v->pending_op, KEY_ADD))            /* + */
              {
                mpstr(v->MPmvals[n], MPn) ;
                mpadd(MPn, v->MPdisp_val, v->MPmvals[n]) ;
              }
            else if (IS_KEY(v->pending_op, KEY_SUB))            /* - */
              {
                mpstr(v->MPmvals[n], MPn) ;
                mpsub(MPn, v->MPdisp_val, v->MPmvals[n]) ;
              }
            else if (IS_KEY(v->pending_op, KEY_MUL))            /* x */
              {
                mpstr(v->MPmvals[n], MPn) ;
                mpmul(MPn, v->MPdisp_val, v->MPmvals[n]) ;
              }
            else if (IS_KEY(v->pending_op, KEY_DIV))            /* / */
              {
                mpstr(v->MPmvals[n], MPn) ;
                mpdiv(MPn, v->MPdisp_val, v->MPmvals[n]) ;
              }
            else mpstr(v->MPdisp_val, v->MPmvals[n]) ;

            v->pending_op = 0 ;
            make_registers(MEM) ;
          }
        return ;
      }

  if (IS_KEY(v->current, KEY_ADD) || IS_KEY(v->current, KEY_SUB) ||
      IS_KEY(v->current, KEY_MUL) || IS_KEY(v->current, KEY_DIV))
    v->pending_op = v->current ;
}


void
do_trig(void)         /* Perform all trigonometric functions. */
{
  int i, MPtemp[MP_SIZE], MP1[MP_SIZE], MP2[MP_SIZE] ;
  double cval ;
  int MPcos[MP_SIZE], MPsin[MP_SIZE] ;

  if (!v->inverse)
    {
      if (!v->hyperbolic)
        {
               if (v->ttype == DEG)
            {
              mppi(MP1) ;
              mpmul(v->MPdisp_val, MP1, MP2) ;
              i = 180 ;
              mpcim(&i, MP1) ;
              mpdiv(MP2, MP1, MPtemp) ;
            }
          else if (v->ttype == GRAD)
            {
              mppi(MP1) ;
              mpmul(v->MPdisp_val, MP1, MP2) ;
              i = 200 ;
              mpcim(&i, MP1) ;
              mpdiv(MP2, MP1, MPtemp) ;
            }
          else mpstr(v->MPdisp_val, MPtemp) ;
        }
      else mpstr(v->MPdisp_val, MPtemp) ;

      if (!v->hyperbolic)
        {
               if (IS_KEY(v->current, KEY_COS))                  /* Cos */
            mpcos(MPtemp, v->MPtresults[(int) RAD]) ;
          else if (IS_KEY(v->current, KEY_SIN))                  /* Sin */
            mpsin(MPtemp, v->MPtresults[(int) RAD]) ;
          else if (IS_KEY(v->current, KEY_TAN))                  /* Tan */
            {
              mpsin(MPtemp, MPsin) ;
              mpcos(MPtemp, MPcos) ;
              mpcmd(MPcos, &cval) ;
              if (cval == 0.0) doerr(vstrs[(int) V_ERROR]) ;
              mpdiv(MPsin, MPcos, v->MPtresults[(int) RAD]) ;
            }
        }
      else
        {
               if (IS_KEY(v->current, KEY_COS))                  /* Cosh */
            mpcosh(MPtemp, v->MPtresults[(int) RAD]) ;
          else if (IS_KEY(v->current, KEY_SIN))                  /* Sinh */
            mpsinh(MPtemp, v->MPtresults[(int) RAD]) ;
          else if (IS_KEY(v->current, KEY_TAN))                  /* Tanh */
            mptanh(MPtemp, v->MPtresults[(int) RAD]) ;
        }

      mpstr(v->MPtresults[(int) RAD], v->MPtresults[(int) DEG]) ;
      mpstr(v->MPtresults[(int) RAD], v->MPtresults[(int) GRAD]) ;
    }
  else
    {
      if (!v->hyperbolic)
        {
                 if (IS_KEY(v->current, KEY_COS))                /* Acos */
              mpacos(v->MPdisp_val, v->MPdisp_val) ;
            else if (IS_KEY(v->current, KEY_SIN))                /* Asin */
              mpasin(v->MPdisp_val, v->MPdisp_val) ;
            else if (IS_KEY(v->current, KEY_TAN))                /* Atan */
              mpatan(v->MPdisp_val, v->MPdisp_val) ;
        }
      else
        {
                 if (IS_KEY(v->current, KEY_COS))                /* Acosh */
              mpacosh(v->MPdisp_val, v->MPdisp_val) ;
            else if (IS_KEY(v->current, KEY_SIN))                /* Asinh */
              mpasinh(v->MPdisp_val, v->MPdisp_val) ;
            else if (IS_KEY(v->current, KEY_TAN))                /* Atanh */
              mpatanh(v->MPdisp_val, v->MPdisp_val) ;
        }

      if (!v->hyperbolic)
        {
          i = 180 ;
          mpcim(&i, MP1) ;
          mpmul(v->MPdisp_val, MP1, MP2) ;
          mppi(MP1) ;
          mpdiv(MP2, MP1, v->MPtresults[(int) DEG]) ;

          i = 200 ;
          mpcim(&i, MP1) ;
          mpmul(v->MPdisp_val, MP1, MP2) ;
          mppi(MP1) ;
          mpdiv(MP2, MP1, v->MPtresults[(int) GRAD]) ;
        }
      else
        {
          mpstr(v->MPdisp_val, v->MPtresults[(int) DEG]) ;
          mpstr(v->MPdisp_val, v->MPtresults[(int) GRAD]) ;
        }

      mpstr(v->MPdisp_val, v->MPtresults[(int) RAD]) ;
    }

  show_display(v->MPtresults[(int) v->ttype]) ;
  mpstr(v->MPtresults[(int) v->ttype], v->MPdisp_val) ;
  v->cur_op = '?';
}


void
do_trigtype(void)          /* Change the current trigonometric type. */
{
       if (v->current == TRIG_DEG) v->ttype = DEG ;
  else if (v->current == TRIG_GRA) v->ttype = GRAD ;
  else if (v->current == TRIG_RAD) v->ttype = RAD ;
  else return ;

  if (IS_KEY(v->cur_op, KEY_COS) ||
      IS_KEY(v->cur_op, KEY_SIN) ||
      IS_KEY(v->cur_op, KEY_TAN))
    {
      mpstr(v->MPtresults[(int) v->ttype], v->MPdisp_val) ;
      show_display(v->MPtresults[(int) v->ttype]) ;
    }
  set_option_menu((int) TTYPEITEM, (int)v->ttype);
  v->pending = 0 ;
}


BOOLEAN
ibool(double x)
{
  BOOLEAN p ;

       if (x >  68719476736.00) return(0) ;
  else if (x < -68719476736.00) return(0) ;
  else
    {
      while (x <  0.0)           x += 4294967296.00 ;
      while (x >= 4294967296.00) x -= 4294967296.00 ;
      p = x ;
      return(p) ;
    }
}

BOOLEAN
ibool2(double x)
{
  BOOLEAN p ;

  if (x >  9007199254740991.00 || x < -9007199254740991.00)
  {
     return(0) ;
  }
  else
    {
      while (x <  0.0)           x += 4294967296.00 ;
      while (x >= 4294967296.00) x -= 4294967296.00 ;
      p = x ;
      return(p) ;
    }
}


/*  The following MP routines were not in the Brent FORTRAN package. They are
 *  derived here, in terms of the existing routines.
 */

/*  MP precision arc cosine.
 *
 *  1. If (x < -1.0  or x > 1.0) then report DOMAIN error and return 0.0.
 *
 *  2. If (x = 0.0) then acos(x) = PI/2.
 *
 *  3. If (x = 1.0) then acos(x) = 0.0
 *
 *  4. If (x = -1.0) then acos(x) = PI.
 *
 *  5. If (0.0 < x < 1.0) then  acos(x) = atan(sqrt(1-(x**2)) / x)
 *
 *  6. If (-1.0 < x < 0.0) then acos(x) = atan(sqrt(1-(x**2)) / x) + PI
 */

void
mpacos(int *MPx, int *MPretval)
{
  int MP0[MP_SIZE],  MP1[MP_SIZE],  MP2[MP_SIZE] ;
  int MPn1[MP_SIZE], MPpi[MP_SIZE], MPy[MP_SIZE], val ;

  mppi(MPpi) ;
  val = 0 ;
  mpcim(&val, MP0) ;
  val = 1 ;
  mpcim(&val, MP1) ;
  val = -1 ;
  mpcim(&val, MPn1) ;

  if (mpgt(MPx, MP1) || mplt(MPx, MPn1))
    {
      doerr("acos DOMAIN error") ;
      mpstr(MP0, MPretval) ;
    }
  else if (mpeq(MPx, MP0))
    {
      val = 2 ;
      mpdivi(MPpi, &val, MPretval) ;
    }
  else if (mpeq(MPx, MP1))  mpstr(MP0, MPretval) ;
  else if (mpeq(MPx, MPn1)) mpstr(MPpi, MPretval) ;
  else
    {
      mpmul(MPx, MPx, MP2) ;
      mpsub(MP1, MP2, MP2) ;
      mpsqrt(MP2, MP2) ;
      mpdiv(MP2, MPx, MP2) ;
      mpatan(MP2, MPy) ;
      if (mpgt(MPx, MP0)) mpstr(MPy, MPretval) ;
      else                 mpadd(MPy, MPpi, MPretval) ;
    }
}


/*  MP precision hyperbolic arc cosine.
 *
 *  1. If (x < 1.0) then report DOMAIN error and return 0.0.
 *
 *  2. acosh(x) = log(x + sqrt(x**2 - 1))
 */

void
mpacosh(int *MPx, int *MPretval)
{
  int MP1[MP_SIZE], val ;

  val = 1 ;
  mpcim(&val, MP1) ;
  if (mplt(MPx, MP1))
    {
      doerr("acosh DOMAIN error") ;
      val = 0 ;
      mpcim(&val, MPretval) ;
    }
  else
    {
      mpmul(MPx, MPx, MP1) ;
      val = -1 ;
      mpaddi(MP1, &val, MP1) ;
      mpsqrt(MP1, MP1) ;
      mpadd(MPx, MP1, MP1) ;
      mpln(MP1, MPretval) ;
    }
}


/*  MP precision hyperbolic arc sine.
 *
 *  1. asinh(x) = log(x + sqrt(x**2 + 1))
 */

void
mpasinh(int *MPx, int *MPretval)
{
  int MP1[MP_SIZE], val ;

  mpmul(MPx, MPx, MP1) ;
  val = 1 ;
  mpaddi(MP1, &val, MP1) ;
  mpsqrt(MP1, MP1) ;
  mpadd(MPx, MP1, MP1) ;
  mpln(MP1, MPretval) ;
}


/*  MP precision hyperbolic arc tangent.
 *
 *  1. If (x <= -1.0 or x >= 1.0) then report a DOMAIn error and return 0.0.
 *
 *  2. atanh(x) = 0.5 * log((1 + x) / (1 - x))
 */

void
mpatanh(int *MPx, int *MPretval)
{
  int MP0[MP_SIZE], MP1[MP_SIZE], MP2[MP_SIZE] ;
  int MP3[MP_SIZE], MPn1[MP_SIZE], val ;

  val = 0 ;
  mpcim(&val, MP0) ;
  val = 1 ;
  mpcim(&val, MP1) ;
  val = -1 ;
  mpcim(&val, MPn1) ;

  if (mpge(MPx, MP1) || mple(MPx, MPn1))
    {
      doerr("atanh DOMAIN error") ;
      mpstr(MP0, MPretval) ;
    }
  else
    {
      mpadd(MP1, MPx, MP2) ;
      mpsub(MP1, MPx, MP3) ;
      mpdiv(MP2, MP3, MP3) ;
      mpln(MP3, MP3) ;
      MPstr_to_num("0.5", DEC, MP1) ;
      mpmul(MP1, MP3, MPretval) ;
    }
}


/*  MP precision common log.
 *
 *  1. log10(x) = log10(e) * log(x)
 */

void
mplog10(int *MPx, int *MPretval)
{
  int MP1[MP_SIZE], MP2[MP_SIZE], n ;

  n = 10 ;
  mpcim(&n, MP1) ;
  mpln(MP1, MP1) ;
  mpln(MPx, MP2) ;
  mpdiv(MP2, MP1, MPretval) ;
}


void
process_parens(char current)
{
  int i ;
  int last_lpar ;     /* Position in stack of last left paren. */
  int last_num ;      /* Position is numeric stack to start processing. */

/*  Check to see if this is the first outstanding parenthesis. If so, and
 *  their is a current operation already defined, then push the current
 *  result on the numeric stack, and note it on the op stack, with a -1,
 *  which has this special significance.
 *  Zeroise current display value (in case of invalid operands inside the
 *  parentheses.
 *  Add the current pending operation to the opstack.
 *  Increment parentheses count.
 */

  if (IS_KEY(current, KEY_LPAR))
    {
      if (!v->noparens && v->cur_op != '?')
        {
          push_num(v->MPresult) ;
          push_op(-1) ;
          i = 0 ;
          mpcim(&i, v->MPdisp_val) ;
          push_op(v->cur_op) ;
        }
      v->noparens++ ;     /* Count of left brackets outstanding. */
      save_pending_values(current) ;
    }

/*  If we haven't had any left brackets yet, and this is a right bracket,
 *  then just ignore it.
 *  Decrement the bracket count.
 *  Add a equals to the op stack, to force a calculation to be performed
 *  for two op operands. This is ignored if the preceding element of the
 *  op stack was an immediate operation.
 *  Work out where the preceding left bracket is in the stack, and then
 *  process the stack from that point until this end, pushing the result
 *  on the numeric stack, and setting the new op stack pointer appropriately.
 *  If there are no brackets left unmatched, then clear the pending flag,
 *  clear the stack pointers and current operation, and show the display.
 */

  else if (IS_KEY(current, KEY_RPAR))
    {
      v->noparens-- ;
      push_op('=') ;
      last_lpar = v->opsptr - 1 ;
      last_num = v->numsptr ;
      while (!IS_KEY(v->opstack[last_lpar], KEY_LPAR))
        {
          if (v->opstack[last_lpar] == -1) last_num-- ;
          last_lpar-- ;
        }
      process_stack(last_lpar + 1, last_num, v->opsptr - last_lpar - 1) ;
      if (!v->noparens)
        {
          if (v->opsptr > 1)
            {
              push_op(KEY_EQ) ;
              process_stack(0, 0, v->opsptr) ;
            }
          v->pending = v->opsptr = v->numsptr = 0 ;
          v->cur_op = '?' ;
          set_item(OPITEM, "") ;
          if (v->error)
            {
              set_item(DISPLAYITEM, vstrs[(int) V_ERROR]) ;
              set_item(OPITEM,      vstrs[(int) V_CLR]) ;
              STRCPY(v->display,    vstrs[(int) V_ERROR]) ;
            }
          else
            {
              show_display(v->MPdisp_val) ;
              mpstr(v->MPdisp_val, v->MPlast_input) ;
            }
        }
      return ;
    }
  push_op(current) ;
}


void
push_num(int *MPval)            /* Try to push value onto the numeric stack. */
{
  if (v->numsptr < 0) return ;
  if (v->numsptr >= MAXSTACK)
    {
      STRCPY(v->display, vstrs[(int) V_NUMSTACK]) ;
      set_item(DISPLAYITEM, v->display) ;
      v->error = 1 ;
      beep() ;
      set_item(OPITEM, vstrs[(int) V_CLR]) ;
    }
  else
    {
      if (v->MPnumstack[v->numsptr] == NULL)
        v->MPnumstack[v->numsptr] =
                        (int *) LINT_CAST(calloc(1, sizeof(int) * MP_SIZE)) ;
      mpstr(MPval, v->MPnumstack[v->numsptr++]) ;
    }
}


void
push_op(int val)     /* Try to push value onto the operand stack. */
{
  if (v->opsptr < 0) return ;
  if (v->opsptr >= MAXSTACK)
    {
      STRCPY(v->display, vstrs[(int) V_OPSTACK]) ;
      set_item(DISPLAYITEM, v->display) ;
      v->error = 1 ;
      set_item(OPITEM, vstrs[(int) V_CLR]) ;
    }
  else v->opstack[v->opsptr++] = val ;
}


void
save_pending_values(int val)
{
  int n ;

  v->pending = val ;
  for (n = 0; n < TITEMS; n++)
  {
    if (val == buttons[n].value)
       v->pending_n = n ;
  }
  v->pending_win = v->curwin ;
  if (v->pending_win == FCP_MODE)
     v->pending_mode = v->modetype ;
}


double
setbool(BOOLEAN p)
{
  BOOLEAN q ;
  double val ;

  q = p & 0x80000000 ;
  p &= 0x7fffffff ;
  val = p ;
  if (q) val += 2147483648.0 ;
  return(val) ;
}

double
do_round(double result, int ndigits)
{
    char buf2[40], buffer[100];
    int temp;

    if (isnan(result)) return result;
#if defined(_AIX) || defined(__aix) || defined(__osf__) || defined(__sparc)
    temp = finite(result);
    if (!temp)
       return (temp > 0) ? HUGE : -HUGE;
#else
#if defined(USL) || defined(__uxp__)
    temp = finite(result);
    if (!temp)
       return (temp > 0) ? HUGE : -HUGE;
#else
    if ((temp = isinf(result))) return (temp > 0) ? HUGE : -HUGE;
#endif /* USL or __uxp__ */
#endif /* _AIX or __osf__ */

    if (ndigits >= 0 && ndigits < MAX_DIGITS)
    {
        result += 0.5 * (result > 0 ? mods[ndigits] : -mods[ndigits]);
        result -= fmod(result, mods[ndigits]);
    }

    sprintf(buf2, "%%.%dlg", MAX_DIGITS);
    sprintf(buffer, buf2, result);
    return atof(buffer);
}

BOOLEAN
try_compute_i(double guess, double *result, int method)
{
    double sum_pos, sum_pos_prime, sum_neg, sum_neg_prime, w = guess;
    double new_w;
    int niter = 0;

    for (;;)
    {
	double term, term_prime, f, f_prime, lsp, lsn;

	sum_pos = sum_pos_prime = sum_neg = sum_neg_prime = 0;

	if (v->MPfvals[2] != 0.0)
	{
	    if (w == 1)
	    {
		term = 1;
		term_prime = v->MPfvals[0];
	    }
	    else
	    {
		term = pow(w, v->MPfvals[0]);
		term_prime = (v->MPfvals[0]) * pow(w, v->MPfvals[0] - 1.0);
	    }
	    if (v->MPfvals[2] > 0.0)
	    {
		sum_pos += v->MPfvals[2] * term;
		sum_pos_prime += v->MPfvals[2] * term_prime;
	    }
	    else
	    {
		sum_neg -= v->MPfvals[2] * term;
		sum_neg_prime -= v->MPfvals[2] * term_prime;
	    }
	}
	if (v->MPfvals[3] != 0.0)
	{
	    if (w == 1.0)
	    {
		term = v->MPfvals[0];

		term_prime = v->MPfvals[0] * (v->MPfvals[0] - 1) / 2.0 +
			     v->MPfvals[0] * (0.0);
	    }
	    else
	    {
		double wn = pow(w, v->MPfvals[0]);
		double wdb = pow(w, 0.0);

		term = (wn - 1.0) * wdb / (w - 1.0);

		term_prime = (v->MPfvals[0] * pow(w,(0.0 + v->MPfvals[0] - .01))
                              + (wn - 1.0) * (0.0) * pow(w, (0.0 - 1.0))) /
                              (w - 1.0) - (wn - 1.0) * wdb /
			      ((w - 1.0) * (w - 1.0));

	    }
	    if (v->MPfvals[3] > 0.0)
	    {
		sum_pos += v->MPfvals[3] * term;
		sum_pos_prime += v->MPfvals[3] * term_prime;
	    }
	    else
	    {
		sum_neg -= v->MPfvals[3] * term;
		sum_neg_prime -= v->MPfvals[3] * term_prime;
	    }
	}
	if (v->MPfvals[4] != 0.0)
	{
	    if (v->MPfvals[4] > 0.0) sum_pos += v->MPfvals[4];
	    else sum_neg -= v->MPfvals[4];
	}

	lsp = log(sum_pos);
	lsn = log(sum_neg);

	switch (method)
	{
            default:
	    case 1:
		f = lsp - lsn;
		f_prime = sum_pos_prime / sum_pos - sum_neg_prime / sum_neg;
		break;
	    case 2:
		f = lsp / lsn - 1.0;
		f_prime = (lsn * sum_pos_prime / sum_pos -
			   lsp * sum_neg_prime / sum_neg) /
		          (lsn * lsn);
		break;
	}

	new_w = w - f / f_prime;

#if defined(_AIX) || defined(__aix) || defined (__osf__) || defined(__sparc)
	if (!(!isnan(new_w) && finite(new_w)))
	    return FALSE;
#else
#if defined(USL) || defined(__uxp__)
	if (!(!isnan(new_w) && finite(new_w)))
	    return FALSE;
#else
	if (!(!isnan(new_w) && !isinf(new_w)))
	    return FALSE;
#endif
#endif /* _AIX or __osf__ */

	if (new_w == w || (w != 0.0 && fabs((new_w - w) / w) < FIN_EPSILON))
           break;

	w = new_w;

	if (niter++ >= MAX_FIN_ITER)
	    return FALSE;
    }

    *result = do_round((new_w - 1.0) * 100.0 * v->MPfvals[5], -1);
    return TRUE;
}

static void
compute_i(double *target)
{
    double p[3];
    double first_period, last_period;
    int nsc;
    BOOLEAN success;

    first_period = 1.0;
    last_period = 0.0;

    if (first_period < 0.0 || last_period < 0.0)
    {
        doerr(GETMESSAGE(5, 5, "ERROR:Invalid odd period values"));
	return;
    }

    p[0] = v->MPfvals[2] + (first_period == 0.0 ? v->MPfvals[3] : 0);
    p[1] = v->MPfvals[3];
    p[2] = v->MPfvals[4] + (last_period == 0.0 ? v->MPfvals[3] : 0);

    nsc = count_sign_changes(p, 3);

    if (nsc == 0)
    {
        int MP1[MP_SIZE], MP2[MP_SIZE], MP3[MP_SIZE], MP4[MP_SIZE];
        int MP5[MP_SIZE] ;
        int val;
        double temp;

        temp = v->MPfvals[4]/v->MPfvals[2];
        mpcdm(&temp, MP1);
        val = 1 ;
        mpcim(&val, MP2) ;
        mpcdm(&(v->MPfvals[0]), MP4);
        mpdiv(MP2, MP4, MP3) ;
        mppwr2(MP1, MP3, MP5) ;
        val = -1 ;
        mpaddi(MP5, &val, MP1) ;
        val = 1200 ;
        mpmuli(MP1, &val, v->MPdisp_val) ;
        mpcmd(v->MPdisp_val, target);
	return;
    }
    else if (nsc > 1)
    {
        doerr(GETMESSAGE(5, 3, "ERROR: Multiple Solutions"));
	return;
    }
    else if (v->MPfvals[0] <= 0)
    {
        doerr(GETMESSAGE(5, 4, "ERROR: Term <= 0"));
	return;
    }

    success = try_compute_i((double)1.0, target, 1);
    success = success || try_compute_i((double)1.0e-12, target, 1);
    success = success || try_compute_i((double)1.0, target, 2);
    success = success || try_compute_i((double)1.0e-12, target, 2);

    if (!success)
        doerr(GETMESSAGE(5, 1, "ERROR: Computation Failed"));
}

static int
count_sign_changes(double *cf, int count)
{
    int i, curr_sign = 0, result = 0;

    for (i = 0; i < count; i++)
    {
        if (cf[i] == 0.0) continue;

        if (curr_sign == 1)
        {
            if (cf[i] > 0.0) continue;
            curr_sign = -1;
            result++;
        }
        else if (curr_sign == -1)
        {
            if (cf[i] < 0.0) continue;
            curr_sign = 1;
            result++;
        }
        else
        {
            if (cf[i] > 0.0) curr_sign = 1;
            else curr_sign = -1;
        }
    }

    return result;
}

