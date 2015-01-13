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
/* $TOG: dtcm_editor.c /main/14 1999/09/20 10:32:17 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Xm/XmAll.h>
#include <Dt/EnvControlP.h>
#include <Dt/Dnd.h>
#include <Dt/Icon.h>
#include <Dt/UserMsg.h>
#include <Tt/tttk.h>
#include <csa.h>
#include "calendar.h"
#include "props.h"
#include "props_pu.h"
#include "dssw.h"
#include "rfp.h"
#include "dnd.h"
#include "util.h"
#include "cm_tty.h"
#include <nl_types.h>
#include <locale.h>
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE       0
#endif

int debug = 0;
static Tt_message load_cb();

static const char *ptype = "Dt_AppointmentEditor";

typedef enum {no_tt, file_tt, buffer_tt} Dtcm_editor_start;

/* Absolute value macro */
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif
 

#include "drag_xbm"
#include "drag_mask_xbm"

/*
**  Structure for stand-alone editor
*/
typedef struct {
	Widget		attach_button;
	Calendar	*c;
	DSSW		*dssw;
	char		*file;
	char		*vtype;
	Dtcm_editor_start init;
	Boolean 	read_only;
	Tt_pattern	*contract_pats;
	Tt_message	contract;
	Widget		form;
	Props		*p;
	Props_pu	*pu;
	Widget		reset_button;
	RFP		*rfp;
	Widget		top_level;
	Boolean		modified;
	Dtcm_appointment *orig_appt;
	Widget		drag_source;
        int             initialX;
        int             initialY;
        Boolean         doing_drag;
        Widget          drag_icon;
	Pixmap		drag_bitmap;
	Pixmap		drag_mask;
	int		dsswFlags;
	int		rfpFlags;
} DTCM_editor;

static void de_apply_proc(Widget , XtPointer , XtPointer );

/*
**  Hate like hell to make this global, but I can find no way to get at it
**  when I need it in other methods ...
**
**  Also, help uses a global reference to calendar which hoses us ...
*/
XtAppContext	app;
Calendar	*calendar;

static void
de_mark_change(Widget w, XtPointer data, XtPointer cbs) {
 
        DTCM_editor        *de = (DTCM_editor *)data;

        de->modified = True;

	if ((w == de->dssw->start_text) ||
	    (w == de->dssw->start_am) ||
	    (w == de->dssw->start_pm) ||
	    (w == de->dssw->stop_text) ||
	    (w == de->dssw->stop_am) ||
	    (w == de->dssw->stop_pm))
	  de->dsswFlags = 0;

	if (w == de->rfp->repeat_menu)
	  de->rfpFlags = 0;
}

static void
merge_old_values(Dtcm_appointment *original, Dtcm_appointment *new) {

	/* This routine takes in the original appointment structure 
	   from the edited appointment, and merges back in any attributes 
	   that aren't controlled by our editor, so that when the 
	   appointment is written back out to the buffering agent, those 
	   attributes are not lost. */

	int	source_count, dest_count, new_attrs;
	int	dest_attr_num = new->count;
	Boolean found;

	if (original == NULL)
		return;
	
	/* We need to see how much larger the attribute array needs to 
	   get.  For each of the guys in the source attribute array, 
	   we need to look and see if it's already a member of the 
	   destination array.  Those that don't exist, we count, and 
	   later move. */

	for (source_count = 0, new_attrs = 0; source_count < original->count; source_count++) {
		found = False;
		for (dest_count = 0; dest_count < dest_attr_num; dest_count++) {
			if (original->attrs[source_count].name && new->attrs[dest_count].name) {
				if (strcmp(original->attrs[source_count].name, new->attrs[dest_count].name) == 0) {
					found = True;
					break;
				}
			}
		}

		if (found == False)
			new_attrs++;
	}

	if (new_attrs == 0)
		return;

	/* realloc the attrs array to be large enough to accomodate the new 
	   attibutes */

	new->attrs = (CSA_attribute *) realloc(new->attrs, (dest_attr_num + new_attrs) * sizeof(CSA_attribute));

	/* Copy in the new attribute values */
	for (source_count = 0; source_count < original->count; source_count++) {
		found = False;
		for (dest_count = 0; dest_count < new->count; dest_count++) {
			if (original->attrs[source_count].name && new->attrs[dest_count].name) {
				if (strcmp(original->attrs[source_count].name, new->attrs[dest_count].name) == 0) {
					found = True;
					break;
				}
			}
		}

		if (found == False)
		{
			new->attrs[new->count] = original->attrs[source_count];
			new->count++;
		}
	}

	/* reset the appointment links, as the old ones have been 
	   thrashed by reallocing the attribute array. */

	set_appt_links(new);
}

