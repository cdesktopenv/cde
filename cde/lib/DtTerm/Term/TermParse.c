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
static char rcs_id[] = "$TOG: TermParse.c /main/3 1999/10/15 12:25:13 mgreess $";
#endif	/* VERBOSE_REV_INFO */
#endif	/* lint */

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
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimI.h"
#include "TermP.h"
#include "TermPrimData.h"
#include "TermData.h"
#include "TermPrimBuffer.h"
#include "TermPrimParserP.h"
#include "TermFunction.h"
#include "TermBuffer.h"
#include "TermFunctionKey.h"
#include "TermParseTableP.h"
#include "TermBufferP.h"

#if defined (__hpux)
/*
 * On HP MAXINT is defined in both <values.h> and <sys/param.h>
 */
#undef MAXINT
#endif
#if defined(CSRG_BASED)
#define MAXSHORT SHRT_MAX
#else
#include  <values.h>
#endif

#if defined(USL) || defined(__uxp__)
#include <ctype.h>
#include <wctype.h>
#endif

/*****************************************************************************/

#define NPARAM  16
#define PCOUNT(c)  ((c)->parms[0])
#define BASE   1   /* row and column count base 0 or 1 */

/*********************************************************************
 *  Macros for handling cursor constraints - return values in coordinate
 *  system for Term widget (base 0)  BASE which is 1 reflects the Vt base
 *  
 *  These defines need to account of for Origin mode eventually  TMH */

#define TOPROW(w)    (1-BASE)
#define TOPMARGIN(w) ((w)->term.tpd->scrollLockTopRow-(1-BASE))
#define BOTTOMMARGIN(w) ((w)->term.tpd->scrollLockBottomRow-(1-BASE))
#define BOTTOMROW(w) ((w)->term.rows-BASE) 
/* FIRSTCOLUMN(w)  ABS(BASE-1) */
#define FIRSTCOLUMN(w) 0
#define LASTCOLUMN(w) ((w)->term.columns-BASE)
#define FLOOR(w)  ((w)->vt.td->originMode?(w)->term.tpd->scrollLockBottomRow:\
                                             BOTTOMROW(w))
#define CEILING(w)  ((w)->vt.td->originMode?(w)->term.tpd->scrollLockTopRow:\
                                             TOPROW(w))
#define KILLWRAP(w) if((w)->term.tpd->cursorColumn>LASTCOLUMN(w)) \
                                (w)->term.tpd->cursorColumn=LASTCOLUMN(w)
#define STORELASTARG(c) (c)->parms[PCOUNT(c)]=(c)->workingNum

/******************************************************************
 *   Upon entering a parse routine the parameter count is contained 
 *   in  context->parms[0].
 *                                        
 *   The last parmameter parsed is in context->workingNum
 *   Next to last parmameter is in {context->parms[0]==1?context->workingNum:
 *                                  context->parms[context->param[0]-1]}
 *                    .
 *                    .
 *
 *   First parameter is in  context->parms[1]
 *
 *   In some of the parse routines, the default case of too many parameters
 *   needs to be handled like Vt (silently use the first n parameters)
 *   Vt has a max of 16 numeric parameters;  The rest fall off the end
 *   silently.
 *******************************************************************/

void 
_DtTermClearParam(Widget w)
{
    ParserContext context ;
    context = GetParserContext(w) ;
  
    PCOUNT(context)=0 ;
    _DtTermPrimParserClearParm(w) ;
    _DtTermPrimParserClrStrParm(w) ;
}

void 
_DtTermEnterNum(Widget w)
{
  ParserContext context ;
  context = GetParserContext(w) ;
  _DtTermPrimParserEnterNum(w) ;
  if (!PCOUNT(context)) PCOUNT(context)++ ; /* increment for first number ONLY*/
}

void 
_DtTermParsePushNum(Widget w)
{
  ParserContext context ;
  context = GetParserContext(w) ;
  /* parmN(w,PCOUNT(c)) ;  Should  use this but not global   */
  if ( context->workingNumIsDefault != True) {  /* did we actually parse a num*/
     _DtTermPrimParserNumParmPush(w,PCOUNT(context)) ;
     }
  else {
   if (!PCOUNT(context)) PCOUNT(context)++ ;/* increment for first number ONLY*/
  }
    
  if ( ++PCOUNT(context)>NPARAM ) {
    Debug('P',fprintf(stderr,">>Too many parameters\n")) ;
    return ;
  }
}

void
_DtTermSaveChar(Widget w)
{
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  ParserContext context ;
  wchar_t c;
  context = GetParserContext(w) ;

  mbtowc(&c,(char *)context->inputChar,MB_CUR_MAX);
  if ( context->stringParms[0].length < STR_SIZE && iswprint(c) ) {
      strncpy( (
         (char *)&context->stringParms[0].str[context->stringParms[0].length]), 
                (char *)context->inputChar, context->inputCharLen) ;
   context->stringParms[0].length += context->inputCharLen ;
  }
}

void
_DtTermPrintState(Widget w)
{
  ParserContext context ;
  context = GetParserContext(w) ;
  /* fprintf(stderr,"Warning: Not a valid escape sequence\n") ;*/
  context->stateTable =  _DtTermStateStart ;
  _DtTermClearParam(w) ;
}

/* a utility for checking for S8C1TMode */
static void
sendEscSequence(Widget w, char *transmitString)
{
    DtTermWidget    tw  = (DtTermWidget)w;
    DtTermData      td = tw->vt.td;

    if ( td->S8C1TMode ) {
           char *cbuf =malloc(strlen(transmitString)+1);
           strcpy(cbuf,transmitString) ;
           cbuf[1] = 0x9B ;
           (void) _DtTermPrimSendInput(w, (unsigned char *) (cbuf+1),
                strlen(cbuf+1));
           free(cbuf) ;
         }
        else {
           (void) _DtTermPrimSendInput(w, (unsigned char *) transmitString,
                strlen(transmitString));
        }
}

