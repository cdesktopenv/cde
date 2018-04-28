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
 * $XConsortium: TermPrimData.h /main/2 1996/10/30 13:00:24 cde-hp $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 * (c) Copyright 1995       Digital Equipment Corporation               *
 */

#ifndef	_Dt_TermPrimData_h
#define	_Dt_TermPrimData_h

#include "TermHeader.h"
#include "TermPrimBuffer.h"
#include "TermPrimParser.h"
#include "TermPrimPendingText.h"
#include "TermPrimRender.h"
#include "TermPrimSelect.h"
#include "TermPrimLineFont.h"
#include <stdio.h>
#include <limits.h>

#define	NUM_FONTS	4
#define KEYBOARD_LOCKED(kbdLocked) ((kbdLocked).escape      || \
                                    (kbdLocked).xferPending || \
                                    (kbdLocked).error       || \
                                    (kbdLocked).record)
    
typedef struct {
    Font fid;
    Pixel foreground;
    Pixel background;
    GC gc;
} TermGC;

/* 
** There are 4 reasons that the keyboard could be locked...
*/
typedef struct _TermKbdLockStatus
{
    Boolean escape;         /* escape sequence (or auto lock)       */
    Boolean xferPending;    /* a transfer is pending                */
    Boolean error;          /* an error message is being displayed  */
    Boolean record;         /* we're in record mode                 */
} TermKbdLockStatus;


typedef struct _OnTheSpotData {
    short row;
    short column;
    short start;
    short end;
    short pre_len;
    short cursor;
    Boolean under_preedit;
} OnTheSpotDataRec, *OnTheSpotData;

#define PreRow(w) ((w)->term.tpd->onthespot->row)
#define PreColumn(w) ((w)->term.tpd->onthespot->column)
#define PreStart(w) ((w)->term.tpd->onthespot->start)
#define PreEnd(w) ((w)->term.tpd->onthespot->end)
#define PreLen(w) ((w)->term.tpd->onthespot->pre_len)
#define PreCursor(w) ((w)->term.tpd->onthespot->cursor)
#define PreUnder(w) ((w)->term.tpd->onthespot->under_preedit)