Widget
CreateDragSourceIcon(
        Widget          widget,
        Pixmap          pixmap,
        Pixmap          mask)
{
        Widget          dragIcon;
        Window          rootWindow;
        int             pixmapX, pixmapY;
        unsigned int    pixmapWidth, pixmapHeight, pixmapBorder, pixmapDepth;
        Arg             args[20];
        Cardinal        nn = 0;
 
        XGetGeometry (XtDisplayOfObject(widget), pixmap, &rootWindow,
                &pixmapX, &pixmapY, &pixmapWidth, &pixmapHeight,
                &pixmapBorder, &pixmapDepth);
 
        XtSetArg(args[nn], XmNwidth, pixmapWidth);  nn++;
        XtSetArg(args[nn], XmNheight, pixmapHeight);  nn++;
        XtSetArg(args[nn], XmNmaxWidth, pixmapWidth);  nn++;
        XtSetArg(args[nn], XmNmaxHeight, pixmapHeight);  nn++;
        XtSetArg(args[nn], XmNpixmap, pixmap);  nn++;
        XtSetArg(args[nn], XmNmask, mask);  nn++;
        XtSetArg(args[nn], XmNdepth, pixmapDepth);  nn++;
        dragIcon = XmCreateDragIcon(widget, "sourceIcon", args, nn);

        return(dragIcon);
}

/*
 * getIcon
 *
 * Returns a new IconInfo structure with bitmap, mask, width, height,
 * icon type and name.
 */
static void
GetIcon(DTCM_editor *de)
{
 
        Display        *display = XtDisplay(calendar->frame);
        Window          window = XtWindow(calendar->frame);
        unsigned char  *bitmapData, *bitmapMask;
 
        if (de->drag_bitmap == 0) {
                de->drag_bitmap = XCreateBitmapFromData(display,
                        window, (char *) drag_xbm_bits,
                        drag_xbm_width, drag_xbm_height);
                if (de->drag_bitmap == 0) {

                        printf("%s", catgets(calendar->DT_catd, 1, 237, "XCreateBitmapFromData() failed for bitmap.\n"));
                        return;
                }
        }
        if (de->drag_mask == 0) {
                de->drag_mask = XCreateBitmapFromData(display,
                        window, (char *) drag_mask_xbm_bits,
                        drag_mask_xbm_width, drag_mask_xbm_height);
                if (de->drag_mask == 0) {
                        printf("%s", catgets(calendar->DT_catd, 1, 238, "XCreateBitmapFromData() failed for mask.\n"));
                        return;
                }
        }
}
 
 
/*
 * DragFinishCB
 *
 * Resets drag state to indicate the drag is over. Free memory allocated
 * with the drag.
 */
static void
DragFinishCB(
        Widget          widget,
        XtPointer       clientData,
        XtPointer       callData)
{
        DragContext     *context = (DragContext *) clientData;

        if (!context)
                return;

	if (context->editor_type == StandAloneEditor)
	    ((DTCM_editor *) context->editor)->doing_drag = False;

        if (context->data)
	    free(context->data);

        free(context);
}

/*
 * ApptConvertCB
 *
 * Fills in data object with calendar appointment string based on which
 * appointment in the list was under the pointer when the drag started.
 */
static void
ApptConvertCB(
        Widget          dragContext,
        XtPointer       clientData,
        XtPointer       callData)
{
        DtDndConvertCallbackStruct *convertInfo 
					= (DtDndConvertCallbackStruct*)callData;
        DtDndBuffer 	*data		= &(convertInfo->dragData->data.buffers[0]);
        DragContext     *context        = (DragContext *)clientData;
        Display         *display        = XtDisplay(dragContext);
        Atom            CMAPPOINTMENT 	
			= XmInternAtom(display, "CalendarAppointment", False);
	Calendar	*c = context->calendar;

	if (convertInfo->reason != DtCR_DND_CONVERT_DATA)
		return;

        /* REMIND: Need to check convertInfo->reason, handle DELETE, etc */

        data->bp   = XtNewString(context->data);
        data->size = strlen(data->bp);
        data->name = XtNewString(catgets(c->DT_catd, 1, 236, "CalendarAppointment"));
}

void
StandaloneApptDragStart(
        Widget          widget,
        XEvent          *event,
        DTCM_editor     *de,
        EditorType      editor_type)
{
        static XtCallbackRec convertCBRec[] = { {ApptConvertCB, NULL},
                                                {NULL, NULL} };
        static XtCallbackRec dragFinishCBRec[] =  { {DragFinishCB, NULL},
                                                    {NULL, NULL} };
 
        Display        *display 	= XtDisplay(widget);
        int             itemCount, selectedPos;
        DragContext     *context = calloc(sizeof(DragContext), 1);
	Calendar	*c = de->c;
	Dtcm_appointment        *appt;
	int		old_attr_count;
        char            *apptstr;
	int		preDsswFlags, preRfpFlags;

	/* Convert appointment into string.  If not successful, don't start drag. */
	appt = allocate_appt_struct(appt_write, DATAVER_ARCHIVE, NULL);
	load_appt_defaults(appt, de->p);
	preDsswFlags = de->dsswFlags;
	preRfpFlags = de->rfpFlags;
	if (!dssw_form_flags_to_appt(de->dssw, appt, de->c->calname,
				     now(), &de->dsswFlags) ||
	    !rfp_form_flags_to_appt(de->rfp, appt, de->c->calname,
				    &de->rfpFlags) ||
	    (preDsswFlags != de->dsswFlags) ||
	    (preRfpFlags != de->rfpFlags))
	{
	    de->doing_drag = False;
	    free_appt_struct(&appt);
	    free(context);
	    return;
	}

	/* save the old count of attributes so that when this appointment 
	   gets freed, we can prevent the added attribute references from 
	   being freed. */

	old_attr_count = appt->count;
	merge_old_values(de->orig_appt, appt);
	apptstr = parse_attrs_to_string(appt, de->p,
					attrs_to_string(appt->attrs, appt->count));
	appt->count = old_attr_count;
	free_appt_struct(&appt);

        context->data = apptstr;
        context->calendar = c;
        context->editor_type = editor_type;
        context->editor = (caddr_t) de;
 
        GetIcon(de);
 
        convertCBRec[0].closure = (XtPointer)context;
        dragFinishCBRec[0].closure = (XtPointer)context;
 
        if (de->drag_icon == NULL) {
                de->drag_icon = CreateDragSourceIcon(widget, de->drag_bitmap, de->drag_mask);
        }
 
        if (DtDndVaDragStart(widget, event, DtDND_BUFFER_TRANSFER, 1,
            	XmDROP_COPY, 
		convertCBRec, dragFinishCBRec,
		DtNsourceIcon,		de->drag_icon,
		NULL)
            == NULL) {
 
                printf("%s", catgets(c->DT_catd, 1, 239, "DragStart returned NULL.\n"));
        }
}

