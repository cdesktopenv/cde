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
/*******************************************************************************
**
**  blist.c
**
**  $TOG: blist.c /main/5 1999/02/08 17:02:59 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static 	char sccsid[] = "@(#)blist.c 1.66 95/03/28 Copyr 1994 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include "browser.h"
#include "calendar.h"
#include "util.h"
#include "misc.h"
#include "timeops.h"
#include "blist.h"
#include "props.h"
#include "help.h"


static void
bl_pending_change(Widget w, XtPointer data, XtPointer cbs) {
	Calendar *c = (Calendar *) data;
	Browselist	*bl = (Browselist *)c->browselist;

	if (bl->bl_pending_message_up == False) {
		/* NL_COMMENT
		   Attention Translator:

		   Message 841 is used in the Menu Editor's footer.  The
		   Menu Editor is accessed through the browse menu.  The 
		   message is displayed when something is typed into the User
		   Name field in the editor.  If the translated footer message 
		   is too long it causes the menu editor to grow horizontally
		   which we do not want.  If you notice that the translated
		   string causes the editor to grow horizontally please
		   insert a newline (\n) character somewhere in the middle
		   of the string so the footer will use two lines and the
		   editor will grow vertically.
		*/
		set_message(bl->message, catgets(c->DT_catd, 1, 841, "Click on \"Add Name\" to add a name, \"Apply\" to commit changes."));
		bl->bl_pending_message_up = True;
		XtSetSensitive(bl->add_button, True);
	}
}

static void
bl_clear_pending_change(Browselist *bl) {

	if (bl->bl_pending_message_up == True) {
		set_message(bl->message, " ");
		bl->bl_pending_message_up = False;
		XtSetSensitive(bl->add_button, False);
	}
}

static void
bl_list_is_changed(Browselist *bl) {

	if (bl->bl_list_changed == False) {
		bl->bl_list_changed = True;
		XtSetSensitive(bl->ok_button, True);
		XtSetSensitive(bl->apply_button, True);
		XtSetSensitive(bl->reset_button, True);
        	XtVaSetValues(bl->form, XmNdefaultButton, bl->apply_button, NULL);
	}
}

static void
bl_list_not_changed(Browselist *bl) {

	bl->bl_list_changed = False;
	XtSetSensitive(bl->ok_button, False);
	XtSetSensitive(bl->apply_button, False);
	XtSetSensitive(bl->reset_button, False);
       	XtVaSetValues(bl->form, XmNdefaultButton, bl->cancel_button, NULL);
}

static void
bl_list_selection_cb(Widget w, XtPointer data, XtPointer cbs) {
	Calendar *c = (Calendar *) data;
	Browselist	*bl = (Browselist *)c->browselist;
	int		*pos_list, pos_cnt;

	XmListGetSelectedPos(bl->browse_list, &pos_list, &pos_cnt);
	if (pos_cnt <= 0) 
		XtSetSensitive(bl->remove_button, False);
	else
		XtSetSensitive(bl->remove_button, True);
}

/*
 * Remove the name from the UI and from the list if the calendar handle is NULL.
 * If the calendar handle is not null, tag it as deleted and it will be taken
 * care of later.
 */
static void
blist_removenames(Widget widget, XtPointer client_data, XtPointer call_data) {
	int		i, idx, valid_cnt, *pos_list, pos_cnt;
	Calendar	*c = (Calendar *)client_data;
	BlistData	*bd = NULL;
	Browselist	*bl = (Browselist *)c->browselist;
	int		rejected_name = 0;

	set_message(bl->message, " ");
	XmListGetSelectedPos(bl->browse_list, &pos_list, &pos_cnt);
	if (pos_cnt <= 0) {
		set_message(bl->message, catgets(calendar->DT_catd, 1, 17,
					      "Select a name to remove"));
		return;
	}

	for (i = 0; i < pos_cnt; i++) {
		if (pos_list[i] == 1) {
			set_message(bl->message, catgets(calendar->DT_catd, 1,
				16, "You may not remove the default calendar"));
			rejected_name++;
			continue;
		}

		XmListDeletePos(bl->browse_list, pos_list[i] - i + rejected_name);
		bd = (BlistData *)CmDataListGetData(bl->blist_data, pos_list[i]);

		if (bd)
		{
			bd->tag = BLIST_DELETE;
		}
	}
	blist_clean(bl, False);
	bl_list_is_changed(bl);
}

