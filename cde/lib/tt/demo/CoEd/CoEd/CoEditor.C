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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: CoEditor.C /main/4 1998/05/18 16:41:01 rafi $ 			 				
/*
 * CoEditor.C 1.14	94/11/17
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "CoEditor.h"
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/CascadeB.h>
#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

extern const char *ToolName;
extern int	   abortCode;
extern int         timeOutFactor;
extern Tt_pattern *sessPats;
extern Display    *myDpy;
extern char       *myAltMediaType;
extern XtAppContext myContext;
extern int	   maxBuffers;
extern int	  *pArgc;
extern char	 **globalArgv;

const int	MaxEditors		= 100;
CoEditor       *CoEditor::editors[ MaxEditors ];
int		CoEditor::numEditors	= 0;

static void
userChoiceCB(
	Widget    but,
	XtPointer pChoice,
	XtPointer //callData
)
{
	XtVaGetValues( but, XmNuserData, pChoice, 0 );
}

#define  TIGHTNESS  20

static int
userChoice(
	XtAppContext	context,
	Widget		parent,
	const char     *explanation,
	unsigned int	numChoices,
	...
)
{
	const char *choiceString;
	Dimension bh, bw;
	Position bx, by;
	Widget action_area, but, label, notice, pane, rc;
	XmString label_str;

	XtVaGetValues(parent,
                      XmNx,      &bx,
                      XmNy,      &by,
                      XmNwidth,  &bw,
                      XmNheight, &bh,
                      0);
	notice = XtVaCreatePopupShell( "notice",
				       xmDialogShellWidgetClass, parent,
                                       XmNtitle,                 "Message",
                                       XmNdeleteResponse,        XmDESTROY,
                                       XmNx,                     bx,
                                       XmNy,                     by+(bh/2),
                                       0 );
	pane = XtVaCreateWidget("pane", xmPanedWindowWidgetClass, notice,
                                XmNsashWidth , 1,
                                XmNsashHeight, 1,
                                0 );
        rc = XtVaCreateWidget("control_area", xmRowColumnWidgetClass, pane, 0 );
	label_str = XmStringCreate((char *) explanation,
                                       XmFONTLIST_DEFAULT_TAG);
	label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, rc,
                                       XmNlabelString, label_str,
                                       XmNleftAttachment, XmATTACH_FORM,
                                       XmNtopAttachment,  XmATTACH_FORM,
                                       0 );
	XtVaSetValues(label, XmNlabelString, label_str, 0);
	XmStringFree(label_str);
	XtManageChild(rc);

	action_area = XtVaCreateWidget("action_area", xmFormWidgetClass, pane,
                                       XmNfractionBase, TIGHTNESS*numChoices - 1,
                                       0 );

	va_list	args;
	va_start( args, numChoices );
	int choice = -1;
	for (int i = 0; i < numChoices; i++) {
		choiceString = va_arg( args, const char * );
		but = XtVaCreateManagedWidget( choiceString,
                        xmPushButtonWidgetClass, action_area,
                        XmNleftAttachment, i ? XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition,   TIGHTNESS*i,
                        XmNtopAttachment,  XmATTACH_FORM,
                        XmNbottomAttachment, XmATTACH_FORM,
                        XmNrightAttachment,
                          i != numChoices-1 ? XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition,  TIGHTNESS*i + (TIGHTNESS-1),
                        XmNshowAsDefault,  i == 0,
                        XmNdefaultButtonShadowThickness, 1,
                        XmNuserData,                     i,
                        0 );
		XtAddCallback( but, XmNactivateCallback, userChoiceCB, &choice );
	}
	XtManageChild(action_area);
	XtManageChild(pane);
	XtPopup( notice, XtGrabExclusive );
	while (choice < 0) {
		XEvent event;
		XtAppNextEvent( context, &event );
		XtDispatchEvent( &event );
	}
	XtDestroyWidget(notice);
	return choice;
}

CoEditor::CoEditor(
	Widget *parent
)
{
	_init();
	_init( parent );
}

CoEditor::CoEditor(
	Widget     *parent,
	const char *file
)
{
	_init();
	_init( parent );
	_load( file );
}

CoEditor::CoEditor(
	Widget	       *parent,
	Tt_message	msg,
	const char     * /*docname*/,
	Tt_status      &status
)
{
	_init();
	status = _init( msg );
	if (status != TT_OK) {
		return;
	}
	_init( parent );
	status = _acceptContract( msg );
}

CoEditor::CoEditor(
	Widget	       *parent,
	Tt_message	msg,
	int		/*readOnly*/,
	const char     *file,
	const char     * /*docname*/,
	Tt_status      &status
)
{
	_init();
	status = _init( msg );
	if (status != TT_OK) {
		return;
	}
	_init( parent );
	status = _load( file );
	if (status != TT_OK) {
		return;
	}
	status = _acceptContract( msg );
}