void 
_DtTermDeviceStatus(Widget w)     /* DSR CSI?pn  */
{
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermData vtd = ((DtTermWidget)w)->vt.td;
  ParserContext context ;
  char buf[100] ;
  int col,cnt ;
  Debug('P', fprintf(stderr,">>In func  _DtTermDeviceStatus\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  switch ( context->parms[1] )  {
     case 15: /* printer status - we don't have one         */
             sendEscSequence(w,"\033[?13n") ;
             break;
     case 25: /* User defined keys locked?  */
             if ( vtd->userKeysLocked )
                sendEscSequence(w,"\033[?21n") ;
             else
                sendEscSequence(w,"\033[?20n") ;
             break;
     case 26: /* keyboard status - always send North American */
             sendEscSequence(w,"\033[?27;1n") ;
             break;
  }
}

void 
_DtTermDeviceStatusAnsi(Widget w)     /* DSR CSIpn  */
{
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermData vtd = ((DtTermWidget)w)->vt.td;
  ParserContext context ;
  char buf[100] ;
  int row,col,cnt ;
  Debug('P', fprintf(stderr,">>In func  _DtTermDeviceStatus\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  switch ( context->parms[1] )  {
     case 5: /* as far as I know, all's well... So send OK */
             sendEscSequence(w,"\033[0n") ;
             break;
     case 6:
       /* cursor could be past last row (for autowrap) */
       if ((col=tw->term.tpd->cursorColumn)>LASTCOLUMN(tw)) 
                     col=LASTCOLUMN(tw) ;
       row = tw->term.tpd->cursorRow+BASE ;
       if (vtd->originMode==True) row -= TOPMARGIN(tw) ;
       sprintf(buf,"\033[%d;%dR",row , col+BASE) ;
       sendEscSequence(w,buf) ;
       break ;
  }
}

void
_DtTermPModeSet(Widget w)   /*  DECSET CSI?ph */
{
  ParserContext context ;
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermWidget vtw = (DtTermWidget)w;
  int i,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermPModeSet\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  for (i=1;i<=cnt;i++) {
      switch ( context->parms[i]) {
          /* from vt220 */
          case 1: /* Cursor key (Application) */
                  vtw->vt.td->applicationMode=True;
                  break;
          /* case 2:  can't return from vt52 mode */
          case 3: /* Column (132) */
                   if ( vtw->vt.c132 )  {  /* xterm allow 132 mode */
                     (void) _DtTermFuncClearBuffer(w, NULL, fromParser);
                     vtw->term.tpd->cursorRow = TOPROW(vtw) ;
                     vtw->term.tpd->cursorColumn = FIRSTCOLUMN(vtw) ;
                     vtw->term.tpd->scrollLockTopRow = TOPROW(vtw);
                     vtw->term.tpd->scrollLockBottomRow = BOTTOMROW(vtw) ;
                     XtVaSetValues(w, DtNcolumns, 132, NULL) ;
                     vtw->vt.td->col132Mode = True ;
                  }
                  break;
          case 4: /* Scroll (smooth) */
                  _DtTermPrimScrollComplete(w, True) ;
                  vtw->term.jumpScroll = False ;
                  break;
          case 5: /* Screen (Reverse) */
		  /*DKS: this is just to test out the functionality... */
		  vtw->term.reverseVideo = True;
		  /* clear the window to force refresh... */
		  (void) XClearArea(XtDisplay(w), XtWindow(w),
			0, 0, 0, 0, True);
		  break;
          case 6: /* Origin mode (On) */
                  vtw->vt.td->originMode=True;
                  _DtTermPrimCursorMove(w, TOPMARGIN(vtw), 0);
                  break;
          case 7: /* Auto wrap (On) */
                  vtw->term.tpd->autoWrapRight=True ;
                  break;
          /* case 8: ** Auto repeat (On) 
           *      ** This is handled by the X server 
           *      break;
           * 
           * case 18: ** Print form feed (On) 
           *      break;
           * case 19: ** Print extent (full screen)
           *      break;
           */
          case 25: /* Text cursor enable (On) */
                  (void) _DtTermPrimSetCursorVisible(w, True);
                  break;
           /* from xterm */
          case 40: /*  80/132 mode  (allow) */
                  vtw->vt.c132 = True ;
                  break;
          case 41: /*  curses fix */
                  vtw->vt.td->fixCursesMode = True ;
                  break;
          case 44: /* margin bell (on) */
                  vtw->term.marginBell = True ;
                  break;
          case 45: /* Reverse-wraparound mode */
                  vtw->vt.td->reverseWrapMode=True;
                  break;
          case 46: /* logging (start)  */
                  vtw->term.logging = True ;
                  _DtTermPrimStartLog(w);
                  break;
          /*  case 47: ** screen buffer (alternate)  
           *      break;
           *  case 1000: ** Send mouse x and y 
           *      break;
           *  case 1001: ** Use hilite mouse tracking 
           *      break;
           */
      }
   }
}


void
_DtTermPModeReset(Widget w)   /* DECRST CSI?pl */
{
  ParserContext context ;
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermWidget vtw = (DtTermWidget)w;
  int i,cnt ;
  Debug ('P', fprintf(stderr,">>In func _DtTermPModeReset\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  for (i=1;i<=cnt;i++) {
      switch ( context->parms[i]) {
          /* from vt220 */
          case 1: /* Cursor key (Normal) */
                  vtw->vt.td->applicationMode=False;
                  break;
          /* case 2:   ** set vt52 mode */
          case 3: /* Column (80) */
                  if (vtw->vt.c132)  {   /* allow 132 mode */
                    (void) _DtTermFuncClearBuffer(w, NULL, fromParser);
                    vtw->term.tpd->cursorRow = TOPROW(vtw) ;
                    vtw->term.tpd->cursorColumn = FIRSTCOLUMN(vtw) ;
                    vtw->term.tpd->scrollLockTopRow = TOPROW(vtw);
                    vtw->term.tpd->scrollLockBottomRow = BOTTOMROW(vtw) ;
                    XtVaSetValues(w, DtNcolumns, 80, NULL) ;
                    vtw->vt.td->col132Mode = False ;
                  }
                  break;
          case 4: /* Scroll (jump) */
                  _DtTermPrimScrollComplete(w, True) ;
                  vtw->term.jumpScroll = True ;
                  break;
          case 5: /* Screen (normal ) */
		  /*DKS: this is just to test out the functionality... */
		  vtw->term.reverseVideo = False;
		  /* clear the window to force refresh... */
		  (void) XClearArea(XtDisplay(w), XtWindow(w),
			0, 0, 0, 0, True);
                  break;
          case 6: /* Normal cursor (not Origin mode) */
                  vtw->vt.td->originMode=False;
                  _DtTermPrimCursorMove(w, 0, 0);
                  break;
          case 7: /* Auto wrap (Off) */
                  vtw->term.tpd->autoWrapRight=False;
                  break;
          /* case 8: ** Auto repeat (Off) 
           *      ** implemented in the server 
           *      break;
           * case 18: ** Print form feed (Off) 
           *      break;
           * case 19: ** Print extent (scrolling region)
           *      break;
           */
          case 25: /* Text cursor enable (Off) */
                  (void) _DtTermPrimSetCursorVisible(w, False);
                  break;
           /* from xterm */
          case 40: /*  80/132 mode  (disallow) */
                  vtw->vt.c132 = False;
                  break;
          case 41: /*  curses fix  */
                  vtw->vt.td->fixCursesMode = False;
                  break;
          case 44: /* margin bell (off) */
                  vtw->term.marginBell = False ;
                  break;
          case 45: /* Reverse-wraparound mode */
                  vtw->vt.td->reverseWrapMode=False;
                  break;
          case 46: /* logging (stop )  */
                  vtw->term.logging = False ;
                  _DtTermPrimCloseLog(w) ;
                  break;
          /* case 47: ** screen buffer  (normal)
           *      break;
           * case 1000: ** don't send mouse x and y 
           *      break;
           * case 1001: ** don't use hilite mouse tracking 
           *      break;
           */
      }
  }
}

void
_DtTermSetMode(Widget w)   /* SM CSIph */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget)w ;
  int i,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermSetMode\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  for (i=1;i<=cnt;i++) {
      switch ( context->parms[i]) {
          case 2: /* KAM Keyboard Action (locked) */
                  vtw->term.tpd->keyboardLocked.escape=True ;
		  (void) _DtTermPrimInvokeStatusChangeCallback(w);
                  break;
          case 4: /* Insert mode */
                  _DtTermPrimInsertCharUpdate(w, DtTERM_INSERT_CHAR_ON) ;
                  break;
          case 12: /* SRM Send/Receive (Off) */
		  vtw->term.tpd->halfDuplex = False;
                  break;
          case 20: /* Line feed/newline (new line) */
                  vtw->term.tpd->autoLineFeed=True ;
                  break;
           }
    }
}

void 
_DtTermResetMode(Widget w)   /* RM CSIpl  */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget)w ;
  int i,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermResetMode\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  for (i=1;i<=cnt;i++) {
      switch ( context->parms[i]) {
          case 2: /* KAM Keyboard Action (unlocked) */
                  vtw->term.tpd->keyboardLocked.escape=False;
		  (void) _DtTermPrimInvokeStatusChangeCallback(w);
                  break;
          case 4: /* Replace mode */
                  _DtTermPrimInsertCharUpdate(w, DtTERM_INSERT_CHAR_OFF) ;
                  break;
          case 12: /* SRM Send/Receive (On) */
		  vtw->term.tpd->halfDuplex = True;
                  break;
          case 20: /* Line feed/newline (line feed) */
                  vtw->term.tpd->autoLineFeed=False ;
                  break;
           }
    }
}
 
void 
_DtTermCursorPos(Widget w)   /* CUP CSIp;pH */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int row, col;
  Debug( 'P', fprintf(stderr,">>In func _DtTermCursorPos\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  row = context->parms[1] ;
  col = context->parms[2] ;
  if (row)row-=BASE ;
  if (col)col-=BASE ;
  if (vtw->vt.td->originMode) row += vtw->term.tpd->scrollLockTopRow ;
  if (row > FLOOR(vtw)) row = FLOOR(vtw) ;
  if (col > LASTCOLUMN(vtw)) col = LASTCOLUMN(vtw) ;
  vtw->term.tpd->cursorRow = row ;
  vtw->term.tpd->cursorColumn = col ;
}

void 
_DtTermEraseDisplay(Widget w)    /* ED CSIpJ */
{
  int cnt ;
  DtTermWidget vtw = (DtTermWidget) w;
  ParserContext context ;
  DtEraseMode   eraseMode;
  int  row, col;
  
  Debug('P', fprintf(stderr,">>In func _DtTermEraseDisplay\n")) ;
  context = GetParserContext(w) ;
  KILLWRAP(vtw) ;
  STORELASTARG(context) ;
  row = vtw->term.tpd->cursorRow;
  col = vtw->term.tpd->cursorColumn ;

  /*
  ** pick the correct erase mode...
  */
  switch (context->parms[1])
  {
    case 0: 
      eraseMode = eraseToEOB;
      break;
    case 1: 
      eraseMode = eraseFromRow0Col0;
      break;
    case 2: 
      eraseMode = eraseBuffer;
      break ; 
   }
   _DtTermFuncEraseInDisplay(w, (int)eraseMode, fromParser);
  vtw->term.tpd->cursorRow = row;
  vtw->term.tpd->cursorColumn = col;
}

void 
_DtTermEraseChars(Widget w)   /* ECH  CSIpX */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int row,col,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermEraseChars\n")) ;
  KILLWRAP(vtw) ;
  context = GetParserContext(w) ;
  row = vtw->term.tpd->cursorRow ;
  col = vtw->term.tpd->cursorColumn ;
  STORELASTARG(context) ;
  cnt = context->parms[1] ;
  if (!(cnt))cnt=1;
  else 
    if ( cnt > LASTCOLUMN(vtw)-col+1 ) cnt = LASTCOLUMN(vtw)-col+1 ;

  _DtTermFuncEraseCharacter(w, cnt, fromParser);
}

void 
_DtTermInsertChars(Widget w)    /* ICH  CSIp@ */
{
    DtTermWidget vtw = (DtTermWidget) w;
    ParserContext context ;
    int row, col, cnt ;
    DtTermInsertCharMode	saveInsertCharMode;

    Debug('P', fprintf(stderr,">>In func _DtTermInsertChars\n")) ;
    KILLWRAP(vtw) ;
    context = GetParserContext(w) ;
    vtw->term.tpd->parserNotInStartState = False ;
    row = vtw->term.tpd->cursorRow ;
    col = vtw->term.tpd->cursorColumn ;
    STORELASTARG(context) ;
    cnt = context->parms[1] ;
    if (!cnt)
	cnt++;
    else
	if (cnt > LASTCOLUMN(vtw) - col + 1)
	    cnt = LASTCOLUMN(vtw) - col + 1;

    /* save away the insert char mode... */
    saveInsertCharMode = vtw->term.tpd->insertCharMode;

    /* turn on insert char mode.  We don't want to invoke any status
     * change callbacks, so we will justs set it directly...
     */
    vtw->term.tpd->insertCharMode = DtTERM_INSERT_CHAR_ON;

    /* insert blanks... */
    while (cnt--)
	(void) _DtTermPrimInsertText(w,(unsigned char *) " ", 1) ;

    /* restore the insert char mode... */
    vtw->term.tpd->insertCharMode = saveInsertCharMode;

    _DtTermPrimCursorMove(w,row,col) ;
}

