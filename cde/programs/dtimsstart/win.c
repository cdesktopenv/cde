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
/* @(#)$TOG: win.c /main/9 1997/06/18 17:33:01 samborn $ */

#include	<X11/X.h>
#include	<X11/Xlib.h>
#include	<X11/Xatom.h>
#include	<X11/Intrinsic.h>
#include	<X11/cursorfont.h>
#include	<Xm/Xm.h>
#include	<Xm/Protocols.h>
#include	<Xm/MwmUtil.h>

#include	<Xm/Form.h>
#include	<Xm/RowColumn.h>
#include	<Xm/LabelG.h>
#include	<Xm/SeparatoG.h>
#include	<Xm/DialogS.h>
#include	<Xm/MessageB.h>
#include	<Xm/TextF.h>
#include	<Xm/Text.h>

#include	"xims.h"
#include	<limits.h>


Display			*Dpy = (Display *) 0;
Widget			TopW = (Widget) 0;
XtAppContext		appC = (XtAppContext) 0;

static Widget		SelW = (Widget) 0;
static Widget		ModeW = (Widget) 0;
static Widget		MsgW = (Widget) 0;
static Widget		HelpW = (Widget) 0;
static Widget		HostW = (Widget) 0;
static Widget		HostText = (Widget) 0;

static Widget		SelRC = (Widget) 0;
static Widget		HostRC = (Widget) 0;

static Widget		FocusW = (Widget) 0;

static int		(*defaultErrorHandler)() = 0;


/* application resource */
typedef struct {
    String	selectionHelpMsg;
    String	modeHelpMsg;
    String	windowLocation;
} AppResRec, *AppResP;

static AppResRec	appres;

static XtResource app_resources[] = {
    { "selectionHelpMsg", "SelectionHelpMsg", XtRString, sizeof(String),
      XtOffset(AppResP, selectionHelpMsg), XtRString, (XtPointer)NULL },
    { "modeHelpMsg", "ModeHelpMsg", XtRString, sizeof(String),
      XtOffset(AppResP, modeHelpMsg), XtRString, (XtPointer)NULL },
    { "windowLocation", "WindowLocation", XtRString, sizeof(String),
      XtOffset(AppResP, windowLocation), XtRString, (XtPointer)NULL },
};

static XrmOptionDescRec	options[] = {
    { "-geometry", "*XmDialogShell.geometry", XrmoptionSepArg, (XPointer) NULL },
    { "-fn", "*fontList", XrmoptionSepArg, (XPointer) NULL },
    { "-font", "*fontList", XrmoptionSepArg, (XPointer) NULL },

};

static char	*fallbacks[] = {
    NULL
};


#define	OK_BTN			0
#define	CANCEL_BTN		1
#define	CLEAR_BTN		2
#define	HOST_BTN		3
#define	HELP_BTN		4

#ifdef	DEBUG2
#define	WIN_FUNC	(MWM_FUNC_RESIZE | MWM_FUNC_MOVE)
#define	WIN_DECR	(MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_RESIZEH)
#else
#define	WIN_FUNC	(MWM_FUNC_MOVE)
#define	WIN_DECR	(MWM_DECOR_BORDER)
#endif

#define	MSGWIN_FUNC	0
#define	MSGWIN_DECR	(MWM_DECOR_BORDER)
/* #define	DIALOG_STYLE		XmDIALOG_MODELESS */
#define	DIALOG_STYLE		XmDIALOG_PRIMARY_APPLICATION_MODAL

#define	MAX_HOSTNAME_LEN	63	/* limitation of ARPA host name */
#define	TEXT_COLUMNS		(MAX_HOSTNAME_LEN / 4)

    /* beep */
#ifdef	DEBUG
#define	Beep()		XBell(Dpy, 0); DPR(("Beep!\n"))
#else
#define	Beep()		XBell(Dpy, 0)
#endif	/* DEBUG */


	/* local functions */
    /* window env */
static int	ignoreBadWindow(/* dpy, error */);
static void	finish_window(/* w, end_window */);
static int	own_main_atom(/* win */);
static int	disown_main_atom(/* win */);
    /* selection window */
static void	free_ims_list(/* list */);
static void	clear_selection_var(/*  */);
static void	finish_selection_window(/* end_window */);
static void	done_cb(/* w, client_data, call_data */);
static void	select_cb(/* w, client_data, call_data */);
static void	host_btn_cb(/* w, client_data, call_data */);
static void	help_cb(/* w, client_data, call_data */);
static int	create_selection_window(/*  */);
static int	change_host(/* new_host, host_type */);
static void	add_btn_trans(/* btn */);
static void	change_ims_list(/* last_ims_name, init_idx */);
static void	set_host_label(/* host_type, hostname */);
static void	add_cmd_btn(/* parent_rc, cb_ok, cb_clear, cb_cancel, cb_host, cb_help */);
    /* host window */
static void	start_host_window(/* cur_host */);
static void	finish_host_window(/*  */);
static void	host_done_cb(/* w, client_data, call_data */);
static void	host_clear_cb(/* w, client_data, call_data */);
static void	host_done_action(/* w, ev, args, num_args */);
static void	create_host_window(/* cur_host */);
    /* mode window */
static void	finish_mode_window(/* end_window */);
static void	mode_done_cb(/* w, client_data, call_data */);
static void	mode_cb(/* w, client_data, call_data */);
static void	mode_help_cb(/* w, client_data, call_data */);
static int	create_mode_window(/* cur_mode */);
    /* help window */
static void	help_ok(/* w, client_data, call_data */);
static void	create_help(/*  */);
    /* msg window */
static void	dialog_resp_cb(/* w, client_data, call_data */);
static int	wait_confirmation(/* w */);
    /* locate window */
static int	window_location(/* loc_str */);
static void	locate_window(/* w */);
    /* cursor */
static void	set_cursor(/* w, is_wait */);
    /* timer */
static void	xt_timer_cb(/* client_data, timer_id */);


	/* ********  window env  ******** */

int	open_display()
{
    if (Dpy)	return NoError;		/* already done */

    Dpy = XOpenDisplay(Opt.DisplayName);
    if (!Dpy) {
	DPR(("%s: cannot open display \"%s\"\n",
				ProgramName, XDisplayName(Opt.DisplayName)));
	return ErrOpenDpy;
    }
    return NoError;
}

void	close_display()
{
    if (TopW)	return;		/* Xt not finished */

    if (Dpy) {
	XCloseDisplay(Dpy);
	Dpy = (Display *) 0;
    }
    return;
}

int	window_env_ok()
{
    return winEnv.status != WIN_ST_NONE ? True : False;
}

