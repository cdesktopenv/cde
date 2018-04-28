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
/*%%  $XConsortium: edit.c /main/3 1995/10/23 09:46:34 rswiston $ 			 				 */
/*
 * edit.c
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

/* 
 * Implementation of a simple "remote-control" editor. Accepts requests
 * to edit, save, and close files as well as hilite "objects" created in
 * the file. Objects are kept track of by wrapping them in C-style
 * comments with their respective object ids. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <desktop/tt_c.h>
#include "Sun_EditDemo_opnums.h"

Display *dpy;
XtAppContext app;
char current_file[MAXPATHLEN];
int do_close = 0;
int text_modified = -1;

Widget	edit_ui_base_window;
Widget	edit_ui_textpane;
Widget	edit_ui_panel;
Widget	edit_ui_obj_button;
Widget	edit_ui_message;


void
main(argc, argv)
	int		argc;
	char		**argv;
{
	char	*dfile;

	/*
	 * Initialize Motif.
	 */
	XtToolkitInitialize();
	app = XtCreateApplicationContext();
	dpy = XtOpenDisplay(app, 0, 0, "edit", 0, 0, &argc, argv );
	
	/*
	 * Initialize user interface components.
	 */
	edit_ui_initialize();
	if (! edit_init_tt()) {
		fprintf(stderr,"%s: Can't initialize ToolTalk\n", argv[0]);
		exit(1);
	}
	/*
	 * Turn control over to Motif.
	 */
	while (! do_close) {
		XEvent event;
		XtAppNextEvent(app, &event);
		XtDispatchEvent(&event);
	}
	if ((dfile = tt_default_file()) != (char *)0) {
		tt_file_quit(dfile);
	}
	tt_close();
	exit(0);
}


void
write_footer(message)
	char *message;
{
	XmString label;

	label = XmStringCreateSimple(message);
	XtVaSetValues(edit_ui_message, XmNlabelString, label, 0);
	XmStringFree(label);
}


