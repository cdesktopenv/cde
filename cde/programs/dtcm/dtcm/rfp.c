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
/* $XConsortium: rfp.c /main/10 1996/10/14 16:07:07 barstow $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdlib.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushB.h>
#include <Xm/MwmUtil.h>
#include <Xm/ComboBox.h>
#include "util.h"
#include "timeops.h"
#include "misc.h"
#include "rfp.h"
#include "props_pu.h"
#ifdef SVR4
#include <sys/param.h>
#endif /* SVR4 */

#define	PRIVATE_FOREVER -9999999
#define GAP 5

extern Dimension ComputeMaxWidth(Widget, Widget, Widget, Widget);

static char *dow_str(time_t);
static void change_to_last_week(Dtcm_appointment *, int, time_t, RFP *);

/*******************************************************************************
**
**  Static functions visibile to rfp.c only
**
*******************************************************************************/
/*
**  Set the necessary for stuff grey/ungrey
*/
static void
rfp_toggle_for_grey(RFP *rfp, Boolean status) {
	Widget	list, text;

	XtVaSetValues(rfp->for_label, XmNsensitive, status, NULL);
	XtVaGetValues(rfp->for_menu, XmNlist, &list, XmNtextField, &text, NULL);
	XtVaSetValues(list, XmNsensitive, status, NULL);
	XtVaSetValues(text, XmNsensitive, status, NULL);
	XtVaSetValues(rfp->for_scope, XmNsensitive, status, NULL);
}

static void
rfp_toggle_repeat_grey(RFP *rfp, Boolean status) {
	Widget	list;

	XtVaSetValues(rfp->repeat_label, XmNsensitive, status, NULL);
	XtVaGetValues(rfp->repeat_menu, XmNlist, &list, NULL);
	XtVaSetValues(list, XmNsensitive, status, NULL);
}


/* This routine makes sure that the repeat menu has the right set 
   of values in it for the connection version.  The REPEAT_EVERY 
   item should not appear for vcalendars of version 3 and lower. */

extern void
rfp_set_repeat_values(RFP *rfp)

{
	int 		i, limit;
	XmString 	xmstr;
	Widget 		list;

	/* delete all the old items. */

	XtVaGetValues(rfp->repeat_menu, XmNlist, &list, NULL);
	XtVaGetValues(list, XmNitemCount, &limit, NULL);

	if ((limit == 11) && (rfp->cal->general->version >= DATAVER3))
		return;

	if ((limit == 10) && (rfp->cal->general->version == DATAVER2))
		return;

	if ((limit == 7) && (rfp->cal->general->version == DATAVER1))
		return;


	if (limit == 11) {
		XmComboBoxDeletePos(rfp->repeat_menu, 11);
		if (rfp->cal->general->version == DATAVER1) {
			XmComboBoxDeletePos(rfp->repeat_menu, 10);
			XmComboBoxDeletePos(rfp->repeat_menu, 9);
			XmComboBoxDeletePos(rfp->repeat_menu, 8);
		}
		if (rfp->repeat_popup_frame)
		  XtUnmanageChild(rfp->repeat_form_mgr);
		return;
	}
	else if (limit == 10) {
        	xmstr = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 901, "Repeat Every ..."));
        	XmComboBoxAddItem(rfp->repeat_menu, xmstr, 0, False);
        	XmStringFree(xmstr);

	}
	else if (limit == 7) {
        	xmstr = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 902, "Monday Thru Friday"));
        	XmComboBoxAddItem(rfp->repeat_menu, xmstr, 0, False);
        	XmStringFree(xmstr);
	 
        	xmstr = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 903, "Mon, Wed, Fri"));
        	XmComboBoxAddItem(rfp->repeat_menu, xmstr, 0, False);
        	XmStringFree(xmstr);
        	xmstr = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 861, "Tuesday, Thursday"));
        	XmComboBoxAddItem(rfp->repeat_menu, xmstr, 0, False);
        	XmStringFree(xmstr);

		if (rfp->cal->general->version == DATAVER3) {
        		xmstr = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 862, "Repeat Every..."));
        		XmComboBoxAddItem(rfp->repeat_menu, xmstr, 0, False);
        		XmStringFree(xmstr);
		}

	}
}

/*
**  This is used for the rfp_repeat_menu_proc and the rfp_apply_proc functions.
**  It simply sets default for values given a Repeat_op
*/
static void
rfp_set_default_for(RFP *rfp, Repeat_menu_op op, Boolean status) {
	Widget		text;
	XmString	new_scope_str;
        Calendar	*c = rfp->cal;

	XtVaGetValues(rfp->for_menu, XmNtextField, &text, NULL);
	XmTextFieldSetString(text, default_repeat_cnt_str(op));

	new_scope_str = XmStringCreateLocalized(
				default_repeat_scope_str(c->DT_catd, op));
	XtVaSetValues(rfp->for_scope, XmNlabelString, new_scope_str,
		NULL);
	XmStringFree(new_scope_str);

	rfp_toggle_for_grey(rfp, status);
}