int	init_window_env()
{
    enum { XA_DTIMS_ATOM_MAIN, XA_DTIMS_ATOM_STATUS, XA_DTIMS_ATOM_DATA, 
	   NUM_ATOMS }; 
    static char *atom_names[] = { 
      DTIMS_ATOM_MAIN, DTIMS_ATOM_STATUS, DTIMS_ATOM_DATA };

    Display		*dpy = (Display *) 0;
    Widget		topW = (Widget) 0;
    XtAppContext	app = (XtAppContext) 0;
    int			ret = NoError;
    Atom		atoms[XtNumber(atom_names)];

    if (winEnv.status != WIN_ST_NONE)	return NoError;

	/* disable XIM on my own XmText* */
    putenv("XMODIFIERS=@im=_XIMP_None");	/* putenv("XMODIFIERS=@im=none"); */
    XtSetLanguageProc(NULL, NULL, NULL);

    if (!Dpy) {
	ret = open_display();
	if (ret != NoError)
	    return ret;
    }

    if (!(OpFlag & FLAG_NORESOURCE) && isXsession()) {
	if (open_display() == NoError) {	/* set RESOURCE_MANAGER */
	    load_resources();
	}
    }
    if (Dpy)	close_display();

    topW = XtAppInitialize(&app, DTIMS_CLASS,
		options, XtNumber(options),
		&Wargc, Wargv, fallbacks, (ArgList) NULL, (Cardinal) 0);

    dpy = XtDisplay(topW);
    XtSetMappedWhenManaged(topW, False);
    XtRealizeWidget(topW);

    /* set my error handler */
    defaultErrorHandler = XSetErrorHandler(ignoreBadWindow);

    XtGetApplicationResources(topW, &appres,
			      app_resources, XtNumber(app_resources),
			      NULL, 0);

#if	0
    /* handle system Menu's "close" */
    XmAddWMProtocolCallback(topW,
			XmInternAtom(dpy, "WM_DELETE_WINDOW", True),
			(XtCallbackProc)end_window_env, (XtPointer)0);
#endif

    Dpy = dpy; TopW = topW; appC = app;	/* @@@ */

    XInternAtoms(dpy, atom_names, XtNumber(atom_names), False, atoms);

    winEnv.Dpy = dpy;
    winEnv.TopW = topW;
    winEnv.appC = app;
    winEnv.atom_main = atoms[XA_DTIMS_ATOM_MAIN];
    winEnv.atom_status = atoms[XA_DTIMS_ATOM_STATUS];
    winEnv.atom_data = atoms[XA_DTIMS_ATOM_DATA];
    winEnv.atom_owner = XGetSelectionOwner(dpy, winEnv.atom_main);
    winEnv.status = WIN_ST_INIT;

    clear_cmd_property(XtWindow(topW));

    if (winEnv.atom_main == None)
	return ErrOpenDpy;

    if ((OpMode == MODE_START || OpMode == MODE_LISTNAME) 
	&& !(OpFlag & FLAG_REMOTERUN)) {
	if (own_main_atom(XtWindow(TopW)) != True) {
	    DPR(("another program is running on the display '%s'\n",
							XDisplayString(Dpy)));
	    return ErrAnotherProg;
	}
    }

    return NoError;
}

void	end_window_env()
{
    if (TopW) {
	disown_main_atom(XtWindow(TopW));
	XSetErrorHandler(defaultErrorHandler);

	/* XtUnmapWidget(TopW); */
	XtDestroyWidget(TopW);
	XtDestroyApplicationContext(appC);
    } else if (Dpy)
	close_display();

    TopW = (Widget) 0;
    appC = (XtAppContext) 0;
    Dpy = (Display *) 0;

    ximsMain();

    /* return; */
}


int	clear_cmd_property(win)			/* clear WM_COMMAND property */
    Window	win;
{
    int		ret, ac = 0;
    int		clear_ok = False;
    char	**av = NULL;

    /* if (!Dpy || !win)	return False; */

    ret = XGetCommand(Dpy, win, &av, &ac);
    if (ret && ac > 0) {
	XFreeStringList(av);
	XSetCommand(Dpy, win, 0, 0);
	XSync(Dpy, False);
	clear_ok = True;
	DPR2(("\tWM_COMMAND cleared on %#x\n", win));
    }

    return clear_ok;
}

static int	ignoreBadWindow(dpy, error)
    Display *dpy;
    XErrorEvent *error;
{
    if (error->error_code == BadWindow || error->error_code == BadDrawable) {
	return 0;
    } else {		/* invoke default error handler */
	return (*defaultErrorHandler)(dpy, error);
    }
}

#ifdef	unused
static void	finish_window(w, end_window)
    Widget	*w;
    int		end_window;
{
    if (*w) {
	XtUnmapWidget(*w);
	XtDestroyWidget(*w);
	*w = (Widget) 0;
    }

    if (end_window)
	end_window_env();

    return;
}
#endif	/* unused */


static int	own_main_atom(win)	/* own DTIMS_ATOM_MAIN */
    Window	win;
{
    Window	owner = None;

    if (winEnv.atom_main == None)	return False;
    /* if (winEnv.atom_owner != None)	return False; */

    owner = XGetSelectionOwner(winEnv.Dpy, winEnv.atom_main);
    if (owner != None && owner != win) {
	winEnv.atom_owner = owner;
	return False;
    }

    XSetSelectionOwner(winEnv.Dpy, winEnv.atom_main, win, CurrentTime);
    DPR2(("own_main_atom(%#x): atom=%s[%d]\n",
				win, DTIMS_ATOM_MAIN, winEnv.atom_main));
    winEnv.atom_owner = XGetSelectionOwner(winEnv.Dpy, winEnv.atom_main);

    return winEnv.atom_owner == win ? True : False;
}

static int	disown_main_atom(win)	/* disown DTIMS_ATOM_MAIN */
    Window	win;
{
    if (winEnv.atom_main == None)	return False;
    /* if (winEnv.atom_owner == None)	return False; */

    /* give up ownership of DTIMS_ATOM_MAIN */
    if (XGetSelectionOwner(winEnv.Dpy, winEnv.atom_main) == winEnv.atom_owner)
	XSetSelectionOwner(winEnv.Dpy, winEnv.atom_main, None, CurrentTime);
    winEnv.atom_owner = XGetSelectionOwner(winEnv.Dpy, winEnv.atom_main);

    return True;
}


	/* ********  resource_manager  ******** */

static char	*saved_xdefs = NULL;	/* should not be freed */

int	save_RM()
{
    if (!Dpy)	return False;
    saved_xdefs = XResourceManagerString(Dpy);
    return saved_xdefs ? True : False;
}

int	merge_RM(res1, res2)
    char	*res1, *res2;
{
    char	cmdbuf[BUFSIZ*2];
    int		ret;

    if (!Dpy)	return False;

    cmdbuf[0] = 0;
    if (isDT()) {
	char	*dtres_cmd = DTSESSION_RES_PATH;
	if (is_executable(dtres_cmd)) {
	    sprintf(cmdbuf, "%s -merge -system -xdefaults ", dtres_cmd);
	    if (res1) {	strcat(cmdbuf, " -file "); strcat(cmdbuf, res1); }
	    if (res2) {	strcat(cmdbuf, " -file "); strcat(cmdbuf, res2); }
	    strcat(cmdbuf, " >/dev/null 2>&1");
	}
    }
    if (!cmdbuf[0]) {
	sprintf(cmdbuf, "%s %s %s 2>/dev/null | %s -merge -quiet - 2>/dev/null",
					CAT_PATH, res1, res2, XRDB_PATH);
    }

    ret = system(cmdbuf);

    DPR2(("merge_RM(): '%s' & '%s' merged to RESOURCE_MANAGER\n", res1, res2));
    return ret == 0 ? True : False;
}

