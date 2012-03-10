/* $XConsortium: TermPrim.h /main/1 1996/04/21 19:16:41 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef	_Dt_TermPrim_h
#define	_Dt_TermPrim_h

#include <sys/wait.h>
#include <Xm/Xm.h>

#ifdef	__cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* Resources */

#ifndef DtNbackgroundIsSelect
#define	DtNbackgroundIsSelect	"backgroundIsSelect"
#endif
#ifndef DtNblinkRate
#define DtNblinkRate		"blinkRate"
#endif
#ifndef DtNcharCursorStyle
#define	DtNcharCursorStyle	"charCursorStyle"
#endif
#ifndef DtNconsoleMode
#define	DtNconsoleMode		"consoleMode"
#endif
#ifndef DtNcsWidth
#define	DtNcsWidth		"csWidth"
#endif
#ifndef DtNemulationId
#define	DtNemulationId		"emulationId"
#endif
#ifndef	DtNinputVerifyCallback
#define	DtNinputVerifyCallback	"inputVerifyCallback"
#endif
#ifndef DtNjumpScroll
#define	DtNjumpScroll		"jumpScroll"
#endif
#ifndef DtNkshMode
#define	DtNkshMode              "kshMode"       
#endif
#ifndef DtNlogFile
#define	DtNlogFile              "logFile"    
#endif
#ifndef DtNlogInhibit
#define	DtNlogInhibit		"logInhibit"
#endif
#ifndef DtNlogging
#define	DtNlogging              "logging"    
#endif
#ifndef DtNloginShell
#define	DtNloginShell		"loginShell"
#endif
#ifndef DtNmapOnOutput
#define	DtNmapOnOutput       	"mapOnOutput"
#endif
#ifndef DtNmapOnOutputDelay
#define	DtNmapOnOutputDelay     "mapOnOutputDelay"
#endif
#ifndef DtNmarginBell
#define	DtNmarginBell		"marginBell"
#endif
#ifndef DtNnMarginBell
#define	DtNnMarginBell		"nMarginBell"
#endif
#ifndef	DtNoutputLogCallback
#define	DtNoutputLogCallback	"outputLogCallback"
#endif
#ifndef DtNpointerBlank
#define	DtNpointerBlank         "pointerBlank"
#endif
#ifndef DtNpointerBlankDelay
#define	DtNpointerBlankDelay    "pointerBlankDelay"
#endif
#ifndef DtNpointerColor
#define	DtNpointerColor         "pointerColor"
#endif
#ifndef DtNpointerColorBackground
#define	DtNpointerColorBackground "pointerColorBackground"
#endif
#ifndef DtNpointerShape
#define	DtNpointerShape         "pointerShape"
#endif
#ifndef DtNreverseVideo
#define	DtNreverseVideo         "reverseVideo"
#endif
#ifndef DtNsaveLines
#define	DtNsaveLines		"saveLines"
#endif
#ifndef DtNsizeList
#define	DtNsizeList		"sizeList"
#endif
#ifndef DtNstatusChangeCallback
#define	DtNstatusChangeCallback	"statusChangeCallback"
#endif
#ifndef DtNstickyNextCursor
#define	DtNstickyNextCursor	"stickyNextCursor"
#endif
#ifndef DtNstickyPrevCursor
#define	DtNstickyPrevCursor	"stickyPrevCursor"
#endif
#ifndef DtNsubprocessLoginShell
#define	DtNsubprocessLoginShell	"subprocessLoginShell"
#endif
#ifndef DtNsubprocessPid
#define	DtNsubprocessPid	"subprocessPid"
#endif
#ifndef DtNsubprocessExec
#define	DtNsubprocessExec	"subprocessExec"
#endif
#ifndef DtNsubprocessTerminationCatch
#define	DtNsubprocessTerminationCatch "subprocessTerminationCatch"
#endif
#ifndef DtNsubprocessCmd
#define	DtNsubprocessCmd	"subprocessCmd"
#endif
#ifndef DtNsubprocessArgv
#define	DtNsubprocessArgv	"subprocessArgv"
#endif
#ifndef DtNsubprocessTerminationCallback
#define	DtNsubprocessTerminationCallback "subprocessTerminationCallback"
#endif
#ifndef DtNtermEmulationMode
#define	DtNtermEmulationMode	"termEmulationMode"
#endif
#ifndef DtNtermDevice
#define	DtNtermDevice		"termDevice"
#endif
#ifndef DtNtermDeviceAllocate
#define	DtNtermDeviceAllocate	"termDeviceAllocate"
#endif
#ifndef DtNtermId
#define	DtNtermId		"termId"
#endif
#ifndef DtNtermName
#define	DtNtermName		"termName"
#endif
#ifndef DtNtermSlaveName
#define	DtNtermSlaveName	"termSlaveName"
#endif
#ifndef DtNttyModes
#define	DtNttyModes		"ttyModes"
#endif
#ifndef DtNuseFontSets
#define	DtNuseFontSets		"useFontSets"
#endif
#ifndef DtNuseLineDraw
#define	DtNuseLineDraw		"useLineDraw"
#endif
#ifndef DtNuserFont
#define	DtNuserFont		"userFont"
#endif
#ifndef DtNuserBoldFont
#define	DtNuserBoldFont		"userBoldFont"
#endif
#ifndef DtNvisualBell
#define	DtNvisualBell		"visualBell"
#endif
#ifndef DtNallowSendEvents
#define	DtNallowSendEvents      "allowSendEvents"
#endif
#ifndef DtNbaseHeight
#define DtNbaseHeight		XmNbaseHeight
#endif
#ifndef DtNbaseWidth
#define DtNbaseWidth		XmNbaseWidth
#endif
#ifndef DtNcolumns
#define DtNcolumns		XmNcolumns
#endif
#ifndef DtNheightInc
#define DtNheightInc		XmNheightInc
#endif
#ifndef DtNmarginHeight
#define DtNmarginHeight		XmNmarginHeight
#endif
#ifndef DtNmarginWidth
#define DtNmarginWidth		XmNmarginWidth
#endif
#ifndef DtNrows
#define DtNrows			XmNrows
#endif
#ifndef DtNshadowType
#define DtNshadowType		XmNshadowType
#endif
#ifndef DtNverticalScrollBar
#define DtNverticalScrollBar	XmNverticalScrollBar
#endif
#ifndef DtNwidthInc
#define DtNwidthInc		XmNwidthInc
#endif