/*
**  Callbacks for the repeat every popup
*/
static void
rfp_repeat_scope_proc(Widget w, XtPointer client_data, XtPointer cbs) {
	RFP			*rfp = (RFP *)client_data;
	Repeat_scope_menu_op	op;

	if (!rfp)
		return;

	XtVaGetValues(w, XmNuserData, &op, NULL);
	XtVaSetValues(rfp->repeat_popup_menu, XmNuserData, op, NULL);
}

static Boolean
string_is_number(char *str) {
 
 
        if (blank_buf(str)) {
                return(False);
        }
 
        while (str && *str) {
                if (!isdigit(*str) && *str != ' ' && *str != '\t')
                        return(False);
 
                str++;
        }
 
        return(True);
}

static void
rfp_apply_proc(Widget w, XtPointer client_data, XtPointer cbs) {
	RFP			*rfp = (RFP *)client_data;
	char			*str, buf[128];
	XmString		xmstr;
        Calendar 		*c = rfp->cal;
	Props_pu		*pu = (Props_pu *)c->properties_pu;
	Repeat_scope_menu_op	op = REPEAT_DAYS;
	char			*repeat_str;

	if (!rfp)
		return;

	str = XmTextGetString(rfp->repeat_popup_text);

	if (!string_is_number(str)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 969, "Calendar : Error - Repeat Every"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 970, "The Repeat Every value must be an integer with no sign."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));

		XtFree(str);
                dialog_popup(c->frame,
                        DIALOG_TITLE, title,
                        DIALOG_TEXT, text,
                        BUTTON_IDENT, 1, ident1,
                        DIALOG_IMAGE, pu->xm_error_pixmap,
                        NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		return;
	}

	rfp->repeat_nth = atoi(str);
	XtFree(str);
	if (rfp->repeat_nth == 0) {
	  XtUnmanageChild(rfp->repeat_form_mgr);
	  return;
	}

	/*
	 * Change the string to be more informative.
	 */
	XtVaGetValues(rfp->repeat_popup_menu, XmNuserData, &op, NULL);

	switch (op) {

	case REPEAT_DAYS: repeat_str = catgets(c->DT_catd, 1, 825, "Days");
				break;

	case REPEAT_WEEKS: repeat_str = catgets(c->DT_catd, 1, 826, "Weeks");
				break;

	case REPEAT_MONTHS: repeat_str = catgets(c->DT_catd, 1, 827, "Months");
				break;
	}

	/*
	 * Now change the text field
	 */
	{
	  char *nl_repeat = XtNewString(repeat_str);
	  sprintf(buf, catgets(c->DT_catd, 1, 531, "Every %d %s"),
		  rfp->repeat_nth, nl_repeat);
	  XtFree(nl_repeat);
	}

	xmstr = XmStringCreateLocalized(buf);
	XtVaSetValues(rfp->repeat_menu, XmNlabelString, xmstr, NULL);
	XmStringFree(xmstr);

	switch(op) {
	case REPEAT_DAYS:
		rfp->repeat_type = CSA_X_DT_REPEAT_EVERY_NDAY;
		rfp_set_default_for(rfp, DAILY, True);
		break;
	case REPEAT_WEEKS:
		rfp->repeat_type = CSA_X_DT_REPEAT_EVERY_NWEEK;
		rfp_set_default_for(rfp, WEEKLY, True);
		break;
	case REPEAT_MONTHS:
	default:
		rfp->repeat_type = CSA_X_DT_REPEAT_EVERY_NMONTH;
		rfp_set_default_for(rfp, MONTHLY_BY_DATE, True);
		break;
	}

	XtUnmanageChild(rfp->repeat_form_mgr);
}

static void
rfp_cancel_proc(Widget w, XtPointer client_data, XtPointer cbs) {
	RFP	*rfp = (RFP *)client_data;

	rfp->repeat_nth = 0;
	rfp->repeat_type = CSA_X_DT_REPEAT_ONETIME;

	/*
	 * If the user cancels or quits the popup, we want the
	 * combo box to say "One Time".
	 * NOTE: Changing the menu has the effect of popping
	 * down the popup.
	 */
	XtVaSetValues(rfp->repeat_menu,
		      XmNselectedPosition, ONE_TIME + 1, NULL);
}