int	restore_RM()
{
    int		len = saved_xdefs ? strlen(saved_xdefs) : 0;
    int		max = (XMaxRequestSize(Dpy) << 2) - 28;
    int		mode = PropModeReplace;
    unsigned char	*bp = (unsigned char *) saved_xdefs;
    Window	root = RootWindow(Dpy, 0);

    if (!Dpy /* || len < 0 */)		return False;

    DPR2(("restore_RM(): saved RESOURCE_MANAGER = %d (bytes)\n", len));

    if (saved_xdefs == NULL) {		/* len == 0 */
	XDeleteProperty(Dpy, root, XA_RESOURCE_MANAGER);
#if	0
	XSync(Dpy, False);
	{   char	buf[BUFSIZ];
	    sprintf(buf, "%s -quiet -remove 2>/dev/null", XRDB_PATH);
	    DPR(("restore_RM(): '%s'\n", buf));
	    system(buf);
	}
#endif
	return True;
    }

    if (len > max) {
	XGrabServer(Dpy);
	do {
	    XChangeProperty(Dpy, root,
			XA_RESOURCE_MANAGER, XA_STRING, 8, mode, bp, len);
	    bp += max; len -= max;
	    mode = PropModeAppend;
	} while (len > max);
    }
    XChangeProperty(Dpy, root,
			XA_RESOURCE_MANAGER, XA_STRING, 8, mode, bp, len);
    if (mode != PropModeReplace)
	XUngrabServer(Dpy);

    return True;
}


	/* ********  selection window  ******** */

static int		CurIdx = -1;
static char		*CurHost = NULL;
static int		CurHostType = HOST_UNKNOWN;
static ImsList		*curList = 0;
static bool		selection_changed = False;


static void	free_ims_list(list)
    ImsList	*list;
{
    /* DPR(("free_ims_list(list=%p)\n", list)); */
    if (list && list != localList && list != userSel.list) {
	clear_ImsList(list);
	FREE(list);
    }
}

static void	clear_selection_var()
{
    free_ims_list(curList);
    curList = (ImsList *) 0;

    if (CurHost)	FREE(CurHost);
    CurHost = NULL;
    CurHostType = HOST_UNKNOWN;
    CurIdx = -1;
    selection_changed = False;
}

int	start_selection_window()
{
    int		ret = NoError;

    if ((ret = init_window_env()) != NoError)
	return ret;

    clear_selection_var();
    curList = userSel.list;
    /* curHost = NEWSTR(userSel.hostname); */

    if ((ret = create_selection_window()) != NoError)
	return ret;

    /* handle system Menu's "close" */
    XmAddWMProtocolCallback(XtParent(SelW),
			XmInternAtom(Dpy, "WM_DELETE_WINDOW", True),
			(XtCallbackProc)done_cb, (XtPointer)CANCEL_BTN);

    if (isXsession()) {
	clear_cmd_property(XtWindow(TopW));
    }

    locate_window(SelW);
    XtManageChild(SelW);
    XtPopup(XtParent(SelW), XtGrabNone);

    if (FocusW)
	XmProcessTraversal(FocusW, XmTRAVERSE_CURRENT);

    XtRealizeWidget(TopW);
    XtAppMainLoop(appC);
    /* not reached */
}

static void	finish_selection_window(end_window)
    int		end_window;
{
    DPR(("finish_selection_window(end=%s)\n", end_window ? "True" : "False"));

    clear_selection_var();

    stop_help();

    if (SelW) {
#if	0
	if (TopW)
	    disown_main_atom(XtWindow(TopW));
#endif

	XtUnmanageChild(SelW);
	XtPopdown(XtParent(SelW));
	XtDestroyWidget(XtParent(SelW));
	SelW = (Widget) 0;

	if (HostW) {
	    XtUnmanageChild(HostW);
	    XtPopdown(XtParent(HostW));
	    XtDestroyWidget(XtParent(HostW));
	    HostW = (Widget) 0;
	}
    }

    if (end_window)
	end_window_env();

    return;
}

static void	done_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    int		canceled = (int)client_data == CANCEL_BTN;
    int		idx;
    UserSelection	*sel = &userSel;

    if (!canceled) {		/* update userSel */
	idx = CurIdx;
	DPR2(("done_cb(): selected IMS: [%d]%s\n",
					idx, curList->elist[idx]->name));

	if (curList->elist[idx]->status != NoError) {
	    Beep();
	    return;
	}

	if ((CurHost != sel->hostname) 
	    || (CurHost && sel->hostname && strcmp(sel->hostname, CurHost))
	    || sel->list != curList || sel->ims_idx != idx)
	    selection_changed = True;

	if (selection_changed == True) {
	    update_user_selection(sel, curList, idx, CurHost, CurHostType);
	    curList = (ImsList *) 0;
	}
    }

    finish_selection_window(False);

    OpErrCode = NoError;
    OpState = canceled ? State_Select_Canceled : State_Select_Done;
    ximsMain();
}

static void	select_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    int		new_idx = (int) client_data;

    if (new_idx < 0 || new_idx >= curList->num_ent) {
	DPR(("select_cb():\tinvalid index (%d)\n", new_idx));
	return;
    }

    DPR3(("select_cb(): '%s' selected (%d <- %d)\n",
	    curList->elist[new_idx]->name, new_idx, CurIdx));

    CurIdx = new_idx;
}

static void	host_btn_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    start_host_window(CurHost);
}

static void	help_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    ximsHelp(HELP_SELECTION);
}


static int	create_selection_window()
{
    Widget	form, sel_rc, host_rc, cmd_rc, lb, sep;
    Widget	w_tbl[8];
    Arg		args[16];
    int		i, j, n;
    int		ret;
    bool	use_local = False;

	/* selection top window */

	i = 0;
	XtSetArg(args[i], XmNmwmFunctions, WIN_FUNC); i++;
	XtSetArg(args[i], XmNmwmDecorations, WIN_DECR); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
	XtSetArg(args[i], XmNnoResize, True); i++;
	XtSetArg(args[i], XmNallowShellResize, True); i++;
  	/* XtSetArg(args[i], XmNresizable, True); i++; */
	XtSetArg(args[i], XmNmappedWhenManaged, False); i++;
    SelW =
    form = XmCreateFormDialog(TopW, "Selection", args, i);

	/* for child of form */
	i = 0;
	XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
	XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM); i++;
    n = 0;
    w_tbl[n] = 0;
	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
    w_tbl[n] = lb = XmCreateLabelGadget(form, "title", args, i + 1);

    if (RemoteOn()) {
	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	j = i + 1;
	XtSetArg(args[j], XmNorientation, XmHORIZONTAL); j++;
	XtSetArg(args[j], XmNpacking, XmPACK_TIGHT); j++;
	XtSetArg(args[j], XmNisAligned, TRUE); j++;
	XtSetArg(args[j], XmNentryAlignment, XmALIGNMENT_BEGINNING); j++;
	XtSetArg(args[j], XmNentryBorder, 0); j++;
	XtSetArg(args[j], XmNmarginHeight, 1); j++;
	XtSetArg(args[j], XmNmarginWidth, 20); j++;
    HostRC =
    w_tbl[++n] = host_rc = XmCreateRowColumn(form, "host_rc", args, j);
    }

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	j = i + 1;
	XtSetArg(args[j], XmNorientation, XmHORIZONTAL); j++;
	XtSetArg(args[j], XmNpacking, XmPACK_COLUMN); j++;
	XtSetArg(args[j], XmNnumColumns, curList->num_ent); j++;
	XtSetArg(args[j], XmNisAligned, TRUE); j++;
	XtSetArg(args[j], XmNentryAlignment, XmALIGNMENT_BEGINNING); j++;
	XtSetArg(args[j], XmNentryBorder, 0); j++;
	XtSetArg(args[j], XmNmarginHeight, 10); j++;
	/* XtSetArg(args[j], XmNmarginWidth, 40); j++; */
    SelRC =
    w_tbl[++n] = sel_rc = XmCreateRadioBox(form, "select_rc", args, j);

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
    w_tbl[++n] = sep = XmCreateSeparatorGadget(form, "sep", args, i + 1);

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	XtSetArg(args[i+1], XmNbottomAttachment, XmATTACH_FORM);
    w_tbl[++n] = cmd_rc = XmCreateRowColumn(form,"cmd_rc", args, i + 2);

    use_local = True;
    if (RemoteOn()) {
	switch (userSel.host_type) {
	    case HOST_UNKNOWN:
		put_xims_errmsg(ErrUnknownHost, userSel.hostname, 0, 0);
		ret = change_host(NULL, HOST_LOCAL);
		break;
	    case HOST_REMOTE:
		ret = change_host(userSel.hostname, userSel.host_type);
		if (ret == NoError) {
		    change_ims_list(userSel.name, -1);
		    use_local = False;
		} else {
		    if (ret != ErrRemoteAction) {
			put_xims_errmsg(ret, 0 /* userSel.hostname */, 0, 0);
		    }
		}
	}
    }
    if (use_local)
	change_ims_list(NULL, userSel.ims_idx);

    add_cmd_btn(cmd_rc, done_cb, 0, done_cb,
					RemoteOn() ? host_btn_cb : 0, help_cb);

    XtManageChildren(w_tbl, ++n);

    XtManageChild(SelW); locate_window(SelW); XtUnmanageChild(SelW);
    XtVaSetValues(SelW, XmNmappedWhenManaged, True, NULL);
    XtVaSetValues(XtParent(SelW), XmNmappedWhenManaged, True, NULL);
    set_cursor(SelW, False);

    return NoError;
}


