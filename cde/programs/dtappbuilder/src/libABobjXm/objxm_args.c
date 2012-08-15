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

/*
 *    $XConsortium: objxm_args.c /main/5 1996/10/02 15:33:46 drk $
 *
 *	@(#)objxm_args.c	1.99 28 Apr 1995	
 *
 *     RESTRICTED CONFIDENTIAL INFORMATION:
 *    
 *    The information in this document is subject to special
 *    restrictions in a confidential disclosure agreement between
 *    HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *    document outside HP, IBM, Sun, USL, SCO, or Univel without
 *    Sun's specific written approval.  This document and all copies
 *    and derivative works thereof must be returned or destroyed at
 *    Sun's request.
 *
 *    Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 ***********************************************************************
 * objxm_args.c - routines to manage Motif Resource ArgLists
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>
#include <Dt/SpinBox.h>
#include <Dt/ComboBox.h>
#include <ab_private/trav.h>
#include "objxmP.h"

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
*************************************************************************/

int 		objxmP_set_attachment_arg(
    		    ABObj	     obj,
    	 	    OBJXM_CONFIG_TYPE   ctype,
    		    ABObj	     parent_obj,
    		    AB_COMPASS_POINT dir
    		);

/*************************************************************************
**                                                                      **
**       Public Function Definitions                                    **
**                                                                      **
*************************************************************************/ 

/*
 * Create Resource arglists for all objects in a tree
 * starting at root
 */
int
objxm_tree_set_ui_args(
    ABObj root, 
    OBJXM_CONFIG_TYPE ctype, 
    BOOL replace
)
{
    AB_TRAVERSAL    trav;
    ABObj        obj;

    /*
     * We only need to call set_args on salient objects,
     * so we're going to skip the others to improve speed
     */
    for (trav_open(&trav, root, AB_TRAV_SALIENT);
        (obj= trav_next(&trav)) != NULL; )
    {
	if (!obj_has_flag(obj, CreateAttrsFlag))
            objxm_comp_set_ui_args(obj, ctype, replace);
    }
    trav_close(&trav);
    return 0;
}

/*
 * Remove Resource arglists for all objects in a tree
 * starting at root
 */
int
objxm_tree_remove_ui_args(
    ABObj root, 
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_TRAVERSAL    trav;
    ABObj        obj;

    /*
     * We only need to call set_args on salient objects,
     * so we're going to skip the others to improve speed
     */
    for (trav_open(&trav, root, AB_TRAV_SALIENT);
        (obj= trav_next(&trav)) != NULL; )
    {
        objxm_obj_remove_all_ui_args(obj, ctype);
	obj_clear_flag(obj, CreateAttrsFlag);
    }
    trav_close(&trav);
    return 0;
}

/*
 * Create Resource arglists for a Composite Object hierarchy
 *
 * Assumes: obj is a composite root
 */
int
objxm_comp_set_ui_args(
    ABObj 	obj, 
    OBJXM_CONFIG_TYPE ctype,
    BOOL 	replace
)
{
    ABObj	xy_obj = NULL, /* position object */
	        s_obj = NULL; /* size object */
    assert(obj_is_root(obj));

    /* projects, modules and composite-subobjs not allowed */
    if ((!obj_is_ui(obj)) || obj_is_project(obj) || obj_is_module(obj) ||
	  obj_is_message(obj) || obj_is_sub(obj))
	return 0;

    util_dprintf(3,
	"Building Resource List for %s\n", util_strsafe(obj_get_name(obj)));

    xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
    s_obj  = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

    /* Set resources common to all object types 
     */
    if (obj->x >= 0)
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNx, obj->x);

    if (obj->y >= 0)
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNy, obj->y); 

    if (obj->width >= 0 && s_obj != NULL)
        objxm_obj_set_ui_arg(s_obj, AB_ARG_INT, XmNwidth, (Dimension)obj->width);

    if (obj->height >= 0)
        objxm_obj_set_ui_arg(s_obj, AB_ARG_INT, XmNheight, (Dimension)obj->height); 

    /* If widget is already instantiated, set colors now, else wait until
     * the post-instantiate phase so that the correct widget-id gets associated
     * with the allocated colors in the Xt resource cache.
     */
    if (objxm_get_widget(obj) != NULL)
    	objxm_comp_set_color_args(obj, ctype, ObjxmBackground | ObjxmForeground);

    /* Only Set XmNsensitive if it is False */
    /*
     * CMVC#6399 - XmNsensitive is getting set on the menu, rather than
     * on the insensitive item. The position config obj for the item is
     * considered to be the parent of the item, which does not work in
     * this case. In general, I think the position config obj should not 
     * be the parent, but the item, itself.	 -dunn 3/3/95
     */
    if (ctype == OBJXM_CONFIG_CODEGEN)
    {
	if (!obj_is_initially_active(obj))
	{
	    objxm_obj_set_ui_arg(obj, AB_ARG_BOOLEAN, XmNsensitive, False);
	}
    }

    /* Call method which sets resources specific to the object-type 
     */
    objxmP_comp_set_type_args(obj, ctype);

    /* Code Generation also requires that Attachment Args and
     * Post-instantiation arg  are generated on the first pass
     */
    if (ctype == OBJXM_CONFIG_CODEGEN)
    {
	objxm_comp_set_attachment_args(obj, ctype);
        objxmP_comp_type_post_instantiate(obj, ctype);
	obj_comp_clear_flag(obj, XmCfgForBuildFlag);
	obj_comp_set_flag(obj, XmCfgForCodeFlag);
    }
    else
    {
	obj_comp_clear_flag(obj, XmCfgForCodeFlag);
	obj_comp_set_flag(obj, XmCfgForBuildFlag);
    }
    obj_comp_set_flag(obj, CreateAttrsFlag);

    return 0;
}

int
objxm_tree_set_post_inst_args(
    ABObj 	root,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_TRAVERSAL    trav;
    ABObj        obj;

    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj= trav_next(&trav)) != NULL; )
    {
        objxm_obj_set_post_inst_args(obj, ctype);
    }
    trav_close(&trav);
    return 0;
}

