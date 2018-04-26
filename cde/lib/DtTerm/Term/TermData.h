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
 * $XConsortium: TermData.h /main/1 1996/04/21 19:15:38 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermData_h
#define	_Dt_TermData_h

#include "TermPrimData.h"
#include "TermPrimRender.h"
#include "TermFunctionKey.h"

#define	NUM_RENDER_FONTS	3
#define	RENDER_FONT_NORMAL	0
#define	RENDER_FONT_LINEDRAW	1
#define	RENDER_FONT_BOLD	2

#define	NO_USER_KEY_STRINGS 15
typedef struct _VtFontInfoRec {
    TermFont termFont;
    short id;
} VtFontRec, *VtFont;
    
typedef struct _VtColorPairRec {
    XColor fg;
    XColor bg;
    XColor hb;
    Boolean fgCommon;
    Boolean hbValid;
    Boolean bgCommon;
    Boolean initialized;
} VtColorPairRec, *VtColorPair;

typedef struct _VtsaveCursor {
    /* The following "save" symbols are for "Save cursor state" (ESC7 & ESC8) */
    short cursorRow;      /* to save current row    */
    short cursorColumn;   /* to save current column */
    char enhFieldState; /* to save current Char Erase state   */
    char enhVideoState; /* to save current video enhancements */
    char enhFgColorState;   /* index into color pair for fg text color*/
    char enhBgColorState;   /* index into color pair for bg text color */
    Boolean originMode;  /* to save current origin mode        */
    Boolean wrapMode;    /* to save current Wrap mode    */
    int *GL;			/* left graphics character set	*/
    int *GR;			/* right graphics character set	*/
    int G0;			/* graphics 0 character set	*/
    int G1;			/* graphics 1 character set	*/
    int G2;			/* graphics 2 character set	*/
    int G3;			/* graphics 3 character set	*/
    int singleShiftFont;	/* single shift character set	*/
    Boolean singleShiftPending;	/* single shift ready to go	*/
    /* short charSets;        don't need these yet */
    /*  short charShift;  ** to save char shift states for G0, etc */
 } VtSaveCursorRec, *VtSaveCursor ;

typedef struct _VtSaveDECMode {
    Boolean allow80_132ColMode;  /* true if we allow changing (xterm spec)*/
    Boolean col132Mode ;          /* True if 132 column mode           */
    Boolean applicationMode;  /* true if app mode, false if normal mode     */
    Boolean applicationKPMode;  /* true if app keypad mode                  */
    Boolean originMode;       /* true when origin mode is active          */
    Boolean wrapMode;         /* true when wrapRightMode is true          */
    Boolean jumpScrollMode;  
    Boolean inverseVideoMode;
    Boolean fixCursesMode; 
    Boolean reverseWrapMode;
    Boolean marginBellMode;
    Boolean cursorVisible;
    Boolean logging;
 } VtSaveDECModeRec, *VtSaveDECMode  ;

typedef struct _DtTermDataRec {
    /*********************************************************************
     * Provide the term data structure so we can use a single pointer
     */
    DtTermPrimData    tpd;

    /*********************************************************************
     * Font
     */
    VtFontRec renderFonts[NUM_RENDER_FONTS];
					/* fonts to use for the terinal	*/
    int G0;				/* Graphics sets 0-4		*/
    int G1;
    int G2;
    int G3;
    int *GL;				/* Graphics Left and Right	*/
    int *GR;
    int singleShiftFont;		/* single shift character set	*/
    Boolean singleShiftPending;		/* single shift ready to go	*/
    unsigned char baseFontIndex;	/* number of base font		*/
    unsigned char altFontIndex;		/* number of alternate font	*/

    /*********************************************************************
     * Colors
     */
    VtColorPairRec colorPairs[9];	/* color pair pixels		*/

    /*********************************************************************
     * User (and other) Function keys
     */
    unsigned char *userKeyString[NO_USER_KEY_STRINGS+1] ;

    Boolean applicationMode;  /* true if app mode, false if normal mode     */
    Boolean applicationKPMode;  /* true if app keypad mode                  */
    Boolean originMode;       /* true when origin mode is active          */
    Boolean S8C1TMode;        /* true when xmitting 8 bit c1 codes        */
    Boolean col132Mode ;          /* True if 132 column mode           */
    Boolean reverseWrapMode;  /* true if we're doing Reverse auto  wrap*/
    Boolean fixCursesMode;     /* true if we're handling the tab problem */
    Boolean userKeysLocked;         /* true if user defined keys are locked   */
    Boolean needToLockUserKeys;  /* true if need to lock keys (this is a parser
                                  state and will mostly be false) */

    int compatLevel;         /* 1=vt100, 2=vt200 */
    int terminalId;          /* 220, 100, 101, 102  */
    char enhVideoState;		/* current video enhancement state	*/
    char enhFieldState;		/* current field enhancement state	*/
    char enhFgColorState;	/* current fg color enhancement state	*/
    char enhBgColorState;	/* current bg color enhancement state	*/
    char enhFontState;		/* current font enhancement state	*/

    VtSaveCursorRec saveCursor ;
 
    VtSaveDECModeRec saveDECMode ;
    
} DtTermDataRec, *DtTermData;


#endif	/* _Dt_TermData_h */
