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
/* $XConsortium: props.c /main/9 1996/03/25 10:22:39 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <csa.h>
#include "props_pu.h"
#include "help.h"
#include "calendar.h"
#include "deskset.h"

#define GAP		10
#define TOG_GAP		10
#define RIGHT_GAP	85  /* Space between text field and right edge */
#define MAX_LABEL_SPACE	55
#define INDICATOR_SIZE	20

extern unsigned int p_gap_convert_permissions(Props_pu *);
extern void gap_list_select_proc(Widget, XtPointer, XtPointer);
extern void p_gap_remove_proc(Widget, XtPointer, XtPointer);
extern void p_set_change(Props_pu *);
extern Dimension ComputeMaxWidth(Widget, Widget, Widget, Widget);

static void GAPAddProc(Widget, XtPointer, XtPointer);
static void GAPChangeProc(Widget, XtPointer, XtPointer);
static Widget CreatePermissions(Props_pu *, Widget, Widget, Dimension,
				Dimension);
static int ChangeExistingGAPEntry(Props_pu *);

extern void
v5_gap_pending_change(Widget w, XtPointer data, XtPointer cbs) {
        Props_pu *p = (Props_pu *) data;

        if (p->v5_gap_pending_message_up == False) {
                p->v5_gap_pending_message_up = True;
                XtSetSensitive(p->gap2_add_button, True);
        }
}

extern void
v5_gap_clear_pending_change(Props_pu *p) {
 
	p->v5_gap_pending_message_up = False;
	XtSetSensitive(p->gap2_add_button, False);
}