#ifndef DtCBackground
#define	DtCBackground		XmCBackground
#endif
#ifndef DtCBackgroundIsSelect
#define	DtCBackgroundIsSelect	"BackgroundIsSelect"
#endif
#ifndef DtCBlinkRate
#define DtCBlinkRate		"BlinkRate"
#endif
#ifndef DtCCallback
#define DtCCallback		XmCCallback
#endif
#ifndef DtCCursor
#define	DtCCursor		XmCCursor
#endif
#ifndef DtCConsoleMode
#define	DtCConsoleMode		"ConsoleMode"
#endif
#ifndef DtCCsWidth
#define	DtCCsWidth		"CsWidth"
#endif
#ifndef DtCCharCursorStyle
#define	DtCCharCursorStyle	"CharCursorStyle"
#endif
#ifndef DtCEmulationId
#define	DtCEmulationId		"EmulationId"
#endif
#ifndef DtCForeground
#define	DtCForeground		XmCForeground
#endif
#ifndef DtCJumpScroll
#define	DtCJumpScroll		"JumpScroll"
#endif
#ifndef DtCKshMode
#define	DtCKshMode              "KshMode"       
#endif
#ifndef DtCLogFile
#define	DtCLogFile              "LogFile"    
#endif
#ifndef DtCLogInhibit
#define	DtCLogInhibit		"LogInhibit"
#endif
#ifndef DtCLogging
#define	DtCLogging              "Logging"    
#endif
#ifndef DtCLoginShell
#define	DtCLoginShell		"LoginShell"
#endif
#ifndef DtCMapOnOutput
#define	DtCMapOnOutput       	"MapOnOutput"
#endif
#ifndef DtCMapOnOutputDelay
#define	DtCMapOnOutputDelay     "MapOnOutputDelay"
#endif
#ifndef DtCMarginBell
#define	DtCMarginBell		"MarginBell"
#endif
#ifndef DtCNMarginBell
#define	DtCNMarginBell		"NMarginBell"
#endif
#ifndef DtCPointerBlank
#define	DtCPointerBlank         "PointerBlank"
#endif
#ifndef DtCPointerBlankDelay
#define	DtCPointerBlankDelay    "PointerBlankDelay"
#endif
#ifndef DtCPointerColor
#define	DtCPointerColor         "PointerColor"
#endif
#ifndef DtCPointerColorBackground
#define	DtCPointerColorBackground "PointerColorBackground"
#endif
#ifndef DtCPointerShape
#define	DtCPointerShape         "PointerShape"
#endif
#ifndef DtCReverseVideo
#define	DtCReverseVideo		"ReverseVideo"
#endif
#ifndef DtCSaveLines
#define	DtCSaveLines		"SaveLines"
#endif
#ifndef DtCSizeList
#define	DtCSizeList		"SizeList"
#endif
#ifndef DtCStickyCursor
#define	DtCStickyCursor		"StickyCursor"
#endif
#ifndef DtCSubprocessLoginShell
#define	DtCSubprocessLoginShell	"SubprocessLoginShell"
#endif
#ifndef DtCSubprocessPid
#define	DtCSubprocessPid	"SubprocessPid"
#endif
#ifndef DtCSubprocessExec
#define	DtCSubprocessExec	"SubprocessExec"
#endif
#ifndef DtCSubprocessTerminationCatch
#define	DtCSubprocessTerminationCatch "SubprocessTerminationCatch"
#endif
#ifndef DtCSubprocessCmd
#define	DtCSubprocessCmd	"SubprocessCmd"
#endif
#ifndef DtCSubprocessArgv
#define	DtCSubprocessArgv	"SubprocessArgv"
#endif
#ifndef DtCTermEmulationMode
#define	DtCTermEmulationMode	"TermEmulationMode"
#endif
#ifndef DtCTermDevice
#define	DtCTermDevice		"TermDevice"
#endif
#ifndef DtCTermDeviceAllocate
#define	DtCTermDeviceAllocate	"TermDeviceAllocate"
#endif
#ifndef DtCTermId
#define	DtCTermId		"TermId"
#endif
#ifndef DtCTermName
#define	DtCTermName		"TermName"
#endif
#ifndef DtCTermSlaveName
#define	DtCTermSlaveName	"TermSlaveName"
#endif
#ifndef DtCTtyModes
#define	DtCTtyModes		"TtyModes"
#endif
#ifndef DtCUseFontSets
#define	DtCUseFontSets		"UseFontSets"
#endif
#ifndef DtCUseLineDraw
#define	DtCUseLineDraw		"UseLineDraw"
#endif
#ifndef DtCUserFont
#define	DtCUserFont		"UserFont"
#endif
#ifndef DtCUserBoldFont
#define	DtCUserBoldFont		"UserBoldFont"
#endif
#ifndef DtCVisualBell
#define	DtCVisualBell		"VisualBell"
#endif
#ifndef DtCAllowSendEvents
#define	DtCAllowSendEvents      "AllowSendEvents"
#endif
#ifndef DtCBaseHeight
#define DtCBaseHeight		XmCBaseHeight
#endif
#ifndef DtCBaseWidth
#define DtCBaseWidth		XmCBaseWidth
#endif
#ifndef DtCColumns
#define DtCColumns		XmCColumns
#endif
#ifndef DtCHeightInc
#define DtCHeightInc		XmCHeightInc
#endif
#ifndef DtCMarginHeight
#define DtCMarginHeight		XmCMarginHeight
#endif
#ifndef DtCMarginWidth
#define DtCMarginWidth		XmCMarginWidth
#endif
#ifndef DtCRows
#define DtCRows			XmCRows
#endif
#ifndef DtCShadowType
#define DtCShadowType		XmCShadowType
#endif
#ifndef DtCVerticalScrollBar
#define DtCVerticalScrollBar	XmCVerticalScrollBar
#endif
#ifndef DtCWidthInc
#define DtCWidthInc		XmCWidthInc
#endif