int
objxm_obj_set_post_inst_args(
    ABObj		obj,
    OBJXM_CONFIG_TYPE	ctype
)
{
    if (obj_is_salient(obj))
    {
        objxmP_comp_type_post_instantiate(obj, ctype);

	/* Want to ensure allocated color is associated with correct widget-id
	 * in Xt string-to-pixel converter cache, so need to set color resources
	 * AFTER widget is instantiated.
	 */
        objxm_comp_set_color_args(obj, ctype, ObjxmBackground | ObjxmForeground);
    }
 
    if (obj->attachments != NULL)
        objxm_obj_set_attachment_args(obj, ctype);

    return 0;
}

Arg*
objxm_get_arg(
    ArgList     arglist,
    char        *name
)
{
    register int        num_args;
    register int        i;
    Arg                 *argptr = NULL;

    num_args = objxm_get_num_args(arglist);

    for (i = 0; i < num_args; i++)
    {
        if (strcmp(arglist[i].name, name) == 0)
        {
            argptr = &(arglist[i]);
            break;
        }
    }    
    return argptr;

}

int
objxm_set_literal_arg(
    OBJXM_CONFIG_TYPE ctype,
    ArgList	      *p_arglist,
    String	      name,
    unsigned char     literal 
)
{
    XtArgVal 	enum_value;

    enum_value = (ctype == OBJXM_CONFIG_CODEGEN?
		  (XtArgVal)objxm_get_enum_strdef(name, literal) :
		  (XtArgVal)literal);

    objxm_set_arg(p_arglist, AB_ARG_LITERAL, name, enum_value);

    return 0;
}

int
objxm_set_widget_arg(
    OBJXM_CONFIG_TYPE ctype,
    ArgList           *p_arglist,
    String            name,
    ABObj	      w_obj
)
{
    XtArgVal    w_value;

    /*
     * REMIND: free this ISTRING when the arg is destroyed!
     */
    w_value = (ctype == OBJXM_CONFIG_CODEGEN?
                  (XtArgVal)(istr_create(w_obj? obj_get_name(w_obj) : "NULL")):
                  (XtArgVal)(w_obj? w_obj->ui_handle : NULL));

    objxm_set_arg(p_arglist, AB_ARG_WIDGET, name, w_value);

    return 0;
}


int
objxm_set_arg(
    ArgList	*p_arglist,
    AB_ARG_TYPE argtype,
    String      name,
    XtArgVal	value
)
{
    Arg             *oldarg;
    Arg             newarg;

    if (argtype == -1)
        argtype = objxm_get_res_type(name);

    XtSetArg(newarg, name, (XtArgVal)value);

    if ((oldarg = objxm_get_arg(*p_arglist, name)) != NULL)
    {
        /* replace existing arg */
        *oldarg = newarg;
        return 0;
    }
    objxmP_merge_arg(p_arglist, &newarg);

    return 0;

}


/*
 *  sets the argument only if it doesn't already exist in the arglist
 *  returns 0 if the arg is set, negative if not
 */
int
objxm_set_arg_if_new(
    ArgList	*p_arglist,
    AB_ARG_TYPE type,
    STRING 	name, 
    XtArgVal 	value
)
{
    Arg	 *old_arg; 

    old_arg = objxm_get_arg(*p_arglist, name);

    if (old_arg != NULL)
    {
	return 1;
    }
    return objxm_set_arg(p_arglist, type, name, value);
}

void
objxm_remove_all_args(
    ArgList 		*p_arglist,
    OBJXM_CONFIG_TYPE	ctype
)
{
    int 	num_args;
    ArgList	args;
    AB_ARG_TYPE type;
    ISTRING	istr;
    int i;

    /* Free any allocated XmStrings... */
    if (*p_arglist != NULL)
    {
	args     = *p_arglist;
	num_args = objxm_get_num_args(args);
	for (i = 0; i < num_args; i++)
	{
	    type = objxm_get_res_type(args[i].name);
	    if (type == AB_ARG_XMSTRING && args[i].value)
	    {
		util_dprintf(3, "%s: Freeing XmString\n", args[i].name);
		if (ctype == OBJXM_CONFIG_BUILD)
		    XmStringFree((XmString)args[i].value);
		else /* CONFIG_CODEGEN */
		{
		    istr = (ISTRING)args[i].value;
		    istr_destroy(istr);
		}
	    }
	}
	XtFree((char *)*p_arglist);
        *p_arglist = NULL;
    }
}

int
objxm_get_num_args(
   ArgList arglist 
)
{
    register ArgList args = arglist;
    register int     n = 0;

    for ( ; (args != (ArgList)NULL) && (args->name != NULL);
             args++, n++)
        ;

    return n;
}

/*
 * Remove the specified arg from the args and re-arrange 
 */
int
objxm_remove_arg(
    ArgList *p_arglist,
    STRING  name
)
{
    register int       i;
    register ArgList   args;
    register int       nargs;

    if (*p_arglist == NULL)
    {
        return 0;
    }
    nargs = objxm_get_num_args(*p_arglist);    
    if (nargs > 0)
    {
        args = *p_arglist;
        *p_arglist = (ArgList)NULL;
        for (i = 0; i < nargs ; i++)
        {
            if (args[i].name && (strcmp(args[i].name, name) == 0))
                continue;
            objxmP_merge_arg(p_arglist, &args[i]);

        }
/** LOA	(void)free((STRING )args); **/
    }
    return 0;
}



 /******************************************************************
 * Functions that set Resource args on an entire Composite Obj
 ******************************************************************/

/*
 * Set attachment Resources for a Composite Object hierarchy
 */
objxm_comp_set_attachment_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_TRAVERSAL        trav;
    static ABObj        comp_root = NULL;
    ABObj               comp_sub = NULL;
 
    objxm_obj_set_attachment_args(obj, ctype);
 
    if (obj->part_of == NULL) /* No SubObjs */
        return 0;
 
    if (comp_root == NULL) /* Set Root */
        comp_root = obj;
    
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
        (comp_sub= trav_next(&trav)) != NULL; )
    {
        if (comp_sub->part_of == comp_root)
            objxm_comp_set_attachment_args(comp_sub, ctype);
    }
    trav_close(&trav);
 
    if (obj == comp_root)
        comp_root = NULL; /* Reset */
 
    return 0;

}

