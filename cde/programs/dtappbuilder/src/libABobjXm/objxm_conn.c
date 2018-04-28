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

/*
 *	$XConsortium: objxm_conn.c /main/4 1995/11/06 18:45:37 rswiston $
 *
 *	@(#)objxm_conn.c	1.23 04 May 1995	cde_app_builder/src/libABobjXm
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 *  objxm_conn.c - handles connections stuff
 *
 */
#include <assert.h>
#include <ab_private/objxm.h>
#include <Dt/SpinBox.h>
#include <Dt/ComboBox.h>

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static STRING	get_resource_for_set_value(ABObj compObj, ABObj obj);
static STRING	get_resource_for_set_text(ABObj compObj, ABObj obj);
static STRING	get_resource_for_set_label(ABObj compObj, ABObj obj);


/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

#define ObjWClassIs(obj, class_name) \
		(util_streq(obj_get_class_name(obj), class_name))

#define ObjWClassIsApplicationShell(obj) \
		(ObjWClassIs((obj), _applicationShell))

#define ObjWClassIsArrowButton(obj) \
		(ObjWClassIs((obj), _xmArrowButton))

#define	ObjWClassIsBulletinBoard(obj) \
		(ObjWClassIs((obj), _xmBulletinBoard))

#define	ObjWClassIsCascadeButton(obj) \
		(ObjWClassIs((obj), _xmCascadeButton))

#define	ObjWClassIsDialogShell(obj) \
		(ObjWClassIs((obj), _xmDialogShell))

#define ObjWClassIsDrawingArea(obj) \
		(ObjWClassIs((obj), _xmDrawingArea))

#define	ObjWClassIsDrawnButton(obj) \
		(ObjWClassIs((obj), _xmDrawnButton))

#define ObjWClassIsFileChooserShell(obj) \
		(ObjWClassIs((obj), _xmFileSelectionBox))

#define	ObjWClassIsForm(obj) \
		(ObjWClassIs((obj), _xmForm))

#define ObjWClassIsLabel(obj) \
		(ObjWClassIs((obj), _xmLabel))

#define ObjWClassIsList(obj) \
		(ObjWClassIs((obj), _xmList))

#define ObjWClassIsMainWindow(obj) \
		(ObjWClassIs((obj), _xmMainWindow))

#define	ObjWClassIsMenuButton(obj) \
		(ObjWClassIs((obj), _dtMenuButton))

#define	ObjWClassIsMenuShell(obj) \
		(ObjWClassIs((obj), _xmMenuShell))

#define	ObjWClassIsPanedWindow(obj) \
		(ObjWClassIs((obj), _xmPanedWindow))

#define	ObjWClassIsPrimitive(obj) \
		(ObjWClassIs((obj), _xmPrimitive))

#define	ObjWClassIsPushButton(obj) \
		(ObjWClassIs((obj), _xmPushButton))

#define	ObjWClassIsRowColumn(obj) \
		(ObjWClassIs((obj), _xmRowColumn))

#define	ObjWClassIsScrolledWindow(obj) \
		(ObjWClassIs((obj), _xmScrolledWindow))

#define	ObjWClassIsTextField(obj) \
		(ObjWClassIs((obj), _xmTextField))

#define	ObjWClassIsText(obj) \
		(ObjWClassIs((obj), _xmText))

#define	ObjWClassIsToggleButton(obj) \
		(ObjWClassIs((obj), _xmToggleButton))

#define ObjWClassIsBaseWinShell(obj) \
		(ObjWClassIsApplicationShell(obj))

#define ObjWClassIsScale(obj) \
		(ObjWClassIs((obj), _xmScale))

#define ObjWClassIsSpinBox(obj) \
		(ObjWClassIs((obj), _dtSpinBox))

/*
 * Complex checks
 */
#define ObjWClassIsShell(obj) \
	(   (ObjWClassIsBaseWinShell(obj)) \
	 || (ObjWClassIsDialogShell(obj)) \
	 || (ObjWClassIsFileChooserShell(obj)) \
	 || (ObjWClassIsMenuShell(obj)) \
	 )


