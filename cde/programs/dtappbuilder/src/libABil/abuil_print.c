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
 *	$XConsortium: abuil_print.c /main/4 1996/10/02 11:38:09 drk $
 *
 * @(#)abuil_print.c	1.30 19 Oct 1994	cose/unity1/cde_app_builder/src/libABil
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
 * This file contains the implementation of the uil print
 * component
 */
#include <ctype.h>

#include <sys/param.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/abio.h>
#include <ab_private/objxm.h>
#include <ab_private/istr.h>
#include "abuil_print.h"
#include "../libABobj/obj_utils.h"

#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/ScrolledW.h>
#include <Dt/Term.h>
#include <Dt/SpinBox.h>
#include <Dt/ComboBox.h>

/*----------------------------------------*
 * Private macro definitions
 *----------------------------------------*/
#define CLASS_SUFFIX		"WidgetClass"
#define GROUP_CLASS		"groupWidgetClass"
#define GROUP_SUBSTITUTE	"XmBulletinBoard"
#define INSTANCE_SUFFIX		""
#define TABSP			4
#define DBLSP			(TABSP+TABSP)

/*----------------------------------------*
 * Private function prototypes
 *----------------------------------------*/

static int	indent(
		    FILE *fp,
		    int spaces
		);
static int	print(
		    ABObjPtr obj
		);
static int	print_indented(
		    FILE *fp,
		    ABObjPtr obj
		);
static int	print_tree(
		    FILE *fp,
		    ABObjPtr obj
		);
static int	print_tree_indented(
		    FILE *fp,
		    ABObjPtr obj
		);
static int	verify_tree(
		    ABObjPtr root
		);

static int	output_ui_arg(
		    FILE *fp,
		    ABObj obj,
    		    Arg	*arg
		);

static int	handle_dtterm(
		    FILE *fp,
		    ABObj obj
		);

static int	handle_combo_box(
		    FILE *fp,
		    ABObj obj
		);

static int	handle_spin_box(
		    FILE *fp,
		    ABObj obj
		);

static int	handle_menu_button(
		    FILE *fp,
		    ABObj obj
		);

/*----------------------------------------
 * Write UIL for the project containing one AppBuilder interface.
 *
 * Not supported in the 1.0 Release:
 *    Major things:
 *	- Handle callbacks 
 *    Minor things:
 *	- Convert names > 31 characters to something unique and < 31 chars
 *	- Do something with messages other than ignoring them
 */
extern void
abuil_obj_print_uil(
    FILE	*fp,
    ABObjPtr	ab_proj
)
{
    /* 
    ** The module we are about to export as UIL may have already been
    ** configured and have build-mode UI args.  We need to remove those
    ** and set the UI args to be in CODEGEN mode.
    */
    objxm_tree_remove_ui_args(ab_proj,OBJXM_CONFIG_BUILD);
    objxm_tree_set_ui_args(ab_proj,OBJXM_CONFIG_CODEGEN,TRUE);

    /* Print the tree as UIL */
    print_tree(fp,ab_proj);

    /* Now remove all the UI args we set */
    objxm_tree_remove_ui_args(ab_proj,OBJXM_CONFIG_CODEGEN);
}

static int
indent(
    FILE *fp,
    int	spaces
)
{
    int	i;
    
    for (i = 0; i < spaces; ++i)
	putc(' ',fp);

    return(0);
}

static STRING
fix_prefix_suffix(
    STRING	widget_class_name
)
{
    static char	s[1024];
    STRING	m;
    STRING	p;
    int		n;
    int		cl_len = strlen(CLASS_SUFFIX);
    
    if (widget_class_name == NULL)
	return(util_strsafe(widget_class_name));

    if (!strcmp(widget_class_name, GROUP_CLASS))
	return(GROUP_SUBSTITUTE);
    
    /* Search for class suffix */
    p = NULL;
    m = strstr(widget_class_name, CLASS_SUFFIX);
    while (m != NULL)
    {
	p = m;
	m = strstr(m+cl_len, CLASS_SUFFIX);
    }
    if (p != NULL && *(p+cl_len) == '\0')
	n = p - widget_class_name;
    else
	n = strlen(widget_class_name);

    /* Strip the class suffix */
    strncpy(s, widget_class_name, n);
    s[n] = 0;

    /* Capitalize first char */
    s[0] = toupper(s[0]);
    return(s);
}

static STRING
suffixed(
    STRING	widget_instance_name
)
{
    static char buf[1024];

    if (widget_instance_name == NULL)
	return(util_strsafe(widget_instance_name));
    
    snprintf(buf, sizeof(buf), "%s%s", widget_instance_name, INSTANCE_SUFFIX);
    return(buf);
}

static int 
print_indented(
    FILE	*fp,
    ABObjPtr	obj
)
{
    AB_TRAVERSAL	trav;
    ABObjPtr		child_obj	= NULL;
    ArgList		argList;
    Arg			*arg = NULL;
    ABObj		menu_ref, obj_obj, root_obj;
    char 		*child_class;

    if (obj == NULL)
    {
	fprintf(fp,"! NULL Object");
	return(0);
    }

    /* Skip over non-ui objects (e.g. connections) */
    if( !obj_is_ui(obj) ) return(0);

    /* Skip over AB objects we don't map to UIL, namely:
    **    - message objects
    **    - menus (but we shouldn't skip menu-ref objects!)
    **	  - items for a combo box, list or spin box
    **	  - ApplicationShell objects
    **	  - Objects with the NoCodeGenFlag set
    */
    if( obj_is_message(obj) || 
	(obj_is_menu(obj) && !obj_is_ref(obj)) ||
	obj_is_combo_box_item(obj) ||
	obj_is_list_item(obj) ||
	obj_is_spin_box_item(obj) ||
	(util_streq(obj_get_class_name(obj),_applicationShell) ||
	obj_has_flag(obj,NoCodeGenFlag) )
      ) return(0);

    /* Intercept non-UIL objects and call special-case handlers for them */
    if(obj_is_term_pane(obj)) {
	handle_dtterm(fp,obj);
	return(0);
    }
    if(util_streq(obj_get_class_name(obj),_dtSpinBox)) {
	handle_spin_box(fp,obj);
	return(0);
    }
    if(util_streq(obj_get_class_name(obj),_dtComboBox)) {
	handle_combo_box(fp,obj);
	return(0);
    }
    if(obj_is_menu_button(obj)) {
	handle_menu_button(fp,obj);
	return(0);
    }

    /*
     * Write object instance declaration
     */
    fprintf(fp,"\nobject\n");
    fprintf(fp,"%s:%s {\n", 
	   suffixed(obj_get_name(obj)),
	   fix_prefix_suffix(obj_get_class_name(obj)));

    /* 
    ** Special case handling for menus.  App Builder support sharable menus 
    ** by creating just one instance of the menu itself and then attaching
    ** special menu-reference objects as children of each object that uses the
    ** sharable menu.  (The menu-reference objects each point to the real menu
    ** object.)  Motif doesn't support this, so each time we find a menu-ref
    ** object we have to write out in UIL a copy of the menu to which the
    ** menu-ref points as a unique, separate menu with the same name (and 
    ** class) as the menu-ref object.
    */
    if(obj_is_menu_ref(obj)) {
	menu_ref = obj;				/* save the menu_ref obj */
	obj = obj_get_actual_obj(menu_ref);	/* substitute the actual menu */
    }

    argList = objxm_obj_get_ui_args(obj);

    indent(fp,TABSP); fprintf(fp,"arguments {\n");
    for(arg=(Arg *)argList; (arg != NULL) && (arg->name != NULL); arg++) {
	output_ui_arg(fp,obj,arg);
    }
    
    /* 
    ** Special case handling for Option Menus.  The actual OptionMenu subobject
    ** in the composite option menu ABObj must have its XmNsubMenuId resource
    ** set to point to the PulldownMenu subobject.
    */
    root_obj = obj_get_root(obj);
    if(obj_is_option_menu(root_obj)) {
        obj_obj = objxm_comp_get_subobj(root_obj,AB_CFG_OBJECT_OBJ);
	if(obj_obj == obj) {
	    child_obj=objxm_comp_get_subobj(root_obj,AB_CFG_PARENT_OBJ);
	    if(child_obj != (ABObj)NULL) {
	        indent(fp,DBLSP);
	        fprintf(fp,"XmNsubMenuId = %s ; \n",obj_get_name(child_obj));
	    }
        }
    }

    /* End of Arguments section */
    indent(fp,DBLSP); fprintf(fp,"};\n");


    trav_open(&trav, obj, AB_TRAV_CHILDREN);
    if ((child_obj = trav_next(&trav)) != NULL)
    {
	/*
	 * Write out controls section
	 */
	indent(fp,TABSP); fprintf(fp,"controls {\n");

	do
	{
	    /* Transform Dt type child to UIL-supported Xm type */
	    if(obj_is_term_pane(child_obj))     {
		child_class = "XmText";
	    }
	    else if(util_streq(obj_get_class_name(child_obj),_dtSpinBox)) {
	    	child_class = "XmTextField";
	    }
	    else if(util_streq(obj_get_class_name(child_obj),_dtComboBox)) {
	    	child_class = "XmOptionMenu";
	    }
	    else if(obj_is_menu_button(child_obj))     {
		child_class = "XmPushButton";
	    }
	    else child_class = fix_prefix_suffix(obj_get_class_name(child_obj));

	    /* Now output the child reference */
	    indent(fp,DBLSP); 
	    fprintf(fp,"%s %s;\n", child_class,
			  suffixed(obj_get_name(child_obj)));
	}
	while ((child_obj = trav_next(&trav)) != NULL);

	indent(fp,DBLSP); fprintf(fp,"};\n");
    }
    trav_close(&trav);

    /*
     * Write out callbacks section
     */
    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");
    
    fprintf(fp,"};\n");
    return(0);
} /* print_indented */


static int
print_tree(
    FILE	*fp,
    ABObjPtr	obj
)
{
    int	iRet	= 0;
    
    fprintf(fp,"! ***** Object tree *****\n");
    iRet = print_tree_indented(fp,obj);
    iRet = verify_tree(obj);
    if (iRet < 0)
    {
	fprintf(fp,"\n! ***\n! ***  TREE IS CORRUPT!\n***");
    }
    fprintf(fp,"! ******* Tree End ******\n");
    return(iRet);
}


static int
print_tree_indented(
    FILE	*fp,
    ABObjPtr	obj
)
{
    AB_TRAVERSAL	trav;
    ABObjPtr		child_obj	= NULL;

    if (obj == NULL)
    {
	fprintf(fp,"! NULL Tree\n");
	return(0);
    }

    switch (obj->type)
    {
      case AB_TYPE_PROJECT:
	for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	     (child_obj = trav_next(&trav)) != NULL; )
	{
	    print_tree_indented(fp,child_obj);
	}
	break;
      case AB_TYPE_MODULE:
	fprintf(fp,"module %s\n", nullstr(obj_get_name(obj)));
	fprintf(fp,"names = case_sensitive\n");
	
	for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	     (child_obj = trav_next(&trav)) != NULL; )
	{
	    print_tree_indented(fp,child_obj);
	}
	fprintf(fp,"end module;\n");
	break;
      default:
	print_indented(fp,obj);
	for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	     (child_obj = trav_next(&trav)) != NULL; )
	{
	    print_tree_indented(fp,child_obj);
	}
	break;
    }
    trav_close(&trav);

    return(0);
}

