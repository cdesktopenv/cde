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
 * $XConsortium: TermPrimP.h /main/1 1996/04/21 19:18:08 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimP_h
#define	_Dt_TermPrimP_h

#include <Xm/LabelP.h>
#include "TermPrim.h"
#include "TermPrimData.h"

/* include the other internal Term include files...
 */
#include "TermPrimScroll.h"
#include "TermPrimRender.h"
#include "TermPrimCursor.h"
#include "TermPrimLineFont.h"
#include "TermPrimBuffer.h"
#include "TermPrimSubproc.h"

/* 
** Now we can choose what shell we want
** NOTE:
**  (is it worth making resources for defaultShell
**  and defaultShellArgv0?)  JRM
*/
#ifdef    hpV4
#   define DEFAULT_SHELL        "/usr/bin/sh"
#   define DEFAULT_SHELL_ARGV0  "sh"
#else  /* hpV4 */
#   define DEFAULT_SHELL        "/bin/sh"
#   define DEFAULT_SHELL_ARGV0  "sh"
#endif /* hpV4 */

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

/* term class structure... */

typedef int
(*TermInsertProc)
(
    Widget		  w,
    unsigned char 	 *buffer,
    int			  length
);
    
typedef struct _DtTermPrimitiveClassPart
{
    StateTable			*parser_start_state;
    Boolean			use_history_buffer;
    Boolean			allow_scroll_below_buffer;
    Boolean			wrap_right_after_insert;
    BufferCreateProc            buffer_create_proc;
    BufferFreeProc		buffer_free_proc;
    TermInsertProc		term_insert_proc;
    short                       sizeOfBuffer;
    short                       sizeOfLine;
    short                       sizeOfEnh;
} DtTermPrimitiveClassPart;

/* full class record declaration for Term class... */
typedef struct _DtTermPrimitiveClassRec {
    CoreClassPart		core_class;
    XmPrimitiveClassPart	primitive_class;
    DtTermPrimitiveClassPart	term_primitive_class;
} DtTermPrimitiveClassRec;

externalref DtTermPrimitiveClassRec dtTermPrimitiveClassRec;