void 
_DtTermCursorUp(Widget w)    /* CUU  CISpA */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int trow, row, tmargin, cnt;
  Debug('P', fprintf(stderr,">>In func _DtTermCursorUp\n")) ;
  context = GetParserContext(w) ;
  KILLWRAP(vtw) ;
  tmargin = TOPMARGIN(vtw) ;
  trow = TOPROW(vtw) ;
  STORELASTARG(context) ;
  cnt = context->parms[1] ;
  if (!(cnt))cnt=1;
  else if (cnt>MAXSHORT) cnt=MAXSHORT-100;
  row = vtw->term.tpd->cursorRow-cnt;
  /* are we about the top margin and did we actually cross it? */
  /* This handles the case top margin == top row   */
  if (row < tmargin && row+cnt >= tmargin) row = tmargin;
  _DtTermPrimCursorMove(w, row, vtw->term.tpd->cursorColumn) ;
}

void 
_DtTermCursorDown(Widget w)    /* CUD CISpB */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int  bmargin, row, cnt, brow ;
  Debug('P', fprintf(stderr,">>In func _DtTermCursorDown\n")) ;
  context = GetParserContext(w) ;
  KILLWRAP(vtw) ;
  bmargin = BOTTOMMARGIN(vtw) ;
  brow = BOTTOMROW(vtw) ;
  STORELASTARG(context) ;
  cnt = context->parms[1] ;
  if (!(cnt))cnt=1;
  row = vtw->term.tpd->cursorRow+cnt;
  /* are we below bottom margin and did we actually cross it? */
  /* NOTE that this also handles the case when bmargin == brow */
  if (row > bmargin && row-cnt <= bmargin) row = bmargin;
  /* did we start below bottom margin and cross bottom row?   */
  if (row > brow && bmargin < brow ) row = brow;
  _DtTermPrimCursorMove(w, row, vtw->term.tpd->cursorColumn) ;
}

void 
_DtTermCursorForward(Widget w)    /* CUF CISpC */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int col,lcol,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermCursorForward\n")) ;
  context = GetParserContext(w) ;
  KILLWRAP(vtw) ;
  lcol = LASTCOLUMN(vtw) ;
  STORELASTARG(context) ;
  cnt = context->parms[1] ;
  if (!cnt) cnt=1;
  col = vtw->term.tpd->cursorColumn+cnt;
  if (col>lcol) col=lcol;
  _DtTermPrimCursorMove(w,vtw->term.tpd->cursorRow,col);
}

void 
_DtTermCursorBack(Widget w)    /* CUB CISpD */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int i,k,j,row,col,fcol,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermCursorBack\n")) ;
  KILLWRAP(vtw) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = context->parms[1] ;
  fcol = FIRSTCOLUMN(vtw) ;
  row = vtw->term.tpd->cursorRow;
  col = vtw->term.tpd->cursorColumn;
  if (!cnt) cnt=1;
  if ((col -= cnt) < 0) {
     if(vtw->vt.td->reverseWrapMode && vtw->term.tpd->autoWrapRight) {
        int lastrow = BOTTOMROW(vtw);
        if (vtw->vt.td->originMode) {
           row-=TOPMARGIN(vtw) ;
           lastrow = BOTTOMMARGIN(vtw)-TOPMARGIN(vtw);
         }
        if((i = (j = LASTCOLUMN(vtw) + 1) * row + col) < 0) {
              k = j * (lastrow + 1);
              i += ((-i) / k + 1) * k;
        }
        row = i / j;
        if (vtw->vt.td->originMode) row+=TOPMARGIN(vtw);
        col = i % j;
      } else
        col = 0;
  }
  _DtTermPrimCursorMove(w,row,col);
}

void 
_DtTermCursorToLineUp(Widget w)   /* CPL  CSIpF */
{
  DtTermWidget vtw = (DtTermWidget) w;
  ParserContext context ;
  Debug('P', fprintf(stderr,">>In func _DtTermCursorToLineUp\n")) ;
  KILLWRAP(vtw) ; 
  context = GetParserContext(w) ;
  vtw->term.tpd->cursorColumn = FIRSTCOLUMN(vtw) ;
  _DtTermCursorUp(w) ;
}

void 
_DtTermCursorToCol(Widget w)    /* CHA CSIpG */
{
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  ParserContext context ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  if (!context->parms[1] ) context->parms[1]=1;
  if (context->parms[1]>tw->term.columns) context->parms[1]=tw->term.columns;
  tw->term.tpd->cursorColumn = context->parms[1] - BASE ;
}

void 
_DtTermEraseInLine(Widget w)  /* EL  ESC[pK */
{
  DtTermWidget vtw = (DtTermWidget) w;
  ParserContext context ;
  DtEraseMode eraseMode;
  

  Debug('P', fprintf(stderr,">>In func _DtTermEraseInLine\n")) ;
  KILLWRAP(vtw) ; 
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  /*
  ** pick the correct erase mode...
  */
  switch ( context->parms[1] )
  {
    case 0:
      eraseMode = eraseToEOL;
      break;

    case 1:
      eraseMode = eraseFromCol0;
      break;

    case 2:
      eraseMode = eraseLine;
      break;

  }
  _DtTermFuncEraseInLine(w, (int)eraseMode, fromParser);
}

void 
_DtTermInsertLines(Widget w)  /* IL CSIpL */
{
  DtTermWidget vtw = (DtTermWidget) w;
  ParserContext context ;
  int lines , cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermInsertLines\n")) ;
  if ( vtw->term.tpd->cursorRow < TOPMARGIN(vtw) ||   
      vtw->term.tpd->cursorRow > BOTTOMMARGIN(vtw) )/*outside scrolling region*/
     return ;
  context = GetParserContext(w) ;
  KILLWRAP((DtTermWidget)w) ;
  STORELASTARG(context) ;
  cnt=context->parms[1];
  if (!cnt)cnt++;
  lines  = BOTTOMMARGIN(vtw) -  vtw->term.tpd->cursorRow + 1;
  if (cnt > lines) cnt=lines ;
  _DtTermFuncInsertLine(w,cnt,fromParser) ;
}

void 
_DtTermDeleteLines(Widget w)   /* DL CSIpM */
{
  DtTermWidget vtw = (DtTermWidget) w;
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermDeleteLines\n")) ;

  if ( vtw->term.tpd->cursorRow < TOPMARGIN(vtw) ||   
      vtw->term.tpd->cursorRow > BOTTOMMARGIN(vtw) )  /* outside scrolling region*/
     return ;
  KILLWRAP((DtTermWidget)w) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = context->parms[1];
  if(!cnt)cnt++;
  _DtTermFuncDeleteLine(w,cnt,fromParser) ;
}

void 
_DtTermDeleteChars(Widget w)   /* DCH CSIpP */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermDeleteChars\n")) ;
  KILLWRAP(vtw) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt= context->parms[1];
  if (!cnt) cnt++;
  else if (cnt>MAXSHORT) cnt=MAXSHORT-100;
  _DtTermFuncDeleteChar(w,cnt,fromParser) ;
}

void 
_DtTermNextLine(Widget w) /* NEL ESCE */
{
  DtTermWidget vtw = (DtTermWidget) w;
  int brow;
  Debug('P', fprintf(stderr,">>In func _DtTermNextLine\n")) ;
  vtw->term.tpd->cursorColumn = FIRSTCOLUMN(vtw) ;
  _DtTermIndex(w) ;  /* use IND */
}

void 
_DtTermReverseIndex(Widget w) /* RI ESCM */
{
  DtTermWidget vtw = (DtTermWidget) w;
  int tmargin, trow, col;
  Debug('P', fprintf(stderr,">>In func _DtTermReverseIndex\n")) ;
  KILLWRAP(vtw) ;
  trow = TOPROW(vtw) ;
  tmargin = TOPMARGIN(vtw) ;
  col = vtw->term.tpd->cursorColumn ;
  if ( vtw->term.tpd->cursorRow == tmargin) {
       _DtTermFuncInsertLine(w,1,fromParser) ;
       /* _DtTermFuncInsertLine sets column to first column */
       vtw->term.tpd->cursorColumn = col ;
   }
  else if (vtw->term.tpd->cursorRow > trow){ 
           vtw->term.tpd->cursorRow-- ;
   }
}

void 
_DtTermIndex(Widget w) /* IND  ESCD */
{
  DtTermWidget vtw = (DtTermWidget) w;
  int bmargin, brow ;
  Debug('P', fprintf(stderr,">>In func _DtTermIndex\n")) ;
  KILLWRAP(vtw) ;
  brow = BOTTOMROW(vtw) ;
  bmargin = BOTTOMMARGIN(vtw) ;
  if ( vtw->term.tpd->cursorRow == bmargin) {
       (void) _DtTermPrimScrollText(w, 1);
   }
  else if (vtw->term.tpd->cursorRow < brow){
         vtw->term.tpd->cursorRow++ ;
   }
  (void) _DtTermPrimFillScreenGap(w);
}