int
objxm_comp_set_color_args(
    ABObj 	      obj,
    OBJXM_CONFIG_TYPE ctype,
    unsigned int      flag
)
{
    static ABObj 	rootObj = NULL;
    static XtArgVal	fgvalue;
    static XtArgVal	bgvalue;
    static BOOL		cgen_args;
    AB_TRAVERSAL 	trav;
    Widget		widget;
    Pixel		pixel;
    ABObj	 	subObj;
    int			status;
    char		name[256];
    int			iRet = 0;

    if (rootObj == NULL) /* First time */
    {
	rootObj = obj;
	cgen_args = (ctype == OBJXM_CONFIG_CODEGEN? TRUE : FALSE);
	if (!cgen_args)
	    widget = (objxm_get_widget(obj)? objxm_get_widget(obj) : ObjxmP_toplevel); 

        if (flag & ObjxmBackground) 
	{
	    if (util_strempty(obj_get_bg_color(rootObj)))
                flag &= ~ObjxmBackground;
	    else
	    {
	    	if (!cgen_args) 
	    	{
            	    status = objxm_name_to_pixel(widget, obj_get_bg_color(rootObj), &pixel);

		    if (status == ERR) /* Color couldn't be allocated! */
		    {
                    	obj_get_safe_name(rootObj, name, 256);
                    	util_printf_err(catgets(OBJXM_MESSAGE_CATD,
				OBJXM_MESSAGE_SET, 21,
				"WARNING: Object '%s'\nCould not allocate Background color \"%s\".\n\
Object's Background will default to white."),
			 	name, obj_get_bg_color(rootObj));
			pixel = WhitePixelOfScreen(XtScreen(ObjxmP_toplevel));
		    }
		    bgvalue = (XtArgVal)pixel;
	    	}
	    	else /* Generating Code */
		     bgvalue= (XtArgVal)istr_create(obj_get_bg_color(rootObj));
	    }
	}
    	if (flag & ObjxmForeground)
	{
	    if (util_strempty(obj_get_fg_color(obj)))
		flag &= ~ObjxmForeground;
	    else
	    {
	    	if (!cgen_args)
	    	{
            	    status = objxm_name_to_pixel(widget, obj_get_fg_color(rootObj), &pixel);

		    if (status == ERR) /* Color couldn't be allocated! */
		    {
                    	obj_get_safe_name(rootObj, name, 256);
                    	util_printf_err(catgets(OBJXM_MESSAGE_CATD,
                        	OBJXM_MESSAGE_SET, 22,
                                "WARNING: Object '%s'\nCould not allocate Foreground color \"%s\".\n\
Object's Foreground will default to black."),
                         	name, obj_get_fg_color(rootObj));
			pixel = BlackPixelOfScreen(XtScreen(ObjxmP_toplevel));
		    }
		    fgvalue = (XtArgVal)pixel;
	    	}
	    	else /* Generating Code */
		    fgvalue = (XtArgVal)istr_create(obj_get_fg_color(rootObj));
	    }
        }
	if (flag == ObjxmNone) /* No Colors to Set */
  	{
	    rootObj = NULL;
	    return iRet;
	}
    }

    /* If in Build-mode, set value to Pixel, else (CodeGen-mode)
     * set the value to the color-name
     */
    if (flag & ObjxmBackground && 
        /* Don't set background on Window Footer SubObjs */
	!obj_has_flag(obj, NoCodeGenFlag) &&
	!util_streq(obj_get_class_name(obj), _xmScrolledWindow))
    {
    	objxm_obj_set_ui_arg(obj, AB_ARG_PIXEL, XmNbackground, bgvalue);
	obj_set_flag(obj, AttrChangedFlag);
    }

    if (flag & ObjxmForeground &&
	/* Don't set foreground on Window Select SubObj */
        !(obj_is_label(obj) && obj_is_window(obj_get_root(obj)) && 
          obj == objxm_comp_get_subobj(obj, AB_CFG_SELECT_OBJ)) &&
	!(util_streq(obj_get_class_name(obj), _xmScrolledWindow)))
    {
    	objxm_obj_set_ui_arg(obj, AB_ARG_PIXEL, XmNforeground, fgvalue);
	obj_set_flag(obj, AttrChangedFlag);
    }
    /*
     * set args recursively for any "sub-objs" of this obj
     */
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
         (subObj = trav_next(&trav)) != NULL; )
    {
        if (subObj->part_of == rootObj)  
            objxm_comp_set_color_args(subObj, ctype, flag);
    }
    trav_close(&trav);

    /* If we've recursed back up to the root obj, then we're done.
     */
    if (obj == rootObj)
        rootObj = NULL;

    return iRet;
}

int
objxm_comp_set_default_button_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE	ctype
)
{
    AB_TRAVERSAL	trav;
    ABObj   		bbobj, defaultb, child, button;
    ABObj   		button_panel;
    int			thickness = 1;

    defaultb = obj_get_default_act_button(obj);

    if (defaultb == NULL) /* No Default Button specified */
    {
	thickness = 0;
	if (ctype == OBJXM_CONFIG_CODEGEN)
	    return 0; /* don't bother setting it */
    }
    bbobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_BB_OBJ);
 
    objxm_obj_set_widget_ui_arg(bbobj, ctype, XmNdefaultButton, defaultb);
    obj_set_flag(bbobj, AttrChangedFlag);

    button_panel = objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL);

    /* If Dialog has a Button Panel, set all it's button DefaultShadowThickness */
    if (button_panel != NULL)
    {
    	for (trav_open(&trav, button_panel, AB_TRAV_SALIENT_UI);
        	(child= trav_next(&trav)) != NULL; )
    	{
	    if (obj_is_button(child))
	    {
		button = objxm_comp_get_subobj(child, AB_CFG_OBJECT_OBJ);
		objxm_obj_set_ui_arg(button, AB_ARG_INT,
			XmNdefaultButtonShadowThickness, thickness);
		obj_set_flag(button, AttrChangedFlag);
	    }
    	}
        trav_close(&trav);
    }
    return 0;
}

