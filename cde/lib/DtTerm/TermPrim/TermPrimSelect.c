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
static char rcs_id[] = "$TOG: TermPrimSelect.c /main/6 1999/10/14 16:22:53 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1995, 1996 Digital Equipment Corporation.		*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/CutPaste.h>
#include <Xm/XmPrivate.h>
#include <Xm/ScrollBarP.h>
#include "TermPrimAction.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimData.h"
#include "TermPrimRender.h"
#include "TermPrimSelectP.h"
#include "TermPrimBufferP.h"
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>

#if defined(USL) || defined(OPENBSD_ARCHITECTURE)
#include <ctype.h>
#include <wctype.h>
#endif

/* This is for Sun's two button mouse */

static char _DtTermEventBindingsCDE[] = "\
~c ~s ~m ~a <Btn1Down>:process-press(grab-focus,process-bdrag)\n\
~c s ~m ~a <Btn1Down>:process-press(extend-start,process-bdrag)\n\
~c ~m ~a <Btn1Motion>:select-adjust()\n\
~c ~m ~a <Btn1Up>:extend-end()";
static char _DtTermEventBindingsCDEBtn2[] = "\
<Btn2Down>:extend-start()\n\
<Btn2Motion>:select-adjust()\n\
<Btn2Up>:extend-end()";

static
XmTextScanType defaultScanArray[] =
{
    XmSELECT_POSITION,
    XmSELECT_WORD,
    XmSELECT_LINE,
    XmSELECT_ALL
};

static void RegisterDropSite( Widget w );
static void doExtendedSelection (Widget  w,Time  eventTime);

/* 
** Get the current server time (I ripped this off from Xm/TextIn.c).
*/
static Time
getServerTime
(
     Widget w
)
{
    XEvent    event;
    EventMask shellMask;
    
    while(!XtIsShell(w))
    {
        w = XtParent(w);
    }

    shellMask = XtBuildEventMask(w);

    if (!(shellMask & PropertyChangeMask))
    {
       XSelectInput(XtDisplay(w), XtWindow(w), shellMask | PropertyChangeMask);
    }

    XChangeProperty(XtDisplay(w), XtWindow(w), XA_WM_HINTS, XA_WM_HINTS,
                    32, PropModeAppend, (unsigned char *)NULL, 0);

    XWindowEvent(XtDisplay(w), XtWindow(w), PropertyChangeMask, &event);

    if (!(shellMask & PropertyChangeMask))
    {
       XSelectInput(XtDisplay(w), XtWindow(w), shellMask);
    }

    return(event.xproperty.time);
}


static void
setScanType
(
    Widget  w,
    XEvent *event
)
{
    TermSelectInfo  selectInfo = 
                    ((DtTermPrimitiveWidget)w)->term.tpd->selectInfo;
    int             multiClickTime;
    int             i;

    multiClickTime = XtGetMultiClickTime(XtDisplay(w));

    if (event->xbutton.time > selectInfo->lastTime &&
        event->xbutton.time - selectInfo->lastTime <
			 (multiClickTime == 200 ? 500 : multiClickTime))
    {
        i = 0;
	while (i < selectInfo->scanArraySize && 
               selectInfo->scanArray[i] != selectInfo->scanType)
        {
            i++;
        }

	if (++i >= selectInfo->scanArraySize)
        {
            i = 0;
        }
	selectInfo->scanType = selectInfo->scanArray[i];
    } 
    else
    {	
        /* single-click event */
        selectInfo->scanType = selectInfo->scanArray[0];
    }
    
    selectInfo->lastTime = event->xbutton.time;
}

/* 
** convert a row,col pair into the equivalent XmTextPosition
** 
** NOTE: 
**     this routine assumes that the calling routine as already checked
**     row and col to insure that they are within the bounds of the terminal
**     buffer (see _DtTermPrimSelectGrabFocus)
*/
XmTextPosition
rowColToPos
(
    DtTermPrimitiveWidget   tw,
    short                   row,
    short                   col
)
{
    DtTermPrimData  tpd = tw->term.tpd;

    return(((tpd->selectInfo->columns + 1) * 
            (row + tpd->lastUsedHistoryRow)) + col);
}

/* 
** getSelection
*/
Boolean
_DtTermPrimSelectGetSelection
(
    Widget          w,
    XmTextPosition *begin,
    XmTextPosition *end
)
{
    TermSelectInfo  selectInfo = 
                    ((DtTermPrimitiveWidget)w)->term.tpd->selectInfo;

    if (selectInfo->ownPrimary && 
        (selectInfo->begin <= selectInfo->end) &&
        selectInfo->begin >= 0)
    {
        *begin = selectInfo->begin;
        *end   = selectInfo->end;
        return(True);
    }
    else
    {
        *begin = 0;
        *end   = 0;
        selectInfo->ownPrimary = False;
        return(False);
    }
}    


/* 
** convert an x,y pair into the appropriate text position
** 
** Since positions count the number of inter-character spaces, there is
** one more x position on a line than columns on a line; xPos can be in
** the range (0, selectInfo->columns + 1).  The same is true for yPos,
** it can be in the range (0, tpd->lastUsedRow - tpd->topRow - 1)
**
** In the case that we have a history buffer to deal with, text positions
** in the history buffer are forced to come before positions in the term
** buffer.
**
** NOTE: 
**     this routine assumes that the calling routine as already checked
**     x and y to insure that they are within the bounds of the terminal
**     window (see _DtTermPrimSelectGrabFocus)
** NOTE: 
**     I believe I'm now doing all checking in this routine for confining
**     the x,y to the window.  Disregard the previous note. TMH
*/
static 
XmTextPosition
xyToPos
(
    DtTermPrimitiveWidget   tw,
    int                     x,                /* pixel */
    int                     y                 /* pixel */
)
{
    DtTermPrimData  tpd        = tw->term.tpd;
    TermSelectInfo  selectInfo = tpd->selectInfo;
    TermBuffer      tb;
    short           row;
    short           yPos;
    short           xPos;
    static short    oldYPos = -1;
    static short    oldXPos = -1;
    

    if ( x<0) x = 0;  
    if ( x > (int) tw->core.width) x = tw->core.width;  
    /* 
    ** convert pixel units to character positions
    */
    yPos = (MAX(0, y) - tpd->offsetY) / tpd->cellHeight;
    
    /* 
    ** yPos cannot exceed the buffer or screen
    */
    yPos = MIN(yPos, MIN(tw->term.rows, tpd->lastUsedRow - tpd->topRow) - 1) +
           tpd->topRow;

    /* 
    ** consider the possibility that we have a history buffer
    */
    if (tpd->useHistoryBuffer)
    {
        if (yPos < 0)
        {
            /* 
            ** yPos is not in the history buffer (order is important,
            ** step 2 must come before step 3):
            **    1) point to history buffer
            **    2) adjust yPos
            **    3) decide which row of the buffer we are concerned
            **       with
            */
            tb  = tpd->historyBuffer;
            yPos += tpd->lastUsedHistoryRow;
            row = yPos;
        }
        else
        {
            /* 
            ** yPos is not in the history buffer (order is important,
            ** step 2 must come before step 3):
            **    1) point to term buffer
            **    2) decide which row of the buffer we are concerned
            **       with
            **    3) adjust yPos
            */
            tb  = tpd->termBuffer;        
            row = yPos;    
            yPos += tpd->lastUsedHistoryRow;            
        }
    }
    else
    {
        tb   = tpd->termBuffer;        
        row  = yPos; 
    }

    xPos = (((x - tpd->offsetX) + (tpd->cellWidth / 2)) / tpd->cellWidth) ;

    if ( MB_CUR_MAX > 1 )  /* check if xPos splits a 2 col char */
     {      
       TermCharInfoRec charInfoRec ;
       if (_DtTermPrimGetCharacterInfo(tb,row,xPos,&charInfoRec) )
         {
            if (charInfoRec.width == 2 && charInfoRec.startCol != xPos)
              {
                if (xPos*tpd->cellWidth < x - tpd->offsetX )
                   xPos++ ;  /* set to right of char */
                else
                   xPos-- ;  /* set to left of char */
              }
         }
     }
    
        
    if ((yPos != oldYPos) || (xPos != oldXPos))
    {
        oldYPos = yPos;
        oldXPos = xPos;
    }
    return (((selectInfo->columns + 1) * yPos) + xPos);
}

/*
 * Takes a linear position and return buffer, row, and col.
 * Since positions are between characters, this returns the col to
 * right of the position.
 */
static void
posToBufferRowCol
(
    DtTermPrimitiveWidget  tw,
    XmTextPosition pos,
    TermBuffer *pb,
    short *row,
    short *col
)
{
    DtTermPrimData   tpd        = tw->term.tpd;
    TermSelectInfo   selectInfo = tpd->selectInfo;
    short lrow, lcol;

    lrow = pos / (selectInfo->columns + 1);
    lcol = pos - (lrow * (selectInfo->columns + 1));

    if ( tpd->useHistoryBuffer ) lrow -= tpd->lastUsedHistoryRow ;  

    if ( lrow < 0 )  {   /* in history buffer */
       *pb=tw->term.tpd->historyBuffer ;
       lrow += tpd->lastUsedHistoryRow ; 
     }
    else
     {
       *pb = tpd->termBuffer ;
     }
    *row = lrow ;
    *col = lcol ;
}

/*
 * Takes a buffer, row and column and returns the linear position.
 */
static XmTextPosition
bufferRowColToPos
(
    DtTermPrimitiveWidget  tw,
    TermBuffer pb,
    short row,
    short col
)
{
    DtTermPrimData   tpd        = tw->term.tpd;
    TermSelectInfo   selectInfo = tpd->selectInfo;
    short lrow, lcol;
    XmTextPosition pos;

    /* assume row, col in the history buffer or there is no history */
    pos = (tpd->selectInfo->columns + 1) * row + col;

    if ( tpd->useHistoryBuffer && pb == tpd->termBuffer)   
      pos += (tpd->selectInfo->columns + 1) * (tpd->lastUsedHistoryRow) ;
    return(pos) ;
}
   