/*
 * Append the new name to the list and set the necessary tags.  Note this only
 * adds the name to the linked list - not to the UI.  Also, it won't add a
 * duplicate name.
 */
static int
blist_name_append(Calendar *c, char *name, BlistTag t) {
        int             i = 1;
	Boolean		marked = False;
        int             location = 2;
        BlistData       *bd;
        Browselist      *bl = (Browselist *)c->browselist;
	char		*defname;
 

	/* This while loop is doing double duty here.  The primary 
	   purpose of the list is to find out if the name we're 
	   inserting is already in the list.  While looping thru, 
	   the secondary purpose is to find the lexicographical 
	   position of the name relative to the current set of entries.  
	   The business about marked and location is related to that 
	   second purpose. */

        while ((bd = (BlistData *)CmDataListGetData(bl->blist_data, i)) &&
               strcmp(bd->name, name) != 0)
	{
                if ((marked == False) && (strcoll(name, bd->name) < 0) && 
		    (i != 1)) {
                        location = i;
			marked = True;
		}
                ++i;
	}

        if (bd) {
		/* Since the user may have hit the reset button, any items
		 * that had been marked for deletion need to be made active.
		 */
		if (bd->tag == BLIST_DELETE)
			bd->tag = BLIST_ACTIVE;

                return -1;
	}
	
	/* 2 special cases here.  If the name is that of the calendar 
	   owner, it should always be at the head of the list.  If the 
	   name wasn't maked against any of the people on the list, 
	   then it should be inserted at the end. */

	defname = get_user_calendar();
	if (strcmp(defname, name) == 0) {
		location = 1;
	} else if (marked == False) {
		location = i;
	}
	free(defname);
 
        bd = (BlistData *)ckalloc(sizeof(BlistData));
        bd->name = cm_strdup(name);
        bd->tag = t;
        bd->cal_handle = 0;
        CmDataListAdd(bl->blist_data, (void *)bd, location);
 
        return location;

}

/*
 * Callback for the add name button.  This simply calls the blist_name_append
 * function and adds the name to the UI.
 */
static void
blist_addname(Widget widget, XtPointer client_data, XtPointer cbs) {
	char		*new_name, *end_ptr, buf[MAXNAMELEN];
	XEvent		*e = ((XmAnyCallbackStruct *)cbs)->event;
	XmString	xmstr;
	Calendar	*c = (Calendar *)client_data;
	Browselist	*bl = (Browselist *)c->browselist;
	int		insert_location;


	XtVaSetValues(bl->form, XmNresizePolicy, XmRESIZE_NONE, NULL);
	bl_clear_pending_change(bl);
	bl_list_is_changed(bl);
	set_message(bl->message, " ");
	new_name = XmTextFieldGetString(bl->username);

	/* crush out leading white space for the name 
	   comparison/insert process */
	
	while ((*new_name == ' ') || (*new_name == '\t'))
		new_name++;

	/* compress off trailing whitespace */

	end_ptr = new_name;
	while (*end_ptr)
		end_ptr++;
	while ((end_ptr > new_name) && 
	       ((*(end_ptr - 1) == ' ') || (*(end_ptr - 1) == '\t')))
		end_ptr--;

	*end_ptr = '\0';


	if (blank_buf(new_name)) {
		set_message(bl->message, catgets(c->DT_catd, 1, 603,
			"Type a name to add in the User Name field"));
		return;
	}

	if (embedded_blank(new_name)) {
                set_message(bl->message, catgets(c->DT_catd, 1, 684,
                        "User Names may not have embedded blanks or tabs"));
                return;
        }


	if ((insert_location = blist_name_append(c, new_name, BLIST_INSERT)) != -1) {
		xmstr = XmStringCreateLocalized(new_name);
		XmListAddItem(bl->browse_list, xmstr, insert_location);
		XmStringFree(xmstr);
		cm_select_text(bl->username, e->xbutton.time);
	} else {
		sprintf(buf, "%s %s", new_name,
			catgets(c->DT_catd, 1, 604, "is already in the list"));
		set_message(bl->message, buf);
	}
	XtVaSetValues(bl->form, XmNresizePolicy, XmRESIZE_ANY, NULL);
}

