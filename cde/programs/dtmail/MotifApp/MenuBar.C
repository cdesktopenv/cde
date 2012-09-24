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
/* $TOG: MenuBar.C /main/6 1997/06/03 16:12:28 mgreess $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//         Copyright 1991 by Prentice Hall
//         All Rights Reserved
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// MenuBar.C: A menu bar whose panes support items
//            that execute Cmd's
//////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// MODIFIED TO SUPPORT SUBMENUS - not described in Book
///////////////////////////////////////////////////////////


#include "Application.h"
#include "MenuBar.h"
#include "Cmd.h"
#include "CmdList.h"
#include "ToggleButtonInterface.h"
#include "ButtonInterface.h"
#include "Help.hh"
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>


extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}

MenuBar::MenuBar ( Widget parent, char *name, unsigned char type ) 
: UIComponent ( name )
{
    // Base widget is a Motif menu bar widget
    
    if(parent == NULL)
	return;

    if( type == XmMENU_POPUP) {
	Arg args[1];
	int i = 0;

	XtSetArg(args[i], XmNwhichButton, theApplication->bMenuButton()); i++;
    	_w = XmCreatePopupMenu( parent, _name, args, i );
//        XtVaSetValues(_w, XmNwhichButton, theApplication->bMenuButton(), NULL);
    } else {
	type = XmMENU_BAR; // Force it to menu bar
    	_w = XmCreateMenuBar ( parent, _name, NULL, 0 );
    }

    printHelpId("_w", _w);
    /* install callback */
    // XtAddCallback(_w, XmNhelpCallback, HelpCB, helpId);
    
    installDestroyHandler();
}
Widget MenuBar::addCommands ( Widget *menuBarCascade, CmdList *list, Boolean help, unsigned char type )
{
    return( createPulldown ( _w, list, menuBarCascade, help, type ) );
}

Widget MenuBar::addCommands ( CmdList *list, Boolean help, unsigned char type )
{
    return( createPulldown ( _w, list, help, type ) );
}