extern void
p_create_v5_group_access_pane(
	Props_pu *p)
{
	Calendar *c = 	 calendar;
        XmString    	 xstr = NULL;
        int 		 i = 0;
        Widget 		 gap2_form_mgr;
	Dimension	 max_left_label_width,
			 max_button_label_width,
			 tmp;
        Arg 		 args[20]; 
	XtWidgetGeometry geo;

	gap2_form_mgr =
	    XtVaCreateWidget("gap2_form_mgr",
		xmFormWidgetClass,
		p->base_form_mgr,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->separator1,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			0,
		XmNbottomAttachment, 		XmATTACH_WIDGET,
		XmNbottomOffset, 		GAP,
		XmNbottomWidget, 		p->separator2,
		NULL);

	p->pane_list[GROUP_ACCESS_PANE] = gap2_form_mgr;

	/* 
	 * Create our three left hand side labels first in order to figure
	 * out the offset required between the middle widgets and the left
	 * side.
	 */
        xstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 410, "User Name:"));
	p->gap2_user_label =
	    XtVaCreateWidget("gap2_label_user_name",
		xmLabelGadgetClass,
		gap2_form_mgr,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		NULL);

	XmStringFree(xstr);

        xstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 738, "Access List:"));
	p->gap2_access_label =
	    XtVaCreateWidget("gap2_label_access",
		xmLabelGadgetClass,
		gap2_form_mgr,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		NULL);

	XmStringFree(xstr);

        xstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 739, "Access Rights:"));
	p->gap2_permissions_label =
	    XtVaCreateWidget("gap2_label_permissions",
		xmLabelGadgetClass,
		gap2_form_mgr,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		NULL);

	XmStringFree(xstr);

	max_left_label_width = ComputeMaxWidth(p->gap2_user_label,
					       p->gap2_access_label,
					       p->gap2_permissions_label,
					       NULL)
					       + 2 * GAP;

	p->gap2_user_text =
	    XtVaCreateWidget("gap2_text_user_name",
		xmTextWidgetClass,
		gap2_form_mgr,
		XmNmaxLength, 			80,
		XmNbottomAttachment, 		XmATTACH_NONE,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			max_left_label_width,
		XmNtopAttachment, 		XmATTACH_FORM,
                XmNtopOffset, 			GAP,
		NULL);
	p->gap_user_text = p->gap2_user_text;

	XtAddCallback(p->gap_user_text, XmNvalueChangedCallback, 
					v5_gap_pending_change, (XtPointer)p);

	p->gap2_perm_form = CreatePermissions(p, gap2_form_mgr,
					      p->gap2_user_text,
				   	      max_left_label_width, RIGHT_GAP);

	p->gap2_button_form =
	    XtVaCreateWidget("gap2_button_form_mgr",
		xmFormWidgetClass,
		gap2_form_mgr,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_perm_form,
		XmNtopOffset, 			GAP,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset, 		GAP,
		NULL);

        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 740, "Add"));
        p->gap2_add_button =
            XtVaCreateWidget("gap2_button_add",
                xmPushButtonWidgetClass,
                p->gap2_button_form,
                XmNleftAttachment, 		XmATTACH_FORM,
                XmNleftOffset, 			0,
                XmNtopAttachment, 		XmATTACH_FORM,
                XmNtopOffset, 			5,
                XmNalignment, 			XmALIGNMENT_CENTER,
                XmNlabelType, 			XmSTRING,
                XmNlabelString, 		xstr,
                NULL);

	XmStringFree(xstr);
	XtAddCallback(p->gap2_add_button, XmNactivateCallback, GAPAddProc, p);

        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 419, "Delete"));
        p->gap2_delete_button =
            XtVaCreateWidget("gap2_button_delete",
                xmPushButtonWidgetClass,
                p->gap2_button_form,
                XmNleftAttachment, 		XmATTACH_FORM,
                XmNleftOffset, 			0,
                XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_add_button,
		XmNtopOffset, 			GAP,
                XmNalignment, 			XmALIGNMENT_CENTER,
                XmNlabelType, 			XmSTRING,
                XmNlabelString, 		xstr,
                NULL);
        p->gap_remove_button = p->gap2_delete_button; 

	XmStringFree(xstr);
	XtAddCallback(p->gap2_delete_button, XmNactivateCallback,
		      p_gap_remove_proc, p);

        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 343, "Change"));
        p->gap2_change_button =
            XtVaCreateWidget("gap2_button_change",
                xmPushButtonWidgetClass,
                p->gap2_button_form,
                XmNleftAttachment, 		XmATTACH_FORM,
                XmNleftOffset, 			0,
                XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_delete_button,
		XmNtopOffset, 			GAP,
                XmNalignment, 			XmALIGNMENT_CENTER,
                XmNlabelType, 			XmSTRING,
                XmNlabelString, 		xstr,
                NULL);

	XmStringFree(xstr);
	XtAddCallback(p->gap2_change_button, XmNactivateCallback, GAPChangeProc,
		      p);

	max_button_label_width = ComputeMaxWidth(p->gap2_add_button,
						 p->gap2_delete_button,
						 p->gap2_change_button,
						 NULL);

	XtVaSetValues(p->gap2_add_button,
		XmNrightAttachment,		XmATTACH_OPPOSITE_FORM,
		XmNrightOffset,			-max_button_label_width,
		NULL);
	XtVaSetValues(p->gap2_delete_button,
		XmNrightAttachment,		XmATTACH_OPPOSITE_FORM,
		XmNrightOffset,			-max_button_label_width,
		NULL);
	XtVaSetValues(p->gap2_change_button,
		XmNrightAttachment,		XmATTACH_OPPOSITE_FORM,
		XmNrightOffset,			-max_button_label_width,
		NULL);

	p->gap_list = NULL;
        XtSetArg(args[i], XmNscrollBarDisplayPolicy, 	XmAUTOMATIC);  	    ++i;
        XtSetArg(args[i], XmNlistSizePolicy, 		XmVARIABLE);        ++i;
        XtSetArg(args[i], XmNvisibleItemCount, 		10);  		    ++i;
	XtSetArg(args[i], XmNrightAttachment,		XmATTACH_WIDGET);   ++i;
	XtSetArg(args[i], XmNrightWidget,		p->gap2_button_form); ++i;
	XtSetArg(args[i], XmNrightOffset,		GAP * 2);         ++i;
        XtSetArg(args[i], XmNleftAttachment, 		XmATTACH_FORM);     ++i;
        XtSetArg(args[i], XmNleftOffset, 		max_left_label_width);
									    ++i;
        XtSetArg(args[i], XmNtopAttachment, 		XmATTACH_WIDGET);   ++i;
        XtSetArg(args[i], XmNtopOffset, 		GAP);   	    ++i;
        XtSetArg(args[i], XmNtopWidget, 		p->gap2_perm_form); ++i;
        XtSetArg(args[i], XmNbottomAttachment, 		XmATTACH_WIDGET);   ++i;
        XtSetArg(args[i], XmNbottomOffset, 		0);   	    	    ++i;
        XtSetArg(args[i], XmNbottomWidget, 		p->separator2);     ++i;
	XtSetArg(args[i], XmNdoubleClickInterval, 	5); 		    ++i;

        p->gap2_access_list =
            XmCreateScrolledList(gap2_form_mgr, "gap2_access_list", args, i);
	p->gap_access_list = p->gap2_access_list;

	XtAddCallback(p->gap2_access_list, XmNbrowseSelectionCallback,
					   gap_list_select_proc, p);

	XtManageChild(p->gap2_access_list);

	XtQueryGeometry( p->gap2_button_form, NULL, &geo );

	XtVaSetValues( p->gap2_user_text,
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, geo.width + GAP + GAP,
			NULL );
	XtVaSetValues( p->gap2_perm_form,
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, geo.width + GAP + GAP,
			NULL );


	ManageChildren(p->gap2_button_form);

	XtVaSetValues(p->gap2_user_label,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset,			GAP,
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			p->gap2_user_text,
		XmNrightOffset,			5,
		NULL);

	XtVaSetValues(p->gap2_permissions_label,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_user_text,
		XmNtopOffset,			GAP,
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			p->gap2_perm_form,
		XmNrightOffset,			5,
		NULL);

	XtVaSetValues(p->gap2_access_label,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_perm_form,
		XmNtopOffset,			GAP,
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			p->gap2_access_list,
		XmNrightOffset,			5,
		NULL);

	ManageChildren(gap2_form_mgr);
}

