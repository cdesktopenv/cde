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
/* 
** $XConsortium: TermPrimParserP.h /main/1 1996/04/21 19:18:27 drk $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimParserP_h
#define   _Dt_TermPrimParserP_h

#include  "TermPrimParser.h"
#include  <limits.h>

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */


typedef enum _TermParserSign
{
    TermPARSER_SIGNnone = 0,
    TermPARSER_SIGNnegative,
    TermPARSER_SIGNpositive
} TermParserSign;

#define GetInputChar(context)           ((context)->inputChar)
#define GetWorkingNum(context)          ((context)->workingNum)
#define GetWorkingNumIsDefault(context)	((context)->workingNumIsDefault)
#define	GetSign(context)		((context)->sign)
#define GetParm(context, i)		((context)->parms[i])

#define SetWorkingNum(context, value)   (context)->workingNum = (value)
#define SetWorkingNumIsDefault(context, value) \
					(context)->workingNumIsDefault = (value)
#define	SetSign(context, value)		(context)->sign = (value)
#define SetParm(context, i, value)	((context)->parms[i]) = (value)

/*
**  seq - parser state machine transition table
*/

typedef struct _StateEntryRec
{
    unsigned char   lower;	    /* lower end of character range	 */
    unsigned char   upper;	    /* upper end of character range	 */
    StateTable      nextState;	    /* next state to enter		 */
    void            (*action)();    /* index to next runtime routine     */
} StateEntryRec, *StateEntry;

typedef struct _StateTableRec
{
    Boolean	    startState;	    /* is this a start state?		 */
    StateEntry      stateEntry;	    /* state entry table for state	 */
    StateEntry      statePreParseEntry;
				    /* pre-parse state entry table	 */
} StateTableRec;

/* 
** Maximum length of a softkey definition.
*/
#define STR_SIZE 256

typedef struct _stringParameter
{
    unsigned char   str[STR_SIZE + 1];   /* buffer for string */
    unsigned char   length;              /* length of str     */
} stringParameter;

/*
** states in escape sequence parsing
*/
typedef enum _parseState
{
    START,          /* initial state                */
    ESCAPE,         /* saw an escape                */
    AMP,            /* saw an escape &              */
    AMP_A,          /* saw an escape & a            */
    AMP_A_SIGN,     /* saw an ^[&a- or ^[&a+        */
    AMP_A_NUM,	    /* saw an escape & a num        */
    AMP_D,	    /* saw an escape & d            */
    AMP_F,	    /* saw an escape & f            */
    AMP_F_NUM,      /* saw an escape & f num        */
    AMP_F_MIN,	    /* saw an escape & f -          */
    AMP_F_MIN_NUM,  /* saw an escape & f - num      */
    AMP_F_STR,	    /* saw an escape & f string     */
    AMP_J,	    /* saw an escape & j            */
    AMP_J_NUM,	    /* saw an escape & j num        */
    AMP_J_NUM_L,    /* saw an escape & j num L      */
    AMP_J_NUM_D,	
    AMP_K,	    /* saw an escape & k            */
    AMP_K_NUM,	    /* saw an escape & k [0-9]*     */
    AMP_R,	    /* saw an escape & r            */
    AMP_R_NUM,	    /* saw an escape & r num        */
    AMP_S,	    /* saw an escape & s            */
    AMP_S_0,	    /* saw an escape & s 0          */
    AMP_S_1,	    /* saw an escape & s 1          */
    LPAREN,	    /* saw an escape (              */
    RPAREN,	    /* saw an escape )              */
    ASTERISK,	    /* saw an escape *              */
    ASTERISK_D,	    /* saw an escape * d            */
    ASTERISK_S,	    /* saw an escape * s            */
    AMP_P,
    AMP_P_NUM,
    AMP_P_NUM_W,
    AMP_P_W,
    ERROR,          /* error in escape sequence     */
    ASTERISK_X,     /* saw an escape * x            */
    ASTERISK_Y,     /* saw an escape * y            */
    ASTERISK_Y_EXC, /* saw an escape * y !          */
    AMP_V,          /* saw an escape & v            */
    AMP_V_ZERO,     /* saw an escape & v 0          */
    AMP_V_ONE,      /* saw an escape & v 1          */
    AMP_V_2_7,      /* saw an escape & v [2-7]      */
    AMP_V_DEC,      /* saw an escape & v .          */
    AMP_X,          /* saw an escape & x            */
    AMP_X_0,        /* saw an escape & x 0          */
    AMP_X_1,        /* saw an escape & x 1          */
    KJCODE          /* Kanji State  1/17/86 NY      */
} parseState;

#define NUM_PARMS  20

typedef struct _ParserContextRec
{
    /*
    ** escape sequence parsing variables
    */
    StateTable       stateTable;    /* state table of the current state  */
    unsigned char    inputChar[MB_LEN_MAX];
				    /* character being parsed            */
    short	     inputCharLen;  /* length of the above		 */
    parseState       stateName;	    /* enum type with current state      */
    int              workingNum;    /* working number for entering parm  */
    Boolean	     workingNumIsDefault;
				    /* true if working num not touched   */
    TermParserSign   sign;	    /* for numeric sign			 */
    int              parms[NUM_PARMS];	 /* parameters                   */
    stringParameter  stringParms[2];
} ParserContextRec;

/* 
** Parser support routines.
*/
extern void _DtTermPrimParserSaveSign ( Widget w);
extern void _DtTermPrimParserNextState(Widget w);
extern void _DtTermPrimParserClearParm(Widget w);
extern void _DtTermPrimParserClrStrParm(Widget w);
extern void _DtTermPrimParserEnterNum(Widget w);
extern void _enterMinNum(Widget w);
extern void _DtTermPrimParserParm1(Widget w);
extern void _DtTermPrimParserParm2(Widget w);
extern void _DtTermPrimParserParm3(Widget w);
extern void _DtTermPrimParserParm4(Widget w);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimParserP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