Widget 
MenuBar::createPulldown ( 
	Widget parent, 
	CmdList *list, 
	Widget *cascade, 
	Boolean help ,
	unsigned char type)
{
    int    i;
    Widget pulldown;
    Arg  args2[2];
    char * helpId;
    XmString label_str;

    if(type != XmMENU_POPUP)
	type = XmMENU_BAR;
    
    // Create a pulldown menu pane for this list of commands
    
    if( type == XmMENU_BAR) {
		pulldown = XmCreatePulldownMenu( parent,
				(char *) list->name(), NULL, 0 );
		label_str = XmStringCreateLocalized(list->getLabel());
		XtVaSetValues(pulldown,
			      XmNlabelString, label_str,
			      NULL);
		printHelpId("pulldown", pulldown);
		/* install callback */
		// XtAddCallback(pulldown, XmNhelpCallback, HelpCB, helpId);

		
		// Each entry in the menu bar must have a cascade button
		// from which the user can pull down the pane

		if (cascade != NULL && *cascade != NULL)
                        XtVaSetValues (*cascade, XmNsubMenuId, pulldown, 
				       XmNlabelString, label_str,
				       NULL);
                else {

			XtSetArg(args2[0], XmNsubMenuId, pulldown);
			*cascade = XtCreateWidget ( list->name(), 
				       xmCascadeButtonWidgetClass,
				       parent, 
				       args2, 1);
			XtVaSetValues(*cascade,
				      XmNlabelString, label_str,
				      NULL);
			if (help) {
				XtSetArg (args2[0], XmNmenuHelpWidget, *cascade);
				XtSetValues (parent, args2, 1);
			}
			
			XtManageChild ( *cascade );
			printHelpId("cascade", *cascade);
			/* install callback */


			// Install callbacks for each of the 
			// pulldown menus so we can get
			// On Item help for them.

			helpId = XtName(*cascade);
			if (helpId == "Mailbox") {
				XtAddCallback(*cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILCONTAINERMENUID);
			} else if (helpId == "Edit") {
				XtAddCallback(*cascade, XmNhelpCallback, 
					HelpCB, (void *)DTMAILEDITMENUID);
			} else if (helpId == "Message") {
				XtAddCallback(*cascade, XmNhelpCallback, 
					HelpCB, (void *)DTMAILMESSAGEMENUID);
			} else if (helpId == "Attachments") {
				XtAddCallback(*cascade, XmNhelpCallback, 
					HelpCB, (void *)DTMAILATTACHMENUID);
			} else if (helpId == "View") {
				XtAddCallback(*cascade, XmNhelpCallback, 
					HelpCB, (void *)DTMAILVIEWMENUID);
			} else if (helpId == "Compose") {
				XtAddCallback(*cascade, XmNhelpCallback, 
					HelpCB, (void *)DTMAILCOMPOSEMENUID);
			}
		}
     } else
		pulldown = _w;
	
	
    
    // Loop through the cmdList, creating a menu 
    // entry for each command. 
    Widget *head_wl, *wl;
    Cardinal num_wl = 0;
    head_wl = wl = new Widget[ list->size() ];
    
    for ( i = 0; i < list->size(); i++)
    {

	if(!strcmp((*list)[i]->className(), "CmdList")) {
	    Widget pane = createPulldown(pulldown, 
		(CmdList*) (*list)[i], FALSE, XmMENU_BAR);
	    ((CmdList *)(*list)[i])->setPaneWidget(pane);
	    label_str = XmStringCreateLocalized(((CmdList *)(*list)[i])->getLabel());
	    XtVaSetValues(pane, 
			  XmNlabelString, label_str,
			  NULL);
	} else {
	    if ( !strcmp((*list)[i]->className(),"SeparatorCmd")) {
		*(wl++) = XtCreateWidget ( (*list)[i]->name(),
					  xmSeparatorWidgetClass,
					  pulldown,
					  NULL, 0);
	    } else if (!strcmp((*list)[i]->className(),"ToggleButtonCmd")) {
		CmdInterface *ci;
		ci = new ToggleButtonInterface(pulldown, (*list)[i] );
		*(wl++) = ci->baseWidget();
	    } else if(!strcmp((*list)[i]->className(),"LabelCmd")) {
		label_str = XmStringCreateLocalized((*list)[i]->getLabel());

		*(wl++) = XtVaCreateWidget ( (*list)[i]->name(),
					xmLabelWidgetClass,
					pulldown,
					XmNlabelString, label_str,
					NULL);
	    } else {
		CmdInterface *ci;
		ci  = new ButtonInterface ( pulldown, (*list)[i] );
		*(wl++) = ci->baseWidget();
	    }

	    num_wl++;
	}
    }

    XtManageChildren ( head_wl, num_wl );
    delete head_wl;

    return(pulldown);
}