static int	change_host(new_host, host_type)
    char	*new_host;
    int		host_type;
{
    int		ret = NoError;
    ImsList	*new_list = (ImsList *) 0;

    if (new_host && host_type == HOST_REMOTE) {
	set_cursor(HostW, True);
	ret = get_remote_conf(&new_list, new_host, NULL, NULL);
	set_cursor(HostW, False);
    } else {
	if (!localList)
	    ret = get_ims_list(&localList, NULL, True);
	new_host = NULL;
	new_list = localList;
    }

    if (ret == NoError) {
	if (CurHost)	FREE(CurHost);
	CurHost = new_host ? NEWSTR(new_host) : NULL;
	CurHostType = host_type;

	DPR(("change_host(%s): curList=(%p) <- (%p)\n",
					new_host, new_list, curList));
	free_ims_list(curList);
	curList = new_list;
    } else if (new_list) {
	free_ims_list(new_list);
    }

    return ret;
}

# define	SelectByReturn	1

#ifdef	SelectByReturn
#include	<Xm/ToggleB.h>
#include	<Xm/PushB.h>
#define	createPB	XmCreatePushButton
#define	createTB	XmCreateToggleButton

static void	add_btn_trans(btn)
    Widget	btn;
{
    char	btn_trans_str[] = "<Key>Return: ArmAndActivate()";
    static XtTranslations	btn_trans = 0;

    if (!btn_trans)
	btn_trans = XtParseTranslationTable(btn_trans_str);
   XtOverrideTranslations(btn, btn_trans);
}

#else
#include	<Xm/ToggleBG.h>
#include	<Xm/PushBG.h>
#define	createPB	XmCreatePushButtonGadget
#define	createTB	XmCreateToggleButtonGadget
#define	add_btn_trans(btn)
# endif	/* SelectByReturn */


static void	change_ims_list(last_ims_name, init_idx)
    char	*last_ims_name;
    int		init_idx;
{
    int	idx;
    int	i, j;
    Arg	args[8];
    XmString	str;
    static int		num_chld = 0;
    static Widget	tb[MAXIMSENT];

#define	IMS_OK(i)	(curList->elist[i]->status == NoError)

	/* deternime initial selection */
    idx = -1;
    if (last_ims_name) {
	i = get_ims_idx(curList, last_ims_name);
	if (i >= 0 && IMS_OK(i))
	    idx = i;
    }
    if (idx < 0) {
	if (init_idx >= 0 && init_idx < curList->num_ent && IMS_OK(init_idx)) {
	    idx = init_idx;
	} else {
	    idx = curList->default_idx;
	    if (!(idx >= 0 && idx < curList->num_ent && IMS_OK(idx))) {
		for (idx = 0; idx < curList->num_ent; idx++)
		    if (IMS_OK(idx))	break;
		if (idx >= curList->num_ent)
		    idx = 0;
	    }
	}
    }

    for (j = 0; j < curList->num_ent; j++) {
	str = XmStringCreateLocalized(curList->elist[j]->label);
	i = 0;
	XtSetArg(args[i], XmNset, j == idx ? True : False); i++;
	XtSetArg(args[i], XmNsensitive, IMS_OK(j)); i++;
	XtSetArg(args[i], XmNlabelString, str); i++;
	XtSetArg(args[i], XmNspacing, 10); i++;
	if (j < num_chld) {
	    XtSetValues(tb[j], args, i);
	} else {
	    tb[j] = createTB(SelRC, "ims", args, i);
	    XtAddCallback(tb[j], XmNvalueChangedCallback, select_cb, (XtPointer)j);
	    add_btn_trans(tb[j]);
	}
	XmStringFree(str);
    }
    if (j > num_chld)
	num_chld = j;
    else if (j < num_chld)
	XtUnmanageChildren(tb + j, num_chld - j);
    XtManageChildren(tb, j);

    CurIdx = idx;
    if (RemoteOn()) {
	if (CurHostType == HOST_UNKNOWN)
	    CurHostType = check_hostname(CurHost);
	set_host_label(CurHostType, CurHost);
    }

#undef	IMS_OK
}

static void	set_host_label(host_type, hostname)
    int		host_type;
    char	*hostname;
{
    static Widget	lb_host = 0;

    /* if (!HostRC)	return; */

    if (host_type == HOST_REMOTE) {	/* change HostLabel & manage HostRC */
	XmString	str;

	if (!lb_host) {
	    Widget	lb;
	    Arg	args[4];
	    int	n = 0;

	    XtSetArg(args[n], XmNmarginLeft, 100); n++;
	    lb = XmCreateLabelGadget(HostRC, "host_label", args, n);
	    lb_host = XmCreateLabelGadget(HostRC, "hostname", NULL, 0);
	    XtManageChild(lb);
	    XtManageChild(lb_host);
	}
	str = XmStringCreateLocalized(hostname);
	XtVaSetValues(lb_host, XmNlabelString, str, NULL);
	XmStringFree(str);

	XtManageChild(HostRC);
    } else {
	    /* unmanage HostRC */
	XtUnmanageChild(HostRC);
    }
}


