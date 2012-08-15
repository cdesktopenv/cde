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
/* $TOG: dnd.c /main/12 1998/04/09 11:43:47 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <X11/Xlib.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/DragC.h>
#include <Xm/DragIcon.h>
#include <Xm/AtomMgr.h>
#include <Xm/ToggleBG.h>
#include <Dt/Dt.h>
#include <Dt/Dnd.h>
#include <csa.h>

#include "util.h"
#include "editor.h"
#include "todo.h"
#include "group_editor.h"
#include "calendar.h"
#include "props_pu.h"
#include "props.h"
#include "dnd.h"
#include "getdate.h"
#include "cm_tty.h"
#include "misc.h"
#include "help.h"

#ifdef FNS
#include "cmfns.h"
#endif

#include "drag_xbm"
#include "drag_mask_xbm"

static Bool lookForButton(Display *, XEvent *, XPointer);

#if !defined(linux) && !defined(__FreeBSD__)
extern char	*sys_errlist[];
#endif

extern int	drag_load_proc(char*, Calendar *);
static char	dnd_filename[20];

static Boolean 
validate_dropped_appt(char *filename, Calendar *c) {
	Props			*p = (Props *)c->properties;
	Props_pu		*pu = (Props_pu *)c->properties_pu;
	CmDataList		*list = CmDataListCreate();
	Validate_op		op;
	int			i;
	Dtcm_appointment	*a;

	if (!filename || *filename == '\0')
		return(False);

	op = parse_appt_from_file(c->DT_catd, filename, list, p, query_user, 
				  (void *)c, c->general->version);

	for (i = 1; i <= list->count; i++)
		if (a = (Dtcm_appointment *)CmDataListGetData(list, i))
			free_appt_struct(&a);
	CmDataListDestroy(list, B_FALSE);

	if (op == VALID_APPT)
		return(True);
	else
		return(False);

}

static void
handle_animate_cb(
	Widget		dragContext,
	XtPointer	client_data,
	XtPointer	call_data)
{
        DtDndDropAnimateCallbackStruct *animateInfo = (DtDndDropAnimateCallbackStruct *)call_data;
	Calendar	*c;
	char		*data;
	int		size;
	FILE		*fp;
	int		i;
#if defined(FNS) && defined(FNS_DEMO)
	char		buf[256];
	char		addr_buf[256];
#endif

	c = (Calendar *)client_data;

	for (i = 0; i < animateInfo->dropData->numItems; i++) {
		switch(animateInfo->dropData->protocol) {
		case DtDND_FILENAME_TRANSFER:
			data = animateInfo->dropData->data.files[i];
	
#if defined(FNS) && defined(FNS_DEMO)
			if (cmfns_use_fns(c->properties) &&
			    cmfns_name_from_file(data, buf, sizeof(buf)) == 1) {
				/* 
				 * Looks like an HFS file has been dropped on us.
				 * Get the calendar service associated with the
				 * FNS name and browse it
				 */
				if (cmfns_lookup_calendar(buf,
						addr_buf, sizeof(addr_buf)) == 1) {
					
					switch_it(c, addr_buf, main_win);
					return;
				}
			}
#endif
			drag_load_proc(data, c);
			break;
		case DtDND_BUFFER_TRANSFER:
	
			/*
			 * Save data to a file so we can pass it to drag_load_proc().
			 */

#ifdef NOT
			strcpy(filename, "/tmp/cmXXXXXX");
			mktemp(filename);
#endif

			if (!dnd_filename[0]){
				return;
			}
	
#ifdef NOT
	
			if ((fp = fopen(dnd_filename, "w")) == 0) {
				return;
			}
	
			data = animateInfo->dropData->data.buffers[0].bp;
			size = animateInfo->dropData->data.buffers[0].size;
			fwrite(data, 1, size, fp);
			fclose(fp);
#endif
	
			drag_load_proc(dnd_filename, c);
	
			unlink(dnd_filename);
			dnd_filename[0] = NULL;
			break;
		default:
			return;
		}
	}

	return;
}

