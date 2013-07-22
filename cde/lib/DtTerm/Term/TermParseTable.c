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
** $XConsortium: TermParseTable.c /main/3 1996/10/11 17:45:10 rswiston $
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

#include <Xm/Xm.h>
#ifndef   TEST
#include "TermHeader.h"
#endif /* TEST */
#include "TermPrimParserP.h"
#include "TermPrimParseTable.h"		/* DKS: the things we care about should be moved to TermParseTable.h */
#include "TermParseTableP.h"

static StateTableRec stateStart;

/* This is our basic pre-parse table
 */
static StateEntryRec
pre_parse_table[] =
{
  { 0x00, 0x00, NULL,	NULL,		/* null chars -- ignore */ },
#ifdef	NOTDEF
  { 0x05, 0x05, NULL,	enquire,	/* send answer back	*/ },
#endif	/* NOTDEF */
  { 0x07, 0x07, NULL,	_DtTermPrimParseBell,   /* ring the bell  */ },
  { 0x08, 0x08, NULL,	_DtTermParseBackspace,  /* backspace  */ },
  { 0x09, 0x09, NULL,	_DtTermParseTab,         /* horizontal tab */ },
  { 0x0A, 0x0A, NULL,	_DtTermParseLF,          /* newline       */ },
  { 0x0B, 0x0B, NULL,	_DtTermParseLF,    /* Vertical Tab (Ctrl-k) */ },
  { 0x0C, 0x0C, NULL,	_DtTermParseLF,    /* Form Feed or New Page */ },
  { 0x0D, 0x0D, NULL,	_DtTermPrimParseCR,  /* carriage return   */ },
  { 0x0E, 0x0F, NULL,	_DtTermParseFontShift, /* font shift in/out */ },
  { 0x1B, 0x1B, &_DtTermStateEscape, _DtTermClearParam, /*   Esc        */ },
  { 0x84, 0x84, &stateStart,_DtTermIndex,     /* Index                 */ },
  { 0x85, 0x85, &stateStart,_DtTermNextLine,  /* Next Line             */ },
  { 0x88, 0x88, &stateStart,_DtTermTabSet,    /* Horizontal Tab Set    */ },
  { 0x8D, 0x8D, &stateStart,_DtTermReverseIndex, /* Reverse Index      */ },
  { 0x8E, 0x8E, &stateStart,_DtTermSingleShiftG2,/* Single Shift G2    */ },
  { 0x8F, 0x8F, &stateStart,_DtTermSingleShiftG3, /* Single Shift G3   */ },
  { 0x90, 0x90, &_DtTermStateP,  _DtTermPrimParserNextState, /* DCS     */ },
  { 0x9B, 0x9B, &_DtTermStateLeftBracket, _DtTermClearParam,/*CSI*/ },
  { 0x00, 0xFF, NULL,	NULL,		    /* end of table */ },
};

/*
** Since the preparse was added this state only serves to indicate
** the end of parsing  (originally it was the start state)
*/
static StateEntryRec
start_table[] =
{
  { 0x00, 0xFF, &stateStart,  _DtTermPrimParserNextState /* end of table */ },
};

static StateTableRec stateStart = {True, start_table, pre_parse_table};
StateTable _DtTermStateStart = &stateStart;