/*
**  This, obviously, pops up the repeat every ...
*/
static void
rfp_repeat_every_popup(RFP *rfp) {
	Widget		label, apply_button, cancel_button, separator;
	XmString	xmstr;
        Calendar	*c = rfp->cal;
	char		*title;
	XmString	label_str;

	title = XtNewString(catgets(c->DT_catd, 1, 532,
				    "Calendar : Repeat Every"));
	rfp->repeat_popup_frame = XtVaCreatePopupShell("repeat_popup_frame",
		xmDialogShellWidgetClass, rfp->parent,
		XmNtitle, title,
		XmNallowShellResize, True,
		XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE,
		XmNdeleteResponse, XmDO_NOTHING,
		NULL);
	XtFree(title);

	setup_quit_handler(rfp->repeat_popup_frame, rfp_cancel_proc,
			   (XtPointer)rfp);

	rfp->repeat_form_mgr = XtVaCreateWidget("form_mgr",
		xmFormWidgetClass, rfp->repeat_popup_frame,
		XmNautoUnmanage, False,
                XmNhorizontalSpacing, 0,
                XmNverticalSpacing, 5,
		XmNfractionBase, 2,
		NULL);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 460, "Apply"));
        rfp->repeat_apply_button = XtVaCreateWidget("repeat_apply_button",
		xmPushButtonWidgetClass, rfp->repeat_form_mgr,
		XmNlabelString,	label_str,
		XmNleftOffset, 10,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 0,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 1,
		XmNbottomAttachment, XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(rfp->repeat_apply_button, 
			XmNactivateCallback, rfp_apply_proc, rfp);
 
	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
        rfp->repeat_cancel_button = XtVaCreateWidget("repeat_cancel_button",
		xmPushButtonWidgetClass, rfp->repeat_form_mgr,
		XmNlabelString,	label_str,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 1,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 2,
		XmNrightOffset, 10,
		XmNbottomAttachment, XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(rfp->repeat_cancel_button, 
			XmNactivateCallback, rfp_cancel_proc, rfp);

	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                rfp->repeat_form_mgr,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	rfp->repeat_apply_button,
		XmNbottomOffset, 	5,
                NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 533,
						"Repeat Every:"));
        label = XtVaCreateWidget("every",
		xmLabelGadgetClass, rfp->repeat_form_mgr,
        	XmNlabelString, xmstr,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 10,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 5,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, separator,
                NULL);
	XmStringFree(xmstr);

        rfp->repeat_popup_text = XtVaCreateWidget("text_field",
		xmTextWidgetClass, rfp->repeat_form_mgr,
                XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNcolumns, FOR_LEN,
		XmNmaxLength, FOR_LEN - 1,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 10,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, label,
		XmNleftOffset, 5,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, separator,
                NULL);
 
	rfp->repeat_popup_menu = 
		create_repeat_scope_menu(rfp->repeat_form_mgr, NULL,
					 rfp_repeat_scope_proc, (XtPointer)rfp);


	XtVaSetValues(rfp->repeat_popup_menu,
		XmNuserData, REPEAT_DAYS,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 6,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, rfp->repeat_popup_text,
		XmNleftOffset, 5,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, separator,
		NULL);

	XtVaSetValues(rfp->repeat_form_mgr, XmNdefaultButton,
		      rfp->repeat_apply_button, NULL);
	XtVaSetValues(rfp->repeat_form_mgr, XmNcancelButton,
		      rfp->repeat_cancel_button, NULL);

	ManageChildren(rfp->repeat_form_mgr);
}

static void
show_repeat_every(XtPointer uData, XtIntervalId *id)
{
  RFP *rfp = (RFP *)uData;

  if (!rfp->repeat_form_mgr)
    rfp_repeat_every_popup(rfp);

  XtManageChild(rfp->repeat_form_mgr);
}

static void
hide_repeat_every(XtPointer uData, XtIntervalId *id)
{
  RFP *rfp = (RFP *)uData;

  /* Make sure repeat_every form no longer displayed. */
  if (rfp->repeat_form_mgr)
    XtUnmanageChild(rfp->repeat_form_mgr);
}

/*
**  This callback is attached to the repeat menu - it sets the for duration and
**  scope fields (actually called for_menu and for_scope)
*/
static void
rfp_repeat_menu_proc(Widget w, XtPointer data, XtPointer cbs) {
	RFP			*rfp = (RFP *)data;
	Repeat_menu_op		item_no;

	if (!rfp)
		return;

	XtVaGetValues(rfp->repeat_menu,
		      XmNselectedPosition, &item_no,
		      NULL);
	--item_no;

	/*
	 * We use timeouts to make sure the menu is popped
	 * down before we pop up/down the repeat_every form.
	 * Otherwise, Xt gets confused about grabs.
	 */
	if (item_no == REPEAT_EVERY)
	{
	  XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			  0, show_repeat_every, (XtPointer)rfp);
	}
	else
	{
	  XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			  0, hide_repeat_every, (XtPointer)rfp);

	  rfp_set_default_for(rfp, item_no, item_no ? True : False);
	}
}

