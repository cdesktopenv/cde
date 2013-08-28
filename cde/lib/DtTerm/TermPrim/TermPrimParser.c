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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$TOG: TermPrimParser.c /main/2 1999/10/15 12:23:41 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include <Xm/Xm.h>
#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimParserP.h"
#include "TermPrimBuffer.h"

#if defined (__hpux)
/*
 * On HP MAXINT is defined in both <values.h> and <sys/param.h>
 */
#undef MAXINT
#endif
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include  <values.h>
#endif

#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
static void
ParseTrap()
{
    static int count = 0;

    (void) count++;
}

/*
** Parse the character, tell the calling routine if we are not
** in the start state.
*/
Boolean
_DtTermPrimParse
(
    Widget          w,
    unsigned char  *parseChar,
    int		    parseCharLen
)
{
    ParserContext   context = GetParserContext(w);
    StateEntry      thisEntry;
    StateEntry      thisPreParseEntry;

#ifdef    NOCODE
    /*
    ** This decision should be made somewhere else.
    */
    if (tp->t_modes.disp_func == 1)
    {
        in_disp_func();
        return(False);
    }
#endif /* NOCODE */

    if (parseCharLen == 1) {
	*context->inputChar = *parseChar;
    } else {
	(void) memmove(context->inputChar, parseChar, parseCharLen);
    }
    context->inputCharLen = parseCharLen;

    if (isDebugFSet('p', 1)) {
#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
	static unsigned char debugChar;
	static Boolean first = True;
	char *c;

	if (parseCharLen == 1) {
	    _DtTermProcessLock();
	    if (first) {
		if (!(c = getenv("dttermDebugParseChar"))) {
		    c = "0x03";
		}
		debugChar = strtol(c, (char **) 0, 0);
		first = False;
	    }
	    _DtTermProcessUnlock();

	    if (*parseChar == debugChar) {
		ParseTrap();
		return;
	    }
	}
    }
	
    /*
    ** Determine which state entry to use.
    */
    thisPreParseEntry = context->stateTable->statePreParseEntry;
    thisEntry = context->stateTable->stateEntry;

    /* first run through the preParse entry... */
    if (thisPreParseEntry && (parseCharLen == 1)) {
	while ((*parseChar < thisPreParseEntry->lower) ||
		(*parseChar > thisPreParseEntry->upper)) {
	    thisPreParseEntry++;
	}
	/* if we hit the end, ignore it... */
	if ((0x00 == thisPreParseEntry->lower) &&
		(0xff == thisPreParseEntry->upper)) {
	    thisPreParseEntry = (StateEntry) 0;
	}
    }

    /* if we hit a valid preParseEntry, then let's execute it and
     * return...
     */
    if (thisPreParseEntry) {
	/*
	** Now change states.  If the next state is NULL, stay in the
	** current state.  This is for parse entries that do not break us
	** out of the current parse thread.  If we need to bail out of the
	** current parse thread, then we have a new state specified and
	** will switch to it.  We do this before we execute the function
	** incase the function needs to change the state as well...
	*/
	if (thisPreParseEntry->nextState) {
	    context->stateTable = thisPreParseEntry->nextState;
	}

	/*
	** Execute the action associated with the entry.
	*/
	if (thisPreParseEntry->action) {
	    (*thisPreParseEntry->action)(w);
	}

	return(!context->stateTable->startState);
    }

    /* HACK ALERT!!!!
     *
     * We need two different search algorithms - the first to deal
     * with single byte characters, the second to deal with multi-byte
     * characters.  For now, we will match multi-byte character with
     * the parse entry that covers 0..255.  If we find that this will
     * not work for everything, we may need to rethink this.
     */
    if (parseCharLen == 1) {
	while ((*parseChar < thisEntry->lower) ||
		(*parseChar > thisEntry->upper))
	{
	    thisEntry++;
	}
    } else {
	while ((0x00 != thisEntry->lower) ||
		(0xff != thisEntry->upper))
	{
	    thisEntry++;
	}
    }
	
    /*
    ** Now change states.  We do this before we execute the function incase
    ** the function needs to change the state as well...
    */
    context->stateTable = thisEntry->nextState;

    /*
    ** Execute the action associated with the entry.
    */
    if (thisEntry->action) {
	(*thisEntry->action)(w);
    }

    return(!context->stateTable->startState);
}

/*
**
**  _DtTermPrimParserClrStrParm()
**
**  clrStrParm clears the string parameters.
**
*/
void
_DtTermPrimParserClrStrParm
(
    Widget w
)
{
    ParserContext   context = GetParserContext(w);

    context->stringParms[0].length = 0;
    (void) memset(context->stringParms[0].str, 0x00, STR_SIZE + 1);

    context->stringParms[1].length = 0;
    (void) memset(context->stringParms[1].str, 0x00, STR_SIZE + 1);
}

/*
**
**  _DtTermPrimParserNextState()
**
**  nextState is a do nothing routine.  It is called when the only
**  action that needs to be preformed is changing states in the
**  state machine.
**
*/
void
_DtTermPrimParserNextState
(
    Widget w
)
{
}

/*
**
**  _DtTermPrimParserClearParm()
**
**  _DtTermPrimParserClearParm clears all common parameters.
**
*/
void
_DtTermPrimParserClearParm
(
    Widget w
)
{
    ParserContext  context = GetParserContext(w);
    int             i;

    for (i = 0; i < NUM_PARMS; i++) {
        context->parms[i] = 0;
    }
    context->workingNum = 0;
    context->workingNumIsDefault = True;
    context->sign = TermPARSER_SIGNnone;
}