CoEditor::CoEditor(
	Widget	       *parent,
	Tt_message	msg,
	int		/*readOnly*/,
	unsigned char  *contents,
	int             /*len*/,
	const char     * /*docname*/,
	Tt_status      &status
)
{
	_init();
	status = _init( msg );
	if (status != TT_OK) {
		return;
	}
	_init( parent );
	XmTextSetString(_text, (char *) contents);
	XtAddCallback(_text, XmNvalueChangedCallback,
                   (XtCallbackProc) CoEditor::_textUpdateCB_, (caddr_t)this );
	status = _acceptContract( msg );
}

CoEditor::~CoEditor()
{
	//
	// No need for a separate save if we are sending the document
	// back in a reply.
	//
	if (_contract == 0) {
		if (_modifiedByMe) {
			// we revert before quitting if we don't want to save
			_save();
		}
	} else {
		int   len;
		char *contents = _contents( &len );
		ttmedia_load_reply( _contract, (unsigned char *)contents,
				    len, 1 );
		if (contents != 0) {
			XtFree( contents );
		}
		_contract = 0;
	}
	numEditors--;
	for (int i = 0; i < MaxEditors; i++) {
		if (editors[i] == this) {
			editors[i] = 0;
		}
	}
}

Tt_message
CoEditor::loadISOLatin1_(
	Tt_message	msg,
	void           *pWidget,
	Tttk_op		op,
	Tt_status	diagnosis,
	unsigned char  *contents,
	int             len,
	char	       *file,
	char	       *docname
)
{
	static const char *here = "CoEditor::loadISOLatin1_()";

	Tt_status status   = TT_OK;
	CoEditor *coEditor = 0;
	if (diagnosis != TT_OK) {
		// toolkit detected an error
		if (tt_message_status( msg ) == TT_WRN_START_MESSAGE) {
			//
			// Error is in start message!  We now have no
			// reason to live, so tell main() to exit().
			//
			abortCode = 2;
		}
		// let toolkit handle the error
		return msg;
	}
	if ((op == TTME_COMPOSE) && (file == 0)) {
		coEditor = new CoEditor( (Widget *)pWidget, msg, docname,
					 status );
	} else if (len > 0) {
		coEditor = new CoEditor( (Widget *)pWidget, msg,
					 (op == TTME_DISPLAY),
					 contents, len, docname, status );
	} else if (file != 0) {
		coEditor = new CoEditor( (Widget *)pWidget, msg,
					 (op == TTME_DISPLAY),
					 file, docname, status );
	} else {
		tttk_message_fail( msg, TT_DESKTOP_ENODATA, 0, 1 );
	}
	tt_free( (caddr_t)contents );
	tt_free( file );
	tt_free( docname );
	return 0;
}

void
CoEditor::_init()
{
	_baseFrame	= 0;
	_controls	= 0;
	_fileBut	= 0;
	_editBut	= 0;
	_text		= 0;
	_modifiedByMe	= FALSE;
	_modifiedByOther= 0;
	_contract	= 0;
	_subContract	= 0;
	_contractPats	= 0;
	_filePats	= 0;
	_file		= 0;
	_x		= INT_MAX;
	_y		= INT_MAX;
	_w		= INT_MAX;
	_h		= INT_MAX;
}

Tt_status
CoEditor::_init(
	Tt_message msg
)
{
	int width, height, xOffset, yOffset;
	width = height = xOffset = yOffset = INT_MAX;
	_contract = msg;
	ttdt_sender_imprint_on( 0, msg, 0, &_w, &_h, &_x, &_y,
				myContext, 10 * timeOutFactor );
	return TT_OK;
}

typedef enum {
	Open,
	Save,
	SaveAs,
	Revert,
	QuitChildren
} FileOp;

static const char *fileButs[] = {
	"Open...",
	"Save",
	"Save as...",
	"Revert",
	"Quit Children"
};

const int numFileButs = sizeof( fileButs ) / sizeof( const char * );

typedef enum {
	Undo,
	Cut,
	Copy,
	Paste,
	Delete,
	SelText
} EditOp;

static const char *editButs[] = {
	"Undo",
	"Cut",
	"Copy",
	"Paste",
	"Delete",
	"Text as alternate media type",
};

const int numEditButs = sizeof( editButs ) / sizeof( const char * );