STRING
objxm_get_resource_for_when(
    ABObj	obj, 
    AB_WHEN	when
)
{
    STRING    res= NULL;

    /********** GET "DEFAULT" RESOURCE NAME *********/

    switch (when)
    {
        case AB_WHEN_DESTROYED: res= XmNdestroyCallback;
        break;
    }


    /********** GET TYPE-SPECIFIC RESOURCE NAME *********/

    switch (obj_get_type(obj))
    {
    case AB_TYPE_BASE_WINDOW:
	switch (when)
	{
            case AB_WHEN_HIDDEN: res= XmNpopdownCallback;
            break;
            case AB_WHEN_SHOWN: res= XmNpopupCallback;
            break;
	}
    break;

    case AB_TYPE_BUTTON:
        switch (when)
        {
            case AB_WHEN_ACTIVATED: res= XmNactivateCallback; 
            break;
        }
    break;

    case AB_TYPE_CHOICE:
    break;

    case AB_TYPE_COMBO_BOX:
	if (when == AB_WHEN_ITEM_SELECTED)
	    res = DtNselectionCallback;
    break;

    case AB_TYPE_CONTAINER:
    break;

    case AB_TYPE_DIALOG:
        switch (when)
        {
            case AB_WHEN_HIDDEN: res= XmNpopdownCallback;
            break;
            case AB_WHEN_SHOWN: res= XmNpopupCallback;
            break;
        }
    break;

    case AB_TYPE_DRAWING_AREA:
        switch (when)
        {
            case AB_WHEN_REPAINT_NEEDED: res= XmNexposeCallback;
            break;
            case AB_WHEN_AFTER_RESIZED: res= XmNresizeCallback;
            break;
        }
    break;

    case AB_TYPE_FILE_CHOOSER:
	switch (when)
	{
	    case AB_WHEN_OK: res = XmNokCallback;
		break;
	    case AB_WHEN_CANCEL: res = XmNcancelCallback;
		break;
            case AB_WHEN_HIDDEN: res= XmNpopdownCallback;
                break;
            case AB_WHEN_SHOWN: res= XmNpopupCallback;
                break;
	}
    break;

    case AB_TYPE_ITEM:
	switch (obj_get_item_type(obj))
	{
	    case AB_ITEM_FOR_CHOICE:
		switch (when)
		{
		    /* For optionmenu items */
		    case AB_WHEN_ACTIVATED:
			if (ObjWClassIsPushButton(obj))
			{
			    res = XmNactivateCallback;
			}
			break;

		    /* For checkbox/radiobox items */
		    case AB_WHEN_TOGGLED:
			    res = XmNvalueChangedCallback;
		        break;
		}
	    break;
	    case AB_ITEM_FOR_LIST:
	    break;
            case AB_ITEM_FOR_MENUBAR:
/*
                switch (when)
                {
                    case AB_WHEN_BEFORE_POST_MENU: 
			res = XmNcascadingCallback;
                    break;
                }
*/
            break;
	    case AB_ITEM_FOR_MENU:
		switch (when)
		{
		    case AB_WHEN_ACTIVATED: 
			res = XmNactivateCallback;
		    break;
		}
	    break;
	}
    break;

    case AB_TYPE_LABEL:
    break;

    case AB_TYPE_LAYERS:
    break;

    case AB_TYPE_LIST:
	switch (when)
	{
	    case AB_WHEN_ITEM_SELECTED:
		switch (obj_get_selection_mode(obj))
		{
		    case AB_SELECT_SINGLE:
			res = XmNsingleSelectionCallback;
		 	break;
		    case AB_SELECT_BROWSE:
			res = XmNbrowseSelectionCallback;
		 	break;
		    case AB_SELECT_MULTIPLE:
			res = XmNmultipleSelectionCallback;
		 	break;
		    case AB_SELECT_BROWSE_MULTIPLE:
			res = XmNextendedSelectionCallback;
		 	break;
		}
		break;
	
	    case AB_WHEN_DOUBLE_CLICKED_ON:
		res = XmNdefaultActionCallback;
		break;
	}
    break;

    case AB_TYPE_MENU:
        switch (when)
        {
            case AB_WHEN_POPPED_UP: res= XmNpopupCallback;
            break;
            case AB_WHEN_POPPED_DOWN: res= XmNpopdownCallback;
            break;
        }
    break;

    case AB_TYPE_SEPARATOR:
    break;

    case AB_TYPE_SPIN_BOX:
	if (when == AB_WHEN_VALUE_CHANGED)
	    res = XmNvalueChangedCallback;
    break;

    case AB_TYPE_SCALE:
        switch (when)
        {
            case AB_WHEN_VALUE_CHANGED: res= XmNvalueChangedCallback;
            break;
	
	    case AB_WHEN_DRAGGED: res = XmNdragCallback;
	    break;
        }
    break;

    case AB_TYPE_TERM_PANE:
    break;

    case AB_TYPE_TEXT_FIELD:
    case AB_TYPE_TEXT_PANE:
        switch (when)
        {
            case AB_WHEN_BEFORE_TEXT_CHANGED: 
		res= XmNmodifyVerifyCallback;
            break;

            case AB_WHEN_TEXT_CHANGED: 
		res= XmNvalueChangedCallback;
            break;
        }
    break;
    } /* switch obj_get_type() */

    return res;
}


/*
 * The parameter 'obj' is the actual subobject (probably from
 * objxm_comp_get_target_for_builtin_action()) that the resource
 * will be set on.
 * 
 */
