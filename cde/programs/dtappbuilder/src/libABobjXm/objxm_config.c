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
 *      $XConsortium: objxm_config.c /main/4 1995/11/06 18:45:06 rswiston $
 *
 * @(#)objxm_config.c	1.27 15 Feb 1994      cde_app_builder/src/libABobjXm
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * objxm_config.c - Implements configuring an object into the
 *		   appropriate Motif hierarchy
 */
#include <stdlib.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/trav.h>
#include "objxmP.h"

#define    MAX_XMCONFIG_INFOS    35

/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/
static XmConfigInfo  	config_info_table[MAX_XMCONFIG_INFOS];
static int   		config_info_count = 0;


/*************************************************************************
**                                                                      **
**       Private Function Declarations					**
**                                                                      **
**************************************************************************/
static int	xm_configure_tree(
		    ABObj	root,
		    OBJXM_CONFIG_TYPE ctype
		);
static int	xm_configure_tree_attachments(
    		    ABObj       root
		);

static int	config_attachment( 
    		    ABObj       obj, 
    		    ABObj       parent_obj, 
    		    AB_COMPASS_POINT dir 
		); 

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Add XmConfigure info for a given object type to the XmConfigInfo table
 */
void
objxm_register_config_info(
    XmConfigInfo  cfginfo
)
{
    int        i;
    Boolean    exists = FALSE;

    /* Search to make sure it doesn't already exist in the table */
    for (i = 0; i < config_info_count; i++)
        if (config_info_table[i] == cfginfo)
        {
            exists = TRUE;
            break;
        }

    if (!exists)
    {
        if (config_info_count < MAX_XMCONFIG_INFOS)
            config_info_table[config_info_count++] = cfginfo;
        else
        {
            if (util_get_verbosity() > 0)
                fprintf(stderr,"objxm_register_config_info: config_info table full\n");
            return;
        }
    }
}

/*
 * return the XmConfigInfo corresponding to the Object
 */
XmConfigInfo
objxmP_get_config_info(
    ABObj    obj
)
{
    int 	 i;
    XmConfigInfo cfginfo = NULL;
    ABObj        vobj;

    vobj = obj_get_root(obj);

    for (i=0; i < config_info_count; i++)
        if ((*config_info_table[i]->is_a_test)(vobj))
	{
            cfginfo = config_info_table[i];
	    break;
	}

    return(cfginfo);

}

/*
 * XmConfigure an Object into its Composite Object hierarchy 
 */
int
objxm_obj_configure(
    ABObj    	      obj,
    OBJXM_CONFIG_TYPE ctype,
    BOOL	      set_args		
)
{
    int			iReturn = OK;
    XmConfigInfo	cfginfo;

    if (!obj_has_flag(obj, XmConfiguredFlag))
    {
	obj = obj_get_root(obj);

    	if (obj_is_project(obj) || 
	    obj_is_module(obj) || obj_is_message(obj))
	    obj_set_flag(obj, XmConfiguredFlag);
    	else
    	{
    	    cfginfo = objxmP_get_config_info(obj);

    	    if (cfginfo != NULL && cfginfo->xmconfig != NULL)
            	iReturn = (*cfginfo->xmconfig)(obj);
	    else
		iReturn = -1;

	    if (iReturn == OK)
	    {
                obj_comp_set_flag(obj, XmConfiguredFlag);
/*
		if (set_args)
    		    objxm_comp_set_ui_args(obj, ctype, TRUE);
*/
	    }

    	}
    }
    return iReturn;
}

/*
 * Collapse a Composite Object hierarchy back into a SINGLE
 * UnConfigured object
 */
int 
objxm_obj_unconfigure(
    ABObj	obj
)
{
    int                 iReturn= -1;
    XmConfigInfo        cfginfo;

    if (obj_has_flag(obj, XmConfiguredFlag))
    {
	obj = obj_get_root(obj);

        if (obj_is_project(obj) || obj_is_module(obj))
	{
            obj_clear_flag(obj, XmConfiguredFlag);
	    iReturn = 0;
	}
        else
        {
            cfginfo = objxmP_get_config_info(obj);

            if (cfginfo != NULL && cfginfo->xmunconfig != NULL)
                iReturn = (*cfginfo->xmunconfig)(obj);

	    if (iReturn == OK) /* Reset common fields */
	    {
		obj->part_of = NULL;
		obj_set_class_name(obj, NULL);
		obj_clear_flag(obj, XmConfiguredFlag);
	    }
        }
    }    
    else if (util_get_verbosity() > 2)
    {
	fprintf(stderr,"objxm_obj_unconfigure: %s not XmConfigured\n",
		util_strsafe(obj_get_name(obj)));
    }

    return iReturn;
}

/*
 *  XmConfigures all salient objects in the tree.  Objects already
 *  configured are ignored.
 */
int
objxm_tree_configure(
    ABObj root,
    OBJXM_CONFIG_TYPE ctype
)
{
    /* Phase I */
    xm_configure_tree(root, ctype);

    /* Phase II (Phase I *MUST* happen first)*/
    xm_configure_tree_attachments(root);

    /* Phase III */
    objxm_tree_set_ui_args(root, ctype, TRUE);

    /* Phase IV (ONLY happens here for Code-generation! */
    if (ctype == OBJXM_CONFIG_CODEGEN) 
        objxm_tree_set_post_inst_args(root, ctype); 

    return 0;

}

/*
 * Collapses all salient Objects back down into their
 * UnConfigured state
 */
int
objxm_tree_unconfigure(
    ABObj root
)
{
    int                 iRetVal= 0;
    AB_TRAVERSAL        trav;
    ABObj               obj= NULL;
 
    /* MUST be BOTTOM-UP Traversal */
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (obj= trav_next(&trav)) != NULL; )
        objxm_tree_unconfigure(obj);

    if (obj_is_salient(root))
    	objxm_obj_unconfigure(root);

    trav_close(&trav);
    return 0;
}
 