/*******************************************************************************
**
**  External functions
**
*******************************************************************************/
extern void
build_rfp(
	RFP 		*rfp, 
	Calendar 	*c, 
	Widget 		 parent)
{
	XmString	 tmp;
	Dimension	 max_left_label_width;
	Dimension	 freq_label_height;
	XmString	label_str;

	rfp->parent = parent;
	rfp->cal = c;

	rfp->rfp_form_mgr = XtVaCreateWidget("rfp_form_mgr",
		xmFormWidgetClass, rfp->parent,
		XmNautoUnmanage, False,
		XmNuserData, rfp,
		NULL);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 836, "Frequency"));
        rfp->frequency_label = XtVaCreateWidget("frequency_label",
		xmLabelGadgetClass, 	rfp->rfp_form_mgr,
		XmNlabelString,		label_str,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		GAP,
		NULL);
	XmStringFree(label_str);
 
	tmp = XmStringCreateLocalized(catgets(c->DT_catd, 1, 534, "Occurs:"));
        rfp->repeat_label = XtVaCreateWidget("repeat",
		xmLabelGadgetClass, 	rfp->rfp_form_mgr,
		XmNlabelString,		label_str,
        	XmNlabelString, 	tmp,
		NULL);
	XmStringFree(tmp);

	tmp = XmStringCreateLocalized(catgets(c->DT_catd, 1, 535, "For:"));
        rfp->for_label = XtVaCreateWidget("for",
		xmLabelGadgetClass, 	rfp->rfp_form_mgr,
        	XmNlabelString, 	tmp,
		XmNsensitive, 		False,
                NULL);
	XmStringFree(tmp);

	tmp = XmStringCreateLocalized( catgets(c->DT_catd, 1, 536, "Privacy:"));
       	rfp->privacy_label = XtVaCreateWidget("privacy",
		xmLabelGadgetClass, 	rfp->rfp_form_mgr,
       		XmNlabelString, 	tmp,
               	NULL);
	XmStringFree(tmp);

        max_left_label_width = ComputeMaxWidth(rfp->frequency_label,
                                               rfp->repeat_label,
                                               rfp->for_label,
					       rfp->privacy_label)
                                               + GAP;

        XtVaGetValues(rfp->frequency_label,
                XmNheight,       	&freq_label_height,
                NULL);

	rfp->repeat_menu = create_repeat_menu(rfp->rfp_form_mgr,
		rfp_repeat_menu_proc, (XtPointer)rfp);

	rfp_set_repeat_values(rfp);
	XtVaSetValues(rfp->repeat_menu,
		XmNuserData, 		ONE_TIME,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		max_left_label_width,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		freq_label_height + 3 * GAP,
		NULL);
	tmp = XmStringCreateLocalized("\0");

	rfp->for_menu = create_for_menu(rfp->rfp_form_mgr);
	XtVaSetValues(rfp->for_menu,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		max_left_label_width,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		rfp->repeat_menu,
		XmNtopOffset, 		2 * GAP,
		NULL);

        rfp->for_scope = XtVaCreateWidget("for_scope",
		xmLabelGadgetClass, 	rfp->rfp_form_mgr,
		XmNlabelString, 	tmp,
		XmNbottomAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget, 	rfp->for_label,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		rfp->for_menu,
		XmNleftOffset, 		GAP,
                NULL);

	XmStringFree(tmp);

	rfp->privacy_menu = create_privacy_menu(rfp->rfp_form_mgr);
	XtVaSetValues(rfp->privacy_menu,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		max_left_label_width,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		rfp->for_menu,
		XmNtopOffset, 		2 * GAP,
		NULL);

        XtVaSetValues(rfp->frequency_label,
                XmNrightAttachment,     XmATTACH_WIDGET,
                XmNrightWidget,         rfp->repeat_menu,
                XmNrightOffset,         GAP,
                NULL);

        XtVaSetValues(rfp->repeat_label,
		XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                XmNbottomWidget,        rfp->repeat_menu,
                XmNbottomOffset,        GAP,
                XmNrightAttachment,     XmATTACH_WIDGET,
                XmNrightWidget,         rfp->repeat_menu,
                XmNrightOffset,         GAP,
                NULL);

        XtVaSetValues(rfp->for_label,
		XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
                XmNbottomWidget,        rfp->for_menu,
                XmNbottomOffset,        GAP,
                XmNrightAttachment,     XmATTACH_WIDGET,
                XmNrightWidget,         rfp->for_menu,
                XmNrightOffset,         GAP,
                NULL);

        XtVaSetValues(rfp->privacy_label,
		XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                XmNbottomWidget,        rfp->privacy_menu,
                XmNbottomOffset,        GAP,
                XmNrightAttachment,     XmATTACH_WIDGET,
                XmNrightWidget,         rfp->privacy_menu,
                XmNrightOffset,         GAP,
                NULL);
}