static void
handle_drop_cb(
	Widget		w,
	XtPointer	client_data,
	XtPointer	call_data)
{
	Display		*display = XtDisplay(w);
	DtDndDropCallbackStruct *transfer_info = (DtDndDropCallbackStruct *)call_data;
	Calendar	*c;
	char		filename[20];
	char		*data;
	int		size;
	FILE		*fp;
	int		i;
#if defined(FNS) && defined(FNS_DEMO)
	char		buf[256];
	char		addr_buf[256];
#endif

	c = (Calendar *)client_data;

	transfer_info->status = DtDND_SUCCESS;

	for (i = 0; i < transfer_info->dropData->numItems; i++) {
		switch(transfer_info->dropData->protocol) {
		case DtDND_FILENAME_TRANSFER:
			data = transfer_info->dropData->data.files[i];
	
#if defined(FNS) && defined(FNS_DEMO)
			if (cmfns_use_fns(c->properties) &&
			    cmfns_name_from_file(data, buf, sizeof(buf)) == 1) {
				/* 
				 * Looks like an HFS file has been dropped on us.
				 * Get the calendar service associated with the
				 * FNS name and browse it
				 */
				if (cmfns_lookup_calendar(buf,
						addr_buf, sizeof(addr_buf)) == 1) {
					
					switch_it(c, addr_buf, main_win);
					return;
				}
			}
#endif
	
			if (validate_dropped_appt(data, c) == False) {
				transfer_info->status = DtDND_FAILURE;
			}
			break;
		case DtDND_BUFFER_TRANSFER:
	
			/*
			 * Save data to a file so we can pass it to drag_load_proc().
			 */
			strcpy(dnd_filename, "/tmp/cmXXXXXX");
			mktemp(dnd_filename);
	
			if ((fp = fopen(dnd_filename, "w")) == 0) {
				transfer_info->status = DtDND_FAILURE;
				return;
			}
	
			data = transfer_info->dropData->data.buffers[0].bp;
			size = transfer_info->dropData->data.buffers[0].size;
			fwrite(data, 1, size, fp);
			fclose(fp);
	
			if (validate_dropped_appt(dnd_filename, c) == False) {
				unlink(dnd_filename);
				dnd_filename[0] = NULL;	
				transfer_info->status = DtDND_FAILURE;
			}
#ifdef NOT
			unlink(filename);
#endif
			break;
		default:
			transfer_info->status = DtDND_FAILURE;
			return;
		}
	}

	return;
}

void
cm_register_drop_site(
	Calendar	*c,
	Widget		w)

{
	XtCallbackRec	transfer_cb_rec[] = { {handle_drop_cb, NULL},
					      {NULL, NULL} };
	static XtCallbackRec animateCBRec[] = { {handle_animate_cb, NULL},
						{NULL, NULL} };
	Display		*display = XtDisplayOfObject(w);

	/*
	 * The above string "CalendarAppointment" is hard coded to match the type
	 * used in dtdnddemo.  In the future we need to use the true
	 * type from the data typing database
	 */

	transfer_cb_rec[0].closure = (XtPointer)c;
	animateCBRec[0].closure = (XtPointer)c;

	DtDndVaDropRegister(w, DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
			XmDROP_COPY,
			transfer_cb_rec, 
			DtNdropAnimateCallback, animateCBRec,
			DtNtextIsBuffer, 	True,
			NULL);
	return;
}

/*
 * Insert the appointment into the calendar.
 *
 * Returns:
 *		1	Success
 *		0	User overuled.  Appointment not inserted.
 *		-1	Failure
 */