static int
xm_configure_tree(
    ABObj 	root,
    OBJXM_CONFIG_TYPE ctype
)
{
    int			iRetVal= 0;
    AB_TRAVERSAL	trav;
    ABObj		obj= NULL;
    int			numSalientObjs= 0;
    int			configCount= 0;

    /* Must be TOP-DOWN traversal */
    numSalientObjs= trav_count(root, AB_TRAV_SALIENT);
    for (trav_open(&trav, root, 
	    AB_TRAV_SALIENT | AB_TRAV_MOD_PARENTS_FIRST | AB_TRAV_MOD_SAFE);
	(obj= trav_next(&trav)) != NULL; )
    {
	++configCount;
	objxm_obj_configure(obj, ctype, FALSE);
    }
    trav_close(&trav);

    if (configCount!= numSalientObjs)
    {
	fprintf(stderr, "%s",
	    catgets(OBJXM_MESSAGE_CATD, OBJXM_MESSAGE_SET, 8,
	       "INTERNAL ERROR: salient counts don't match.\n") );
	iRetVal= -1;
	goto epilogue;
    }

epilogue:
    return iRetVal;
}

static int
xm_configure_tree_attachments(
    ABObj	root
)
{
    AB_TRAVERSAL trav;
    ABObj	 obj;

    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj= trav_next(&trav)) != NULL; )
	if (obj->attachments)
	    objxmP_obj_config_attachments(obj);

    trav_close(&trav);
   
    return 0;

}

/*
 * Call method for setting object-type specific Resources
 * for a Composite Object hierarchy
 */
int
objxmP_comp_set_type_args(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    XmConfigInfo cfginfo;

    if (obj_has_flag(obj, XmConfiguredFlag))
    {
    	cfginfo = objxmP_get_config_info(obj);

    	if (cfginfo != NULL && cfginfo->set_args != NULL)
            return((*cfginfo->set_args)(obj_get_root(obj),ctype));
    }

    return -1;
}

/*
 * Call method for Post-instantiation-handling specific
 * to an object-type 
 */
int
objxmP_comp_type_post_instantiate(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    XmConfigInfo cfginfo;

    cfginfo = objxmP_get_config_info(obj);

    if (cfginfo != NULL && cfginfo->post_instantiate != NULL)
            return((*cfginfo->post_instantiate)(obj_get_root(obj), ctype));

    return -1;
}
    
/*
 * Convert Attachment fields for an UnConfigured object
 * into Attachments appropriate for its XmConfigured 
 * Composite Object hierarchy
 */
int
objxmP_obj_config_attachments(
    ABObj       obj
)
{
    ABObj       parent_obj = obj_get_parent(obj);

    if (obj->attachments)
    {
        config_attachment(obj, parent_obj, AB_CP_NORTH);
        config_attachment(obj, parent_obj, AB_CP_WEST);
        config_attachment(obj, parent_obj, AB_CP_EAST);
        config_attachment(obj, parent_obj, AB_CP_SOUTH);
    }
    return 0;
}

