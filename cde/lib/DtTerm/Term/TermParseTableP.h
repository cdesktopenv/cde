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