static int
schedule_appt(Calendar *c, Dtcm_appointment *a) {
	char		date_buf[MAXNAMELEN], buf[BUFSIZ], buf2[BUFSIZ];
	int		answer;
	Editor		*e = (Editor *)c->editor;
	ToDo		*t = (ToDo *)c->todo;
	Props		*p = (Props *)c->properties;
	CSA_entry_handle	entry;
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	SeparatorType	st = get_int_prop(p, CP_DATESEPARATOR);
	Tick		tick;
	int		rc;

	if (strcmp(c->calname, c->view->current_calendar) != 0) {
		/*
		 * Make sure user really meant to insert appointment
		 * into somebody elses calendar.
		 */
		char *ident = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 212,
				"Calendar : Schedule Appointment"));
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 210, "The appointment will be scheduled in the calendar\nyou are currently browsing.  Do you still want to schedule it?"));
		sprintf(buf2, "%s %s", catgets(c->DT_catd, 1, 211, "Schedule in"),
			c->view->current_calendar);
		answer = dialog_popup(c->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, buf,
			BUTTON_IDENT, 1, ident,
			BUTTON_IDENT, 2, buf2,
			NULL);
		XtFree(title);
		XtFree(ident);
		if (answer == 1)
			return 0;
	}

	if (a->end_time) {
        if (!editor_created(e))
	{
                e_make_editor(c);
		XtUnmanageChild(e->base_form_mgr);
		e->editor_is_up = False;
	}
	if ((rc = editor_insert(a, &entry, c)) == True) {

		_csa_iso8601_to_tick(a->time->value->item.string_value, &tick);
		format_tick(tick, ot, st, date_buf);
		sprintf(buf, catgets(c->DT_catd, 1, 214,
				     "Appointment scheduled: %s"), date_buf);
		set_message(c->message_text, buf);
		return 1;
	} else {
		set_message(c->message_text, "");
		if ( rc == 2)
			return 0;
			return -1;
	}
	} else {
	if (!t->frame)
	{
		t_make_todo(c);
		XtUnmanageChild(t->frame);
		t->todo_is_up = False;
	}
	if (todo_insert(a, &entry, c)) {
		/*
		 * No messages displayed on calendar for todo.
		 */
		return 1;
	} else {
		/*
		 * No messages displayed on calendar for todo.
		 */
		return -1;
	}
	}
}

/*
 * Call the routines in file_parse (in libDtCm) to read the appointments!
 */
extern int 
drag_load_proc(char *filename, Calendar *c) {
	int			ret_val, i = 1;
	char			buf[MAXNAMELEN * 2];
	CmDataList		*list = CmDataListCreate();
	Props			*p = (Props *)c->properties;
	Props_pu		*pu = (Props_pu *)c->properties_pu;
	Validate_op		op;
	Dtcm_appointment	*a;
	char			*msg;

	if (!filename || *filename == '\0')
		return -1;

	op = parse_appt_from_file(c->DT_catd, filename, list, p, query_user, 
				  (void *)c, c->general->version);
	if (list->count <= 0) {
		op = CANCEL_APPT;
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 842, 
	     "The information transferred did not\ncontain any appointments."));
	}

	switch(op) {
	case COULD_NOT_OPEN_FILE:
	  	msg = XtNewString(catgets(c->DT_catd, 1, 843, 
					"Drag and Drop operation failed."));
		sprintf(buf, "%s\n%s",
			msg,
			catgets(c->DT_catd, 1, 844, 
			      "Unable to locate the transferred information."));
		XtFree(msg);
		break;
	case INVALID_DATE:
		sprintf(buf, "%s",
			catgets(c->DT_catd, 1, 218, "Invalid DATE specified"));
		break;
	case INVALID_START:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 219,
					   "Invalid START time specified"));
		break;
	case INVALID_STOP:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 220,
					   "Invalid END time specified"));
		break;
	case MISSING_DATE:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 221,
					   "Empty or missing DATE field"));
		break;
	case MISSING_START:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 222,
					   "Empty or missing START field"));
		break;
	case MISSING_WHAT:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 223,
					   "Empty or missing WHAT field"));
		break;
	case REPEAT_FOR_MISMATCH:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 224,
					   "REPEAT and FOR field mismatch"));
		break;
	case VALID_APPT:
		break;
	case CANCEL_APPT:
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 225,
					   "Schedule appointment was cancelled."));
		break;
	default:
		op = CANCEL_APPT;
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 225,
					   "Schedule appointment was cancelled."));
		break;
	}

	while (op == VALID_APPT && i <= list->count) {
		extern void scrub_attr_list(Dtcm_appointment *); 

		a = (Dtcm_appointment *)CmDataListGetData(list, i);

		scrub_attr_list(a);

		ret_val = schedule_appt(c, a);
		if (ret_val < 0) {
			op = CANCEL_APPT;
			sprintf(buf, "%s", catgets(c->DT_catd, 1, 226,
				"Internal error scheduling appointment."));
		} else if (ret_val == 0) {
			op = CANCEL_APPT;
			sprintf(buf, "%s", catgets(c->DT_catd, 1, 225,
				"Schedule appointment was cancelled."));
		}
		++i;
	}

	for (i = 1; i <= list->count; i++)
		if (a = (Dtcm_appointment *)CmDataListGetData(list, i))
			free_appt_struct(&a);
	CmDataListDestroy(list, B_FALSE);

	if (op != VALID_APPT) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 1073,
					  "Calendar : Error - Drag and Drop"));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(c->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, buf,
			BUTTON_IDENT, 1, ident,
			BUTTON_HELP, DND_ERROR_HELP,
			DIALOG_IMAGE, pu->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(title);
		return -1;
	}

	return 0;
}

