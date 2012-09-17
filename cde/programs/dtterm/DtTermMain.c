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
static char rcs_id[] = "$TOG: DtTermMain.c /main/19 1999/09/17 13:26:34 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifdef sun
#include <dlfcn.h>
#endif /* sun */
#include "TermHeader.h"
#include "TermPrimDebug.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <signal.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/Protocols.h>
#include <Xm/Display.h>
/*
 * This for the sleazy hack to get the window manager frame width/height
 */
#include <Xm/VendorSEP.h>
#define	DEBUG_INCLUDES	/*DKS*/
#ifdef	DEBUG_INCLUDES
#include "TermPrimBufferP.h"
#include "TermPrimParserP.h"
#include "TermPrimPendingTextP.h"
#include "TermPrimSelectP.h"
#include "TermPrimP.h"
#include "TermPrimRenderP.h"
#include "TermPrimLineDrawP.h"
#include "TermPrimLineFontP.h"
#include "TermBufferP.h"
#include "TermP.h"
#include "TermParseTableP.h"
#include "TermFunction.h"
/* #include "TermFunctionKeyP.h" */
#include "TermViewP.h"
#endif	/* DEBUG_INCLUDES */
#include "TermPrim.h"
#include "TermView.h"
#include "TermViewMenu.h"
#include "DtTermMain.h"
#include "DtTermSyntax.h"
#ifdef	TERMINAL_SERVER
#include "DtTermServer.h"
#endif	/* TERMINAL_SERVER */
#include "TermPrimWMProtocols.h"
#include "TermPrimMessageCatI.h"
#ifdef	LOG_USAGE
#include "DtTermLogit.h"
#endif	/* LOG_USAGE */
#ifdef  HPVUE
#include <Xv/Session.h>
#include <Xv/EnvControl.h>
#define DtSessionSavePath 	XvSessionSavePath 
#define DtSessionRestorePath 	XvSessionRestorePath 
#define DtWsmGetWorkspacesOccupied XvGetWorkspacePresence
#define DtWsmSetWorkspacesOccupied XvSetWorkspaceHints
#else   /*  HPVUE */
#include <Dt/Dt.h>
#include <Dt/Wsm.h>
#include <Dt/EnvControlP.h>
#endif  /* HPVUE */ 