/*
** this is the table used when an escape has been seen
*/
static StateEntryRec
escape_table[] =
{
  {    '[', '[', &_DtTermStateLeftBracket,    _DtTermPrimParserNextState, },
  {    '#', '#', &_DtTermStatePound,       _DtTermPrimParserNextState,    },
  {    '(', '(', &_DtTermStateLeftParen,   _DtTermPrimParserNextState,  },
  {    ')', ')', &_DtTermStateRightParen,  _DtTermPrimParserNextState,   },
  {    '*', '*', &_DtTermStateAsterisk,  _DtTermPrimParserNextState, },
  {    '+', '+', &_DtTermStatePlus,     _DtTermPrimParserNextState, },
  {    '7', '7', &stateStart,    _DtTermSaveCursor, /* Save Cursor (DECSC) */ },
  {    '8', '8', &stateStart,_DtTermRestoreCursor,  /* Restore Cursor (DECRC)*/ },
  {    '=', '=', &stateStart,    _DtTermAppKeypad ,      /* App keypad */ },
  {    '>', '>', &stateStart,    _DtTermNormalKeypad,    /* Normal keypad  */ },
  {    'D', 'D', &stateStart,    _DtTermIndex,             /* cursor down IND */ },
  {    'E', 'E', &stateStart,    _DtTermNextLine,          /* next line (NEL) */ },
  {    'H', 'H', &stateStart,    _DtTermTabSet,            /* Tab Set (HTS) */ },
  {    'M', 'M', &stateStart,    _DtTermReverseIndex,        /* RI */ },
  {    'N', 'N', &stateStart,    _DtTermSingleShiftG2,       /* SS2 */ },
  {    'O', 'O', &stateStart,    _DtTermSingleShiftG3,       /* SS3 */ },
  {    'P', 'P', &_DtTermStateP,    _DtTermPrimParserNextState,  /* DCS */ },
  {    'Z', 'Z', &stateStart,    _DtTermParseStatus,     /* Same as DA */ },
  {    ']', ']', &_DtTermStateRightBracket,    _DtTermPrimParserNextState,  },
  {    '^', '^', &_DtTermStateUpArrow,  _DtTermPrimParserNextState, /* PM */ },
  {    '_', '_', &_DtTermStateUnderscore,    _DtTermPrimParserNextState, /*ACP*/ },
  {    'c', 'c', &stateStart,    _DtTermReset,  /* Full Reset (RIS) */ },
  {    'n', 'n', &stateStart,    _DtTermSelectG2,  /* Select G2 character set */ },
  {    'o', 'o', &stateStart,    _DtTermSelectG3,  /* Select G3 character set */ },
  {    '|', '|', &stateStart,    _DtTermInvokeG3,  /* Invoke G3 character set */ },
  {    '}', '}', &stateStart,    _DtTermInvokeG2,  /* Select G2 character set */ },
  {    '~', '~', &_DtTermStateTilda, _DtTermPrimParserNextState,  /* Invoke G1*/ },
  {    ' ', ' ', &_DtTermStateC1, _DtTermPrimParserNextState,/* set C1 control*/ },
  { 0x00, 0xFF, &stateStart, _DtTermPrimParserNextState  /* end of table */ },
};
StateTableRec _DtTermStateEscape = {False, escape_table, pre_parse_table};


/*
** this is the table used when an escape sp has been seen
*/
static StateEntryRec
set_c1_table[] =
{
 {  'F', 'F', &stateStart,    _DtTermS7C1T, /* Select 7-bit C1 Control */ },
 {  'G', 'G', &stateStart,    _DtTermS8C1T, /* Select 8-bit C1 Control */ },
 {  0x00, 0xFF, &stateStart,    _DtTermPrintState ,    /* end of table  */ },
};
StateTableRec _DtTermStateC1      = {False, set_c1_table, pre_parse_table};

/*
** this is the table used when an escape # been seen
*/
static StateEntryRec
pound_table[] =
{
 {  '3',  '3',  &stateStart,    _DtTermDoubleHigh, },
 {  '5',  '5',  &stateStart,    _DtTermSingleWide, },
 {  '6',  '6',  &stateStart,    _DtTermDoubleWide, },
 {  '8',  '8',  &stateStart,    _DtTermAlignTest, },
 {  0x00, 0xFF, &stateStart,    _DtTermPrintState ,  /* end of table */ },
};
StateTableRec _DtTermStatePound   = {False, pound_table, pre_parse_table};


/*
** this is the table used when an escape ( has been seen
*/
static StateEntryRec
left_paren_table[] =
{
 {  0x00, 0xFF, &stateStart,      _DtTermParseFontG0     /* end of table */ },
};
StateTableRec _DtTermStateLeftParen   = {False, left_paren_table, pre_parse_table};

/*
** this is the table used when an escape ) a has been seen
*/
static StateEntryRec
right_paren_table[] =
{
 {  0x00, 0xFF, &stateStart,      _DtTermParseFontG1     /* end of table */ },
};
StateTableRec _DtTermStateRightParen   = {False, right_paren_table, pre_parse_table};

/*
** this is table used when ESC * has been seen
*/
static StateEntryRec
esc_asterisk_table[] =
{
 {  0x00, 0xFF, &stateStart,       _DtTermParseFontG2     /* end of table */ },
};
StateTableRec _DtTermStateAsterisk  = {False, esc_asterisk_table, pre_parse_table};