/*
**
**  enterNum()
**
**  enterNum enters a numerical parameter.
**
*/
void
_DtTermPrimParserEnterNum
(
    Widget w
)
{
    ParserContext   context = GetParserContext(w);

    if ( context->workingNum < MAXINT>>4 ) 
        context->workingNum = context->workingNum * 10 +
	    (*context->inputChar - '0');
    context->workingNumIsDefault = False;
}

void
_DtTermPrimParserSaveSign
(
    Widget w
)
{
    ParserContext   context = GetParserContext(w);

    SetSign(context, (*GetInputChar(context) == '-') ? TermPARSER_SIGNnegative :
	    TermPARSER_SIGNpositive);
}

/*
**
**  _DtTermPrimParserNumParmPush()
**
**  _DtTermPrimParserNumParmPush() takes the number in workingNum and 
**   stores it in parm[parmNum].
**
*/
void
_DtTermPrimParserNumParmPush
(
    Widget w,
    int    parmNum
)
{
    ParserContext   context = GetParserContext(w);

    if ( parmNum < NUM_PARMS ) {
      context->parms[parmNum] = context->workingNum;
      context->workingNum     = 0;
      context->workingNumIsDefault = False;
    }
}

/*
** Initialize the context of the parser.
*/
void
_DtTermPrimParserInitContext
(
    Widget w
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    tpd->context = (ParserContext) XtMalloc(sizeof(ParserContextRec));
    (void) memset(tpd->context, '\0', sizeof(ParserContextRec));

    tpd->context->stateTable =
	    *(((DtTermPrimitiveClassRec *) (tw->core.widget_class))->
	    term_primitive_class.parser_start_state);
    tpd->context->stateName  = START;
}

#ifdef TEST

#include <ctype.h>

#define testFunction(f) \
void f ( Widget w ) {printf(#f " called.\n");}

int cursor_col, cursor_row;
int logical_col, logical_row;

testFunction(_termCR)
testFunction(_termLF)
testFunction(_termTab)
testFunction(_termBackspace)
testFunction(RecordDC1Rcvd)
testFunction(_termWriteChar)
testFunction(_DtTermPrimBell)
testFunction(_termShiftOut)
testFunction(_termShiftIn)
testFunction(set_tab)
testFunction(clear_tab)
testFunction(clr_all_tab)
testFunction(set_left_marg)
testFunction(set_right_marg)
testFunction(clear_marg)
testFunction(curs_up)
testFunction(curs_down)
testFunction(curs_right)
testFunction(curs_left)
testFunction(home_down)
testFunction(home_up)
testFunction(clr_display)
testFunction(clear_line)
testFunction(insert_line)
testFunction(delete_line)
testFunction(delete_char)
testFunction(insert_char)
testFunction(off_insert_char)
testFunction(roll_up)
testFunction(roll_down)
testFunction(next_page)
testFunction(prev_page)
testFunction(format_mode_on)
testFunction(format_mode_off)
testFunction(display_func)
testFunction(term_status)
testFunction(rel_curs_pos)
testFunction(abs_curs_pos)
testFunction(enable_key)
testFunction(disable_key)
testFunction(enter_line)
testFunction(back_tab)
testFunction(user_key_menu_on)
testFunction(user_key_menu_off)
testFunction(second_status)
testFunction(mlock_on)
testFunction(mlock_off)
testFunction(start_unprotect)
testFunction(stop_field)

void
_parserPrintContext
(
    ParserContext   context
)
{
    printf("    inputChar  : ");
    if (isprint(*context->inputChar))
    {
        printf("'%c'\n", *context->inputChar);
    }
    else
    {
        printf("\x2X\n", *context->inputChar);
    }
    for (i = 0; i < 9; i++)
    {
        printf("    parm[%d]      : %d\n", i, context->parm[i]);
    }
    printf("    workingNum : %d\n", context->workingNum);
}

void
parseString
(
    Widget          w;
    unsigned char  *string
)
{
    ParserContext   context = GetParserContext(w);
    int i;

    _parserPrintContext(context);
    for (i = 0; i < strlen((char *)string); i++)
    {
        _DtTermPrimParse(context, string[i]);
        _parserPrintContext(context);
    }
}

/* the following is to allow for a single main function in the code... */
#define	parserMain	main
parserMain()
{
    parserContext   context;

    _parserInitContext(&context);

    cursor_col = 0;
    cursor_row = 0;

    logical_col = 5;
    logical_row = 5;

    printf("\nparsing <esc>&a5r5C\n");
    parseString(&context, (unsigned char *)"\033&a5r5C");

    printf("\nparsing <esc>a5r35C\n");
    parseString(&context, (unsigned char *)"\033&a5r35C");

    printf("\nparsing <esc>&a5r3r4c5C\n");
    parseString(&context, (unsigned char *)"\033&a5r3r4c5C");

    printf("\nparsing <esc>&arC\n");
    parseString(&context, (unsigned char *)"\033&arC");

    printf("\nparsing <esc>&a5C\n");
    parseString(&context, (unsigned char *)"\033&a5C");

    printf("\nparsing <esc>&a+5C\n");
    parseString(&context, (unsigned char *)"\033&a+5C");
}
#endif /* TEST */
