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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $TOG: cntl.c /main/4 1998/05/18 16:55:06 rafi $ 			 				 */
/*
 * cntl.c
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

/* 
 * An example controller for the remote-control editor defined in edit.c.
 * Puts up a simple panel with a text field to enter the filename to
 * operate on and sends out ToolTalk messages to start up an editor to
 * carry out the operations. Also allows browsing any sub-file specs
 * defined in the file.
 */
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/CascadeB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/Protocols.h>
#include <desktop/tt_c.h>

#define  TITLE_LINE_HEIGHT  25

Display *dpy;
XtAppContext app;

Widget	cntl_ui_base_window;
Widget	cntl_ui_base_controls;
Widget	cntl_ui_file_button;
Widget  cntl_ui_file_label;
Widget	cntl_ui_file_field;
Widget	cntl_ui_message;
Widget	cntl_ui_file_menu;
 
Widget	cntl_ui_saveas_popup;
Widget	cntl_ui_saveas_controls;
Widget	cntl_ui_save_as_dlabel;
Widget	cntl_ui_save_as_directory;
Widget	cntl_ui_new_flabel;
Widget	cntl_ui_new_filename;
Widget	cntl_ui_save_as_button;
 
Widget	cntl_ui_obj_popup;
Widget	cntl_ui_obj_controls;
Widget	cntl_ui_olist_label;
Widget	cntl_ui_olist;
Widget	cntl_ui_hilite_button;
Widget	cntl_ui_obj_field;

char		*cntl_objid = (char *)0;

Tt_scope	msg_scope = TT_FILE_IN_SESSION;

void
main(argc, argv)
	int		argc;
	char		**argv;
{
	void		cntl_ui_initialize();

	/*
	 * Initialize Motif.
	 */
	XtToolkitInitialize();
        app = XtCreateApplicationContext();
        dpy = XtOpenDisplay(app, 0, 0, "cntl", 0, 0, &argc, argv);
	/*
	 * Initialize user interface components.
	 */
	cntl_ui_initialize();
	
	if (! cntl_init_tt()) {
		fprintf(stderr,"%s: Can't initialize ToolTalk\n", argv[0]);
		exit(1);
	}
	/*
	 * Turn control over to Motif.
	 */
	XtAppMainLoop(app);
	tt_close();
	exit(0);
}


int
is_window_showing(widget)
	Widget widget;
{
	return(XtIsManaged(widget)) ;
}


void
get_screen_size(widget, width, height)
	Widget widget;
	int *width, *height;
{
	Display *dpy = XtDisplay(widget);
	int screen   = DefaultScreen(dpy);

	*width  = DisplayWidth(dpy, screen);
	*height = DisplayHeight(dpy, screen);
}


void
force_popup_on_screen(popup, px, py)
	Widget popup;
	int *px, *py;
{
	Dimension popup_width, popup_height;
	Position left, top;
	int n, x, y, screen_width, screen_height;

	x = *px;
	y = *py;

/* Get the screen size. */

	get_screen_size(popup, &screen_width, &screen_height);

	XtVaGetValues(popup,
                      XmNwidth,  &popup_width,
                      XmNheight, &popup_height,
                      0);
 
/* Make sure frame does not go off side of screen. */
 
	n = x + (int) popup_width;
	if (n > screen_width) x -= (n - screen_width);
	else if (x < 0) x = 0;
 
/* Make sure frame doen't go off top or bottom. */
 
	n = y + (int) popup_height;
	if (n > screen_height) y -= n - screen_height;
	else if (y < 0) y = 0;
 
/* Set location and return. */

	left = (Position) x;
	top  = (Position) y;
	XtVaSetValues(popup,
                      XmNx, left,
                      XmNy, top,
                      0);

	*px = x;
	*py = y;
}