static int
config_attachment(
    ABObj       obj,
    ABObj       parent_obj,
    AB_COMPASS_POINT dir
)
{
    ABObj        attobj;
    ABAttachment *attachment = NULL;

    switch(dir)
    {
        case AB_CP_NORTH:
            attachment = &(obj->attachments->north);
            break;
        case AB_CP_WEST:
            attachment = &(obj->attachments->west);
            break;
        case AB_CP_EAST:
            attachment = &(obj->attachments->east);
            break;
        case AB_CP_SOUTH:
            attachment = &(obj->attachments->south);
            break;
        default:
            return OK;
    }
    if (attachment->type == AB_ATTACH_OBJ)
    {
        attobj = (ABObj)(attachment->value);

        if (attobj == obj_get_root(parent_obj))
        {
            attobj = objxm_comp_get_subobj(attobj, AB_CFG_PARENT_OBJ);
            attachment->value = (void*)attobj;
        }
    }    

    return OK;
}

/*
 * Call method for object-type which retreives the Composite
 * Object's SubObj responsible for 'AB_CFG_OBJ_TYPE' (ie. SIZE,
 * POSITION, etc)
 */
ABObj
objxm_comp_get_subobj(
    ABObj	obj,
    AB_CFG_OBJ_TYPE type
)
{
    XmConfigInfo	cfginfo;
    ABObj		cfgobj = NULL;

    if (obj_has_flag(obj, XmConfiguredFlag))
    {
    	cfginfo = objxmP_get_config_info(obj);
 
    	if (cfginfo != NULL && cfginfo->get_config_obj != NULL)
            cfgobj= 
		((*cfginfo->get_config_obj)(obj_get_root(obj), type));
    }
    else if (util_get_verbosity() >= 3) 
    {
	char	name[256];
	obj_get_safe_name(obj, name, 256);
        fprintf(stderr,
	    catgets(OBJXM_MESSAGE_CATD, OBJXM_MESSAGE_SET, 9,
	      "ERROR: objxm_comp_get_subobj - object not xmconfigured: %s\n"),
	    name);
			
    }

    return cfgobj;
}

ABObj
objxmP_create_direct_subobj(
    ABObj	root,
    AB_OBJECT_TYPE type,
    STRING      subname
)
{
    ABObj	subobj;
 
    subobj = obj_create(type, NULL);
    subobj->part_of = root;
 
    /* Move children from root to immediate SubObj */
    /* NOTE: obj must NOT have a MenuRef SubObj at this
     * point!
     */
    if (obj_get_num_children(root) > 0) 
        obj_move_children(subobj, root);
 
    obj_append_child(root, subobj);
/*
    obj_set_name_from_parent(subobj, subname);
*/
    obj_set_unique_name(subobj,
	ab_ident_from_name_and_label(obj_get_name(root), subname));

    return subobj;

}

/*
 * If the Obj should have a border-frame, sets the RootObj 
 * to be a Frame and creates a SubObj directly under the Obj
 * to represent the actual Object inside the Frame, and 
 * returns a handle to the SubObj
 * If the Obj does not have a border-frame, return NULL
 */
ABObj
objxm_comp_config_border_frame(
    ABObj	obj
)
{
    ABObj	subobj = NULL;

    if (obj_has_border_frame(obj))
    {
	/* Make the RootObj the Frame */
        obj->part_of = obj;
        obj_set_class_name(obj, _xmFrame);

	/* Create a SubObj directly under the Frame */
        subobj = objxmP_create_direct_subobj(obj, AB_TYPE_CONTAINER, "obj");
    }
    return subobj;

}

/*
 * If Obj is XmConfigured with a Frame, collapse it
 * back down to remove the Frame
 */
int
objxm_comp_unconfig_border_frame(
    ABObj	obj
)
{
    ABObj subobj;

    if (obj_has_border_frame(obj))
    {
        subobj = obj_get_child(obj, 0);

	/* If First child is Menu-ref, then SubObj
	 * should be next child
	 */
	if (obj_is_menu_ref(subobj))
	    subobj = obj_get_child(obj, 1);
 
	if (subobj)
	{
            if (obj_get_num_children(subobj) > 0)
            	obj_move_children(obj, subobj);
 
            obj_destroy(subobj);
	}
	else if (util_get_verbosity() > 2)
	{
	    fprintf(stderr,"objxm_comp_unconfig_border_frame: %s : can't find SubObj\n", 
		util_strsafe(obj_get_name(obj)));
	}
    }
    return 0;

}