/* From Xm/BaseClassI.h */
extern XmWidgetExtData _XmGetWidgetExtData( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */

/* forward declarations... */
static void activateCallback(
    Widget		  w,
    XtPointer		  client_data,
    XtPointer		  call_data
);

static void ToggleMenuBar(
    Widget		  w,
    XtPointer		  client_data,
    XtPointer		  call_data
);

static void SaveSessionCB(
	Widget w,			
	caddr_t client_data,	
	caddr_t call_data );
static Boolean RestoreSession(
	Widget topDog,
	Arg *arglist,
	int i);
static void SaveTerm(
        DtTermViewWidget dtvw,
        int cloneNum,
        int fd);
static void RestoreTerm(
	Widget shellWidget,
        DtTermViewWidget dtvw,
	int termNum,
        XrmDatabase db);
static void SetWorkSpaceHints(
        Widget shell,
        char *workspaces );

static XtEventHandler TestProbeHandler(
 	Widget	w,
	XtPointer	client_data,
	XEvent		*event,
	Boolean		*ctd);
static void RegisterTestProbe(
    Widget                topLevel,
    DtTermViewWidget      tw);

static char defaultIgnoredKeysyms[] = "\
<Key>F1\n\
<Key>F2\n\
<Key>F3\n\
<Key>F4\n\
<Key>F5\n\
<Key>F6\n\
<Key>F7\n\
<Key>F8\n\
<Key>F9\n\
<Key>F10\n\
<Key>F11\n\
<Key>F12\
";

typedef struct _termattrs {
        char    *debugString;
        char    *ignoredKeysyms;
        char    *session;
        Boolean  loginShell;
#ifdef TERMINAL_SERVER
        Boolean  block;
        Boolean  exitOnLastClose;
        Boolean  server;
        char    *serverId;
        Boolean  standAlone;
        int      PingInterval;
#endif
#ifdef SUN_TERMINAL_SERVER
       Boolean server;
#endif   
        char *title;
        char *icon_name;
        char *dtterm_name;
} TERMATTRS;

#ifdef TERMINAL_SERVER
static TERMATTRS attrs = { NULL, defaultIgnoredKeysyms, NULL, False,
                           TRUE, False, False, NULL, False, 5, NULL, NULL, NULL};
#elif SUN_TERMINAL_SERVER
static TERMATTRS attrs = { NULL, defaultIgnoredKeysyms, NULL, False, False, NULL, NULL, NULL};
#else
static TERMATTRS attrs = { NULL, defaultIgnoredKeysyms, NULL, False, NULL, NULL, NULL };
#endif

static Atom xa_MOTIF_WM_MESSAGES = None;
static Atom xa_DTTERM_TOGGLE_MENUBAR = None;
static Atom xa_DTTERM_TEST_PROBE_IDS = None;
static Atom xa_DTTERM_REMOTE_PROBE_REQUEST = None;
static Atom xa_WM_SAVE_YOURSELF;
static XtAppContext appContext;
static Widget topLevel;
static Widget topShell;
static char *title = NULL;
static DtTermViewWidget *instance_list = NULL;
static Boolean RestoringTerms = False;
static int  savedArgc = 0;
static char	**savedArgv = NULL;

#ifdef SUN_TERMINAL_SERVER
int InstanceCount = 0;
#else
#ifndef TERMINAL_SERVER
static int InstanceCount = 0;
#endif  /* TERMINAL_SERVER */
#endif /* SUN_TERMINAL_SERVER */

static const char _true[] = "true";
static const char _false[] = "false";

/* command line options...
 */
static XrmOptionDescRec optionDescList[] = {
/* special (not end user) options...
 */
{"-~",             "*debugLevel",       XrmoptionSepArg, (XtPointer) NULL},
{"-debugLevel",    "*debugLevel",       XrmoptionSepArg, (XtPointer) NULL},
{"-session",       "*session",		XrmoptionSepArg, (XtPointer) NULL},

#ifdef	TERMINAL_SERVER
/* server options...
 */
{"-noBlocking",    "*blocking",		XrmoptionNoArg,  (XtPointer) _false},
{"-server",        "*server",		XrmoptionNoArg,  (XtPointer) _true},
{"-serverId",      "*serverId",		XrmoptionSepArg, (XtPointer) NULL},
#endif	/* TERMINAL_SERVER */
#ifdef	SUN_TERMINAL_SERVER
{"-dtserver",      "*server",		XrmoptionNoArg,  (XtPointer) _true},
{"-sdtserver",     "*server",	        XrmoptionNoArg,  (XtPointer) _true},
#endif	/* SUN_TERMINAL_SERVER */

/* terminal emulation options...
 */
{"-132",           "*c132",		XrmoptionNoArg,  (XtPointer) _true},
{"+132",           "*c132",		XrmoptionNoArg,  (XtPointer) _false},
{"-aw",            "*autoWrap",		XrmoptionNoArg,  (XtPointer) _true},
{"+aw",            "*autoWrap",		XrmoptionNoArg,  (XtPointer) _false},
{"-bs",            "*backgroundIsSelect",
					XrmoptionNoArg,  (XtPointer) _true},
{"+bs",            "*backgroundIsSelect",
					XrmoptionNoArg,  (XtPointer) _false},
{"-C",             "*consoleMode",	XrmoptionNoArg,  (XtPointer) _true},
{"-cc",            "*charClass",	XrmoptionSepArg, (XtPointer) NULL},
{"-cu",            "*curses",           XrmoptionNoArg,  (XtPointer) _true},
{"+cu",            "*curses",           XrmoptionNoArg,  (XtPointer) _false},
{"-e",             NULL,		XrmoptionSkipLine,
							 (XtPointer) NULL},
{"-fb",            "*userBoldFont",	XrmoptionSepArg, (XtPointer) NULL},
{"-fn",            "*userFont",		XrmoptionSepArg, (XtPointer) NULL},
{"+iconic",        ".iconic",           XrmoptionNoArg,  (XtPointer) _false},
{"-j",             "*jumpScroll",	XrmoptionNoArg,  (XtPointer) _true},
{"+j",             "*jumpScroll",       XrmoptionNoArg,  (XtPointer) _false},
{"-kshMode",       "*kshMode",		XrmoptionNoArg,  (XtPointer) _true},
{"+kshMode",       "*kshMode",		XrmoptionNoArg,  (XtPointer) _false},
{"-l",             "*logging",		XrmoptionNoArg,  (XtPointer) _true},
{"+l",             "*logging",		XrmoptionNoArg,  (XtPointer) _false},
{"-lf",            "*logFile",		XrmoptionSepArg, (XtPointer) NULL},
{"-ls",            "*loginShell",	XrmoptionNoArg,  (XtPointer) _true},
{"+ls",            "*loginShell",	XrmoptionNoArg,  (XtPointer) _false},
{"-map",           "*mapOnOutput",	XrmoptionNoArg,  (XtPointer) _true},
{"+map",           "*mapOnOutput",	XrmoptionNoArg,  (XtPointer) _false},
{"-mb",            "*marginBell",	XrmoptionNoArg,  (XtPointer) _true},
{"+mb",            "*marginBell",	XrmoptionNoArg,  (XtPointer) _false},
{"-ms",            "*pointerColor",	XrmoptionSepArg, (XtPointer) NULL},
{"-n",  	   "*iconName",		XrmoptionSepArg, (XtPointer) NULL},
{"-nb",            "*nMarginBell",	XrmoptionSepArg, (XtPointer) NULL},
{"-rw",            "*reverseWrap",	XrmoptionNoArg,  (XtPointer) _true},
{"+rw",            "*reverseWrap",	XrmoptionNoArg,  (XtPointer) _false},
{"-sb",            "*scrollBar",        XrmoptionNoArg,  (XtPointer) _true},
{"+sb",            "*scrollBar",        XrmoptionNoArg,  (XtPointer) _false},
{"-sf",            "*sunFunctionKeys",  XrmoptionNoArg,  (XtPointer) _true},
{"+sf",            "*sunFunctionKeys",  XrmoptionNoArg,  (XtPointer) _false},
{"-si",            "*scrollTtyOutput",  XrmoptionNoArg,  (XtPointer) _true},
{"+si",            "*scrollTtyOutput",  XrmoptionNoArg,  (XtPointer) _false},
{"-sk",            "*scrollKey",        XrmoptionNoArg,  (XtPointer) _true},
{"+sk",            "*scrollKey",        XrmoptionNoArg,  (XtPointer) _false},
{"-sl",            "*saveLines",	XrmoptionSepArg, (XtPointer) NULL},
{"-ti",            "*termId",		XrmoptionSepArg, (XtPointer) NULL},
{"-tm",            "*ttyModes",		XrmoptionSepArg, (XtPointer) NULL},
{"-tn",            "*termName",		XrmoptionSepArg, (XtPointer) NULL},
{"-standAlone",    "*standAlone",	XrmoptionNoArg,	 (XtPointer) _true},
{"-vb",            "*visualBell",	XrmoptionNoArg,  (XtPointer) _true},
{"+vb",            "*visualBell",	XrmoptionNoArg,  (XtPointer) _false},
/* legacy options that are ignored...
 */
{"-ah",            NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+ah",            NULL,		XrmoptionNoArg,  (XtPointer) _false},
{"-b",             NULL,		XrmoptionSepArg, (XtPointer) NULL},
{"-cb",            NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+cb",            NULL,		XrmoptionNoArg,  (XtPointer) _false},
{"-cn",            NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+cn",            NULL,		XrmoptionNoArg,  (XtPointer) _false},
{"-cr",            NULL,		XrmoptionSepArg, (XtPointer) NULL},
{"-s",             NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+s",             NULL,		XrmoptionNoArg,  (XtPointer) _false},
{"-t",             NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+t",             NULL,		XrmoptionNoArg,  (XtPointer) _false},
{"-ut",            NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+ut",            NULL,		XrmoptionNoArg,  (XtPointer) _false},
{"-wf",            NULL,		XrmoptionNoArg,  (XtPointer) _true},
{"+wf",            NULL,		XrmoptionNoArg,  (XtPointer) _false},
};

/* application specific resources...
 */
static XtResource applicationResources[] = {
    {"debugLevel", "DebugLevel", XtRString, sizeof(char *),
	    XtOffsetOf(TERMATTRS, debugString), XtRImmediate, NULL},
    {"ignoredKeysyms", "IgnoredKeysyms", XtRString, sizeof(char *),
	    XtOffsetOf(TERMATTRS, ignoredKeysyms), XtRImmediate, NULL},
    {"loginShell", "LoginShell", XtRBoolean, sizeof(Boolean),
	    XtOffsetOf(TERMATTRS, loginShell), XtRString, "false"},
    {"session", "Session", XtRString, sizeof(char *),
	    XtOffsetOf(TERMATTRS, session), XtRString, NULL},
#ifdef	TERMINAL_SERVER
    {"blocking", "Blocking", XtRBoolean, sizeof(Boolean),
	    XtOffsetOf(TERMATTRS, block), XtRString, "true"},
    {"exitOnLastClose", "ExitOnLastClose", XtRBoolean, sizeof(Boolean),
	    XtOffsetOf(TERMATTRS, exitOnLastClose), XtRString, "false"},
    {"server", "Server", XtRBoolean, sizeof(Boolean),
	    XtOffsetOf(TERMATTRS, server), XtRString, "false"},
    {"serverId", "ServerId", XtRString, sizeof(char *),
	    XtOffsetOf(TERMATTRS, serverId), XtRImmediate, NULL},
    {"standAlone", "StandAlone", XtRBoolean, sizeof(Boolean),
	    XtOffsetOf(TERMATTRS, standAlone), XtRString, "false"},
    {"pingInterval", "PingInterval", XtRInt, sizeof(int),
	    XtOffsetOf(TERMATTRS, PingInterval), XtRString, "5"},
#endif	/* TERMINAL_SERVER */
#ifdef	SUN_TERMINAL_SERVER
    {"server", "Server", XtRBoolean, sizeof(Boolean),
	    XtOffsetOf(TERMATTRS, server), XtRString, "false"},
#endif	/* SUN_TERMINAL_SERVER */
    {XtNtitle, XtCTitle, XtRString, sizeof(char *),
	XtOffsetOf(TERMATTRS, title), XtRString, (caddr_t) NULL},
    {XtNiconName, XtCIconName, XtRString, sizeof(char *),
	XtOffsetOf(TERMATTRS, icon_name), XtRString, (caddr_t) NULL},
    {"name", "Name", XtRString, sizeof(char *),
	XtOffsetOf(TERMATTRS, dtterm_name), XtRString, (caddr_t) NULL},
};

extern String fallbackResources[];

#ifdef sun
void
sunSetupIA(Widget w)
{
    int	i, major_op, first_event, first_err;
    int	*pids;
    void 	(*IAptr)()=(void (*)())NULL;
    DtTermPrimitiveWidget tw;

    if ((IAptr = (void (*)())dlsym(RTLD_NEXT, "XSolarisIASetProcessInfo"))!=NULL) {
        pids = (int *)malloc(sizeof(int) * (InstanceCount + 1));
        pids[0] = getpid();
        for (i = 0; i < InstanceCount; i++) {
	    tw = (DtTermPrimitiveWidget)instance_list[i]->termview.term;
            pids[i + 1] = tw->term.subprocessPid;
        }
        if (XQueryExtension(XtDisplay(w), "SolarisIA",
		        &major_op, &first_event, &first_err))
            (*IAptr)(XtDisplay(w),pids,
	         0x1,InstanceCount + 1);
        free(pids);
    }
}
#endif /* sun */

#ifdef	DELAY_DESTROY
static void
DestroyTree(XtPointer client_data, XtIntervalId *id)
{
    (void) XtDestroyWidget((Widget) client_data);
}
#endif	/* DELAY_DESTROY */

/*ARGSUSED*/
static void
DestroyDtTerm(Widget w, XtPointer client_data, XtPointer call_data)
{
    /* destroy the whole interface... */
    (void) XtDestroyWidget((Widget) client_data);
}

/*ARGSUSED*/
static void
ExitTimeout(XtPointer client_data, XtIntervalId *id)
{
    if (title) {
      XtFree(title);
    }
    (void) exit(0);
}

/*ARGSUSED*/
static void
DecrementInstanceCountAndExit(Widget w, XtPointer client_data,
	XtPointer call_data)
{
    DtTermViewWidget *new_instance_list;
    int i,j;
    
/****************
 *
 * First, update the instance list cache by "deleting" the current instance
 *
 ****************/
    InstanceCount--;
    if (InstanceCount > 0)
    {
        new_instance_list = (DtTermViewWidget *)XtMalloc(sizeof(DtTermViewWidget *)*InstanceCount);
	for (j = 0,i = 0; i < (InstanceCount+1); i++)
	    if (instance_list[i] != (DtTermViewWidget )w)
	    {
		new_instance_list[j] = instance_list[i];
		j++;
	    }
        XtFree((char *)instance_list);
	instance_list = new_instance_list;
    }
    else 
        XtFree((char *)instance_list);
		
    /* check off this instance... */
#ifdef	TERMINAL_SERVER
    if (attrs.standAlone || !attrs.serverId) {
#endif	/* TERMINAL_SERVER */
#ifdef notdef
    if (!attrs.server) {
#endif /* notdef */
	if (InstanceCount <= 0) {
	    /* use a timeout so that the toolkit can finish killing off
	     * this widget...
	     */
	    (void) XtAppAddTimeOut(XtWidgetToApplicationContext(w), 0,
		    ExitTimeout, (XtPointer) 0);
	}
#ifdef	TERMINAL_SERVER
    } else {
	ServerInstanceTerminated((Widget) client_data);
    }
#endif	/* TERMINAL_SERVER */
#ifdef	notdef
    } else {
	/* Here is where one would add the call into sunDtTermServer.c
	 * implement a "lurking" dtterm server. */
    }
#endif	/* notdef */
}

/***************************************************************************
 *									   *
 * RegisterTestProbe - shoves a property on the root window that says	   *
 * we're ready to be tested, and sets up the event handler for the client  *
 * messages.								   *
 *									   *
 ***************************************************************************/
static void RegisterTestProbe(
    Widget		  topLevel,
    DtTermViewWidget	  tw
)
{
    Window win = XtWindow(topLevel);

    if (xa_DTTERM_TEST_PROBE_IDS == None) 
    {
	xa_DTTERM_TEST_PROBE_IDS = XInternAtom(XtDisplay(topLevel),
		"_DTTERM_TEST_PROBE_IDS", False);
    }
    if (xa_DTTERM_REMOTE_PROBE_REQUEST == None) 
    {
	xa_DTTERM_REMOTE_PROBE_REQUEST = XInternAtom(XtDisplay(topLevel),
		"_DTTERM_REMOTE_PROBE_REQUEST", False);
    }
    
    XChangeProperty(XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
                    xa_DTTERM_TEST_PROBE_IDS, xa_DTTERM_TEST_PROBE_IDS,
		    32, PropModeReplace, (unsigned char *)&win, 1);
    XtAddEventHandler(topLevel, NoEventMask , True, (XtEventHandler )TestProbeHandler, 
                      (XtPointer )tw);
}

#define ONE_BYTE 0
#define TWO_BYTE 1
#define FOUR_BYTE 2
#define FOUR_BYTE_PTR 3
#define GET_USER_KEY_LOCK 4
#define GET_AUTO_LINE_FEED 5
#define GET_CURSOR_VISIBLE 6
#define	GET_SCROLLBAR_WINDOW 7
/***************************************************************************
 *									   *
 * TestProbeHandler - handle the client message for test probes		   *
 *									   *
 ***************************************************************************/
static XtEventHandler TestProbeHandler(
 	Widget	w,
	XtPointer	client_data,
	XEvent		*event,
	Boolean		*ctd)
{
    XClientMessageEvent *cm = (XClientMessageEvent *)event;
    DtTermViewWidget tw = (DtTermViewWidget )client_data;
    Window win;
    Atom   prop_name, actual_type;
    int	   prop_type;		/* THIS WILL BREAK ON A 16-BIT INT SYSTEM */
    int    actual_format;
    long   offset, length;
    unsigned long nitems, bytes_left;
    unsigned char *data;
    typedef union {
	char c;
	unsigned char uc;
	int i;
	short s;
	char *cp;
    } alltype;

    alltype fourbyte;
    alltype twobyte;
    alltype onebyte;
    unsigned char  *reply;
    Arg   al[5];
    
    *ctd = True;
    if (cm->send_event == False) return;
    if (cm->message_type != xa_DTTERM_REMOTE_PROBE_REQUEST) return;
    
    reply = (unsigned char*) malloc(sizeof(unsigned char) * 1024);

/****************
 *
 * If it's a xa_DTTERM_REMOTE_PROBE_REQUEST, then the data is three 32-bit
 * quantities: 0 is the window ID, 1 is the property atom to get off the
 * window, and 2 is the number of bytes required to getvalues the requested 
 *  data.
 *
 ****************/

    *ctd = False;
    win = cm->data.l[0];
    prop_name = cm->data.l[1];	
    prop_type = cm->data.l[2];
    offset = 0;
    length = 1000L;
   	
    if (XGetWindowProperty(XtDisplay(w), win, prop_name, offset, length,
    			   False, AnyPropertyType, &actual_type,&actual_format,
			   &nitems, &bytes_left, &data) == Success)
    {
        switch(prop_type) 
	{
            case GET_CURSOR_VISIBLE:
		sprintf((char *)reply,"%d", (int )_DtTermPrimGetCursorVisible(tw->termview.term));
		break;
    
            case GET_USER_KEY_LOCK:
		sprintf((char *)reply,"%d", (int )_DtTermGetUserKeyLock(tw->termview.term));
		break;

            case GET_AUTO_LINE_FEED:
		sprintf((char *)reply,"%d", (int )_DtTermGetAutoLineFeed(tw->termview.term));
		break;

	    case GET_SCROLLBAR_WINDOW:
		sprintf((char *)reply, "0x%lx", tw->termview.scrollBar.widget ?
			XtWindow(tw->termview.scrollBar.widget) : 0);
		break;

	    case ONE_BYTE:
		XtSetArg(al[0], (XtPointer ) data,&onebyte);
	        if (!strcmp((char *)data, XmNmenuBar) ||
		    !strcmp((char *)data, DtNscrollBar))
		    XtGetValues((Widget)tw, al, 1);
		else
		    XtGetValues(tw->termview.term, al, 1);
		sprintf((char *)reply,"%d", onebyte.uc);
		break;

	    case TWO_BYTE:
		XtSetArg(al[0], (XtPointer ) data,&twobyte);
		XtGetValues(tw->termview.term, al, 1);
		sprintf((char *)reply,"%d", twobyte.s);
		break;
		
	    case FOUR_BYTE:
	    case FOUR_BYTE_PTR:
		XtSetArg(al[0], (XtPointer ) data,&fourbyte);
		XtGetValues(tw->termview.term, al, 1);
		if (prop_type == FOUR_BYTE_PTR)
		    if (fourbyte.cp) {
			sprintf((char *)reply,"%s", fourbyte.cp);
		    } else {
			sprintf((char *)reply,"%s", "<NULL>");
		    }
		else 
		    sprintf((char *)reply,"%d", fourbyte.i);
		break;
	     default:
	     	sprintf((char *)reply,"%s","ERROR");
	}
	     
	XChangeProperty(XtDisplay(w), win, prop_name, prop_name, 8,
	                PropModeReplace, reply, strlen((char *)reply));
	XSync(XtDisplay(w),False);
    }
    free(reply);
}

	
static void
AddMenubarToggle
(
    Widget		  topLevel,
    Widget		  termViewWidget
)
{
    Arg al[20];
    int ac;
    char *menu;
    char buffer[BUFSIZ];

    /* intern the necessary atoms... */
    if (xa_MOTIF_WM_MESSAGES == None) {
	/* get the atom value... */
	xa_MOTIF_WM_MESSAGES = XInternAtom(XtDisplay(topLevel),
		"_MOTIF_WM_MESSAGES", False);
    }
    if (xa_DTTERM_TOGGLE_MENUBAR == None) {
	/* get the atom value... */
	xa_DTTERM_TOGGLE_MENUBAR = XInternAtom(XtDisplay(topLevel),
		"_DTTERM_TOGGLE_MENUBAR", False);
    }

    /* add a menubar toggle to the mwm pulldown... */
    ac = 0;
    menu = GETMESSAGE(NL_SETN_Main,1,"\
	Sep             f.separator\n\
	Toggle\\ Menu\\ Bar  f.send_msg %d\
	");
    (void) sprintf(buffer, menu, xa_DTTERM_TOGGLE_MENUBAR);
    (void) XtSetArg(al[ac], XmNmwmMenu, buffer); ac++;
    (void) XtSetValues(topLevel, al, ac);

    /* add a protocoll handler for MOTIF_WM_MESSAGES... */
    if (xa_MOTIF_WM_MESSAGES != None) {
	(void) XmAddProtocols(topLevel, xa_MOTIF_WM_MESSAGES,
		&xa_DTTERM_TOGGLE_MENUBAR, 1);
	(void) XmAddProtocolCallback(topLevel, xa_MOTIF_WM_MESSAGES,
		xa_DTTERM_TOGGLE_MENUBAR,
		ToggleMenuBar, (XtPointer) termViewWidget);
    }
}

static void
CloneCallback(Widget wid, XtPointer client_data, XtPointer call_data)
{
    register DtTermViewWidget w = (DtTermViewWidget) wid;
    DtTermViewCallbackStruct *termCB = (DtTermViewCallbackStruct *) call_data;

    Widget parent = XtParent((Widget) w);
    Widget shellWidget;
    Widget termWidget;
    Arg arglist[10];
    int i;
    /* duplicate the dtterm widget's parent widget...
     */
    if (XtIsShell(parent)) {
	int argc;
	String *argv;
	Boolean allowShellResize;
#ifdef	COPY_WHEN_CLONING
	String geometry;
	Dimension width;
	Dimension height;
#endif	/* COPY_WHEN_CLONING */
	char *topShellIconName;

	i = 0;
	if (XtIsApplicationShell(parent)) {
	    (void) XtSetArg(arglist[i], XmNargc, &argc); i++;
	    (void) XtSetArg(arglist[i], XmNargv, &argv); i++;
	}
#ifdef	COPY_WHEN_CLONING
	(void) XtSetArg(arglist[i], XmNgeometry, &geometry); i++;
	(void) XtSetArg(arglist[i], XmNwidth, &width); i++;
	(void) XtSetArg(arglist[i], XmNheight, &height); i++;
#endif	/* COPY_WHEN_CLONING */
	(void) XtSetArg(arglist[i], XmNallowShellResize, &allowShellResize);
		i++;
	(void) XtGetValues(parent, arglist, i);

	i = 0;
	if (XtIsApplicationShell(parent)) {
	    (void) XtSetArg(arglist[i], XmNargc, argc); i++;
	    (void) XtSetArg(arglist[i], XmNargv, argv); i++;
	}
#ifdef	COPY_WHEN_CLONING
	(void) XtSetArg(arglist[i], XmNgeometry, geometry); i++;
	(void) XtSetArg(arglist[i], XmNwidth, width); i++;
	(void) XtSetArg(arglist[i], XmNheight, height); i++;
#endif	/* COPY_WHEN_CLONING */
	(void) XtSetArg(arglist[i], XmNallowShellResize, allowShellResize); i++;
	/* Set the title (and icon name) for the clone (new) dtterm window 
	   the same as the initial title (and icon name) of the top level 
	   shell dtterm window 
	 */
	(void) XtSetArg(arglist[i], XmNtitle, attrs.title); i++;
	if (attrs.icon_name)
	    topShellIconName = attrs.icon_name;
	else if (attrs.dtterm_name)
	    topShellIconName = attrs.dtterm_name;
	else topShellIconName = attrs.title;
	(void) XtSetArg(arglist[i], XmNiconName, topShellIconName); i++;

	shellWidget = XtAppCreateShell((char *) 0, "Dtterm",
		XtClass(parent), XtDisplay((Widget) w), arglist, i);
    } else {
	i = 0;
	(void) XtSetArg(arglist[i], XmNallowShellResize, True); i++;
	shellWidget = XtAppCreateShell((char *) 0, "Dtterm",
		applicationShellWidgetClass, XtDisplay((Widget) w), arglist, i);
    }

#ifdef	COPY_WHEN_CLONING
    termWidget = DtCreateTermView(shellWidget, "dtTermView", termCB->arglist,
	    termCB->argcount);
#else	/* COPY_WHEN_CLONING */
    termWidget = DtCreateTermView(shellWidget, "dtTermView", termCB->arglist,
	    0);
#endif	/* COPY_WHEN_CLONING */

    /* add a menubar toggle to the mwm pulldown... */
    (void) AddMenubarToggle(shellWidget, termWidget);

    (void) InstanceCount++;
    (void) XtManageChild(termWidget);

    /* destroy the widget on child termination... */
    (void) XtAddCallback(termWidget, DtNsubprocessTerminationCallback,
	    DestroyDtTerm, (XtPointer) shellWidget);
    (void) XtAddCallback(termWidget, XmNdestroyCallback,
	    DecrementInstanceCountAndExit, (XtPointer) topLevel);
    (void) _DtTermPrimAddDeleteWindowCallback(shellWidget, DestroyDtTerm,
	    (XtPointer) shellWidget);
    (void) XtAddCallback(termWidget, DtNnewCallback,
	    CloneCallback, (XtPointer) 0);
    instance_list = (DtTermViewWidget *)XtRealloc((char *)instance_list,
         					(sizeof(DtTermViewWidget *)*InstanceCount));
    instance_list[InstanceCount - 1] = (DtTermViewWidget )termWidget;

    (void) XtRealizeWidget(shellWidget);
    return;
}

static void
ToggleMenuBar
(
    Widget		  w,
    XtPointer		  client_data,
    XtPointer		  call_data
)
{
    XmAnyCallbackStruct	 *cb = (XmAnyCallbackStruct *) call_data;

    if (cb->event->xclient.message_type == xa_MOTIF_WM_MESSAGES) {
	/* can't use case here, since we are not switching on a pre-defined
	 * value...
	 */
	if (cb->event->xclient.data.l[0] == xa_DTTERM_TOGGLE_MENUBAR) {
	    (void) _DtTermViewMenuToggleMenuBar((Widget) client_data);
	}
    }
}
#ifdef SUN_TERMINAL_SERVER
Boolean
#else
static void
#endif /* SUN_TERMINAL_SERVER */
CreateInstance
(
    Widget		  topLevelWidget,
    char		 *name,
    Arg			 *arglist,
    int			  argcnt,
    Boolean		  enableCloneCallback
)
{
    Widget termViewWidget;

    /* bump our session count... */
#ifdef	LOG_USAGE
    (void) LogBumpSessionCount(1);
#endif	/* LOG_USAGE */

    termViewWidget = DtCreateTermView(topLevelWidget, name, arglist, argcnt);

    /* add a menubar toggle to the mwm pulldown... */
    (void) AddMenubarToggle(topLevelWidget, termViewWidget);

#ifdef	TERMINAL_SERVER
    if (attrs.standAlone || !attrs.serverId) {
#endif	/* TERMINAL_SERVER */
	(void) InstanceCount++;
#ifdef	TERMINAL_SERVER
    }
#endif	/* TERMINAL_SERVER */
    (void) XtManageChild(termViewWidget);
    /* destroy the widget on termination... */
    (void) XtAddCallback(termViewWidget, DtNsubprocessTerminationCallback,
	    DestroyDtTerm, (XtPointer) topLevelWidget);
    (void) XtAddCallback(termViewWidget, XmNdestroyCallback,
	    DecrementInstanceCountAndExit, (XtPointer) topLevelWidget);
    (void) _DtTermPrimAddDeleteWindowCallback(topLevelWidget, DestroyDtTerm,
	    (XtPointer) topLevelWidget);
    instance_list = (DtTermViewWidget * )XtRealloc((char *)instance_list,(sizeof(DtTermViewWidget *)*InstanceCount));
    instance_list[InstanceCount - 1] = (DtTermViewWidget )termViewWidget;
    if (enableCloneCallback)
	(void) XtAddCallback(termViewWidget, DtNnewCallback,
		CloneCallback, (XtPointer) 0);
}

void
AtExitProc()
{
    static int i = 0;

    i++;
#ifdef	BBA
    _bA_dump();
#endif	/* BBA */
}

void
SignalHandler
(
    int			  sig
)
{
    DebugF('s', 10, fprintf(stderr, ">>SignalHandler() starting\n"));
    _DtTermPrimPtyCleanup();
    _DtTermPrimUtmpCleanup();
    _DtTermPrimLogFileCleanup();
    DebugF('s', 10, fprintf(stderr, ">>SignalHandler() finished\n"));
    (void) exit(0);
}

static int signalList[] = {
    SIGHUP,
    SIGINT,
    SIGQUIT,
    /* SIGILL, */
    /* SIGTRAP, */
    /* SIGABRT, */
    /* SIGIOT, */
    /* SIGEMT, */
    /* SIGFPE, */
    SIGKILL,
    /* SIGBUS, */
    /* SIGSEGV, */
    /* SIGSYS, */
    SIGPIPE,
    SIGALRM,
    SIGTERM,
    SIGUSR1,
    SIGUSR2,
    /* SIGVTALRM, */
    /* SIGPROF, */
    /* SIGLOST, */
};

static Boolean
GetBindingsProperty
(
    Display		 *display,
    String		  property,
    String		 *binding
)
{
    char		 *prop = (char *) 0;
    Atom		  actualType;
    int			  actualFormat;
    unsigned long	  numItems;
    unsigned long	  bytesAfter;

    (void) XGetWindowProperty(display,
	    RootWindow(display, 0),
	    XmInternAtom(display, property, FALSE),
	    0,
	    (long) 1000000,
	    FALSE,
	    XA_STRING,
	    &actualType,
	    &actualFormat,
	    &numItems,
	    &bytesAfter,
	    (unsigned char **) &prop);

    if ((actualType != XA_STRING) ||
	    (actualFormat != 8) ||
	    (numItems == 0)) {
	if (prop) {
	    XFree(prop);
	}
	return(False);
    } else {
	*binding = prop;
	return(True);
    }
}

#ifndef	XmNdefaultVirtualBindings
#define	XmNdefaultVirtualBindings	"defaultVirtualBindings"
#endif	/* XmNdefaultVirtualBindings */
static void
FixOSFBindings
(
    Display		 *display
)
{
    Widget		  w;
    Arg			  al[10];
    int			  ac;
    char		 *origDisplayBindings,*freeOrigBindings=NULL;
    char		 *newDisplayBindings;
    char		 *c1;
    char		 *c2;
    char		 *cHold;
    int			  i1;
    char		**ignoredKeysymList;
    int			  numIgnoredKeysyms;
    
    /* malloc a copy of the ignoredKeysym string... */
    if (attrs.ignoredKeysyms && *attrs.ignoredKeysyms) {
	c1 = XtMalloc(strlen(attrs.ignoredKeysyms) + 1);
	(void) strcpy(c1, attrs.ignoredKeysyms);
	attrs.ignoredKeysyms = c1;
    }

    /* count the number of keysyms in the list... */
    for (c1 = attrs.ignoredKeysyms, numIgnoredKeysyms = 0; c1 && *c1; ) {
	numIgnoredKeysyms++;
	c1 = strchr(c1, '\n');
	if (c1) {
	    (void) c1++;
	}
    }

    /* malloc space for the array... */
    ignoredKeysymList = (char **) XtMalloc((numIgnoredKeysyms + 1) *
	    sizeof(char *));

    /* build the list... */
    for (c1 = attrs.ignoredKeysyms, i1 = 0; i1 < numIgnoredKeysyms; i1++) {
	ignoredKeysymList[i1] = c1;
	c1 = strchr(c1, '\n');
	if (c1 && *c1) {
	    *c1++ = '\0';
	}
    }

    /* null terminate the list... */
    ignoredKeysymList[i1] = (char *) 0;


    w = XmGetXmDisplay(display);

    ac = 0;
    (void) XtSetArg(al[ac], XmNdefaultVirtualBindings, &origDisplayBindings);
	    ac++;
    (void) XtGetValues(w, al, ac);

    if (!origDisplayBindings) {
	GetBindingsProperty(display, "_MOTIF_BINDINGS", &origDisplayBindings);
        freeOrigBindings = origDisplayBindings ;
    }

    if (!origDisplayBindings) {
	GetBindingsProperty(display, "_MOTIF_DEFAULT_BINDINGS",
		&origDisplayBindings);
        freeOrigBindings = origDisplayBindings ;
    }

    if (!origDisplayBindings) {
        if (ignoredKeysymList) XtFree((char *)ignoredKeysymList);
	return;
    }

    DebugF('b', 10, fprintf(stderr,
	    "FixOSFBindings(): bindings: %s\n", origDisplayBindings));
    newDisplayBindings = XtMalloc(strlen(origDisplayBindings) + 1);
    c1 = origDisplayBindings;
    c2 = newDisplayBindings;
    while (*c1) {
	/* remember the current position in the new string... */
	cHold = c2;

	/* copy over everything up to the '\n' or null... */
	while (*c1 && (*c1 != '\n')) {
	    *c2++ = *c1++;
	}

	/* null terminate the string... */
	*c2 = '\0';

	/* run through the exclusion list and look for a match... */
	for (i1 = 0; i1 < numIgnoredKeysyms; i1++) {
	    DebugF('b', 10, fprintf(stderr, "!strcmp(\"%s\", \"%s\") = %d\n",
		    c2 - strlen(ignoredKeysymList[i1]),
		    ignoredKeysymList[i1],
		    !strcmp(c2 - strlen(ignoredKeysymList[i1]),
		    ignoredKeysymList[i1])));
	    if ((strlen(ignoredKeysymList[i1]) <= (size_t)(c2 - cHold)) &&
		    !strcmp(c2 - strlen(ignoredKeysymList[i1]),
		    ignoredKeysymList[i1])) {
		/* we hit a match... */
		DebugF('b', 10, fprintf(stderr, "match!\n"));
		break;
	    }
	}

	/* if we hit a match, back up the string... */
	if (i1 < numIgnoredKeysyms) {
	    DebugF('b', 10, fprintf(stderr, "dropping %s\n", cHold));
	    c2 = cHold;
	    /* skip over whitespace in the source... */
	    while (isspace(*c1)) {
		c1++;
	    }
	} else {
	    /* copy over whitespace in the source... */
	    DebugF('b', 10, fprintf(stderr, "keeping %s\n", cHold));
	    while (isspace(*c1)) {
		*c2++ = *c1++;
	    }
	    cHold = c2;
	}
    }

    /* null terminate the string... */
    *c2 = '\0';

    /* and set this new bindings string... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNdefaultVirtualBindings, newDisplayBindings);
	    ac++;
    (void) XtSetValues(w, al, ac);

  /* changed from XtFree to free as it was allocated with malloc to avoid FMM */
    if (freeOrigBindings) free(origDisplayBindings);

    if (ignoredKeysymList) XtFree((char *)ignoredKeysymList);

    /* and finally, reparse the string... */
    _XmVirtKeysInitialize(w);
}

static void
removeWmCommandCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XEvent		 *event,
    Boolean		 *cont
)
{
    static Atom		  xa_WM_STATE = None;
    Boolean		  firstTime = True;

    if (firstTime) {
	xa_WM_STATE = XInternAtom(XtDisplay(w), "WM_STATE", True);
	firstTime = False;
    }

    if (((XPropertyEvent *) event)->atom == xa_WM_STATE) {
	/* blow away the atom... */
	(void) XDeleteProperty(XtDisplay(w), XtWindow(w),
		XA_WM_COMMAND);

	/* we have done our job and don't need to be called again... */
	(void) XtRemoveEventHandler(w, (EventMask) XtAllEvents,
		True, removeWmCommandCallback, clientData);
    }
}

static char *
base_name(
     char *name
)
{
	register char *cp;

	cp = strrchr(name, '/');
	return(cp ? cp + 1 : name);
}

int
main(int argc, char **argv)
{
    Arg			  arglist[20];
    int			  i;
    char		 *c;
    char		 *c2;
    char		 *saved_arg0;
    char		**commandToExecute = (char **) 0;
    int			  ptyMasterFd = -1;
    struct sigaction	  sa;
#ifdef SUN_TERMINAL_SERVER
    Boolean 		  iAmTheServer = False;
#endif /* SUN_TERMINAL_SERVER */
    
    (void) atexit(AtExitProc);

    if (c = getenv("dttermDebugFlags")) {
	(void) setDebugFlags(c);
    }
    if (isDebugSet('T')) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) timeStamp("starting");
    }

    if (c = getenv("dttermDelayFlags"))
    {
	if (NULL != c)
	{
	    int delay = atoi(c);
	    pid_t pid = getpid();
	    printf("dtterm (%d) sleeping for %d seconds\n", pid, delay);
	    sleep(delay);
	}
    }

    /* before we do anything, let's set up signal handlers to clean up
     * after ourself...
     */
    (void) sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = SignalHandler;
    /* trap all signals that we want to clean up after... */
    for (i = 0; i < (sizeof(signalList) / sizeof(signalList[0])); i++) {
	(void) sigaction(signalList[i], &sa, (struct sigaction *) 0);
    }

    /* ignore SIGHUP... */
    sa.sa_handler = SIG_IGN;
    (void) sigaction(SIGHUP, &sa, (struct sigaction *) 0);

    /*
    ** set up the environment variables for this client...
    */
#if defined(HPVUE)
#if       (OSMINORVERSION > 01)
    VuEnvControl(VUE_ENV_SET);
#endif /* (OSMINORVERSION > 01) */
#else
    _DtEnvControl(DT_ENV_SET);
#endif   /* HPVUE */

    /* set up the I18N world... */
    (void) XtSetLanguageProc(NULL, NULL, NULL);

    if (isDebugSet('T')) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) timeStamp("XtSetLangProc() finished");
    }

    /* before we initialize Xt, let's save argv and argc... */
    savedArgc = argc;
    savedArgv = (char **) XtMalloc((argc + 1) * sizeof(char *));
    for (i = 0; i < argc; i++) {
	savedArgv[i] = XtMalloc(strlen(argv[i]) + 1);
	(void) strcpy(savedArgv[i], argv[i]);
#ifdef SUN_TERMINAL_SERVER
	if ((!strcmp(argv[i], "-sdtserver")) || (!strcmp(argv[i], "-dtserver")))
	    iAmTheServer = TRUE;
#endif /* SUN_TERMINAL_SERVER */
    }
    savedArgv[i] = (char *) 0;

#ifdef sun
    (void) setsid();
#endif /* sun */


    /* before we do anything with the toolkit, let's usurp the constraint
     * initialize proc for the XmManager widget class.  This will allow us
     * to prevent it from performing that awful
     * DoMagicBBCompatibilityStuff() on us that steals (among other things)
     * our return key.  Note that currently, this is all that the
     * XmManager widget does for widgets, but if it did more than we
     * would loose out on that.
     */
    /* initialize Xt and the X world... */
    (void) DtTermInitialize();

    i = 0;
    (void) XtSetArg(arglist[i], XmNallowShellResize, True); i++;

    topLevel = XtAppInitialize(&appContext,
	    "Dtterm",
	    optionDescList,
	    XtNumber(optionDescList),
	    &argc,
	    argv,
	    fallbackResources,
	    arglist,
	    i);

#ifdef SUN_TERMINAL_SERVER
    if (iAmTheServer) {
       (void) ServerStartSession(topLevel, savedArgc, savedArgv,
                attrs.loginShell,
                commandToExecute);
    } 
#endif /* SUN_TERMINAL_SERVER */

    /* get application specific resources... */
    i = 0;
    /* Need to specify base address of the resources. In our case, attrs */
    (void) XtGetApplicationResources(topLevel, (XtPointer)&attrs, 
				     applicationResources,
				     XtNumber(applicationResources), 
				     arglist, i);    

    /* set all the debug flags... */
    (void) setDebugFlags(attrs.debugString);
    
    (void) FixOSFBindings(XtDisplay(topLevel));

    /* Add the Save_yourself callback... */
    xa_WM_SAVE_YOURSELF = XInternAtom(XtDisplay(topLevel),
                                      "WM_SAVE_YOURSELF", False);
    XmAddWMProtocolCallback(topLevel, xa_WM_SAVE_YOURSELF,
                           (XtCallbackProc)SaveSessionCB, (XtPointer)NULL);
    
    i = 0;
    (void) XtSetArg(arglist[i], XmNmappedWhenManaged, False); i++;
    (void) XtSetArg(arglist[i], XmNallowShellResize, True); i++;
    topShell = XtAppCreateShell((char *) 0, "Dtterm",
		applicationShellWidgetClass, XtDisplay(topLevel), arglist, i);

    if (attrs.session != NULL)
       RestoringTerms = TRUE;

    if (isDebugSet('T')) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) timeStamp("XtAppInitialize() finished");
    }

    for (i = 1; i < argc; i++) {
	if (argv[i][0] != '-') {
	    (void) Syntax(argv[0], argv[i]);
	    exit(1);
	}

	switch(argv[i][1]) {
	case 'S' :
	    /* slave mode...
	     */
	    /* skip past the '-S'... */
	    c = argv[i] + 2;
	    if (c2 = strchr(c, '.')) {
		/* skip past the '.'... */
		(void) c2++;
	    } else {
		/* skip past the first 2 characters since they are the
		 * name of the pty...
		 */
		c2 = c + 2;
	    }
	    /* use scanf instead of atoi since it won't change the value
	     * of ptyMasterFd if the string does not contain a valid
	     * number...
	     */
	    (void) sscanf(c2, "%d", &ptyMasterFd);
	    break;

	case 'e' :
	    /* command to exec...
	     */
	    (void) i++;
	    if (i < argc) {
		commandToExecute = &argv[i];
		i = argc;
	    }
	    break;
	case 'h' :
	case 'u' :
	    /* help/usage...
	     */
	    (void) Help(argv[0]);
	    exit(0);
	    break;

	default :
	    (void) Syntax(argv[0], argv[i]);
	    (void) exit(1);
	    break;
	}
    }

#ifdef	TERMINAL_SERVER
    if (attrs.standAlone || !attrs.serverId) {
#endif	/* TERMINAL_SERVER */
	/* log our startup... */
#ifdef	LOG_USAGE
	(void) LogStart(0, argc, argv);
#endif	/* LOG_USAGE */

	i = 0;
	if (commandToExecute) {	    
	    (void) XtSetArg(arglist[i], DtNsubprocessArgv, commandToExecute);
		    i++;
	}
        
        if (!RestoringTerms || !RestoreSession(topShell, arglist, i)) {

	    RestoringTerms = False;

	    if (ptyMasterFd >= 0) {
		/* set the workspace for the initial dtterm instance... */
		(void) XtSetArg(arglist[i], DtNtermDevice, ptyMasterFd); i++;
		(void) XtSetArg(arglist[i], DtNtermDeviceAllocate, False); i++;
		(void) XtSetArg(arglist[i], DtNsubprocessExec, False); i++;
		(void) CreateInstance(topShell, "dtTermView", arglist, i, False);
	    } 
	    else {
		(void) CreateInstance(topShell, "dtTermView", arglist, i, True);
	    }
	}

	/* realize the interface... */
	i = 0;
	(void) XtSetArg(arglist[i], XmNmappedWhenManaged, False); i++;
	(void) XtSetValues(topLevel, arglist, i);
	(void) XtRealizeWidget(topLevel);

#ifdef SUN_TERMINAL_SERVER
	if (iAmTheServer)
	    FinishToolTalkInit(topLevel);
#endif /* SUN_TERMINAL_SERVER */

	/* Set title to the command to execute (if any) 
	 */
	if (commandToExecute && !attrs.title) {
	   attrs.title = base_name (commandToExecute[0]);
	}

	/* Else set title to default ("Terminal")
	 */
	if (!attrs.title) {	    
	    title = XtMalloc(strlen(GETMESSAGE(NL_SETN_Main, 3, "Terminal")) + 1);
	    (void) strcpy(title, GETMESSAGE(NL_SETN_Main, 3, "Terminal"));
	    attrs.title = title;
	}

	/* realize the interface... */
	i = 0;
        (void) XtSetArg(arglist[i], XmNtitle, attrs.title); i++;
	if ((!attrs.icon_name) && (!attrs.dtterm_name))
	  (void) XtSetArg(arglist[i], XmNiconName, attrs.title); i++;
	(void) XtSetArg(arglist[i], XmNmappedWhenManaged, False); i++;
	(void) XtSetArg(arglist[i], XmNgeometry, NULL); i++;
	(void) XtSetValues(topShell, arglist, i);
	(void) XtRealizeWidget(topShell);

        if (RestoringTerms != True) {
	    /* use the wm command to pass the -xrm resource to the wm... */
	    (void) XSetCommand(XtDisplay(topShell), XtWindow(topShell),
		    savedArgv, savedArgc);
	    /* add a callback to remove it after it has been seen... */
	    (void) XtAddEventHandler(topShell, (EventMask) PropertyChangeMask,
		    False, removeWmCommandCallback, (XtPointer) 0);
	}

        (void) XtSetArg(arglist[0], XmNmappedWhenManaged, True); 
        (void) XtSetValues(topShell, arglist, 1);
	XtMapWidget(topShell);
        RegisterTestProbe(topShell,instance_list[0]);
	
	if (ptyMasterFd >= 0) {
	    /* write out the window id... */
	    char buffer[BUFSIZ];

	    (void) sprintf(buffer, "%lx\n", XtWindow(topLevel));
	    (void) write(ptyMasterFd, buffer, strlen(buffer));
	}
#ifdef sun
	sunSetupIA(topLevel);
#endif /* sun */

#ifdef	TERMINAL_SERVER
    } else {
	if (ptyMasterFd >= 0) {
	    (void) fprintf(stderr,
		    GETMESSAGE(NL_SETN_Main,2, "%s: the -S option cannot be used with the -server or -serverid options\n"),
		    argv[0]);
	    (void) exit(1);
	}

	i = 0;
	(void) XtSetArg(arglist[i], XmNmappedWhenManaged, False); i++;
	(void) XtSetValues(topLevel, arglist, i);
	(void) XtRealizeWidget(topLevel);

	(void) ServerStartSession(topLevel, savedArgc, savedArgv,
		attrs.server, attrs.serverId, attrs.exitOnLastClose, 
		attrs.block, attrs.loginShell,
		commandToExecute);
    }
#endif	/* TERMINAL_SERVER */

    /* and hand off to Xt... */
    (void) XtAppMainLoop(appContext);
}