void 
_DtTermScrollingRegion(Widget w)   /* DECSTBM  CSIp;pr */
{
  DtTermWidget vtw = (DtTermWidget)w ;
  ParserContext context ;
  int row1,row2 ;
  Debug('P', fprintf(stderr,">>In func _DtTermScrollingRegion\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  row1 = context->parms[1] ;
  row2 = context->parms[2] ;
  if (!row1) row1 = 1 ;
  if (!row2 || row2>vtw->term.rows) row2 = vtw->term.rows ;
  row1-=BASE ;
  row2-=BASE ;
  if ( row1 < row2 ) {
    if ( row1 == TOPROW(vtw) && row2 == BOTTOMROW(vtw)) 
       vtw->term.tpd->memoryLockMode=SCROLL_LOCKoff ; 
    else 
       vtw->term.tpd->memoryLockMode=SCROLL_LOCKon ;
    vtw->term.tpd->scrollLockTopRow = row1 ;
    vtw->term.tpd->scrollLockBottomRow = row2 ;
    if (vtw->vt.td->originMode) _DtTermPrimCursorMove(w,row1,0) ;
    else  _DtTermPrimCursorMove(w,0,0) ;
  }
}

void 
_DtTermCharAttributes(Widget w)   /* SGR CSIpm */
{
  ParserContext context ;
  int i,cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermCharAttributes\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  if(PCOUNT(context)) {
    for (cnt=1; cnt <= PCOUNT(context); cnt++)
       _DtTermVideoEnhancement(w,context->parms[cnt]) ;
   }
  else
   _DtTermVideoEnhancement(w,0) ;
}

void 
_DtTermDeviceAttributes(Widget w)    /* DA CSIpc */
{
  DtTermWidget vtw = (DtTermWidget) w;
  ParserContext context ;
  int cnt ;
  char buf[50];
  Debug('P', fprintf(stderr,">>In func _DtTermDeviceAttributes\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  switch(context->parms[1]) {
    case 0:
         switch(vtw->vt.td->terminalId ) {
            case 100:
                     sprintf(buf,"\033[?%d;%dc",1,2) ;
                     break;
            case 101:
                     sprintf(buf,"\033[?%d;%dc",1,0) ;
                     break;
            case 102:
                     sprintf(buf,"\033[?%dc",6) ;
                     break;
            case 220:
                /*  class 2 terminal (62), 132 columns (1), printer port (2),
                 *  selective erase (6), DRCS (7), UDK (8), 
                 *  national replacement char sets
                 */
                sprintf(buf,"\033[?%d;%d;%d;%d;%d;%d;%dc",62,1,2,6,7,8,9) ;
                break;
          }
        sendEscSequence(w,buf) ;
        break;
  }
}

void 
_DtTermChangeTextParam(Widget w)  /* xterm  CSIp;pcCtrl-G  */
{
  ParserContext context ;
  DtTermWidget tw = (DtTermWidget) w;
  DtTermData vtd = tw->vt.td;
  Widget sw;
  int i ;
  Arg arg[5];
  Debug('P', fprintf(stderr,">>In func _DtTermChangeTextParam\n")) ;
  context = GetParserContext(w) ;
  i=0;
  for (sw = w; !XtIsShell(sw); sw = XtParent(sw));
  /*
   *  context->parms[1] is inited to 0, so if no numeric param was entered,
   *  we default appropriately to case 0.
   */
  switch(context->parms[1]) {
    case 0: /* change Icon name and Window title */
            XtSetArg(arg[i], XmNtitle, context->stringParms[0].str); i++;
            XtSetArg(arg[i], XmNiconName, context->stringParms[0].str); i++;
            XtSetValues(sw,arg,i) ;
            break;
    case 1: /* change Icon name */
            XtSetArg(arg[i], XmNiconName, context->stringParms[0].str); i++;
            XtSetValues(sw,arg,i) ;
            break;
    case 2: /* change Window title */
            XtSetArg(arg[i], XmNtitle, context->stringParms[0].str); i++;
            XtSetValues(sw,arg,i) ;
            break;
    case 3: /* change current working directory */
            tw->term.subprocessCWD = XtRealloc(tw->term.subprocessCWD,
		    strlen((char *) context->stringParms[0].str) + 1);
            (void) strcpy(tw->term.subprocessCWD,
		    (char *) context->stringParms[0].str);
            break;
   /*  These are handled by xterm but not by us.   
    case 46:  Change log file to context->stringParms[0] 
            break;
    case 50: ** change font to context->stringParms[0] 
            break;
   */
  }
}

void 
_DtTermTabClear(Widget w)    /*  TBC CSIpg */
{
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  TermBuffer tb = tw->term.tpd->termBuffer ;
  ParserContext context ;
  int type ;
  Debug('P', fprintf(stderr,">>In func _DtTermTabClear\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  type = context->parms[1] ;
  switch ( type ) {
    case 0:(void)_DtTermPrimBufferClearTab(tb,tw->term.tpd->cursorColumn) ;
           break ;
    case 3: (void)_DtTermPrimBufferClearAllTabs(tb) ;
           break ;
   }
}

void 
_DtTermRequestParam(Widget w) /* DECREQTPARM CSIpx */
{
    ParserContext context ;
    int row ;
    char buf[100];
    Debug('P', fprintf(stderr,">>In func _DtTermRequestParam\n")) ;
    context = GetParserContext(w) ;
    STORELASTARG(context) ;
    row = context->parms[1] ;
    if ( row < 2) {
       row += 2;
   
       /* row (?), 1 no parity, 1 eight bits, 112 xmit 9600 baud,
        * 112, receive 9600 baud, 1 clock multiplier (?),
        * 0 STP flags (?)   These are from xterm file 'charproc.c'.
        */
       sprintf(buf,"\033[%d;%d;%d;%d;%d;%d;%dx",row,1,1,112,112,1,0) ;
       sendEscSequence(w,buf) ; 
     }
}
   

void 
_DtTermTabSet(Widget w) /* HTS ESCH */
{
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  TermBuffer tb = tw->term.tpd->termBuffer ;
  Debug('P', fprintf(stderr,">>In func _DtTermTabSet\n")) ;
  (void)_DtTermPrimBufferSetTab(tb,tw->term.tpd->cursorColumn) ;
}

void 
_DtTermSaveCursor(Widget w) /* DECSC ESC7 */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermBuffer tb = (DtTermBuffer)tw->term.tpd->termBuffer ;
  DtTermData vtd = vtw->vt.td;

  Debug('P', fprintf(stderr,">>In func _DtTermSaveCursor\n")) ;
  vtd->saveCursor.cursorRow = vtw->term.tpd->cursorRow ;
  vtd->saveCursor.cursorColumn = vtw->term.tpd->cursorColumn ;
  vtd->saveCursor.originMode    = vtw->vt.td->originMode ;
  /* The following is save at the TermPrim level */
  /* vtw->vt.td->saveCursor.wrapMode    = */
  vtd->saveCursor.enhVideoState = vtd->enhVideoState ; 
  vtd->saveCursor.enhFieldState = vtd->enhFieldState  ;  
  vtd->saveCursor.enhFgColorState = vtd->enhFgColorState;
  vtd->saveCursor.enhBgColorState = vtd->enhBgColorState;
  vtd->saveCursor.GL = vtd->GL;
  vtd->saveCursor.GR = vtd->GR;
  vtd->saveCursor.G0 = vtd->G0;
  vtd->saveCursor.G1 = vtd->G1;
  vtd->saveCursor.G2 = vtd->G2;
  vtd->saveCursor.G3 = vtd->G3;
  vtd->saveCursor.singleShiftFont = vtd->singleShiftFont;
  vtd->saveCursor.singleShiftPending = vtd->singleShiftPending;
}

void 
_DtTermRestoreCursor(Widget w) /* DECRC ESC8 */
{
  DtTermPrimitiveWidget      tw        = (DtTermPrimitiveWidget) w;
  DtTermPrimData        tpd        = tw->term.tpd;
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermData vtd = vtw->vt.td;

  Debug('P', fprintf(stderr,">>In func _DtTermRestoreCursor\n")) ;
  tpd->cursorRow = vtd->saveCursor.cursorRow ;
  tpd->cursorColumn = vtd->saveCursor.cursorColumn ;
  vtw->vt.td->originMode = vtd->saveCursor.originMode    ;
  tpd->autoWrapRight = vtd->saveCursor.wrapMode ;
  vtd->enhVideoState  = vtd->saveCursor.enhVideoState ;  
  vtd->enhFieldState  = vtd->saveCursor.enhFieldState ; 
  vtd->enhFgColorState = vtd->saveCursor.enhFgColorState;
  vtd->enhBgColorState = vtd->saveCursor.enhBgColorState;
  vtd->GR = vtd->saveCursor.GR;
  vtd->GL = vtd->saveCursor.GL;
  vtd->G0 = vtd->saveCursor.G0;
  vtd->G1 = vtd->saveCursor.G1;
  vtd->G2 = vtd->saveCursor.G2;
  vtd->G3 = vtd->saveCursor.G3;
  vtd->singleShiftFont = vtd->saveCursor.singleShiftFont;
  vtd->singleShiftPending = vtd->saveCursor.singleShiftPending;
  (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhVideo,
              vtd->enhVideoState);
  (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhField,
              vtd->enhFieldState);
  (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhFont,
              *(vtd->GL));
  (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhBgColor,
              vtd->saveCursor.enhBgColorState);
  (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhFgColor,
              vtd->saveCursor.enhFgColorState);
  if ( vtd->originMode ) {
     if (tpd->cursorRow < tpd->scrollLockTopRow ) 
        tpd->cursorRow = tpd->scrollLockTopRow ;
     else if (tpd->cursorRow > tpd->scrollLockBottomRow ) 
        tpd->cursorRow = tpd->scrollLockBottomRow ;
  }
}  

void 
_DtTermAppKeypad(Widget w)  /* DECPAM  ESC= */
{
  DtTermWidget vtw = (DtTermWidget) w;
  Debug('P', fprintf(stderr,">>In func _DtTermAppKeypad\n")) ;
  vtw->vt.td->applicationKPMode=True;
}

void 
_DtTermNormalKeypad(Widget w) /* DECPNM ESC> */
{
  DtTermWidget vtw = (DtTermWidget) w;
  Debug('P', fprintf(stderr,">>In func _DtTermNormalKeypad\n")) ;
  vtw->vt.td->applicationKPMode=False;
}

void 
_DtTermS8C1T(Widget w)   /* S8C1T  ESCG  */
{
  DtTermWidget vtw = (DtTermWidget) w;
  Debug('P', fprintf(stderr,">>In func _vt8CIT\n")) ;
  vtw->vt.td->S8C1TMode=True;
}

void 
_DtTermS7C1T(Widget w)   /* S7C1T  ESCF */
{
  DtTermWidget vtw = (DtTermWidget) w;
  Debug('P', fprintf(stderr,">>In func _vt8CIT\n")) ;
  vtw->vt.td->S8C1TMode=False;
}

void 
_DtTermSetCompatLevel(Widget w)   /* DECSCL CSI p;p"p (last p literal) */
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  Debug('P', fprintf(stderr,">>In func _DtTermSetCompatLevel\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  vtw->vt.td->compatLevel= 2;  /* assume vt200 mode */
  switch ( PCOUNT(context) )  {
      case 1: switch (context->parms[1]) {
                 case 61:  vtw->vt.td->compatLevel= 1;
                           vtw->vt.td->S8C1TMode=False;
                           break;
                 case 62:  vtw->vt.td->S8C1TMode=True;
                           break;
               }
              break;
      case 2: switch (context->parms[2]) {
                 case 0: 
                 case 2: vtw->vt.td->S8C1TMode=True;
                         break;
                 case 1:
                         vtw->vt.td->S8C1TMode=False;
                         break;
               }
    }
}

void 
_DtTermInvokeG3(Widget w)  /* LS3R  ESC| */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermInvokeG3\n")) ;
  context = GetParserContext(w) ;
}

void 
_DtTermInvokeG2(Widget w)  /* LS3R  ESC} */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermInvokeG2\n")) ;
  context = GetParserContext(w) ;
}

void 
_DtTermScrollUp(Widget w)    /* SU CSIpS */
{
    ParserContext context ;
    DtTermWidget vtw = (DtTermWidget) w;
    int row,col,cnt ;

    Debug('P', fprintf(stderr,">>In func _DtTermScrollUp\n")) ;
#define  EXIT_IF_OUTSIDE_SR(w) if((w)->term.tpd->scrollLockTopRow >  \
				   (w)->term.tpd->cursorRow  ||    \
				   (w)->term.tpd->scrollLockBottomRow < \
				   (w)->term.tpd->cursorRow)   \
				   return;
    EXIT_IF_OUTSIDE_SR(vtw)
    KILLWRAP(vtw) ;
    context = GetParserContext(w) ;
    row = vtw->term.tpd->cursorRow;
    col = vtw->term.tpd->cursorColumn;
    STORELASTARG(context) ;
    if (!context->parms[1])
	context->parms[1] = 1;
    else if (context->parms[1] > MAXSHORT)
	context->parms[1] = MAXSHORT - 100;

    /* clip the number of rows to the scroll region... */
    if (context->parms[1] > (vtw->term.tpd->scrollLockBottomRow -
	    vtw->term.tpd->scrollLockTopRow + 1)) {
	context->parms[1] = vtw->term.tpd->scrollLockBottomRow -
		vtw->term.tpd->scrollLockTopRow + 1;
    }

    vtw->term.tpd->cursorRow = TOPMARGIN(vtw) ;
    _DtTermFuncDeleteLine(w, context->parms[1], fromParser);
    vtw->term.tpd->cursorRow = row;
    vtw->term.tpd->cursorColumn = col;
}

void 
_DtTermScrollDown(Widget w)  /* SD CSIpT */
{
    ParserContext context ;
    DtTermWidget vtw = (DtTermWidget) w;
    int row,col,cnt ;

    Debug('P', fprintf(stderr,">>In func _DtTermScrollDown\n")) ;
    EXIT_IF_OUTSIDE_SR(vtw)
    KILLWRAP(vtw) ;
    context = GetParserContext(w) ;
    row = vtw->term.tpd->cursorRow;
    col = vtw->term.tpd->cursorColumn ;
    STORELASTARG(context) ;
    if (!context->parms[1])
	context->parms[1] = 1;
    else if (context->parms[1] > MAXSHORT)
	context->parms[1] = MAXSHORT - 100;

    /* clip the number of rows to the scroll region... */
    if (context->parms[1] > (vtw->term.tpd->scrollLockBottomRow -
	    vtw->term.tpd->scrollLockTopRow) + 1) {
	context->parms[1] = vtw->term.tpd->scrollLockBottomRow -
		vtw->term.tpd->scrollLockTopRow + 1;
    }

    vtw->term.tpd->cursorRow = TOPMARGIN(vtw) ;
    _DtTermFuncInsertLine(w, context->parms[1], fromParser) ;
    vtw->term.tpd->cursorRow = row;
    vtw->term.tpd->cursorColumn = col;
}

void 
_DtTermRestoreModeValues(Widget w)  /* xterm - Restore DEC mode values CSI?pr */
{
  ParserContext context ;
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermWidget vtw = (DtTermWidget)w;
  DtTermData vtd = vtw->vt.td;
  int i,cnt ;
  Debug ('P', fprintf(stderr,">>In func _DtTermPModeReset\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  for (i=1;i<=cnt;i++) {
      switch ( context->parms[i]) {
          /* from vt220 */
          case 1: /* Cursor key (Normal/Application) */
                  vtd->applicationMode = vtd->saveDECMode.applicationMode ;
                  break;
#ifdef	NOT_SUPPORTED
          case 2:  /* set vt52 mode */
                  break;
#endif	/* NOT_SUPPORTED */
          case 3: /* Columns (80/132) */
                  vtd->col132Mode = vtd->saveDECMode.col132Mode ;       
                  break;
          case 4: /* Scroll Mode ( jump/smooth ) */
                  _DtTermPrimScrollComplete(w, True) ;
                  vtw->term.jumpScroll = vtd->saveDECMode.jumpScrollMode ;
                  break;
          case 5: /* Screen mode */
		  vtw->term.reverseVideo = vtd->saveDECMode.inverseVideoMode ;
                  break;
          case 6: /*  Origin mode (on/off)*/
                  vtd->originMode = vtd->saveDECMode.originMode ;
                  break;
          case 7: /* Auto wrap (On/Off) */
                  vtw->term.tpd->autoWrapRight = vtd->saveDECMode.wrapMode ;
                  break;
          case 8: /* Auto repeat (Off) */
                  /* implemented in the server */
                  break;
#ifdef	NOT_SUPPORTED
          case 18: /* Print form feed (Off) */
                  break;
          case 19: /* Print extent (scrolling region) */
                  break;
#endif	/* NOT_SUPPORTED */
          case 25: /* Text cursor enable (Off) */
		  _DtTermPrimSetCursorVisible(w,
			vtd->saveDECMode.cursorVisible);
                  break;
          
           /* from xterm */
          case 40: /*  80/132 mode  (disallow/allow) */
                  vtw->vt.c132 = vtd->saveDECMode.allow80_132ColMode;
                  break;
          case 41: /*  curses fix  (off/on) */
                  vtd->fixCursesMode = vtd->saveDECMode.fixCursesMode;
                  break;
          case 44: /* margin bell (off/on) */
                  vtw->term.marginBell = vtd->saveDECMode.marginBellMode;
                  break;
          case 45: /* Reverse-wraparound mode */
                  vtd->reverseWrapMode = vtd->saveDECMode.reverseWrapMode ;
                  break;
          case 46: /* logging (stop/start )  */
		  if (vtd->saveDECMode.logging) {
		      vtw->term.logging = True ;
		      _DtTermPrimStartLog(w);
		  } else {
		      vtw->term.logging = False ;
		      _DtTermPrimCloseLog(w) ;
		  }
                  break;
#ifdef	NOT_SUPPORTED
          case 47: /* screen buffer  (normal) */
                  break;
          case 1000: /* don't send mouse x and y */
                  break;
          case 1001: /* don't use hilite mouse tracking */
                  break;
#endif	/* NOT_SUPPORTED */
      }
   }
}

void 
_DtTermSaveModeValues(Widget w)  /* xterm - Save DEC mode values CSI?ps  */
{
  ParserContext context ;
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  DtTermWidget vtw = (DtTermWidget)w;
  DtTermData vtd = vtw->vt.td;
  int i,cnt ;
  Debug ('P', fprintf(stderr,">>In func _DtTermPModeReset\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  cnt = PCOUNT(context) ;
  for (i=1;i<=cnt;i++) {
      switch ( context->parms[i]) {
          /* from vt220 */
          case 1: /* Cursor key (Normal/Application) */
                  vtd->saveDECMode.applicationMode = vtd->applicationMode ;
                  break;
#ifdef	NOT_SUPPORTED
          case 2:  /* set vt52 mode */
                  break;
#endif	/* NOT_SUPPORTED */
          case 3: /* Columns (80/132) */
                  vtd->saveDECMode.col132Mode = vtd->col132Mode ;       
                  break;
          case 4: /* Scroll Mode ( jump/smooth ) */
                  vtd->saveDECMode.jumpScrollMode = vtw->term.jumpScroll ;
                  break;
          case 5: /* Screen mode */
                  vtd->saveDECMode.inverseVideoMode = vtw->term.reverseVideo ;
                  break;
          case 6: /*  Origin mode (on/off)*/
                  vtd->saveDECMode.originMode = vtd->originMode ;
                  break;
          case 7: /* Auto wrap (On/Off) */
                  vtd->saveDECMode.wrapMode = vtw->term.tpd->autoWrapRight ;
                  break;
          case 8: /* Auto repeat (Off) */
                  /* implemented in the server */
                  break;
#ifdef	NOT_SUPPORTED
          case 18: /* Print form feed (Off) */
                  break;
          case 19: /* Print extent (scrolling region) */
                  break;
#endif	/* NOT_SUPPORTED */
          case 25: /* Text cursor enable (Off) */
		  vtd->saveDECMode.cursorVisible =
			    _DtTermPrimGetCursorVisible(w);
                  break;
           /* from xterm */
          case 40: /*  80/132 mode  (disallow/allow) */
                  vtd->saveDECMode.allow80_132ColMode = vtw->vt.c132;
                  break;
          case 41: /*  curses fix  (off/on) */
                  vtd->saveDECMode.fixCursesMode = vtd->fixCursesMode ;     
                  break;
          case 44: /* margin bell (off/on) */
                  vtd->saveDECMode.marginBellMode = vtw->term.marginBell ;
                  break;
          case 45: /* Reverse-wraparound mode */
                  vtd->saveDECMode.reverseWrapMode = vtd->reverseWrapMode ;
                  break;
          case 46: /* logging (stop/start ) */
		  vtd->saveDECMode.logging = vtw->term.logging;
                  break;
#ifdef	NOT_SUPPORTED
          case 47: /* screen buffer  (normal) */
                  break;
          case 1000: /* don't send mouse x and y */
                  break;
          case 1001: /* don't use hilite mouse tracking */
                  break;
#endif	/* NOT_SUPPORTED */
      }
   }
}


void 
_DtTermAlignTest(Widget w)     /* DECALN ESC#8 */
{
  DtTermWidget vtw = (DtTermWidget) w;
  int tlrow, blrow, cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermAlignTest\n")) ;
  vtw->term.tpd->parserNotInStartState = False ;
  tlrow =  vtw->term.tpd->scrollLockTopRow ;
  blrow =  vtw->term.tpd->scrollLockBottomRow ;
  vtw->term.tpd->scrollLockTopRow = TOPROW(w) ; ;
  vtw->term.tpd->scrollLockBottomRow = BOTTOMROW(vtw) ;
  cnt = (BOTTOMROW(vtw)+BASE)*(LASTCOLUMN(vtw)+BASE);
  vtw->term.tpd->cursorColumn = FIRSTCOLUMN(w) ;
  vtw->term.tpd->cursorRow    = TOPROW(w) ;
  while(cnt--) _DtTermPrimInsertText(w,(unsigned char *) "E", 1) ;
  vtw->term.tpd->scrollLockTopRow = tlrow ;
  vtw->term.tpd->scrollLockBottomRow = blrow ;
  vtw->term.tpd->cursorRow = CEILING(vtw) ;
  vtw->term.tpd->cursorColumn = FIRSTCOLUMN(w) ;
}

void 
_DtTermInvokeG1(Widget w)     /* ESC~ESC */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermInvokeG1\n")) ;
  context = GetParserContext(w) ;
}

void 
_DtTermSelEraseInLine(Widget w) /* DECSEL ESC?pK */
{
    DtTermWidget vtw = (DtTermWidget) w;
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    TermBuffer tb = tw->term.tpd->termBuffer ;
    DtTermPrimData  tpd = tw->term.tpd;
    DtTermData  td = vtw->vt.td;
    ParserContext context ;
    enhValues evalues;
    int c,col1,col2,col,row ;
    short cnt ;
    DtTermInsertCharMode	saveInsertCharMode;

    Debug('P', fprintf(stderr,">>In func _DtTermSelEraseInLine\n")) ;
    KILLWRAP(vtw) ;
    context = GetParserContext(w) ;
    STORELASTARG(context) ;
    col1 = FIRSTCOLUMN(vtw) ;
    col2 = LASTCOLUMN(vtw) ;
    switch ( context->parms[1] )  {
    case 0:
	col1= tpd->cursorColumn;
	break;
    case 1:
	col2 = tpd->cursorColumn;
	break;
    case 2:
	break ;
    default:
	return ;
    }
    /* save away the current cursor position... */
    col = tpd->cursorColumn ; 
    row = tpd->cursorRow    ; 

    /* save away the current insert mode... */
    saveInsertCharMode = vtw->term.tpd->insertCharMode;

    /* insert char needs to be off... */
    tpd->insertCharMode = DtTERM_INSERT_CHAR_OFF;

    tpd->parserNotInStartState = False ;
    c = col1;

    while (c <= col2) {
	(void)_DtTermPrimBufferGetEnhancement(tb, tpd->cursorRow, 
		    c, &evalues, &cnt, countAll);
	if (!cnt)
	    break ;
	if (evalues[(int)enhField] == FIELD_UNPROTECT) {
	    (void) _DtTermPrimBufferSetEnhancement(tb, tpd->cursorRow,
		    c, enhVideo, evalues[(int)enhVideo]);
	    (void) _DtTermPrimBufferSetEnhancement(tb, tpd->cursorRow,
		    c, enhFgColor, evalues[(int)enhFgColor]);
	    (void) _DtTermPrimBufferSetEnhancement(tb, tpd->cursorRow,
		    c, enhBgColor, evalues[(int)enhBgColor]);
	    while ((c <= col2) && cnt--) {
	        tpd->cursorColumn = c;
	        (void) _DtTermPrimInsertText(w, (unsigned char *) " ", 1);
	        c++;
	    }
	} else {
	    c += cnt;
	}
    }

    /* restore the insert char mode... */
    tpd->insertCharMode = saveInsertCharMode;

    /* restore the cursor position... */
    tpd->cursorColumn = col; 
    tpd->cursorRow = row; 

    /* restore the current enhancement state... */
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	      tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhVideo,
	      td->enhVideoState);
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	      tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhFgColor,
	      td->enhFgColorState);
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	      tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhBgColor,
	      td->enhBgColorState);
}

void 
_DtTermSelEraseInDisplay(Widget w) /* DECSED ESC?pJ */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
  TermBuffer tb = tw->term.tpd->termBuffer ;
  ParserContext context ;
  enhValues evalues;
  int r,col1,col2,row1,row2,col,row ;
  int   cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermSelEraseInDisplay\n")) ;
  KILLWRAP(vtw) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  col1 = FIRSTCOLUMN(vtw) ;
  row1 = TOPROW(vtw) ;
  col2 = LASTCOLUMN(vtw) ;
  row2 = BOTTOMROW(vtw) ;
  switch ( context->parms[1] )  {
      case 0: col1= vtw->term.tpd->cursorColumn;
              row1 = vtw->term.tpd->cursorRow;
              break;
      case 1: col2 = vtw->term.tpd->cursorColumn;
              row2 = vtw->term.tpd->cursorRow;
              break;
      case 2: break ;
      default: return ;
    }
  col = vtw->term.tpd->cursorColumn;
  row = vtw->term.tpd->cursorRow;
  vtw->term.tpd->cursorColumn = col1 ;
  vtw->term.tpd->cursorRow = row1;
  context->workingNum  = 0 ;  /* erase to the end of line */
  _DtTermSelEraseInLine(w) ;
  context->workingNum  = 2 ;  /* erase the whole row */
  for ( r=row1+1; r<row2; r++) {
     vtw->term.tpd->cursorRow = r;
     _DtTermSelEraseInLine(w) ;
   }
  vtw->term.tpd->cursorColumn = col2 ;
  vtw->term.tpd->cursorRow = row2;
  context->workingNum  = 2 ;  /* erase the beginning of the line */
  _DtTermSelEraseInLine(w) ;
  vtw->term.tpd->cursorColumn = col ;
  vtw->term.tpd->cursorRow = row;
}