static XmTextPosition
scan
(
    DtTermPrimitiveWidget   tw,
    XmTextPosition          scanStart, 
    XmTextScanType          scanType,
    TermScanDirection       scanDir,
    int                     count,
    Boolean                 inclusive
)
{
    int              i;
    DtTermPrimData   tpd        = tw->term.tpd;
    TermSelectInfo   selectInfo = tpd->selectInfo;
    XmTextPosition   position   = scanStart;
    short            row;
    short            col;
    TermBuffer pb ;
    

    switch(scanType)
    {
      case XmSELECT_POSITION:
        posToBufferRowCol(tw, position, &pb, &row, &col) ;
        if ( col  > _DtTermPrimBufferGetLineWidth(pb, row) )
         {
            col  = selectInfo->columns + 1;
            position = bufferRowColToPos(tw,pb,row,col) ;
         }
        break;
      case XmSELECT_WORD:
       {
        short width;
        posToBufferRowCol(tw, position, &pb, &row, &col) ;
        width = _DtTermPrimBufferGetLineWidth(pb,row);
        if ( col > width ) break;

        if (  MB_CUR_MAX > 1 ) 
          {
            TermCharInfoRec charInfoRec ;

            _DtTermPrimGetCharacterInfo(pb,row,col,&charInfoRec);
            col = charInfoRec.startCol ;  /* align first */

            switch(scanDir)
            {
              case scanLeft:
                _DtTermPrimGetCharacterInfo(pb,row,col?--col:0,&charInfoRec);
                while( !iswspace(*(wchar_t *)charInfoRec.u.pwc) &&
                                       ((col=charInfoRec.startCol-1)>=0) )
                 {
                   _DtTermPrimGetCharacterInfo(pb,row,col,&charInfoRec) ;
                 }
                col ++ ;
                break;
              case scanRight:
                _DtTermPrimGetCharacterInfo(pb,row,col,&charInfoRec);
                while(++col<=width  && !iswspace(*(wchar_t *)charInfoRec.u.pwc))
                   _DtTermPrimGetCharacterInfo(pb,row,col,&charInfoRec);
                col--;
                break;
             }
          }
        else
          {
            char pbuf[10];
            switch(scanDir)
            {
              case scanLeft:
                _DtTermPrimBufferGetText(pb, row, col?--col:0, 1, pbuf, False);
                while( !isspace(*pbuf) && --col >= 0)
                   _DtTermPrimBufferGetText(pb, row, col, 1, pbuf, False );
                col++ ;
                break;
              case scanRight:
                _DtTermPrimBufferGetText(pb, row, col, 1, pbuf, False);
                while( ++col <= width && !isspace(*pbuf))
                   _DtTermPrimBufferGetText(pb, row, col, 1, pbuf, False );
                col--;
                break;
             }
           }
         position = bufferRowColToPos(tw,pb,row,col) ;
        }
        break;
      case XmSELECT_LINE:
       {
        
        posToBufferRowCol(tw, position, &pb, &row, &col) ;
        col = 0;
        switch(scanDir)
        {
          case scanLeft:
            break;
          case scanRight:
            col =  selectInfo->columns + 1;
            break;
        }
        position = bufferRowColToPos(tw,pb,row,col) ;
       }
       break;
      case XmSELECT_ALL:
        switch(scanDir)
        {
          case scanLeft:
            position = 0;
            break;
          case scanRight:
            pb  = tpd->termBuffer ;
            row = tpd->lastUsedRow-1;
            col = _DtTermPrimBufferGetLineWidth(pb,row) ;
            position = bufferRowColToPos(tw,pb,row,col) ;
            break;
        }
        break;
    }
    return(position);
}

/* 
** refresh all text from start up to stop
** 
** NOTE: 
**     We assume that start is always <= than stop
*/
void
_DtTermPrimRenderRefreshTextLinear
(
    Widget          w,
    XmTextPosition  start,
    XmTextPosition  stop
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData          tpd        = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;
    short                   startRow, startCol;
    short                   stopRow , stopCol;
    

    /* 
    ** Turn XmTextPosition into a row and column
    */
    startRow = start / (selectInfo->columns + 1);
    startCol = start - (startRow * (selectInfo->columns + 1));
    stopRow  = stop / (selectInfo->columns + 1);
    stopCol  = stop - (stopRow * (selectInfo->columns + 1));
    
    /* 
    ** Accomodate the history buffer as necessary
    */
    if (tpd->useHistoryBuffer)
    {
        startRow -= tpd->lastUsedHistoryRow;
        stopRow  -= tpd->lastUsedHistoryRow;
    }

    /* 
    ** Now adjust for the top of the window
    */
    startRow -= tpd->topRow;
    stopRow  -= tpd->topRow;
    

    /*
    ** refresh the first (and possibly only) line
    */
    if (startRow == stopRow)
    {
        _DtTermPrimRefreshText((Widget)tw, startCol, startRow, 
                               stopCol, startRow);
        return;
    }
    _DtTermPrimRefreshText((Widget)tw, startCol, startRow, 
                      selectInfo->columns - 1, startRow);
    
    /* 
    ** refresh the middle block (if there is one)
    */
    if (startRow++ < stopRow)
    {
        _DtTermPrimRefreshText((Widget)tw, 0, startRow, 
                         selectInfo->columns - 1, stopRow - 1);
    }

    /* 
    ** refresh the last line 
    */
    _DtTermPrimRefreshText((Widget)tw, 0, stopRow, stopCol, stopRow);
    
}


static void
setSelection
(
    DtTermPrimitiveWidget   tw,
    XmTextPosition          begin, 
    XmTextPosition          end,
    Time                    selectTime,
    Boolean                 fromLoseSelection
)
{
    TermSelectInfo  selectInfo = tw->term.tpd->selectInfo;
    XmTextPosition  oldBegin, oldEnd;
    Boolean         disJoint;       /* true if new and current are disjoint */
    short	    selectLineBegin;
    short	    selectColBegin;
    short	    selectLineEnd;
    short	    selectColEnd;

    Debug('c', fprintf(stderr, ">>setSelection() starting\n"));

    if (selectInfo->ownPrimary == False &&
        begin > end)
    {
        Debug('c', fprintf(stderr, ">>setSelection() finishing a\n"));
        return;
    }    

    if (begin < 0)
    {
        begin = 0;
        end   = 0;
    }
    
    if (selectInfo->ownPrimary)
    {
        /* 
        ** we own the selection see how much (if any) of the selected
        ** area needs to be unhighlighted...
        */
        if (selectInfo->begin < selectInfo->end)
        {
            /* 
            ** We own the selection, and its highlighted...
            */
            if ((end <= selectInfo->begin) ||
                (begin >= selectInfo->end))
            {
                /* 
                ** The two areas don't intersect, simply clear the old
                ** area...
                */
                Debug('c', fprintf(stderr, "    new & old are disjoint\n"));
                selectInfo->ownPrimary = False;
                _DtTermPrimRenderRefreshTextLinear((Widget)tw, 
                                                   selectInfo->begin,
                                                   selectInfo->end - 1);
                selectInfo->ownPrimary = True;
                disJoint = True;
            }
            else
            {
                /* 
                ** There is some intersection, save the current begin
                ** and end so we can clean things up later.
                */
                Debug('c', fprintf(stderr, "    new & old intersect\n"));
                oldBegin = selectInfo->begin;
                oldEnd   = selectInfo->end;
                disJoint = False;
            }
        }
        else
        {
            /* 
            ** We own the selection, but nothing is highlighted...
            */
            disJoint = True;
        }
    }
    else
    {
        /* 
        ** we don't own the selection (yet), come up with some reasonable
        ** defaults
        */
        disJoint = True; 
        oldBegin = begin;
        oldEnd   = end;        
    }


    selectInfo->begin = begin;
    selectInfo->end   = end;

    if (begin <= end)
    {
        if (selectInfo->ownPrimary == False)
        {
            if (!XtOwnSelection((Widget)tw, XA_PRIMARY, selectTime,
                                _DtTermPrimSelectConvert,
                                _DtTermPrimSelectLoseSelection,
                                (XtSelectionDoneProc) NULL))
            {
                /* 
                ** XtOwnSelection failed, make a dummy call to setSelection
                ** (with begin > end) to clear things up...
                */
                setSelection(tw, 1, -99, selectTime, False);
            }
            else
            {
                selectInfo->ownPrimary  = True;
                selectInfo->primaryTime = selectTime;
            }
        }

        /* 
        ** now highlight the currently selected text...
        */
        if (selectInfo->ownPrimary)
        {
            if (disJoint == True)
            {
                /* 
                ** the selections are disjoint, simply draw the new one
                */
                _DtTermPrimRenderRefreshTextLinear((Widget)tw, begin, end - 1);
            }
            else
            {
                if (begin != oldBegin)
                {
                    if (begin < oldBegin)
                    {
                        /* 
                        ** refresh from the new beginning to the old 
                        ** beginning
                        */
                        _DtTermPrimRenderRefreshTextLinear((Widget)tw, begin,
                                                     oldBegin - 1);
                    }
                    else if (oldBegin < begin)
                    {
                        /* 
                        ** refresh from the old beginning to the new 
                        ** beginning
                        **
                        ** NOTE: in this case we want to unhighlight
                        **       previously selected text, so we
                        **       temporarily set ownPrimary to false
                        */
                        selectInfo->ownPrimary = False;
                        _DtTermPrimRenderRefreshTextLinear((Widget)tw, oldBegin,
                                                     begin - 1);
                        selectInfo->ownPrimary = True;
                    }
                }
                if (end != oldEnd)
                {
                    if (end < oldEnd)
                    {
                        /* 
                        ** refresh from the new end to the original end
                        **
                        ** NOTE: in this case we want to unhighlight
                        **       previously selected text, so we
                        **       temporarily set ownPrimary to false
                        */
                        selectInfo->ownPrimary = False;
                        _DtTermPrimRenderRefreshTextLinear((Widget)tw, end,
                                                     oldEnd - 1);
                        selectInfo->ownPrimary = True;
                    }
                    else if (oldEnd < end)
                    {
                        /* 
                        ** refresh from the old end to the new end.
                        */
                        _DtTermPrimRenderRefreshTextLinear((Widget)tw, oldEnd,
                                                     end - 1);
                    }
                }
            }
        }
    }
    else
    {
        if (!fromLoseSelection)
        {
            XtDisownSelection((Widget)tw, XA_PRIMARY, selectTime);
        }
        selectInfo->ownPrimary = False;
    }

    selectLineBegin = selectInfo->begin / (selectInfo->columns + 1);
    selectColBegin = selectInfo->begin % (selectInfo->columns + 1);
    selectLineEnd = (selectInfo->end - 1) / (selectInfo->columns + 1);
    selectColEnd = (selectInfo->end - 1) % (selectInfo->columns + 1);

    DebugF('c', 1,
	    fprintf(stderr, "set selection units: %d-%d  lines: %d-%d\n",
	    selectInfo->begin, selectInfo->end,
	    selectLineBegin,
	    selectLineEnd));
    if (tw->term.tpd->useHistoryBuffer && tw->term.tpd->lastUsedHistoryRow>0) {
	if (selectLineEnd > tw->term.tpd->lastUsedHistoryRow) {
	    (void) _DtTermPrimBufferSetSelectLines(tw->term.tpd->historyBuffer,
		    selectLineBegin, selectColBegin,
		    tw->term.tpd->lastUsedHistoryRow - 1, selectInfo->columns);
	} else {
	    (void) _DtTermPrimBufferSetSelectLines(tw->term.tpd->historyBuffer,
		    selectLineBegin, selectColBegin,
		    selectLineEnd, selectColEnd);
	}
	selectLineBegin -= tw->term.tpd->lastUsedHistoryRow;
	if (selectLineBegin < 0) {
	    selectLineBegin = 0;
	    selectColBegin = 0;
	}
	selectLineEnd -= tw->term.tpd->lastUsedHistoryRow;
    }
    
    if (selectLineEnd > tw->term.tpd->lastUsedRow) {
	(void) _DtTermPrimBufferSetSelectLines(tw->term.tpd->termBuffer,
		selectLineBegin, selectColBegin,
		tw->term.tpd->lastUsedRow, selectInfo->columns);
    } else {
	(void) _DtTermPrimBufferSetSelectLines(tw->term.tpd->termBuffer,
		selectLineBegin, selectColBegin,
		selectLineEnd, selectColEnd);
    }
    Debug('c', fprintf(stderr, ">>setSelection() finishing b\n"));
}