int
objxm_comp_set_label_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	subObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
    Pixmap	label_pm;
    STRING	label;
    int		status = OK;
    BOOL	cgen_args;

    if (subObj == NULL)
	return 0;

    cgen_args = (ctype == OBJXM_CONFIG_CODEGEN? TRUE : FALSE);
    label = obj_get_label(obj);

    switch (obj->label_type)
    {
        case AB_LABEL_STRING:
	    if (obj_is_window(obj))
		objxm_obj_set_ui_arg(subObj, AB_ARG_STRING, XmNtitle, label);
	    else
	    {
		XtArgVal	valstr;

		objxm_obj_set_literal_ui_arg(subObj, ctype, XmNlabelType, XmSTRING);

		/* If in Build-mode, convert label to XmString, else (CodeGen-mode)
		 * set value to label
		 */
	 	if (cgen_args)
		    valstr = (XtArgVal)istr_create(label);
		else
		{
		    /* NOTE:This XmString is freed in objxm_remove_all_args() */
		    util_dprintf(3, "%s : allocating XmString\n", XmNlabelString);
		    valstr = (XtArgVal)objxm_str_to_xmstr(ObjxmP_toplevel, label);
		}
                objxm_obj_set_ui_arg(subObj, AB_ARG_XMSTRING, XmNlabelString, valstr);
	    }
            break;
        case AB_LABEL_GLYPH:
	    if (!util_strempty(label))
	    {
	    	if (!cgen_args)
	    	{
		    status = objxm_filebase_to_pixmap(ObjxmP_toplevel, label, &label_pm);
		    if (status != OK)
		    	util_printf_err(objxm_pixmap_conversion_error_msg(obj, label, status));
	    	}
	    	if (status == OK)
	    	{
		    objxm_obj_set_literal_ui_arg(subObj, ctype, XmNlabelType, XmPIXMAP);
                    objxm_obj_set_ui_arg(subObj, AB_ARG_PIXMAP, XmNlabelPixmap,
                                cgen_args? (XtArgVal)istr_create(obj_get_label(obj)):
				(XtArgVal)label_pm);
	    	}
	    }
	    break;
        case AB_LABEL_ARROW_DOWN:
            obj_set_class_name(subObj, _xmArrowButton);
            objxm_obj_set_literal_ui_arg(subObj, ctype, XmNarrowDirection, XmARROW_DOWN);
            break;
        case AB_LABEL_ARROW_UP:
            obj_set_class_name(subObj, _xmArrowButton);
            objxm_obj_set_literal_ui_arg(subObj, ctype, XmNarrowDirection, XmARROW_UP);
            break;
        case AB_LABEL_ARROW_RIGHT:
            obj_set_class_name(subObj, _xmArrowButton);
            objxm_obj_set_literal_ui_arg(subObj, ctype, XmNarrowDirection, XmARROW_RIGHT);
            break;
        case AB_LABEL_ARROW_LEFT:
            obj_set_class_name(subObj, _xmArrowButton);
            objxm_obj_set_literal_ui_arg(subObj, ctype, XmNarrowDirection,XmARROW_LEFT);
            break;
	case AB_LABEL_SEPARATOR:
	    obj_set_class_name(subObj, _xmSeparator);
	    objxm_obj_set_line_style_arg(subObj, ctype);
	    break;

    }
    obj_set_flag(subObj, AttrChangedFlag);
    return 0;
}

int
objxm_comp_set_lbl_align_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj 		subObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
    unsigned char   	value;

    switch(obj_get_label_alignment(obj))
    {
        case AB_ALIGN_RIGHT:
            value = XmALIGNMENT_END;
            break;
        case AB_ALIGN_LEFT:
            value = XmALIGNMENT_BEGINNING;
            break;
        case AB_ALIGN_CENTER:
            value = XmALIGNMENT_CENTER;
	    break;
	default:
	    return -1;
    }
    objxm_obj_set_literal_ui_arg(subObj, ctype, XmNalignment, value);
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

int
objxm_comp_set_lbl_pos_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	     subObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);

    if (!obj_has_label(obj))
	return 0;

    switch (obj_get_label_position(obj)) 
    { 
        case AB_CP_WEST: 
	    objxm_obj_set_literal_ui_arg(subObj, ctype, 
			XmNorientation, XmHORIZONTAL);
            objxm_obj_set_literal_ui_arg(subObj, ctype, 
			XmNentryAlignment, XmALIGNMENT_END);
            break; 
        case AB_CP_NORTH: 
	    objxm_obj_set_literal_ui_arg(subObj, ctype, 
			XmNorientation, XmVERTICAL);
	    objxm_obj_set_literal_ui_arg(subObj, ctype, 
			XmNentryAlignment, XmALIGNMENT_BEGINNING);
            break; 
	default:
	    return -1;
    }
    objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNmarginHeight, 0); 
    objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNmarginWidth, 0); 
    objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNspacing, 0);
    objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNadjustLast, True);
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

int
objxm_comp_set_icon_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj       shobj; /* Shell SubObj */
    STRING	icon, mask, icon_label;
    Pixmap	icon_pm, mask_pm;
    int		status = OK;

    if (!obj_is_base_win(obj))
	return -1;

    shobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (ctype == OBJXM_CONFIG_CODEGEN)
	objxm_obj_set_literal_ui_arg(shobj, ctype, XmNinitialState,
		obj_is_initially_iconic(obj)? IconicState : NormalState);

    /* Setup Icon Pixmap */
    if ((icon = obj_get_icon(obj)) != NULL)
    {
        if (ctype != OBJXM_CONFIG_CODEGEN)
        {
            status = objxm_filebase_to_pixmap(ObjxmP_toplevel, icon, &icon_pm);
            if (status != OK)
                util_printf_err(objxm_pixmap_conversion_error_msg(obj, icon, status));
        }
        if (status == OK)
            objxm_obj_set_ui_arg(shobj, AB_ARG_PIXMAP, XmNiconPixmap,
                ctype == OBJXM_CONFIG_CODEGEN? (XtArgVal)istr_create(icon) : icon_pm);
    }
    else if (ctype == OBJXM_CONFIG_BUILD)
	objxm_obj_set_ui_arg(shobj, AB_ARG_PIXMAP, XmNiconPixmap, NULL);

    if ((mask = obj_get_icon_mask(obj)) != NULL)
    {
        if (ctype != OBJXM_CONFIG_CODEGEN)
        {
            status = objxm_filebase_to_pixmap(ObjxmP_toplevel, mask, &mask_pm);
            if (status != OK)
                util_printf_err(objxm_pixmap_conversion_error_msg(obj, mask, status));
        }
        if (status == OK)
            objxm_obj_set_ui_arg(shobj, AB_ARG_PIXMAP, XmNiconMask,
                ctype == OBJXM_CONFIG_CODEGEN? (XtArgVal)istr_create(mask) : mask_pm);
    }
    else if (ctype == OBJXM_CONFIG_BUILD) 
        objxm_obj_set_ui_arg(shobj, AB_ARG_PIXMAP, XmNiconMask, NULL); 

    /* Setup Icon Label */
    if ((icon_label = obj_get_icon_label(obj)) != NULL)
        objxm_obj_set_ui_arg(shobj, AB_ARG_STRING, XmNiconName, icon_label);

    obj_set_flag(shobj, AttrChangedFlag);

    return 0;
}