void 
_DtTermSingleShiftG2(Widget w) /* SS2  ESCN */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermData vtd =      vtw->vt.td;

  Debug('P', fprintf(stderr,">>In func _DtTermSingleShiftG2\n")) ;
  vtd->singleShiftPending = True;
  vtd->singleShiftFont = vtd->G2;
}

void 
_DtTermSingleShiftG3(Widget w) /* SS3  ESCO */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermData vtd =      vtw->vt.td;

  Debug('P', fprintf(stderr,">>In func _DtTermSingleShiftG3\n")) ;
  vtd->singleShiftPending = True;
  vtd->singleShiftFont = vtd->G3;
}

void 
_DtTermLoadCharSet(Widget w) /* vt220 DCS - download char set ESCPpST  */
{
  /* we're not implementing this */
}

/*********************************************************************
 *  The next 3 functions handle the user key string loading
 *    DCS Pc;Pl | Ky1/st1;ky2/st2;...kyn/stn ST
 */
void 
_DtTermParseUserKeyClear(Widget w) /* vt220 DECUDK clear function keys*/
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermData vtd =      vtw->vt.td;
  ParserContext context ;
  Debug('P', fprintf(stderr,">>In func _DtTermLoadSoftKey\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  if ( !vtd->userKeysLocked ) 
  {
     vtd->needToLockUserKeys = False ;
     if (context->parms[1] == 0)  
        _DtTermFunctionKeyClear(w) ;
     if (context->parms[0] == 2 && context->parms[2] == 0) 
                                      vtd->needToLockUserKeys = True ; 
  }
  _DtTermClearParam(w) ;
}
void
_DtTermParseUserKeyLoad(Widget w) /* vt220 DECUDK load function string */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermData vtd =      vtw->vt.td;
  ParserContext context ;
  Debug('P', fprintf(stderr,">>In func _DtTermLoadSoftKey\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  if ( !vtd->userKeysLocked)
  {
     context->stringParms[0].str[context->stringParms[0].length/2] = 
                          (unsigned char)0 ;  /* Null terminate */
     _DtTermFunctionKeyStringStore(w,context->parms[1],     
                                           (char *)context->stringParms[0].str);
  }
  _DtTermClearParam(w) ;
}

void
_DtTermParseUserKeyLoadLast(Widget w) /* vt220 DECUDK load function string */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermData vtd =      vtw->vt.td;
   _DtTermParseUserKeyLoad(w);
  if (vtd->needToLockUserKeys) vtd->userKeysLocked = True ;
  vtd->needToLockUserKeys = False ;
}

void
_DtTermParseHexDigit(Widget w) /* vt220 DECUDK store hex digit*/
{
  ParserContext context ;
  int ci, left_byte, i;
  char c, cbuf[2] ;
  Debug('P', fprintf(stderr,">>In func _DtTermLoadSoftKey\n")) ;
  context = GetParserContext(w) ;
  if ( (int)context->stringParms[0].length < 2*STR_SIZE-1 ) {
      /* fold the hex digits into characters */
      i = context->stringParms[0].length ;
      cbuf[0] = *context->inputChar ;
      cbuf[1] = (char)0 ;
      sscanf(cbuf,"%x",&ci)   ;
      c = (char)ci ;
      left_byte  = (i+1)%2 ;
      i /= 2 ;
      if ( left_byte  )
           context->stringParms[0].str[i] = c<<4 ;
      else
           context->stringParms[0].str[i] |= c & 0x0F ;
      context->stringParms[0].length++ ;
   }
}

void 
_DtTermReset(Widget w) /* RIS  ESCc */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermReset\n")) ;
  context = GetParserContext(w) ;
  _DtTermFuncHardReset(w,0 ,fromParser) ;
}