/************************************************************************
 *
 *  SaveSessionCB 
 *
 ************************************************************************/

/* ARGSUSED */
static void 
SaveSessionCB(
        Widget w,			/* widget id */
        caddr_t client_data,		/* data from application  */
        caddr_t call_data )		/* data from widget class */
{
    char *longpath, *fileName;
    char bufr[1024];
    int fd;
    char *xa_CommandStr[1024];
    int i,j;
    
     if(!XtIsShell(w))
	w = XtParent(w);

    DtSessionSavePath(w, &longpath, &fileName);

    /*  Create the session file  */
    if ((fd = creat (longpath, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)) == -1)
    {
        return;
    }

    sprintf(bufr, "*dtterm.numClonedTerms: %d\n", InstanceCount);
    write (fd, bufr, strlen(bufr));

    for(i = 0; i < InstanceCount; i++)
        SaveTerm(instance_list[i], i, fd);

    sprintf(bufr,
          "*dtterm.sessionFileVersionId: dttermSessionFileVersion.1.0.0\n");
    write (fd, bufr, strlen(bufr));

    close(fd);

    i = 0;
    xa_CommandStr[i] = savedArgv[0]; i++;
    xa_CommandStr[i] =  "-session"; i++;
    xa_CommandStr[i] =  fileName; i++;
/****************
 *
 * Now restore the command line minus the old session parameter 
 *
 ****************/
    for (j = 1; j < savedArgc; j++)
    {
        if (strcmp(savedArgv[j], "-session") == 0)
	{
	    j++;
	    continue;
	}
        else
	{
	    xa_CommandStr[i] =  savedArgv[j];
	    i++;
	}
    }

    XSetCommand(XtDisplay(topLevel), 
		XtWindow(topLevel), xa_CommandStr, i);


    XtFree ((char *)fileName);
}