void
position_popup(base, popup)
	Widget base, popup;
{
	int bw, bx, by, px, py;
	int screen_width, screen_height;
	Position base_x, base_y, popup_x, popup_y;
	Dimension base_width, base_height, popup_width, popup_height;

	XtVaGetValues(base,
                      XmNx,      &base_x,
                      XmNy,      &base_y,
                      XmNwidth,  &base_width,
                      XmNheight, &base_height,
                      0);
	bx = (int) base_x;
	by = (int) base_y;
	bw = (int) base_width;

	XtVaGetValues(popup,
                      XmNx,      &popup_x,
                      XmNy,      &popup_y,
                      XmNwidth,  &popup_width,
                      XmNheight, &popup_height,
                      0);
 
	px = (int) popup_x;
	py = (int) popup_y;
 
	get_screen_size(popup, &screen_width, &screen_height);
	px = bx + bw + 5;
	py = by - TITLE_LINE_HEIGHT;
	force_popup_on_screen(popup, &px, &py);
}


void
show_popup(widget)
	Widget widget;
{
	if (!is_window_showing(widget))
		position_popup(cntl_ui_base_window, widget) ;
	XtMapWidget(widget);
	XtManageChild(widget);
	XtPopup(XtParent(widget), XtGrabNone);
}


void
dismiss_popup(widget, client_data, cbs)
	Widget widget;
	XtPointer client_data;
	XmAnyCallbackStruct *cbs;
{
	XtUnmapWidget(widget);
	XtUnmanageChild(widget);
}


void
add_delete_callback(widget)
	Widget widget;
{
	Atom WM_DELETE_WINDOW;

	WM_DELETE_WINDOW = XmInternAtom(XtDisplay(widget),
                                        "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(widget, WM_DELETE_WINDOW,
                                (XtCallbackProc) dismiss_popup, NULL);
}


void
write_footer(message)
	char *message;
{
	XmString label;

	label = XmStringCreateSimple(message);
	XtVaSetValues(cntl_ui_message, XmNlabelString, label, 0);
	XmStringFree(label);
}


/* 
 * Initialize our ToolTalk environment.
 */
int
cntl_init_tt()
{
	char			*procid = tt_open();
	int			ttfd;
	void			cntl_receive_tt_message();
	Tt_pattern		pat;
	Tt_callback_action	cntl_update_obj_panel_callback();


	if (tt_pointer_error(procid) != TT_OK) {
		return 0;
	}
	ttfd = tt_fd();

	/* register a dynamic pattern to catch whenever the remote */
	/* editor creates a sub-file object so that we can update our */
	/* object query panel. */

	pat = tt_pattern_create();
	tt_pattern_op_add(pat, "Sun_EditDemo_new_object");
	tt_pattern_scope_add(pat, msg_scope);
	tt_pattern_category_set(pat, TT_OBSERVE);
	tt_pattern_callback_add(pat, cntl_update_obj_panel_callback);
	if (tt_pattern_register(pat) != TT_OK) {
		return 0;
	}

	XtAppAddInput(app, ttfd, (XtPointer) XtInputReadMask,
                      cntl_receive_tt_message, 0);

	tt_session_join(tt_default_session());

	tt_free(procid);
	return 1;
}



/* 
 * Handle any incoming ToolTalk messages. Since all the messages we send
 * out have callback functions, no further action is required after doing
 * a tt_message_receive.
 */
void
cntl_receive_tt_message(client_data, fid, id)
	XtPointer client_data;
	int *fid;
	XtInputId *id;
{
	Tt_message		msg;

	msg = tt_message_receive();
	if (msg != NULL &&  tt_pointer_error(msg) == TT_ERR_NOMP) {
		fprintf(stderr,"ToolTalk server down.\n");
		exit(0);
	}
}


/* 
 * Callback for our dynamic pattern informing us when an object has
 * been added to a file.  We use this information to update the file
 * objects panel if it is up.
 */
Tt_callback_action
cntl_update_obj_panel_callback(m, p)
     Tt_message m;
     Tt_pattern p;
{
	int 	mark;

	mark = tt_mark();

	if (cntl_is_same_file(tt_message_file(m),
                              XmTextGetString(cntl_ui_file_field))) {

		/* update the objects panel if it's showing */
		/* objects for the same file as the one in the */
		/* message. */
		cntl_update_obj_panel();
	}

	/*
	 * no further action required for this message. Destroy it
	 * and return TT_CALLBACK_PROCESSED so no other callbacks will
	 * be run for the message.
	 */
	tt_message_destroy(m);
	tt_release(mark);
	return TT_CALLBACK_PROCESSED;
}