static Widget
CreatePermissions(
	Props_pu	*p,
	Widget 		 parent,
	Widget 		 widget_above,
	Dimension	 left_gap,
	Dimension	 right_gap) 
{
	Calendar *c = 	 calendar;
	Widget 		 perm_form;
	int 		 n,
			 max_label_width,
			 label_offset;
	Dimension	 tog_width,
			 view_label_width,
			 insert_label_width,
			 change_label_width;
        XmString    	 xstr = NULL,
			 xstr_empty = NULL;

	/* Create the box containing the permission settings */

	perm_form =
	    XtVaCreateWidget("gap2_perm_form_mgr",
		xmFormWidgetClass,
		parent,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			left_gap,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			widget_above,
		XmNtopOffset, 			GAP,
		XmNshadowThickness,             1,
		XmNshadowType,                  XmSHADOW_IN,
		NULL);

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 743, "Public:"));
	p->gap2_public_label =
	    XtVaCreateWidget("gap2_public_label",
		xmLabelGadgetClass,
		perm_form,
		XmNlabelString, 		xstr,
		NULL);

	XmStringFree (xstr);

	xstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 744, "Semiprivate:"));
	p->gap2_semi_label =
	    XtVaCreateWidget("gap2_semi_label",
		xmLabelGadgetClass,
		perm_form,
		XmNlabelString, 		xstr,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNleftOffset,			10,
		NULL);

	XmStringFree (xstr);

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 745, "Private:"));
	p->gap2_private_label =
	    XtVaCreateWidget("gap2_private_label",
		xmLabelGadgetClass,
		perm_form,
		XmNlabelString, 		xstr,
		NULL);

	XmStringFree (xstr);

	/* Margin to the left of the View label */
	max_label_width = ComputeMaxWidth(p->gap2_public_label,
					  p->gap2_semi_label,
					  p->gap2_private_label,
					  NULL) + 2 * GAP;

	/* Create the labels and toggles for the permissions */

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 40, "View"));
	p->gap2_view_label =
	    XtVaCreateWidget("gap2_view_label",
		xmLabelGadgetClass,
		perm_form,
		XmNlabelString, 		xstr,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			max_label_width,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		NULL);

	XmStringFree (xstr);

	XtVaGetValues(p->gap2_view_label,
		XmNwidth,	&view_label_width,
		NULL);

	/* 
	 * If label is greater than the allocated space for the
	 * label, then we use a 0 offset.
	 */