void
CoEditor::_init(
	Widget *parent
)
{
	Arg args[20];
	Atom WM_DELETE_WINDOW;
	XmString label_str;
	int n;

	if (*parent == 0) {
		if (_contract != 0) {
			//
			// Re-open display, since $DISPLAY may have changed by
			// ttdt_sender_imprint_on().
			//
			XtCloseDisplay( myDpy );
			myDpy = XtOpenDisplay( myContext, 0, 0, "CoEd", 0, 0, 
					       pArgc, globalArgv );
		}
		*parent = XtAppCreateShell( 0, "CoEd",
				applicationShellWidgetClass, myDpy, 0, 0 );
		XtVaSetValues( *parent,
			       XtNmappedWhenManaged, False,
			       XtNheight, 1,
			       XtNwidth, 1,
			       0 );
		XtRealizeWidget( *parent );
	}
	shell = XtCreatePopupShell( "CoEd",
				applicationShellWidgetClass, *parent, 0, 0 );
	XtVaSetValues( shell, XmNuserData, this, 0 );
	// Pop up next to our parent
	if ((_x != INT_MAX) && (_y != INT_MAX) && (_w != INT_MAX)) {
		// XXX Be smarter about picking a geometry
		Dimension x	= _x + _w;
		Dimension y	= _y;
		XtVaSetValues( shell, XtNx, x, XtNy, y, 0 );
	}
	XtAddCallback( shell, XtNdestroyCallback, CoEditor::_destroyCB_,
		       this );
	WM_DELETE_WINDOW = XmInternAtom( XtDisplay(shell),
                                         "WM_DELETE_WINDOW", False );
	XmAddWMProtocolCallback( shell, WM_DELETE_WINDOW,
                            (XtCallbackProc) CoEditor::_wmProtocolCB_, this );
	XtVaSetValues( shell, XmNdeleteResponse, XmDO_NOTHING, 0 );
	_baseFrame = XtVaCreateManagedWidget(
				"baseFrame", xmFormWidgetClass, shell, 0 );

	n = 0;
        XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset,      5);             n++;
	XtSetArg(args[n], XmNrightOffset,     5);             n++;
	XtSetArg(args[n], XmNtopOffset,       5);             n++;
        _controls = XmCreateMenuBar(_baseFrame, "controls", args, n );

	_fileBut = XmCreatePulldownMenu(_controls, "File", NULL, 0 );
	label_str = XmStringCreateSimple("File");
        XtVaCreateManagedWidget("File", xmCascadeButtonWidgetClass, _controls,
                                XmNlabelString, label_str,
                                XmNmnemonic,    'F',
                                XmNsubMenuId,   _fileBut,
                                0);
	XmStringFree(label_str);
        for (int i = 0; i < numFileButs; i++) {
		Widget but = XtVaCreateManagedWidget( fileButs[i],
				xmPushButtonWidgetClass, _fileBut,
				XmNuserData, i, 0 );
		XtAddCallback( but, XmNactivateCallback,
                               CoEditor::_fileButsCB_, this );
	}

	_editBut = XmCreatePulldownMenu( _controls, "Edit", NULL, 0);
	label_str = XmStringCreateSimple("Edit");
        XtVaCreateManagedWidget("Edit", xmCascadeButtonWidgetClass, _controls,
                                XmNlabelString, label_str,
                                XmNmnemonic,    'E',
                                XmNsubMenuId,   _editBut,
                                0);
	for (i = 0; i < numEditButs; i++) {
		Widget but = XtVaCreateManagedWidget( editButs[i],
				xmPushButtonWidgetClass, _editBut,
				XmNuserData, i, 0 );
		XtAddCallback( but, XmNactivateCallback,
                               CoEditor::_editButsCB_, this );
	}
	XtManageChild(_controls);

	n = 0;
	XtSetArg(args[n], XmNeditMode,         XmMULTI_LINE_EDIT);  n++;
	XtSetArg(args[n], XmNresizable,        True);               n++;
	XtSetArg(args[n], XmNuserData,         this);               n++;
	XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
	XtSetArg(args[n], XmNtopWidget,        _controls);          n++;
	XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
	XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
	XtSetArg(args[n], XmNleftOffset,       5);                  n++;
	XtSetArg(args[n], XmNrightOffset,      5);                  n++;
	XtSetArg(args[n], XmNtopOffset,        5);                  n++;
	XtSetArg(args[n], XmNbottomOffset,     5);                  n++;
	XtSetArg(args[n], XmNrows,             24);                 n++;
	XtSetArg(args[n], XmNcolumns,          80);                 n++;
	_text = XmCreateScrolledText(_baseFrame, "text", args, n);

	XtManageChild(_text);
	XtRealizeWidget( shell );
	XtPopup( shell, XtGrabNone );
	if (numEditors < MaxEditors) {
		for (int i = 0; i < MaxEditors; i++) {
			if (editors[i] == 0) {
				editors[i] = this;
				break;
			}
		}
		numEditors++;
	}
	if (numEditors >= maxBuffers) {
		tt_ptype_undeclare( "Sun_CoEd" );
	}
}

Tt_status
CoEditor::_unload()
{
	Tt_status status = TT_OK;
	if (_filePats != 0) {
		status = ttdt_file_quit( _filePats, 1 );
		_filePats = 0;
	}
	if (_file != 0) {
		free( _file );
		_file = 0;
	}
	return status;
}