/*
**  Get and set values on the screen
*/
extern void
get_rfp_repeat_val(
	RFP 		*rfp,
	time_t		 tick)
{
	char		*str,
			 rule_buf1[32],
			 rule_buf2[32];
	Widget		 tf;
	Repeat_menu_op	 op;
	Boolean		 sensitive;

	/* clear the buffers */
	memset (rule_buf1, 0, 32);
	memset (rule_buf2, 0, 32);

	/* This routine has to set up the recurrence values in two spots. 
	   It has to set up the old style recurrence values for a daemon
	   in data versions 1-3, and it has to set up a recurrence rule for a 
	   data version 4 daemon. */

	/*
	**  First, get the value on the repeat menu
	*/
	XtVaGetValues(rfp->repeat_menu, XmNselectedPosition, &op, NULL);
	--op;

	switch(op) {
	case ONE_TIME:
		rfp->repeat_type = CSA_X_DT_REPEAT_ONETIME;
		strcpy(rule_buf1, "D1 ");
		break;
	case DAILY:
		rfp->repeat_type = CSA_X_DT_REPEAT_DAILY;
		strcpy(rule_buf1, "D1 ");
		break;
	case WEEKLY:
		rfp->repeat_type = CSA_X_DT_REPEAT_WEEKLY;
		strcpy(rule_buf1, "W1 ");
		break;
	case EVERY_TWO_WEEKS:
		rfp->repeat_type = CSA_X_DT_REPEAT_BIWEEKLY;
		strcpy(rule_buf1, "W2 ");
		break;
	case MONTHLY_BY_DATE:
		rfp->repeat_type = CSA_X_DT_REPEAT_MONTHLY_BY_DATE;
		strcpy(rule_buf1, "MD1 ");
		break;
	case MONTHLY_BY_WEEKDAY: {
		int	wk;

		rfp->repeat_type = CSA_X_DT_REPEAT_MONTHLY_BY_WEEKDAY;
		/* 
		 * The current behavior of cm/dtcm is that if an appt is
		 * scheduled for the 5 wk of the month, it repeats on the
		 * last week of the month.
		 */
		if (weekofmonth(tick, &wk) && wk == 5)
			sprintf(rule_buf1, "MP1 1- %s ", dow_str(tick));
		else
			strcpy(rule_buf1, "MP1 ");
		break;
	}
	case YEARLY:
		rfp->repeat_type = CSA_X_DT_REPEAT_YEARLY;
		strcpy(rule_buf1, "YM1 ");
		break;
	case MONDAY_THRU_FRIDAY:
		rfp->repeat_type = CSA_X_DT_REPEAT_MON_TO_FRI;
		strcpy(rule_buf1, "W1 MO TU WE TH FR ");
		break;
	case MON_WED_FRI:
		rfp->repeat_type = CSA_X_DT_REPEAT_MONWEDFRI;
		strcpy(rule_buf1, "W1 MO WE FR ");
		break;
	case TUESDAY_THURSDAY:
		rfp->repeat_type = CSA_X_DT_REPEAT_TUETHUR;
		strcpy(rule_buf1, "W1 TU TH ");
		break;
	case REPEAT_EVERY:
		/* REPEAT_EVERY is handled in rfp_apply_proc() */
		switch(rfp->repeat_type) {
		case CSA_X_DT_REPEAT_EVERY_NDAY:
			sprintf(rule_buf1, "D%d ", rfp->repeat_nth);
			break;
		case CSA_X_DT_REPEAT_EVERY_NWEEK:
			sprintf(rule_buf1, "W%d ", rfp->repeat_nth);
			break;
		case CSA_X_DT_REPEAT_EVERY_NMONTH:
		default:
			sprintf(rule_buf1, "MD%d ", rfp->repeat_nth);
			break;
		}
		break;
	default:
		break;
	}

	if (op != REPEAT_EVERY)
		rfp->repeat_nth = 0;

	/*
	**  Now get the duration - the for menu and scope.
	*/
	XtVaGetValues(rfp->for_menu, 
			XmNtextField, 	&tf, 
			NULL);
	XtVaGetValues(tf,
			XmNsensitive,	&sensitive,
			NULL);

	if (sensitive) {
		char	*forever_str = catgets(calendar->DT_catd, 1, 876, 
								   "forever"); 
		str = XmTextGetString(tf);
		if (strcmp(str, forever_str) == 0) {
			rfp->for_val = PRIVATE_FOREVER;
			strcat(rule_buf2, "#0");
		} else {
			rfp->for_val = atoi(str);
			if (rfp->for_val == 0)
				strcat(rule_buf2, "#1");
			else {
				if (op != REPEAT_EVERY) {
					sprintf(rule_buf2, "#%d", rfp->for_val);
				} else {
					int duration;

					if (rfp->for_val % rfp->repeat_nth)
						duration = 1 + 
						   rfp->for_val/rfp->repeat_nth;
					else
						duration = 
						   rfp->for_val/rfp->repeat_nth;

					sprintf(rule_buf2, "#%d", duration);
				}
			}
		}
		XtFree(str);
	} else {
		strcat(rule_buf2, "#1");
		rfp->for_val = 0;

	}

	strcat (rule_buf1, rule_buf2);

	if (rfp->recurrence_rule)
		free(rfp->recurrence_rule);

	rfp->recurrence_rule = cm_strdup(rule_buf1);
}

extern void
get_rfp_privacy_val(RFP *rfp) {
	int	i;

	XtVaGetValues(rfp->privacy_menu, XmNselectedPosition, &i, NULL);
	--i;

	switch (i) {
	
		case 0: rfp->privacy_val = CSA_CLASS_PUBLIC;
		   break;
	
		case 1: rfp->privacy_val = CSA_CLASS_CONFIDENTIAL;
		   break;
	
		case 2: rfp->privacy_val = CSA_CLASS_PRIVATE;
		   break;

	}
}

extern void
get_rfp_vals(RFP *rfp, time_t tick) {
	get_rfp_repeat_val(rfp, tick);
	get_rfp_privacy_val(rfp);
}

/*
**  This function will consume form values and stuff them into an appointment.
*/
extern Boolean
rfp_form_to_appt(RFP *rfp, Dtcm_appointment *a, char *name)
{
  return rfp_form_flags_to_appt(rfp, a, name, (int *)NULL);
}