static int
verify_tree(
    ABObjPtr	root
)
{
    int			ret_val		= 0;
    AB_TRAVERSAL	trav;
    ABObjPtr		obj		= NULL;
    ABObjPtr		next_sibling	= NULL;
    ABObjPtr		prev_sibling	= NULL;
    ABObjPtr		parent		= NULL;

    if (root == NULL)
    {
	/* empty tree is valid (I guess) */
	return(0);
    }

    for (trav_open(&trav, root, AB_TRAV_ALL); 
	 (obj = trav_next(&trav)) != NULL; )
    {
	next_sibling = obj->next_sibling;
	prev_sibling = obj->prev_sibling;
	parent = obj->parent;

	if (next_sibling != NULL)
	{
	    if (next_sibling->prev_sibling != obj)
	    {
		ret_val = -1;
		fprintf(stderr, "Bad prev_sibling 0x%p %s\n",
			obj, nullstr(obj_get_name(obj)));
	    }
	}

	if (prev_sibling != NULL)
	{
	    if (prev_sibling->next_sibling != obj)
	    {
		ret_val = -1;
		fprintf(stderr, "Bad next_sibling 0x%p %s\n",
			obj, nullstr(obj_get_name(obj)));
	    }
	}

	if (parent != NULL)
	{
	    AB_TRAVERSAL	trav;
	    ABObjPtr		child;
	    BOOL		found	= FALSE;
	    
	    for (trav_open(&trav, parent, AB_TRAV_CHILDREN);
		 (child = trav_next(&trav)) != NULL; )
	    {
		if (child == obj)
		{
		    found = TRUE;
		}
	    }
	    trav_close(&trav);
	    if (!found)
	    {
		ret_val = -1;
		fprintf(stderr,"Bad parent 0x%p %s\n",
		       obj, nullstr(obj_get_name(obj)));
	    }
	}
    }
    trav_close(&trav);

    return(ret_val);
}