/*
 * dragMotionHandler
 *
 * Determine if the pointer has moved beyond the drag threshold while button 1
 * was being held down.
 */
static void
EditApptDragMotionHandler(
        Widget          dragInitiator,
        XtPointer       clientData,
        XEvent         *event)
{
        int             diffX, diffY;
        DTCM_editor     *de = (DTCM_editor *) clientData;
	Calendar	*c = de->c;
        Dimension       source_height, source_width;
        Position        source_x, source_y;
 
        if (!de->doing_drag) {

                /* check to see if the iniital value was within the
                   bounds for the drag source icon. */
 
                XtVaGetValues(de->drag_source,
                                XmNx, &source_x,
                                XmNy, &source_y,
                                XmNheight, &source_height,
                                XmNwidth, &source_width,
                                NULL);
 
                if ((event->xmotion.x < source_x) ||
                    (event->xmotion.y < source_y) ||
                    (event->xmotion.x > (int) (source_x + source_width)) ||
                    (event->xmotion.y > (int) (source_y + source_height)))
                        return;
 

                /*
                 * If the drag is just starting, set initial button down coords
                 */
                if (de->initialX == -1 && de->initialY == -1) {
                        de->initialX = event->xmotion.x;
                        de->initialY = event->xmotion.y;
                }
                /*
                 * Find out how far pointer has moved since button press
                 */
                diffX = de->initialX - event->xmotion.x;
                diffY = de->initialY - event->xmotion.y;
 
                if ((ABS(diffX) >= DRAG_THRESHOLD) ||
                    (ABS(diffY) >= DRAG_THRESHOLD)) {
                        de->doing_drag = True;
			StandaloneApptDragStart(dragInitiator, event, de, StandAloneEditor);
                        de->initialX = -1;
                        de->initialY = -1;
                }
        }
}

/*
**  Static callbacks
*/
static void
de_quit_handler(Widget w, XtPointer cdata, XtPointer data) {
	DTCM_editor *de = (DTCM_editor *)cdata;
	int answer;

	if (de->modified == True) {
	  	char *title = XtNewString(catgets(de->c->DT_catd, 1, 1008, "Calendar Appointment : Help"));
		char *text = XtNewString(catgets(de->c->DT_catd, 1, 451, "You have made unsaved changes.\nYou may save your changes, discard your changes, \nor return to your previous place in the dialog."));
		char *ident1 = XtNewString(catgets(de->c->DT_catd, 1, 452, "Save"));
		char *ident2 = XtNewString(catgets(de->c->DT_catd, 1, 700, "Discard"));
		char *ident3 = XtNewString(catgets(de->c->DT_catd, 1, 923, "Cancel"));
                answer = dialog_popup(de->top_level,
                        DIALOG_TITLE, title,
                        DIALOG_TEXT, text,
                        BUTTON_IDENT, 1, ident1,
                        BUTTON_IDENT, 2, ident2,
                        BUTTON_IDENT, 3, ident3,
                        DIALOG_IMAGE, de->pu->xm_warning_pixmap,
                        NULL);
		XtFree(ident3);
		XtFree(ident2);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);

		if (answer == 1) {
			de_apply_proc(NULL, (XtPointer)de, NULL);
			XtPopdown(de->top_level);
		}
		else  if (answer == 2) {
			if (de->contract)
				tttk_message_fail(de->contract, TT_DESKTOP_ECANCELED, NULL, True);
			XtPopdown(de->top_level);
		}
		else  if (answer == 3) {
			return;
		}

	}
	else {
		if (de->contract)
			tttk_message_fail(de->contract, TT_DESKTOP_ECANCELED, NULL, True);
		XtPopdown(de->top_level);
	}

	free(de->c);
	free(de->dssw);
	if (de->file)
		free(de->file);
	free(de->p);
	free(de->pu);
	free(de->rfp);
	free(de);
	exit(0);
}