/*
 * This sends the new list of names to the properties database.  Note it ignores
 * items that have been tagged for deletion.
 */
static void
blist_write_list(Browselist *bl, Props *p) {
	int		i, len = 0;
	char		*buf;
	BlistData	*bd;

	/*
	 * First pass, count the number of bytes we're going to need
	 */
	for (i = 1; i <= bl->blist_data->count; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, i);
		if (bd && bd->name && bd->tag != BLIST_DELETE)
			len += cm_strlen(bd->name) + 2; /* one for spacing */
	}
	if (len <= 0)
		return;

	/*
	 * We have names, so build the string, making sure to exclude items
	 * tagged for delete.
	 */
	buf = (char *)ckalloc(len);
	memset(buf, '\0', len);
	for (i = 1; i <= bl->blist_data->count; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, i);
		if (bd && bd->name && bd->tag != BLIST_DELETE) {
			cm_strcat(buf, bd->name);
			cm_strcat(buf, " ");
			bd->tag = BLIST_ACTIVE;
		}
	}
        set_char_prop(p, CP_DAYCALLIST, buf);
	save_props(p);
        free(buf);
}

/*
 * Callback for the OK button - will write the list to the properties
 * database, will update the browse menu on the main menu, and will update the
 * menu on the multi browser if it's up.
 */
static void
blist_ok(Widget widget, XtPointer client_data, XtPointer call_data) {
	Calendar 	*c = (Calendar *)client_data;
	Browselist	*bl = (Browselist *)c->browselist;
	Browser		*b = (Browser *)c->browser;
	Props		*p = (Props *)c->properties;

	bl_clear_pending_change(bl);
	bl_list_not_changed(bl);
	set_message(bl->message, " ");
	blist_write_list(bl, p);

	XtVaSetValues(c->browse_button, XmNsubMenuId, NULL, NULL);
	update_browse_menu_names(c);
	XtVaSetValues(c->browse_button, XmNsubMenuId, c->browse_menu, NULL);
	if (b && b->frame)
		browser_reset_list(c);

	XtPopdown(bl->frame);
}

/*
 * Callback for the apply button - will write the list to the properties
 * database, will update the browse menu on the main menu, and will update the
 * menu on the multi browser if it's up.
 */
static void
blist_apply(Widget widget, XtPointer client_data, XtPointer call_data) {
	Calendar 	*c = (Calendar *)client_data;
	Browselist	*bl = (Browselist *)c->browselist;
	Browser		*b = (Browser *)c->browser;
	Props		*p = (Props *)c->properties;

	bl_clear_pending_change(bl);
	bl_list_not_changed(bl);
	set_message(bl->message, " ");
	blist_write_list(bl, p);

	XtVaSetValues(c->browse_button, XmNsubMenuId, NULL, NULL);
	update_browse_menu_names(c);
	XtVaSetValues(c->browse_button, XmNsubMenuId, c->browse_menu, NULL);
	if (b && b->frame)
		browser_reset_list(c);
}

static void
blist_cancel_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	Calendar	*c = (Calendar *)client_data;
	Browselist	*bl = (Browselist *)c->browselist;

	blist_reset(c);

	XtPopdown(bl->frame);
}

static void
blist_reset_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	Calendar	*c = (Calendar *)client_data;
	Browselist	*bl = (Browselist *)c->browselist;

	blist_reset(c);
}