Widget 
MenuBar::createPulldown ( 
	Widget parent, 
	CmdList *list, 
	Boolean help ,
	unsigned char type)
{
    int    i;
    Widget pulldown, cascade;
    Arg  args2[2];
    char * helpId;
    XmString label_str;

    if(type != XmMENU_POPUP)
	type = XmMENU_BAR;
    
    // Create a pulldown menu pane for this list of commands
    
    if( type == XmMENU_BAR) {
		pulldown = XmCreatePulldownMenu( parent,
				(char *) list->name(), NULL, 0 );
		label_str = XmStringCreateLocalized(list->getLabel());
		XtVaSetValues(pulldown,
			      XmNlabelString, label_str,
			      NULL);
		printHelpId("pulldown", pulldown);
		/* install callback */
		// XtAddCallback(pulldown, XmNhelpCallback, HelpCB, helpId);

		
		// Each entry in the menu bar must have a cascade button
		// from which the user can pull down the pane

		XtSetArg(args2[0], XmNsubMenuId, pulldown);
		cascade = XtCreateWidget ( list->name(), 
			       xmCascadeButtonWidgetClass,
			       parent, 
			       args2, 1);
		XtVaSetValues(cascade,
			      XmNlabelString, label_str,
			      NULL);
		if (help) {
			XtSetArg (args2[0], XmNmenuHelpWidget, cascade);
			XtSetValues (parent, args2, 1);
		}
		
		XtManageChild ( cascade );
		printHelpId("cascade", cascade);
		/* install callback */


		// Install callbacks for each of the 
		// pulldown menus so we can get
		// On Item help for them.

		helpId = XtName(cascade);
		if (helpId == "Mailbox") {
			XtAddCallback(cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILCONTAINERMENUID);
		} else if (helpId == "Edit") {
			XtAddCallback(cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILEDITMENUID);
		} else if (helpId == "Message") {
			XtAddCallback(cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILMESSAGEMENUID);
		} else if (helpId == "Attachments") {
			XtAddCallback(cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILATTACHMENUID);
		} else if (helpId == "View") {
			XtAddCallback(cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILVIEWMENUID);
		} else if (helpId == "Compose") {
			XtAddCallback(cascade, XmNhelpCallback, 
				HelpCB, (void *)DTMAILCOMPOSEMENUID);
		}
     } else
		pulldown = _w;
	
	
    
    // Loop through the cmdList, creating a menu 
    // entry for each command. 
    
    for ( i = 0; i < list->size(); i++)
    {

	if(!strcmp((*list)[i]->className(), "CmdList")) {
	    Widget pane = createPulldown(pulldown, 
			(CmdList*) (*list)[i], FALSE, XmMENU_BAR);
	    ((CmdList *)(*list)[i])->setPaneWidget(pane);
	    label_str = XmStringCreateLocalized((*list)[i]->getLabel());
	    XtVaSetValues(pane, 
			  XmNlabelString, label_str,
			  NULL);
	} else {
	    if ( !strcmp((*list)[i]->className(),"SeparatorCmd")) {
		XtCreateManagedWidget ( (*list)[i]->name(),
					  xmSeparatorWidgetClass,
					  pulldown,
					  NULL, 0);
	    } else if (!strcmp((*list)[i]->className(),"ToggleButtonCmd")) {
		CmdInterface *ci;
		ci = new ToggleButtonInterface(pulldown, (*list)[i]);
		ci->manage();
	    } else if(!strcmp((*list)[i]->className(),"LabelCmd")) {
		Widget	_i18n;
		Arg _args[1];
		XmString xms;
		
		_i18n = XtCreateManagedWidget(
					(*list)[i]->name(),
					xmLabelWidgetClass,
					pulldown,
					NULL, 0
					);

		xms = XmStringCreateLocalized((char*) (*list)[i]->getLabel());
		XtSetArg( _args[0], XmNlabelString, xms );
		XtSetValues( _i18n, _args, 1 );
		XmStringFree(xms);
	    } else {
		CmdInterface *ci;
		ci  = new ButtonInterface ( pulldown, (*list)[i] );
		ci->manage();
	    }
	}
    }
    return(pulldown);
}

// SR - Added to handle dynamic menus

Widget
MenuBar::addCommands(
    Widget pulldown,
    CmdList *new_list
)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    // 	return NULL;

    int    i;
    XtArgVal num_children;
    WidgetList children;
    Boolean haveNoSeparator;
    int newItemIndex, numPBUnmanaged, tmpPBUnmanaged;

    XtVaGetValues(pulldown,
	XmNnumChildren, &num_children,
	NULL);
    
    children = (WidgetList)XtMalloc(sizeof(Widget) * num_children);
    
    XtVaGetValues(pulldown,
	XmNchildren, &children,
	NULL);

    // Handle the creation or management of the Separator.

    haveNoSeparator = TRUE;
    numPBUnmanaged = 0;
    
    for (i=0; (i < num_children); i++) {
	Widget wid = (Widget) children[i];
	if (XtIsSubclass(wid, xmSeparatorWidgetClass)) {
	    XtManageChild(wid);
	    haveNoSeparator = FALSE;
	}
	else if (XtIsSubclass(wid, xmPushButtonWidgetClass)) {
	    if (!XtIsManaged(wid)) { // If widget is unmanaged
		numPBUnmanaged++;
	    }
	}
    }
    if (haveNoSeparator) {
	XtCreateManagedWidget ("Separator",
		xmSeparatorWidgetClass,
		pulldown,
		NULL, 0);
	haveNoSeparator = FALSE;
    }
    
    // Now handle the pushButton case

    newItemIndex = 0;
    tmpPBUnmanaged = 0;

    // Loop through the cmdList, creating a menu 
    // entry for each command. 
	
    for (newItemIndex = 0; newItemIndex < new_list->size(); newItemIndex++) {

	tmpPBUnmanaged = numPBUnmanaged;

	if (numPBUnmanaged > 0) { // If there exists unmanaged PBs
	    for (i = 0; 
		 (i < num_children) && 
		 (tmpPBUnmanaged == numPBUnmanaged); 
		 i++) {

		Widget wid = (Widget) children[i];
		
		if (XtIsSubclass(wid, xmPushButtonWidgetClass)) {
		    if (!XtIsManaged(wid)) { 
			// If widget is unmanaged
			// Set its label to be the newItemIndex widget's.
			// Manage it.
			// Bump up newItemIndex
			XtVaSetValues(wid,
			    XmNlabelString, XmStringCreateLocalized(
				(char *) (*new_list)[newItemIndex]->getLabel()),
			    NULL);
			XtManageChild(wid);
			numPBUnmanaged--;
		    }
		}
	    }
	}
	else {		// No unmanaged push buttons available
	    CmdInterface *ci;
	    ci  = new ButtonInterface ( 
		pulldown, (*new_list)[newItemIndex] 
	    );
	    ci->manage();
	}
    }

    return(pulldown);
}