static void
load_from_file(DTCM_editor *de) {
	int			i;
	CmDataList		*list = NULL;
	Dtcm_appointment	*appt;

	list = CmDataListCreate();
	parse_appt_from_file(de->c->DT_catd, de->file, list, de->p, 
			     query_user, de->c, DATAVER_ARCHIVE);
	if (appt = (Dtcm_appointment *)CmDataListGetData(list, 1)) {
		dssw_attrs_to_form(de->dssw, appt);
		rfp_attrs_to_form(de->rfp, appt);
	}
	for (i = 1; i <= list->count; i++)
		if (appt = (Dtcm_appointment *)
		    CmDataListGetData(list, i)) {
			if (de->orig_appt)
				free_appt_struct(&de->orig_appt);

			de->orig_appt = appt;
		}
	CmDataListDestroy(list, B_FALSE);
	de->dsswFlags = de->rfpFlags = 0;
}

static void
de_set_defaults(DTCM_editor *de) {

	if (de->file && (access(de->file, R_OK) == 0)) {
		load_from_file(de);
	} else {
		set_dssw_defaults(de->dssw, now(), True);
		set_rfp_defaults(de->rfp);
		de->dsswFlags = de->rfpFlags = 0;
	}

	de->modified = False;
}

static void
de_apply_proc(Widget w, XtPointer client_data, XtPointer data) {
	char			*str;
	FILE			*fp;
	DTCM_editor		*de = (DTCM_editor *)client_data;
	Dtcm_appointment	*appt;
	int			old_attr_count;
	Display			*dpy = XtDisplayOfObject(w);

	appt = allocate_appt_struct(appt_write, 
				    	DATAVER_ARCHIVE,
					NULL);
	load_appt_defaults(appt, de->p);
	de->dsswFlags = 0;
	dssw_form_to_appt(de->dssw, appt, de->c->calname, now());
	de->rfpFlags = 0;
	rfp_form_to_appt(de->rfp, appt, de->c->calname);

	/* save the old count of attributes so that when this appointment 
	   gets freed, we can prevent the added attribute references from 
	   being freed. */

	old_attr_count = appt->count;
	merge_old_values(de->orig_appt, appt);
	str = parse_attrs_to_string(appt, de->p, attrs_to_string(appt->attrs, appt->count));
	appt->count = old_attr_count;
	de->modified = False;

	if (!str)
		return;

	/* we need to distinguish between whether the data is being 
	   saved as part of application termination, or as part of 
	   pressing the "Apply" button.  If it's is app termination, 
	   we need to call ttmedia_load_reply, and if it is an 
	   intermediate save, we are supposed to call ttmedia_Deposit.  
	   Hmmph.  We also need to be pretty clear about saving it back 
	   in the same fashion is was supplied.  If it came as part of 
	   a buffer, it needs to go back that way. */


	/* The first case is from application exit */

	if (w == NULL) {

		/* if it was a file transfer, we should write out the 
		   file, and set the buffer to null.  If it came from 
		   a buffer, we don't bother to write out the file. */

		if (de->read_only) {
			ttmedia_load_reply(de->contract, NULL, 0, True);
		}
		else {
			if (de->init == file_tt) {
				fp = fopen(de->file, "w");

				if (!fp) {
					XBell(dpy, 50);
					return;
				}

				fprintf(fp, "%s", str);
				fclose(fp);
				ttmedia_load_reply(de->contract, NULL, 0, True);
			}
			else {
				ttmedia_load_reply(de->contract, (unsigned char *) str, strlen(str), True);
			}
		}
	}
	else {

		/* This case is from the "Apply" button.  In this case, 
		   if the app was started normally (no tt), then we 
		   just write out the file.  If it was from tt, we then 
		   use ttmedia_Deposit to send the data back. */

		if ((de->init == no_tt) || (de->init == file_tt)) {
			fp = fopen(de->file, "w");

			if (!fp) {
				XBell(dpy, 50);
				return;
			}

			fprintf(fp, "%s", str);
			fclose(fp);

			if (de->init == file_tt)
				ttmedia_Deposit(de->contract,
						NULL,
						de->vtype,
						NULL,
						0,
						de->file,
						app,
						30000);
		}
		else {
			ttmedia_Deposit(de->contract,
					NULL,
					de->vtype,
					(const unsigned char *)str,
					strlen(str),
					NULL,
					app,
					30000);
		}

	}

	free(str);
	free_appt_struct(&appt);
}

static void
de_reset_proc(Widget w, XtPointer client_data, XtPointer data) {
	DTCM_editor	*de = (DTCM_editor *)client_data;

	de_set_defaults(de);
	de->dsswFlags = de->rfpFlags = 0;
}

static void
display_command_usage() {
	fprintf(stderr, "\ndtcm_editor Usage:  dtcm [filename]\n\n");
	exit(0);
}

static void
OKCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild((Widget) client_data);
}