/* gets a pointer to the currently selected appointment in the editor.  
   This will need to be changed if we ever allow more than one item 
   to be selected in the editor at a time. */

CSA_entry_handle
get_appt_struct(DragContext *context) {
	int             *item_list = NULL, item_cnt = 0, answer;
	char		buf[MAXNAMELEN];
	Widget 		list;
	Calendar 	*c = context->calendar;
	Props_pu	*pr;
	CSA_entry_handle	entry;
	Access_data	*ad;

	pr = (Props_pu *)(c->properties_pu);

	if (context->editor_type == SingleEditorList)
		list = ((Editor *) context->editor)->appt_list;
	else if (context->editor_type == GroupEditorList)
		list = ((GEditor *) context->editor)->appt_list;
	else if (context->editor_type == TodoEditorList)
		list = ((ToDo *) context->editor)->todo_list;

        if (!XmListGetSelectedPos(list, &item_list, &item_cnt)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 230, 
				"Calendar : Error - Drag Appointment"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 231, "Select an appointment and DRAG again."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
                answer = dialog_popup(c->frame,
                        DIALOG_TITLE, title,
                        DIALOG_TEXT, text,
                        BUTTON_IDENT, 1, ident,
			BUTTON_HELP, RESELECT_ERROR_HELP,
                        DIALOG_IMAGE, pr->xm_error_pixmap,
                        NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
                return(NULL);
        }
 

	if (context->editor_type == SingleEditorList)
		entry = editor_nth_appt((Editor *)context->editor,
					item_list[0] - 1);
	else if (context->editor_type == GroupEditorList)
		entry = geditor_nth_appt((GEditor *)context->editor,
					 item_list[0] - 1, &ad);
	else if (context->editor_type == TodoEditorList)
		entry = t_nth_appt((ToDo *)context->editor,
					 item_list[0] - 1);


        if (!entry) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 230, 
					"Calendar : Error - Drag Appointment"));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 845,
		"Drag and Drop operation Failed\nInternal consistency error."));
                answer = dialog_popup(c->frame,
                        DIALOG_TITLE, title,
                        DIALOG_TEXT, buf,
                        BUTTON_IDENT, 1, ident,
                        DIALOG_IMAGE, pr->xm_error_pixmap,
                        NULL);
		XtFree(ident);
		XtFree(title);
                XtFree((XtPointer)item_list);
                return(NULL);
        }

	return entry;
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

/*
 * getIcon
 *
 * Returns a new IconInfo structure with bitmap, mask, width, height,
 * icon type and name.
 */