static void	add_cmd_btn(parent_rc, cb_ok, cb_clear, cb_cancel, cb_host, cb_help)
    Widget	parent_rc;
    void	(*cb_ok)(), (*cb_cancel)(), (*cb_clear)(),
			(*cb_host)(), (*cb_help)();
{
    Widget	pb[4];
    Arg		args[12];
    int		n, nbtn;

    nbtn = 0;
    if (cb_ok)		nbtn++;
    if (cb_clear)	nbtn++;
    if (cb_cancel)	nbtn++;
    if (cb_host)	nbtn++;
    if (cb_help)	nbtn++;
    if (nbtn == 0)	return;

	/* command buttons on "cmd_rc" */
	n = 0;
      if (nbtn > 3) {
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
      } else {
	XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
      }
	XtSetArg(args[n], XmNnumColumns, nbtn); n++;
	XtSetArg(args[n], XmNisAligned, TRUE); n++;
	XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
	XtSetArg(args[n], XmNentryBorder, 0); n++;
	XtSetArg(args[n], XmNmarginHeight, 1); n++;
	XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	/* XtSetArg(args[n], XmNmarginWidth, 20); n++; */
	/* XtSetArg(args[n], XmNspacing, 20); n++; */
    XtSetValues(parent_rc, args, n);

	/* for child of parent_rc */

    nbtn = 0;
	n = 0;
    if (cb_ok) {
	pb[nbtn] = createPB(parent_rc, "OK", args, n);
	XtAddCallback(pb[nbtn], XmNactivateCallback,
					cb_ok, (XtPointer)OK_BTN);
	FocusW = pb[nbtn];
	/* XtVaSetValues(parent_rc, XmNinitialFocus, pb, NULL); */
	nbtn++;
    }
    if (cb_clear) {
	pb[nbtn] = createPB(parent_rc, "Clear", args, n);
	XtAddCallback(pb[nbtn], XmNactivateCallback,
					cb_clear, (XtPointer)CLEAR_BTN);
	nbtn++;
    }
    if (cb_cancel) {
	pb[nbtn] = createPB(parent_rc, "Cancel", args, n);
	XtAddCallback(pb[nbtn], XmNactivateCallback,
					cb_cancel, (XtPointer)CANCEL_BTN);
	nbtn++;
    }
    if (cb_host) {
	pb[nbtn] = createPB(parent_rc, "ChangeHost", args, n);
	XtAddCallback(pb[nbtn], XmNactivateCallback,
					cb_host, (XtPointer)HOST_BTN);
	nbtn++;
    }
    if (cb_help) {
	pb[nbtn] = createPB(parent_rc, "Help", args, n);
	XtAddCallback(pb[nbtn], XmNactivateCallback,
					cb_help, (XtPointer)HELP_BTN);
	nbtn++;
    }
#ifdef	SelectByReturn
    for (n = 0; n < nbtn; n++)
	add_btn_trans(pb[n]);
#endif	/* SelectByReturn */

    XtManageChildren(pb, nbtn);
}

	/* ***** host window ***** */

static void	start_host_window(cur_host)
    char	*cur_host;
{
    int	ret;

    if (!HostW) {
	create_host_window(cur_host);

	/* handle system Menu's "close" */
	XmAddWMProtocolCallback(XtParent(HostW),
			XmInternAtom(Dpy, "WM_DELETE_WINDOW", True),
			(XtCallbackProc)host_done_cb, (XtPointer)CANCEL_BTN);
    }

    XtVaSetValues(HostText, XmNvalue, CurHost, NULL);

    if (HostText)
	XmProcessTraversal(HostText, XmTRAVERSE_CURRENT);

    locate_window(HostW);
    XtManageChild(HostW);
    XtPopup(XtParent(HostW), XtGrabNone);

#ifdef	DEBUG
    pr_brk("start_host_window");
#endif

    return;
}

static void	finish_host_window()
{
    DPR(("finish_host_window()\n"));

    if (HostW) {
	XtUnmanageChild(HostW);
	XtPopdown(XtParent(HostW));
	    /* not destroy */
    }
}

static void	host_done_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    int		cancel = (int)client_data == CANCEL_BTN;
    int		ret = NoError;
    char	*new_host, *txt, *p;
    bool	host_changed = False;
    int		host_type;

    if (cancel) {
	finish_host_window();
	return;
    }

    /* if (!HostText)	return; */
    new_host = NULL;
    txt = XmTextFieldGetString(HostText);
    if (txt) {
	p = trim_line(txt);
	new_host = (*p) ? p : NULL;
    }
    if (new_host) {
	if (!(CurHost) || strcmp(CurHost, new_host)) {
	    host_type = check_hostname(new_host);

	    switch (host_type) {
		case HOST_LOCAL:
		    new_host = NULL;
		    if (CurHost)
			host_changed = True;
		    break;
		case HOST_REMOTE:
		    host_changed = True;
		    break;
		case HOST_UNKNOWN:
		    host_changed = False;
		    put_xims_errmsg(ErrUnknownHost, new_host, 0, 0);

			/* don't finish window */
		    XtFree(txt);
		    return;
	    }
	}
    } else if (CurHost) {		/* 'new_host == NULL' ==> local host */
	host_type = HOST_LOCAL;
	host_changed = True;
    }

    DPR(("host_cb(): hostname %s changed '%s' (<- %s)\n",
			    host_changed ? NULL : "NOT", new_host, CurHost));

    if (host_changed) {
	char	*last_ims = NULL;

	    /* save the name of selected ims */
	if (CurIdx >= 0 && CurIdx < curList->num_ent)
	    last_ims = NEWSTR(curList->elist[CurIdx]->name);

	ret = change_host(new_host, host_type);
	if (ret == NoError) {
	    finish_host_window();
	    change_ims_list(last_ims, -1);
	    selection_changed = True;
	} else {
	    if (ret != ErrRemoteAction) {
		put_xims_errmsg(ret, new_host, 0, 0);
	    }
		/* don't finish window */
	}

	FREE(last_ims);
    } else {
	finish_host_window();
    }

    XtFree(txt);

#ifdef	DEBUG
    pr_brk("host_done_window");
#endif
}

static void	host_clear_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    /* if (!HostText)	return; */
    XtVaSetValues(HostText, XmNvalue, "", NULL);
}

static void	host_done_action(w, ev, args, num_args)
    Widget	w;
    XEvent	*ev;
    String	*args;
    Cardinal	*num_args;
{
    host_done_cb(w, (XtPointer) OK_BTN, 0);
}