STRING
objxm_get_resource_for_builtin_action(
    ABObj		obj, 
    AB_BUILTIN_ACTION	builtin_action
)
{
    ABObj	compObj = obj_get_root(obj);
    STRING	resource = NULL;

    switch (builtin_action)
    {
    case AB_STDACT_DISABLE:
    case AB_STDACT_ENABLE:
	resource = XmNsensitive;
    break;

    case AB_STDACT_HIDE:
    case AB_STDACT_SHOW:
	/* This is not handled through a resource directly, but through
	 *  a convenience function. So, this case isn't really necessary, but
	 *  is here for completeness. 
	 */
	resource = XmNmappedWhenManaged;
    break;

    case AB_STDACT_SET_LABEL:
	resource = get_resource_for_set_label(compObj, obj);
    break;

    case AB_STDACT_SET_TEXT:
	resource = get_resource_for_set_text(compObj, obj);
    break;

    case AB_STDACT_SET_VALUE:
	resource = get_resource_for_set_value(compObj, obj);
    break;
    }

    return resource;
}


ABObj
objxm_comp_get_source_for_when(ABObj subObj, AB_WHEN when)
{
    ABObj	source = NULL;
    ABObj	compObj = obj_get_root(subObj);

    switch (when)
    {
	case AB_WHEN_POPPED_UP:
	case AB_WHEN_POPPED_DOWN:
	    /* If this object is an optionmenu or a menu 
	     * we need to get a handle for the pulldown 
	     * menu, so that we can add a callback to 
	     * its menuShell.
	     */
/*
	    if (obj_is_option_menu(compObj))
	    {
		source = objxm_comp_get_subobj(compObj, AB_CFG_PARENT_OBJ);
	    }
	    else if (obj_is_menu_ref(subObj))
	    {
		source = subObj;
	    }
*/
	    if (obj_is_menu_ref(subObj))
            {
                source = subObj;
            }
	    break;
	 	
	default:
            source = objxm_comp_get_subobj(compObj, AB_CFG_OBJECT_OBJ);
    }

    return source;
}


ABObj
objxm_comp_get_target_for_builtin_action(ABObj subObj, AB_BUILTIN_ACTION action)
{
    ABObj	compObj = obj_get_root(subObj);
    ABObj	actualTarget = NULL;
    BOOL	useObjectObj = FALSE;

    /*
     * Adjust the actual "to" object
     */
    switch (action)
    {
    case AB_STDACT_ENABLE:
    case AB_STDACT_DISABLE:
	actualTarget = compObj;
    break;

    case AB_STDACT_HIDE:
    case AB_STDACT_SHOW:
	if (obj_is_popup_win(compObj))
	{
	    actualTarget = objxm_comp_get_subobj(compObj, AB_CFG_WINDOW_BB_OBJ);
	}
	else if (obj_is_base_win(compObj))
	{
	    actualTarget = objxm_comp_get_subobj(compObj, AB_CFG_OBJECT_OBJ);
	}
	else if (obj_is_file_chooser(compObj))
	{
	    actualTarget = objxm_comp_get_subobj(compObj, AB_CFG_OBJECT_OBJ);
	}
	else
	{
	    actualTarget = compObj;
	}
	break;

    case AB_STDACT_SET_LABEL:
	actualTarget = objxm_comp_get_subobj(compObj, AB_CFG_LABEL_OBJ);
    break;

    default:
	useObjectObj = TRUE;
    break;
    }

    if (actualTarget == NULL)
    {
	if (useObjectObj)
        {
	    actualTarget = objxm_comp_get_subobj(compObj, AB_CFG_OBJECT_OBJ);
        }
    }

    return actualTarget;
}


/*
 * the set-label connections we support:
 *
 *	button
 *	choice
 *	combobox
 *	label
 *	scale
 *	spinbox
 *	textfield
 *	item (choice, menu, menubar)
 */
static STRING
get_resource_for_set_label(ABObj compObj, ABObj obj)
{
    STRING	resource = NULL;

	assert(!ObjWClassIsArrowButton(obj));	/* not supported */
	if (ObjWClassIsShell(obj))
	{
	    resource = XmNtitleString;
	}
	else if (   ObjWClassIsCascadeButton(obj)
		 || ObjWClassIsDrawnButton(obj)
	         || ObjWClassIsMenuButton(obj)
		 || ObjWClassIsPushButton(obj)
		 || ObjWClassIsToggleButton(obj)
	         || ObjWClassIsLabel(obj) )
	{
	    if (obj_get_label_type(compObj) == AB_LABEL_GLYPH)
	    {
		resource = XmNlabelPixmap;
	    }
	    else
	    {
		resource = XmNlabelString;
	    }
	}
    return resource;
}


static STRING
get_resource_for_set_text(ABObj compObj, ABObj obj)
{
    STRING	resource = NULL;
    compObj = compObj;	/* avoid warning */

	if (ObjWClassIsSpinBox(obj))
	{
	    resource = DtNposition;
	}
	else
	{
	    resource = XmNvalue;
	}

    return resource;
}


static STRING
get_resource_for_set_value(ABObj compObj, ABObj obj)
{
    STRING	resource = NULL;
    compObj = compObj;	/* avoid warning */

	if (ObjWClassIsScale(obj))
	    resource = XmNvalue;
	else if (ObjWClassIsSpinBox(obj))
	    resource = DtNposition;

    return resource;
}