/* 
** Generalized routine to output an object's UI argument.  This is used to
** fill in the 'arguments' section in the UIL definition for the object.
*/
static int
output_ui_arg(FILE *fp, ABObj obj, Arg *arg)
{
    ABObj		module, attachObj;
    STRING		attachObjName;

    switch(objxm_get_res_type(arg->name)) {
	case AB_ARG_INT:
	    indent(fp,DBLSP); 
	    fprintf(fp,"XmN%s = %d ;\n",arg->name,(int)(arg->value));
	    break;
	case AB_ARG_STRING:
	    indent(fp,DBLSP); 
	    fprintf(fp,"XmN%s = \"%s\" ;\n",arg->name, (char *) arg->value);
	    break;
	case AB_ARG_BOOLEAN:
	    /* REMIND: handle item obj's XmNset resource for an array */
	    indent(fp,DBLSP); 
	    fprintf(fp,"XmN%s = %s ;\n",arg->name, 
	    	((arg->value)?"true":"false") );
	    break;
	case AB_ARG_PIXEL:		/* istring */
	    indent(fp,DBLSP);
	    fprintf(fp,"XmN%s = color('%s') ;\n",arg->name,
		(arg->value != 0 ? (char *) istr_string((ISTRING)arg->value):"?")
		);
	    break;
	case AB_ARG_PIXMAP:		/* istring */
	    indent(fp,DBLSP);
	    fprintf(fp,"XmN%s = \"%s\" ;\n",arg->name,
	    	(char *) istr_string((ISTRING)arg->value));
	    break;
	case AB_ARG_FONT:
	    indent(fp,DBLSP);
	    fprintf(fp,"XmN%s = \"%s\" ;\n",arg->name, (char *) arg->value);
	    break;
	case AB_ARG_LITERAL:	/* istring, but not quoted in output */
	    indent(fp,DBLSP);
	    fprintf(fp,"XmN%s = %s ;\n",arg->name,
	    	(char *) istr_string((ISTRING)arg->value));
	    break;
	case AB_ARG_WIDGET:		/* istring (object name, not widget) */
	    if( (module = obj_get_module(obj)) == (ABObj)NULL) {
	       fprintf(fp,"!Couldn't get module for arg %s\n",arg->name);
	       break;
	    }
	    if((attachObjName = istr_string((ISTRING)arg->value)) == NULL) {
	       fprintf(fp,"!Couldn't get attach obj name for %s\n",
	    	arg->name);
	       break;
	    }
	    if( (attachObj=obj_scoped_find_by_name(module,attachObjName)) ==
	    	NULL) {
	       fprintf(fp,"!Couldn't get attach obj for %s\n", arg->name);
	       break;
	    }
	    if(attachObj != (ABObj)NULL) {
	        indent(fp,DBLSP);
	        fprintf(fp,"XmN%s = %s ;\n",arg->name,
	    	suffixed(obj_get_name(attachObj)));
	    }
	    break;
	case AB_ARG_XMSTRING:	/* istring */
	        indent(fp,DBLSP);
	        fprintf(fp,"XmN%s = \"%s\" ;\n",arg->name,
	        (char *) istr_string((ISTRING)arg->value));
	    break;
	case AB_ARG_VOID_PTR:
	    /* Only meaningful for rowcolumn's XmNentryClass resource */
	    if(! util_streq(arg->name, XmNentryClass)) break;

	    indent(fp,DBLSP);
	    fprintf(fp,"XmN%s = ",arg->name);
	    if((WidgetClass)arg->value == xmCascadeButtonWidgetClass) {
	        fprintf(fp,"\"xmCascadeButtonWidgetClass\" ;\n");
	    }
	    else if((WidgetClass)arg->value == xmScrolledWindowWidgetClass){
	        fprintf(fp,"\"xmScrolledWindowWidgetClass\" ;\n");
	    }
	    else if((WidgetClass)arg->value == xmToggleButtonWidgetClass) {
	        fprintf(fp,"\"xmToggleButtonWidgetClass\" ;\n");
	    }
	    else {
	        fprintf(fp,"!! unrecognized XmNentryClass !!;\n");
	    }
	    break;
	default:
	    fprintf(fp,"!	XmN%s = currently unsupported\n",arg->name);
	    break;
    }
    return 0;
}