static void
handleSelection
(
    DtTermPrimitiveWidget   tw,
    int                     x,
    int                     y,
    Time                    selectTime
)
{
    XmTextPosition   position;
    XmTextPosition   newBegin;
    XmTextPosition   newEnd;
    TermSelectInfo   selectInfo = tw->term.tpd->selectInfo;

    Debug('c', fprintf(stderr, ">>handleSelection() starting\n"));

    position = xyToPos(tw, x, y);
    newBegin = scan(tw, position, selectInfo->scanType, scanLeft,
                    1,  False);
    newEnd   = scan(tw, position, selectInfo->scanType, scanRight,
                    1,  selectInfo->scanType == XmSELECT_LINE);

    setSelection(tw, newBegin, newEnd, selectTime, False);
    
    if ((position - newBegin) <
        (newEnd - position))
    {
        selectInfo->extendDir = scanLeft;
    }
    else
    {
        selectInfo->extendDir = scanRight;
    }
    selectInfo->origBegin = newBegin;
    selectInfo->origEnd   = newEnd;
}


/************************************************************************
 *                                                                      *
 * browseScroll - timer proc that scrolls the list if the user has left *
 *              the window with the button down. If the button has been *
 *              released, call the standard click stuff.                *
 *                                                                      *
 ************************************************************************/
/* ARGSUSED */
static void
browseScroll
(
        XtPointer closure,
        XtIntervalId *id 
)
{
    Widget w = (Widget) closure;
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo selectInfo           = tpd->selectInfo ;
    XmScrollBarWidget vsb = (XmScrollBarWidget) tw->term.verticalScrollBar;
    unsigned long interval;

    if (selectInfo->cancel) {
       selectInfo->selectID = 0;
       return;
    }

    if (!selectInfo->selectID) return;

    _DtTermPrimScrollComplete(w, True);
    if ( selectInfo->isScrollUp ) {
       if ( tpd->lastUsedRow-1 >= tpd->topRow + tw->term.rows) 
                 _DtTermPrimScrollText(w, 1);
     }
    else
       _DtTermPrimScrollText(w, -1);
    _DtTermPrimScrollComplete(w, True);
    if (selectInfo->extending)
      doExtendedSelection(w, XtLastTimestampProcessed(XtDisplay(w)));

    if (vsb)
       interval = (unsigned long) vsb->scrollBar.repeat_delay;
    else
       interval = 100;

    XSync (XtDisplay(w), False);

    selectInfo->selectID = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
                                   interval, browseScroll, (XtPointer) w);
}

/* ARGSUSED */
static Boolean
CheckTimerScrolling
(
    Widget w,
    XEvent *event 
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo  = tpd->selectInfo;
    XmScrollBarWidget vsb = (XmScrollBarWidget) tw->term.verticalScrollBar;
    unsigned long interval;

    selectInfo->extend.x = event->xmotion.x;
    selectInfo->extend.y = event->xmotion.y;

    if ( (event->xmotion.y > (int) tpd->offsetY) &&
        (event->xmotion.y < (int) (tpd->offsetY + tw->term.rows * 
                                                  tpd->cellHeight))) {

       if (selectInfo->selectID) {
          XtRemoveTimeOut(selectInfo->selectID);
          selectInfo->selectID = 0;
       }
    } else {
       /* above the text */
        if (event->xmotion.y <= (int) tpd->offsetY) {
           selectInfo->extend.x = 0;
           selectInfo->extend.y  = (int) (tpd->offsetY);
           selectInfo->isScrollUp = False ;

       /* below the text */
        } else if (event->xmotion.y >= (int) (tpd->offsetY + tw->term.rows *
                                                  tpd->cellHeight)) {
           selectInfo->extend.x = tw->core.width;
           selectInfo->extend.y = (int) (tpd->offsetY + tw->term.rows *
                                                  tpd->cellHeight);
           selectInfo->isScrollUp = True ;
        }

       if (vsb)
          interval = (unsigned long) vsb->scrollBar.initial_delay;
       else
          interval = 200;

       if (!selectInfo->selectID)
         selectInfo->selectID = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
                                         interval, browseScroll, (XtPointer) w);
       return True;
    }
    return False;
}

/* 
** Create and initialize the selection specific information
*/
TermSelectInfo
_DtTermPrimSelectCreate
(
    Widget  w
)
{
    DtTermPrimitiveWidget   tw  = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd = tw->term.tpd;
    TermSelectInfo          selectInfo;
    int                     i;    

    selectInfo = (TermSelectInfo)XtMalloc(sizeof(TermSelectInfoRec));
    
    selectInfo->begin         = 0;
    selectInfo->end           = 0;
    selectInfo->columns       = tw->term.columns;
    selectInfo->rows          = tpd->bufferRows + tpd->historyBufferRows;
    selectInfo->direction     = (TermScanDirection) XmTEXT_FORWARD;
    selectInfo->extend.x      = 0;
    selectInfo->extend.y      = 0;
    selectInfo->extending     = False;
    selectInfo->hint.x        = 0;
    selectInfo->hint.y        = 0;
    selectInfo->lastTime      = 0;
    selectInfo->origBegin     = 0;
    selectInfo->origEnd       = 0;
    selectInfo->ownPrimary    = False;
    selectInfo->threshold     = 5;
    selectInfo->selectID      = 0;
    selectInfo->selectType    = TermSelect_NORMAL;
    selectInfo->scanType      = defaultScanArray[0];
    selectInfo->scanArraySize = XtNumber(defaultScanArray);
    selectInfo->scanArray     = (XmTextScanType *)
                                XtMalloc(selectInfo->scanArraySize * 
                                         sizeof(XmTextScanType));
    selectInfo->cancel        = True;   /* used by scroll selection */
    selectInfo->anchor        = -1;     /* in case extend happens before set*/
    selectInfo->sel_start     = False;
    for (i = 0; i < selectInfo->scanArraySize; i++)
    {
        selectInfo->scanArray[i] = defaultScanArray[i];
    }
    
    RegisterDropSite(w);
    return(selectInfo);
}

void
_DtTermPrimSelectInitBtnEvents(Widget w) 
{
    Boolean btn1_transfer = False;
    XtVaGetValues((Widget)XmGetXmDisplay(XtDisplay(w)), "enableBtn1Transfer",
           &btn1_transfer, NULL);
    if (btn1_transfer)
        XtOverrideTranslations(w,
            XtParseTranslationTable(_DtTermEventBindingsCDE));
    if (btn1_transfer == True) /* for btn2 extend case */
        XtOverrideTranslations(w,
            XtParseTranslationTable(_DtTermEventBindingsCDEBtn2));
}

void
_DtTermPrimSelectDisown 
(
    Widget          w
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;

    if (selectInfo->ownPrimary == True)
    {
        XtDisownSelection(w, XA_PRIMARY, getServerTime(w));
        selectInfo->ownPrimary = False ;
    }
}

void
_DtTermPrimSelectDestroy
(
    Widget          w,
    TermSelectInfo  selectInfo
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;

    if (selectInfo->ownPrimary == True)
    {
        XtDisownSelection(w, XA_PRIMARY, getServerTime(w));
    }
    selectInfo->ownPrimary = False ;
    XtFree((char *) selectInfo->scanArray);
    XtFree((char *) selectInfo);
    tpd->selectInfo = NULL ;
}


/* 
** determine how much (if any) of the text is selected
**
** NOTE:
**     beginCol + width will never exceed the width of the terminal
**     buffer
*/
Boolean
_DtTermPrimSelectIsInSelection
(
    Widget      w,
    int		row,
    short	startCol,
    short	width,
    short      *selWidth
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo  = tpd->selectInfo;
    Boolean                 inSelection = True;
    XmTextPosition          endPosition;
    XmTextPosition          position;
    XmTextPosition          begin;
    XmTextPosition          end;
    short                   beginRow, beginCol;
    short                   endRow  , endCol;

    position    = rowColToPos(tw, row, startCol);
    endPosition = position + width;

    begin = selectInfo->begin;
    end   = selectInfo->end;

    if ((begin >= endPosition) || (end <= position))
    {
        /*
        ** outside of selection range...
        */
        inSelection = False;
    }
    else
    {
        /* 
        ** we're in the selection range, clip endPosition as necessary...
        */
        if (position < begin)
        {
            /*
            ** we start to the left of the selection...
            */
            inSelection = False;
            endPosition = MIN(endPosition, begin);

        } 
        else 
        {
            /*
            ** we must be in the selection, clip endPosition as
            ** necessary...
            */
            endPosition = MIN(endPosition, end);

        }
    }

    *selWidth = endPosition - position;
    return(inSelection);
}

/*ARGSUSED*/
void
_DtTermPrimSelectDoSelection
(
    Widget      w,
    XEvent     *event,
    String     *params,
    Cardinal   *paramCount
)
{
    DtTermPrimitiveWidget  tw = (DtTermPrimitiveWidget) w;

    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectDoSelection() starting\n"));

    handleSelection(tw, event->xbutton.x,  event->xbutton.y,
                    event->xbutton.time);
}               

/*ARGSUSED*/
void
_DtTermPrimSelectSetHint
(
    Widget      w,
    XEvent     *event,
    String     *params,
    Cardinal   *paramCount
)
{
    TermSelectInfo  selectInfo = 
                    ((DtTermPrimitiveWidget)w)->term.tpd->selectInfo;

    selectInfo->hint.x = event->xbutton.x;
    selectInfo->hint.y = event->xbutton.y;
}

/*ARGSUSED*/
void
_DtTermPrimSelectStart
(
    Widget      w,
    XEvent     *event,
    String     *params,
    Cardinal   *paramCount
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;
    XButtonEvent           *btnEvent   = (XButtonEvent *) event;
    XmTextPosition          begin;
    XmTextPosition          end;

    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectStart() starting\n"));

    /* 
    ** set the selection hints, and scan type
    */
    _DtTermPrimSelectSetHint(w, event, params, paramCount);
    setScanType(w, event);

    /* 
    ** Set the current anchor point
    */
    selectInfo->anchor = xyToPos(tw, btnEvent->x, btnEvent->y);
    
    if (selectInfo->scanType != XmSELECT_POSITION ||
            (_DtTermPrimSelectGetSelection(w, &begin, &end) && begin != end)
         )
    {
        _DtTermPrimSelectDoSelection(w, event, params, paramCount);
    }
}