int
objxm_comp_set_read_only_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj       subObj;
    BOOL        read_only;

    read_only = obj_get_read_only(obj);
    subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    switch(obj->type)
    {
	case AB_TYPE_COMBO_BOX:
            objxm_obj_set_literal_ui_arg(subObj, ctype, DtNcomboBoxType,
                read_only? DtDROP_DOWN_LIST : DtDROP_DOWN_COMBO_BOX);
	    break;
	case AB_TYPE_SCALE:
            objxm_obj_set_ui_arg(subObj, AB_ARG_INT, "slidingMode", 
		read_only? 1 : 0);
	    break;
	case AB_TYPE_TEXT_PANE:
	case AB_TYPE_TEXT_FIELD:
            objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNeditable,
		read_only? False : True);
	    objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNcursorPositionVisible,
		read_only? False : True);
	    break;

	default:
	    return -1;
    }
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

int
objxm_comp_set_border_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	   b_obj;
    int		   bstyle;

    if ((b_obj = objxm_comp_get_subobj(obj, AB_CFG_BORDER_OBJ)) == NULL)
	return OK;

    switch(obj_get_border_frame(obj))
    {
	case AB_LINE_SHADOW_IN:
	    bstyle = XmSHADOW_IN;
	    break;
	case AB_LINE_SHADOW_OUT:
	    bstyle = XmSHADOW_OUT;
	    break;
        case AB_LINE_ETCHED_IN:
            bstyle = XmSHADOW_ETCHED_IN;
            break;
        case AB_LINE_ETCHED_OUT:
            bstyle = XmSHADOW_ETCHED_OUT; 
            break; 
	case AB_LINE_NONE:
	default:
	    return -1;
    }

    objxm_obj_set_literal_ui_arg(b_obj, ctype, XmNshadowType, bstyle);
    obj_set_flag(b_obj, AttrChangedFlag);

    return 0;

}

int
objxm_comp_set_msgbox_button_arg(
    ABObj       	obj,
    OBJXM_CONFIG_TYPE 	ctype,
    unsigned char	which_btn
)
{
    ABObj       msgbox_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    BOOL        cgen_args;
    Widget	msgbox, button;
    BOOL     	(*func)(ABObj obj)= NULL;

    msgbox = objxm_get_widget(msgbox_obj);
    switch(which_btn)
    {
	case XmDIALOG_OK_BUTTON:
	    func = obj_has_action1_button; 
	    break;
	case XmDIALOG_CANCEL_BUTTON:
	    func = obj_has_cancel_button; 
	    break;
	case XmDIALOG_HELP_BUTTON:
	    func = obj_has_help_button; 
	    break;
    }
    button = XmMessageBoxGetChild(msgbox, which_btn);
    cgen_args = (ctype == OBJXM_CONFIG_CODEGEN? TRUE : FALSE);

    if (func(obj))
    {
	if (!XtIsManaged(button))
	{
	    XtManageChild(button);
	}
    }
    else
    {
        /* Unmanage the button */
	XtUnmanageChild(button);
    }
    obj_set_flag(msgbox_obj, AttrChangedFlag);

    return OK;
}


 /***********************************************************
 * Functions that set Resource args on a SINGLE SubObj
 ***********************************************************/

int
objxm_obj_set_arrow_style_arg(
    ABObj               subObj,
    OBJXM_CONFIG_TYPE   ctype
)
{
    unsigned char       value;

    switch(obj_get_arrow_style(obj_get_root(subObj)))
    {
        case AB_ARROW_FLAT_BEGIN:
            value = DtARROWS_FLAT_BEGINNING;
            break;
        case AB_ARROW_FLAT_END:
            value = DtARROWS_FLAT_END;
            break;
        case AB_ARROW_BEGIN:
            value = DtARROWS_BEGINNING;
            break;
        case AB_ARROW_END:
            value = DtARROWS_END;
            break;
        case AB_ARROW_SPLIT:
            value = DtARROWS_SPLIT;
            break;
        default:
            return -1;
    }
    objxm_obj_set_literal_ui_arg(subObj, ctype, DtNarrowLayout, value);
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

int
objxm_obj_set_attachment_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj         parent_obj;

    if (obj == NULL)
        return -1;

    if (obj->attachments != NULL)
    {
        parent_obj = obj_get_parent(obj);

        objxmP_set_attachment_arg(obj, ctype, parent_obj, AB_CP_NORTH);
        objxmP_set_attachment_arg(obj, ctype, parent_obj, AB_CP_WEST);
        objxmP_set_attachment_arg(obj, ctype, parent_obj, AB_CP_EAST);
        objxmP_set_attachment_arg(obj, ctype, parent_obj, AB_CP_SOUTH);
    }

    return 0;

}

int
objxm_obj_set_direction_arg(
    ABObj               subObj,
    OBJXM_CONFIG_TYPE   ctype
)
{
    unsigned char       value;
 
    switch(obj_get_direction(obj_get_root(subObj)))
    {
        case AB_DIR_LEFT_TO_RIGHT:
            value = XmMAX_ON_RIGHT;
            break;
        case AB_DIR_RIGHT_TO_LEFT:
            value = XmMAX_ON_LEFT;
            break;
        case AB_DIR_TOP_TO_BOTTOM:
            value = XmMAX_ON_BOTTOM;
            break;
        case AB_DIR_BOTTOM_TO_TOP:
            value = XmMAX_ON_TOP;
            break;
        default:
            return -1;
    }
    objxm_obj_set_literal_ui_arg(subObj, ctype, XmNprocessingDirection, value);
    obj_set_flag(subObj, AttrChangedFlag);
 
    return 0;
}