Tt_status
CoEditor::_load(
	const char *file
)
{
	int reloading = 1;
	if (file != 0) {
		if ((_file != 0) && (strcmp( file, _file ) != 0)) {
			reloading = 0;
			_unload();
		} else {
			_file = strdup( file );
		}
	}
	// Can be called recursively, below
	if (_filePats == 0) {
		_filePats = ttdt_file_join( _file, TT_SCOPE_NONE, 1,
					    CoEditor::_fileCB_, this );
	}
	_read_file(_text, _file);
	XtAddCallback(_text, XmNvalueChangedCallback,
                   (XtCallbackProc) CoEditor::_textUpdateCB_, (caddr_t)this );
	if (_modifiedByMe && reloading) {
		ttdt_file_event( _contract, TTDT_REVERTED, _filePats, 1 );
	}
	_modifiedByMe = 0;
	XtSetSensitive( shell, False );
	_modifiedByOther = ttdt_Get_Modified( _contract, _file, TT_BOTH,
					      myContext, 10 * timeOutFactor );
	XtSetSensitive( shell, True );
	if (_modifiedByOther) {
		int choice = userChoice( myContext, _baseFrame,
				"Another tool has modifications pending for "
				"this file.\nDo you want to ask it to save "
				"or revert the file?", 3, "Save", "Revert",
				"Ignore" );
		Tt_status status = TT_OK;
		switch (choice) {
		    case 0:
			status = ttdt_Save( _contract, _file, TT_BOTH,
					    myContext, 10 * timeOutFactor );
			break;
		    case 1:
			status = ttdt_Revert( _contract, _file, TT_BOTH,
					      myContext, 10 * timeOutFactor );
			break;
		}
		if (status != TT_OK) {
			char *s = tt_status_message( status );
			userChoice( myContext, _baseFrame, s, 1, "Okay" );
			tt_free( s );
		} else if (choice == 0) {
			// file was saved, so reload
			return _load( 0 );
		} else if (choice == 1) {
			// file was reverted
			_modifiedByOther = 0;
		}
	}
	return TT_OK;
}

// Read in the contents of a file to the text widget.
//
// XXX: Needs to be made more robust, and handle large files.

Tt_status
CoEditor::_read_file(
	Widget	widget,
	char	*file
)
{
	char buf[BUFSIZ], *text;
	struct stat statb;
	int len;
	FILE *fp;
	Tt_status status;

// Make sure the file is a regular text file and open it.

	if (stat(file, &statb) == -1 || (statb.st_mode & S_IFMT) != S_IFREG ||
	    !(fp = fopen(file, "r"))) {
		perror(file);
		return TT_ERR_FILE;
	}

// Put the contents of the file in the Text widget by allocating enough
// space for the entire file, reading the file into the space, and using
// XmTextSetString() to show the file.

	len = (int) statb.st_size;
	if (!(text = XtMalloc((unsigned)(len+1)))) {
		sprintf(buf, "%s: XtMalloc(%ld) failed", file, len);
		XmTextSetString(widget, buf);
	} else {
		if (fread(text, sizeof(char), len, fp) != len) {
			status = TT_ERR_FILE;
		} else {
			status = TT_OK;
			text[len] = 0;
			XmTextSetString(widget, text);
		}
	}
	XtFree(text);
	fclose(fp);
	return status;
}


// Write the contents of a text widget to a file.

Tt_status
CoEditor::_write_file(
	Widget  widget,
	char    *file
)
{
	char *text;
	int len;
	FILE *fp;
	Tt_status status;

	if (!(fp = fopen(file, "w"))) {
		perror(file);
		return TT_ERR_FILE;
	}

// Saving -- get text from the text widget.

	text = XmTextGetString(widget);
	len = (int) XmTextGetLastPosition(widget);

// Write it to file (check for error).

	if (fwrite(text, sizeof(char), len, fp) != len) {
		status = TT_ERR_FILE;
	} else {
		status = TT_OK;

// Make sure a newline terminates file.

		if (text[len-1] != '\n') {
			fputc('\n', fp);
		}
	}
	fclose(fp);
	return status;
}


Tt_status
CoEditor::_load(
	unsigned char  *contents,
	int		//len
)
{
	_unload();
	_read_file(_text, (char *) contents);
	XtAddCallback(_text, XmNvalueChangedCallback,
                   (XtCallbackProc) CoEditor::_textUpdateCB_, (caddr_t)this );
	_modifiedByMe = 0;
	_modifiedByOther = 0;
	return TT_OK;
}

//
// Caller responsible for reporting any errors to user
//
Tt_status
CoEditor::_save()
{
	Tt_status status;
	if (_file != 0) {
		if (_write_file(_text, _file) != TT_OK) {
			return TT_DESKTOP_EIO;
		}
		_modifiedByMe = 0;
		_modifiedByOther = 0;
		ttdt_file_event( _contract, TTDT_SAVED, _filePats, 1 );
	}
	if (_contract != 0) {
		int   len	= 0;
		char *contents	= 0;
		if (_file == 0) {
			// If you worry that the buffer might be big,
			// you could instead try a a temp file to
			// transfer the data "out of band".
			contents = _contents( &len );
		}
		status = ttmedia_Deposit( _contract, 0, myAltMediaType,
					  (unsigned char *)contents,
					  len, _file, myContext,
					  10 * timeOutFactor );
		if (status != TT_OK) {
			return status;
		}
		_modifiedByMe = 0;
		_modifiedByOther = 0;
		if (contents != 0) {
			XtFree( contents );
		}
	}
	return status;
}