extern Boolean
rfp_form_flags_to_appt(RFP *rfp, Dtcm_appointment *a, char *name, int *flagsP)
{
	int		wk;
	char		buf[MAXNAMELEN];
	time_t		tick;
	Calendar	*c = rfp->cal;
	Props_pu	*p = (Props_pu *)rfp->cal->properties_pu;
	int		flags = 0;

	if (flagsP == (int *)NULL)
	  flagsP = &flags;

	_csa_iso8601_to_tick(a->time->value->item.date_time_value, &tick);

	if (rfp->rfp_form_mgr)
		get_rfp_vals(rfp, tick);

	a->private->value->item.sint32_value = rfp->privacy_val;

	if (a->version < DATAVER4) {
		a->repeat_type->value->item.sint32_value = rfp->repeat_type;
		a->repeat_times->value->item.uint32_value = rfp->for_val == PRIVATE_FOREVER ? CSA_X_DT_DT_REPEAT_FOREVER : rfp->for_val;

		if ((a->version == DATAVER3) ||(a->version == DATAVER_ARCHIVE)) {
			if (a->repeat_interval)
				a->repeat_interval->value->item.uint32_value =
					rfp->repeat_nth;
			if (a->repeat_week_num)
				a->repeat_week_num->value->item.sint32_value =
					-1;
		}
	}
	else
		a->recurrence_rule->value->item.string_value = 
						cm_strdup(rfp->recurrence_rule);

	if (rfp->repeat_type != CSA_X_DT_REPEAT_ONETIME) {
		if (rfp->for_val == 0) {
			editor_err_msg(rfp->parent, name, REPEAT_FOR_MISMATCH,
				p->xm_error_pixmap);
			return False;
		}
		if (rfp->repeat_type == CSA_X_DT_REPEAT_MONTHLY_BY_WEEKDAY) {
			if (weekofmonth(tick, &wk) && wk == 4)
			{
			    if (*flagsP == 0)
			    {
			  	char *title = XtNewString(catgets(c->DT_catd, 1, 537,
						"Calendar : Editor - Schedule Appointment"));
			  	char *text = XtNewString(catgets(c->DT_catd, 1, 538,
						"Would you like to schedule this appointment as the last\nweek of the month or the 4th week of the month?"));
			  	char *ident1 = XtNewString(catgets(c->DT_catd, 1,
						923, "Cancel"));
			  	char *ident2 = XtNewString(catgets(c->DT_catd, 1,
						540, "Last Week"));
			  	char *ident3 = XtNewString(catgets(c->DT_catd, 1,
						541, "4th Week"));
				*flagsP = dialog_popup(rfp->parent,
					DIALOG_TITLE, title,
					DIALOG_TEXT, text,
					BUTTON_IDENT, -1, ident1,
					BUTTON_IDENT, RFP_MBW_LAST, ident2,
					BUTTON_IDENT, RFP_MBW_4TH, ident3,
					DIALOG_IMAGE, p->xm_question_pixmap,
					NULL);
				XtFree(ident3);
				XtFree(ident2);
				XtFree(ident1);
				XtFree(text);
				XtFree(title);
			    }
			    switch (*flagsP)
			    {
			    case RFP_MBW_LAST:
			        change_to_last_week(a, wk, tick, rfp);
			        break;

			    case RFP_MBW_4TH:
			        if (a->version < DATAVER4)
				  a->repeat_week_num->value->item.sint32_value = wk;
			        break;

			    default:
				*flagsP = 0;
			        return False;
			    }
			} else if (wk != 5)
				if (a->version < DATAVER4)
					a->repeat_week_num->value->item.sint32_value = wk;
		}
		else
                {
                        if (a->repeat_type == NULL) {
                                if ((a->repeat_type =
				      (CSA_attribute *)ckalloc(sizeof(CSA_attribute)))
				    == NULL)
				  /* return (CSA_E_INSUFFICIENT_MEMORY); */
				  return False;

                                memset(a->repeat_type, 0, sizeof(CSA_attribute));

                                _DtCm_set_sint32_attrval(rfp->repeat_type,
							 &a->repeat_type->value);
                        }
                        else a->repeat_type->value->item.sint32_value = rfp->repeat_type;
                }
        }
        else
        {
                if (a->repeat_type == NULL) {
                        if ((a->repeat_type =
			     (CSA_attribute *)ckalloc(sizeof(CSA_attribute)))
			    == NULL)
			  /* return (CSA_E_INSUFFICIENT_MEMORY); */
			  return False;

                        memset(a->repeat_type, 0, sizeof(CSA_attribute));

                        _DtCm_set_sint32_attrval(rfp->repeat_type,&a->repeat_type->value);
                }
                else a->repeat_type->value->item.sint32_value = rfp->repeat_type;
	}

	return True;
}

/*
 * Given a rule, change it to always happen on the last week of the month.
 */
static void
change_to_last_week(
	Dtcm_appointment 	*a,
	int			 wk,
	time_t			 tick,
	RFP			*rfp)
{
	if (a->version < DATAVER4)
		return;
	else {
		char 	*weekday = dow_str(tick),
			*new_rule,
			 duration_buf[10];

		memset(duration_buf, 0, 10);

		/* The new rule uses a little more memory than the old */
		new_rule = (char *)calloc(1, 
		      strlen(a->recurrence_rule->value->item.string_value) + 8);

		/* Free the old rule */
		free(a->recurrence_rule->value->item.string_value);

		sprintf(new_rule, "MP1 1- %s", weekday);
		if (rfp->for_val == PRIVATE_FOREVER)
			strcat(new_rule, " #0");
		else if (rfp->for_val == 0)
			strcat(new_rule, " #1");
		else {
			sprintf(duration_buf, " #%d", rfp->for_val);
			strcat(new_rule, duration_buf);
		}

		a->recurrence_rule->value->item.string_value = new_rule;

		free(weekday);
	}
}