/*ARGSUSED*/
void
_DtTermPrimSelectGrabFocus
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *paramCount
)
{
    DtTermPrimitiveWidget   tw       = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd      = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;
    XButtonEvent           *btnEvent = (XButtonEvent *) event;

    /* setDebugFlags("c") ; */
    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectGrabFocus() starting\n"));
    
    /* turn off the cursor */
    _DtTermPrimCursorOff(w);

    selectInfo->cancel = False;
    tw->term.allowOsfKeysyms = True;   /* normal dtterm doesn't honor these*/
    /* 
    ** constrain the button event to the terminal's text area
    */
    if (btnEvent->x <= (int) tpd->offsetX)
    {
        /* left */
        btnEvent->x = (int)(tpd->offsetX + 1);
    } else if (btnEvent->x >= (int)(tw->core.width - tpd->offsetX))
    {
        /* right */
        btnEvent->x = (int)(tw->core.width - tpd->offsetX - 1);
    } 

    if (btnEvent->y <= (int)tpd->offsetY)
    {
        /* above */
        btnEvent->y = (int)(tpd->offsetY + 1);
    }
    else if (btnEvent->y - ((int)(tpd->offsetY + 
                                  ((tpd->lastUsedRow - tpd->topRow) *
                                   tpd->cellHeight))) >= selectInfo->threshold)
    {
        /* below */
        btnEvent->y = (int)(tpd->offsetY + ((tpd->lastUsedRow - tpd->topRow) *
                                           tpd->cellHeight) - 1);
    }
    
    if (_XmGetFocusPolicy(w) == XmEXPLICIT)
        (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

    _DtTermPrimSelectStart(w, event, params, paramCount);
}


static
Boolean
dragged
(
    TermSelectionHint   hint,
    XEvent             *event,
    int                 threshold
)
{
    return ((abs(hint.x - event->xbutton.x) > threshold) || 
            (abs(hint.y - event->xbutton.y) > threshold));
}

/* ARGSUSED */
static void 
doExtendedSelection
(
    Widget  w,
    Time    eventTime
) 
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget) w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;
    XmTextPosition          position;
    XmTextPosition          begin;
    XmTextPosition          end;
    XmTextPosition          cursorPos;
    float                   midPoint;

    if (selectInfo->cancel) {
          if (selectInfo->selectID) XtRemoveTimeOut(selectInfo->selectID);
          selectInfo->selectID = 0;
          return;
       }

    position = xyToPos(tw, selectInfo->extend.x, selectInfo->extend.y);

    if (!(_DtTermPrimSelectGetSelection(w, &begin, &end)) || 
        (begin == end))
    {
        begin                 = position;
        end                   = position;
        if ( selectInfo->anchor <0) selectInfo->anchor  = position;   
        selectInfo->origBegin = selectInfo->anchor;
        selectInfo->origEnd   = selectInfo->anchor;
        midPoint              = (float)selectInfo->anchor;
    }
    else 
    {
        midPoint = (float)
                  (((float)(selectInfo->origEnd - 
                             selectInfo->origBegin) / 2.0) +
                    (float)selectInfo->origBegin);
    }

    /*
    ** shift anchor and direction to opposite end of the selection
    */
    if ((float)(position) <= midPoint)
    {
        selectInfo->anchor = selectInfo->origEnd;
        if (!selectInfo->extending)
        {
             selectInfo->extendDir = scanLeft;
        }
    } 
    else if ((float)(position) > midPoint)
    {
          selectInfo->anchor = selectInfo->origBegin;
          if (!selectInfo->extending)
          {
             selectInfo->extendDir = scanRight;
          }
    } 

    selectInfo->extending = TRUE;

    /*
    ** check for change in extend direction
    */
    if ((selectInfo->extendDir == scanRight && 
         position < selectInfo->anchor) ||
        (selectInfo->extendDir == scanLeft && 
         position > selectInfo->anchor))
    {
        selectInfo->extendDir = (selectInfo->extendDir == scanRight) ? 
                                scanLeft : scanRight;

        begin = selectInfo->begin;
        end   = selectInfo->end;
    }
    
    
    if (selectInfo->extendDir == scanRight)
    {
        cursorPos = scan(tw, position, selectInfo->scanType, scanRight, 1,
                         selectInfo->scanType == XmSELECT_LINE);
        end       = cursorPos;
        begin     = selectInfo->anchor;
    } 
    else
    {
        cursorPos = scan(tw, position, selectInfo->scanType, scanLeft, 1,
                         FALSE);
        begin     = cursorPos;
        end       = selectInfo->anchor;
        if (selectInfo->scanType == XmSELECT_WORD && 
                                             (int)tw->term.tpd->cellWidth > 1)
        {
            if (position == scan (tw, begin, selectInfo->scanType, scanRight, 1,
                                  FALSE))
            {
                begin = cursorPos = position;
            }
        }
    }

    setSelection(tw, begin, end, eventTime, False);
}

void
_DtTermPrimSelectExtendStart(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;

    selectInfo->cancel = False;
    tw->term.allowOsfKeysyms = True  ;

    _DtTermPrimSelectExtend(w, event, params, num_params);
}

/*ARGSUSED*/
void
_DtTermPrimSelectExtend
(
    Widget      w,
    XEvent     *event,
    String     *params,
    Cardinal   *paramCount
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;

    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectExtend() starting\n"));

    /* turn off the cursor */
    _DtTermPrimCursorOff(w);

    if (_XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

    if (selectInfo->cancel) return ;  

    if ((selectInfo->hint.x > 0) || (selectInfo->hint.y > 0))
    {
        if (dragged(selectInfo->hint, event, selectInfo->threshold))
        {
            /*
            ** extend the selection
            */
            handleSelection(tw,selectInfo->hint.x,selectInfo->hint.y, 
                                     event->xbutton.time);
            selectInfo->hint.x    = 0;
            selectInfo->hint.y    = 0;
            selectInfo->extending = True;
        }
        else
        {
            /* 
            ** do nothing
            */
            return;
        }
    }
    /*
    ** check for timer scrolling here
    ** NOTE: CheckTimerScrolling(w,event) will set extend.[x|y]
    *      selectInfo->extend.x = event->xbutton.x;
    *      selectInfo->extend.y = event->xbutton.y;
    */
    if (!CheckTimerScrolling(w,event) )
       doExtendedSelection(w, event->xbutton.time);
}

/*ARGSUSED*/
void
_DtTermPrimSelectExtendEnd
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *paramCount
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;

    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectExtendEnd() starting\n"));
 
    selectInfo->cancel = True;   /* used by scroll selection */
    tw->term.allowOsfKeysyms = False;

    if (selectInfo->extending)
    {
        _DtTermPrimSelectGetSelection(w, &selectInfo->origBegin, 
                                         &selectInfo->origEnd);
        setSelection(tw, selectInfo->origBegin, selectInfo->origEnd, 
                                            event->xbutton.time, False);
        /* _DtTermPrimSelectExtend(w, event, params, paramCount);*/
    }
    
    if (selectInfo->selectID > 0)
    {
        XtRemoveTimeOut(selectInfo->selectID);
        selectInfo->selectID = 0;
    }
    
    selectInfo->extend.x  = 0;
    selectInfo->extend.y  = 0;
    selectInfo->extending = False;
    selectInfo->hint.x    = 0;
    selectInfo->hint.y    = 0;

    /* turn off the cursor */
    _DtTermPrimCursorOn(w);
}