Tt_status
CoEditor::_revert()
{
	//if (! _modifiedByMe) {
	//	return TT_OK;
	//}
	return _load( 0 ); // XXX what if it's not a file? keep last deposit
}

void
CoEditor::_destroyCB_(
	Widget    w,
	XtPointer coEditor,
	XtPointer call_data
)
{
	((CoEditor *)coEditor)->_destroyCB( w, call_data );
}

void
CoEditor::_destroyCB(
	Widget    ,
	XtPointer //call_data
)
{
	// No, we delete this whenever we destroy our shell.
	// We hope that Xt does not call us
	//delete this;
}

static Tt_callback_action
_ttdtQuitCB(
	Tt_message msg,
	Tt_pattern
)
{
	Tt_status replyStatus;
	switch (tt_message_state(msg)) {
	    case TT_HANDLED:
		// The Quit occurred.
		replyStatus = TT_OK;
		break;
	    case TT_FAILED:
		// The Quit did not occur.  Find out why.
		replyStatus = (Tt_status)tt_message_status( msg );
		if (replyStatus == TT_OK) {
			// No reason given!  Assume canceled.
			replyStatus = TT_DESKTOP_ECANCELED;
		}
		break;
	    default:
		// Not in final state; our address space is probably handler
		return TT_CALLBACK_CONTINUE;
	}
	// Wind down the counter so that tttk_block_while() will return.
	int *pWaiting = (int *)tt_message_user( msg, 0 );
	*pWaiting = 0;
	// Save the reply status.
	tt_message_user_set( msg, 1, (void *)replyStatus );
	return TT_CALLBACK_PROCESSED;
}

static Tt_status
ttdt_Quit(
	Tt_message	toQuit,
	const char     *handler,
	int		silent,
	int		force,
	XtAppContext	context,
	int		ms_timeout
)
{
	Tt_message msg = tttk_message_create( toQuit, TT_REQUEST, TT_SESSION,
					      handler, "Quit", _ttdtQuitCB );
	tt_message_iarg_add( msg, TT_IN, Tttk_boolean, silent );
	tt_message_iarg_add( msg, TT_IN, Tttk_boolean, force );
	char *id = tt_message_id( toQuit );
	tt_message_arg_add( msg, TT_IN, Tttk_message_id, id );
	tt_free( id );
	Tt_status status = tt_message_send( msg );
	if (status != TT_OK) {
		tt_message_destroy( msg );
		return status;
	}
	int waiting = 1;
	tt_message_user_set( msg, 0, &waiting );
	status = tttk_block_while( context, &waiting, ms_timeout );
	if (status != TT_OK) {
		tttk_message_destroy( msg );
		return status;
	}
	//
	// _ttdtQuitCB() stored the reply status under key 1
	//
	void *replyStatus = tt_message_user( msg, 1 );
	status = tt_ptr_error( replyStatus );
	if (status != TT_OK) {
		return status;
	}
	return (Tt_status)replyStatus;
}

//
// Returns:
//	0	canceled
//	1	destructor has been called!
//
int
CoEditor::_quit(
	int silent,
	int force
)
{
	int choice = 0;
	if (_modifiedByMe && (! silent)) {
		if (force) {
			choice = userChoice( myContext, _baseFrame,
					     "The text has unsaved changes.",
					     2, "Save, then Quit",
					     "Discard, then Quit" );
		} else {
			choice = userChoice( myContext, _baseFrame,
					     "The text has unsaved changes.",
					     3, "Save, then Quit",
					     "Discard, then Quit",
					     "Cancel" );
		}
	}
	Tt_status status;
	char *subContractor;
	if (_subContract != 0) {
		subContractor = (char *)tt_message_user( _subContract, 0 );
	}
	switch (choice) {
	    case 0: // "Save, then Quit"
		if (_subContract == 0) {
			// Destructor will _save() and reply to _contract
			break;
		}
		// Synchronously quit outstanding actions
		status = ttdt_Quit( _subContract, subContractor,
				    silent, force, myContext,
				    60 * timeOutFactor );
		if ((! force) && (status == TT_DESKTOP_ECANCELED)) {
			return 0;
		}
		if (status != TT_OK) {
			_adviseUser( "ttdt_Quit()", status );
		}
		// Destructor will _save() and reply to _contract
		break;
	    case 1: // "Discard, then Quit"
		if (_subContract != 0) {
			// Asynchronously quit outstanding actions
			status = ttdt_Quit( _subContract, subContractor,
					    silent, force, myContext, 0 );
			if (   (status != TT_OK)
			    && (status != TT_DESKTOP_ETIMEDOUT))
			{
				_adviseUser( "ttdt_Quit()", status );
			}
		}
		if (_contract != 0) {
			tttk_message_fail( _contract, TT_DESKTOP_ECANCELED,
					   0, 1 );
			_contract = 0;
		}
		_unload();
		break;
	    case 2: // "Cancel"
		return 0;
	}
	delete this;
	XtDestroyWidget( shell );
	return 1;
}