Tt_status
read_file(widget, file)
	Widget widget;
	char    *file;
{
	char buf[BUFSIZ], *text;
	struct stat statb;
	int len;
	FILE *fp;
	Tt_status status;

	/*
 	 * Make sure the file is a regular text file and open it.
 	 */

	if (stat(file, &statb) == -1 || (statb.st_mode & S_IFMT) != S_IFREG ||
		 !(fp = fopen(file, "r"))) {
		perror(file);
		return TT_ERR_FILE;
	}
           
	/*
  	 * Put the contents of the file in the Text widget by allocating
	 * enough space for the entire file, reading the file into the space,
	 * and using XmTextSetString() to show the file.
	 */

	strcpy(current_file, file);
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


/* Write the contents of a text widget to a file. */

Tt_status
write_file(widget, file)
	Widget  widget;
	char    *file;
{
	char *text;
	int len;
	FILE *fp;
	Tt_status status;

	if (!(fp = fopen(file, "w"))) {
		perror(file);
		return TT_ERR_FILE;
	}

/* Saving -- get text from the text widget. */

	text = XmTextGetString(widget);
	len = (int) XmTextGetLastPosition(widget);
            
/* Write it to file (check for error). */
            
	if (fwrite(text, sizeof(char), len, fp) != len) {
		status = TT_ERR_FILE;
	} else {
		status = TT_OK;
	}
	fclose(fp);
	return status;
}


/* 
 * Initialize our ToolTalk environment.
 */
int
edit_init_tt()
{
	int	mark;
	char	*procid = tt_open();
	int	ttfd;
	void	edit_receive_tt_message();

	mark = tt_mark();

	if (tt_pointer_error(procid) != TT_OK) {
		return 0;
	}
	if (tt_ptype_declare("Sun_EditDemo") != TT_OK) {
		fprintf(stderr,"Sun_EditDemo is not an installed ptype.\n");
		return 0;
	}
	ttfd = tt_fd();
	XtAppAddInput(app, ttfd, (XtPointer) XtInputReadMask,
                      edit_receive_tt_message, 0);

	tt_session_join(tt_default_session());

	/*
	 * Note that without tt_mark() and tt_release(), the above
	 * combination would leak storage -- tt_default_session() returns
	 * a copy owned by the application, but since we don't assign the
	 * pointer to a variable we could not free it explicitly.
	 */

	tt_release(mark);
	return 1;
}


/* 
 * Handle any incoming ToolTalk messages.
 */
void
edit_receive_tt_message(client_data, fid, id)
	XtPointer client_data;
	int *fid;
	XtInputId *id;
{
	Tt_message		msg_in;
	int			opnum;
	int                     opstatus;
	char 			*file;

	msg_in = tt_message_receive();
	if (msg_in == NULL) return;
	if (tt_pointer_error(msg_in) == TT_ERR_NOMP) {
		fprintf(stderr,"ToolTalk server down.\n");
		exit(0);
	}
	switch (tt_message_opnum(msg_in)) {
	      case SUN_EDITDEMO_EDIT:
	        opstatus = edit_edit(msg_in);
		break;
	      case SUN_EDITDEMO_SAVE:
		opstatus = edit_save(msg_in);
		break;
	      case SUN_EDITDEMO_SAVE_AS:
		opstatus = edit_save(msg_in);
		break;
	      case SUN_EDITDEMO_CLOSE:
		opstatus = edit_close(msg_in);
		break;
	      case SUN_EDITDEMO_HILITE_OBJ:
		opstatus = edit_hilite_obj(msg_in);
		break;
	      default:
		/* don't know what else to do with this message */
		tt_message_reject(msg_in);
		break;
	}
	if (opstatus == 0) {
		tt_message_fail(msg_in);
	} else {
		tt_message_reply(msg_in);
	}
	tt_message_destroy(msg_in);

	return;
}


/* 
 * Handle the "edit" op.
 */
int
edit_edit(msg)
     Tt_message msg;
{
	int	mark = tt_mark();
	char	*file = tt_message_file(msg);
	char	*dfile = tt_default_file();
	char	buf[255];

	if (access(file, R_OK) != 0) {
		tt_message_status_set(msg, TT_ERR_FILE);
		tt_message_status_string_set(msg,"Can't open file for read.");
		tt_release(mark);
		return 0;
	} else {
		if (dfile == (char *)0 || 0 != strcmp(dfile, file)) {
			/* if not already editing this file, load it */
			/* in. */
			tt_default_file_set(file);
			tt_file_join(file);
			read_file(edit_ui_textpane, file);
			sprintf(buf,"Sun_EditDemo_edit: (%s)", file);
			XtVaSetValues(edit_ui_base_window,
                                      XmNtitle, buf, 0);
		}
		tt_release(mark);
		return 1;
	}
	
}


/* 
 * Handle the "save" op.
 */
int
edit_save(msg)
     Tt_message msg;
{
	int		mark = tt_mark();
	char		*new_file;

	if (text_modified == 0) {
		/* no save is needed */
		tt_release(mark);
		return 1;
	} 

	if (tt_message_opnum(msg) == SUN_EDITDEMO_SAVE) {
		if (write_file(edit_ui_textpane, current_file) == TT_OK) {
			text_modified = 0;
			tt_message_reply(msg);
			tt_release(mark);
			return 1;
		}
	} else {
		/* handle SAVE_AS */
		new_file = tt_message_arg_val(msg, 0);
		if (write_file(edit_ui_textpane, new_file) == TT_OK) {
			tt_file_quit(tt_default_file());
			tt_default_file_set(new_file);
			tt_file_join(new_file);
			read_file(edit_ui_textpane, new_file);
			tt_release(mark);
			return 1;
		}
	}

	/* couldn't complete operation */
	tt_message_status_set(msg, TT_ERR_FILE);
	tt_message_status_string_set(msg, "Couldn't save file");
	tt_release(mark);
	return 0;
}


/* 
 * Handle the "close" op.
 */
int
edit_close(msg)
     Tt_message msg;
{
	Atom wmchs;
	Window w;
	XClientMessageEvent xev;

	if (text_modified > 0) {
		tt_message_status_set(msg, TT_ERR_FILE);
		tt_message_status_string_set(msg, "File has been modified");
		return 0;
	} else {
		do_close = 1;

/* Send an event to force the event loop to "click on" and quit the program. */

		w = XtWindow(edit_ui_base_window);
		wmchs = XInternAtom(dpy, "WM_CHANGE_STATE", True);
		xev.type         = ClientMessage;
		xev.message_type = wmchs;
		xev.window       = w;
		xev.format       = 32;
		xev.data.l[0]    = IconicState;
		XSendEvent(dpy, w, True, (SubstructureRedirectMask |
                           SubstructureNotifyMask), &xev) ;
		XFlush(dpy);
		return 1;
	}
}

/* 
 * Handle the "hilite_obj" op.
 */
int
edit_hilite_obj(msg)
     Tt_message msg;
{
	int	mark = tt_mark();
	char	*objid = tt_message_arg_val(msg, 0);
	char	obj_start_text[100];
	char	obj_end_text[100];

	if (tt_message_status(msg) == TT_WRN_START_MESSAGE
	    && edit_edit(msg) == 0) {
		/* we were started to hilite an object but couldn't load the */
		/* file into the textpane. */
		tt_release(mark);
		return 0;
	}

	/* expect objects to be wrapped by appropiately formatted */
	/* C-style comments. */

	sprintf(obj_start_text," /* begin_object(%s) */", objid);
	sprintf(obj_end_text,"	/* end_object(%s) */", objid);

	if (select_region(edit_ui_textpane,
			  obj_start_text,
			  obj_end_text) == 1) {
		tt_release(mark);
		return 1;
	} else {
		tt_message_status_set(msg, TT_ERR_OBJID);
		tt_message_status_string_set(msg,"Couldn't find object");

		tt_release(mark);
		return 0;
	}
}


/* 
 * Make a ToolTalk spec out of the selected text in this textpane. Once
 * the spec is successfully created and written to a database, wrap the
 * text with C-style comments in order to delimit the object and send out
 * a notification that an object has been created in this file.
 */
void
edit_ui_make_object(widget, client_data, call_data)
	Widget widget;
	XtPointer  client_data, call_data;
{
	int		mark = tt_mark();
	char		*objid;
	char		*file;
	char		*sel;
	XmTextPosition	first, last;
	char		obj_start_text[100];
	char		obj_end_text[100];
	Tt_message	msg;

	if (! get_selection(edit_ui_textpane, &sel, &first, &last)) {
		write_footer("First select some text");
		tt_release(mark);
		return;
	}
	file = tt_default_file();

	if (file == (char *)0) {
		write_footer("Not editing any file");
		tt_release(mark);
		return;
	}

	/* create a new spec */

	objid = tt_spec_create(tt_default_file());
	if (tt_pointer_error(objid) != TT_OK) {
		write_footer("Couldn't create object");
		tt_release(mark);
		return;
	}


	/* set its otype */

	tt_spec_type_set(objid, "Sun_EditDemo_object");
	if (tt_spec_write(objid) != TT_OK) {
		write_footer("Couldn't write out object");
		tt_release(mark);
		return;
	}

	/* wrap spec's contents (the selected text) with C-style */
	/* comments. */

	sprintf(obj_start_text," /* begin_object(%s) */", objid);
	sprintf(obj_end_text,"	/* end_object(%s) */", objid);
	(void)wrap_selection(edit_ui_textpane,
                             obj_start_text, obj_end_text);
	
	/* now send out a notification that we've added a new object */

	msg = tt_pnotice_create(TT_FILE_IN_SESSION,"Sun_EditDemo_new_object");
	tt_message_file_set(msg, file);
	tt_message_send(msg);

	tt_release(mark);
	return;
}



/* 
 * Get the current selection. Returns 1 if the selection is in the passed
 * in textsw.
 */
int
get_selection(widget, selection, first_ptr, last_ptr)
	Widget		widget;
	char		**selection;
	XmTextPosition	*first_ptr, *last_ptr;
{
	char			*ptr;

	ptr = XmTextGetSelection(widget);
	if (ptr == NULL) return 0;
	*selection = malloc(strlen(ptr) + 1);
	if (*selection == (char *)0) {
		return -1;
	}
	(void)strcpy(*selection, ptr);
	XmTextGetSelectionPosition(widget, first_ptr, last_ptr);

	return 1;
}



void
reverse_str(str)
	char *str;
{
	char c;
	int i, n;

	n = strlen(str);
	for (i = 0; i < n/2; i++) {
		c = str[i];
		str[i] = str[n-i];
		str[n-i] = c;
	}
}


int
text_find_bytes(widget, first, last, str, reverse)
	Widget widget;
	XmTextPosition *first, *last;
	int reverse;
	char *str;
{
	char *ptr, *text;
	int n;

	text = XmTextGetString(widget);
	if (reverse) {
		reverse_str(text);
		reverse_str(str);
	}
	ptr = strstr(&text[*first], str);
	if (ptr == NULL) return -1;
	n = strlen(str);
	if (reverse) {
		*last  = (XmTextPosition) ptr;
		*first = (XmTextPosition) ptr - n;
	} else {
		*first = (XmTextPosition) ptr;
		*last  = (XmTextPosition) ptr + n;
	}
	return 0;
}


/* 
 * Selects region between "begin" and "end" in textsw. Returns 1 if
 * successful. 
 */
int
select_region(widget, begin, end)
     Widget		widget;
     char		*begin;
     char		*end;
{
	XmTextPosition		bfirst, blast, efirst, elast;
	XmTextPosition		inspoint;
	int			status;

	inspoint = XmTextGetInsertionPosition(widget);
	/* Find the "begin" text */

	/* first search forward */
	bfirst = inspoint;
	status = text_find_bytes(widget, &bfirst, &blast, begin, 0);
	if (status == -1) {
	        bfirst = inspoint;
		/* search failed, search backwards */
		status = text_find_bytes(widget, &bfirst, &blast, begin, 1);
		if (status == -1) {
			return 0;
		}
	}

	/* Find the "end" text */

	efirst = inspoint;
	/* first search forward */
	status = text_find_bytes(widget, &efirst, &elast, end, 0);
	if (status == -1) {
	        efirst = inspoint;
		/* search failed, search backwards */
		status = text_find_bytes(widget, &efirst, &elast, end, 1);
		if (status == -1) {
			return 0;
		}
	}
	XmTextSetSelection(widget,
                           (inspoint = (blast < elast) ? blast : elast),
                           (efirst > bfirst) ? efirst : bfirst,
                           CurrentTime);

	XmTextSetInsertionPosition(widget, inspoint);
	XmTextShowPosition(widget, inspoint);
	return 1;
}

/* 
 * Wraps the selected text in textsw with the begin and end strings
 * supplied. Returns 1 if successful.
 */
int
wrap_selection(widget, begin, end)
     Widget		widget;
     char		*begin;
     char		*end;
{
	char			*buf;
	char			*sel;
	XmTextPosition		sel_first, sel_last;
	int			sel_status;

	if (! (sel_status = get_selection(widget,
                                          &sel, &sel_first, &sel_last))) {
		return sel_status;
	}
		
	buf = malloc(strlen(sel) + strlen(begin) + strlen(end) + 3);
	if (buf == (char *)0) {
		return -1;
	}
	sprintf(buf,"%s\n%s\n%s", begin, sel, end);
	XmTextReplace(widget, sel_first, sel_last + 1, buf);
	free(buf);
	free(sel);
	
	return sel_status;
}


/*
 *  Increment a flag every time this routine is called. Note that it's
 *  called once, when the text is initially loaded, so the flag is initially
 *  -1, to counter this.
 */
text_modify(widget, client_data, cbs)
	Widget widget;
	XtPointer client_data;
	XmTextVerifyCallbackStruct *cbs;
{
	text_modified++;
}


/* 
 * Initialize our ui environment.
 */
edit_ui_initialize()
{
	Arg args[20];
	Pixmap icon;
	XmString label;
	int n, screen;

	static unsigned short	icon_bits[] = {
	0x3FFF,0xFF00,
	0x2000,0x0180,
	0x2000,0x0140,
	0x2000,0x0120,
	0x2000,0x0110,
	0x2000,0x0108,
	0x2000,0x0104,
	0x2000,0x01FE,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x2000,0x0006,
	0x3FFF,0xFFFE,
	0x1FFF,0xFFFE
	};

	edit_ui_base_window = XtAppCreateShell(0, "base_frame",
                                applicationShellWidgetClass, dpy, 0, 0 );
	screen = DefaultScreen(dpy);
	icon = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, screen),
                                      (char *) icon_bits, 32, 32, 1, 0, 1);
	XtVaSetValues(edit_ui_base_window,
                      XmNwidth,      509,
                      XmNheight,     420,
                      XmNtitle,      "Sun_EditDemo_edit",
                      XmNiconName,   "Sun_EditDemo_edit",
                      XmNiconPixmap, icon,
                      0);
	
	edit_ui_panel = XtVaCreateManagedWidget("panel",
                                  xmFormWidgetClass, edit_ui_base_window, 0);

	label = XmStringCreateSimple("Make object");
	edit_ui_obj_button = XtVaCreateManagedWidget("button",
                xmPushButtonWidgetClass, edit_ui_panel,
                XmNtopAttachment,    XmATTACH_FORM,
                XmNtopOffset,        5,  
                XmNleftAttachment,   XmATTACH_FORM,
                XmNleftOffset,       5,
                XmNlabelString,      label,
                0);          
	XmStringFree(label); 
	XtAddCallback(edit_ui_obj_button,
                  XmNactivateCallback, edit_ui_make_object, 0);

        XtManageChild(edit_ui_panel);

	n = 0;
        XtSetArg(args[n], XmNeditMode,         XmMULTI_LINE_EDIT);  n++;
        XtSetArg(args[n], XmNresizable,        True);               n++;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
        XtSetArg(args[n], XmNtopWidget,        edit_ui_obj_button); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
        XtSetArg(args[n], XmNleftOffset,       5);                  n++;
        XtSetArg(args[n], XmNrightOffset,      5);                  n++;
        XtSetArg(args[n], XmNtopOffset,        5);                  n++;
        XtSetArg(args[n], XmNrows,             24);                 n++;
        XtSetArg(args[n], XmNcolumns,          80);                 n++;
        edit_ui_textpane = XmCreateScrolledText(edit_ui_panel, "textpane",
                                                args, n);

	XtAddCallback(edit_ui_textpane, XmNmodifyVerifyCallback,
                      text_modify, 0);
	edit_ui_message = XtVaCreateManagedWidget("message",
                                 xmLabelWidgetClass,  edit_ui_panel,
                                 XmNtopAttachment,    XmATTACH_WIDGET,
                                 XmNtopWidget,        edit_ui_textpane,
                                 XmNtopOffset,        10, 
                                 XmNleftAttachment,   XmATTACH_FORM,
                                 XmNleftOffset,       5, 
                                 XmNrightAttachment,  XmATTACH_FORM,
                                 XmNrightOffset,      5, 
                                 XmNbottomAttachment, XmATTACH_FORM,
                                 XmNbottomOffset,     5, 
                                 0);
	label = XmStringCreateSimple("");
	XtVaSetValues(edit_ui_message, XmNlabelString, label, 0);
	XmStringFree(label);

        XtManageChild(edit_ui_textpane);
        XtRealizeWidget(edit_ui_base_window);
}