extern void
blist_init_names(Calendar *c) {
        char		*name, *namelist;
        Props		*p = (Props*)c->properties;
	Browselist	*bl = (Browselist *)c->browselist;
 
	if (!bl->blist_data)
		bl->blist_data = CmDataListCreate();

	/*
	 * Make sure the default calendar is in list
	 */
	name = get_user_calendar();
	blist_name_append(c, name, BLIST_ACTIVE);
	free(name);

	/*
	 * Make sure user's calendar is in list
	 */
	blist_name_append(c, c->calname, BLIST_ACTIVE);

	/* make sure the intiial view name is in the list. */

	blist_name_append(c, get_char_prop(p, CP_DEFAULTCAL), BLIST_ACTIVE);

        namelist = cm_strdup(get_char_prop(p, CP_DAYCALLIST));
        if (namelist == NULL || *namelist == '\0' )
		return;

	name = strtok(namelist, " ");
	while (name) {
		blist_name_append(c, name, BLIST_ACTIVE);
		name = strtok(NULL, " ");
	}
	free(namelist);
}

extern void
blist_init_ui(Calendar *c) {
	int		i;
	XmString	xmstr;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;

	XtVaSetValues(bl->form, XmNresizePolicy, XmRESIZE_NONE, NULL);
	XmListDeleteAllItems(bl->browse_list);
	for (i = 1; i <= bl->blist_data->count; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, i);
		if (bd && bd->name && bd->tag != BLIST_DELETE) {
			xmstr = XmStringCreateLocalized(bd->name);
			if (!XmListItemExists(bl->browse_list, xmstr))
				XmListAddItem(bl->browse_list, xmstr, 0);
			XmStringFree(xmstr);
		}
	}
	XmTextFieldSetString(bl->username, "");
	XtVaSetValues(bl->form, XmNresizePolicy, XmRESIZE_ANY, NULL);
}

extern void
blist_clean(Browselist *bl, Boolean clean_all) {
	int		i, left_cnt = 1, cnt;
	BlistData	*bd;

	if (NULL == bl->blist_data) return;

	cnt = bl->blist_data->count;
	for (i = 1; i <= cnt; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, left_cnt);
		if (bd && bd->cal_handle == 0 &&
		    (clean_all || bd->tag != BLIST_ACTIVE)) {
			if (bd->name)
				free(bd->name);
			CmDataListDeletePos(bl->blist_data, left_cnt, True);
		} else
			++left_cnt;
	}
}

extern void
blist_reset(Calendar *c) {
	Browselist	*bl = (Browselist *)c->browselist;

	blist_clean(bl, True);
	blist_init_names(c);
	blist_init_ui(c);
	bl_clear_pending_change(bl);
	bl_list_not_changed(bl);
	XtSetSensitive(bl->remove_button, False);
}

static void
blist_quit_handler(Widget w, XtPointer cdata, XtPointer data) {
	Browselist	*bl = (Browselist *)cdata;

	XtPopdown(w);
}