int
CoEditor::_quitAll(
	int silent,
	int force
)
{
	for (int i = 0; i < MaxEditors; i++) {
		if (editors[i] == 0) {
			continue;
		}
		if (! editors[i]->_quit( silent, force )) {
			return 0;
		}
	}
}

void
CoEditor::_wmProtocolCB_(
	Widget    ,
	XtPointer coEditor,
	XmAnyCallbackStruct *
)
{
	((CoEditor *)coEditor)->_wmProtocolCB();
}

void
CoEditor::_wmProtocolCB()
{
	if (! _quit()) {
		return;
	}
	/*
	if (numEditors > 1) {
		// More windows; just destroy this one
		XtDestroyWidget( shell );
		delete this;
	}
	*/
}

void
CoEditor::_fileButsCB_(
	Widget    button,
	XtPointer coEditor,
	XtPointer call_data
)
{
	((CoEditor *)coEditor)->_fileButsCB( button, call_data );
}

void
CoEditor::_fileButsCB(
	Widget    button,
	XtPointer //call_data
)
{
	FileOp op;
	XtVaGetValues( button, XmNuserData, &op, 0 );
	Tt_status status = TT_OK;
	switch (op) {
	    case Open:
		break;
	    case Revert:
		status =_revert();
		break;
	    case Save:
		status =_save();
		break;
	    case SaveAs:
		break;
	    case QuitChildren:
		if (_subContract == 0) {
			return;
		}
		status = ttdt_Quit( _subContract,
				    (char *)tt_message_user( _subContract, 0 ),
				    0, 0, myContext, 60 * timeOutFactor );
		if (status == TT_DESKTOP_ECANCELED) {
			return;
		}
		if (status != TT_OK) {
			_adviseUser( "ttdt_Quit()", status );
		}
		return;
	}
	if (status != TT_OK) {
		_adviseUser( 0, status );
	}
}

void
CoEditor::_editButsCB_(
	Widget    button,
	XtPointer coEditor,
	XtPointer call_data
)
{
	((CoEditor *)coEditor)->_editButsCB( button, call_data );
}

void
CoEditor::_editButsCB(
	Widget    button,
	XtPointer //call_data
)
{
	EditOp op;
	XtVaGetValues( button, XmNuserData, &op, 0 );
	Tt_status status = TT_OK;
	switch (op) {
		int		len;
		char	       *contents;
		const char     *mediaType;
		Tt_message	msg;
	    case SelText:
		if (_subContract != 0) {
			userChoice( myContext, _baseFrame,
				    "Already editing buffer in another editor",
				    1, "Okay" );
			return;
		}
		//contents = _selection( &len );
		contents = _contents( &len );
		if (len <= 0) {
			return;
		}
		msg = ttmedia_load( _contract, CoEditor::_mediaLoadMsgCB_,
				    this, TTME_EDIT, myAltMediaType,
				    (unsigned char *)contents, len, 0, 0, 1 );
		if (contents != 0) {
			XtFree( contents );
		}
		status = tt_ptr_error( msg );
		if (status != TT_OK) {
			break;
		}
		ttdt_subcontract_manage( msg, CoEditor::subContractCB_,
					 shell, this );
		_subContract = msg;
		break;
	}
	if (status != TT_OK) {
		char *s = tt_status_message( status );
		char buf[ 1024 ];
		sprintf( buf, "%d: %s", status, s );
		tt_free( s );
		userChoice( myContext, _baseFrame, buf, 1, "Okay" );
	}
}

char *
CoEditor::_contents(
	int *len
)
{
	char *contents = XmTextGetString(_text);

	*len = 0;
	if (contents != 0) {
		*len = strlen( contents );
	}
	return contents;
}

Tt_status
CoEditor::_acceptContract(
	Tt_message msg
)
{
	static const char *here = "CoEditor::_acceptContract()";

	_contract = msg;
	if (tt_message_status( msg ) == TT_WRN_START_MESSAGE) {
		//
		// Join session before accepting start message,
		// to prevent unnecessary starts of our ptype
		//
		Widget session_shell = shell;
		if (maxBuffers > 1) {
			//
			// If we are in multi-window mode, just use
			// our unmapped toplevel shell as our session
			// shell, since we do not know if any particular
			// window will exist the whole time we are in
			// the session.
			//
			session_shell = XtParent(shell );
		}
		sessPats = ttdt_session_join( 0, CoEditor::contractCB_,
					      session_shell, this, 1 );
	}
	_contractPats = ttdt_message_accept( msg, CoEditor::contractCB_,
					     shell, this, 1, 1 );
	Tt_status status = tt_ptr_error( _contractPats );
	if (status != TT_OK) {
		return status;
	}
	return status;
}