/*ARGSUSED*/
static void
doHandleTargets
(
    Widget          w,
    XtPointer       closure,
    Atom           *seltype,
    Atom           *type,
    XtPointer       value,
    unsigned long  *length,
    int            *format
)
{
    _TermSelectPrimaryRec  *primSelect = (_TermSelectPrimaryRec *) closure;
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd        = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;
    XTextProperty           tmpProp;
    XmTextBlockRec          block;
    int                     i, status;
    char                   *pChar;
    char                   *pCharEnd;
    char                   *pCharFollow;
    int                     malloc_size=0 , numVals ;
    char                   *total_tmp_value ;
    char                  **tmp_value ;

    Debug('c', fprintf(stderr, ">>doHandleTargets() starting\n"));

    if (_XmGetFocusPolicy(w) == XmEXPLICIT)
    {
        (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    }
    
    if (*type == XmInternAtom(XtDisplay(w), "COMPOUND_TEXT", False) ||
        *type == XA_STRING)
    {

        tmpProp.value    = (unsigned char *) value;
        tmpProp.encoding = *type;
        tmpProp.format   = *format;
        tmpProp.nitems   = *length;
        numVals           = 0;
        status            = XmbTextPropertyToTextList(XtDisplay(w), &tmpProp,
                                                      &tmp_value, &numVals );

        /*
        ** if no conversions, numVals  doesn't change
        */
        if (numVals  && (status == Success || status > 0))
        { 
            for (i = 0; i < numVals  ; i++)
            {
                 malloc_size += strlen(tmp_value[i]);
            }
            total_tmp_value = XtMalloc ((unsigned) malloc_size + 1);
            total_tmp_value[0] = '\0';
            for (i = 0; i < numVals  ; i++)
            {
                strcat(total_tmp_value, tmp_value[i]);
            }
            block.ptr    = total_tmp_value;
            block.length = strlen(total_tmp_value);
            block.format = XmFMT_8_BIT;
            XFreeStringList(tmp_value);
        } 
        else
        {
            malloc_size      = 1; /* to force space to be freed */
            total_tmp_value  = XtMalloc ((unsigned)1);
            *total_tmp_value = '\0';
            block.ptr = total_tmp_value;
            block.length = 0;
            block.format = XmFMT_8_BIT;
        }
     } else {
    
        block.ptr    = (char*)value;
        block.length = (int) *length; /* NOTE: this causes a truncation on
			   some architectures */
        block.format = XmFMT_8_BIT;
     }
 
     pCharEnd    = block.ptr + block.length;
     pCharFollow = (char *)block.ptr;

     for (pChar = (char *)block.ptr; pChar < pCharEnd; pChar++)
     {
         if (*pChar == '\n')
         {
             *pChar = '\r';
             DtTermSubprocSend(w, (unsigned char *) pCharFollow,
                           pChar - pCharFollow + 1);
             pCharFollow = pChar + 1;
         }        
     }
     if (pCharFollow < pCharEnd)
     {
         DtTermSubprocSend(w, (unsigned char *) pCharFollow,
                       pCharEnd - pCharFollow);
     }

    if (malloc_size != 0) XtFree(total_tmp_value);
    XtFree((char *)value);
    if (primSelect && (--primSelect->ref_count == 0))
    {
       XtFree((char *)primSelect);
    }    
    value = NULL ;
}

/*
** Look at the target list and determine what target to place in the
** pair.  it will then do any necessary conversions before "thrusting"
** the selection value onto the receiver.  this will guarantee the
** best chance at a successful exchange.
*/
/*ARGSUSED*/
static void
handleTargets
(
    Widget          w,
    XtPointer       closure,
    Atom           *selType,
    Atom           *type,
    XtPointer       value,
    unsigned long  *length,
    int            *format
)
{
    Atom                    CS_OF_LOCALE;
    Atom                    COMPOUND_TEXT;
    Boolean                 supportsLocaleData;
    Boolean                 supportsCompoundText;
    Atom                   *atomPtr;
    _TermSelectRec         *tmpAction;
    _TermSelectPrimaryRec  *primSelect; 
    char                   *abcString;
    XTextProperty           tmpProp;
    int                     status;
    XtPointer               closures[2];
    Atom                    targets[2];
    int                     i;

    /* 
    ** make sure we have something to do...
    */
    tmpAction = (_TermSelectRec *) closure;
    if (!length || *length == 0) {
       XtFree((char *)value);
       value = NULL;
       XtFree((char *)tmpAction->event);
       XtFree((char *)tmpAction);
       return;
    }

    COMPOUND_TEXT        = XmInternAtom(XtDisplay(w),"COMPOUND_TEXT", False);
    supportsLocaleData   = False;
    supportsCompoundText = False;
    abcString            = "ABC";  /* characters in XPCS, so... safe */
    atomPtr              = (Atom *)value;

    tmpProp.value = NULL;
    status = XmbTextListToTextProperty(XtDisplay(w), &abcString, 1,
                                      (XICCEncodingStyle)XTextStyle, &tmpProp);
    if (status == Success)
    {
        CS_OF_LOCALE = tmpProp.encoding;
    }
    else
    {
        /* 
        ** Kludge for failure of XmbText... to 
        ** handle XPCS characters.  Should never 
        ** happen, but this prevents a core dump 
        ** if X11 is broken.
        */
        CS_OF_LOCALE = (Atom)9999;
    }
    if (tmpProp.value != NULL) 
    {
        XFree((char *)tmpProp.value);
    }

    for (i = 0; i < *length; i++, atomPtr++)
    {
        if (*atomPtr == CS_OF_LOCALE)
        {
            supportsLocaleData = True;
            break;
        }
        if (*atomPtr == COMPOUND_TEXT)
        {
            supportsCompoundText = True;
        }
    }
    primSelect = (_TermSelectPrimaryRec *)
                 XtMalloc((unsigned) sizeof(_TermSelectPrimaryRec));

    /*
    ** If owner and I are using the same codeset, ask for it.  If not,
    ** and if the owner supports compound text, ask for compound text.
    ** If not, fall back position is to ask for STRING and try to
    ** convert it locally.
    */
    if (supportsLocaleData)
    {
        primSelect->target = targets[0] = CS_OF_LOCALE;
    }
    else if (supportsCompoundText)
    {
        primSelect->target = targets[0] = COMPOUND_TEXT;
    }
    else
    {
        primSelect->target = targets[0] = XA_STRING;
    }
    closures[0] = (char *)primSelect;

    primSelect->ref_count = 1;
    /*
    ** Make request to call doHandleTargets() with the primary selection.
    */
    XtGetSelectionValue(w, XA_PRIMARY, targets[0], doHandleTargets,
                        (XtPointer)primSelect,
                        tmpAction->event->xbutton.time);

    XtFree((char *)value);
    value = NULL;
    XtFree((char *)tmpAction->event);
    XtFree((char *)tmpAction);
}

static  char *
getString
(
    Widget          w,
    XmTextPosition  begin,
    XmTextPosition  end,
    Boolean         needWideChar
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget) w;
    DtTermPrimData          tpd        = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;
    TermBuffer              tb;
    short                   beginRow, 
                            beginCol;
    short                   endRow,
                            endCol;
    short                   thisRow;
    short                   numRows;
    char                   *buffer;
    char                   *pBuf;
    short                   len;
    
    beginRow = begin / (selectInfo->columns + 1);
    beginCol = begin - (beginRow * (selectInfo->columns + 1));
    endRow   = end / (selectInfo->columns + 1);
    endCol   = end - (endRow * (selectInfo->columns + 1));
    numRows  = endRow - beginRow + 1;

    /*
    ** we need to store end - begin characters, a terminating byte, plus
    ** a new line for each line...
    **
    ** NOTE: end - begin could result in a truncated long.
    */
    buffer = XtMalloc(((int)(end - begin) + 1 + numRows) * sizeof(char)
                                      * BYTES_PER_CHAR(tpd->termBuffer));

    /* 
    ** return a null string if there is nothing to do
    */
    if (begin == end)
    {
        *buffer = 0x00;
        return(buffer);
    }

    /* 
    ** Accomodate the history buffer as necessary
    */
    if (tpd->useHistoryBuffer)
    {
        beginRow -= tpd->lastUsedHistoryRow;
        endRow   -= tpd->lastUsedHistoryRow;
    }

    /*
    ** get the first (and possibly only) line of text
    */
    pBuf = buffer;
    if (beginRow == endRow)
    {
        if (beginRow < 0)
        {
            tb      = tpd->historyBuffer;
            thisRow = beginRow + tpd->lastUsedHistoryRow;
        }
        else
        {
            tb      = tpd->termBuffer;
            thisRow = beginRow;
        }
        len = _DtTermPrimBufferGetText(tb, thisRow, beginCol, 
                                       endCol - beginCol, pBuf, needWideChar);
        pBuf += len;
    }
    else
    {
        if (beginRow < 0)
        {
            tb      = tpd->historyBuffer;
            thisRow = beginRow + tpd->lastUsedHistoryRow;
        }
        else
        {
            tb      = tpd->termBuffer;
            thisRow = beginRow;
        }
        len = _DtTermPrimBufferGetText(tb, thisRow, beginCol, 
                                 selectInfo->columns - beginCol, pBuf,
                                 needWideChar);
        pBuf += len;
        
        if ( !_DtTermPrimBufferTestLineWrapFlag(tb,thisRow) ) { 
           *pBuf = '\n'; /* newline */
           pBuf++;
         }

        /* 
        ** get the middle block (if there is one)
        */
        beginRow++;
        while(beginRow < endRow)
        {
            if (beginRow < 0)
            {
                tb      = tpd->historyBuffer;
                thisRow = beginRow + tpd->lastUsedHistoryRow;
            }
            else
            {
                tb      = tpd->termBuffer;
                thisRow = beginRow;
            }
            len = _DtTermPrimBufferGetText(tb, thisRow, 0,
                                           selectInfo->columns , pBuf,
                                           needWideChar);

            pBuf += len;
            /* if (len != 0 &&  len < selectInfo->columns ) { */
            if ( !_DtTermPrimBufferTestLineWrapFlag(tb,thisRow) ) { 
               *pBuf = '\n'; /* newline */
               pBuf++;
             }
            beginRow++;
        }

        /* 
        ** get the last line 
        */
        if (endRow < 0)
        {
            tb      = tpd->historyBuffer;
            thisRow = endRow + tpd->lastUsedHistoryRow;
        }
        else
        {
            tb      = tpd->termBuffer;
            thisRow = endRow;
        }
        len = _DtTermPrimBufferGetText(tb, thisRow, 0, endCol, pBuf, 
                                       needWideChar);
        pBuf += len;
    }    
    *pBuf = 0x00;

    return(buffer);
}

/* 
** Request targets from selection owner.
*/
static void
getTargets
(
    Widget      w,
    XEvent     *event,
    String     *params,
    Cardinal   *paramCount
)
{
    _TermSelectRec *tmp;
    
    tmp = (_TermSelectRec*)XtMalloc(sizeof(_TermSelectRec));

    /*
    ** Request targets from the selection owner so you can decide what to
    ** request.  The decision process and request for the selection is
    ** taken care of in handleTargets().
    */

    tmp->event = (XEvent *) XtMalloc(sizeof(XEvent));
    memcpy((void *)tmp->event, (void *)event, sizeof(XEvent));

    tmp->params     = params;
    tmp->num_params = paramCount;

    XtGetSelectionValue(w, XA_PRIMARY,
                        XmInternAtom(XtDisplay(w), "TARGETS", False),
                        handleTargets, (XtPointer)tmp, event->xbutton.time);
}

Boolean
_DtTermPrimSelectConvert
(
    Widget          w,
    Atom           *selection,
    Atom           *target,
    Atom           *type,
    XtPointer      *value,
    unsigned long  *length,
    int            *format
)
{
    Atom  TARGETS        = XmInternAtom(XtDisplay(w), "TARGETS",         False);
    Atom  CS_OF_LOCALE;
    Atom  COMPOUND_TEXT  = XmInternAtom(XtDisplay(w), "COMPOUND_TEXT",   False);
    Atom  TEXT          = XmInternAtom(XtDisplay(w), "TEXT",            False);
    Atom MOTIF_DROP = XmInternAtom(XtDisplay(w), "_MOTIF_DROP", False);
    int     maxTargets  = 10;
    int     targetCount;
    int     status;
    Widget widget;
    Boolean ownPrimary;
    XmTextPosition begin;
    XmTextPosition end;
    char           *tmpValue;
    char           *tmpString = "ABC";  /* characters in XPCS, so... safe */
    XTextProperty   tmpProp;
    XtPointer c_ptr;
    Arg args[1];
    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectConvert() starting\n"));

    if (*selection == MOTIF_DROP) {
       XtSetArg(args[0], XmNclientData, &c_ptr);
       XtGetValues(w, args, 1);
       widget = (Widget)c_ptr;
    } else
       widget = w;

    if (widget == NULL) return False;

    tmpProp.value = NULL;
    status = XmbTextListToTextProperty(XtDisplay(widget), &tmpString, 1,
				(XICCEncodingStyle)XTextStyle, &tmpProp);
    if (status == Success)
    {
        CS_OF_LOCALE = tmpProp.encoding;
    }
    else
    {
        /*
        ** XmbTextList... SHOULD never fail for
        ** XPCS character.  But if it does, this
	** prevents a core dump.
	*/
        CS_OF_LOCALE = (Atom) 9999;
    }

    if (tmpProp.value != NULL)
    	XFree((char *) tmpProp.value);


    if (*selection == XA_PRIMARY || *selection == MOTIF_DROP)
    {
        ownPrimary = _DtTermPrimSelectGetSelection(widget, &begin, &end);
    }
    else
    {
        return(False);
    }

    if (*target == TARGETS)
    {
        Atom *targets = (Atom *)XtMalloc((unsigned)(maxTargets * sizeof(Atom)));

        /* 
        ** Xt should take care of TIME_STAMP for us.
        */
        targetCount = 0;    
        *value     = (XtPointer)targets;
        *targets++ = TARGETS;           targetCount++;
	if (!isDebugFSet('s', 1)) {
	    *targets++ = COMPOUND_TEXT;     targetCount++;
	}
	if (!isDebugFSet('s', 2)) {
	    *targets++ = CS_OF_LOCALE;      targetCount++;
	}
	if (!isDebugFSet('s', 3)) {
	    *targets++ = TEXT;              targetCount++;
	}
	if (!isDebugFSet('s', 4)) {
	    *targets++ = XA_STRING;         targetCount++;
	}
        *type   = XA_ATOM;
        *length = (targetCount * sizeof(Atom)) >> 2; /* convert to work count */
        *format = 32;
    }
    else if (!ownPrimary)
    {
       return(False);
    }  
    else if ((*target == XA_STRING && !isDebugFSet('s', 4)) || 
             (*target == COMPOUND_TEXT && !isDebugFSet('s', 1)))
    {
      tmpValue  = getString(widget, begin, end, False);
      tmpProp.value = NULL;
        if ((*target == XA_STRING) && !isDebugFSet('s', 4)) {
            *type     = (Atom) XA_STRING;
            *format   = 8;
            status  = XmbTextListToTextProperty(XtDisplay(widget), &tmpValue, 1,
					  (XICCEncodingStyle)XStringStyle, 
					  &tmpProp);
        }
        else if ((*target == COMPOUND_TEXT) && !isDebugFSet('s',1)) {
            *type    = COMPOUND_TEXT;
            *format  = 8;
            status  = XmbTextListToTextProperty(XtDisplay(widget), &tmpValue, 1,
					  (XICCEncodingStyle)XCompoundTextStyle,
					  &tmpProp);
        }
        XtFree(tmpValue);
        if (status == Success || status > 0)
        {
            /*
            ** NOTE: casting tmpProp.nitems could result in a truncated long.
            */
	    if (tmpProp.nitems > 0)
              *value  = (XtPointer) XtMalloc((unsigned)tmpProp.nitems);
	    else
              *value  = (XtPointer) XtMalloc(1);

            *length = tmpProp.nitems;
            memcpy((void*)*value, (void*)tmpProp.value,
                   (unsigned)tmpProp.nitems);
	    if (tmpProp.value != NULL)
            	XFree((char*)tmpProp.value);
        }
        else
        {
            *value  = NULL;
            *length = 0;
	    if (tmpProp.value != NULL)
            	XFree((char*)tmpProp.value);
            return(False);
        }
    }
    else if (((*target == CS_OF_LOCALE) && !isDebugFSet('s', 2)) ||
             (*target == TEXT  && !isDebugFSet('s', 3)))
    {
        *type   = CS_OF_LOCALE;
        *format = 8;
        *value  = (XtPointer)getString(widget, begin, end, False);
        *length = strlen((char*) *value);
    } 
   else
   {
      *value  =  NULL;
      *length = 0;
      return(False);
   }
    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectConvert() exiting\n"));
    return(True);
}