extern void
make_browselist(Calendar *c)
{
	int		ac;
	Arg		args[15];
	XmString	label_str;
	Widget		separator,
			button_form;
	Browselist	*bl = (Browselist *)c->browselist;
	Dimension	label_width;
	char		*title;

	if (!bl)
		return;

	title = XtNewString(catgets(c->DT_catd, 1, 963, 
				    "Calendar : Menu Editor"));
	bl->frame = XtVaCreatePopupShell("menu_editor_frame",
                xmDialogShellWidgetClass, c->frame,
		XmNdeleteResponse, 	XmDO_NOTHING,
                XmNtitle, title,
                XmNallowShellResize, 	True,
		XmNmappedWhenManaged, 	False,
		NULL);
	setup_quit_handler(bl->frame, blist_quit_handler, (caddr_t)bl);
	XtFree(title);

        bl->form = XtVaCreateWidget("menu_editor_form",
                xmFormWidgetClass, bl->frame,
                XmNautoUnmanage, 	False,
                XmNfractionBase, 	5,
                NULL);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 410, "User Name:"));
       	bl->username_label = XtVaCreateWidget("name_label",
		xmLabelGadgetClass, bl->form,
		XmNlabelString, 	label_str,
               	NULL);
	XmStringFree(label_str);

	ac = 0;
	XtSetArg(args[ac], XmNorientation, 	XmVERTICAL); 	++ac;
	XtSetArg(args[ac], XmNpacking, 		XmPACK_COLUMN); ++ac;
	XtSetArg(args[ac], XmNtopAttachment, 	XmATTACH_FORM); ++ac;
	XtSetArg(args[ac], XmNtopOffset, 	6); 		++ac;
	XtSetArg(args[ac], XmNrightAttachment, 	XmATTACH_FORM); ++ac;
	XtSetArg(args[ac], XmNrightOffset, 	5); 		++ac;
	bl->edit_rc_mgr = XmCreateRowColumn(bl->form, "edit_rc_mgr", args, ac);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 686, "Add Name"));
       	bl->add_button = XtVaCreateWidget("add_button",
		xmPushButtonWidgetClass, bl->edit_rc_mgr,
		XmNlabelString, 	label_str,
		XmNnavigationType, 	XmTAB_GROUP,
               	NULL);
	XmStringFree(label_str);
	XtAddCallback(bl->add_button, XmNactivateCallback,
				blist_addname, (XtPointer)c);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 687, "Remove Name"));
   	bl->remove_button = XtVaCreateWidget("remove_button",
		xmPushButtonWidgetClass, bl->edit_rc_mgr,
		XmNlabelString, 	label_str,
		XmNnavigationType, 	XmTAB_GROUP,
                NULL);
	XmStringFree(label_str);
        XtAddCallback(bl->remove_button, XmNactivateCallback, 
				blist_removenames, (XtPointer)c);
	ManageChildren(bl->edit_rc_mgr);

	XtVaGetValues(bl->username_label, XmNwidth, &label_width, NULL);

       	bl->username = XtVaCreateWidget("username",
		xmTextFieldWidgetClass, bl->form,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		label_width + 15,
		XmNrightAttachment, 	XmATTACH_WIDGET,
		XmNrightWidget, 	bl->edit_rc_mgr,
		XmNrightOffset, 	5,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset, 		5,
		XmNcolumns, 		40,
               	NULL);
	XtAddCallback(bl->username, XmNactivateCallback, blist_addname,
				(XtPointer)c);
	XtAddCallback(bl->username, XmNvalueChangedCallback, 
				bl_pending_change, (XtPointer)c);

	XtVaSetValues(bl->username_label,
		XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                XmNbottomWidget,        bl->username,
                XmNbottomOffset,        5,
                XmNrightAttachment,     XmATTACH_WIDGET,
                XmNrightWidget,         bl->username,
                XmNrightOffset,         5,
                NULL);

	label_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 688, "Browse Menu Items"));
       	bl->list_label = XtVaCreateWidget("list_label", 
		xmLabelWidgetClass, bl->form,
		XmNlabelString, 	label_str,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		bl->username,
		XmNtopOffset, 		10,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
               	NULL);
	XmStringFree(label_str);

	bl->message = XtVaCreateWidget("message_text",
                xmLabelGadgetClass, 
		bl->form,
		XmNalignment, 		XmALIGNMENT_BEGINNING,
                XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
                XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
                XmNbottomAttachment, 	XmATTACH_FORM,
		XmNbottomOffset, 	5,
                NULL);

	button_form = XtVaCreateWidget("menu_editor_button_form_mgr",
                xmFormWidgetClass,
                bl->form,
                XmNautoUnmanage,        False,
                XmNfractionBase,        5,
                XmNhorizontalSpacing,   5,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNleftOffset,          5,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNrightOffset,         5,
                XmNbottomAttachment,    XmATTACH_WIDGET,
                XmNbottomWidget,        bl->message,
                XmNbottomOffset,        5,
                NULL);
	
	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 655, "OK"));
	bl->ok_button = XtVaCreateWidget("ok_button",
		xmPushButtonWidgetClass,
		button_form,
		XmNlabelString, 	label_str,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	0,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	1,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(bl->ok_button, XmNactivateCallback, blist_ok,
		      		(XtPointer)c);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 460, "Apply"));
	bl->apply_button = XtVaCreateWidget("apply_button",
		xmPushButtonWidgetClass,
		button_form,
		XmNlabelString, 	label_str,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	1,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	2,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(bl->apply_button, XmNactivateCallback, blist_apply,
		      		(XtPointer)c);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 691, "Reset"));
        bl->reset_button = XtVaCreateWidget("reset_button",
		xmPushButtonWidgetClass,
		button_form,
		XmNlabelString, 	label_str,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	2,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(bl->reset_button, XmNactivateCallback,
			blist_reset_cb, (XtPointer)c);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 923, "Cancel"));
        bl->cancel_button = XtVaCreateWidget("cancel_button",
	 	xmPushButtonWidgetClass,
		button_form,
		XmNlabelString, 	label_str,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	3,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	4,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(bl->cancel_button, XmNactivateCallback, blist_cancel_cb,
				(XtPointer)c);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        bl->help_button = XtVaCreateWidget("help_button",
		xmPushButtonWidgetClass,
		button_form,
		XmNlabelString, 	label_str,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	4,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	5,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_FORM,
                NULL);
	XmStringFree(label_str);
        XtAddCallback(bl->help_button, XmNactivateCallback, 
		(XtCallbackProc)help_cb, MENU_EDITOR_HELP_BUTTON);
        XtAddCallback(bl->form, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) MENU_EDITOR_HELP_BUTTON);

	ManageChildren(button_form);

	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                bl->form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNbottomAttachment,   	XmATTACH_WIDGET,
                XmNbottomWidget,        button_form,
		XmNbottomOffset, 	5,
                NULL);
	ac = 0;
	XtSetArg(args[ac], XmNvisibleItemCount, 8); ++ac;
	XtSetArg(args[ac], XmNselectionPolicy, XmMULTIPLE_SELECT); ++ac;
	XtSetArg(args[ac], XmNdoubleClickInterval, 5); ++ac;
	bl->browse_list = (Widget)XmCreateScrolledList(bl->form, "browse_list",
						       args, ac);
        bl->browse_list_sw = XtParent(bl->browse_list);

	XtVaSetValues(bl->browse_list_sw,
                XmNvisualPolicy, 	XmVARIABLE,
                XmNtopAttachment, 	XmATTACH_WIDGET,
                XmNtopWidget, 		bl->list_label,
		XmNtopOffset, 		5,
                XmNleftAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 		bl->list_label,
		XmNrightAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, 	bl->username,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	separator,
		XmNbottomOffset,	5,
		XmNwidth, 50,
		NULL);
	XtManageChild(bl->browse_list);

        XtAddCallback(bl->browse_list, XmNmultipleSelectionCallback, 
				bl_list_selection_cb, (XtPointer)c);

	ManageChildren(bl->form);
	XtManageChild(bl->form);

        /*
	 * Set default button
	 */
        XtVaSetValues(bl->form, XmNcancelButton, bl->cancel_button, NULL);
        XmProcessTraversal(bl->username, XmTRAVERSE_CURRENT);
        XtVaSetValues(bl->form, XmNinitialFocus, bl->username, NULL);

	/*
	 * For storing the list of names
	 */
	if (!bl->blist_data)
		bl->blist_data = CmDataListCreate();
}

extern void
show_browselist(Calendar *c) {
	Browselist	*bl;

	if (c->browselist == NULL)
		c->browselist = (caddr_t)ckalloc(sizeof(Browselist));
	bl = (Browselist *)c->browselist;

	if (!bl->frame)
		make_browselist(c);
	blist_reset(c);
        XtVaSetValues(bl->frame, XmNmappedWhenManaged, True, NULL);
        /*
	 * Set default button
	 */
        XtVaSetValues(bl->form, XmNcancelButton, bl->cancel_button, NULL);
        XmProcessTraversal(bl->username, XmTRAVERSE_CURRENT);
        XtVaSetValues(bl->form, XmNinitialFocus, bl->username, NULL);

	XtPopup(bl->frame, XtGrabNone);
	set_message(bl->message, "\0");
}