/* 
 * Default callback for all the ToolTalk messages we send.
 */
Tt_callback_action
cntl_msg_callback(m, p)
     Tt_message m;
     Tt_pattern p;
{
	int		mark;
	char		msg[255];
	char		*errstr;


	mark = tt_mark();
	switch (tt_message_state(m)) {
	      case TT_STARTED:
		write_footer("Starting editor...");
		break;
	      case TT_HANDLED:
		write_footer("");
		break;
	      case TT_FAILED:
		errstr = tt_message_status_string(m);
		if (tt_pointer_error(errstr) == TT_OK && errstr) {
			sprintf(msg,"%s failed: %s", tt_message_op(m), errstr);
		} else if (tt_message_status(m) == TT_ERR_NO_MATCH) {
			sprintf(msg,"%s failed: Couldn't contact editor",
				tt_message_op(m),
				tt_status_message(tt_message_status(m)));
		} else {
			sprintf(msg,"%s failed: %s",
				tt_message_op(m),
				tt_status_message(tt_message_status(m)));
		}
		write_footer(msg);
		break;
	      default:
		break;
	}

	/*
	 * no further action required for this message. Destroy it
	 * and return TT_CALLBACK_PROCESSED so no other callbacks will
	 * be run for the message.
	 */
	tt_message_destroy(m);
	tt_release(mark);
	return TT_CALLBACK_PROCESSED;
}


/* 
 * Handle the edit actions by sending out a message to Sun_EditDemo to
 * start an editing session.
 */
void
cntl_edit(file)
     char *file;
{
	Tt_message	msg;

	write_footer("");
	msg = tt_prequest_create(msg_scope, "Sun_EditDemo_edit");
	tt_message_file_set(msg, file);
	tt_message_callback_add(msg, cntl_msg_callback);
	tt_message_send(msg);
}


/* 
 * Handle the save actions by sending out a message to the Sun_EditDemo
 * editing this file to save the file.
 */
void
cntl_save(file)
     char *file;
{
	Tt_message	msg;

	write_footer("");
	msg = tt_prequest_create(msg_scope, "Sun_EditDemo_save");
	tt_message_file_set(msg, file);
	tt_message_callback_add(msg, cntl_msg_callback);
	tt_message_send(msg);
}



/* 
 * Shut down the Sun_EditDemo that is editing this file.
 */
void
cntl_close(file)
     char *file;
{
	Tt_message	msg;

	write_footer("");
	msg = tt_prequest_create(msg_scope, "Sun_EditDemo_close");
	tt_message_file_set(msg, file);
	tt_message_callback_add(msg, cntl_msg_callback);
	tt_message_send(msg);
}


/* 
 * Instruct the Sun_EditDemo that is editing this file to save the file
 * under a new name.
 */
void
cntl_save_as(file, dir, newfile)
     char *file;
     char *dir;
     char *newfile;
{
	Tt_message	msg;
	char		buf[255];
	int		dirlen;

	msg = tt_prequest_create(msg_scope, "Sun_EditDemo_save_as");

	/* construct new filename out of directory and new file */
	/* fields. */

	dirlen = strlen(dir);
	if (dirlen > 0 && dir[dirlen - 1] == '/') {
		sprintf(buf,"%s%s", dir, newfile);
	} else {
		sprintf(buf,"%s/%s", dir, newfile);
	}

	tt_message_arg_add(msg, TT_IN, "string", buf);
	tt_message_file_set(msg, file);
	tt_message_callback_add(msg, cntl_msg_callback);
	tt_message_send(msg);
}



/*
 * Handler for `file_menu (Edit)'.
 */
void
cntl_ui_edit(widget, client_data, call_data)
	Widget widget;
	XtPointer client_data, call_data;
{
	write_footer("");
	cntl_edit(XmTextGetString(cntl_ui_file_field));
}

/*
 * Handler for `file_menu (Save)'.
 */
void
cntl_ui_save(widget, client_data, call_data)
	Widget widget;
	XtPointer client_data, call_data;
{
	write_footer("");
	cntl_save(XmTextGetString(cntl_ui_file_field));
}

/*
 * Handler for `file_menu (Save as...)'.
 */
