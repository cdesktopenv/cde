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
#ifndef	lint
#ifdef	VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermFunctionKey.c /main/1 1996/04/21 19:15:53 drk $";
#endif	/* VERBOSE_REV_INFO */
#endif	/* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include <Xm/Xm.h>
#include "TermAction.h"
#include "TermPrimI.h"
#include "TermP.h"
/* #include "TermFunctionKeyP.h" */
/* #include "TermPrimFunction.h" */
#include "TermSendEsc.h"

#define NO_SUN_FUNC_KEYS 35
#define NO_VT_FUNC_KEYS 20

static char *functionKey[] = { F1_VT,F2_VT,F3_VT,F4_VT,F5_VT,F6_VT,F7_VT,F8_VT,F9_VT,F10_VT,F11_VT,F12_VT,F13_VT,F14_VT,F15_VT,F16_VT,F17_VT,F18_VT,F19_VT,F20_VT};
static char *sunFunctionKey[] = { F1_VT_SUN,F2_VT_SUN,F3_VT_SUN,F4_VT_SUN,F5_VT_SUN,F6_VT_SUN,F7_VT_SUN,F8_VT_SUN,F9_VT_SUN,F10_VT_SUN,F11_VT_SUN,F12_VT_SUN,F13_VT_SUN,F14_VT_SUN,F15_VT_SUN,F16_VT_SUN,F17_VT_SUN,F18_VT_SUN,F19_VT_SUN,F20_VT_SUN,F21_VT_SUN,F22_VT_SUN,F23_VT_SUN,F24_VT_SUN,F25_VT_SUN,F26_VT_SUN,F27_VT_SUN,F28_VT_SUN,F29_VT_SUN,F30_VT_SUN,F31_VT_SUN,F32_VT_SUN,F33_VT_SUN,F34_VT_SUN,F35_VT_SUN};
      

/* NOTE: the following function assumes that the keys start at 0
 * (i.e., F1 == 0, F2 == 1, etc)...
 */
void
_DtTermFunctionKeyExecute(
    Widget		  w,
    short		  keyNumber,
    Boolean		  shift
)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;

    if (!shift || tw->vt.sunFunctionKeys == True ) {
      if ( tw->vt.sunFunctionKeys == True ) {  
        if ( --keyNumber < NO_SUN_FUNC_KEYS )
            (void) _DtTermWriteEscSeq(w, sunFunctionKey[keyNumber]);
       }
      else {
        if ( --keyNumber < NO_VT_FUNC_KEYS )
            (void) _DtTermWriteEscSeq(w, functionKey[keyNumber]);
      }
    } else {
	(void) _DtTermFunctionKeyUserKeyExecute(w, keyNumber, shift);
    }
}

/* NOTE: the following function assumes that the keys start at 0
 * (i.e., F1 == 0, F2 == 1, etc)...
 */
void
_DtTermFunctionKeyUserKeyExecute(
    Widget		  w,
    short		  keyno,
    Boolean               shift
)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;

    keyno -= 6 ;  /* valid keys are F6-F20 */
    if ( keyno >= 0 && keyno <= NO_USER_KEY_STRINGS
                                  && td->userKeyString[keyno] )
       _DtTermPrimSendInput(w,td->userKeyString[keyno],
                                   strlen((char *)td->userKeyString[keyno])) ;
}

void
_DtTermFunctionKeyStringStore(       /* store the label for the given key number */
     Widget w,
     int  keyno,  
     char *string
) 
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    int ukey = 0 ;

    switch ( keyno  ) {
       case 17: ukey = 0 ; break;
       case 18: ukey = 1 ; break;
       case 19: ukey = 2 ; break;
       case 20: ukey = 3 ; break;
       case 21: ukey = 4 ; break;
       case 23: ukey = 5 ; break;
       case 24: ukey = 6 ; break;
       case 25: ukey = 7 ; break;
       case 26: ukey = 8 ; break;
       case 28: ukey = 9; break;
       case 29: ukey = 10 ; break;
       case 31: ukey = 11 ; break;
       case 32: ukey = 12 ; break;
       case 33: ukey = 13 ; break;
       case 34: ukey = 14 ; break;
       default: ukey = -1 ; break;
     }
    if (ukey == -1) return ;
    if (td->userKeyString[ukey])  free(td->userKeyString[ukey]) ;
    td->userKeyString[ukey] = malloc(strlen(string)+1) ;
    strcpy((char *)td->userKeyString[ukey],string) ;
}

void
_DtTermFunctionKeyClear(
    Widget   w
) 
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    int i ;

    for (i=0; i<=NO_USER_KEY_STRINGS ; i++)  {
      if (td->userKeyString[i]) {
         free(td->userKeyString[i]) ;
         td->userKeyString[i] = 0;
       }
    }
}

void
_DtTermFunctionKeyInit(
   Widget w
)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    int i ;

    for (i=0; i<=NO_USER_KEY_STRINGS ; i++)  {
         td->userKeyString[i] = 0;
       }
}