static void	create_host_window(cur_host)
    char	*cur_host;
{
    Widget	form, cmd_rc, lb1, sep, w_tbl[5];
    Widget	host_desc, host_rc, host_lb, host_txt;
    Arg		args[16], hargs[8];
    int		i, j, n;
    Widget	hw[4];
    int		nhw = 0;
    XtTranslations	trans;
    static char		host_trans[] = "<Key>Return: host-done()";
    static XtActionsRec	host_actions[] = {
	{ "host-done", (XtActionProc) host_done_action }
    };

	i = 0;
	XtSetArg(args[i], XmNmwmFunctions, WIN_FUNC); i++;
	XtSetArg(args[i], XmNmwmDecorations, WIN_DECR); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
	XtSetArg(args[i], XmNnoResize, True); i++;
	XtSetArg(args[i], XmNallowShellResize, True); i++;
	XtSetArg(args[i], XmNmappedWhenManaged, False); i++;
	XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    HostW =
    form = XmCreateFormDialog(TopW, "Host", args, i);

	/* for child of form */
	i = 0;
	XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
	XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM); i++;
    n = 0;
    w_tbl[n] = 0;
	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
    w_tbl[n] = lb1 = XmCreateLabelGadget(form, "title", args, i + 1);

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	j = i + 1;
	XtSetArg(args[j], XmNmarginHeight, 8); j++;
	XtSetArg(args[j], XmNmarginWidth, 8); j++;
    w_tbl[++n] = host_desc = XmCreateLabelGadget(form, "host_desc", args, j);

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	j = i + 1;
	XtSetArg(args[j], XmNorientation, XmHORIZONTAL); j++;
	XtSetArg(args[j], XmNpacking, XmPACK_TIGHT); j++;
	XtSetArg(args[j], XmNisAligned, TRUE); j++;
	XtSetArg(args[j], XmNentryAlignment, XmALIGNMENT_BEGINNING); j++;
	XtSetArg(args[j], XmNentryBorder, 0); j++;
	XtSetArg(args[j], XmNmarginHeight, 1); j++;
	/* XtSetArg(args[j], XmNmarginWidth, 20); j++; */
    w_tbl[++n] = host_rc = XmCreateRowColumn(form, "host_rc", args, j);

    nhw = 0;
	j = 0;
	XtSetArg(hargs[j], XmNalignment, XmALIGNMENT_BEGINNING); j++;
    hw[nhw++] = host_lb = XmCreateLabelGadget(host_rc, "host_label", hargs, j);
	j = 0;
	/* XtSetArg(hargs[j], XmNcolumns, TEXT_COLUMNS); j++; */
	/* XtSetArg(hargs[j], XmNmaxLength, MAX_HOSTNAME_LEN); j++; */
	XtSetArg(hargs[j], XmNeditable, True); j++;
	XtSetArg(hargs[j], XmNvalue, cur_host); j++;
	XtSetArg(hargs[j], XmNmarginHeight, 1); j++;
	XtSetArg(hargs[j], XmNmarginWidth, 1); j++;
    HostText =
    hw[nhw++] = host_txt = XmCreateTextField(host_rc, "host_text", hargs, j);
    XtManageChildren(hw, nhw);

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
    w_tbl[++n] = sep = XmCreateSeparatorGadget(form, "sep", args, i + 1);
	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	XtSetArg(args[i+1], XmNbottomAttachment, XmATTACH_FORM);
    w_tbl[++n] = cmd_rc = XmCreateRowColumn(form, "cmd_rc", args, i + 2);
    XtManageChildren(w_tbl, ++n);

    add_cmd_btn(cmd_rc, host_done_cb, host_clear_cb, host_done_cb, 0, 0);

    /* set 'host-done' action for 'Return' key */
    XtAppAddActions(appC, host_actions, XtNumber(host_actions));
    trans = XtParseTranslationTable(host_trans);
    XtOverrideTranslations(HostText, trans);

    XtManageChild(HostW); locate_window(HostW); XtUnmanageChild(HostW);
    XtVaSetValues(XtParent(HostW), XmNmappedWhenManaged, True, NULL);
    XtVaSetValues(HostW, XmNmappedWhenManaged, True, NULL);
    set_cursor(HostW, False);

    return;
}


	/* ********  mode window ******** */

static int	OrgMode = SEL_MODE_NONE;
static int	CurMode = SEL_MODE_NONE;

int	start_mode_window(cur_mode)
    int		cur_mode;
{
    int	ret;

    OrgMode = CurMode = cur_mode;

    if ((ret = init_window_env()) != NoError)
	return ret;

    if ((ret = create_mode_window(cur_mode)) != NoError)
	return ret;

    /* handle system Menu's "close" */
    XmAddWMProtocolCallback(XtParent(ModeW),
			XmInternAtom(Dpy, "WM_DELETE_WINDOW", True),
			(XtCallbackProc)mode_done_cb, (XtPointer)CANCEL_BTN);

    locate_window(ModeW);
    XtManageChild(ModeW);
    XtPopup(XtParent(ModeW), XtGrabNone);

    if (FocusW)
	XmProcessTraversal(FocusW, XmTRAVERSE_CURRENT);

    XtAppMainLoop(appC);
    /* not rearched */
}

static void	finish_mode_window(end_window)
    int		end_window;
{
    if (ModeW) {
	XtUnmanageChild(ModeW);
	XtPopdown(XtParent(ModeW));
	XtDestroyWidget(XtParent(ModeW));
	ModeW = (Widget) 0;
    }
    stop_help();

    if (end_window)
	end_window_env();

    return;
}

static void	mode_done_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    int		ret = NoError;
    int		canceled = (int)client_data == CANCEL_BTN;

    DPR(("mode_done(%s):\torg=%d  cur=%d\n",
				canceled ? "Cancel" : "OK", OrgMode, CurMode));
    if (canceled) {
	OpErrCode = NoError;
	OpState = State_Mode_Canceled;
    } else {
	ret = set_select_mode(OrgMode, CurMode);

	OpErrCode = ret;
	OpState = State_Mode_Done;

	if (OpErrCode != NoError) {
	    finish_mode_window(False);
	    LastErrMsg = OpErrCode;
	    put_xims_errmsg(OpErrCode, 0, 0, 0);
	    if (WaitingDialogReply) {
		DPR(("mode_done_cb(): enter xevent_loop()\n"));
		xevent_loop();	/* never returns */
	    }
	}
    }
    finish_mode_window(True);
	/* never returns */
}

static void	mode_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    int		is_set = (int)((XmToggleButtonCallbackStruct *)call_data)->set;
    int		is_auto = (int)client_data;

    CurMode = (is_auto && is_set) ? SEL_MODE_AUTO : SEL_MODE_NOAUTO;
}

static void	mode_help_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    ximsHelp(HELP_MODE);
}

static int	create_mode_window(cur_mode)
    int		cur_mode;
{
    Widget	form, mode_rc, cmd_rc, lb1, sep, w_tbl[8];
    Arg		args[16];
    int		i, j, n;

	i = 0;
	XtSetArg(args[i], XmNmwmFunctions, WIN_FUNC); i++;
	XtSetArg(args[i], XmNmwmDecorations, WIN_DECR); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
	XtSetArg(args[i], XmNnoResize, True); i++;
	XtSetArg(args[i], XmNallowShellResize, True); i++;
	XtSetArg(args[i], XmNmappedWhenManaged, False); i++;
    ModeW =
    form = XmCreateFormDialog(TopW, "Mode", args, i);

	/* for child of form */
	i = 0;
	XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
	XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM); i++;
    n = 0;
    w_tbl[n] = 0;
	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
    w_tbl[n] = lb1 = XmCreateLabelGadget(form, "title", args, i + 1);

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	j = i + 1;
	XtSetArg(args[j], XmNisAligned, TRUE); j++;
	XtSetArg(args[j], XmNentryAlignment, XmALIGNMENT_BEGINNING); j++;
	XtSetArg(args[j], XmNentryBorder, 0); j++;
	XtSetArg(args[j], XmNmarginHeight, 10); j++;
	XtSetArg(args[j], XmNmarginWidth, 30); j++;