cntl_ui_save_as(widget, client_data, call_data)
	Widget widget;
	XtPointer client_data, call_data;
{
	write_footer("");
	show_popup(cntl_ui_saveas_popup);
}

/*
 * Handler for `file_menu (Close)'.
 */
cntl_ui_close(widget, client_data, call_data)
	Widget widget;
	XtPointer client_data, call_data;
{
	write_footer("");
	cntl_close(XmTextGetString(cntl_ui_file_field));
}

/*
 * Callback function for `save_as_button'.
 */
void
cntl_ui_save_as_button_handler(widget, client_data, call_data)
	Widget widget;
	XtPointer  client_data, call_data;
{
	write_footer("");
	cntl_save_as(XmTextGetString(cntl_ui_file_field),
                     XmTextGetString(cntl_ui_save_as_directory),
                     XmTextGetString(cntl_ui_new_filename));
}

/* 
 * Function to insert the objid given into the scrolling lists of objects
 * for a file. Used inside tt_file_objects_query as it iterates through
 * all the ToolTalk objects in a file.
 */
Tt_filter_action
cntl_gather_specs(objid, list_count, acc)
     char *objid;
     void *list_count;
     void *acc;
{
	int *i = (int *)list_count;
	XmString label;

	label = XmStringCreateSimple(objid);
	XmListAddItem(cntl_ui_olist, label, *i);
	XmStringFree(label);

	*i = (*i + 1);

	/* continue processing */
	return TT_FILTER_CONTINUE;
}
       
     
/* 
 * Called to update the scrolling list of objects for a file. Uses
 * tt_file_objects_query to find all the ToolTalk objects.
 */
int
cntl_update_obj_panel()
{
        static int         list_item = 0;
	char               *file;
	int                i;

	cntl_objid = (char *)0;

	for (i = list_item; i >= 0; i--) {
		XmListDeletePos(cntl_ui_olist, i);
	}

	list_item = 0;
	file = XmTextGetString(cntl_ui_file_field);
		if  (tt_file_objects_query(file,
				   (Tt_filter_function)cntl_gather_specs,
				   &list_item, NULL) != TT_OK) {
		write_footer("Couldn't query objects for file");
		return 0;
	}

	return 1;
}


/* 
 * Callback function for "File objects..." menu item.
 */
void
cntl_ui_file_objects(widget, client_data, call_data)
	Widget widget;
	XtPointer client_data, call_data;
{
	char *string;

	write_footer("");
	string = XmTextGetString(cntl_ui_file_field);
	if (cntl_update_obj_panel()) {
		(void) tt_file_join(XmTextGetString(cntl_ui_file_field));
		show_popup(cntl_ui_obj_popup);
	}
}


/*
 * Notify callback function for `cntl_ui_olist'.
 */
void
cntl_ui_olist_handler(widget, client_data, cbs)
	Widget widget;
	XtPointer client_data;
	XmListCallbackStruct *cbs;
{
	char *string;

	string = XmStringUnparse(cbs->item, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
			NULL, 0, XmOUTPUT_ALL);
	cntl_objid = strdup(string);
	XtFree (string);
}


/*
 * Notify callback function for `cntl_ui_hilite_button'.
 */
void
cntl_ui_hilite_button_handler(widget, client_data, call_data)
	Widget widget;
	XtPointer client_data, call_data;
{
	Tt_message	msg;
	
	if (cntl_objid == (char *)0) {
		write_footer("No object id selected");
		return;
	}
	msg = tt_orequest_create(cntl_objid, "hilite_obj");
	tt_message_arg_add(msg, TT_IN, "string", cntl_objid);
	tt_message_callback_add(msg, cntl_msg_callback);
	tt_message_send(msg);
}


/*
 * Initialize ui environment.
 */