static int
handle_dtterm(FILE *fp, ABObj obj)
{
    ABObjPtr		child_obj	= NULL;
    ArgList		argList;
    Arg			*arg = NULL;

    /*
     * Write object instance declaration, substituting XmText
     */
    fprintf(fp,"\n! DtTerm object transformed into XmText\n");
    fprintf(fp,"object\n");
    fprintf(fp,"%s:XmText {\n", suffixed(obj_get_name(obj)));

    argList = objxm_obj_get_ui_args(obj);

    /*
    ** Output DtTerm object arguments, mapping to XmText as we go.  
    ** There are two cases to be dealt with:
    **   1) DtTerm with a vertical scroll bar - This will yield an
    **	    XmScrolledWindow with a DtTerm child that has its 
    **	    XmNverticalScrollBar resource set.  XmText doesn't support
    **	    XmNverticalScrollBar, so we shouldn't output it.   Also, we need to
    **	    make sure XmText is multi-line instead of the default single line.
    **   2) DtTerm with no vertical scrolling - Here we just get the DtTerm
    **	    and don't have to worry about scrollbars.
    ** In both cases we also want to make sure we don't print the DtTerm's
    ** DtNsubprocessCmd resource.
    */

    /* Process/filter the DtTerm ui_arg list */
    indent(fp,TABSP); fprintf(fp,"arguments {\n");
    for(arg=(Arg *)argList; (arg != NULL) && (arg->name != NULL); arg++) {
	/* Skip DtNsubprocessCmd resource */
	if(util_streq(arg->name, DtNsubprocessCmd)) continue;

	/* Skip XmNverticalScrollBar resource */
	if(util_streq(arg->name, XmNverticalScrollBar)) continue;

	output_ui_arg(fp,obj,arg);
    }

    /* Add extra resources needed when you transform DtTerm into XmText */
    if(obj_has_vscrollbar(obj)) {
	fprintf(fp,"XmNedit_mode = XmMULTI_LINE_EDIT ;\n");
    }

    indent(fp,DBLSP); fprintf(fp,"};\n");
    
    /* 
    ** A DtTerm widget can't have children, so don't bother with a
    ** 'controls' section
    */

    /*
     * Write out callbacks section
     */
    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");
    
    fprintf(fp,"};\n");
    return(0);
}