/* Representation types */

#define DtRDtTermCharCursorStyle	"DtTermCharCursorStyle"
#define DtRDtTermEmulationMode    	"DtTermEmulationMode"
#define	DtRDtTermTerminalSize		"DtTermTerminalSize"
#define	DtRDtTermTerminalSizeList	"DtTermTerminalSizeList"

/* DtNshadowType values */

#ifndef DtSHADOW_IN
#define DtSHADOW_IN		XmSHADOW_IN
#endif
#ifndef DtSHADOW_OUT
#define DtSHADOW_OUT		XmSHADOW_OUT
#endif
#ifndef DtSHADOW_ETCHED_IN
#define DtSHADOW_ETCHED_IN	XmSHADOW_ETCHED_IN
#endif
#ifndef DtSHADOW_ETCHED_OUT
#define DtSHADOW_ETCHED_OUT	XmSHADOW_ETCHED_OUT
#endif

#define	DtTERM_CHAR_CURSOR_BOX		0
#define	DtTERM_CHAR_CURSOR_BAR		1
#define	DtTERM_CHAR_CURSOR_INVISIBLE	2

#define	DtTermEMULATIONHP	0
#define	DtTermEMULATIONANSI	1

/* Callback reasons */

#define	DtCR_TERM_SUBPROCESS_TERMINATION	1
#define DtCR_TERM_STATUS_CHANGE		2
#define	DtCR_TERM_INPUT_VERIFY		7
#define	DtCR_TERM_OUTPUT_LOG		8