/*
** this is table used when Esc + has been seen
*/
static StateEntryRec
plus_table[] =
{
 {  0x00, 0xFF, &stateStart,       _DtTermParseFontG3     /* end of table */ },
};
StateTableRec _DtTermStatePlus  = {False, plus_table, pre_parse_table};

/*
** this is table used when "Esc ] l<string>Esc"  has been seen
*/
static StateEntryRec   
get_titleBS_table[] =
{
 {  '\\', '\\', &stateStart, _DtTermParserSunWindowTitle, },
 {  0x00,0xFF, &_DtTermStateGetTitle, _DtTermSaveChar,   },
};
StateTableRec _DtTermStateGetTitleBS  = {False, get_titleBS_table};

/*
** this is table used when "Esc ] l"  has been seen
*/
static StateEntryRec   
get_title_table[] =
{
 {  0x1B, 0x1B,&_DtTermStateGetTitleBS,_DtTermPrimParserNextState,  },
 {  0x00,0xFF, &_DtTermStateGetTitle, _DtTermSaveChar,   },
};
StateTableRec _DtTermStateGetTitle  = {False, get_title_table};

/*
** this is table used when "Esc ] L<string>Esc"  has been seen
*/
static StateEntryRec   
get_iconBS_table[] =
{
 {  '\\', '\\', &stateStart, _DtTermParserSunWindowIcon, },
 {  0x00,0xFF, &_DtTermStateGetIcon, _DtTermSaveChar,   },
};
StateTableRec _DtTermStateGetIconBS  = {False, get_iconBS_table};

/*
** this is table used when "Esc ] L"  has been seen
*/
static StateEntryRec   
get_icon_table[] =
{
 {  0x1B, 0x1B,&_DtTermStateGetIconBS , _DtTermPrimParserNextState, },
 {  0x00,0xFF, &_DtTermStateGetIcon, _DtTermSaveChar,   },
};
StateTableRec _DtTermStateGetIcon  = {False, get_icon_table};

/*
** this is table used when "Esc ] I<file>Esc"  has been seen
*/
static StateEntryRec
get_fileBS_table[] =
{
 {  '\\', '\\', &stateStart, _DtTermParserSunIconFile, },
 {  0x00,0xFF, &_DtTermStateGetIconFile, _DtTermSaveChar, },
};
StateTableRec _DtTermStateGetIconFileBS  = {False, get_fileBS_table};

/*
** this is table used when "Esc ] I"  has been seen
*/
static StateEntryRec
get_file_table[] =
{
 {  0x1B, 0x1B,&_DtTermStateGetIconFileBS , _DtTermPrimParserNextState, },
 {  0x00,0xFF, &_DtTermStateGetIconFile, _DtTermSaveChar, },
};
StateTableRec _DtTermStateGetIconFile  = {False, get_file_table};

/*
** CDExc21154:
** this is table used when "Esc ] number ;<string>Esc"  has been seen
*/
static StateEntryRec
get_stringBS_table[] =
{
 {  '\\', '\\', &stateStart, _DtTermPrimParserNextState,/*valid terminator*/ },
 {  0x07, 0x07, &stateStart, _DtTermChangeTextParam,/*new Icon,Title,etc*/ },
 {  0x00, 0xFF, &_DtTermStateGetString, _DtTermSaveChar,   /* text*/ },
};
StateTableRec _DtTermStateGetStringBS  = {False, get_stringBS_table};

/*
** this is table used when "Esc ] number ;"  has been seen
*/
static StateEntryRec   
get_string_table[] =
{
 {  0x07, 0x07, &stateStart, _DtTermChangeTextParam,/*new Icon,Title,etc*/ },
 {  0x1B, 0x1B, &_DtTermStateGetStringBS , _DtTermPrimParserNextState, },
 {  0x00,0xFF, &_DtTermStateGetString, _DtTermSaveChar,   /* text*/ },
};
StateTableRec _DtTermStateGetString  = {False, get_string_table};