void
MenuBar::removeOnlyCommands(
    Widget pulldown,
    CmdList *redundant_list
)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    //	return;

    int    i, j;
    XtArgVal num_children;
    WidgetList children;
    
    XtVaGetValues(pulldown,
	XmNnumChildren, &num_children,
	NULL);
    
    children = (WidgetList)XtMalloc(sizeof(Widget) * num_children);
    
    XtVaGetValues(pulldown,
	XmNchildren, &children,
	NULL);
     
    // Loop through widget list.  Destroy those widgets that map to those
    // in the redundant list.

    for (i=0; i<num_children; i++) {

	Widget wid = (Widget) children[i];

	if (XtIsSubclass(wid, xmPushButtonWidgetClass)) {

	    for (j=0; j<redundant_list->size(); j++) {
		char name[200];
		ButtonInterface::mapName((*redundant_list)[j]->name(), name);
		if (strcmp(XtName(wid), name) == 0) {
		    // The redundant item has been found.
		    XtUnmanageChild(wid);
		}
	    }
	}
    }
}
void
MenuBar::removeCommands(
    Widget pulldown,
    CmdList *redundant_list
)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    //	return;

    int    i, j;
    XtArgVal num_children;
    WidgetList children;
    
    XtVaGetValues(pulldown,
	XmNnumChildren, &num_children,
	NULL);
    
    children = (WidgetList)XtMalloc(sizeof(Widget) * num_children);
    
    XtVaGetValues(pulldown,
	XmNchildren, &children,
	NULL);
     
    // Loop through widget list.  Destroy those widgets that map to those
    // in the redundant list.

    for (i=0; i<num_children; i++) {

	Widget wid = (Widget) children[i];

	if (XtIsSubclass(wid, xmSeparatorWidgetClass)) {
	    XtUnmanageChild(wid);
	}
	else if (XtIsSubclass(wid, xmPushButtonWidgetClass)) {

	    for (j=0; j<redundant_list->size(); j++) {
		XmString str=NULL;
		String label=NULL;
		XtVaGetValues(wid, XmNlabelString, &str, NULL);
		if (str == NULL) continue;
	        label = NULL;
        	label = (char *) _XmStringUngenerate(
					str, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
		XmStringFree(str);
		if (label == NULL) continue;
		if (strcmp(label, (*redundant_list)[j]->getLabel()) == 0) {
		    // The redundant item has been found.
		    XtUnmanageChild(wid);
		}
                XtFree(label);
	    }
	}
    }
}

void
MenuBar::addCommand(
    Widget pulldown,
    Cmd *cmd
)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    //	return;

    CmdInterface *ci;
    ci  = new ButtonInterface ( pulldown, cmd);
    ci->manage();
}

void
MenuBar::changeLabel(
    Widget pulldown,
    int	index,
    char * name
)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    // 	return;

    int managed_widgets, i;
    XtArgVal num_children;
    WidgetList children;
    XmString label;
    Widget wid;

    XtVaGetValues(pulldown,
	XmNnumChildren, &num_children,
	NULL);

    children = (WidgetList)XtMalloc(sizeof(Widget) * num_children);

    XtVaGetValues(pulldown,
	XmNchildren, &children,
	NULL);
    
    // Some widgets may be unmanaged, so find the real index
    for (managed_widgets=0, i=0; 
		managed_widgets <= index && i < num_children; i++) {
        wid = (Widget) children[i];
        if (XtIsManaged(wid)) managed_widgets++;
    }
    if (--i >= num_children) return;

    wid = (Widget) children[i];

    label = XmStringCreateLocalized(name);

    XtVaSetValues(wid,
	XmNlabelString, label,
	NULL);
    XmStringFree(label);
}