void
_DtTermPrimSelectLoseSelection
(
    Widget  w,
    Atom   *selection
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;
    Boolean		    restoreCursor = False;

    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectLoseSelection() starting\n"));

    if (*selection == XA_PRIMARY && selectInfo->ownPrimary)
    {

        /* 
        ** We've lost the primary selection, make a  dummy call to 
        ** setSelection (with begin > end) to clear things up...
        */
	/* turn off the cursor */
	if (tw->term.tpd->cursorState != CURSORoff) {
	    _DtTermPrimCursorOff(w);
	    restoreCursor = True;
	}

        setSelection(tw, 1, -99, XtLastTimestampProcessed(XtDisplay(w)), True);

	/* turn on the cursor */
	if (restoreCursor) {
	    _DtTermPrimCursorOn(w);
	}
    }
    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectLoseSelection() exiting\n"));
}
/*ARGSUSED*/
void
_DtTermPrimSelectBDragRelease
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *paramCount
)
{
    TermSelectInfo  selectInfo = 
                    ((DtTermPrimitiveWidget)w)->term.tpd->selectInfo;
    XButtonEvent    *btnEvent = (XButtonEvent *) event;

    Debug('c', fprintf(stderr, ">>_DtTermPrimSelectBDragRelease() starting\n"));

    /* Work around for intrinsic bug.  Remove once bug is fixed.
    ** this is for drag/drop
    */
    XtUngrabPointer(w, btnEvent->time);

    if ( selectInfo->sel_start ) getTargets(w, event, params, paramCount);
}

/*ARGSUSED*/
void
_DtTermPrimSelectInsert
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *paramCount
)
{
    DtTermPrimitiveWidget   tw         = (DtTermPrimitiveWidget)w;
    TermSelectInfo          selectInfo = tw->term.tpd->selectInfo;

    if (!selectInfo->cancel)
        _DtTermPrimSelectBDragRelease(w, event, params, paramCount);

    selectInfo->cancel = True  ;

    /* turn on the cursor */
    _DtTermPrimCursorOn(w);
}

Boolean
_DtTermPrimSelectIsAboveSelection
(
   Widget w,
   short row,
   short col
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo  = tpd->selectInfo;
    XmTextPosition          curPos, endPos;
   
    endPos = selectInfo->end ;
    curPos = rowColToPos(tw,row,col)  ;

    if ( curPos < endPos ) 
      return(True) ;
    else
      return(False) ;
}

void
_DtTermPrimSelectResize
(
   Widget w
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo  = tpd->selectInfo;
    
    _DtTermPrimSelectDisown(w) ;
    selectInfo->columns  = tw->term.columns ;
}

extern void
_DtTermPrimSelectMoveLines
(
   Widget w,
   short src,
   short dest,
   short len
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    TermSelectInfo selectInfo = tw->term.tpd->selectInfo;
    short selectLineBegin;
    short selectLineEnd;
    TermBuffer pb;
    short row,col;

    posToBufferRowCol (tw, selectInfo->begin, &pb, &row, &col);

    /* if there are no lines, etc.  return... */
    if ((len <= 0) || (src == dest) || !selectInfo->ownPrimary ||
          pb == tw->term.tpd->historyBuffer )  {
	return;
    }
    if (row >= src && row < (src + len)) {
	selectInfo->begin -= (src - dest) * (selectInfo->columns + 1);
        selectInfo->end -= (src - dest) * (selectInfo->columns + 1);
    } 
}

extern void
_DtTermPrimSelectDeleteLines
(
    Widget w,
    short src,
    short len
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    TermSelectInfo selectInfo = tw->term.tpd->selectInfo;
    short selectLineBegin;
    short selectLineEnd;
    TermBuffer pb;
    short row,col;

    posToBufferRowCol (tw, selectInfo->begin, &pb, &row, &col);

    /* if there are no lines, etc. return... */
    if ((len <= 0) || !selectInfo->ownPrimary ||  
            ((tw->term.tpd->scrollLockTopRow > 0 ||
              (tw->term.tpd->scrollLockBottomRow < tw->term.rows-1)) &&
              row < tw->term.tpd->scrollLockTopRow)) {
              
	return;
    }

    /* figure out what the begin and end lines are... */
    selectLineBegin = selectInfo->begin / (selectInfo->columns + 1);
    selectLineEnd = (selectInfo->end - 1) / (selectInfo->columns + 1);

    /* if the beginning of the selection is after the source, we need to
     * move the selection up...
     */
    if (selectLineBegin > src) {
	selectInfo->begin -= len * (selectInfo->columns + 1);
        selectInfo->end -= len * (selectInfo->columns + 1);
        if (selectInfo->begin < 0) {
	    (void) _DtTermPrimSelectDisown(w);
	}
    }
}

extern void
_DtTermPrimSelectInsertLines
(
   Widget w,
   short src,
   short len
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    TermSelectInfo selectInfo = tw->term.tpd->selectInfo;
    short selectLineBegin;
    short selectLineEnd;

    /* if there are no lines, return... */
    if ((len <= 0) || !selectInfo->ownPrimary)  {
	return;
    }

    /* figure out what the begin and end lines are... */
    selectLineBegin = selectInfo->begin / (selectInfo->columns + 1);
    selectLineEnd = (selectInfo->end - 1) / (selectInfo->columns + 1);

    /* if the beginning of the selection is at or after the source, we need to
     * move the selection up...
     */
    if (selectLineBegin >= src) {
	selectInfo->begin += len * (selectInfo->columns + 1);
        selectInfo->end += len * (selectInfo->columns + 1);
    }
}

void
_DtTermPrimSelectAll
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *paramCount
)
{
    DtTermPrimitiveWidget   tw       = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd      = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;
    XButtonEvent           *btnEvent = (XButtonEvent *) event;
    XmTextPosition	    begin;
    XmTextPosition	    end;
    
    /*  position not used in XmSELECT_ALL case */
    begin = scan(tw, (XmTextPosition) 0, XmSELECT_ALL, scanLeft,
                    1,  False);
    end   = scan(tw, (XmTextPosition) 0, XmSELECT_ALL, scanRight,
                    1,  selectInfo->scanType == XmSELECT_LINE);

    /* turn off the cursor */
    _DtTermPrimCursorOff(w);

    setSelection(tw, begin, end, event->xbutton.time, False);

    /* turn on the cursor */
    _DtTermPrimCursorOn(w);
}

void
_DtTermPrimSelectPage
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *paramCount
)
{
    DtTermPrimitiveWidget   tw       = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd      = tw->term.tpd;
    TermSelectInfo          selectInfo = tpd->selectInfo;
    XButtonEvent           *btnEvent = (XButtonEvent *) event;
    XmTextPosition begin, end;
    short lastRow, width;
    TermBuffer pb;

    begin = xyToPos(tw, 1, 1);
    end   = xyToPos(tw, tw->core.width-1, tw->core.height-1);

    /* turn off the cursor */
    _DtTermPrimCursorOff(w);

    setSelection(tw, begin, end, event->xbutton.time, False);

    /* turn on the cursor */
    _DtTermPrimCursorOn(w);
}
  
/*
 * DROP SITE code
 */

static XContext _DtTermDNDContext = 0;

static void
DropTransferCallback(
        Widget w,
        XtPointer closure,
        Atom *seltype,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format )
{
    _DtTermDropTransferRec *transfer_rec = (_DtTermDropTransferRec *) closure;
    DtTermPrimitiveWidget   tw   = (DtTermPrimitiveWidget)transfer_rec->widget;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo  = tpd->selectInfo;

     /* When type = NULL, we are assuming a DELETE request has been requested */
    if (*type == 0) {
      if (value) {
           XtFree((char *)value);
           value = NULL;
        }
      return;
     }
    doHandleTargets((Widget)tw,NULL,seltype,type,value,length,format) ;
    if (transfer_rec->move) {
       XmDropTransferEntryRec transferEntries[1];
       XmDropTransferEntryRec *transferList = NULL;

       transferEntries[0].client_data = (XtPointer) transfer_rec;
       transferEntries[0].target = XmInternAtom(XtDisplay(w),"DELETE",
                                                False);
       transferList = transferEntries;
       XmDropTransferAdd(w, transferEntries, 1);
    }
}