static void
GetIcon(Calendar *calendar)
{
 
        Display        *display = XtDisplay(calendar->frame);
        Window          window = XtWindow(calendar->frame);
        unsigned char  *bitmapData, *bitmapMask;
        Editor          *e = (Editor *) calendar->editor;
        GEditor         *ge = (GEditor *) calendar->geditor;
 
        if (e->drag_bitmap == NULL) {
                e->drag_bitmap = XCreateBitmapFromData(display,
                        window, (char *) drag_xbm_bits,
                        drag_xbm_width, drag_xbm_height);
                if (e->drag_bitmap == NULL) {

                        printf(catgets(calendar->DT_catd, 1, 237, "XCreateBitmapFromData() failed for bitmap.\n"));
                        return;
                }
                else
                        ge->drag_bitmap = e->drag_bitmap;
        }
        if (e->drag_mask == NULL) {
                e->drag_mask = XCreateBitmapFromData(display,
                        window, (char *) drag_mask_xbm_bits,
                        drag_mask_xbm_width, drag_mask_xbm_height);
                if (e->drag_mask == NULL) {
                        printf(catgets(calendar->DT_catd, 1, 238, "XCreateBitmapFromData() failed for mask.\n"));
                        return;
                }
                else
                        ge->drag_mask = e->drag_mask;
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

	if ((context->editor_type == SingleEditorList) ||
	    (context->editor_type == SingleEditorIcon))
        	((Editor *) context->editor)->doing_drag = False;
	else if ((context->editor_type == TodoEditorList) ||
		 (context->editor_type == TodoEditorIcon))
        	((ToDo *) context->editor)->doing_drag = False;
	else if ((context->editor_type == GroupEditorList) ||
		 (context->editor_type == GroupEditorIcon)) 
        	((GEditor *) context->editor)->doing_drag = False;

        if (context->data)
                free(context->data);

        free(context);
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

void
TranslationDragStart(
        Widget          widget,
        XEvent          *event,
	String		*parms,
	Cardinal	*num_params)
{
        static XtCallbackRec convertCBRec[] = { {ApptConvertCB, NULL},
                                                {NULL, NULL} };
        static XtCallbackRec dragFinishCBRec[] =  { {DragFinishCB, NULL},
                                                    {NULL, NULL} };
 
        Display        *display 	= XtDisplay(widget);
        DragContext     *context = calloc(sizeof(DragContext), 1);
        Editor          *e = (Editor *) calendar->editor;
	CSA_entry_handle	entry;
	char		*apptstr;
	Props		*p = (Props *)calendar->properties;
 
        context->calendar = calendar;

	if (((Editor *)calendar->editor)->appt_list == widget) {
        	context->editor_type = SingleEditorList;
                context->editor = (caddr_t) calendar->editor;
	} else if (((GEditor *)calendar->geditor)->appt_list == widget) { 
		context->editor_type = GroupEditorList;
		context->editor = (caddr_t) calendar->geditor;
	} else if (((ToDo *)calendar->todo)->todo_list == widget) { 
		context->editor_type = TodoEditorList;
		context->editor = (caddr_t) calendar->todo;
	}
	else
	{
	  free(context);
	  return;
	}

	if (((entry = get_appt_struct(context)) == (CSA_entry_handle)NULL) ||
	    ((apptstr = parse_appt_to_string(calendar->cal_handle,
					     entry, p,
					     calendar->general->version))
	     == (char *)NULL))
	{
	  switch (context->editor_type)
	  {
	  case SingleEditorList:
	    ((Editor *)context->editor)->doing_drag = False;
	    break;

	  case GroupEditorList:
	    ((GEditor *)context->editor)->doing_drag = False;
	    break;

	  case TodoEditorList:
	    ((ToDo *)context->editor)->doing_drag = False;
	    break;
	  }

	  free(context);
	  return;
	}

	context->data = apptstr;

        GetIcon(calendar);

        convertCBRec[0].closure = (XtPointer)context;
        dragFinishCBRec[0].closure = (XtPointer)context;

        if (e->drag_icon == NULL) {
                e->drag_icon = CreateDragSourceIcon(widget, 
						    e->drag_bitmap, 
						    e->drag_mask);
        }

        if (DtDndVaDragStart(widget, event, DtDND_BUFFER_TRANSFER, 1,
            		     XmDROP_COPY, 
			     convertCBRec, dragFinishCBRec,
			     DtNsourceIcon, e->drag_icon,
			     NULL) == NULL) {
 
                printf(catgets(calendar->DT_catd, 1, 239, 
					"DragStart returned NULL.\n"));
        }
}
 
void
ApptDragStart(
        Widget          widget,
        XEvent          *event,
        Calendar        *calendar,
        EditorType      editor_type)
{
        static XtCallbackRec convertCBRec[] = { {ApptConvertCB, NULL},
                                                {NULL, NULL} };
        static XtCallbackRec dragFinishCBRec[] =  { {DragFinishCB, NULL},
                                                    {NULL, NULL} };
 
        Display        *display 	= XtDisplay(widget);
        DragContext     *context = calloc(sizeof(DragContext), 1);
	Editor		*e = (Editor *) calendar->editor;
	GEditor		*ge = (GEditor *) calendar->geditor;
	ToDo		*t = (ToDo *) calendar->todo;
	Dtcm_appointment        *appt;
	char		*apptstr;
	int		preDsswFlags, preRfpFlags;
 
        context->calendar = calendar;
        context->editor_type = editor_type;
 
        if (editor_type == SingleEditorIcon)
	{
                context->editor = (caddr_t) e;
		appt = allocate_appt_struct(appt_write, DATAVER_ARCHIVE, NULL);
		load_appt_defaults(appt, (Props *) calendar->properties);
		preDsswFlags = e->dsswFlags;
		preRfpFlags = e->rfpFlags;
		if (!dssw_form_flags_to_appt(&e->dssw, appt,
					     calendar->calname,
					     now(), &e->dsswFlags) ||
		    !rfp_form_flags_to_appt(&e->rfp, appt,
					    calendar->calname,
					    &e->rfpFlags) ||
		    (preDsswFlags != e->dsswFlags) ||
		    (preRfpFlags != e->rfpFlags))
		{
		  e->doing_drag = False;
		  free_appt_struct(&appt);
		  free(context);
		  return;
		}
	}
        else if (editor_type == GroupEditorIcon)
	{
                context->editor = (caddr_t) ge;
		appt = allocate_appt_struct(appt_write, DATAVER_ARCHIVE, NULL);
		load_appt_defaults(appt, (Props *) calendar->properties);
		preDsswFlags = ge->dsswFlags;
		preRfpFlags = ge->rfpFlags;
		if (!dssw_form_flags_to_appt(&ge->dssw, appt,
					     calendar->calname,
					     now(), &ge->dsswFlags) ||
		    !rfp_form_flags_to_appt(&ge->rfp, appt,
					    calendar->calname,
					    &ge->rfpFlags) ||
		    (preDsswFlags != ge->dsswFlags) ||
		    (preRfpFlags != ge->rfpFlags))
		{
		  ge->doing_drag = False;
		  free_appt_struct(&appt);
		  free(context);
		  return;
		}
	}
        else if (editor_type == TodoEditorIcon)
	{
                context->editor = (caddr_t) t;
		if (t->cal->general->version < DATAVER4)
		  appt = allocate_appt_struct(appt_write, DATAVER_ARCHIVE, 
			      CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I,
			      CSA_ENTRY_ATTR_LAST_UPDATE_I,
			      CSA_ENTRY_ATTR_ORGANIZER_I,
			      CSA_ENTRY_ATTR_START_DATE_I,
			      CSA_ENTRY_ATTR_TYPE_I,
			      CSA_ENTRY_ATTR_CLASSIFICATION_I,
			      CSA_ENTRY_ATTR_END_DATE_I,
			      CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
			      CSA_ENTRY_ATTR_SUMMARY_I,
			      CSA_ENTRY_ATTR_STATUS_I,
			      CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
			      CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
			      CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
			      CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
			      CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
			      CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
			      CSA_ENTRY_ATTR_MAIL_REMINDER_I,
			      CSA_ENTRY_ATTR_POPUP_REMINDER_I,
			      NULL);
		else
		  appt = allocate_appt_struct(appt_write,
					      DATAVER_ARCHIVE, NULL); 

		dssw_form_to_todo(&t->dssw, appt, calendar->calname, now());
		preRfpFlags = t->rfpFlags;
		if (!rfp_form_flags_to_appt(&t->rfp, appt,
					    calendar->calname,
					    &t->rfpFlags) ||
		    (preRfpFlags != t->rfpFlags))
		{
		  t->doing_drag = False;
		  free_appt_struct(&appt);
		  free(context);
		  return;
		}
		appt->type->value->item.sint32_value = CSA_TYPE_TODO;
		appt->show_time->value->item.sint32_value = True;
		t->completed_val = 
		  XmToggleButtonGadgetGetState(t->completed_toggle);
		appt->state->value->item.sint32_value = 
		  (t->completed_val) ? 
		    CSA_STATUS_COMPLETED : CSA_X_DT_STATUS_ACTIVE;
	}
	else
	{
	  free(context);
	  return;
	}
	apptstr = parse_attrs_to_string(appt, (Props *)calendar->properties, 
					attrs_to_string(appt->attrs,
							appt->count));
	free_appt_struct(&appt);

	context->data = apptstr;

        GetIcon(calendar);
 
        convertCBRec[0].closure = (XtPointer)context;
        dragFinishCBRec[0].closure = (XtPointer)context;
 
        if (e->drag_icon == NULL) {
                e->drag_icon = CreateDragSourceIcon(widget, e->drag_bitmap,
						    e->drag_mask);
        }
 
        if (DtDndVaDragStart(widget, event, DtDND_BUFFER_TRANSFER, 1,
            	XmDROP_COPY, 
		convertCBRec, dragFinishCBRec,
		DtNsourceIcon,		e->drag_icon,
		NULL)
            == NULL) {
 
                printf(catgets(calendar->DT_catd, 1, 239,
			       "DragStart returned NULL.\n"));
        }
}

#define DAMPING 5
#define ABS_DELTA(x1, x2) (x1 < x2 ? x2 - x1 : x1 - x2)

static Bool
lookForButton(
        Display * display,
        XEvent * event,
        XPointer arg)
{
    if (event->type == MotionNotify)
    {
	XEvent *press = (XEvent *)arg;

	if ((ABS_DELTA(press->xbutton.x_root,
		       event->xmotion.x_root) > DAMPING) ||
            (ABS_DELTA(press->xbutton.y_root,
		       event->xmotion.y_root) > DAMPING))
	    return(True);
    }
    else if (event->type == ButtonRelease)
	return(True);

    return(False);
}

/*
 *
// DtcmProcessPress
//
// Translation implementing Motif 1.2.5 ProcessPress function
//
 * Taken from dtmail/dtmail/RoamMenuWindow.C
 */

#define SELECTION_ACTION        0
#define TRANSFER_ACTION         1

void
DtcmProcessPress(
        Widget          w,
        XEvent          *event,
	String		*params,
	Cardinal	*num_params)
{
   int i, action, cur_item;
   int *selected_positions, nselected_positions;

   /*
    *  This action happens when Button1 is pressed and the Selection
    *  and Transfer are integrated on Button1.  It is passed two
    *  parameters: the action to call when the event is a selection,
    *  and the action to call when the event is a transfer.
    */

    if (*num_params != 2 || !XmIsList(w))
      return;

    action = SELECTION_ACTION;
    cur_item = XmListYToPos(w, event->xbutton.y);

    if (cur_item > 0)
    {
        XtVaGetValues(w,
		XmNselectedPositions, &selected_positions,
		XmNselectedPositionCount, &nselected_positions,
		NULL);

	for (i=0; i<nselected_positions; i++)
	{
	    if (cur_item == selected_positions[i])
	    {
		/*
		 * The determination of whether this is a transfer drag
		 * cannot be made until a Motion event comes in.  It is
		 * not a drag as soon as a ButtonUp event happens.
		 */
		XEvent new_event;

		XPeekIfEvent(
			XtDisplay(w),
			&new_event,
			lookForButton,
			(XPointer)event);
                switch (new_event.type)
                {
                    case MotionNotify:
      	               action = TRANSFER_ACTION;
                       break;
                    case ButtonRelease:
        	       action = SELECTION_ACTION;
                       break;
                }
		break;
	    }
	}
    }

    XtCallActionProc(w, params[action], event, params, *num_params);
}