/*
** this is table used when esc ] has been seen
*/
static StateEntryRec
right_bracket_table[] =
{
 {  ';',  ';', &_DtTermStateGetString, _DtTermParsePushNum,/* increment count */ },
 {  '0',  '9', &_DtTermStateRightBracket,  _DtTermEnterNum, /* enter number */ },
 {  'l',  'l', &_DtTermStateGetTitle,  _DtTermPrimParserNextState,  },
 {  'L',  'L', &_DtTermStateGetIcon, _DtTermPrimParserNextState, },
 {  'I',  'I', &_DtTermStateGetIconFile,  _DtTermPrimParserNextState, },
 {  0x00, 0xFF, &stateStart,       _DtTermPrintState     /* end of table */ },
};
StateTableRec _DtTermStateRightBracket  = {False, right_bracket_table};

/*
** this is table used when esc [ has been seen and '?' is not in the follow
*/
static StateEntryRec
left_bracket_table_no_Q[] =
{
 {  ';',    ';', &_DtTermStateLeftBracketNotQ,_DtTermParsePushNum, },
 {  '0',    '9', &_DtTermStateLeftBracketNotQ, _DtTermEnterNum, },
 {  '@',    '@', &stateStart,  _DtTermInsertChars, /* IL Insert Blank char*/ },
 {  'A',    'A', &stateStart,  _DtTermCursorUp,      /* CUU Cursor up n */ },
 {  'B',    'B', &stateStart,  _DtTermCursorDown,    /* CUD Cursor down n*/ },
 {  'C',    'C', &stateStart,  _DtTermCursorForward,/* CUF Cursor forward n*/ },
 {  'D',    'D', &stateStart,  _DtTermCursorBack,    /* CUB Cursor back n*/ },
 {  'F',    'F', &stateStart,  _DtTermCursorToLineUp,/* CPL Curs to Line up*/ },
 {  'G',    'G', &stateStart,  _DtTermCursorToCol,  /* CHA Curs to Col */ },
 {  'H',    'H', &stateStart,  _DtTermCursorPos ,   /* Cursor Position */ },
 {  'J',    'J', &stateStart,  _DtTermEraseDisplay, /* Erase in Display */ },
 {  'K',    'K', &stateStart,  _DtTermEraseInLine, /* EL Erase in Line  */ },
 {  'L',    'L', &stateStart,  _DtTermInsertLines, /* IL Insert Lines  */ },
 {  'M',    'M', &stateStart,  _DtTermDeleteLines, /* DL Delete Lines  */ },
 {  'S',    'S', &stateStart,  _DtTermScrollUp   , /* SU Scroll Up p lines*/ },
 {  'T',    'T', &stateStart,  _DtTermScrollDown ,/*SD Scroll DOwn p lines*/ },
 {  'X',    'X', &stateStart,  _DtTermEraseChars , /* ECH Erase chars   */ },
 {  'c',    'c', &stateStart,  _DtTermDeviceAttributes, /* DA */ },
 {  'P',    'P', &stateStart,  _DtTermDeleteChars, /* DCH Delete Chars */ },
 {  'f',    'f', &stateStart,  _DtTermCursorPos,   /* Cursor Position  */ },
 {  'g',    'g', &stateStart,  _DtTermTabClear,  /* TBC  */ },
 {  'h',    'h', &stateStart,  _DtTermSetMode,   /* SM  */ },
 {  'l',    'l', &stateStart,  _DtTermResetMode,   /* RM  */ },
 {  'm',    'm', &stateStart,  _DtTermCharAttributes, /* SGR  */ },
 {  'n',    'n', &stateStart,  _DtTermDeviceStatusAnsi, /* DSR  */ },
 {  'r',    'r', &stateStart,  _DtTermScrollingRegion, /* DECSTBM */ },
 {  't',    't', &stateStart,  _DtTermParseSunMisc, /* misc sun esc seq */ },
 {  'x',    'x', &stateStart,  _DtTermRequestParam, /* DECREQTPARM */ },
 {  '"',    '"', &_DtTermStateDECSCA, _DtTermPrimParserNextState,/* sel erase */ },
 {  '!',    '!', &_DtTermStateSoftReset, _DtTermPrimParserNextState     , /* set erase attr */ },
 {  0x00, 0xFF, &stateStart,       _DtTermPrintState     /* end of table */ },
};
StateTableRec _DtTermStateLeftBracketNotQ  = {False, left_bracket_table_no_Q, pre_parse_table};