/*
**  This function will take appointment values and stuff them into a form.
*/
extern Boolean
rfp_appt_to_form(RFP *rfp, CSA_entry_handle entry) {
	Boolean			ret_val;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;

	appt = allocate_appt_struct(appt_read,
				    rfp->cal->general->version,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
				    CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I,
				    CSA_ENTRY_ATTR_CLASSIFICATION_I,
				    CSA_ENTRY_ATTR_RECURRENCE_RULE_I,
				    NULL);
	stat = query_appt_struct(rfp->cal->cal_handle, entry, appt);
	backend_err_msg(rfp->cal->frame, rfp->cal->view->current_calendar,
		stat, ((Props_pu *)rfp->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return False;
	}

	ret_val = rfp_attrs_to_form(rfp, appt);
	free_appt_struct(&appt);

	return ret_val;
}

extern Boolean
rfp_attrs_to_form(RFP *rfp, Dtcm_appointment *appt) {

	/* At this point, check out the returning reminder values.  If
	   the old style reminder values are not set, we must have a client
	   that either wrote out the appointment with no reminders, or with
	   a recurrence rule set that will not map into what wee currently
	   understand.  We should inactivate the rfp fields, and not set 
	   any values.  On the other hand, if they are set, we should
	   make sure the controls are made sensitive. */

	if (appt->repeat_type && appt->repeat_type->value) {
		rfp_toggle_repeat_grey(rfp, True);
		rfp->repeat_type = appt->repeat_type->value->item.sint32_value;
		if (appt->version > DATAVER2) {
			rfp->repeat_nth = (appt->repeat_interval) ?
				appt->repeat_interval->value->item.uint32_value : 0;
		}
		if (appt->repeat_times && appt->repeat_times->value)
			rfp->for_val = appt->repeat_times->value->item.uint32_value;
		else
			rfp->for_val = 0;

		rfp->privacy_val = privacy_set(appt);
		set_rfp_vals(rfp);
	}
	else {
		rfp_toggle_for_grey(rfp, False);
		rfp_toggle_repeat_grey(rfp, False);
	}

	return True;
}

extern void
set_rfp_repeat_val(RFP *rfp) {
	char		buf[128];
	Boolean		status = True;
	XmString	xmstr;
	Calendar	*c = rfp->cal;
	const char	*repeat_scope;
	Repeat_menu_op	op;

	/*
	**  Set the repeat menu button based on the period value
	*/
	buf[0] = '\0';
	if (rfp->repeat_type == CSA_X_DT_REPEAT_ONETIME) {
		op = ONE_TIME;
		repeat_scope = default_repeat_scope_str(c->DT_catd, ONE_TIME);
		sprintf(buf, "%s", repeat_str(c->DT_catd, ONE_TIME));
		status = False;
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_DAILY) {
		op = DAILY;
		repeat_scope = default_repeat_scope_str(c->DT_catd, DAILY);
		sprintf(buf, "%s", repeat_str(c->DT_catd, DAILY));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_WEEKLY) {
		op = WEEKLY;
		repeat_scope = default_repeat_scope_str(c->DT_catd, WEEKLY);
		sprintf(buf, "%s", repeat_str(c->DT_catd, WEEKLY));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_BIWEEKLY) {
		op = EVERY_TWO_WEEKS;
		repeat_scope = 
			default_repeat_scope_str(c->DT_catd, EVERY_TWO_WEEKS);
		sprintf(buf, "%s", repeat_str(c->DT_catd, EVERY_TWO_WEEKS));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_MONTHLY_BY_DATE) {
		op = MONTHLY_BY_DATE;
		repeat_scope = 
			default_repeat_scope_str(c->DT_catd, MONTHLY_BY_DATE);
		sprintf(buf, "%s", repeat_str(c->DT_catd, MONTHLY_BY_DATE));
	} else if (rfp->repeat_type ==
			  CSA_X_DT_REPEAT_MONTHLY_BY_WEEKDAY) {
		op = MONTHLY_BY_WEEKDAY;
		repeat_scope = default_repeat_scope_str(c->DT_catd, 
							MONTHLY_BY_WEEKDAY);
		sprintf(buf, "%s", repeat_str(c->DT_catd, MONTHLY_BY_WEEKDAY));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_YEARLY) {
		op = YEARLY;
		repeat_scope = default_repeat_scope_str(c->DT_catd, YEARLY);
		sprintf(buf, "%s", repeat_str(c->DT_catd, YEARLY));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_MON_TO_FRI) {
		op = MONDAY_THRU_FRIDAY;
		repeat_scope = default_repeat_scope_str(c->DT_catd, 
							MONDAY_THRU_FRIDAY);
		sprintf(buf, "%s", repeat_str(c->DT_catd, MONDAY_THRU_FRIDAY));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_MONWEDFRI) {
		op = MON_WED_FRI;
		repeat_scope = 
			default_repeat_scope_str(c->DT_catd, MON_WED_FRI);
		sprintf(buf, "%s", repeat_str(c->DT_catd, MON_WED_FRI));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_TUETHUR) {
		op = TUESDAY_THURSDAY;
		repeat_scope = 
			default_repeat_scope_str(c->DT_catd, TUESDAY_THURSDAY);
		sprintf(buf, "%s", repeat_str(c->DT_catd, TUESDAY_THURSDAY));
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_EVERY_NDAY) {
		op = REPEAT_EVERY;
		repeat_scope = repeat_scope_str(c->DT_catd, REPEAT_DAYS);
		sprintf(buf, catgets(c->DT_catd, 1, 542, "Every %d %s"),
			rfp->repeat_nth, repeat_scope);
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_EVERY_NWEEK) {
		op = REPEAT_EVERY;
		repeat_scope = repeat_scope_str(c->DT_catd, REPEAT_WEEKS);
		sprintf(buf, catgets(c->DT_catd, 1, 543, "Every %d %s"),
			rfp->repeat_nth, repeat_scope);
	} else if (rfp->repeat_type == CSA_X_DT_REPEAT_EVERY_NMONTH) {
		op = REPEAT_EVERY;
		repeat_scope = repeat_scope_str(c->DT_catd, REPEAT_MONTHS);
		sprintf(buf, catgets(c->DT_catd, 1, 544, "Every %d %s"),
			rfp->repeat_nth, repeat_scope);
	} else
		return;

	/*
	 * Set the Repeat combo box
	 */
	xmstr = XmStringCreateLocalized(buf);
	XtVaSetValues(rfp->repeat_menu, XmNselectedPosition, op + 1, NULL);
	XtVaSetValues(rfp->repeat_menu, XmNlabelString, xmstr, NULL);
	XmStringFree(xmstr);

	/*
	**  Set the For button, text item, and scope
	*/
	if (rfp->for_val == CSA_X_DT_DT_REPEAT_FOREVER) {
		if(rfp->repeat_type != CSA_X_DT_REPEAT_ONETIME)
		{
			sprintf(buf, "%s", catgets(c->DT_catd, 1, 876, "forever"));
			xmstr = XmStringCreateLocalized(buf);
			XmComboBoxSetItem(rfp->for_menu, xmstr);
			XmStringFree(xmstr);
		}
		else
		{
			Widget text;
			buf[0]= '\0';
			XtVaGetValues(rfp->for_menu, XmNtextField,
				      &text, NULL);
			XmTextFieldSetString(text, buf);
		}
		repeat_scope = "\0";
	} else if (rfp->for_val >= 2 && rfp->for_val <= 14) {
		sprintf(buf, "%s", for_str(rfp->for_val - 2)); 
		xmstr = XmStringCreateLocalized(buf);
		XmComboBoxSetItem(rfp->for_menu, xmstr);
		XmStringFree(xmstr);
	} else {
		Widget		text;
		sprintf(buf, "%d", rfp->for_val);
		XtVaGetValues(rfp->for_menu, XmNtextField, &text, NULL);
		XmTextFieldSetString(text, buf);
	}

	xmstr = XmStringCreateLocalized((char *)repeat_scope);
	XtVaSetValues(rfp->for_scope, XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);

	rfp_toggle_for_grey(rfp, status);
}

extern void
set_rfp_privacy_val(RFP *rfp) {
	int	i;

	if (rfp->privacy_val == CSA_CLASS_PUBLIC)
		i = 0;
	else if (rfp->privacy_val == CSA_CLASS_CONFIDENTIAL)
		i = 1;
	else
		i = 2;

	XtVaSetValues(rfp->privacy_menu, XmNselectedPosition, i + 1, NULL);
}

extern void
set_rfp_vals(RFP *rfp) {
	set_rfp_repeat_val(rfp);
	set_rfp_privacy_val(rfp);
}

extern void
set_rfp_defaults(RFP *rfp) {
	Props		*p = (Props *)rfp->cal->properties;

	rfp->for_val = 0;
	rfp->privacy_val = convert_privacy_str(get_char_prop(p, CP_PRIVACY));
	rfp->repeat_type = CSA_X_DT_REPEAT_ONETIME;

	set_rfp_vals(rfp);
	rfp_set_default_for(rfp, ONE_TIME, False);
}

static char *
dow_str(
	time_t 	tick)
{
	switch (dow(tick)) {
	case 0:
		return (cm_strdup("SU"));
	case 1:
		return (cm_strdup("MO"));
	case 2:
		return (cm_strdup("TU"));
	case 3:
		return (cm_strdup("WE"));
	case 4:
		return (cm_strdup("TH"));
	case 5:
		return (cm_strdup("FR"));
	case 6:
	default:
		return (cm_strdup("SA"));
	}
}

extern void
rfp_init(
	RFP 		*rfp,
	Calendar	*c,
	Widget		 parent)
{
	Props		*p = (Props *)c->properties;

	rfp->parent = parent;
	rfp->cal = c;
	rfp->for_val = 0;
	rfp->privacy_val = convert_privacy_str(get_char_prop(p, CP_PRIVACY));
	rfp->repeat_type = CSA_X_DT_REPEAT_ONETIME;
	rfp->recurrence_rule = strdup("D1 #1");
}
