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
  { 0x07, 0x07, &stateStart, _DtTermPrimParseBell,      }, /* ring the bell   */
  { 0x08, 0x08, &stateStart, _DtTermPrimParseBackspace, }, /* backspace       */
  { 0x09, 0x09, &stateStart, _DtTermPrimParseTab,       }, /* horizontal tab  */
  { 0x0A, 0x0A, &stateStart, _DtTermPrimParseLF,        }, /* newline         */
  { 0x0D, 0x0D, &stateStart, _DtTermPrimParseCR,        }, /* carriage return */
  { 0x00, 0xFF, &stateStart, _DtTermPrimParseWriteChar, }, /* write to dpy    */
};

static StateTableRec stateStart = {True, startTable};
StateTable _DtTermPrimStateStart = &stateStart;