void
DieFromToolTalkError(DTCM_editor *de, char *errfmt, Tt_status status)
{
    Arg		 args[10];
    Widget	 dialog, dialogShell;
    char	*errmsg, *statmsg, *title;
    XmString	 xms_errmsg, xms_ok, xms_title;
    int		 n;

    if (! tt_is_err(status)) return;

    statmsg = tt_status_message(status);
    errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
    sprintf(errmsg, errfmt, statmsg);

    xms_ok = XmStringCreateLocalized(catgets(de->c->DT_catd, 2, 3, "OK"));
    xms_errmsg = XmStringCreateLocalized(errmsg);
    xms_title = XmStringCreateLocalized(catgets(de->c->DT_catd, 2, 4,
			"Calendar : Appointment Editor - Warning"));

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNokLabelString, xms_ok); n++;
    XtSetArg(args[n], XmNdialogTitle, xms_title); n++;
    XtSetArg(args[n], XmNmessageString, xms_errmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_WARNING); n++;

    dialog = XmCreateMessageDialog(de->top_level, "IconEditorError", args, n);
    XtAddCallback(dialog, XmNokCallback, OKCB, (XtPointer) dialog);
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

    /*
     * Disable the frame menu from dialog since we don't want the user
     * to be able to close dialogs with the frame menu
     */
    dialogShell = XtParent(dialog);
    n = 0;
    XtSetArg(args[n], XmNmwmDecorations, MWM_DECOR_ALL | MWM_DECOR_MENU); n++;
    XtSetValues(dialogShell, args, n);
    XtManageChild(dialog);
    XtRealizeWidget(dialogShell);

    _DtSimpleError("Dtcm", DtWarning, NULL, errmsg);

    XtFree(errmsg);
    XmStringFree(xms_ok);
    XmStringFree(xms_errmsg);
    XmStringFree(xms_title);
}



/* 
 * Initialize tooltalk.  Can be called multiple times: the first call
 * initializes tooltalk, subsequent calls are no-ops.
 *
 * Returns
 *		-1	Error.  Tooltalk not initialized
 *		0	Tooltalk already initialized
 *		1	Tooltalk succussfully intialized
 */

Tt_status
cmtt_init(
	char		*toolname,
	DTCM_editor	*de,
	XtAppContext	context,
	Widget		 shell)

{
	static int	initialized = 0;
	int		ttfd;
	Tt_status	status;
	char            *ttenv;
	char            *session;

	if (de->c->tt_procid) {
		return 0;
	}

        ttenv = (char *)getenv("TT_SESSION");
        if (!ttenv || strlen(ttenv) == 0) {
                session = tt_X_session(XDisplayString(XtDisplay(shell)));
                tt_default_session_set(session);
                tt_free(session);
        }

	de->c->tt_procid = ttdt_open(&ttfd, toolname, "SunSoft", "%I", 1);
	status = tt_ptr_error(de->c->tt_procid);
	if (tt_is_err(status)) {
		de->c->tt_procid = NULL;
		return status;
	}

	/*
	 * Declare our ptype, and register the callback to handle
	 * Edit/Display/Compose requests
	 */
	status = ttmedia_ptype_declare(ptype, 0, load_cb, (void *)de, 1);

	if (tt_is_err(status)) {
		fprintf(stderr, "cmtt_init could not declare ptype: %s\n",
			tt_status_message(status));
		return status;
	}

	status = ttmedia_ptype_declare(ptype, 1000, load_cb, (void *)de, 0);

	if (tt_is_err(status)) {
		fprintf(stderr, "cmtt_init could not declare ptype: %s\n",
			tt_status_message(status));
	}

	ttdt_session_join(0, NULL, shell, de->c, 1);

	XtAppAddInput(context, ttfd, (XtPointer)XtInputReadMask,
			tttk_Xt_input_handler, de->c->tt_procid);

	tttk_Xt_input_handler( 0, 0, 0 );

	return TT_OK;
}

Tt_message
reply_cb(Tt_message m, 
	void *c_data, 
	Tttk_op op, 
	unsigned char *contents, 
	int len, 
	char *file)
{
        char *client_procID = tt_message_handler(m);
        if ( debug && (client_procID != NULL) ) {
                fprintf(stderr, "DEBUG: reply_cb():client_procID = %s\n", client_procID);
                fprintf(stderr, "DEBUG: reply_cb():message_op = %s\n", tt_message_op(m));
        }
        return(m);
}

static Tt_message
contract_cb(
	Tt_message	msg,
	void		*clientdata,
	Tt_message	contract)
{

	/* For now do nothing 

	switch (op) {
	case TTDT_QUIT:
	case TTDT_GET_STATUS:
	case TTDT_PAUSE:
	case TTDT_RESUME:
		break;
	}
*/
	return msg;
}

/*
 * Handle Edit, Display and Compose requests
 */
static Tt_message
load_cb(
	Tt_message	msg,
	void		*clientdata,
	Tttk_op		op,
	Tt_status	diagnosis,
	unsigned char	*contents,
	int		len,
	char 		*file,
	char 		*docname
)

