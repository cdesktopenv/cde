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
** $XConsortium: TermParseTableP.h /main/1 1996/04/21 19:16:20 drk $
*/

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef   _Dt_TermParseTableP_h
#define   _Dt_TermParseTableP_h

#include "TermParseTable.h"

extern StateTable _DtTermStateStart;
extern StateTableRec _vtStateLParen;
extern StateTableRec _vtStateRParen;
extern StateTableRec _vtStateRParenNum;
extern StateTableRec _DtTermStateError;
extern StateTableRec _DtTermStatePound;
extern StateTableRec _DtTermStateLeftParen;
extern StateTableRec _DtTermStateRightParen;
extern StateTableRec _DtTermStatePlus;
extern StateTableRec _vtStateAsterick;
extern StateTableRec _DtTermStateLeftBracket;
extern StateTableRec _DtTermStateRightBracket;
extern StateTableRec _DtTermStateUpArrow;
extern StateTableRec _DtTermStateUnderscore;
extern StateTableRec _DtTermStateError;
extern StateTableRec _DtTermStateAsterisk;
extern StateTableRec _DtTermStateTilda;
extern StateTableRec _DtTermStateQuestion;
extern StateTableRec _DtTermStateEscape;
extern StateTableRec _DtTermStateEscQuestion;
extern StateTableRec _DtTermStateP;
extern StateTableRec _DtTermStateGetString;
extern StateTableRec _DtTermStateC1;
extern StateTableRec _DtTermStateDECSCA;
extern StateTableRec _DtTermStateSoftReset;
extern StateTableRec _DtTermStateSoftKey;
extern StateTableRec _DtTermStateDLCharSet;
extern StateTableRec _DtTermStateIsNextBS_SK;
extern StateTableRec _DtTermStateIsNextBS_DL;
extern StateTableRec _DtTermStateUserKeyString;
extern StateTableRec _DtTermStateUserKey;
extern StateTableRec _DtTermStateGetTitle;
extern StateTableRec _DtTermStateGetIcon;
extern StateTableRec _DtTermStateGetIconFile;
extern StateTableRec _DtTermStateLeftBracketNotQ;

#endif /* _Dt_TermParseTableP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