/*
 * Types
 */

/* insert char mode */

typedef enum {
	DtTERM_INSERT_CHAR_OFF = 0,
	DtTERM_INSERT_CHAR_ON,
	DtTERM_INSERT_CHAR_ON_WRAP
} DtTermInsertCharMode;

typedef struct {
	int		reason;
	XEvent		*event;
	int		cursorX;
	int		cursorY;
	Boolean		capsLock;
	Boolean		stop;
	DtTermInsertCharMode insertCharMode;
	Boolean		locked;
} DtTermStatusChangeCallbackStruct;

typedef struct {
	int		reason;
	XEvent		*event;
	pid_t		pid;
	int		status;
} DtTermSubprocessTerminationCallbackStruct;

typedef struct {
	int		reason;
	XEvent		*event;
	Boolean		doit;
	unsigned char	*text;
	int		length;
} DtTermInputVerifyCallbackStruct;

typedef struct {
	int		reason;
	XEvent		*event;
	unsigned char	*text;
	int		length;
} DtTermOutputLogCallbackStruct;

typedef struct _DtTermTerminalSize {
	short		rows;
	short		columns;
} DtTermTerminalSize;

typedef struct _DtTermTerminalSizeList {
	int		numSizes;
	DtTermTerminalSize *sizes;
} DtTermTerminalSizeList;

/* Widget class and instance */

typedef struct _DtTermPrimitiveClassRec *DtTermPrimitiveWidgetClass;
typedef struct _DtTermPrimitiveRec      *DtTermPrimitiveWidget;


/*
 * Data
 */

/* Widget class record */

externalref WidgetClass dtTermPrimitiveWidgetClass;


/*
 * Functions
 */

extern Widget DtCreateTermPrimitive(
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

extern void DtTermInitialize(void);

extern void DtTermDisplaySend(
		Widget		widget,
		unsigned char	*buffer,
		int		length);

extern void DtTermSubprocSend(
		Widget		widget,
		unsigned char	*buffer,
		int		length);

extern void DtTermSubprocReap(
		pid_t		pid,
		int		*stat_loc);

#ifdef	__cplusplus
}
#endif

#endif	/* _Dt_TermPrim_h */