typedef struct termData {
    /*********************************************************************
     * Pty
     */
    XtInputId ptyOutputId;              /* pty master Xt output id      */
    XtInputId ptyInputId;		/* pty master Xt input id	*/

    /*********************************************************************
     * Font
     */
    TermFont termFont;			/* passed in base term font	*/
    TermFont boldTermFont;		/* passed in bold term font	*/
    TermFont defaultTermFont;		/* default base term font	*/
    TermFont defaultBoldTermFont;	/* default bold term font	*/
#ifdef	DKS
    unsigned char renderTermFontsNum;	/* number of above fonts	*/
    unsigned char baseFontIndex;	/* number of base font		*/
    unsigned char altFontIndex;		/* number of alternate font	*/
#endif	/* DKS */
    short cellWidth;			/* width of base font		*/
    short cellHeight;			/* height of base font		*/
    short ascent;			/* ascent of base font		*/

    /*********************************************************************
     * Buffer
     */
    TermBuffer  termBuffer;             /* character/enhancment buffer  */    
    short bufferRows;			/* rows of term buffer memory	*/
    short bufferRowRatio;		/* scaled ratio of term buffer
					 * memory to term window
					 */

    TermBuffer	historyBuffer;		/* history character buffer	*/
    short historyBufferRows;		/* rows of term history buffer	*/
    
    short leftMargin;			/* left margin			*/
    short rightMargin;			/* right margin			*/
    /* 
    ** 
    */
    short topRow;      	        	/* number of top row on screen  */
    short cursorColumn;			/* current cursor position, col	*/
    short cursorRow;			/* current cursor position, row	*/
    short lastUsedRow;			/* number of last used buffer row.
					 * if == 0, no rows used, if == 1,
					 * 1 row used, etc.
					 */
    short lastUsedHistoryRow;
    short offsetX;			/* width of internal window
					 * border.  With Motif, this is
					 * the width of the
					 * highlightThickness and
					 * shadowThickness.
					 */
    short offsetY;			/* height of internal window
					 * border (see above)
					 */

    /* 
    ** Generic mode information...
    */
    Boolean transmitFunctions;		/* if true, transmit functions	 (A) */
    Boolean autoWrapRight;		/* if true, wrap at right margin (C) */
    Boolean wrapRightAfterInsert;	/* if true, wrap after insert	    */
    enum {
	WRAPbetweenMargins = 0,
	WRAPatRightMargin,
	WRAPpastRightMargin
    }	    wrapState;			/* margin wrap state		    */

    /* 
    ** Memory lock status...
    */
    enum {
	SCROLL_LOCKoff = 0,
	SCROLL_LOCKon,
	SCROLL_LOCKprotect
    }       scrollLockMode;  		/* memory lock mode	            */
    short   scrollLockTopRow;		/* bottom of top scroll lock region */
    short   scrollLockBottomRow;	/* top of bottom scroll lock region */

   /*  for mapOnOutput */
    time_t creationTime ;
    Boolean windowMapped;

   /* pointer blanking */
    Boolean  pointerFirst;                /* for initialization */
    Boolean  pointerOn;                   /* Is pointer on? */
    XtIntervalId pointerTimeoutID;        /* ID of current timeout */
    Boolean  pointerFrozen;               /* Is pointer outside window? */
    Cursor   noPointer;                   /* blank pointer pixmap       */

   /* logging */
    FILE *logStream;                       /* file pointer for logging */
    uid_t uid;                             /* user id of user   */
    gid_t gid;                             /* group id of user   */
    char *shell;                           /* users shell */
        

/* these will go away when the associated files are updated...
 */
#define	memoryLockRow		scrollLockTopRow
#define	memoryLockMode		scrollLockMode
#define	MEMORY_LOCKoff		SCROLL_LOCKoff
#define	MEMORY_LOCKon		SCROLL_LOCKon
#define	MEMORY_LOCKprotect	SCROLL_LOCKprotect


    /*********************************************************************
     * Parser
     */
    ParserContext   context;            /* context of the parser        */
    Boolean         parserNotInStartState;
    int		    mbCurMax;		/* max bytes per char for locale*/
    unsigned char   mbPartialChar[MB_LEN_MAX];
					/* partial multi-byte char	*/
    int		    mbPartialCharLen;	/* length of above		*/

    /*********************************************************************
     * Render GCs
     */
    TermGC renderGC;			/* GC for rendering text	*/
    TermGC renderReverseGC;		/* GC for rendering text	*/
    TermGC clearGC;			/* GC for clearing text		*/

    /*********************************************************************
     * Cursor
     */
    XtIntervalId cursorTimeoutId;	/* cursor timeout id		*/
    TermGC cursorGC;			/* GC for blinking cursor	*/
    enum {
	CURSORon,
	CURSORoff,
	CURSORblink
    } cursorState;			/* state of cursor		*/
    Boolean cursorVisible;		/* true if cursor is visible	*/

    /*********************************************************************
     * Input Method
     */
    Boolean IMHasFocus;			/* true if IM has focus		*/
    /* The following are in character positions, not pixel positions...
     */
    short IMCursorColumn;		/* current IM cursor column	*/
    short IMCursorRow;			/* current IM cursor row	*/

    /*********************************************************************
     * Scroll
     */
    /* the following data reflects the copy area in progress to be used
     * for filling in possible holes in exposure events...
     */
    short scrollSrcX;			/* x src coord of scrolled area	*/
    short scrollSrcY;			/* y src coord of scrolled area	*/
    short scrollDestX;			/* x dest coord of scrolled area*/
    short scrollDestY;			/* y dest coord of scrolled area*/
    short scrollWidth;			/* width of scrolled area	*/
    short scrollHeight;			/* height of scrolled area	*/
    short scrollTopRow;			/* row of scroll top		*/
    short scrollBottomRow;		/* row of scroll bottom		*/
    Boolean *scrollRefreshRows;		/* flags for rows that need to
					 * be refreshed after a scroll
					 */

    Boolean useHistoryBuffer;		/* true if separate history buf	*/
    Boolean allowScrollBelowBuffer;	/* true allows scrolling below
					 * the terminal buffer
					 */

    /* the following is for data for both any current scroll-in-progress
     * as well as any pending (queued) scroll...
     */
    Boolean scrollInProgress;		/* true if scroll in progresss	*/
    union {
	struct {
	    short scrollLines;		/* number of lines to scroll	*/
	    short scrollsPending;	/* number of scrolls pending	*/
	    Boolean scrolled;		/* true when scrolled, cleared
					 * when updated
					 */
	} jump;
	struct {
	    short pendingScrollLines;	/* number of lines in pending	*/
	    short pendingScrollTopRow;	/* top row of pending scroll	*/
	    short pendingScrollBottomRow;
					/* bottom row of pending scroll	*/
	    Boolean pendingScroll;	/* True if scroll queued up	*/
	} nojump;
    } scroll;

    /* the following is for stuffing input data when we have turned off
     * input processing during a scroll...
     */
    PendingText pendingRead;		/* pending read data		*/
    Boolean readInProgress;		/* True if now reading from pty */

    /* 
    ** Set up a linked list of text chunks, data stored here is waiting
    ** to be written.
    */
    PendingText pendingWrite;		/* pending write data		*/
    
    /* 
    ** Keyboard state information...
    */
    TermKbdLockStatus   keyboardLocked;	/* if true, keyboard is locked	*/
    DtTermInsertCharMode  insertCharMode;	/* insert char mode		*/
    Boolean autoLineFeed;		/* if true, in auto LF mode	*/
    Boolean halfDuplex;			/* if true, half duplex		*/

    /*********************************************************************
     * Caps lock tracking
     */
    Boolean capsLock;			/* true == caps lock set	*/
    KeyCode *capsLockKeyCodes;		/* keyboard's modifier keymap	*/
    short numCapsLockKeyCodes;		/* number of caps lock modifiers*/

    /*********************************************************************
     * kshmode tracking
     */
    unsigned int metaMask;		/* mod mask used for meta	*/

    /*********************************************************************
     * STOP indicator tracking
     */
    Boolean outputStopped;		/* true == out from pty stopped	*/
    Boolean oneSecondPause;		/* true == esc @ pause		*/

    /*********************************************************************
     * Warning/error dialog
     */
    Widget warningDialog;		/* warning dialog		*/
    Boolean mapWarningDialog;		/* map dialog when window mapped*/
    Boolean warningDialogMapped;	/* true if dialog is mapped	*/
    enum {
	userMessage,
	memoryFull
    } warningDialogType;

    /*********************************************************************
     * /etc/utmp stuff
     */
    char *utmpId;			/* id of /etc/utmp entry	*/
#ifdef	UTMPID_PROPERTY_NOTIFY
    enum {
	utmpIdInvalid = 0,
	utmpIdValidNotClear,
	utmpIdValid,
	utmpIdError
    } utmpIdState;
#endif	/* UTMPID_PROPERTY_NOTIFY */
#ifdef	UTMPID_PIPE_NOTIFY
    int utmpNotifyPipe[2];
    XtInputId utmpNotifyPipeInputId;
    XtInputId utmpNotifyPipeExceptId;
#endif	/* UTMPID_PIPE_NOTIFY */

    /* 
    ** Selections...
    */
    TermSelectInfo  selectInfo;

    /*
    ** On the spot input method data
    */
    OnTheSpotData onthespot;

} DtTermPrimDataRec, *DtTermPrimData;


#endif	/* _Dt_TermPrimData_h */