/*
** this is table used when esc [ has been seen
*/
static StateEntryRec
left_bracket_table[] =
{
 {  '?',    '?', &_DtTermStateEscQuestion, _DtTermPrimParserNextState,/*DECSET*/ },
 {  ';',    ';', &_DtTermStateLeftBracketNotQ ,_DtTermParsePushNum, },
 {  '0',    '9', &_DtTermStateLeftBracketNotQ ,  _DtTermEnterNum, },
 {  '@',    '@', &stateStart,  _DtTermInsertChars, /* IL Insert Blank char*/ },
 {  'A',    'A', &stateStart,  _DtTermCursorUp,      /* CUU Cursor up n */ },
 {  'B',    'B', &stateStart,  _DtTermCursorDown,    /* CUD Cursor down n*/ },
 {  'C',    'C', &stateStart,  _DtTermCursorForward,/* CUF Cursor forward n*/ },
 {  'D',    'D', &stateStart,  _DtTermCursorBack,    /* CUB Cursor back n*/ },
 {  'F',    'F', &stateStart,  _DtTermCursorToLineUp,/* CPL Curs to Line up*/ },
 {  'G',    'G', &stateStart,  _DtTermCursorToCol,  /* CHA Curs to Col */ },
 {  'H',    'H', &stateStart,  _DtTermCursorPos ,   /* Cursor Position */ },
 {  'J',    'J', &stateStart,  _DtTermEraseDisplay, /* Erase in Display */ },
 {  'K',    'K', &stateStart,  _DtTermEraseInLine, /* EL Erase in Line  */ },
 {  'L',    'L', &stateStart,  _DtTermInsertLines, /* IL Insert Lines  */ },
 {  'M',    'M', &stateStart,  _DtTermDeleteLines, /* DL Delete Lines  */ },
 {  'S',    'S', &stateStart,  _DtTermScrollUp   , /* SU Scroll Up p lines*/ },
 {  'T',    'T', &stateStart,  _DtTermScrollDown ,/*SD Scroll DOwn p lines*/ },
 {  'X',    'X', &stateStart,  _DtTermEraseChars , /* ECH Erase chars   */ },
 {  'c',    'c', &stateStart,  _DtTermDeviceAttributes, /* DA */ },
 {  'P',    'P', &stateStart,  _DtTermDeleteChars, /* DCH Delete Chars */ },
 {  'f',    'f', &stateStart,  _DtTermCursorPos,   /* Cursor Position  */ },
 {  'g',    'g', &stateStart,  _DtTermTabClear,  /* TBC  */ },
 {  'h',    'h', &stateStart,  _DtTermSetMode,   /* SM  */ },
 {  'l',    'l', &stateStart,  _DtTermResetMode,   /* RM  */ },
 {  'm',    'm', &stateStart,  _DtTermCharAttributes, /* SGR  */ },
 {  'n',    'n', &stateStart,  _DtTermDeviceStatusAnsi, /* DSR  */ },
 {  'r',    'r', &stateStart,  _DtTermScrollingRegion, /* DECSTBM */ },
 {  't',    't', &stateStart,  _DtTermParseSunMisc, /* misc sun esc seq */ },
 {  'x',    'x', &stateStart,  _DtTermRequestParam, /* DECREQTPARM */ },
 {  '"',    '"', &_DtTermStateDECSCA, _DtTermPrimParserNextState,/* sel erase */ },
 {  '!',    '!', &_DtTermStateSoftReset, _DtTermPrimParserNextState     , /* set erase attr */ },
 {  0x00, 0xFF, &stateStart,       _DtTermPrintState     /* end of table */ },
};
StateTableRec _DtTermStateLeftBracket  = {False, left_bracket_table, pre_parse_table};

/*
** this is table used when esc [ ! has been seen
*/
static StateEntryRec
soft_reset_table[] =
{
 {  'p',    'p',    &stateStart , _DtTermSoftReset, /* End of text */ },
 {  0x00, 0xFF, &stateStart   ,  _DtTermPrintState      /* end of table*/ },
};
StateTableRec _DtTermStateSoftReset = {False, soft_reset_table, pre_parse_table};

/*
** this is table used when CSI " has been seen
*/
static StateEntryRec
set_erase_table[] =
{
 {  'p',    'p',    &stateStart , _DtTermSetCompatLevel, /* DECSCL */ },
 {  'q',    'q',    &stateStart , _DtTermSetCharEraseMode, /* set sel erase */ },
 {  0x00, 0xFF, &stateStart   ,  _DtTermPrintState      /* end of table*/ },
};
StateTableRec _DtTermStateDECSCA = {False, set_erase_table, pre_parse_table};