#ifdef	SelectByReturn
		/* use TB */
	XtSetArg(args[j], XmNorientation, XmHORIZONTAL); j++;
	XtSetArg(args[j], XmNpacking, XmPACK_COLUMN); j++;
	XtSetArg(args[j], XmNnumColumns, NUM_SEL_MODE); j++;
    w_tbl[++n] = mode_rc = XmCreateRadioBox(form, "mode_rc", args, j);
    {
	Arg	bargs[8];
	Widget	tb[NUM_SEL_MODE];
	int	k = 0;
	int	set_idx = cur_mode == SEL_MODE_AUTO ? 1 : 0;

	k = 0;
	XtSetArg(bargs[k], XmNspacing, 10); k++;
	tb[0] = createTB(mode_rc, "button_0", bargs, k);
	tb[1] = createTB(mode_rc, "button_1", bargs, k);
	for (k = 0; k < NUM_SEL_MODE; k++) {
	    XtVaSetValues(tb[k], XmNset, k == set_idx ? True : False, NULL);
	    XtAddCallback(tb[k], XmNvalueChangedCallback, mode_cb, (XtPointer)k);
	    add_btn_trans(tb[k]);
	}
	XtManageChildren(tb, NUM_SEL_MODE);
    }
#else
		/* use TBGadget */
	XtSetArg(args[j], XmNbuttonCount, NUM_SEL_MODE); j++;
	XtSetArg(args[j], XmNbuttonSet, cur_mode == SEL_MODE_AUTO ? 1 : 0); j++;
	XtSetArg(args[j], XmNsimpleCallback, mode_cb); j++;
    w_tbl[++n] = mode_rc = XmCreateSimpleRadioBox(form, "mode_rc", args, j);
#endif	/* SelectByReturn */

	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
    w_tbl[++n] = sep = XmCreateSeparatorGadget(form, "sep", args, i + 1);
	XtSetArg(args[i], XmNtopWidget, w_tbl[n]);
	XtSetArg(args[i+1], XmNbottomAttachment, XmATTACH_FORM);
    w_tbl[++n] = cmd_rc = XmCreateRowColumn(form, "cmd_rc", args, i + 2);
    XtManageChildren(w_tbl, ++n);

    add_cmd_btn(cmd_rc, mode_done_cb, 0, mode_done_cb, 0, mode_help_cb);

    XtManageChild(ModeW); locate_window(ModeW); XtUnmanageChild(ModeW);
    XtVaSetValues(XtParent(ModeW), XmNmappedWhenManaged, True, NULL);
    XtVaSetValues(ModeW, XmNmappedWhenManaged, True, NULL);
    set_cursor(ModeW, False);

    return NoError;
}


	/* ********  ximsHelp  ******** */

static void	help_ok(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    stop_help();
}

void	stop_help()
{
    if (HelpW) {
	XtUnmanageChild(HelpW);
	XtPopdown(XtParent(HelpW));
    }
}

static void	create_help()
{
    int		i;
    Arg		args[10];

	i = 0;
	XtSetArg(args[i], XmNmwmFunctions, WIN_FUNC); i++;
	XtSetArg(args[i], XmNmwmDecorations, WIN_DECR); i++;
	XtSetArg(args[i], XmNautoUnmanage, True); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
	XtSetArg(args[i], XmNnoResize, True); i++;
	XtSetArg(args[i], XmNallowShellResize, True); i++;
	XtSetArg(args[i], XmNmappedWhenManaged, False); i++;
    HelpW = XmCreateInformationDialog(TopW, "Help", args, i);

    XtUnmanageChild(XmMessageBoxGetChild(HelpW, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(HelpW, XmDIALOG_HELP_BUTTON));
    XtAddCallback(HelpW, XmNokCallback, help_ok, (XtPointer)0);

	/* help_dialog is not located correctly unless once managed */
    /* XtSetMappedWhenManaged(XtParent(HelpW), False); */
    XtManageChild(HelpW);
    XtUnmanageChild(HelpW);
    XtSetMappedWhenManaged(XtParent(HelpW), True);
    XtSetMappedWhenManaged(HelpW, True);
    set_cursor(HelpW, False);

    return;
}

void	ximsHelp(help_type)
    int		help_type;
{
    char	*msg = NULL;
    XmString	str;
    static int	last_help_type = -1;

    if (!HelpW)
	create_help();

    if (help_type != last_help_type) {

	switch (help_type) {
	    case HELP_MODE:		msg = appres.modeHelpMsg; break;
	    default:
	    case HELP_SELECTION:	msg = appres.selectionHelpMsg; break;
	}

	if (!msg || !*msg) {
	    Beep();
#ifdef	DEBUG
	    if (DebugLvl > 0)
		msg = "help message not defined in resource file\n";
	    else
#endif
		return;
	}

	if (XtIsManaged(HelpW))
	    XtUnmanageChild(HelpW);

	str = XmStringCreateLocalized(msg);
	XtVaSetValues(HelpW, XmNmessageString, str, NULL);
	XmStringFree(str);

	last_help_type = help_type;
    }

    locate_window(HelpW);
    XtManageChild(HelpW);
    XtPopup(XtParent(HelpW), XtGrabNone);
}


	/* ********  message dialog  ******** */

static int	dialog_resp = XmCR_NONE;
static bool	in_confirmation = False;

static void	dialog_resp_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    Widget	msg_box = (Widget) client_data;
    XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;

    dialog_resp = cbs->reason;
    XtUnmanageChild(msg_box);

    if (!in_confirmation) {
	WaitingDialogReply = False;
	DPR3(("dialog_resp_cb(): WaitingDialogReply ==> False\n"));

	if (InWaitingState())
	    ximsMain();
    }
}

#ifdef	unused
static int	wait_confirmation(w)
    Widget	w;
{
    XtAppContext	context;

    dialog_resp = XmCR_NONE;

    /* XtManageChild(w); */
    /* context = XtWidgetToApplicationContext(w); */
    context = appC;
    in_confirmation = True;
    while (dialog_resp == XmCR_NONE || XtAppPending(context)) {
	XtAppProcessEvent(context, XtIMAll);
    }
    XtUnmanageChild(w);
    in_confirmation = False;

    return dialog_resp;
}
#endif	/* unused */

