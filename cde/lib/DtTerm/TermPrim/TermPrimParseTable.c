/* 
** $XConsortium: TermPrimParseTable.c /main/1 1996/04/21 19:18:15 drk $
*/

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include "TermPrimP.h"
#include "TermPrimParserP.h"
#include "TermPrimParseTable.h"

static StateTableRec stateStart;

/*
** this is the table used when starting from the initial state
*/
static StateEntryRec
startTable[] =
{
    0x07, 0x07, &stateStart, _DtTermPrimParseBell,      /* ring the bell   */
    0x08, 0x08, &stateStart, _DtTermPrimParseBackspace, /* backspace       */
    0x09, 0x09, &stateStart, _DtTermPrimParseTab,       /* horizontal tab  */
    0x0A, 0x0A, &stateStart, _DtTermPrimParseLF,        /* newline         */
    0x0D, 0x0D, &stateStart, _DtTermPrimParseCR,        /* carriage return */
    0x00, 0xFF, &stateStart, _DtTermPrimParseWriteChar, /* write to dpy    */
};

static StateTableRec stateStart = {True, startTable};
StateTable _DtTermPrimStateStart = &stateStart;