/*
** this is table used when esc ^ ... esc has been seen
*/
static StateEntryRec
up_backslash_table[] =
{
 {  '\\', '\\',&stateStart , _DtTermPrimParserNextState, /* End of text */ },
 {  0x00, 0xFF, &_DtTermStateUpArrow,   _DtTermPrimParserNextState  },
};
StateTableRec _DtTermStateUA_BS = {False, up_backslash_table};

/*
** this is table used when esc ^ has been seen
*/
static StateEntryRec
uparrow_table[] =
{
 {  0x1B, 0x1B,&_DtTermStateUA_BS , _DtTermPrimParserNextState, /* Esc */ },
 {  0x9C, 0x9C, &stateStart, _DtTermPrimParserNextState,        /* ST  */ },
 {  0x00, 0xFF, &_DtTermStateUpArrow,   _DtTermPrimParserNextState  },
};
StateTableRec _DtTermStateUpArrow  = {False, uparrow_table};

/*
** this is table used when esc _ ... esc has been seen
*/
static StateEntryRec
under_backslash_table[] =
{
 {  '\\', '\\',&stateStart , _DtTermPrimParserNextState, /* End of text */ },
 {  0x00, 0xFF, &_DtTermStateUnderscore,  _DtTermPrimParserNextState  /* Go back to Underscore*/ },
};
StateTableRec _DtTermStateUS_BS = {False, under_backslash_table};

/*
** this is table used when esc _ has been seen
*/
static StateEntryRec
underscore_table[] =
{
 {  0x1B, 0x1B,&_DtTermStateUS_BS , _DtTermPrimParserNextState, /* Esc */ },
 {  0x9C, 0x9C, &stateStart, _DtTermPrimParserNextState,  /* ST */ },
 {  0x00, 0xFF, &_DtTermStateUnderscore,  _DtTermPrimParserNextState  },
};
StateTableRec _DtTermStateUnderscore  = {False, underscore_table};

/*
** this is the table used when an CSI ? has been seen
*/
static StateEntryRec
esc_question_table[] =
{
 {  '0', '9', &_DtTermStateEscQuestion , _DtTermEnterNum,     },
 {  'h', 'h', &stateStart , _DtTermPModeSet,   /* DECSET private mode*/ },
 {  'l', 'l', &stateStart , _DtTermPModeReset,  /*DECSET private mode*/ },
 {  'n', 'n', &stateStart , _DtTermDeviceStatus, /*DECSET private mode*/ },
 {  'r', 'r', &stateStart , _DtTermRestoreModeValues, /*restore Mode values*/ },
 {  's', 's', &stateStart , _DtTermSaveModeValues,    /*save Mode values*/ },
 {  'K', 'K', &stateStart   ,    _DtTermSelEraseInLine, /* DECSEL */ },
 {  'J', 'J', &stateStart   ,    _DtTermSelEraseInDisplay,/* DECSEL */ },
 {  0x00, 0xFF, &stateStart   ,  _DtTermPrintState      /* end of table*/ },
};
StateTableRec _DtTermStateEscQuestion  = {False, esc_question_table, pre_parse_table};

/*
** this is the table used when an Esc ~ has been seen
*/
static StateEntryRec
tilda_table[] =
{
 {  0x1B, 0x1B,&stateStart,_DtTermInvokeG1, /* LS1R Invoke G1 Char set */ },
 {  0x00, 0xFF, &stateStart   ,      _DtTermPrintState   /* end of table */ },
};
StateTableRec _DtTermStateTilda  = {False, tilda_table, pre_parse_table};

/*
 * this is the table used when an DCS p1;p2 | p3/ string Esc  has been seen
*/
static StateEntryRec
sk_backslash_table[] =
{
 {  '\\', '\\',&stateStart,  _DtTermParseUserKeyLoadLast, /* ST    */ },
 {  0x00, 0xFF, &_DtTermStateIsNextBS_SK, _DtTermPrimParserNextState/*toss*/ },
};
StateTableRec _DtTermStateIsNextBS_SK  = {False, sk_backslash_table};