{
	Tt_status status;
	char	*p;
	DTCM_editor	*de;
	FILE		*fp;
	char		filename[20];
	CmDataList		*list = NULL;
	Dtcm_appointment	*appt;
	int		i;

	de = (DTCM_editor *)clientdata;

	de->vtype = strdup(tt_message_arg_type(msg, 0));

	if (diagnosis != TT_OK) {
		if (tt_message_status(msg) == TT_WRN_START_MESSAGE) {
			/*
			 * Error in start message!  we may want to exit
			 * here, but for now let toolkit handle error
			 */
			 return msg;
		}

		/* Let toolkit handle the error */
		return msg;
	}

	de->contract_pats = ttdt_message_accept(msg, contract_cb, clientdata,
						de->c->frame, 1, 1);

	tt_ptype_undeclare(ptype);

	if ((status = tt_ptr_error(de->contract_pats)) != TT_OK) {
		fprintf(stderr, "dtcm: load_cb could not accept message: %s\n",
			tt_status_message(status));
	} else {
	/*
		tttk_patterns_destroy(de->contract_pats);
	*/
	}

	de->read_only = False;

	switch (op) {

	case TTME_COMPOSE:
			XtSetSensitive(de->attach_button, True);
			break;
	case TTME_DISPLAY:
	case TTME_EDIT:
			/* for Display only messages, the "Attach" 
			   button makes no sense */

			if (op == TTME_EDIT)
				XtSetSensitive(de->attach_button, True);
			else {
				XtSetSensitive(de->attach_button, False);
				de->read_only = True;
			}
			list = CmDataListCreate();
			if (file == NULL)
			{
				/*
				 * Save data to a file so we can pass it to parse_appt_from_file
				 */
				
				strcpy(filename, "/tmp/cmXXXXXX");
				mktemp(filename);
				if ((fp = fopen(filename, "w")) == 0) {
					tttk_message_fail( msg, TT_DESKTOP_ENODATA, 0, 1 );
					return 0;
				}

				fwrite(contents, 1, len, fp);
				fclose(fp);
				parse_appt_from_file(de->c->DT_catd, filename, 
						list, de->p, query_user, 
						de->c, DATAVER_ARCHIVE);
				unlink(filename);
				de->init = buffer_tt;
			}
			else
			{
				de->file = strdup(file);
				parse_appt_from_file(de->c->DT_catd, de->file, 
						list, de->p, query_user, 
						de->c, DATAVER_ARCHIVE);
				de->init = file_tt;
			}

			if (appt = (Dtcm_appointment *)CmDataListGetData(list, 1)) {
				dssw_attrs_to_form(de->dssw, appt);
				rfp_attrs_to_form(de->rfp, appt);
			}
			for (i = 1; i <= list->count; i++)
				if (appt = (Dtcm_appointment *)
			    	CmDataListGetData(list, i)) {
					if (de->orig_appt)
						free_appt_struct(&de->orig_appt);

					de->orig_appt = appt;
				}
			CmDataListDestroy(list, B_FALSE);

			break;
	}

	de->contract = msg;
	de->modified = False;
	de->dsswFlags = de->rfpFlags = 0;

	tt_free((caddr_t)contents);
	tt_free(file);
	tt_free(docname);

	return 0;
}

static void
handle_drop_cb(
	Widget		w,
	XtPointer	client_data,
	XtPointer	call_data)
{
	Display		*display = XtDisplay(w);
	DtDndDropCallbackStruct *transfer_info = (DtDndDropCallbackStruct *)call_data;
	DTCM_editor	*de;
	char		filename[20];
	char		*data;
	int		size;
	FILE		*fp;
	int		i;

	de = (DTCM_editor *)client_data;

	transfer_info->status = DtDND_SUCCESS;

	for (i = 0; i < transfer_info->dropData->numItems; i++) {
		switch(transfer_info->dropData->protocol) {
		case DtDND_FILENAME_TRANSFER:
			/* REMIND -- handle multiple filenames */
			data = transfer_info->dropData->data.files[0];
	
			de->file = strdup(data);
			load_from_file(de);
			break;
		case DtDND_BUFFER_TRANSFER:
	
			/*
			 * Save data to a file so we can pass it to drag_load_proc().
			 */
			strcpy(filename, "/tmp/cmXXXXXX");
			mktemp(filename);
	
			if ((fp = fopen(filename, "w")) == 0) {
				transfer_info->status = DtDND_FAILURE;
				return;
			}
	
			data = transfer_info->dropData->data.buffers[0].bp;
			size = transfer_info->dropData->data.buffers[0].size;
			fwrite(data, 1, size, fp);
			fclose(fp);
	
			de->file = strdup(filename);
			load_from_file(de);
	
			unlink(filename);
			break;
		default:
			transfer_info->status = DtDND_FAILURE;
			return;
		}
	}

	return;
}

void
de_register_drop_site(
	DTCM_editor	*de,
	Widget		w,
	Boolean		registerchildren)

{
	XtCallbackRec	transfer_cb_rec[] = { {handle_drop_cb, NULL},
					      {NULL, NULL} };
	Display		*display = XtDisplayOfObject(w);

	transfer_cb_rec[0].closure = (XtPointer)de;

	DtDndVaDropRegister(w, DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
			XmDROP_COPY | XmDROP_MOVE,
			transfer_cb_rec, 
			DtNregisterChildren, registerchildren,  
			NULL);
	return;
}