Tt_message
CoEditor::contractCB_(
	Tt_message	msg,
	void	       *coEditor,
	Tt_message	contract
)
{
	return ((CoEditor *)coEditor)->_contractCB( msg, contract );
}

Tt_message
CoEditor::_contractCB(
	Tt_message	msg,
	Tt_message	contract
)
{
	if ((contract != 0) && (contract != _contract)) {
		tttk_message_fail( msg, TT_DESKTOP_ENOMSG, 0, 1 );
		return 0;
	}
	char *opString = tt_message_op( msg );
	Tttk_op op = tttk_string_op( opString );
	tt_free( opString );
	int silent = 0;
	int force  = 0;
	int cancel = 0;
	char *status;
	Boolean sensitive = True;
	switch (op) {
	    case TTDT_QUIT:
		tt_message_arg_ival( msg, 0, &silent );
		tt_message_arg_ival( msg, 1, &force );
		if (contract == 0) {
			if (! _quitAll( silent, force )) {
				cancel = 1;
			}
		} else {
			if (! _quit( silent, force )) {
				cancel = 1;
			}
		}
		if (cancel) {
			tttk_message_fail( msg, TT_DESKTOP_ECANCELED, 0, 1 );
		} else {
			tt_message_reply( msg );
			tttk_message_destroy( msg );
		}
		return 0;
	    case TTDT_GET_STATUS:
		if (contract == 0) {
			if (numEditors < maxBuffers) {
				status = "Awaiting requests to open more "
					 "buffers";
			} else {
				status = "Already have maximum open buffers";
			}
		} else {
			status = "Request in progress";
		}
		tt_message_arg_val_set( msg, 0, status );
		tt_message_reply( msg );
		tttk_message_destroy( msg );
		return 0;
	    case TTDT_PAUSE:
		sensitive = False;
	    case TTDT_RESUME:
		if (contract == 0) {
			int already = 1;
			for (int i = 0; i < MaxEditors; i++) {
				if (editors[i] == 0) {
					continue;
				}
				if (   XtIsSensitive( editors[i]->shell )
				    != sensitive)
				{
					already = 0;
					XtSetSensitive( editors[i]->shell,
							sensitive );
				}
			}
			if (already) {
				tt_message_status_set(msg,TT_DESKTOP_EALREADY);
			}
		} else {
			if (XtIsSensitive( shell ) == sensitive) {
				tt_message_status_set(msg,TT_DESKTOP_EALREADY);
			} else {
				XtSetSensitive( shell, sensitive );
			}
		}
		tt_message_reply( msg );
		tttk_message_destroy( msg );
		return 0;
	    case TTDT_DO_COMMAND:
		tttk_message_fail( msg, TT_DESKTOP_ENOTSUP, 0, 1 );
		return 0;
	}
	return msg;
}

Tt_message
CoEditor::subContractCB_(
	Tt_message	msg,
	void	       *coEditor,
	Tt_message	subContract
)
{
	return ((CoEditor *)coEditor)->_subContractCB( msg, subContract );
}

Tt_message
CoEditor::_subContractCB(
	Tt_message	msg,
	Tt_message	subContract
)
{
	if ((subContract != 0) && (subContract != _subContract)) {
		tttk_message_fail( msg, TT_DESKTOP_ENOMSG, 0, 1 );
		return 0;
	}
	char *opString = tt_message_op( msg );
	Tttk_op op = tttk_string_op( opString );
	tt_free( opString );
	char *subContractor;
	Tt_status status;
	switch (op) {
	    case TTDT_STATUS:
		// Save the procid who is working on the subContract
		subContractor = tt_message_sender( msg );
		status = tt_ptr_error( subContractor );
		if (status == TT_OK) {
			tt_message_user_set( subContract, 0, subContractor );
		}
		tttk_message_destroy( msg );
		msg = 0;
		break;
	}
	return msg;
}

Tt_message
CoEditor::_mediaLoadMsgCB_(
	Tt_message	msg,
	void           *clientData,
	Tttk_op		op,
	unsigned char  *contents,
	int             len,
	char           *file
)
{
	return ((CoEditor *)clientData)->_mediaLoadMsgCB( msg, op,
						contents, len, file );
}