#ifdef __ppc
	label_offset = MAX_LABEL_SPACE;
	label_offset -= view_label_width;
#else
	label_offset = MAX_LABEL_SPACE - view_label_width;
#endif /* __ppc */
	if (label_offset < 0) label_offset = 0;

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 342, "Insert"));

	p->gap2_insert_label =
	    XtVaCreateWidget("gap2_insert_label",
		xmLabelGadgetClass,
		perm_form,
		XmNlabelString, 		xstr,
		XmNleftAttachment, 		XmATTACH_WIDGET,
		XmNleftWidget, 			p->gap2_view_label,
		XmNleftOffset, 			label_offset,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		NULL);

	XmStringFree (xstr);

	XtVaGetValues(p->gap2_insert_label,
		XmNwidth,	&insert_label_width,
		NULL);

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 343, "Change"));

	/* 
	 * If label is greater than the allocated space for the
	 * label, then we use a 0 offset.
	 */
#ifdef __ppc
	label_offset = MAX_LABEL_SPACE;
	label_offset -= insert_label_width;
#else
	label_offset = MAX_LABEL_SPACE - insert_label_width;
#endif /* __ppc */
	if (label_offset < 0) label_offset = 0;

	p->gap2_change_label =
	    XtVaCreateWidget("gap2_change_label",
		xmLabelGadgetClass,
		perm_form,
		XmNlabelString, 		xstr,
		XmNleftAttachment, 		XmATTACH_WIDGET,
		XmNleftWidget, 			p->gap2_insert_label,
		XmNleftOffset, 			label_offset,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		NULL);

	XmStringFree (xstr);

	XtVaGetValues(p->gap2_change_label,
		XmNwidth,	&change_label_width,
		NULL);

	xstr_empty = XmStringCreateLocalized("");
	p->gap2_public_tog[GAP_VIEW] =
	    XtVaCreateWidget("gap2_public_view_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_view_label,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_view_label,
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	XtVaGetValues(p->gap2_public_tog[GAP_VIEW],
		XmNwidth,			&tog_width,
		NULL);

	/*
	 * Center the toggle item under the label above it.
	 */
	XtVaSetValues(p->gap2_public_tog[GAP_VIEW],
		XmNleftOffset, 			view_label_width/2 -
							(int)(tog_width * 3)/8,
		NULL);

	p->gap2_public_tog[GAP_INSERT] =
	    XtVaCreateWidget("gap2_public_insert_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_insert_label,
		XmNleftOffset, 			insert_label_width/2 -
							(int)(tog_width * 3)/8,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_insert_label,
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	p->gap2_public_tog[GAP_CHANGE] =
	    XtVaCreateWidget("gap2_public_change_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_change_label,
		XmNleftOffset, 			change_label_width/2 - 
							(int)(tog_width * 3)/8,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_change_label,
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	p->gap2_semi_tog[GAP_VIEW] =
	    XtVaCreateWidget("gap2_semi_view_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_public_tog[GAP_VIEW],
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_public_tog[GAP_VIEW],
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	p->gap2_semi_tog[GAP_INSERT] =
	    XtVaCreateWidget("gap2_semi_insert_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_public_tog[GAP_INSERT],
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_public_tog[GAP_INSERT],
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);
		
	p->gap2_semi_tog[GAP_CHANGE] =
	    XtVaCreateWidget("gap2_semi_change_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_public_tog[GAP_CHANGE],
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_public_tog[GAP_CHANGE],
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	p->gap2_private_tog[GAP_VIEW] =
	    XtVaCreateWidget("gap2_private_view_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_semi_tog[GAP_VIEW],
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_semi_tog[GAP_VIEW],
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	p->gap2_private_tog[GAP_INSERT] =
	    XtVaCreateWidget("gap2_private_insert_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_semi_tog[GAP_INSERT],
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_semi_tog[GAP_INSERT],
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);
		
	p->gap2_private_tog[GAP_CHANGE] =
	    XtVaCreateWidget("gap2_private_change_tog",
		xmToggleButtonGadgetClass,
		perm_form,
		XmNlabelString, 		xstr_empty,
		XmNleftAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 			p->gap2_semi_tog[GAP_CHANGE],
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			p->gap2_semi_tog[GAP_CHANGE],
		XmNtopOffset, 			TOG_GAP,
		XmNindicatorSize, 		INDICATOR_SIZE,
		NULL);

	XmStringFree (xstr_empty);

	XtVaSetValues(p->gap2_public_label,
		XmNtopAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,			p->gap2_public_tog[GAP_VIEW],
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			p->gap2_public_tog[GAP_VIEW],
		XmNrightOffset,			2 * GAP,
		NULL);

	XtVaSetValues(p->gap2_semi_label,
		XmNtopAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,			p->gap2_semi_tog[GAP_VIEW],
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			p->gap2_public_tog[GAP_VIEW],
		XmNrightOffset,			2 * GAP,
		NULL);

	XtVaSetValues(p->gap2_private_label,
		XmNtopAttachment, 		XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,			p->gap2_private_tog[GAP_VIEW],
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			p->gap2_public_tog[GAP_VIEW],
		XmNrightOffset,			2 * GAP,
		XmNbottomAttachment,		XmATTACH_FORM,
		XmNbottomOffset,		GAP,
		NULL);

	/* Margin to the right of the Change label */
	XtVaSetValues(p->gap2_change_label,
		XmNmarginRight,			GAP,
		NULL);

	ManageChildren(perm_form);
	return (perm_form);
}

static void
GAPAddProc(
	Widget		 	 w,
	XtPointer	 	 client_data,
	XtPointer	 	 callback_struct)
{
	Calendar 		*c = calendar;
	Props_pu        	*p = (Props_pu *)client_data;
	CSA_flags	 	 access_rights;
	CSA_access_rights	*access_entry,
				*access_list_ptr;
	char			*user_name;
	XmString		 xmstr;

	/*
	 * Get the necessary strings from the UI.
	 */
	if (!(access_rights = p_gap_convert_permissions(p)))
		 return;

	/* Get the user's name and host */
	if (!(user_name = (char *)XmTextGetString(p->gap2_user_text)) ||
	    !*user_name) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 750, 
			      "Calendar : Error - Access List and Permissions"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 749,
		      "Name and Access Rights must be set to add an item to the Access List."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95,
						   "Continue"));

		XtFree(user_name);
                dialog_popup(p->frame,
                        DIALOG_TITLE, title,
                        DIALOG_TEXT, text,
                        BUTTON_IDENT, 1, ident1,
			BUTTON_HELP, ACCESS_RIGHTS_ERROR_HELP,
                        DIALOG_IMAGE, p->xm_error_pixmap,
                        NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		return;
	}

	xmstr = XmStringCreateLocalized(user_name);

	/* See if the item is already in the list */
	if (p->gap_list) {
		int	list_pos = XmListItemPos(p->gap_access_list, xmstr);

		if (list_pos) {
			XmListSelectPos(p->gap_access_list, list_pos, False);
			XmListSetBottomPos(p->gap_access_list, list_pos);
			XtSetSensitive(p->gap2_change_button, True);
			XtSetSensitive(p->gap2_delete_button, True);

			/* Does the user want us to change an existing item? */
			if (!ChangeExistingGAPEntry(p)) {
				/* No */
				XtFree(user_name);
				XmStringFree(xmstr);
				return;
			}

			/* Find the entry and change the rights. */
			access_list_ptr = p->gap_list;
			do {
				if (!strcmp(access_list_ptr->user->user_name,
								   user_name)) {
					access_list_ptr->rights = access_rights;
					p->changed_flag = True;
					p_set_change(p);
					XtFree(user_name);
					XmStringFree(xmstr);
					return;
				}
			} while (access_list_ptr = access_list_ptr->next);
		}
	}

	/* Create a new access entry and add it to the end of the list */
	access_entry = 
		(CSA_access_rights *)ckalloc(sizeof(CSA_access_rights));
	access_entry->user = 
		(CSA_calendar_user *)ckalloc(sizeof(CSA_calendar_user));
	access_entry->user->user_name = cm_strdup(user_name);
	access_entry->rights = access_rights;
	access_entry->next = NULL;

	if (!p->gap_list) { 
		p->gap_list = access_entry;
	} else {
		access_list_ptr = p->gap_list;
		while (access_list_ptr->next)
			access_list_ptr = access_list_ptr->next;
		access_list_ptr->next = access_entry;
	}

	XmListAddItem(p->gap_access_list, xmstr, 0);
	p->changed_flag = True;
	p_set_change(p);
	XtFree(user_name);
	XmStringFree(xmstr);
}

static int
ChangeExistingGAPEntry(
	Props_pu	*p)
{
	Calendar 	*c = calendar;
	int		 answer;

	char *title = XtNewString(catgets(c->DT_catd, 1, 1078, 
			"Calendar : Access List and Permissions - Add"));
	char *text = XtNewString(catgets(c->DT_catd, 1, 751,
		"This entry already exists in the Access List.  Do you wish to change it?"));
	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 343,
			"Change"));
	char *ident2 = XtNewString(catgets(c->DT_catd, 1, 923,
			"Cancel"));

	answer = dialog_popup(p->frame,
		DIALOG_TITLE, title,
		DIALOG_TEXT, text,
		BUTTON_IDENT, 2, ident2,
		BUTTON_IDENT, 1, ident1,
		NULL);
	XtFree(ident2);
	XtFree(ident1);
	XtFree(text);
	XtFree(title);

	if (answer == 1)
		return False;
	return True;
}

static void
GAPChangeProc(
	Widget		 	 w,
	XtPointer	 	 client_data,
	XtPointer	 	 callback_struct)
{
	Calendar 		*c = calendar;
	Props_pu        	*p = (Props_pu *)client_data;
	CSA_access_list 	 step = p->gap_list, 
				 last = NULL;
	int			*item_list = NULL,
				 item_cnt = 0;
	CSA_flags	 	 access_rights;
	CSA_access_rights	*access_entry;
	char			*user_name;
	XmString		 xmstr;


	/* An item to change must be selected in the access list */
	if (!XmListGetSelectedPos(p->gap2_access_list, &item_list, &item_cnt)) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 750,
			      "Calendar : Error - Access List and Permissions"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 408,
			      "Please select a name first.                   "));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(p->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		return;
	}

	/*
	 * Get the necessary strings from the UI.
	 */
	if (!(access_rights = p_gap_convert_permissions(p)))
		 return;

	/* Get the user's name and host */
	if (!(user_name = (char *)XmTextGetString(p->gap2_user_text)) ||
	    !*user_name) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 750, 
			      "Calendar : Error - Access List and Permissions"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 756,
	              "Name and Access rights must be set to change an item in the Access List"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));

		XtFree(user_name);
                dialog_popup(p->frame,
                        DIALOG_TITLE, title,
                        DIALOG_TEXT, text,
                        BUTTON_IDENT, 1, ident1,
                        DIALOG_IMAGE, p->xm_error_pixmap,
                        NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		return;
	}

	xmstr = XmStringCreateLocalized(user_name);

	/* Create a new access entry and add it to the end of the list */
	access_entry = 
		(CSA_access_rights *)ckalloc(sizeof(CSA_access_rights));
	access_entry->user = 
		(CSA_calendar_user *)ckalloc(sizeof(CSA_calendar_user));
	access_entry->user->user_name = cm_strdup(user_name);
	access_entry->rights = access_rights;
	access_entry->next = NULL;

	/*
	 * Find and change the Access_Entry/name from the list.
	 */
	item_cnt = 1;
	while(item_cnt < *item_list) {
		last = step;
		step = step->next;
		++item_cnt;
	}

	if (last)
		last->next = access_entry;
	else     
		p->gap_list = access_entry;
 
	access_entry->next = step->next;

	free(step->user);
	free(step);
 
	XmListReplaceItemsPos(p->gap2_access_list, &xmstr, 1, *item_list);

        p->changed_flag = True;
	p_set_change(p);

        XmStringFree(xmstr);
	XtFree((XtPointer)item_list);
}