void 
_DtTermSelectG2(Widget w) /* LS2  ESCn */
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;

    Debug('P', fprintf(stderr,">>In func _DtTermSelectG2\n")) ;
    vtd->GL = &vtd->G2;

    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void 
_DtTermSelectG3(Widget w) /* LS3 ESCo */
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;

    Debug('P', fprintf(stderr,">>In func _DtTermSelectG3\n")) ;
    vtd->GL = &vtd->G3;
    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void
_DtTermParseStatus(Widget w)  /* DECID (same as DA)  ESCZ */
{
   _DtTermDeviceAttributes(w) ;
}

void 
_DtTermSetCharEraseMode(Widget w) /* DECSCA ESCp"q */
{
  DtTermWidget vtw = (DtTermWidget) w;
  DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
  DtTermPrimData        tpd        = tw->term.tpd;
  ParserContext context ;
  Debug('P', fprintf(stderr,">>In func _DtTermSetCharEraseMode\n")) ;
  context = GetParserContext(w) ;
  STORELASTARG(context) ;
  switch ( context->parms[1] ) {
     case 0:  /* set back to default which is unprotect  */
              /* fall through */
     case 2:
             vtw->vt.td->enhFieldState = FIELD_UNPROTECT;
	     (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
                      tpd->topRow + tpd->cursorRow, tpd->cursorColumn, 
                      enhField, vtw->vt.td->enhFieldState);
             break;
     case 1:
             vtw->vt.td->enhFieldState = FIELD_PROTECT;
             (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
                      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
                      enhField, vtw->vt.td->enhFieldState);
             break;
   }
}