int
objxm_comp_config_labeled_obj(
    ABObj    		obj,
    AB_OBJECT_TYPE 	obj_type,
    STRING		obj_suffix,
    STRING		obj_classname
)
{
    ABObj       lbl_obj = NULL;
    ABObj       real_obj;

    if (obj_has_label(obj))
    {
        obj->part_of = obj;

        /* Set Container RootObj class */
        obj_set_class_name(obj, _xmRowColumn);

        /* Create Label SubObj */
        lbl_obj = obj_create(AB_TYPE_LABEL, NULL);
        lbl_obj->part_of = obj;
        obj_set_unique_name(lbl_obj,
                ab_ident_from_name_and_label(obj_get_name(obj), "label"));
        obj_set_class_name(lbl_obj, _xmLabel);

        /* Create Real *Object* SubObj */
        real_obj = obj_create(obj_type, NULL);
        real_obj->part_of = obj;
        obj_set_unique_name(real_obj,
                ab_ident_from_name_and_label(obj_get_name(obj), obj_suffix));
        obj_set_class_name(real_obj, obj_classname);

	if (obj_get_num_children(obj) > 0)
	    obj_move_children(real_obj, obj);

        obj_append_child(obj, lbl_obj);
        obj_append_child(obj, real_obj);
 
    }
    else
	obj_set_class_name(obj, obj_classname);

    return OK;
 
}

int
objxm_comp_unconfig_labeled_obj(
    ABObj	obj
)
{
    ABObj	lbl_obj;
    ABObj	real_obj;

    if (obj_has_label(obj)) /* Has Label */
    {
        /* Destroy Label & Field SubObjs */
        lbl_obj = obj_get_child(obj, 0);
        real_obj  = obj_get_child(obj, 1);

        if (obj_get_num_children(obj) > 0) 
            obj_move_children(obj, real_obj);

        obj_destroy(lbl_obj);
        obj_destroy(real_obj);
    }

    return OK;
}

/*
 * If the Obj has a Menu, create a Menu-reference SubObj
 * as the FIRST child of the Obj and return a handle to
 * the Menu-reference.  If Obj has no menu, or the menu
 * is not found, return NULL.
 */
ABObj
objxm_comp_config_menu_ref(
    ABObj	obj
)
{
    ABObj	menu_obj;
    ABObj	menu_ref = NULL;
    STRING	menuname = obj_get_menu_name(obj);

    if (menuname != NULL)
    {
	/* Make sure Virtual Menu obj exists in module */
	if ((menu_obj = obj_find_menu_by_name(obj_get_module(obj), menuname)) != NULL)
	{
	    /* ALWAYS make Menu Reference FIRST child! */
	    menu_ref = obj_create_ref(menu_obj);
            obj_insert_child(obj, menu_ref, 0);
/*
 REMIND: aim, 9/13/94 - The Menu Ref should not really be a "part of" an object
	 But, leaving this in (commented out) for now in case change
	 causes problems 

	    menu_ref->part_of = obj;
*/

	    /* Menu Reference must point to Virtual menu object */
	    obj_set_unique_name(menu_ref,
		ab_ident_from_name_and_label(obj_get_name(obj), menuname));

	    /* 
	    ** Set the object class of the Menu Reference to be that of the
	    ** virtual menu object to which it refers.  This makes it easier 
	    ** later on to figure out some of the aspects of handling the Menu 
	    ** Reference.
	    */
	    if(obj_is_menubar_item(obj) || obj_is_menu_item(obj)) {
		obj_set_class_name(menu_ref, _xmPulldownMenu);
	    }
	    else {
		obj_set_class_name(menu_ref, _xmPopupMenu);
	    }
	}
    }
    return menu_ref;
}

/* 
 * If Obj has a Menu-ref, remove it
 */
int
objxm_comp_unconfig_menu_ref(
    ABObj	obj
)
{
    ABObj	menu_ref;

    if (obj_has_menu(obj))
    {
    	menu_ref = objxm_comp_get_subobj(obj, AB_CFG_MENU_OBJ);

    	if (obj_is_ref(menu_ref))
	    obj_destroy(menu_ref);
    }
    return 0;

}