static int
handle_combo_box(FILE *fp, ABObj obj)
{
    ABObjPtr		child_obj	= NULL;
    ArgList		argList;
    Arg			*arg = NULL;
    AB_TRAVERSAL	trav;
    char   		child_base_name[64], child_name_buf[64], *child_name;

    /*
     * Write object instance declaration, substituting OptionMenu
     */
    fprintf(fp,"\n! DtComboBox object transformed into XmOptionMenu\n");
    fprintf(fp,"object\n");
    fprintf(fp,"%s:XmOptionMenu {\n", suffixed(obj_get_name(obj)));

    /* We need to create a XmPulldownMenu child for the OptionMenu */
    sprintf(child_base_name,"%s_pmenu",obj_get_name(obj));
    child_name = obj_get_unique_name(obj,child_base_name,64,child_name_buf);

    argList = objxm_obj_get_ui_args(obj);

    /* Process/filter the ui_arg list */
    indent(fp,TABSP); fprintf(fp,"arguments {\n");
    for(arg=(Arg *)argList; (arg != NULL) && (arg->name != NULL); arg++) {
	if(util_streq(arg->name, DtNcomboBoxType))   continue;

	output_ui_arg(fp,obj,arg);
    }
    indent(fp,DBLSP); fprintf(fp,"XmNsubMenuId = %s ;\n",child_name);
    indent(fp,DBLSP); fprintf(fp,"};\n");

    /* ComboBox is now an OptionMenu, which has one child - the PulldownMenu */
    indent(fp,TABSP); fprintf(fp,"controls {\n");
    indent(fp,DBLSP); fprintf(fp,"XmPulldownMenu %s ;\n",child_name);
    indent(fp,DBLSP); fprintf(fp,"};\n");

    /*
     * Write out callbacks section
     */
    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");

    /* End of OptionMenu object */
    fprintf(fp,"};\n");


    /* O.K., now we generate the PulldownMenu child */
    fprintf(fp,"\n! PulldownMenu synthesized as part of faux-DtComboBox\n");
    fprintf(fp,"object\n");
    fprintf(fp,"%s:XmPulldownMenu {\n", child_name);

    /* No need for any arguments */
    indent(fp,TABSP); fprintf(fp,"arguments {\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");

    /* 
    ** Now we transform the combo box's children into items for the 
    ** PulldownMenu
    */
    indent(fp,TABSP); fprintf(fp,"controls {\n");
    trav_open(&trav, obj, AB_TRAV_CHILDREN);
    if ((child_obj = trav_next(&trav)) != NULL)
    {
	do {
	    indent(fp,DBLSP); fprintf(fp,"XmPushButton %s ;\n",
		suffixed(obj_get_name(child_obj)));
	}
	while ((child_obj = trav_next(&trav)) != NULL);
    }
    trav_close(&trav);
    indent(fp,DBLSP); fprintf(fp,"};\n");

    /*
     * Write out callbacks section
     */
    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");

    fprintf(fp,"};\n");


    /* Now generate the menu items themselves */
    trav_open(&trav, obj, AB_TRAV_CHILDREN);
    if ((child_obj = trav_next(&trav)) != NULL)
    {
	do {
	    fprintf(fp,"\n! DtComboBox list item transformed into menu item\n");
	    fprintf(fp,"object\n");
	    fprintf(fp,"%s:XmPushButton {\n",suffixed(obj_get_name(child_obj)));

	    indent(fp,TABSP); fprintf(fp,"arguments {\n");
	    indent(fp,DBLSP); fprintf(fp,"XmNlabelString = \"%s\" ;\n",
				obj_get_label(child_obj));
	    indent(fp,DBLSP); fprintf(fp,"XmNlabelType = XmSTRING ;\n");
	    indent(fp,DBLSP); fprintf(fp,"};\n");

	    /*Write out callbacks section */
	    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
	    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
	    indent(fp,DBLSP); fprintf(fp,"};\n");

    	    fprintf(fp,"};\n");
	}
	while ((child_obj = trav_next(&trav)) != NULL);
    }
    trav_close(&trav);

    /* We're done... */
    return(0);
}