Tt_message
CoEditor::_mediaLoadMsgCB(
	Tt_message	msg,
	Tttk_op		op,
	unsigned char  *contents,
	int		len,
	char           *file
)
{
	Tt_state state = tt_message_state(msg);
	Tt_status status = (Tt_status)tt_message_status(msg);
	if (state == TT_FAILED) {
		if (status == TT_DESKTOP_ECANCELED) {
			// XXX gently say so in window footer
		} else {
			_adviseUser( "_mediaLoadMsgCB()", status );
		}
	} else {
		if (len > 0) {
			XmTextSetString(_text, (char *) contents);
			XtAddCallback(_text, XmNvalueChangedCallback,
				      (XtCallbackProc)CoEditor::_textUpdateCB_,
				      (caddr_t)this );
			// OLIT equivalent: ReplaceBlockInTextBuffer
		} else if (file != 0) {
			// XXX slurp in file
		}
	}
	if (op == TTME_DEPOSIT) {
		tt_message_reply( msg );
	}
	if (   (msg == _subContract)
	    && (state == TT_HANDLED || state == TT_FAILED))
	{
		_subContract = 0;
	}
	tttk_message_destroy( msg );
	return 0;
}

void
CoEditor::_textUpdateCB_(
	Widget				coEditor,
	XtPointer			call_data,
	XmTextVerifyCallbackStruct	*
)
{
	if (coEditor == 0) {
		return;
	}
	((CoEditor *)call_data)->_textUpdateCB();
}

void
CoEditor::_textUpdateCB()
{
	//Tt_status status;
	if (! _modifiedByMe) {
		_modifiedByMe = TRUE;
		ttdt_file_event( _contract, TTDT_MODIFIED, _filePats, 1 );
	}
}

Tt_message
CoEditor::_fileCB_(
	Tt_message	msg,
	Tttk_op		op,
	char           *pathname,
	void	       *coEditor,
	int		trust,
	int		me
)
{
	tt_free( pathname );
	if (coEditor == 0) {
		return msg;
	}
	return ((CoEditor *)coEditor)->_fileCB( msg, op, pathname,
						trust, me );
}

Tt_message
CoEditor::_fileCB(
	Tt_message	msg,
	Tttk_op		op,
	char	       *pathname,
	int,		//trust
	int		me
)
{
	tt_free( pathname );
	Tt_status status = TT_OK;
	switch (op) {
	    case TTDT_MODIFIED:
		if (_modifiedByMe) {
			// Hmm, the other editor either doesn't know or
			// doesn't care that we are already modifying the
			// file, so the last saver will win.
			// XXX Or: a race condition has arisen!
		} else {
			// Interrogate user if she ever modifies the buffer
			_modifiedByOther = 1;
			XtAddCallback( (Widget)_text, XmNmodifyVerifyCallback,
                                (XtCallbackProc)CoEditor::_textModifyCB_, 0 );
		}
		break;
	    case TTDT_GET_MODIFIED:
		tt_message_arg_ival_set( msg, 1, _modifiedByMe );
		tt_message_reply( msg );
		break;
	    case TTDT_SAVE:
		status = _save();
		if (status == TT_OK) {
			tt_message_reply( msg );
		} else {
			tttk_message_fail( msg, status, 0, 0 );
		}
		break;
	    case TTDT_REVERT:
		status = _revert();
		if (status == TT_OK) {
			tt_message_reply( msg );
		} else {
			tttk_message_fail( msg, status, 0, 0 );
		}
		break;
	    case TTDT_REVERTED:
		if (! me) {
			_modifiedByOther = 0;
		}
		break;
	    case TTDT_SAVED:
		if (! me) {
			_modifiedByOther = 0;
			int choice = userChoice( myContext, _baseFrame,
						 "Another tool has saved "
						 "this file.", 2, "Ignore",
						 "Revert" );
			switch (choice) {
			    case 1:
				_revert();
				break;
			}
		}
		break;
	    case TTDT_MOVED:
	    case TTDT_DELETED:
		printf( "CoEditor::_fileCB(): %s\n", tttk_op_string( op ));
		break;
	}
	tttk_message_destroy( msg );
	return 0;
}

void
CoEditor::_textModifyCB_(
	Widget		text,
	XtPointer	,
	XmTextVerifyCallbackStruct *mod
)
{
	CoEditor *coEditor = 0;
	XtVaGetValues( text, XmNuserData, &coEditor, 0 );
	if (coEditor == 0) {
		return;
	}
	coEditor->_textModifyCB( mod );
}

void
CoEditor::_textModifyCB(
	XmTextVerifyCallbackStruct *mod
)
{
	if (_modifiedByOther != 1) {
		return;
	}
	int cancel = userChoice( myContext, _baseFrame,
		       "Another tool has modifications pending for this file.\n"
		       "Are you sure you want to start modifying the file?",
		       2, "Modify", "Cancel" );
	if (cancel) {
		mod->doit = FALSE;
	}
	_modifiedByOther = 2;
}

void
CoEditor::_adviseUser(
	const char *msg,
	Tt_status status
)
{
	char *s = tt_status_message( status );
	char buf[ 1024 ];
	if (msg != 0) {
		sprintf( buf, "%s: %d: %s", msg, status, s );
	} else {
		sprintf( buf, "%d: %s", status, s );
	}
	tt_free( s );
	userChoice( myContext, _baseFrame, buf, 1, "Okay" );
}