void
cntl_ui_initialize()
{
	Arg	args[20];
	int	n;
	Widget	but;
	XmString label;
	void	cntl_ui_saveas_popup_initialize();
	void	cntl_ui_obj_popup_initialize();

	cntl_ui_base_window = XtAppCreateShell(0, "base_frame",
                                applicationShellWidgetClass, dpy, 0, 0 );
	XtVaSetValues(cntl_ui_base_window,
                      XmNwidth,  339,
                      XmNheight, 100,
                      XmNtitle,  "Sun_EditDemo_cntl",
                      0);

        cntl_ui_base_controls = XtVaCreateManagedWidget("controls",
                                  xmFormWidgetClass, cntl_ui_base_window, 0 );

	n = 0;
	XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset,      5);             n++;
	XtSetArg(args[n], XmNrightOffset,     5);             n++;
	XtSetArg(args[n], XmNtopOffset,       5);             n++;
	cntl_ui_file_menu = (Widget) XmCreateMenuBar(cntl_ui_base_controls,
                                            "menubar", args, n);

	cntl_ui_file_button = (Widget) XmCreatePulldownMenu(cntl_ui_file_menu,
                                                   "File", NULL, 0);
	label = XmStringCreateSimple("File");
	XtVaCreateManagedWidget("File",
                                xmCascadeButtonWidgetClass, cntl_ui_file_menu,
                                XmNlabelString, label,
                                XmNmnemonic,    'F',
                                XmNsubMenuId,   cntl_ui_file_button,
                                0);
        XmStringFree(label);
	but = XtVaCreateManagedWidget("Edit",
                           xmPushButtonWidgetClass, cntl_ui_file_button, 0);
	XtAddCallback(but, XmNactivateCallback,
                      (XtCallbackProc) cntl_ui_edit, 0);
	but = XtVaCreateManagedWidget("Save",
                           xmPushButtonWidgetClass, cntl_ui_file_button, 0);
	XtAddCallback(but, XmNactivateCallback,
                      (XtCallbackProc) cntl_ui_save, 0);  
	but = XtVaCreateManagedWidget("Save as...",
                           xmPushButtonWidgetClass, cntl_ui_file_button, 0);
	XtAddCallback(but, XmNactivateCallback,
                      (XtCallbackProc) cntl_ui_save_as, 0);  
	but = XtVaCreateManagedWidget("File Objects...",
                           xmPushButtonWidgetClass, cntl_ui_file_button, 0);
	XtAddCallback(but, XmNactivateCallback,
                      (XtCallbackProc) cntl_ui_file_objects, 0);  
	but = XtVaCreateManagedWidget("Close",
                           xmPushButtonWidgetClass, cntl_ui_file_button, 0);
	XtAddCallback(but, XmNactivateCallback,
                      (XtCallbackProc) cntl_ui_close, 0);
	XtManageChild(cntl_ui_file_menu);

	cntl_ui_file_label = XtVaCreateManagedWidget("File:",
                                 xmLabelWidgetClass,  cntl_ui_base_controls,
                                 XmNtopAttachment,    XmATTACH_WIDGET,
                                 XmNtopWidget,        cntl_ui_file_menu,
                                 XmNtopOffset,        5,
                                 XmNleftAttachment,   XmATTACH_FORM,
                                 XmNleftOffset,       5,
                                 0);
	label = XmStringCreateSimple("File:");
	XtVaSetValues(cntl_ui_file_label, XmNlabelString, label, 0);
	XmStringFree(label);

	cntl_ui_file_field = XtVaCreateManagedWidget("text",
                                 xmTextWidgetClass,   cntl_ui_base_controls,
                                 XmNtopAttachment,    XmATTACH_WIDGET,
                                 XmNtopWidget,        cntl_ui_file_menu,
                                 XmNtopOffset,        5,
                                 XmNleftAttachment,   XmATTACH_WIDGET,
                                 XmNleftWidget,       cntl_ui_file_label,
                                 XmNleftOffset,       5,
                                 XmNrightAttachment,  XmATTACH_FORM,
                                 XmNrightOffset,      5, 
                                 0);
	label = XmStringCreateSimple("");
	XtVaSetValues(cntl_ui_file_field, XmNlabelString, label, 0);
	XmStringFree(label);

	cntl_ui_message = XtVaCreateManagedWidget("message",
                                 xmLabelWidgetClass,  cntl_ui_base_controls,
                                 XmNtopAttachment,    XmATTACH_WIDGET,
                                 XmNtopWidget,        cntl_ui_file_field,
                                 XmNtopOffset,        10, 
                                 XmNleftAttachment,   XmATTACH_FORM,
                                 XmNleftOffset,       5, 
                                 XmNrightAttachment,  XmATTACH_FORM,
                                 XmNrightOffset,      5,
                                 XmNbottomAttachment, XmATTACH_FORM,
                                 XmNbottomOffset,     5, 
                                 0);
	label = XmStringCreateSimple("");
	XtVaSetValues(cntl_ui_message, XmNlabelString, label, 0);
	XmStringFree(label);

	XtRealizeWidget(cntl_ui_base_window);

	cntl_ui_saveas_popup_initialize();
	cntl_ui_obj_popup_initialize();
}