/*
** this is the table used when an DCS p1;p2 |  p3/ has been seen
*/
static StateEntryRec
user_key_string_table[] =
{
 {  '0','9', &_DtTermStateUserKeyString, _DtTermParseHexDigit, /*hex digit*/ },
 {  'a','f', &_DtTermStateUserKeyString, _DtTermParseHexDigit, /*hex digit*/ },
 {  'A','F', &_DtTermStateUserKeyString, _DtTermParseHexDigit, /*hex digit*/ },
 {  ';',  ';', &_DtTermStateUserKey,     _DtTermParseUserKeyLoad,/*load string*/ },
 {  0x1B, 0x1B, &_DtTermStateIsNextBS_SK, _DtTermPrimParserNextState,/*Esc*/ },
 {  0x9C, 0x9C, &stateStart, _DtTermParseUserKeyLoadLast,  /* ST */ },
 {  0x00, 0xFF, &stateStart   ,      _DtTermPrintState   /* end of table */ },
};
StateTableRec _DtTermStateUserKeyString  = {False, user_key_string_table};

/*
** this is the table used when an DCS p1;p2 |  has been seen
*/
static StateEntryRec
softkey_table[] =
{
 {  '0',  '9', &_DtTermStateUserKey,     _DtTermEnterNum,     /* enter number */ },
 {  '/', '/',&_DtTermStateUserKeyString,_DtTermPrimParserNextState,/*store key*/ },
 {  0x1B, 0x1B, &_DtTermStateIsNextBS_SK, _DtTermPrimParserNextState,/*Esc*/ },
 {  0x9C, 0x9C, &stateStart, _DtTermParseUserKeyLoadLast,  /* ST */ },
 {  0x00, 0xFF, &stateStart   ,      _DtTermPrintState   /* end of table */ },
};
StateTableRec _DtTermStateUserKey  = {False, softkey_table};

/*
 * this is the table used when an DCS p1;p2 { ..... Esc  has been seen
*/
static StateEntryRec
dl_backslash_table[] =
{
 {  '\\', '\\',&stateStart,  _DtTermLoadCharSet, /* ST            */ },
 {  0x20,0x7E, &_DtTermStateP, _DtTermPrimParserNextState, /* all text */ },
 {  0x00, 0xFF, &stateStart   ,      _DtTermPrintState   /* end of table */ },
};
StateTableRec _DtTermStateIsNextBS_DL = {False, dl_backslash_table};

/*
** this is the table used when an DCS (ESC P) p1;p2... {  has been seen
*/
static StateEntryRec
dl_char_table[] =
{
 {  '\\', '\\',&stateStart,  _DtTermLoadCharSet ,        /* ST  */ },
 {  0x20,0x7E, &_DtTermStateDLCharSet, _DtTermPrimParserNextState,/* text */ },
 {  0x1B, 0x1B,&_DtTermStateIsNextBS_DL,_DtTermPrimParserNextState,/*end?*/ },
 {  0x9C, 0x9C, &stateStart, _DtTermLoadCharSet,  /* C1 string terminator*/ },
 {  0x00, 0xFF, &stateStart   ,      _DtTermPrintState   /* end of table */ },
};
StateTableRec _DtTermStateDLCharSet  = {False, dl_char_table};

/*
** this is the table used when an Esc P  (DCS) has been seen
*/
static StateEntryRec
esc_P_table[] =
{
 {  ';',  ';', &_DtTermStateP, _DtTermParsePushNum,  /*increment count*/ },
 {  '0',  '9', &_DtTermStateP,  _DtTermEnterNum,     /* enter number */ },
 {  '|',  '|',  &_DtTermStateUserKey, _DtTermParseUserKeyClear, /* DECUDK  */ },
 {  '{',  '{',  &_DtTermStateDLCharSet, _DtTermPrimParserNextState, },
 {                                                /* down-line-load char set */ },
 {  0x00, 0xFF, &stateStart   ,      _DtTermPrintState   /* end of table */ },
};
StateTableRec _DtTermStateP  = {False, esc_P_table, pre_parse_table};


/*
** this is the table used when an error occurred in an escape sequence
*/
static StateEntryRec
error_table[] =
{
 {     'A',    'Z', &stateStart, _DtTermPrimParserNextState, /* legal char */ },
 {  0x00, 0xFF, &_DtTermStateError, _DtTermPrimParserNextState  /* end  */ },
};

StateTableRec _DtTermStateError = {False, error_table};