/*
**  Main line
*/
int 
main(int argc, char **argv) {
	int		dssw_loffset, rfp_loffset, start, stop;
	Dimension	dssw_x, rfp_x;
	DTCM_editor	*de;
	XmString	xmstr;
	Boolean		btn1_transfer;
	Dimension	width, longest_dssw_label, longest_rfp_label;
	WidgetList	children;
	Widget		widgets[20];
	int		i = 0;
	int		j = 0;
	int		n;
	char		*title;
	XmString	label_str;
	Tt_status	status;

	de = (DTCM_editor *)ckalloc(sizeof(DTCM_editor));
	de->file = NULL;

	if (argc > 1) {
		if (strcasecmp(argv[1], "-h") == 0)
			display_command_usage();
		else if (argv[1][0] != '-')
			de->file = cm_strdup(argv[1]);
	}
	
	XtSetLanguageProc(NULL, NULL, NULL);
        _DtEnvControl(DT_ENV_SET); /* set up environment variables */

	de->top_level = XtVaAppInitialize(&app,
		"Dtcm", NULL, 0, &argc, argv, NULL,
		XmNallowShellResize, True,
		XmNdeleteResponse, XmDO_NOTHING,
		NULL);
	setup_quit_handler(de->top_level, de_quit_handler, (caddr_t)de);

	/*
	**  First, create a calendar and fill only the stuff we're going to
	**  use.
	*/
	calendar = (Calendar *)ckalloc(sizeof(Calendar));
	de->c = calendar;
	de->p = (Props *)ckalloc(sizeof(Props));
	de->c->properties = (caddr_t)de->p;
	de->c->general = (General*) ckalloc(sizeof(General));
	de->c->general->version = DATAVER_ARCHIVE;
	de->c->frame = de->top_level;
	de->pu = (Props_pu *)ckalloc(sizeof(Props_pu));
	de->c->properties_pu = (caddr_t)de->pu;
	de->init = no_tt;
	read_props(de->p);
	cal_convert_cmrc(de->p);
	if ((start = get_int_prop(de->p, CP_DAYBEGIN)) < 0)
		start = 0;
	else if (start > 22)
		start = 22;
	if ((stop = get_int_prop(de->p, CP_DAYEND)) <= start)
		stop = start + 1;
	else if (stop > 23)
		stop = 23;
	set_int_prop(de->p, CP_DAYBEGIN, start);
	set_int_prop(de->p, CP_DAYEND, stop);
        de->c->calname = cm_strdup(get_char_prop(de->p, CP_DEFAULTCAL));
        de->c->DT_catd = catopen(DTCM_CAT, NL_CAT_LOCALE);

	/* Open the message catalog for internationalization */
	calendar->DT_catd = catopen(DTCM_CAT, NL_CAT_LOCALE);

	title = XtNewString(catgets(calendar->DT_catd, 1, 1074, 
						"Calendar Appointment"));
	XtVaSetValues(de->top_level, 
		XmNtitle, title,
		NULL);
	XtFree(title);

	/*
        **  Okay, now create the form manager and the widgets
	*/
	de->form = XtVaCreateWidget("form",
		xmFormWidgetClass, de->top_level,
		XmNautoUnmanage, True,
		XmNfractionBase, 5,
		NULL);

	label_str = XmStringCreateLocalized(catgets(de->c->DT_catd, 1, 846, "Save"));
	de->attach_button = XtVaCreateWidget("attach_button",
		xmPushButtonGadgetClass, de->form,
		XmNlabelString, label_str,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 1,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 2,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 10,
		NULL);
	XmStringFree(label_str);
	XtAddCallback(de->attach_button, XmNactivateCallback, de_apply_proc, (XtPointer)de);
	if (!de->file)
		XtSetSensitive(de->attach_button, False);

	label_str = XmStringCreateLocalized(catgets(de->c->DT_catd, 1, 691, "Reset"));
	de->reset_button = XtVaCreateWidget("reset_button",
		xmPushButtonGadgetClass, de->form,
		XmNlabelString, label_str,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 3,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 10,
		NULL);
	XmStringFree(label_str);
	XtAddCallback(de->reset_button, XmNactivateCallback, de_reset_proc, de);

	create_all_pixmaps(de->pu, de->form);

	de->rfpFlags = 0;
	de->rfp = (RFP *)ckalloc(sizeof(RFP));
	build_rfp(de->rfp, de->c, de->form);
	XtVaSetValues(de->rfp->rfp_form_mgr,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, de->attach_button,
		XmNbottomOffset, 25,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 5,
		NULL);
	XtVaGetValues(de->rfp->rfp_form_mgr,
		XmNchildren,            &children,
		XmNnumChildren,         &n,
		NULL);
	/* We don't want to manage the privacy widgets */
        for (i = 0; i < n; i++) {
                if ((children[i] == de->rfp->privacy_label) ||
                    (children[i] == de->rfp->privacy_menu))
                    continue;
                widgets[j++] = children[i];
        }
        XtManageChildren(widgets, n - 2);	

	/*
	 * Add a drag source icon inside the dssw, lower right
	 */
	xmstr = XmStringCreateLocalized(
			catgets(de->c->DT_catd, 1, 627, "Drag Appt"));
	de->drag_source = XtVaCreateWidget("drag_source",
		dtIconGadgetClass, de->form,
		XmNpixmapPosition, XmPIXMAP_TOP,
		XmNstringPosition, XmSTRING_BOTTOM,
		XmNalignment, XmALIGNMENT_CENTER,
		XmNstring, xmstr,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, de->attach_button,
		XmNbottomOffset, 25,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, de->rfp->rfp_form_mgr,
		XmNrightAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,
		NULL);
	XmStringFree(xmstr);

        XtAddEventHandler(XtParent(de->drag_source), Button1MotionMask, False,
                (XtEventHandler)EditApptDragMotionHandler, (XtPointer) de);

        XtVaGetValues((Widget)XmGetXmDisplay(XtDisplay(de->form)),
                "enableBtn1Transfer",   &btn1_transfer,
                NULL);

	/* btn1_transfer is a tri-state variable - see 1195846 */
	if ((Boolean)btn1_transfer != True)
                XtAddEventHandler(XtParent(de->drag_source),
                                Button2MotionMask, False,
                                (XtEventHandler)EditApptDragMotionHandler,
                                (XtPointer) de);


        if (de->pu->drag_icon_xbm)
                XtVaSetValues(de->drag_source,
                                XmNpixmap, de->pu->drag_icon_xbm,
                                NULL);

	de->dsswFlags = 0;
	de->dssw = (DSSW *)ckalloc(sizeof(DSSW));
	build_dssw(de->dssw, de->c, de->form, True, True);
	XtVaSetValues(de->dssw->dssw_form_mgr,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 10,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, de->rfp->rfp_form_mgr,
		XmNbottomOffset, 15,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 10,
		NULL);
	ManageChildren(de->dssw->dssw_form_mgr);

	/* set up callback to detect whether the appointment 
	   definition has been modified */

	XtAddCallback(de->dssw->start_text, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->dssw->start_am, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->dssw->start_pm, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->dssw->stop_text, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->dssw->stop_am, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->dssw->stop_pm, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->dssw->what_text, XmNvalueChangedCallback, de_mark_change, de);
	XtAddCallback(de->rfp->repeat_menu, XmNselectionCallback, de_mark_change, de);
	XtAddCallback(de->rfp->for_menu, XmNselectionCallback, de_mark_change, de);

	ManageChildren(de->form);
	XtManageChild(de->form);
	XtRealizeWidget(de->top_level);

	/*
	**  Do some monkeying to compensate for Motif's pitiful form managers
	XtVaGetValues(de->dssw->start_menu, XmNx, &dssw_x,
		XmNleftOffset, &dssw_loffset,
		NULL);
	XtVaGetValues(de->rfp->repeat_menu, XmNx, &rfp_x,
		XmNleftOffset, &rfp_loffset,
		NULL);

	if (dssw_x > rfp_x)
		XtVaSetValues(de->rfp->rfp_form_mgr,
			XmNleftOffset, rfp_loffset + (dssw_x - rfp_x),
			NULL);
	else if (rfp_x > dssw_x)
		XtVaSetValues(de->dssw->dssw_form_mgr,
			XmNleftOffset, dssw_loffset + (rfp_x - dssw_x),
			NULL);
	*/

/* 	Don't need these any more.	
	XtVaGetValues(de->dssw->date_label, XmNwidth, &longest_dssw_label, NULL);

	XtVaGetValues(de->dssw->start_label, XmNwidth, &width, NULL);
	if (width > longest_dssw_label)
		longest_dssw_label = width;

	XtVaGetValues(de->dssw->stop_label, XmNwidth, &width, NULL);
	if (width > longest_dssw_label)
		longest_dssw_label = width;

	XtVaGetValues(de->dssw->what_label, XmNwidth, &width, NULL);
	if (width > longest_dssw_label)
		longest_dssw_label = width;

	XtVaGetValues(de->rfp->frequency_label, XmNwidth, &longest_rfp_label, NULL);
	XtVaGetValues(de->rfp->for_label, XmNwidth, &width, NULL);
	if (width > longest_rfp_label)
		longest_rfp_label = width;

	XtVaSetValues(de->dssw->dssw_form_mgr, 
			XmNleftOffset, longest_rfp_label - longest_dssw_label, 
			NULL);
*/

	de_set_defaults(de);

	de_register_drop_site(de, de->form, True);
	de_register_drop_site(de, de->dssw->dssw_form_mgr, False);
	de_register_drop_site(de, de->rfp->rfp_form_mgr, False); 

        XmProcessTraversal(de->dssw->what_text, XmTRAVERSE_CURRENT);
        XtVaSetValues(de->form, XmNinitialFocus, de->dssw->what_text, NULL);


	status = cmtt_init("AppointmentEditor", de, app, calendar->frame);
	if (TT_OK != status) {
	    char *errfmt;
    	    errfmt = catgets(calendar->DT_catd, 2, 2,
			"Could not connect to ToolTalk:\n%s\n");
            DieFromToolTalkError( de, errfmt, status );
	}

	XtAppMainLoop(app);

	return 0;
}