void
cntl_ui_saveas_popup_initialize()
{
	Arg args[10];
	int n;
	XmString label;

	n = 0;
	XtSetArg(args[n], XmNtitle,       "Sun_EditMenu_cntl: Save As"); n++;
	XtSetArg(args[n], XmNmappedWhenManaged, False);        n++;
	XtSetArg(args[n], XmNdeleteResponse,    XmDO_NOTHING); n++;
	cntl_ui_saveas_popup = XmCreateDialogShell(cntl_ui_base_window,
                                                   "saveas", args, n);

	cntl_ui_saveas_controls = XtVaCreateManagedWidget("controls",
                            xmFormWidgetClass,    cntl_ui_saveas_popup,
                            XmNrubberPositioning, False,
                            XmNresizePolicy,      XmRESIZE_ANY,
                            XmNautoUnmanage,      False,
                            XmNwidth,             350,
                            XmNheight,            120,
                            0);

	label = XmStringCreateSimple("Directory:");
	cntl_ui_save_as_dlabel = XtVaCreateManagedWidget("dlabel",
                            xmLabelWidgetClass,  cntl_ui_saveas_controls,
                            XmNtopAttachment,    XmATTACH_FORM,
                            XmNtopOffset,        5,
                            XmNleftAttachment,   XmATTACH_FORM,
                            XmNleftOffset,       5, 
                            XmNlabelString,      label,
                            0);
	XmStringFree(label);

	cntl_ui_save_as_directory = XtVaCreateManagedWidget("text",
                            xmTextWidgetClass,   cntl_ui_saveas_controls,
                            XmNcolumns,          35,
                            XmNtopAttachment,    XmATTACH_FORM,
                            XmNtopOffset,        5,
                            XmNleftAttachment,   XmATTACH_WIDGET,
                            XmNleftWidget,       cntl_ui_save_as_dlabel,
                            XmNleftOffset,       5, 
                            XmNrightAttachment,  XmATTACH_FORM,
                            XmNrightOffset,      5, 
                            0);

	label = XmStringCreateSimple("Filename:");
	cntl_ui_new_flabel = XtVaCreateManagedWidget("flabel",
                            xmLabelWidgetClass,  cntl_ui_saveas_controls,
                            XmNtopAttachment,    XmATTACH_WIDGET,
                            XmNtopWidget,        cntl_ui_save_as_directory,
                            XmNtopOffset,        5, 
                            XmNleftAttachment,   XmATTACH_FORM,
                            XmNleftOffset,       5, 
                            XmNlabelString,      label,
                            0);
	XmStringFree(label);

	label = XmStringCreateSimple("");
	cntl_ui_new_filename = XtVaCreateManagedWidget("text",
                            xmTextWidgetClass,   cntl_ui_saveas_controls,
                            XmNcolumns,          35,
                            XmNlabelString,      label,
                            XmNtopAttachment,    XmATTACH_WIDGET,
                            XmNtopWidget,        cntl_ui_save_as_directory,
                            XmNtopOffset,        5, 
                            XmNleftAttachment,   XmATTACH_WIDGET,
                            XmNleftWidget,       cntl_ui_save_as_dlabel,
                            XmNleftOffset,       5, 
                            XmNrightAttachment,  XmATTACH_FORM,
                            XmNrightOffset,      5, 
                            0);
	XmStringFree(label);

	label = XmStringCreateSimple("Save");
	cntl_ui_save_as_button = XtVaCreateManagedWidget("button",
                xmPushButtonWidgetClass, cntl_ui_saveas_controls,
                XmNleftAttachment,   XmATTACH_FORM,
                XmNleftOffset,       5,
                XmNtopAttachment,    XmATTACH_WIDGET,
                XmNtopWidget,        cntl_ui_new_filename,
                XmNtopOffset,        5,
                XmNbottomAttachment, XmATTACH_FORM,
                XmNbottomOffset,     5,
                XmNlabelString,      label,
                0);
	XmStringFree(label);
	XtAddCallback(cntl_ui_save_as_button,
                  XmNactivateCallback, cntl_ui_save_as_button_handler, 0);
	XtManageChild(cntl_ui_saveas_controls);
	add_delete_callback(cntl_ui_saveas_popup);
}