int	put_msg_win(type, msg)
    int		type;
    char	*msg;
{
    int		reply;
    Widget	btn;
    int		wait_resp = FALSE;
    XmString	str;
    int		ret;

    DPR3(("put_msg_win(type=%d):\tmsg=%s", type, msg));

    if (!msg || !*msg) {
	DPR(("put_msg_win(): msg is empty\n"));
	return -1;
    } else if (msg[strlen(msg) - 1] != '\n') {
	DPR(("put_msg_win(): msg isn't terminated by '\\n'\n"));
	return -1;
    }

    switch (type) {
	case MSGTYP_FATAL:		type = XmDIALOG_ERROR; break;
	case MSGTYP_WARN:		type = XmDIALOG_WARNING; break;
	case MSGTYP_CONFIRM:		type = XmDIALOG_QUESTION;
					wait_resp = TRUE; break;
	default:
	case MSGTYP_INFO:		type = XmDIALOG_INFORMATION; break;
    }

    if ((ret = init_window_env()) != NoError)
	return -1;

    if (!MsgW) {		/* create message dialog */
	Arg	args[10];
	int	i = 0;

	XtSetArg(args[i], XmNautoUnmanage, True); i++;
	XtSetArg(args[i], XmNmwmFunctions, WIN_FUNC); i++;
	XtSetArg(args[i], XmNmwmDecorations, WIN_DECR); i++;
	XtSetArg(args[i], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
	XtSetArg(args[i], XmNnoResize, True); i++;
	XtSetArg(args[i], XmNallowShellResize, True); i++;
	XtSetArg(args[i], XmNmappedWhenManaged, False); i++;
	MsgW = XmCreateMessageDialog(TopW, "Message", args, i);
	    XtUnmanageChild(XmMessageBoxGetChild(MsgW, XmDIALOG_HELP_BUTTON));
	XtAddCallback(MsgW, XmNokCallback, dialog_resp_cb, (XtPointer)MsgW);
	XtAddCallback(MsgW, XmNcancelCallback, dialog_resp_cb, (XtPointer)MsgW);

	/* dialog is not located correctly unless once managed */
	XtManageChild(MsgW); XtUnmanageChild(MsgW);
	XtSetMappedWhenManaged(XtParent(MsgW), True);
	XtSetMappedWhenManaged(MsgW, True);
	set_cursor(MsgW, False);
    }
    btn = XmMessageBoxGetChild(MsgW, XmDIALOG_CANCEL_BUTTON);

    if (wait_resp)	XtManageChild(btn);
    else		XtUnmanageChild(btn);

    str = XmStringCreateLocalized(msg);
    XtVaSetValues(MsgW, XmNdialogType, type,
			XmNmessageString, str,
			XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
			/*
			XmNdialogStyle, wait_resp ? XmDIALOG_FULL_APPLICATION_MODAL
							: XmDIALOG_MODELESS,
			*/
			NULL);
    XmStringFree(str);

    locate_window(MsgW);
    XtManageChild(MsgW);
    if (type == MSGTYP_FATAL || type == MSGTYP_WARN) {
	Beep();
    }

    reply = XmCR_OK;
    if (wait_resp) {
#ifdef	unused
	DPR3(("wait_dialog_resp(START): WaitingDialogReply ==> True\n"));
	WaitingDialogReply = True;

	reply = wait_confirmation(MsgW);

	WaitingDialogReply = False;
	DPR3(("wait_dialog_resp(DONE): WaitingDialogReply ==> False\n"));
#endif	/* unused */
    } else {
	DPR3(("put_msg_win(): WaitingDialogReply ==> True\n"));
	WaitingDialogReply = True;
    }

    return reply == XmCR_OK ? True : False;
}


	/* ********  locate_window  ******** */

#define	LOC_NONE		-1
#define	LOC_CENTER		0
#define	LOC_TOP			(1<<0)
#define	LOC_BOTTOM		(1<<1)
#define	LOC_LEFT		(1<<2)
#define	LOC_RIGHT		(1<<3)
#define	LOC_MARGIN		5

static int	window_location(loc_str)
    char	*loc_str;
{
    int		locate_type = LOC_CENTER;
    char	*lower_str, *p;

    lower_str = NEWSTR(loc_str);
    if (p = lower_str) {
	to_lower_str(p);
	if (strstr(lower_str, "center"))	locate_type |= LOC_CENTER;
	if (strstr(lower_str, "top"))		locate_type |= LOC_TOP;
	if (strstr(lower_str, "bottom"))	locate_type |= LOC_BOTTOM;
	if (strstr(lower_str, "left"))		locate_type |= LOC_LEFT;
	if (strstr(lower_str, "right"))		locate_type |= LOC_RIGHT;
	FREE(lower_str);
    }
    return locate_type;
}

static void	locate_window(w)
    Widget	w;
{
    int		scr;
    int		x, y;
    int		dpy_w, dpy_h;
    Dimension	width, height;
    static int	locate_type = LOC_NONE;

    /* if (!isXsession())	return; */

    if (locate_type == LOC_NONE) {
	locate_type = window_location(appres.windowLocation);
	DPR3(("locate_window(): locate_type=%d (%s)\n",
				locate_type, appres.windowLocation));
    }

    width = height = (Dimension) 0;
    scr = XDefaultScreen(Dpy);
    dpy_w = DisplayWidth(Dpy, scr);
    dpy_h = DisplayHeight(Dpy, scr);

    XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);

    x = y = 0;
    if (locate_type) {
	if (locate_type & LOC_TOP)	y = LOC_MARGIN;
	if (locate_type & LOC_BOTTOM)	y = dpy_h - height - LOC_MARGIN * 2;
	if (locate_type & LOC_LEFT)	x = LOC_MARGIN;
	if (locate_type & LOC_RIGHT)	x = dpy_w - width - LOC_MARGIN * 2;
    }
    if (!x)	x = (int) (dpy_w - width - LOC_MARGIN * 2) / 2;
    if (!y)	y = (int) (dpy_h - height - LOC_MARGIN * 2) / 2;

    DPR3(("locate_window(): w=%d h=%d ==> x=%d y=%d\n", width, height, x, y));

    XtVaSetValues(w, XmNx, x, XmNy, y, NULL);
}


	/* ********  cursor (normal / wait)  ******** */

static void	set_cursor(w, is_wait)
    Widget	w;
    int		is_wait;
{
    static Cursor	cursors[2] = { None, None };
    int		idx;

    idx = is_wait ? 1 : 0;

    /* if (!Dpy || !TopW || !w)	return; */
    if (cursors[idx] == None) {
	cursors[idx] = XCreateFontCursor(Dpy, idx ? XC_watch : XC_left_ptr);
    }

    XDefineCursor(Dpy, XtWindow(w), cursors[idx]);
    XFlush(Dpy);
}


	/* *****  waiting functions  ***** */
void	xevent_loop()
{
    if (appC)
	XtAppMainLoop(appC);
}

static time_t		xt_start_tm = 0L;	/* in sec */
static XtIntervalId	xt_last_timer = (XtIntervalId)0;
static bool		xt_waiting = False;

void	xt_start_waiting()
{
    if (!appC)	return;

    xt_start_tm = time((time_t) 0);
    xt_last_timer = XtAppAddTimeOut(appC, (unsigned long) Opt.Interval,
						xt_timer_cb, (XtPointer)0);
    /* if (!xt_last_timer)	return; */

    DPR(("xt_start_waiting(): EventLoop (interval=%d)\n", Opt.Interval));

    xt_waiting = True;
    XtAppMainLoop(appC);
}

void	xt_stop_waiting()
{
    if (xt_last_timer) {
	XtRemoveTimeOut(xt_last_timer);
	xt_last_timer = (XtIntervalId) 0;
    }

    xt_waiting = False;
    ximsWaitDone();
}

static void	xt_timer_cb(client_data, timer_id)
    XtPointer		client_data;
    XtIntervalId	*timer_id;
{
    int		lapse;

    /* DPR3(("xt_timer_cb(timer_id=%d): last_timer=%d\n",
						*timer_id, xt_last_timer)); */

    if (*timer_id != xt_last_timer)	return;
    xt_last_timer = (XtIntervalId) 0;
    lapse = (int) time((time_t) 0) - xt_start_tm;

#ifdef	DEBUG
    if (DebugLvl >= 1)	putc('.', LogFp), fflush(LogFp);
#endif

    if (im_mod_available((RunEnv *) 0) != 0 || lapse >= Opt.Timeout) {
	DPR(("xt_timer_cb(tmout=%d): wait done (%d sec.)\n",
							Opt.Timeout, lapse));
	xt_stop_waiting();	/* never returns */
    }

    xt_last_timer = XtAppAddTimeOut(appC, (unsigned long) Opt.Interval,
						xt_timer_cb, (XtPointer)0);
    return;
}