int
objxm_obj_set_line_style_arg(
    ABObj	subObj,
    OBJXM_CONFIG_TYPE ctype
)
{
    unsigned char   value;

    value = objxm_linestyle_to_enum(obj_get_line_style(obj_get_root(subObj)));

    objxm_obj_set_literal_ui_arg(subObj, ctype, XmNseparatorType, value);
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

int
objxm_obj_set_orientation_arg(
    ABObj               subObj,
    OBJXM_CONFIG_TYPE   ctype
)
{
    unsigned char       value;

    switch (obj_get_orientation(obj_get_root(subObj)))
    {
        case AB_ORIENT_HORIZONTAL:
	    value = XmHORIZONTAL;
            break;
        case AB_ORIENT_VERTICAL:
	    value = XmVERTICAL;
            break;
	default:
	    return -1;
    }
    objxm_obj_set_literal_ui_arg(subObj, ctype, XmNorientation, value);
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

int
objxm_obj_set_selection_arg(
    ABObj               subObj,
    OBJXM_CONFIG_TYPE   ctype
)
{
    unsigned char       value;

    switch (obj_get_selection_mode(obj_get_root(subObj)))
    {
        case AB_SELECT_BROWSE:
            value = XmBROWSE_SELECT;
            break;
        case AB_SELECT_MULTIPLE:
            value = XmMULTIPLE_SELECT;
            break;
        case AB_SELECT_BROWSE_MULTIPLE:
            value = XmEXTENDED_SELECT;
            break;
        case AB_SELECT_SINGLE:
	    value = XmSINGLE_SELECT;
	    break;
    default:
        return -1;
    }
    objxm_obj_set_literal_ui_arg(subObj, ctype, XmNselectionPolicy, value);
    obj_set_flag(subObj, AttrChangedFlag);

    return 0;
}

STRING
objxm_obj_get_default_motif_class(
    ABObj obj
)
{
    STRING    cn= NULL;
    AB_BUTTON_TYPE btype;

    switch (obj->type)
    {
        case AB_TYPE_BASE_WINDOW:
            cn= _applicationShell;
            break;
        case AB_TYPE_BUTTON:
	    btype = (AB_BUTTON_TYPE)obj_get_subtype(obj); 
	    if (btype == AB_BUT_PUSH)
            	cn= _xmPushButton;
	    else if (btype == AB_BUT_DRAWN)
		cn= _xmDrawnButton;
	    else /* AB_BUT_MENU */
		cn= _dtMenuButton;
        break;
        case AB_TYPE_DRAWING_AREA:
            cn= _xmDrawingArea;
            break;
        case AB_TYPE_CONTAINER:
	    if (obj_get_container_type(obj) == AB_CONT_PANED)
		cn = _xmPanedWindow;
	    else
		cn= _xmBulletinBoard;
            break;
        case AB_TYPE_COMBO_BOX:
            cn= _dtComboBox;
            break;
        case AB_TYPE_FILE_CHOOSER:
            cn= _xmFileSelectionBox;
            break;
        case AB_TYPE_MESSAGE:
            cn= _xmMessageBox;
            break;
        case AB_TYPE_MENU:
	    if (obj_get_subtype(obj) == AB_MENU_PULLDOWN)
		cn = _xmPulldownMenu; /* Psuedo-class */
	    else
		cn = _xmPopupMenu;    /* Psuedo-class */
            break;
        case AB_TYPE_ITEM:
            switch (obj->info.item.type)
            {
            case AB_ITEM_FOR_MENU:
                if (obj_get_label(obj) == NULL)
                {
                    cn= _xmSeparator;
                }
                else
                {
                    cn= _xmCascadeButton;
                }
            break;

            case AB_ITEM_FOR_CHOICE:
                cn= _xmToggleButton;
            break;

	    case AB_ITEM_FOR_MENUBAR:
		cn= _xmCascadeButton;
	    break;

            case AB_ITEM_FOR_LIST:
                /* no class for list items (strings) */
            break;
            }
            break;
        case AB_TYPE_LABEL:
            cn= _xmLabel;
            break;
        case AB_TYPE_DIALOG:
            cn= _xmDialogShell;
            break;
        case AB_TYPE_LIST:
            cn= _xmScrolledList; /* Psuedo-class */
            break;
        case AB_TYPE_CHOICE:
            cn= _xmRowColumn;
            break;
        case AB_TYPE_SEPARATOR:
            cn= _xmSeparator;
            break;
        case AB_TYPE_SCALE:
            cn= _xmScale;
            break;
        case AB_TYPE_SPIN_BOX:
            cn= _dtSpinBox;
            break;
        case AB_TYPE_LAYERS:
            cn= NULL;
            break;
        case AB_TYPE_TERM_PANE:
            cn= _dtTerm;
            break;
        case AB_TYPE_TEXT_FIELD:
            cn= _xmTextField;
            if (obj_get_num_rows(obj) > 1)
            {
                cn= _xmText;
            }
            break;
        case AB_TYPE_TEXT_PANE:
            cn= _xmText;
            break;
        default:
            break;
    }
    return cn;
}

/*************************************************************************
 * Utility Functions to dump out Resource Arg Values                     *
 *************************************************************************/
int
objxm_dump_arglist(
    ABObj	obj,
    ArgList	args,
    int		num_args
)
{
    char	name[256];
    util_dprintf(0,"Resources for: %s\n", obj_get_safe_name(obj, name, 256));
    objxm_dump_arglist_indented(obj, args, num_args, 4);

    util_dprintf(0,"\n");
    return 0;
}


int
objxm_dump_arglist_indented(
    ABObj	obj,
    ArgList	args,
    int		num_args,
    int		spaces
)
{
    int		space_count= 0;
    int		i= 0;
    String	valstr= NULL;
    AB_ARG_TYPE	arg_type= AB_ARG_UNDEF;
    Arg		*arg= NULL;
    BOOL	cgen_args = FALSE;

    if (obj_has_flag(obj, XmCfgForCodeFlag))
	cgen_args = TRUE;

    for (i = 0; i < num_args; i++)
    {
	arg= &(args[i]);
	for (space_count= 0; space_count < spaces; ++space_count)
	{
	    util_dprintf(0, " ");
	}

	util_dprintf(0,"XmN%s = ", args[i].name);
	arg_type = objxm_get_res_type(args[i].name);
	switch(arg_type)
	{
	    case AB_ARG_INT:
		util_dprintf(0,"%d ", (int)args[i].value);
		break;
	    case AB_ARG_STRING:
		util_dprintf(0,"%s ", (char*)args[i].value);
		break;
            case AB_ARG_PIXEL:
		if (cgen_args)
		{
		    util_dprintf(0,"%s ", 
			istr_string((ISTRING)(args[i].value)));
		}
		else
		{
		    util_dprintf(0,"(Pixel)%ld ", (long)args[i].value);
		}
		break;
	    case AB_ARG_XMSTRING:
		if (cgen_args)
		    valstr = (String)istr_string((ISTRING)(args[i].value));
		else
		    valstr = arg->value?
			(String)objxm_xmstr_to_str((XmString)arg->value): "NULL";
		util_dprintf(0,"%s ", util_strsafe(valstr));
		break;
	    case AB_ARG_BOOLEAN:
		util_dprintf(0,"%s ", args[i].value? "TRUE" :"FALSE");
		break;
	    case AB_ARG_LITERAL:
		if (cgen_args)
		    valstr = (String)istr_string((ISTRING)(args[i].value));
		else
		    valstr = istr_string(objxm_get_enum_strdef(args[i].name,
					(unsigned char)args[i].value));
		util_dprintf(0,"%s ", util_strsafe(valstr));
		break;
	    case AB_ARG_WIDGET:
		if (cgen_args)
		{
		    util_dprintf(0,"%s ", args[i].value != 0?
		        istr_string((ISTRING)(args[i].value)) : "NULL");
		}
 		else
		{
		    util_dprintf(0,"%lx = %s ", 
			(Widget)args[i].value,
				args[i].value ? XtName((Widget)args[i].value) : "NULL");
		}
		break;
	    case AB_ARG_PIXMAP:
                if (cgen_args)
                {
                    util_dprintf(0,"(Pixmap File)%s ",
                        istr_string((ISTRING)(args[i].value)));
                }
                else
                {
                    util_dprintf(0,"(Pixmap)%ld ", (long)args[i].value);
                }
                break;
	    default:
		util_dprintf(0,"%d ", args[i].value);
		break;
	} /* switch arg_type */

	util_dprintf(0,"%s\n",
	    objxm_res_value_is_default(args[i].name, 
		objxm_get_class_ptr(obj_get_class_name(obj)), args[i].value)? "(Default)":"");
    }

    return 0;
}

#define	A_TOP	 0
#define A_BOTTOM 1
#define A_LEFT   2
#define A_RIGHT  3

int
objxm_dump_widget_geometry(
    Widget	w
)
{
    Position	x,y;
    Dimension   width, height;
    unsigned char attach[4];
    Widget	widget[4];
    int		position[4];
    int		offset[4];
    int		i;
    BOOL	parent_is_form = FALSE;
    char	*attach_str[] = {"XmNtopAttachment   ", "XmNbottomAttachment",
				 "XmNleftAttachment  ", "XmNrightAttachment "};
    char	*widget_str[] = {"XmNtopWidget",        "XmNbottomWidget",
				 "XmNleftWidget",	"XmNrightWidget"};
    char	*pos_str[]    = {"XmNtopPosition",      "XmNbottomPosition",
				 "XmNleftPosition",     "XmNrightPosition"};
    char	*offset_str[] = {"XmNtopOffset",        "XmNbottomOffset",
				 "XmNleftOffset",       "XmNrightOffset"};

    if (w == NULL)
	return -1;

    parent_is_form = XtIsSubclass(XtParent(w), xmFormWidgetClass);

    if (parent_is_form)
    {
        XtVaGetValues(w,
		XmNx,			&x,
		XmNy,			&y,
		XmNwidth,		&width,
		XmNheight,		&height,
		XmNtopAttachment, 	&(attach[A_TOP]),
		XmNbottomAttachment, 	&(attach[A_BOTTOM]),
		XmNleftAttachment, 	&(attach[A_LEFT]),
		XmNrightAttachment, 	&(attach[A_RIGHT]),
		XmNtopWidget,		&(widget[A_TOP]),
		XmNbottomWidget,	&(widget[A_BOTTOM]),
		XmNleftWidget,		&(widget[A_LEFT]),
		XmNrightWidget,		&(widget[A_RIGHT]),
                XmNtopPosition,         &(position[A_TOP]),
                XmNbottomPosition,      &(position[A_BOTTOM]),
                XmNleftPosition,        &(position[A_LEFT]),
                XmNrightPosition,       &(position[A_RIGHT]),
		XmNtopOffset,		&(offset[A_TOP]),
		XmNbottomOffset,	&(offset[A_BOTTOM]),
		XmNleftOffset,		&(offset[A_LEFT]),
		XmNrightOffset,		&(offset[A_RIGHT]),
		NULL);

    }
    else
    {
        XtVaGetValues(w,
		XmNx,			&x,
		XmNy,			&y,
		XmNwidth,		&width,
		XmNheight,		&height,
		NULL);
    }

    util_dprintf(0,"Widget %s is %s\n", 
		XtName(w),
		XtIsRealized(w) ? "realized" : "not realized");
    util_dprintf(0,"Widget %s is %s\n", 
		XtName(w),
		XtIsManaged(w) ? "managed" : "not managed");
    util_dprintf(0,"Geometry for %s -->\n", XtName(w));
    util_dprintf(0,"    x,y                 = %d,%d\n", x, y);
    util_dprintf(0,"    Width x Height      = %d x %d\n", width, height);

    if (parent_is_form)
    {
        for(i=0; i < 4; i++)
        {
	    util_dprintf(0,"    %s = ", attach_str[i]);
	    switch(attach[i])
	    {
	        case XmATTACH_NONE:
		    util_dprintf(0,"XmATTACH_NONE\n");
		    break;
	        case XmATTACH_FORM:
		    util_dprintf(0,"XmATTACH_FORM, Form=%s, %s=%d\n", 
			XtName(XtParent(w)), offset_str[i], offset[i]);
		    break;
	        case XmATTACH_WIDGET:
		    util_dprintf(0,"XmATTACH_WIDGET, %s=%s, %s=%d\n", 
			widget_str[i], XtName(widget[i]), 
			offset_str[i], offset[i]);
		    break;
	        case XmATTACH_POSITION:
		    util_dprintf(0,"XmATTACH_POSITION, %s=%d, %s=%d\n", 
			pos_str[i], position[i], 
			offset_str[i], offset[i]);
		    break;
	    }
        }
    }
    return 0;

}
#undef	A_TOP
#undef	A_BOTTOM
#undef	A_LEFT
#undef	A_RIGHT

/*************************************************************************
**                                                                      **
**       Private Function Definitions                                   **
**                                                                      **
*************************************************************************/ 
/* 
 * To this routine just pass in 'Arg *' and the arglist in the ABObj,
 * it will merge with existing args for obj and stick it back in the arglist 
 *
 */
int
objxmP_merge_arg(
    ArgList *p_arglist,
    Arg     *arg
)
{
    ArgList result;
    int     nargs= 0;

    if ((nargs = objxm_get_num_args(*p_arglist)) == 0)
    {            /* first time ever */
        result  = objxmP_merge_arglists(arg, 1, (ArgList)NULL, 0);
        *p_arglist = result;
    }
    else
    {   /* merge with existing args */
        result = objxmP_merge_arglists(arg, 1, *p_arglist, nargs);
        XtFree((char *)*p_arglist);
        *p_arglist = result;
    }
    return 0;
}


ArgList
objxmP_merge_arglists(
    ArgList args1, 
    int num_args1, 
    ArgList args2, 
    int num_args2
)
{
	ArgList result, args;
	Arg     nullarg = {NULL, 0};

	result = (ArgList)XtMalloc((unsigned) (num_args1 + num_args2 + 1) *
				sizeof(Arg));
	for (args = result; num_args1 != 0; num_args1--)
		*args++ = *args1++;
	for ( ; num_args2 != 0; num_args2--)
		*args++ = *args2++;

	/* ensure last arg is NULL */
	*args = nullarg;
	return result;
}

int
objxmP_set_attachment_arg(
    ABObj	     obj,
    OBJXM_CONFIG_TYPE   ctype,
    ABObj	     parent_obj,
    AB_COMPASS_POINT dir
)
{
    String	   att_side_resource;
    String	   att_widget_resource;
    String	   att_offset_resource;
    String	   att_pos_resource;
    AB_ATTACH_TYPE type;
    void	   *value;
    int		   offset;
    ABObj	   att_obj;

    switch(dir)
    {
	case AB_CP_NORTH:
	    att_side_resource   = XmNtopAttachment;
	    att_widget_resource = XmNtopWidget;
	    att_offset_resource = XmNtopOffset;
	    att_pos_resource    = XmNtopPosition;
	    break;
	case AB_CP_WEST:
	    att_side_resource   = XmNleftAttachment;
	    att_widget_resource = XmNleftWidget;
	    att_offset_resource = XmNleftOffset;
	    att_pos_resource    = XmNleftPosition;
	    break;
	case AB_CP_EAST:
	    att_side_resource   = XmNrightAttachment;
	    att_widget_resource = XmNrightWidget;
	    att_offset_resource = XmNrightOffset;
	    att_pos_resource    = XmNrightPosition;
	    break;
	case AB_CP_SOUTH:
	    att_side_resource   = XmNbottomAttachment;
	    att_widget_resource = XmNbottomWidget;
	    att_offset_resource = XmNbottomOffset;
	    att_pos_resource    = XmNbottomPosition;
	    break;
    }
    type   = obj_get_attach_type(obj, dir);
    value  = obj_get_attach_value(obj, dir);
    offset = obj_get_attach_offset(obj, dir);
    switch(type)
    {
	case AB_ATTACH_POINT:
	    objxm_obj_set_literal_ui_arg(obj, ctype, att_side_resource, XmATTACH_FORM);
	    objxm_obj_set_ui_arg(obj, AB_ARG_INT,     att_offset_resource, offset);
	    break;
	case AB_ATTACH_OBJ:
	case AB_ATTACH_ALIGN_OBJ_EDGE:
	    att_obj    = (ABObj)value;
	    if (att_obj)
	    {
		/* If has an attachment to a widget which isn't generated in
		 * code(window status area), then move the attachment to its parent
		 */
		if (ctype == OBJXM_CONFIG_CODEGEN && obj_has_flag(att_obj, NoCodeGenFlag))
		    att_obj = parent_obj;

	    	if (att_obj == parent_obj)
		    objxm_obj_set_literal_ui_arg(obj, ctype, att_side_resource, 
		  	(type==AB_ATTACH_OBJ? XmATTACH_FORM : XmATTACH_OPPOSITE_FORM));
	    	else
	    	{
		    objxm_obj_set_literal_ui_arg(obj, ctype, att_side_resource, 
		        (type==AB_ATTACH_OBJ? XmATTACH_WIDGET : XmATTACH_OPPOSITE_WIDGET));
		    objxm_obj_set_widget_ui_arg(obj, ctype, att_widget_resource, att_obj);
	    	}
	    	objxm_obj_set_ui_arg(obj, AB_ARG_INT, att_offset_resource, offset);
	    }
	    else if (util_get_verbosity() > 0)
	    {
		char	name[256];
		obj_get_safe_name(obj, name, 256);
	        util_printf_err(catgets(OBJXM_MESSAGE_CATD, OBJXM_MESSAGE_SET, 3,
		      "ERROR: %s: NULL Object attachment\n"), name);
	    }
	    break;

	case AB_ATTACH_GRIDLINE: 
	case AB_ATTACH_CENTER_GRIDLINE: 
	    objxm_obj_set_literal_ui_arg(obj, ctype, att_side_resource, XmATTACH_POSITION); 
	    objxm_obj_set_ui_arg(obj, AB_ARG_INT,     att_pos_resource, value); 
	    objxm_obj_set_ui_arg(obj, AB_ARG_INT,     att_offset_resource, offset);
	    break; 

	case AB_ATTACH_NONE:
	default:
	    objxm_obj_set_literal_ui_arg(obj, ctype, att_side_resource, XmATTACH_NONE);
	    break;
    }
    obj_set_flag(obj, AttrChangedFlag);
    return OK;
}