void
cntl_ui_obj_popup_initialize()
{
	int n;
	Arg args[20];
	XmString label;

	n = 0;
	XtSetArg(args[n], XmNtitle, "Sun_EditMenu_cntl: File Objects"); n++;
	XtSetArg(args[n], XmNmappedWhenManaged, False);        n++;
	XtSetArg(args[n], XmNdeleteResponse,    XmDO_NOTHING); n++;
	cntl_ui_obj_popup = XmCreateDialogShell(cntl_ui_base_window,
                                                "object", args, n);

	cntl_ui_obj_controls = XtVaCreateManagedWidget("controls",
                            xmFormWidgetClass,    cntl_ui_obj_popup,
                            XmNrubberPositioning, False,
                            XmNresizePolicy,      XmRESIZE_ANY,
                            XmNautoUnmanage,      False,
                            XmNwidth,             350,
                            XmNheight,            120,
                            0);

	n = 0;
	label = XmStringCreateSimple("File Objects:");
	XtSetArg(args[n], XmNlabelString,    label);         n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset,     5);             n++;
	XtSetArg(args[n], XmNtopAttachment,  XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopOffset,      5);             n++;
	cntl_ui_olist_label =
		XmCreateLabel(cntl_ui_obj_controls, "label", args, n);
	XmStringFree(label);
	XtManageChild(cntl_ui_olist_label);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment,    XmATTACH_FORM);       n++;
	XtSetArg(args[n], XmNtopOffset,        5);                   n++;
	XtSetArg(args[n], XmNvisibleItemCount, 4);                   n++;
	XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);     n++;
	XtSetArg(args[n], XmNleftWidget,       cntl_ui_olist_label); n++;
        XtSetArg(args[n], XmNleftOffset,       5);                   n++;
	XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);       n++;
	XtSetArg(args[n], XmNrightOffset,      5);                   n++;
	XtSetArg(args[n], XmNresizable,        True);                n++;
	cntl_ui_olist =
		XmCreateScrolledList(cntl_ui_obj_controls, "olist", args, n);
	XtAddCallback(cntl_ui_olist,
                XmNsingleSelectionCallback, cntl_ui_olist_handler, 0);
	XtManageChild(cntl_ui_olist);

	label = XmStringCreateSimple("Display");
	cntl_ui_hilite_button = XtVaCreateManagedWidget("button",
                xmPushButtonWidgetClass, cntl_ui_obj_controls,
                XmNtopAttachment,    XmATTACH_WIDGET,
                XmNtopWidget,        cntl_ui_olist,
                XmNtopOffset,        5,  
                XmNbottomAttachment, XmATTACH_FORM,
                XmNbottomOffset,     5,  
                XmNlabelString,      label,
                0);
	XmStringFree(label);
	XtAddCallback(cntl_ui_hilite_button,
                  XmNactivateCallback, cntl_ui_hilite_button_handler, 0);
	XtManageChild(cntl_ui_obj_controls);
	add_delete_callback(cntl_ui_obj_popup);
}


int
cntl_is_same_file(f1, f2)
     char	*f1;
     char	*f2;
{
	struct stat	f1_stat;
	struct stat	f2_stat;

	if (!strcmp(f1,f2)) {
		return(1);
	}

	return((0 == stat(f1, &f1_stat)) &&
	       (0 == stat(f2, &f2_stat)) &&
	       (f1_stat.st_ino == f2_stat.st_ino) &&
	       (f1_stat.st_dev == f2_stat.st_dev));
}