static void 
SaveTerm(
        DtTermViewWidget dtvw,
        int cloneNum,
        int fd)
{
    char *bufr = malloc(1024);
    Position x,y;
    Dimension width, height;
    Atom *pWsPresence;
    unsigned long numInfo;
    unsigned char	  charCursorStyle;
    int			  blinkRate;
    Boolean		  jumpScroll;
    Boolean		  marginBell;
    int			  nMarginBell;
    Boolean		  visualBell;
    Boolean		  reverseVideo;
    Boolean		  cursorMode;
    Boolean		  keypadMode;
    Boolean		  autoWrap;
    Boolean		  reverseWrap;
    Boolean		  c132;
    Boolean		  lockState;
    Boolean		  autoLineFeed;
    Boolean		  menuBar;
    Boolean		  scrollBar;
    short		  rows;
    short		  columns;
    char		 *cwd;
    static char		 *defaultCwd = (char *) 0;
    Arg			  al[30];
    int			  ac;
    char		  buffer[BUFSIZ];
    DtTermWidget dtw = (DtTermWidget )dtvw->termview.term;
    static Boolean	  firstTime = True;
    static Atom		  xa_WM_STATE = None;
    struct _wmStateStruct {
	CARD32		  state;
	Window		  icon;
    }			 *prop;
    Atom		  actualType;
    int			  actualFormat;
    unsigned long	  nItems;
    unsigned long	  bytesAfter;

    XmVendorShellExtObject vendorExt;
    XmWidgetExtData        extData;

    x = XtX(XtParent(dtvw));
    y = XtY(XtParent(dtvw));

    /*
     * Modify x & y to take into account window mgr frames
     * This is pretty bogus, but I don't know a better way to do it.
     */
    extData = _XmGetWidgetExtData(XtParent(dtvw), XmSHELL_EXTENSION);
    vendorExt = (XmVendorShellExtObject)extData->widget;
    x -= vendorExt->vendor.xOffset;
    y -= vendorExt->vendor.yOffset;
    width = XtWidth(XtParent(dtvw));
    height = XtHeight(XtParent(dtvw));

    sprintf(bufr, "*dtterm_%d.x: %d\n", cloneNum, x);
    sprintf(bufr, "%s*dtterm_%d.y: %d\n", bufr, cloneNum, y);
    sprintf(bufr, "%s*dtterm_%d.width: %d\n", bufr, cloneNum, width);
    sprintf(bufr, "%s*dtterm_%d.height: %d\n", bufr, cloneNum, height);

    /* Write out iconic state...
     */
    if (firstTime) {
	xa_WM_STATE = XInternAtom(XtDisplay(dtvw), "WM_STATE", True);
	firstTime = False;
    }
    if (xa_WM_STATE && (Success == XGetWindowProperty(
	    XtDisplay(dtvw),
	    XtWindow(XtParent(dtvw)),
	    xa_WM_STATE,
	    0L,
	    (sizeof(struct _wmStateStruct) + 3) / 4,
	    False,
	    AnyPropertyType,
	    &actualType,
	    &actualFormat,
	    &nItems,
	    &bytesAfter,
	    (unsigned char **) &prop))) {
	if (prop->state == IconicState) {
	    sprintf(bufr, "%s*dtterm_%d.iconify: %s\n", bufr, cloneNum,
		    "True");
	} else {
	    sprintf(bufr, "%s*dtterm_%d.iconify: %s\n", bufr, cloneNum,
		    "False");
	}
    }
	
    if(DtWsmGetWorkspacesOccupied(XtDisplay(dtvw), 
				  XtWindow(XtParent(dtvw)), &pWsPresence,
				  &numInfo) == Success)
    {
	int i;
	sprintf(bufr, "%s*dtterm_%d.workspaceList: ", bufr, cloneNum);
	for(i = 0; i < numInfo; i++)
	{
	    char *name =  XGetAtomName(XtDisplay(dtvw),
				       pWsPresence[i]);
	    sprintf(bufr, "%s %s", bufr, name);
	    XtFree(name);
	}
	sprintf(bufr, "%s\n", bufr);
	XtFree((char *)pWsPresence);
    }

    write (fd, bufr, strlen(bufr));

    sprintf(bufr, "*dtterm_%d.userFontListIndex: %d\n", cloneNum,
	    _DtTermViewGetUserFontListIndex((Widget )dtvw));
    write (fd, bufr, strlen(bufr));

    /* get the current widget values... */
    ac = 0;
    (void) XtSetArg(al[ac], DtNmenuBar, &menuBar);			ac++;
    (void) XtSetArg(al[ac], DtNscrollBar, &scrollBar);			ac++;
    (void) XtGetValues((Widget)dtvw, al, ac);
    ac = 0;
    (void) XtSetArg(al[ac], DtNrows, &rows);				ac++;
    (void) XtSetArg(al[ac], DtNcolumns, &columns);			ac++;
    (void) XtSetArg(al[ac], DtNblinkRate, &blinkRate);			ac++;
    (void) XtSetArg(al[ac], DtNcharCursorStyle, &charCursorStyle);	ac++;
    (void) XtSetArg(al[ac], DtNblinkRate, &blinkRate);			ac++;
    (void) XtSetArg(al[ac], DtNjumpScroll, &jumpScroll);		ac++;
    (void) XtSetArg(al[ac], DtNmarginBell, &marginBell);		ac++;
    (void) XtSetArg(al[ac], DtNnMarginBell, &nMarginBell);		ac++;
    (void) XtSetArg(al[ac], DtNvisualBell, &visualBell);		ac++;
    (void) XtSetArg(al[ac], DtNreverseVideo, &reverseVideo);		ac++;
    (void) XtSetArg(al[ac], DtNappCursorDefault, &cursorMode);		ac++;
    (void) XtSetArg(al[ac], DtNappKeypadDefault, &keypadMode);		ac++;
    (void) XtSetArg(al[ac], DtNautoWrap, &autoWrap);			ac++;
    (void) XtSetArg(al[ac], DtNreverseWrap, &reverseWrap);		ac++;
    (void) XtSetArg(al[ac], DtNc132, &c132);				ac++;
    (void) XtGetValues((Widget)dtw, al, ac);
    lockState = _DtTermGetUserKeyLock((Widget) dtw);
    autoLineFeed = _DtTermGetAutoLineFeed((Widget) dtw);

    /* get the current working directory if it was saved... */
    cwd = _DtTermGetCurrentWorkingDirectory((Widget) dtw);
    if (!cwd || !*cwd) {
	/* no saved cwd, use the one we were started from... */
	if (!defaultCwd) {
	    /* first time through, use getcwd() to get it... */
	    if (getcwd(buffer, sizeof(buffer))) {
		defaultCwd = XtMalloc(strlen(buffer) + 1);
		(void) strcpy(defaultCwd, buffer);
	    }
	}
	cwd = defaultCwd;
    }

    (void) sprintf(bufr, "*dtterm_%d*rows: %d\n", cloneNum, rows);
    (void) write(fd, bufr, strlen(bufr));
    (void) sprintf(bufr, "*dtterm_%d*columns: %d\n", cloneNum, columns);
    (void) write(fd, bufr, strlen(bufr));

    if(menuBar == False)
    {
	sprintf(bufr, "*dtterm_%d*menuBar: False\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(scrollBar == False)
    {
	sprintf(bufr, "*dtterm_%d*scrollBar: False\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if (_DtTermPrimGetCursorVisible((Widget )dtw))
    {
	if (charCursorStyle == DtTERM_CHAR_CURSOR_BAR)
   	    sprintf(bufr, "*dtterm_%d*charCursorStyle: CHARCURSOR_BAR\n", cloneNum);
	else
	    sprintf(bufr, "*dtterm_%d*charCursorStyle: CHARCURSOR_BOX\n", cloneNum);
    }
    else 
        sprintf(bufr, "*dtterm_%d*charCursorStyle: CHARCURSOR_INVISIBLE\n", cloneNum);
    write (fd, bufr, strlen(bufr));
    

    sprintf(bufr, "*dtterm_%d*blinkRate: %d\n", cloneNum,blinkRate);
    write (fd, bufr, strlen(bufr));

    if(jumpScroll == False)
    {
	sprintf(bufr, "*dtterm_%d*jumpScroll: False\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(marginBell == True)
    {
	sprintf(bufr, "*dtterm_%d*marginBell: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
	sprintf(bufr, "*dtterm_%d*nMarginBell: %d\n", cloneNum,nMarginBell);
        write (fd, bufr, strlen(bufr));
    }
    if(visualBell == True)
    {
	sprintf(bufr, "*dtterm_%d*visualBell: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(reverseVideo == True)
    {
	sprintf(bufr, "*dtterm_%d*reverseVideo: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(cursorMode == True)
    {
	sprintf(bufr, "*dtterm_%d*appCursorDefault: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(keypadMode == True)
    {
	sprintf(bufr, "*dtterm_%d*appKeypadDefault: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(autoWrap == False)
    {
	sprintf(bufr, "*dtterm_%d*autoWrap: False\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(reverseWrap == True)
    {
	sprintf(bufr, "*dtterm_%d*reverseWrap: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(c132 == True)
    {
	sprintf(bufr, "*dtterm_%d*c132: True\n", cloneNum);
        write (fd, bufr, strlen(bufr));
    }
    if(lockState == False)
	sprintf(bufr, "*dtterm_%d*userKeyLock: False\n", cloneNum);
    else
    	sprintf(bufr, "*dtterm_%d*userKeyLock: True\n", cloneNum);
    write (fd, bufr, strlen(bufr));
    if(autoLineFeed == False)
	sprintf(bufr, "*dtterm_%d*autoLineFeed: False\n", cloneNum);
    else
    	sprintf(bufr, "*dtterm_%d*autoLineFeed: True\n", cloneNum);
    write (fd, bufr, strlen(bufr));

    if (cwd && *cwd) {
	(void) sprintf(bufr, "*dtterm_%d*currentWorkingDirectory: %s\n",
		cloneNum, cwd);
	(void) write(fd, bufr, strlen(bufr));
    }
    free(bufr);
}

/***********************************************************************
 *
 * RestoreSession - returns True for success, False for failure...
 *
 ***********************************************************************/
static Boolean
RestoreSession(
            Widget topDog,
	    Arg *passedAl,
	    int passedAc)
{
    char *fileName = attrs.session;
    char *path;
    XrmDatabase db;
    char *tmpStr;
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;
    Widget ShellWidget;
    int numToRestore, i, j;
    Arg			  al[30];
    int			  ac;
    
    if(DtSessionRestorePath(topLevel, &path, fileName) == False)
	path = fileName;

    /*  Open the file as a resource database */
    if ((db = XrmGetFileDatabase (path)) == NULL) 
    {
      /* if we got a bogus filename, bail out... */
      return(False);
    }

    xrm_name[0] = XrmStringToQuark ("dtterm");
    xrm_name[1] = XrmStringToQuark ("numClonedTerms");
    xrm_name[2] = 0;
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);

    /* if we got nothing back, bail out... */
    if (!value.addr || !*((char *) value.addr)) {
	return(False);
    }

    numToRestore = atoi((char *)value.addr);
    /* if we don't have at least one to restore, bail out... */
    if (numToRestore < 1) {
	return(False);
    }

    ShellWidget = topDog;
        
    for (j = 0; j < numToRestore; j++)
    {
	/* use the passed arglist as a starting point... */
	for (ac = 0; ac < passedAc; ac++) {
	    al[ac].value = passedAl[ac].value;
	    al[ac].name = passedAl[ac].name;
	}

	(void) CreateInstance(ShellWidget, "dtTermView", al, ac, True);
        RestoreTerm(ShellWidget, instance_list[j], j, db);
        (void) XtSetArg(al[0], XmNmappedWhenManaged, True); 
        (void) XtSetValues(ShellWidget, al, 1);
	/* mapping the shell widget here is redundant, and it turns out
	 * was responsible for CMVC7216: broken restoring of
	 * iconified instances in multiple workspaces...
	 */

        ac = 0;
	(void) XtSetArg(al[ac], XmNallowShellResize, True); ac++;
        (void) XtSetArg(al[ac], XmNmappedWhenManaged, True); ac++;
	ShellWidget = XtAppCreateShell((char *) 0, "Dtterm",
		    applicationShellWidgetClass, XtDisplay(topDog), al, ac);
    }
    /* success, return True... */
    return(True);
}

static void
RestoreTerm(
	Widget shellWidget,
        DtTermViewWidget dtvw,
	int termNum,
        XrmDatabase db)
{
    char buf[1024];
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;
    DtTermWidget dtw = (DtTermWidget )dtvw->termview.term;
    Position x,y;
    Dimension width, height;
    char                  *ws_list;
    unsigned char	  charCursorStyle;
    int			  blinkRate;
    Boolean		  jumpScroll;
    Boolean		  marginBell;
    int			  nMarginBell;
    Boolean		  visualBell;
    Boolean		  reverseVideo;
    Boolean		  cursorMode;
    Boolean		  keypadMode;
    Boolean		  autoWrap;
    Boolean		  reverseWrap;
    Boolean		  c132;
    Boolean		  lockState;
    Boolean		  autoLineFeed;
    Boolean		  menuBar;
    Boolean		  scrollBar;
    char		 *cwd;
    Arg			  al[30];
    int			  ac;
    int			  rows = 0;
    int			  columns = 0;

    sprintf(buf, "dtterm_%d", termNum);
    xrm_name[0] = XrmStringToQuark(buf);
    xrm_name[2] = 0;

/* Pass 1, before widget realization...
 */

    /* get x position */
    xrm_name[1] = XrmStringToQuark ("x");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    x = atoi((char *)value.addr);

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    y = atoi((char *)value.addr);

    /* get width */
    xrm_name [1] = XrmStringToQuark ("width");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    width = atoi((char *)value.addr);

    /* get height */
    xrm_name [1] = XrmStringToQuark ("height");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    height = atoi((char *)value.addr);

/****************
 *
 * First the shell resources...
 *
 ****************/
    ac = 0;
    (void) XtSetArg(al[ac], XmNx, x);			ac++;
    (void) XtSetArg(al[ac], XmNy, y);			ac++;
    (void) XtSetArg(al[ac], XmNwidth, width);			ac++;
    (void) XtSetArg(al[ac], XmNheight, height);			ac++;

    /* get iconify... */
    xrm_name[1] = XrmStringToQuark("iconify");
    if(XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value)) {
	if (!strcmp((char *)value.addr, "True")) {
	    (void) XtSetArg(al[ac], XmNinitialState, IconicState); ac++;
	} else {
	    (void) XtSetArg(al[ac], XmNinitialState, NormalState); ac++;
	}
    } else {
	(void) XtSetArg(al[ac], XmNinitialState, NormalState);
    }

    (void) XtSetValues(XtParent((Widget)dtvw), al, ac);

/****************
 *
 * Now the TermView resources...
 *
 ****************/
    ac = 0;
    xrm_name[1] = XrmStringToQuark ("rows");
    if(XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value)) {
	rows = atoi((char *)value.addr);
	(void) XtSetArg(al[ac], DtNrows, rows); ac++;
    }

    xrm_name[1] = XrmStringToQuark ("columns");
    if(XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value)) {
	columns = atoi((char *)value.addr);
	(void) XtSetArg(al[ac], DtNcolumns, columns); ac++;
    }

    xrm_name[1] = XrmStringToQuark ("menuBar");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], XmNmenuBar, False);	ac++;}
    xrm_name[1] = XrmStringToQuark ("scrollBar");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNscrollBar, False);	ac++;}

    xrm_name[1] = XrmStringToQuark ("userFontListIndex");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True) {
	_DtTermViewSetUserFontListIndex((Widget )dtvw, atol(value.addr));
    }

    (void) XtSetValues((Widget)dtvw, al, ac);

/****************
 *
 * And finally the termprim stuff...
 *
 ****************/
    ac = 0;
    if(rows > 0) {
	(void) XtSetArg(al[ac], DtNrows, rows); ac++;
    }

    if(columns > 0) {
	(void) XtSetArg(al[ac], DtNcolumns, columns); ac++;
    }

    xrm_name[1] = XrmStringToQuark ("charCursorStyle");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    if (!strcmp(value.addr, "CHARCURSOR_INVISIBLE"))
        _DtTermPrimSetCursorVisible((Widget )dtw, False);
    else  
    {
    	if (!strcmp(value.addr, "CHARCURSOR_BAR"))
              (void) XtSetArg(al[ac], DtNcharCursorStyle, DtTERM_CHAR_CURSOR_BAR);
        else 
              (void) XtSetArg(al[ac], DtNcharCursorStyle, DtTERM_CHAR_CURSOR_BOX);
        ac++;
    }
    
    xrm_name [1] = XrmStringToQuark ("blinkRate");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    blinkRate = atoi((char *)value.addr);
    (void) XtSetArg(al[ac], DtNblinkRate, blinkRate);			ac++;

    xrm_name[1] = XrmStringToQuark ("jumpScroll");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNjumpScroll, False);	ac++;}

    xrm_name[1] = XrmStringToQuark ("marginBell");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
    {
    	(void) XtSetArg(al[ac], DtNmarginBell, True);	ac++;
         xrm_name [1] = XrmStringToQuark ("nMarginBell");
         XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
         nMarginBell = atoi((char *)value.addr);
         (void) XtSetArg(al[ac], DtNnMarginBell, nMarginBell);ac++;
    }

    xrm_name[1] = XrmStringToQuark ("visualBell");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNvisualBell, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("reverseVideo");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNreverseVideo, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("appCursorDefault");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNappCursorDefault, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("appKeypadDefault");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNappKeypadDefault, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("autoWrap");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNautoWrap, False);	ac++;}

    xrm_name[1] = XrmStringToQuark ("reverseWrap");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNreverseWrap, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("c132");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNc132, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("reverseVideo");
    if(XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value) == True)
        {(void) XtSetArg(al[ac], DtNreverseVideo, True);	ac++;}

    xrm_name[1] = XrmStringToQuark ("userKeyLock");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    if (!strcmp(value.addr, "True"))
         _DtTermSetUserKeyLock((Widget) dtw, True);
    else 
         _DtTermSetUserKeyLock((Widget) dtw, False);

    xrm_name[1] = XrmStringToQuark ("autoLineFeed");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    if (!strcmp(value.addr, "True"))
         _DtTermSetAutoLineFeed((Widget) dtw, True);
    else 
         _DtTermSetAutoLineFeed((Widget) dtw, False);

    (void) XtSetValues((Widget)dtw, al, ac);

    xrm_name[1] = XrmStringToQuark ("currentWorkingDirectory");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    if (value.addr && *value.addr) {
	_DtTermSetCurrentWorkingDirectory((Widget) dtw, value.addr);
    }

    /* realize the interface...
     */
    (void) XtRealizeWidget(shellWidget);



/* Pass 2, after widget realization...
 */

    /* get workspace list */
    xrm_name[1] = XrmStringToQuark("workspaceList");
    XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value);
    SetWorkSpaceHints( XtParent((Widget)dtvw),(char *) value.addr);
}

static void
SetWorkSpaceHints
(
    Widget shell,
    char *workspaces
)
{
    char * ptr;
    Atom * workspace_atoms = NULL;
    int num_workspaces=0;
    Atom thisWsAtom;

    if (workspaces) {
	do {
	    ptr = strchr (workspaces, ' ');
	    if (ptr != NULL)
		*ptr = 0;
	    thisWsAtom = None;

	    /* if we haven't hit a match, try interning the atom... */
	    if (thisWsAtom == None) {
		thisWsAtom = XmInternAtom (XtDisplay(shell), workspaces, True);
	    }

	    if (thisWsAtom != None) {
		workspace_atoms = (Atom *) XtRealloc ((char *)workspace_atoms,
			sizeof (Atom) * (num_workspaces + 1));
		workspace_atoms[num_workspaces] = thisWsAtom;
		num_workspaces++;
	    }

	    if (ptr != NULL) {
		*ptr = ' ';
		workspaces = ptr + 1;
	    }
	} while (ptr != NULL);

	if (num_workspaces > 0) {
	    DtWsmSetWorkspacesOccupied(XtDisplay(shell), XtWindow (shell),
		    workspace_atoms, num_workspaces);
	}

	XtFree ((char *) workspace_atoms);
	workspace_atoms = NULL;
    }
}