static void
DeleteDropContext(
        Widget w )
{
   Display *display = XtDisplay(w); 
   Screen  *screen = XtScreen(w);

   XDeleteContext(display, (Window)screen, _DtTermDNDContext);
}

static void
SetDropContext(
        Widget w )
{
   Display *display = XtDisplay(w);
   Screen  *screen = XtScreen(w);

   _DtTermProcessLock();
   if (_DtTermDNDContext == 0)
      _DtTermDNDContext = XUniqueContext();
   _DtTermProcessUnlock();

   XSaveContext(display, (Window)screen,
                _DtTermDNDContext, (XPointer)w);
}

typedef struct _dropDestroyCBClientData { 
   _DtTermDropTransferRec *transfer_rec; 
   XtCallbackRec *dropDestroyCB; } 
dropDestroyCBClientData;

/* ARGSUSED */
static void
DropDestroyCB(
    Widget      w,
    XtPointer   clientData,
    XtPointer   callData )
{
    dropDestroyCBClientData *ptr = (dropDestroyCBClientData *) clientData;

    DeleteDropContext(w);

    if (ptr) {
      if (ptr->transfer_rec)  XtFree((char *) ptr->transfer_rec);
      if (ptr->dropDestroyCB)  XtFree((char *) ptr->dropDestroyCB);
      XtFree((char *) ptr);
    }
}

static void
HandleDrop( Widget w, XmDropProcCallbackStruct *cb )
{
    Widget drag_cont, initiator;
    /* XmTextWidget tw = (XmTextWidget) w;  */
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    DtTermPrimData          tpd         = tw->term.tpd;
    TermSelectInfo          selectInfo  = tpd->selectInfo;
    Cardinal numExportTargets, n;
    Atom *exportTargets;
    Arg args[10];
    XmTextPosition insert_pos, left, right;
    XtCallbackRec *dropDestroyCB, *dd_cb;
    dropDestroyCBClientData *clientData;

    clientData = (dropDestroyCBClientData *) XtMalloc(sizeof(dropDestroyCBClientData));
    dropDestroyCB = dd_cb = (XtCallbackRec *) XtMalloc(2 * sizeof (XtCallbackRec));

    clientData->dropDestroyCB = dropDestroyCB;
    dd_cb->callback = DropDestroyCB;
    dd_cb->closure = NULL;
    dd_cb++;
    dd_cb->callback = (XtCallbackProc) NULL;
    dd_cb->closure = NULL;    

    drag_cont = cb->dragContext;

    n = 0;
    XtSetArg(args[n], XmNsourceWidget, &initiator); n++;
    XtSetArg(args[n], XmNexportTargets, &exportTargets); n++;
    XtSetArg(args[n], XmNnumExportTargets, &numExportTargets); n++;
    XtGetValues((Widget) drag_cont, args, n);

     {
       XmDropTransferEntryRec transferEntries[2];
       XmDropTransferEntryRec *transferList = NULL;
       Atom TEXT = XmInternAtom(XtDisplay(w), "TEXT", False);
       Atom COMPOUND_TEXT = XmInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
       Atom CS_OF_LOCALE;
       char * tmp_string = "ABC"; /* these are characters in XPCS, so... safe */
       XTextProperty tmp_prop;
       _DtTermDropTransferRec *transfer_rec; 
       Cardinal numTransfers = 0;
       Boolean locale_found = False;
       Boolean c_text_found = False;
       Boolean string_found = False;
       Boolean text_found = False;
       int status;

       tmp_prop.value = NULL;

       status = XmbTextListToTextProperty(XtDisplay(w), &tmp_string, 1,
                                      (XICCEncodingStyle)XTextStyle, &tmp_prop);
       if (status == Success)
          CS_OF_LOCALE = tmp_prop.encoding;
       else
          CS_OF_LOCALE = 99999; /* XmbTextList... should never fail for XPCS
                                 * characters.  But just in case someones
                                 * Xlib is broken, this prevents a core dump.
                                 */

       if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);

      /* intialize data to send to drop transfer callback  */
       transfer_rec = (_DtTermDropTransferRec *)
                       XtMalloc(sizeof(_DtTermDropTransferRec));
       clientData->transfer_rec = transfer_rec;
       transfer_rec->widget = w;
     /*  don't actually need all of this for dtterm - it was from Text widget
      *transfer_rec->insert_pos = insert_pos;
      *transfer_rec->num_chars = 0;
      *transfer_rec->timestamp = cb->timeStamp;
      *
      */

      if (cb->operation & XmDROP_MOVE) {
         transfer_rec->move = True;
      } else {
         transfer_rec->move = False;
      }

       transferEntries[0].client_data = (XtPointer) transfer_rec; 
       transferList = transferEntries;
       numTransfers = 1;

       for (n = 0; n < numExportTargets; n++) {
         if (exportTargets[n] == CS_OF_LOCALE) {
            transferEntries[0].target = CS_OF_LOCALE;
            locale_found = True;
            break;
         }
         if (exportTargets[n] == COMPOUND_TEXT) c_text_found = True;
         if (exportTargets[n] == XA_STRING) string_found = True;
         if (exportTargets[n] == TEXT) text_found = True;
       }

       n = 0;
       if (locale_found || c_text_found || string_found || text_found) {
         if (!locale_found) {
            if (c_text_found)
               transferEntries[0].target = COMPOUND_TEXT;
            else if (string_found)
               transferEntries[0].target = XA_STRING;
            else
               transferEntries[0].target = TEXT;
         }

         if ( cb->operation & (XmDROP_COPY|XmDROP_MOVE) ) {
                XtSetArg(args[n], XmNdropTransfers, transferList); n++;
                XtSetArg(args[n], XmNnumDropTransfers, numTransfers); n++;
         } else {
                XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
                XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
         }
       } else {
         XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
         XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
       }
       dropDestroyCB->closure = (XtPointer) clientData; 
       XtSetArg(args[n], XmNdestroyCallback, dropDestroyCB); n++;
       XtSetArg(args[n], XmNtransferProc, DropTransferCallback); n++;
    }
    SetDropContext(w);
    XmDropTransferStart(drag_cont, args, n);
}

/* ARGSUSED */
static void
DropProcCallback(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDropProcCallbackStruct *cb = (XmDropProcCallbackStruct *) call;

    if (cb->dropAction != XmDROP_HELP) {
       HandleDrop(w, cb);
    } else {
       Arg args[2];

       XtSetArg(args[0], XmNtransferStatus, XmTRANSFER_FAILURE);
       XtSetArg(args[1], XmNnumDropTransfers, 0);
       XmDropTransferStart(cb->dragContext, args, 2);
    }
}

static void
RegisterDropSite(
        Widget w )
{
    Atom targets[4];
    Arg args[10];
    int n;
    char * tmp_string = "ABC";  /* these are characters in XPCS, so... safe */
    XTextProperty tmp_prop;
    int status = 0;

    tmp_prop.value = NULL;
    status = XmbTextListToTextProperty(XtDisplay(w), &tmp_string, 1,
                                      (XICCEncodingStyle)XTextStyle, &tmp_prop);
    if (status == Success)
       targets[0] = tmp_prop.encoding;
    else
       targets[0] = 99999; /* XmbTextList... should never fail for XPCS
                            * characters.  But just in case someones
                            * Xlib is broken, this prevents a core dump.
                            */

    if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);

    targets[1] = XmInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
    targets[2] = XA_STRING;
    targets[3] = XmInternAtom(XtDisplay(w), "TEXT", False);

    n = 0;
    XtSetArg(args[n], XmNimportTargets, targets); n++;
    XtSetArg(args[n], XmNnumImportTargets, 4); n++;
    /* XtSetArg(args[n], XmNdragProc, DragProcCallback); n++; */
    XtSetArg(args[n], XmNdropProc, DropProcCallback); n++;
    XmDropSiteRegister(w, args, n);
}


/*
 * DRAG SITE code
 */

/* ARGSUSED */
static void
StartDrag(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    DtTermPrimitiveWidget  tw = (DtTermPrimitiveWidget) w;
    Atom targets[4];
    char * tmp_string = "ABC";  /* these are characters in XPCS, so... safe */
    XTextProperty tmp_prop;
    int status = 0;
    Cardinal num_targets = 0;
    Widget drag_icon;
    Arg args[10];
    int n = 0;

    tmp_prop.value = NULL;
    status = XmbTextListToTextProperty(XtDisplay(w), &tmp_string, 1,
                                      (XICCEncodingStyle)XTextStyle, &tmp_prop);
    if (status == Success)
       targets[num_targets++] = tmp_prop.encoding;
    else
       targets[num_targets++] = 99999; /* XmbTextList...  should never fail
                                        * for XPCS characters.  But just in
                                        * case someones Xlib is broken,
                                        * this prevents a core dump.
                                        */

    if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);

    targets[num_targets++] = XmInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
    targets[num_targets++] = XA_STRING;
    targets[num_targets++] = XmInternAtom(XtDisplay(w), "TEXT", False);

    drag_icon = (Widget) XmeGetTextualDragIcon(w);

    n = 0;
    XtSetArg(args[n], XmNcursorBackground, tw->core.background_pixel);  n++;
    XtSetArg(args[n], XmNcursorForeground, tw->primitive.foreground);  n++;
    XtSetArg(args[n], XmNsourceCursorIcon, drag_icon);  n++; 
    XtSetArg(args[n], XmNexportTargets, targets);  n++;
    XtSetArg(args[n], XmNnumExportTargets, num_targets);  n++;
    XtSetArg(args[n], XmNconvertProc, _DtTermPrimSelectConvert);  n++;
    XtSetArg(args[n], XmNclientData, w);  n++;
    XtSetArg(args[n], XmNdragOperations, ( XmDROP_COPY)); n++;
    (void) XmDragStart(w, event, args, n);
}

static Position
GetXFromPos(Widget w, XmTextPosition pos)
{
   DtTermPrimitiveWidget  tw = (DtTermPrimitiveWidget) w;
   DtTermPrimData tpd = tw->term.tpd;
   TermBuffer pb;
   short row,col;
   TermCharInfoRec charInfoRec ;
   
   posToBufferRowCol(tw,pos,&pb,&row,&col) ;
   return(tpd->offsetX+col*tpd->cellWidth);
}

/* ARGSUSED */
void
_DtTermPrimSelectProcessBDrag(
        Widget w,
        XEvent *event,
        char **params,
        Cardinal *num_params )
{
    DtTermPrimitiveWidget  tw = (DtTermPrimitiveWidget) w;
    TermSelectInfo  selectInfo =
                    ((DtTermPrimitiveWidget)w)->term.tpd->selectInfo;
    XmTextPosition position, left, right;
    Position left_x, left_y, right_x, right_y;
    /* InputData data = tw->text.input->data; */

    selectInfo->cancel = False;
    position = xyToPos(tw, event->xbutton.x, event->xbutton.y);

    if (_DtTermPrimSelectGetSelection(w, &left, &right) &&
        (right != left)) {
       if ((position > left && position < right) 
         || (position == left &&
               event->xbutton.x > GetXFromPos(w, left))
         || (position == right &&
               event->xbutton.x < GetXFromPos(w, right))) {
           selectInfo->sel_start = False;
           StartDrag(w, event, params, num_params);
         }
        else
         {
          selectInfo->sel_start = True ;
         }
     }
    else selectInfo->sel_start = True ;
}