/* term instance record... */
typedef struct _DtTermPrimitivePart
{
    struct termData *tpd;		/* non-widget terminal data	*/
    Widget verticalScrollBar;		/* vertical sb to update/track	*/
    short verticalScrollBarValue;	/* cached sb value		*/
    short verticalScrollBarMaximum;	/* cached sb maximum		*/
    short verticalScrollBarSliderSize;	/* cached sb size		*/
    short verticalScrollBarPageIncrement;
					/* cached sb page inc		*/
    unsigned char charCursorStyle;	/* cursor style: bar or box	*/
    int blinkRate;			/* cursor blink rate (0 == no
					 * blink
					 */
    Boolean consoleMode;		/* true == get console output	*/
    Boolean backgroundIsSelect;		/* true == background is select
					 * color
					 */
    Boolean visualBell;			/* true == visual bell		*/
    Boolean marginBell;			/* true == rt margin waring bell*/
    int nMarginBell;			/* warning bell distance	*/
    Boolean jumpScroll;			/* true == jumpscroll,
					 * false == line by line scroll
					 */
    Boolean hasFocus;			/* true == window has focus
					 * (and the cursor blinks)
					 */
    char *emulationId;			/* $TERMINAL_EMULATOR variable	*/
    char *termId;			/* term ID string for esc seq	*/
    char *termName;			/* $TERM string			*/
    char *ttyModes;			/* ttyModes string		*/
    char *csWidth;			/* csWidth string		*/
    unsigned char shadowType;		/* shadow type for term window	*/
    XmFontList fontList;		/* fontList for base font	*/
    XmFontList boldFontList;		/* fontList for bold font	*/
    XFontStruct *font;			/* font for base font		*/
    XFontStruct *boldFont;		/* font for bold font		*/
    XFontSet fontSet;			/* fontSet generated from either
					 * the fontList or the font above
					 */
    XFontSet boldFontSet;		/* fontSet generated from either
					 * the boldFontList or the boldFont
					 * above
					 */
    Boolean haveFontSet;		/* true == we have a valid fontSet
					 */
    char *saveLines;			/* save lines or screens	*/
    short rows;				/* rows of displayed term win	*/
    short columns;			/* columns of term win & memory	*/
    Dimension marginHeight;		/* width of window's margin	*/
    Dimension marginWidth;		/* height of window's margin	*/
    int baseWidth;			/* base width for size hints	*/
    int baseHeight;			/* base height for size hints	*/
    int widthInc;			/* width inc for size hints	*/
    int heightInc;			/* height inc for size hints	*/
    int ascent;				/* ascent for font		*/
    int pty;				/* file descriptor of pty	*/
    Boolean ptyAllocate;		/* true, allocate a pty		*/
    char *ptySlaveName;			/* name of pty slave device	*/
    int subprocessPid;			/* process id of child (shell)
					 * process
					 */
    char *subprocessCWD;		/* subprocess working directory */
    char *subprocessCmd;		/* subprocess cmd to exec	*/
    char **subprocessArgv;		/* argv to pass to above cmd	*/
    Boolean subprocessLoginShell;	/* if true, prepend argv[0] with
					 * a '-'
					 */
    Boolean subprocessTerminationCatch;	/* if true, catch child exit()	*/
    Boolean subprocessExec;		/* if true, exec subprocess	*/
    _termSubprocId subprocessId;	/* id of subprocess as returned
					 * by the subprocess code.
					 */
    XtCallbackList inputVerifyCallback;	/* callback invoked when keyboard
					 * input or selection input is
					 * received.
					 */
    XtCallbackList outputLogCallback;	/* callback invoked when pty output
					 * is received.
					 */
    XtCallbackList statusChangeCallback;/* callback invoked when there is
					 * some sort of status change
					 * (cursor motion, capsLockChange,
					 * outputStopped, etc).  Reason
					 * indicates which has changed.
					 */
    XtCallbackList subprocessTerminationCallback;
					/* callback invoked by subprocess
					 * termination (exit())
					 */
    Boolean  kshMode;                     /* use meta for escape */
    Boolean  pointerBlank;                /* blank pointer after */
    int      pointerBlankDelay;           /* pointerBlankDelay secs */
    Cursor   pointerShape;                /* pointer pixmap         */
    Pixel    pointerColor;                /* pointer color          */
    Pixel    pointerColorBackground;      /* pointer background color */
    Boolean  mapOnOutput;                 /* map term on pty output after */
    int      mapOnOutputDelay;            /* mapOnOutputDelay secs  */
    Boolean  logging;                     /* copy tty output to file/pipe  */
    Boolean  log_on;                      /* is log file (pipe)  open?     */
    char *   logFile;                     /* named logFile (default "logFile"*/
    Boolean  logInhibit;                  /* don't allow logging         */
    Boolean  reverseVideo;		  /* true, reverse video on	*/
    uid_t    euid_root;                   /* suid */
    uid_t    euid_user;                   /* user id   */
    Boolean  allowSendEvents;             /* allow key, button presses */
    Boolean  allowOsfKeysyms;		  /* allow osfKeysyms through	*/
} DtTermPrimitivePart;

/* full instance record declaration... */

typedef struct _DtTermPrimitiveRec {
    CorePart		core;
    XmPrimitivePart	primitive;
    DtTermPrimitivePart		term;
} DtTermPrimitiveRec;

/* private function declarations... */
/* end private function declarations... */

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */
/* 
** Some handy macros.
*/
#define GetParserContext(w)             (((DtTermPrimitiveWidget)w)->term.tpd->context)


#endif	/* _Dt_TermPrimP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