void 
_DtTermSingleWide(Widget w) /* DECSWL ESC#5 */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermSingleWide\n")) ;
  context = GetParserContext(w) ;
}

void 
_DtTermDoubleWide(Widget w) /* DECDWL ESC#6  */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermDoubleWide\n")) ;
  context = GetParserContext(w) ;
}

void 
_DtTermDoubleHigh(Widget w) /* DECWHL ESC#4 */
{
  ParserContext context ;
  int cnt ;
  Debug('P', fprintf(stderr,">>In func _DtTermDoubleHigh\n")) ;
  context = GetParserContext(w) ;
}

void
_DtTermParseFontG0(Widget w)
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    ParserContext context ;
    context = GetParserContext(w) ;

    Debug('P', fprintf(stderr,">>In func _DtTermParseFontG0\n")) ;
    switch (*context->inputChar) {
    case 'B':
	vtd->G0 = FONT_NORMAL;
	break;

    case '0':
	vtd->G0 = FONT_LINEDRAW;
	break;
    }

    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void
_DtTermParseFontG1(Widget w)
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    ParserContext context ;
    context = GetParserContext(w) ;

    Debug('P', fprintf(stderr,">>In func _DtTermParseFontG1\n")) ;
    switch (*context->inputChar) {
    case 'B':
	vtd->G1 = FONT_NORMAL;
	break;

    case '0':
	vtd->G1 = FONT_LINEDRAW;
	break;
    }

    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void
_DtTermParseFontG2(Widget w)
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    ParserContext context ;
    context = GetParserContext(w) ;

    Debug('P', fprintf(stderr,">>In func _DtTermParseFontG1\n")) ;
    switch (*context->inputChar) {
    case 'B':
	vtd->G2 = FONT_NORMAL;
	break;

    case '0':
	vtd->G2 = FONT_LINEDRAW;
	break;
    }

    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void
_DtTermParseFontG3(Widget w)
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    ParserContext context ;
    context = GetParserContext(w) ;

    Debug('P', fprintf(stderr,">>In func _DtTermParseFontG1\n")) ;
    switch (*context->inputChar) {
    case 'B':
	vtd->G3 = FONT_NORMAL;
	break;

    case '0':
	vtd->G3 = FONT_LINEDRAW;
	break;
    }

    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void
_DtTermParseFontShift(Widget w)  /* Ctrl-E  Ctrl-F  */
{
    DtTermWidget vtw = (DtTermWidget)w ;
    DtTermData vtd =      vtw->vt.td;
    DtTermPrimitiveWidget tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    ParserContext context ;
    context = GetParserContext(w) ;

    Debug('P', fprintf(stderr,">>In func _DtTermParseFontShift\n")) ;
    switch (*context->inputChar) {
    case 'O' & 0x1f:
	/* SHIFT OUT */
	vtd->GL = &vtd->G0;
	break;

    case 'N' & 0x1f:
	/* SHIFT OUT */
	vtd->GL = &vtd->G1;
	break;
    }

    vtd->enhFontState = *vtd->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
	    tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
	    enhFont, vtd->enhFontState);
}

void
_DtTermParseTab(Widget w)    /* Crtl-I */
{
    DtTermWidget vtw = (DtTermWidget)w ;
    Debug('P', fprintf(stderr,">>In func _DtTermParseTab\n")) ;
    if (vtw->term.tpd->cursorColumn > LASTCOLUMN(vtw)) {
	if (vtw->vt.td->fixCursesMode) {
	    (void) _DtTermNextLine(w);
	    (void) _DtTermFuncTab(w, 1, fromParser);
	} else {
	    /* we already tabbed to the end of the line, so do nothing... */
	}
    } else {
	(void) _DtTermFuncTab(w, 1, fromParser);
    }
    return;
}

void
_DtTermParseLF    /* LineFeed (LF) or newline (NL) Ctrl-J, 
                     Vertical Tab Ctrl-K, Form Feed (NP) Ctrl-L */
(
    Widget w
)
{
  DtTermWidget vtw = (DtTermWidget)w ;
  Debug('P', fprintf(stderr,">>In func _vtParseLR\n")) ;
  KILLWRAP(vtw) ;
  if (vtw->term.tpd->cursorRow == BOTTOMMARGIN(vtw))
   {
     /*
        ** scroll one line...
        */
        (void) _DtTermPrimScrollText(w, 1);
        vtw->term.tpd->cursorRow = BOTTOMMARGIN(vtw);
    }
   else {
    if (!(vtw->term.tpd->cursorRow == BOTTOMROW(vtw))) {
       vtw->term.tpd->cursorRow++ ;
     }
   }
   (void) _DtTermPrimFillScreenGap(w);

   if (vtw->term.tpd->autoLineFeed )
        _DtTermPrimParseCR(w);
}