/* 
** Transform a DtSpinBox into a text field
*/
static int
handle_spin_box(FILE *fp, ABObj obj)
{
    ABObjPtr		child_obj	= NULL;
    ArgList		argList;
    Arg			*arg = NULL;

    /*
     * Write object instance declaration, substituting TextField
     */
    fprintf(fp,"\n! DtSpinBox object transformed into XmTextField\n");
    fprintf(fp,"object\n");
    fprintf(fp,"%s:XmTextField {\n", suffixed(obj_get_name(obj)));

    argList = objxm_obj_get_ui_args(obj);

    /* Process/filter the ui_arg list */
    indent(fp,TABSP); fprintf(fp,"arguments {\n");
    for(arg=(Arg *)argList; (arg != NULL) && (arg->name != NULL); arg++) {
	if(util_streq(arg->name, DtNspinBoxChildType))   continue;
	if(util_streq(arg->name, DtNminimumValue))       continue;
	if(util_streq(arg->name, DtNmaximumValue))       continue;
	if(util_streq(arg->name, DtNposition))           continue;
	if(util_streq(arg->name, DtNincrementValue))     continue;
	if(util_streq(arg->name, DtNarrowLayout))        continue;
	if(util_streq(arg->name, XmNdecimalPoints))      continue;

	output_ui_arg(fp,obj,arg);
    }

    indent(fp,DBLSP); fprintf(fp,"};\n");
    
    /* 
    ** While a SpinBox has children, the XmTextField we map it to can't,
    ** so just ignore the 'controls' section
    */

    /*
     * Write out callbacks section
     */
    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");
    fprintf(fp,"};\n");

    return(0);
}

static int
handle_menu_button(FILE *fp, ABObj obj)
{
    ABObjPtr		child_obj	= NULL;
    ArgList		argList;
    Arg			*arg = NULL;

    /*
     * Write object instance declaration, substituting OptionMenu
     */
    fprintf(fp,"\n! DtMenuButton object transformed into XmPushButton\n");
    fprintf(fp,"object\n");
    fprintf(fp,"%s:XmPushButton {\n", suffixed(obj_get_name(obj)));

    argList = objxm_obj_get_ui_args(obj);

    /* Process/filter the ui_arg list */
    indent(fp,TABSP); fprintf(fp,"arguments {\n");
    for(arg=(Arg *)argList; (arg != NULL) && (arg->name != NULL); arg++) {

	output_ui_arg(fp,obj,arg);
    }

    indent(fp,DBLSP); fprintf(fp,"};\n");

    /*
     * Write out callbacks section
     */
    indent(fp,TABSP); fprintf(fp,"callbacks {\n");
    indent(fp,DBLSP); fprintf(fp,"/* unimplemented */\n");
    indent(fp,DBLSP); fprintf(fp,"};\n");

    fprintf(fp,"};\n");
    return(0);
}