/* This is the menu interface for copy clipboard */
Boolean 
_DtTermPrimSelectCopyClipboard
(
    Widget w,
    Time copy_time
)
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    XmTextPosition begin;
    XmTextPosition end;

    char *selected_string = NULL;                  /* text selection */
    long item_id = 0L;                 	           /* clipboard item id */
    long data_id = 0L;                             /* clipboard data id */
    int status;                                    /* clipboard status  */
    XmString clip_label;
    XTextProperty tmp_prop;
    Display *display = XtDisplay(w);
    Window window = XtWindow(w);
    char *atom_name;

    if ( _DtTermPrimSelectGetSelection(w, &begin, &end) && begin != end ) {
      selected_string = getString(w, begin, end, False);
     }
    /*
     * Using the Xm clipboard facilities,
     * copy the selected text to the clipboard
     */
    tmp_prop.value = NULL;
    if (selected_string != NULL) {
       clip_label = XmStringCreateLocalized ("XM_TERM");
       /* start copy to clipboard */
       status = XmClipboardStartCopy(display, window, clip_label, copy_time,
		       w, NULL, &item_id);

       if (status != ClipboardSuccess) {
         XtFree(selected_string);
         XmStringFree(clip_label);
         return False;
       }
 
       status = XmbTextListToTextProperty(display, &selected_string, 1,
	                    (XICCEncodingStyle)XStdICCTextStyle,
			    &tmp_prop);

       if (status != Success && status <= 0) {
          XmClipboardCancelCopy(display, window, item_id);
          XtFree(selected_string);
          XmStringFree(clip_label);
          return False;
        }

       atom_name = XGetAtomName(display, tmp_prop.encoding);

       /* move the data to the clipboard */
       status = XmClipboardCopy(display, window, item_id, atom_name,
	          (XtPointer)tmp_prop.value, tmp_prop.nitems,
		  0, &data_id);

       XtFree(atom_name);

       if (status != ClipboardSuccess) {
            XmClipboardCancelCopy(display, window, item_id);
            XFree((char*)tmp_prop.value);
            XmStringFree(clip_label);
            return False;
        }

       /* end the copy to the clipboard */
       status = XmClipboardEndCopy (display, window, item_id);

       XtFree((char*)tmp_prop.value);
       XmStringFree(clip_label);

       if (status != ClipboardSuccess) return False;
     } else
         return False;

   if (selected_string!=NULL) 
	XtFree(selected_string);
   return True;
}

/* This is the event interface for copy clipboard */
void
_DtTermPrimSelectCopyClipboardEventIF
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
)
{
    _DtTermPrimSelectCopyClipboard(w,event->xkey.time) ;
}

/*
 * Retrieves the current data from the clipboard
 * and paste it at the current cursor position
 */
Boolean 
_DtTermPrimSelectPasteClipboard
(
      Widget w 
)
{
      XmTextPosition sel_left = 0;
      XmTextPosition sel_right = 0;
      XmTextPosition paste_pos_left, paste_pos_right, cursorPos;
      int status;                                /* clipboard status        */
      char * buffer;                             /* temporary text buffer   */
      unsigned long length;                      /* length of buffer        */
      unsigned long outlength = 0L;              /* length of bytes copied  */
      long private_id = 0L;                      /* id of item on clipboard */
      Boolean dest_disjoint = True;
      XmTextBlockRec block, newblock;
      Display *display = XtDisplay(w);
      Window window = XtWindow(w);
      Boolean get_ct = False;
      Boolean freeBlock;
      XTextProperty tmp_prop;
      int malloc_size = 0;
      int num_vals;
      char **tmp_value;
      char * total_tmp_value = NULL;
      int i;

      status = XmClipboardInquireLength(display, window, "STRING", &length);

      if (status == ClipboardNoData || length == 0) {
         status = XmClipboardInquireLength(display, window, "COMPOUND_TEXT",
					   &length);
         if (status == ClipboardNoData || length == 0) return False;
         get_ct = True;
      }

      /* malloc length of clipboard data */
      buffer = XtMalloc((unsigned) length);

      if (!get_ct) {
         status = XmClipboardRetrieve(display, window, "STRING", buffer,
				       length, &outlength, &private_id);
      } else {
         status = XmClipboardRetrieve(display, window, "COMPOUND_TEXT",
				       buffer, length, &outlength, &private_id);
      }


      if (status != ClipboardSuccess) {
	XmClipboardEndRetrieve(display, window);
	XtFree(buffer);
        return False;
      }

      tmp_prop.value = (unsigned char *) buffer;
      if (!get_ct)
         tmp_prop.encoding = XA_STRING;
      else
	 tmp_prop.encoding = XmInternAtom(display, "COMPOUND_TEXT", False);

      tmp_prop.format = 8;
      tmp_prop.nitems = outlength;
      num_vals = 0;

      status = XmbTextPropertyToTextList(display, &tmp_prop, &tmp_value,
					 &num_vals);

     /* if no conversions, num_vals doesn't change */
      if (num_vals && (status == Success || status > 0)) {
	 for (i = 0; i < num_vals ; i++)
	     malloc_size += strlen(tmp_value[i]);

	 total_tmp_value = XtMalloc ((unsigned) malloc_size + 1);
	 total_tmp_value[0] = '\0';
	 for (i = 0; i < num_vals ; i++)
	    strcat(total_tmp_value, tmp_value[i]);
	 block.ptr = total_tmp_value;
	 block.length = strlen(total_tmp_value);
	 block.format = XmFMT_8_BIT;
	 XFreeStringList(tmp_value);
      } else {
	 malloc_size = 1; /* to force space to be freed */
	 total_tmp_value = XtMalloc ((unsigned)1);
	 *total_tmp_value = '\0';
	 block.ptr = total_tmp_value;
	 block.length = 0;
	 block.format = XmFMT_8_BIT;
      }

      /* add new text */
      if ( block.length )
       {
         char  *pChar, *pCharEnd, *pCharFollow;

         pCharEnd    = block.ptr + block.length;
         pCharFollow = (char *)block.ptr;

         for (pChar = (char *)block.ptr; pChar < pCharEnd; pChar++)
         {
             if (*pChar == '\n')
             {
                 *pChar = '\r';
                 DtTermSubprocSend(w, (unsigned char *) pCharFollow,
                               pChar - pCharFollow + 1);
                 pCharFollow = pChar + 1;
             }
         }
         if (pCharFollow < pCharEnd)
         {
             DtTermSubprocSend(w, (unsigned char *) pCharFollow,
                           pCharEnd - pCharFollow);
         }
      }
     XtFree(buffer);
     if (malloc_size != 0) XtFree(total_tmp_value);

     (void) _DtTermPrimCursorOn(w);
}

/* This is the event interface for paste clipboard */
void    
_DtTermPrimSelectPasteClipboardEventIF
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
)
{
    _DtTermPrimSelectPasteClipboard(w) ;
}

/*
 *  This is for the SUN two button mouse
 */
static Bool
LookForButton (
        Display * display,
        XEvent * event,
        XPointer arg)
{

#define DAMPING 5
#define ABS_DELTA(x1, x2) (x1 < x2 ? x2 - x1 : x1 - x2)

    if( event->type == MotionNotify)  {
        XEvent * press = (XEvent *) arg;

        if (ABS_DELTA(press->xbutton.x_root, event->xmotion.x_root) > DAMPING ||
            ABS_DELTA(press->xbutton.y_root, event->xmotion.y_root) > DAMPING)
            return(True);
    }
    else if (event->type == ButtonRelease)
        return(True);
    return(False);
}

/* ARGSUSED */
static Boolean
XmTestInSelection(
        Widget w,
        XEvent *event )
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    TermSelectInfo  selectInfo = tw->term.tpd->selectInfo;
    XmTextPosition position, left, right;
    Position left_x, right_x, dummy;

    position = xyToPos(tw, event->xbutton.x, event->xbutton.y);

    if ((!(_DtTermPrimSelectGetSelection(w, &left, &right) &&
        (left != right && (position > left && position < right)))
         || (position == left &&
               event->xbutton.x > GetXFromPos(w, left))
         || (position == right &&
               event->xbutton.x < GetXFromPos(w, right))) 
         ||
           /* or if it is part of a multiclick sequence */
           (event->xbutton.time > selectInfo->lastTime &&
            event->xbutton.time - selectInfo->lastTime  <
              XtGetMultiClickTime(XtDisplay((Widget)w))) )
        return(False);
    else {
        /* The determination of whether this is a transfer drag cannot be made
           until a Motion event comes in.  It is not a drag as soon as a
           ButtonUp event happens or the MultiClickTimeout expires. */
        XEvent new;

        XPeekIfEvent(XtDisplay(w), &new, LookForButton, (XPointer)event);
        switch (new.type)  {
            case MotionNotify:
               return(True);
               break;
            case ButtonRelease:
               return(False);
               break;
        }
        return(False);
    }
}

#define SELECTION_ACTION        0
#define TRANSFER_ACTION         1

/* ARGSUSED */
void
_DtTermPrimSelect2ButtonMouse(
        Widget w,
        XEvent *event,
        char **params,
        Cardinal *num_params )
{
   /*  This action happens when Button1 is pressed and the Selection
       and Transfer are integrated on Button1.  It is passed two
       parameters: the action to call when the event is a selection,
       and the action to call when the event is a transfer. */

   if (*num_params != 2 /* || !XmIsTextField(w) */)
      return;
   if (XmTestInSelection(w, event))
      XtCallActionProc(w, params[TRANSFER_ACTION], event, params, *num_params);
   else
      XtCallActionProc(w, params[SELECTION_ACTION], event, params, *num_params);
}


void
_DtTermPrimSelectProcessCancel(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    DtTermPrimitiveWidget   tw          = (DtTermPrimitiveWidget)w;
    TermSelectInfo  selectInfo = tw->term.tpd->selectInfo;
    XmTextPosition left_x, right_x;

    XmParentInputActionRec  p_event ;

    if (!tw->term.allowOsfKeysyms) {
      _DtTermPrimActionKeyInput(w,event,params,num_params);
      return;
    }
      
    selectInfo->cancel = True ;

    /* turn off the cursor */
    _DtTermPrimCursorOff(w);

    /* reset to origLeft and origRight */
    setSelection (tw, selectInfo->origBegin, selectInfo->origEnd, 
                              event->xkey.time, False) ;

    /* turn on the cursor */
    _DtTermPrimCursorOn(w);

    if (selectInfo->selectID) {
       XtRemoveTimeOut(selectInfo->selectID);
       selectInfo->selectID = 0;
    }
}