void
_DtTermSoftReset   /* DECSTR  ESC!p  */
(
    Widget w
)
{
  Debug('P', fprintf(stderr,">>In func _DtTermSoftReset\n")) ;
   _DtTermFuncSoftReset(w,0 ,fromParser) ;
}

void
_DtTermParseBackspace  /*  Ctrl-H  */
(
    Widget w
)
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  int row,col,fcol,cnt ;
  Debug('P', fprintf(stderr,">>In func  _DtTermParseBackspace\n")) ;
  context = GetParserContext(w) ;
  fcol = FIRSTCOLUMN(vtw) ;
  row = vtw->term.tpd->cursorRow;
  col = vtw->term.tpd->cursorColumn;
    (void) col--;
    if (col<fcol) {
      if (vtw->vt.td->reverseWrapMode) {
         if ( --row < TOPROW(vtw)) row = BOTTOMROW(vtw);
         if (vtw->vt.td->originMode && row < TOPMARGIN(vtw))
              row =BOTTOMMARGIN(vtw);
         col = LASTCOLUMN(vtw) ;
       }
      else {
         col=fcol;
      }
    }
  vtw->term.tpd->cursorRow = row;
  vtw->term.tpd->cursorColumn = col;
}

static void
GetWindowOffsets
(
    Widget			  w,
    int				 *xOffset,
    int				 *yOffset,
    int				 *widthOffset,
    int				 *heightOffset
)
{
    Display			 *display;
    Window			  parentReturn;
    Window			 *childrenReturn;
    unsigned int		  nChildrenReturn;
    Window			  rootWindow;
    Window			  topWindow;
    Window			  childReturn;
    int				  destXReturn;
    int				  destYReturn;
    unsigned int		  widthReturn;
    unsigned int		  heightReturn;
    unsigned int		  topWidthReturn;
    unsigned int		  topHeightReturn;
    unsigned int		  borderWidthReturn;
    unsigned int		  depthReturn;

    *xOffset = 0;
    *yOffset = 0;
    *widthOffset = 0;
    *heightOffset = 0;

    if (XmIsMotifWMRunning(w)) {
	/* get the window we want to figure the offsets for... */
	topWindow = XtWindow(w);

	/* and the display involved... */
	display = XtDisplay(w);

	(void) XQueryTree(display, topWindow, &rootWindow, &parentReturn,
		&childrenReturn, &nChildrenReturn);
	(void) XFree(childrenReturn);

	/* walk up the tree until the parent window is root... */
	while (parentReturn != rootWindow) {
	    topWindow = parentReturn;
	    (void) XQueryTree(display, topWindow, &rootWindow,
		    &parentReturn, &childrenReturn, &nChildrenReturn);
	    (void) XFree(childrenReturn);
	}

	/* we now know what our top window is.  translate our upper
	 * left corner relative to its upper left corner...
	 */
	(void) XTranslateCoordinates(display, XtWindow(w), topWindow,
		0, 0, &destXReturn, &destYReturn, &childReturn);
	*xOffset = destXReturn;
	*yOffset = destYReturn;

	/* now that we know the top window, we can figure out the size
	 * difference between the two windows...
	 */
	(void) XGetGeometry(display, topWindow, &rootWindow,
		&destXReturn, &destYReturn, &topWidthReturn,
		&topHeightReturn, &borderWidthReturn, &depthReturn);

	(void) XGetGeometry(display, XtWindow(w), &rootWindow,
		&destXReturn, &destYReturn, &widthReturn, &heightReturn,
		&borderWidthReturn, &depthReturn);

	*widthOffset = topWidthReturn - widthReturn;
	*heightOffset = topHeightReturn - heightReturn;
    }
}

void
_DtTermParseSunMisc  /*  Misc sun esc seqs  */
(
    Widget w
)
{
  ParserContext context ;
  DtTermWidget vtw = (DtTermWidget) w;
  Display *display = XtDisplay(w);
  int i, scr_num = XScreenNumberOfScreen(XtScreen(w));
  Window win = XtWindow(w);
  Window sh_win;
  Widget sw;
  XWindowChanges values;
  Arg arg[5];
  char buf[BUFSIZ];
  Position x,y;
  short   rows,columns;
  Dimension height,width;
  char *fmt,*s,*title,*icon;
  int xOffset;
  int yOffset;
  int widthOffset;
  int heightOffset;

  context = GetParserContext(w) ;
  STORELASTARG(context) ;

  for (sw = w; !XtIsShell(sw); sw = XtParent(sw));
  sh_win = XtWindow(sw) ;
  switch (context->parms[1])
  {
    case 1:
      XMapWindow(display,sh_win);
      break;
    case 2:
      XIconifyWindow(display,sh_win,scr_num);
      break;
    case 3:
	(void) GetWindowOffsets(sw, &xOffset, &yOffset, &widthOffset,
		&heightOffset);
	/* DKS
        (void) XMoveWindow(display, sh_win,
		context->parms[2] + xOffset, context->parms[3] + yOffset);
		DKS */
	(void) XtVaSetValues(sw, XmNx, context->parms[2] + xOffset,
		XmNy, context->parms[3] + yOffset, NULL);
        break ;
    case 4:
	(void) GetWindowOffsets(sw, &xOffset, &yOffset, &widthOffset,
		&heightOffset);
        (void) XtVaSetValues(sw, XmNwidth, context->parms[3] - widthOffset,
                       XmNheight,context->parms[2] - heightOffset, NULL);
      break ;
    case 5:
      XRaiseWindow(display,sh_win);
      break ;
    case 6:
      XLowerWindow(display,sh_win);
      break ;
    case 7:
      _DtTermPrimFuncRedrawDisplay(w, 1, fromParser);
      break ;
    case 8:
        rows = context->parms[2];
        columns = context->parms[3];
        (void) XtVaSetValues(w, DtNrows, rows, DtNcolumns, columns, NULL);
        break ;
    case 11:
      if (vtw->term.tpd->windowMapped)          
         sendEscSequence(w,"\033[1t") ;
      else
         sendEscSequence(w,"\033[2t") ;
      break ;
    case 13:
	(void) GetWindowOffsets(sw, &xOffset, &yOffset, &widthOffset,
		&heightOffset);
        (void) XtVaGetValues(sw, XtNx, &x, XtNy, &y, NULL);
        (void) sprintf(buf, "\033[3;%d;%dt", x - xOffset, y - yOffset);
        (void) sendEscSequence(w, buf);
        break;
    case 14:
	(void) GetWindowOffsets(sw, &xOffset, &yOffset, &widthOffset,
		&heightOffset);
        (void) XtVaGetValues(sw, XtNheight, &height, XtNwidth, &width, NULL);
        (void) sprintf(buf,"\033[4;%d;%dt", height + heightOffset,
		width + widthOffset);
        (void) sendEscSequence(w, buf);
        break;
    case 18:
        (void) XtVaGetValues(w, DtNrows, &rows, DtNcolumns, &columns, NULL);
        (void) sprintf(buf, "\033[8;%hd;%hdt", rows, columns);
        (void) sendEscSequence(w, buf);
        break;
    case 20:
      XtVaGetValues(sw,XmNiconName, &icon, NULL);
      fmt = "\033]L%s\033\\";
      if (strlen(icon) + strlen(fmt) + 1 >= sizeof(buf))
	s = XtMalloc(strlen(icon) + strlen(fmt) + 1);
      else 
	s = buf;
      sprintf(s, "%s", fmt);
      sendEscSequence(w,s) ;
      if (s != buf) XtFree(s);
      break ;
    case 21:
      XtVaGetValues(sw,XmNtitle, &title, NULL);
      fmt = "\033]l%s\033\\";
      if (strlen(icon) + strlen(fmt) + 1 >= sizeof(buf))
	s = XtMalloc(strlen(icon) + strlen(fmt) + 1);
      else 
	s = buf;
      sprintf(s, "%s", fmt);
      sendEscSequence(w,s) ;
      if (s != buf) XtFree(s);
      break ;
   }
}

void
_DtTermParserSunWindowIcon( Widget w)   /* sun set icon label */
{
  ParserContext context ;
  Widget sw;
  context = GetParserContext(w) ;
  for (sw = w; !XtIsShell(sw); sw = XtParent(sw));
  XtVaSetValues(sw, XmNiconName, context->stringParms[0].str, NULL) ;
}

void
_DtTermParserSunWindowTitle     /* Sun set title name */
(
Widget w
)
{
  ParserContext context ;
  Widget sw;
  context = GetParserContext(w) ;
  for (sw = w; !XtIsShell(sw); sw = XtParent(sw));
  XtVaSetValues(sw,XmNtitle, context->stringParms[0].str, NULL) ;
}

void
_DtTermParserSunIconFile(Widget w)   /* Sun set icon to pixmap in file */
{
  DtTermBuffer tb =
            (DtTermBuffer)((DtTermPrimitiveWidget)w)->term.tpd->termBuffer ;
  ParserContext context ;
  int fg, bg;
  Widget sw;
  Pixmap icon;

  context = GetParserContext(w) ;
  for (sw = w; !XtIsShell(sw); sw = XtParent(sw));
  XtVaGetValues(sw, XtNforeground, &fg, XtNbackground, &bg, NULL);
  icon = XmGetPixmap(XtScreen(sw),(char *)context->stringParms[0].str,fg,bg);
  if (icon != XmUNSPECIFIED_PIXMAP)
     XtVaSetValues(sw, XmNiconPixmap,  icon, NULL);
}