void
MenuBar::changeLabel(Widget pulldown,
		     const char * button_name,
		     const char * label)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    // 	return;

    // Locate the appropriate widget in the list.
    //
    XtArgVal num_children;
    char wid_name[200];
    XmString label_string = XmStringCreateLocalized((char *)label);

    ButtonInterface::mapName(button_name, wid_name);

    XtVaGetValues(pulldown,
		  XmNnumChildren, &num_children,
		  NULL);

    Widget * children = new Widget[num_children];
    XtVaGetValues(pulldown,
		  XmNchildren, &children,
		  NULL);

    for (int wid = 0; wid < num_children; wid++) {
	if (strcmp(XtName(children[wid]), wid_name) == 0) {
	    if (XtIsManaged(children[wid]))
	   	 XtVaSetValues(children[wid],
			  XmNlabelString, label_string,
			  NULL);
	}
    }

    XmStringFree(label_string);
}
    
void
MenuBar::rotateLabels(
    Widget pulldown,
    int startindex,
    int endindex
)
{
    // if(isValidMenuPane(pulldown) == FALSE)
    // 	return;

    int    num_managed_wids=0, i, j, num_to_change;
    XtArgVal num_children;
    WidgetList children;
    XmString label, endlabel;
    Widget prevwid, wid;

    if (startindex < 0 || endindex < 0)
	return;

    XtVaGetValues(pulldown,
	XmNnumChildren, &num_children,
	NULL);

    if (startindex >= num_children || endindex >= num_children)
	return;
    
    num_to_change = endindex - startindex;
    if (num_children < num_to_change || num_to_change == 0) 
	return;

    children = (WidgetList)XtMalloc(sizeof(Widget) * num_children);

    XtVaGetValues(pulldown,
	XmNchildren, &children,
	NULL);

    // Some of the widgets may be unmanaged: find the first managed widget
    for (i = startindex; i < num_children; i++) {
        if (XtIsManaged(children[i]))
		break;
	startindex++;
    }

    if (startindex == num_children) 
	return;

    // Find the real endindex
    endindex = startindex+1;
    while (endindex < num_children) {
        if (XtIsManaged(children[endindex])) 
		num_managed_wids++;
	if (num_managed_wids == num_to_change)
		// We have found the endindex at this point
		break;
	endindex++;
    } 
 
    if (endindex == num_children) 
	return;

    wid = (Widget) children[endindex];
    XtVaGetValues(wid,
	XmNlabelString, &label,
	NULL);
    endlabel = XmStringCopy(label);

    j = i = endindex; 
    while (i > startindex) {

	do {
		if (--j < startindex) break;
		prevwid = (Widget) children[j];
	} while (!XtIsManaged(prevwid));

	XtVaGetValues(prevwid,
		XmNlabelString, &label,
		NULL);

	XtVaSetValues(wid,
		XmNlabelString, label,
		NULL);

	i = j;

	wid = (Widget) children[i];
    }

    wid = (Widget) children[startindex];
    XtVaSetValues(wid,
	XmNlabelString, endlabel,
	NULL);
    
    XmStringFree(endlabel);
}

#ifdef DEAD_WOOD
Boolean
MenuBar::isValidMenuPane(Widget w)
{
	Boolean retval = FALSE;
	Widget parent = w;

	while(parent && parent != _w)
		parent = XtParent(parent);

	if(parent == _w)
		retval = TRUE;

	return retval;
}
#endif /* DEAD_WOOD */

void
MenuBar::removeCommand(
    Widget pulldown,
    int index
)
{
    //if(isValidMenuPane(pulldown) == FALSE)
    //   return;

    int   managed_widgets, i;
    XtArgVal num_children;
    WidgetList children;

    XtVaGetValues(pulldown,
        XmNnumChildren, &num_children,
        NULL);

    children = (WidgetList)XtMalloc(sizeof(Widget) * num_children);

    XtVaGetValues(pulldown,
        XmNchildren, &children,
        NULL);

    // Some widgets may be unmanaged, so find the real index
    for (managed_widgets=0, i=0;
                managed_widgets <= index && i < num_children; i++) {
        Widget wid = (Widget) children[i];
        if (XtIsManaged(wid)) managed_widgets++;
    }

    if (--i < num_children)
    	XtUnmanageChild(children[i]);
}
