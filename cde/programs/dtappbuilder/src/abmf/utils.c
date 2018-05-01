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
 * $XConsortium: utils.c /main/3 1995/11/06 18:16:13 rswiston $
 * 
 * @(#)utils.c	3.76 16 Feb 1994	cde_app_builder/src/abmf
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * Main utility functions.
 */

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ab_private/XmAll.h>
#include <ab/util_types.h>
#include <ab_private/istr.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include "abmfP.h"
#include "motifdefsP.h"
#include "write_cP.h"
#include "write_codeP.h"
#include "obj_namesP.h"
#include "utilsP.h"


STRING	nlstr = "\n";
STRING	nlstr2 = "\n\n";

static int	find_base_file_name(STRING path, int *startPtr, int *lenPtr);

/*
 * Miscellaneous utility functions.
 */

/*
 * Return the printed representation of a boolean.
 */
STRING
abmfP_boolean_string(int i)
{
    return i ? "True" : "False";
}


/*
 * Check a string to see if contains a legal identifier.
 */
int
abmfP_identifier_ok(STRING s)
{
    return ab_c_ident_is_ok(s);
}


/*
 * Capitalize the first letter of the given string.
 */
STRING
abmfP_capitalize_first_char(STRING str)
{
    static char         name[MAXPATHLEN] = "";

    if (str == NULL)
	return str;

    if (isupper(str[0]))
	return str;

    if (str != name)	/* if we get called w/our own buffer, don't copy */
    {
        snprintf(name, sizeof(name), "%s", str);
    }
    name[0] = toupper(str[0]);

    return name;
}


/*
 * Makes the 1st char lower case
 */
STRING
abmfP_uncapitalize_first_char(STRING str)
{
    static char         name[MAXPATHLEN] = "";

    if (str == NULL)
	return str;

    if (islower(str[0]))
	return str;

    if (str != name)	/* if we get called w/our own buffer, don't copy */
    {
        snprintf(name, sizeof(name), "%s", str);
    }
    name[0] = tolower(str[0]);

    return name;
}


ABObj
abmfP_obj_find_codegen_parent(ABObj obj)
{
    ABObj	cgParent = obj->parent;
    while ((cgParent != NULL) && obj_has_flag(cgParent, NoCodeGenFlag))
    {
	cgParent = cgParent->parent;
    }
    return cgParent;
}

/*
 * Return TRUE if all items have the same Widget Class
 */
BOOL
abmfP_items_are_homogeneous(
    ABObj	obj
)
{
    AB_TRAVERSAL        trav;
    ABObj               item;
    BOOL		same = TRUE;
    STRING		ref_classname;

    trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
    item = trav_next(&trav);
    if (item != NULL)
    {
    	ref_classname = obj_get_class_name(item);
    	for (;(item = trav_next(&trav)) != NULL; )
	    if (util_strcmp(ref_classname, obj_get_class_name(item)) != 0)
            {
		same = FALSE;
            	break;
	    }
    }
    trav_close(&trav);
    return same;
}

BOOL
abmfP_items_have_mnemonics(
    ABObj	obj
)
{
    AB_TRAVERSAL	trav;
    ABObj		item; 
    BOOL		has_mnemonics = False;

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
        (item = trav_next(&trav)) != NULL;)
	if (obj_get_mnemonic(item) != NULL)
	{
	    has_mnemonics = True;
	    break;
	}
    trav_close(&trav);
    return has_mnemonics;
}

BOOL
abmfP_items_have_accelerators(
    ABObj	obj
)
{
    AB_TRAVERSAL        trav; 
    ABObj		item;
    BOOL		has_accel = False;
 
    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ); 
        (item = trav_next(&trav)) != NULL;)
        if (obj_get_accelerator(item) != NULL)
        { 
            has_accel = True; 
            break; 
        } 
    trav_close(&trav); 
    return has_accel;
}


/*
 * Returns TRUE if the objects represent the same UI element.
 * 
 * This shouldn't really be necessary - there should be NO duplicate objects,
 * just multiple references to the same object.
 */
int
abmfP_obj_is_duplicate(ABObj obj, ABObj dupObj)
{
    return (obj == dupObj);
}


BOOL
abmfP_parent_is_class(ABObj obj, STRING classname)
{
    if ((abmfP_parent(obj) == NULL)
	|| (obj_get_class_name(obj) == NULL)
	|| (obj_get_class_name(abmfP_parent(obj)) == NULL))
    {
	return FALSE;
    }
    if (util_streq(obj_get_class_name(abmfP_parent(obj)), classname))
    {
	return TRUE;
    }
    if (abmfP_parent(obj))
	return abmfP_parent_is_class(abmfP_parent(obj), classname);
    return FALSE;
}


/*
 * Return the actual control object.
 * 
 * Since most controls have been converted to multiple toolkit objects (e.g., to
 * add captions) except oblong, menu buttons, get the object that actually
 * implements the type.
 * 
 * The opposite of this is abmfP_get_container_object.
 */
ABObj
abmfP_get_actual_control(ABObj parent)
{
    ABObj               obj = objxm_comp_get_subobj(parent, AB_CFG_OBJECT_OBJ);
    return obj;
}				/* abmfP_get_actual_control */


/*
 * Gets the object that is or contains all of the subobjects that make up a
 * particular type of object.
 * 
 * For instance, if a gauge is represented by a container that has a label and a
 * gauge, this function will return the container object if any of the
 * subobjects is passed in (or if the container itself is passed in).  This
 * function works on any type of object.
 * 
 * The opposite function is abmfP_get_actual_control
 */
ABObj
abmfP_get_whole_object(ABObj obj)
{
    return obj_get_root(obj);
}


/*
 * Returns the object that the callback has been "associated with."
 * Normally the module it is declare in, or the project for cross-module
 * conn's.
 *
 * Returns NULL if the callback is not found.
 */
ABObj
abmfP_find_callback_scope(ABObj obj, STRING callbackName)
{
    ABObj	scopeObj = NULL;
    ABObj 	objModule = obj_get_module(obj);
    ABObj	objProject= obj_get_project(obj);
    StringList	callbackList = NULL;
    AB_TRAVERSAL	trav;
    ABObj	curModule= NULL;

    if ((objModule != NULL) && (mfobj_get_module_data(objModule) != NULL))
    {
	callbackList = mfobj_get_module_data(objModule)->callbacks;
	if (strlist_str_exists(callbackList, callbackName))
	{
	    scopeObj = objModule;
	}
    }

    if ((scopeObj == NULL) && (objProject != NULL))
    {
	for (trav_open(&trav, objProject, AB_TRAV_MODULES);
	    (scopeObj == NULL) && ((curModule = trav_next(&trav)) != NULL); )
	{
	    if (mfobj_get_module_data(curModule) != NULL)
	    {
	        callbackList = mfobj_get_module_data(curModule)->callbacks;
	        if (strlist_str_exists(callbackList, callbackName))
	        {
		    scopeObj = curModule;
	        }
	    }
	}
	trav_close(&trav);

	if (scopeObj == NULL)
	{
	   callbackList = mfobj_get_proj_data(objProject)->callbacks;
	   if (strlist_str_exists(callbackList, callbackName))
	   {
	       scopeObj = NULL;
	   }
	}
    }

    return scopeObj;
}

int
find_longest_label(STRING * label)
{
    int                 len1,
                        len2,
                        longest = 0,
                        i = 0;

    len1 = len2 = 0;
    for (; label[i]; i++)
    {
	len1 = strlen(label[i]);
	if (len1 > len2)
	{
	    len2 = len1;
	    longest = i;
	}
    }
    return longest;
}

/*************************************************************************
 **									**
 **	MEMORY ALLOCATION						**
 **									**
 *************************************************************************/

/*
 * Version of calloc() with error checking.
 */
void               *
abmfP_calloc(size_t n, size_t size)
{
    char               *p;

    if (p = (STRING) calloc(n, size))
	return p;
    util_error(catgets(Dtb_project_catd, 1, 83, "calloc: out of memory"));
    return NULL;
}

/*
 * Version of malloc() with error checking.
 */
void               *
abmfP_malloc(size_t size)
{
    void               *p;

    if (p = malloc(size))
	return p;
    util_error(catgets(Dtb_project_catd, 1, 84, "malloc: out of memory"));
    return NULL;
}

/*
 * Version of realloc() with error checking.
 */
void               *
abmfP_realloc(void *buf, unsigned int size)
{
    void               *p;

    if (p = realloc(buf, size))
	return p;
    util_error(catgets(Dtb_project_catd, 1, 85, "realloc: out of memory"));
    return NULL;
}


/*
 * Return the window parent of an object.
 */
ABObj
abmfP_get_window_parent(ABObj obj)
{
    if (obj_is_top_level(obj))
	return NULL;
    else if (abmfP_parent(obj) && obj_is_top_level(abmfP_parent(obj)))
	return abmfP_parent(obj);
    else if (abmfP_parent(obj))
	return abmfP_get_window_parent(abmfP_parent(obj));
    return ((ABObj) NULL);
}


/*
 * Return the window pane of an object.
 */
ABObj
abmfP_get_window_pane(ABObj obj)
{
    if (abmfP_parent(obj) == NULL)
    {
	return ((ABObj) NULL);
    }
    if (obj_is_top_level(obj))
	return NULL;
    else if (obj_is_pane(abmfP_parent(obj)) || obj_is_top_level(abmfP_parent(obj)))
	return abmfP_parent(obj);

    return abmfP_get_window_pane(abmfP_parent(obj));
}


/*
 * Copy one file to another.
 */
int
abmfP_filecopy(STRING file1, STRING file2)
{
    FILE               *inp;
    FILE               *outp;
    int                 c;

    if ((inp = util_fopen_locked(file1, "r")) == NULL ||
	(outp = util_fopen_locked(file2, "w")) == NULL)
	return ERROR;

    while ((c = fgetc(inp)) != EOF)
	fputc(c, outp);

    fclose(inp);
    fclose(outp);
    return TRUE;
}

/*
 * assigns names to object without them. This is a safty measure.
 */
int
abmfP_assign_widget_names(ABObj root)
{
    int                 iReturn = 0;
    ABObj               win_obj;
    static int          i = 0;
    char                Buf[64];
    AB_TRAVERSAL        trav;

    /*
     * Base windows
     */
    for (trav_open(&trav, root, AB_TRAV_WINDOWS), i = 1;
	 (win_obj = trav_next(&trav)) != NULL;)
    {
	if (obj_get_name(win_obj) == NULL)
	{			/* Only assign once */
	    sprintf(Buf, "%s%d", "widget", i);
	    obj_set_name(win_obj, Buf);
	}
	i++;
    }
    trav_close(&trav);
    return iReturn;
}


abmfP_tree_set_written(ABObj root, BOOL written)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    for (trav_open(&trav, root, AB_TRAV_ALL);
	 (obj = trav_next(&trav)) != NULL;)
    {
	obj_set_was_written(obj, written);
    }
    trav_close(&trav);
    return 0;
}


abmfP_obj_set_items_written(ABObj rootObj, BOOL written)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    for (trav_open(&trav, rootObj, AB_TRAV_ITEMS_FOR_OBJ);
	 (obj = trav_next(&trav)) != NULL;)
    {
	obj_set_was_written(obj, written);
    }
    trav_close(&trav);

    return 0;
}


/*
 * gets the #define string for the file
 */
STRING
abmfP_get_define_from_file_name(STRING fileName)
{
    static char	define[MAX_NAME_SIZE];
    int		fileNameLen= strlen(fileName);
    int		iFileName= 0;
    int		iDefine= 0;
    char	fileNameChar= 0;

    strcpy(define, "_");
    iDefine= 1;
    for (iFileName= 0; iFileName < fileNameLen; ++iFileName)
    {
	fileNameChar= fileName[iFileName];
	if (isalnum(fileNameChar))
	{
	    define[iDefine++]= toupper(fileNameChar);
	}
	else
	{
	    if (define[iDefine-1] != '_')
	    {
		define[iDefine++]= '_';
	    }
	}
    }
    if ((iDefine > 0) && (define[iDefine-1] != '_'))
    {
	define[iDefine++]= '_';
    }
    define[iDefine]= 0;
    return define;
}


BOOL
abmfP_file_name_is_bitmap(STRING fileName)
{
    return util_file_name_has_extension(fileName, "xbm");
}


BOOL
abmfP_file_name_is_xpm(STRING fileName)
{
    return util_file_name_has_extension(fileName, "pm");
}


STRING
abmfP_cvt_file_name_to_bitmap_data_vars(
		STRING	path,
		STRING	*widthVarPtr,
		STRING	*heightVarPtr,
		STRING	*bitsVarPtr
)
{
#define widthVar (*widthVarPtr)
#define heightVar (*heightVarPtr)
#define bitsVar (*bitsVarPtr)
    static char widthVarBuf[256];
    static char heightVarBuf[256];
    static char bitsVarBuf[256];
    int		baseIndex = -1;
    int		baseLen = -1;
    char	*nameStart = NULL;

    widthVar = heightVar = bitsVar = NULL;

    find_base_file_name(path, &baseIndex, &baseLen);
    nameStart = &(path[baseIndex]);

    strncpy(widthVarBuf, nameStart, baseLen);
    widthVarBuf[baseLen]= 0;
    strcpy(heightVarBuf, widthVarBuf);
    strcpy(bitsVarBuf, widthVarBuf);

    strcat(widthVarBuf, "_width");
    strcpy(widthVarBuf, ab_make_valid_c_ident(widthVarBuf));

    strcat(heightVarBuf, "_height");
    strcpy(heightVarBuf, ab_make_valid_c_ident(heightVarBuf));

    strcat(bitsVarBuf, "_bits");
    strcpy(bitsVarBuf, ab_make_valid_c_ident(bitsVarBuf));

    widthVar = widthVarBuf;
    heightVar = heightVarBuf;
    bitsVar = bitsVarBuf;

    return 0;
#undef widthVar
#undef heightVar
#undef bitsVar
}


STRING
abmfP_cvt_file_name_to_xpm_data_var(STRING path)
{
    static char	varName[256];
    int		baseStart = -1;
    int		baseLen = -1;

    *varName = 0;
    find_base_file_name(path, &baseStart, &baseLen);
    if (baseLen > 0)
    {
        strncpy(varName, path+baseStart, baseLen);
        varName[baseLen]= 0;
        strcat(varName, "_pm");
    }
    return ab_make_valid_c_ident(varName);
}


int
abmfP_obj_set_subobjs_written(ABObj obj, BOOL written)
{
    AB_TRAVERSAL	trav;
    ABObj		subObj= NULL;

    for (trav_open(&trav, obj, AB_TRAV_ALL);
	(subObj= trav_next(&trav)) != NULL; )
    {
	if (   (subObj == obj)
	    || (obj_get_root(subObj) != obj))
	{
	    continue;
	}
	obj_set_was_written(subObj, written);
    }
    trav_close(&trav);
    return 0;
}


BOOL
abmfP_objs_in_same_struct(ABObj obj1, ABObj obj2)
{
    ABObj	structObj1= abmfP_obj_get_struct_obj(obj1);
    ABObj	structObj2= abmfP_obj_get_struct_obj(obj2);
    return ((structObj1 != NULL) && (structObj1 == structObj2));
}


BOOL
abmfP_objs_in_same_substruct(ABObj obj1, ABObj obj2)
{
   ABObj	structObj1 = abmfP_obj_get_substruct_obj(obj1);
   ABObj	structObj2 = abmfP_obj_get_substruct_obj(obj2);
   return ((structObj1 != NULL) && (structObj1 == structObj2));
}


/*
 * We have two types of structures:
 *	- one structure for each window
 *	- one structure for all the menus in an interface
 */
ABObj
abmfP_obj_get_struct_obj(ABObj obj)
{
    ABObj	structObj = NULL;

    if (mfobj_has_flags(obj, CGenFlagStructObjValid))
    {
	goto epilogue;
    }

    for (structObj = obj; 
	 structObj != NULL && !abmfP_obj_is_struct_obj(structObj); 
	 structObj = abmfP_parent(structObj)
	) ;

    /*
     * Save the data
     */
    mfobj_set_flags(obj, CGenFlagStructObjValid);
    mfobj_data(obj)->struct_obj = structObj;

epilogue:
    return mfobj_data(obj)->struct_obj;
}


/*
 * Currently, only items go in substructs. List items don't however,
 * because they don't cause widgets to be created.
 */
ABObj
abmfP_obj_get_substruct_obj(ABObj obj)
{
    ABObj	substructObj = NULL;

    if (mfobj_has_flags(obj, CGenFlagSubstructObjValid))
    {
	goto epilogue;
    }

    if (obj_is_item(obj))
    {
	ABObj	parent = abmfP_parent(obj);
	if (parent != NULL)
	{
	    substructObj = obj_get_root(parent);
	}
	if (   (substructObj != NULL) 
	    && (!abmfP_obj_is_substruct_obj(substructObj)) )
	{
	    substructObj = NULL;
	}
    }

    /*
     * Save the data
     */
    mfobj_set_flags(obj, CGenFlagSubstructObjValid);
    mfobj_data(obj)->substruct_obj = substructObj;

epilogue:
    return mfobj_data(obj)->substruct_obj;
}


int
abmfP_get_num_substruct_fields(ABObj obj)
{
    int		numFields = 0;
    ABObj	substruct = abmfP_obj_get_substruct_obj(obj);
    AB_TRAVERSAL	trav;
    ABObj		subobj = NULL;

    /*
     * Don't
     */
    for (trav_open(&trav, substruct, AB_TRAV_UI);
	(subobj = trav_next(&trav)) != NULL; )
    {
	if (abmfP_obj_has_substruct_field(subobj))
	{
	    if (subobj == obj)
	    {
		++numFields;
	    }
	    else if (abmfP_obj_get_substruct_obj(subobj) == obj)
	    {
	        ++numFields;
	    }
	}
    }
    trav_close(&trav);

    return numFields;
}

STRING          
abmfP_convert_wclass_ptr_to_name(WidgetClass wclass)
{
    STRING	wclass_name= NULL;

    if (wclass == xmCascadeButtonWidgetClass)
    {
	wclass_name= istr_string(abmfP_xmCascadeButton);
    }
    else if (wclass == xmScrolledWindowWidgetClass)
    {
	wclass_name= istr_string(abmfP_xmScrolledWindow);
    }
    else if (wclass == xmToggleButtonWidgetClass)
    {
	wclass_name= istr_string(abmfP_xmToggleButton);
    }

    return wclass_name;
}


/*
 * Finds the primary window for the application.
 * Finds the base window whose name is alphabetically first
 */
ABObj
abmfP_get_root_window(ABObj project)
{
    static ABObj	root_window= NULL;
    AB_TRAVERSAL	trav;
    ABObj		window= NULL;

    if (root_window != NULL)
    {
	return root_window;
    }

    if (obj_get_root_window(project) == NULL)
    {
        for (trav_open(&trav, project, AB_TRAV_WINDOWS);
	    (window= trav_next(&trav)) != NULL; )
        {
	    if (!obj_is_base_win(window))
	    {
	        continue;
	    }
	    if (root_window == NULL)
	    {
	        root_window = window;
	    }
	    else
	    {
		if (   (!obj_is_initially_visible(root_window))
		    && (obj_is_initially_visible(window)) )
		{
		    root_window = window;
		}
		else if (util_strcmp(
		    obj_get_name(window), obj_get_name(root_window)) < 0)
	        {
		    root_window= window;
	        }
	    }
        }
        trav_close(&trav);
    } /* root_window == NULL */

    if (obj_get_root_window(project) != NULL)
    {
	root_window= obj_get_root_window(project);
    }
    
    return root_window;
}


/* 
 * Essentially, all salient UI objects have create procs.
 *
 * The exceptions are:
 *	- MENU references not salient, but have create procs
 *	- ITEMs are salient, but do not have create procs
 *		(they are created in their parent's proc)
 */
BOOL
abmfP_obj_has_create_proc(ABObj obj)
{
    if (obj_is_menu(obj))
    {
	/*
	 * menus are considered virtual, in that the "menu" doesn't
	 * actually exist - the menu references do.  We write a create
	 * proc for each menu reference.
	 */
	return (obj_is_ref(obj) && (abmfP_obj_get_struct_obj(obj) != NULL));
    }
    if (obj_is_virtual(obj))
    {
	/* virtual objects never get written */
	return FALSE;
    }
    if (obj_is_item(obj))
    {
	return FALSE;
    }
    
    return obj_is_salient_ui(obj);
}


/*
 * create procs take a pointer to the struct we're going to fill in.
 */
STRING
abmfP_obj_get_create_proc_instance_param_type(ABObj obj)
{
    ABObj	structObj = NULL;

    structObj = abmfP_obj_get_struct_obj(obj);
    if (structObj == NULL)
    {
	return "Widget *";
    }

    return abmfP_get_c_struct_ptr_type_name(structObj);
}


BOOL
abmfP_obj_create_proc_has_instance_param(ABObj obj)
{
    return TRUE;
}


BOOL
abmfP_obj_create_proc_has_name_param(ABObj obj)
{
    return FALSE;
}


/* 
 * - topmost object in structure requires a parent from client
 * - menus need parents because, if they are shared, each instance
 *   will have a different parent
 */
BOOL
abmfP_obj_create_proc_has_parent_param(ABObj obj)
{
    return TRUE;
}


int
abmfP_obj_get_num_create_proc_return_widgets(ABObj obj)
{
    int		numWidgets = 0;

    numWidgets = 0;

    return numWidgets;
}


/*
 * which_param is 0..n-1 params
 */
STRING
abmfP_obj_get_create_proc_return_widget_name(ABObj obj, int which_param)
{
    STRING	paramName = NULL;

    if (which_param > 0)
    {
	return NULL;
    }

    if (abmfP_obj_is_substruct_obj(obj))
    {
	return abmfP_get_c_substruct_ptr_type_name(obj);
    }
    else if (abmfP_obj_is_struct_obj(obj))
    {
	return abmfP_get_c_struct_ptr_type_name(obj);
    }

    return paramName;
}


BOOL
abmfP_obj_create_proc_has_submenu_params(ABObj obj)
{
    return FALSE;
    /* return (abmfP_obj_has_menu(obj) || abmfP_obj_has_item_with_menu(obj)); */
}


int
abmfP_get_num_cp_submenu_params(ABObj obj)
{
    return 0;
    /*
    AB_TRAVERSAL	trav;
    ABObj		item;
    int			numSubmenus = 0;

    if (abmfP_obj_has_menu(obj))
    {
	++numSubmenus;
    }

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	(item = trav_next(&trav)) != NULL; )
    {
	if (abmfP_obj_has_menu(item))
	{
	    ++numSubmenus;
	}
    }
    trav_close(&trav);

    return numSubmenus;
    */
}


STRING
abmfP_get_cp_submenu_param_name(ABObj obj, int whichParam)
{
    ABObj	paramObj = abmfP_get_cp_submenu_param_obj(obj, whichParam);
    if (paramObj == NULL)
    {
	return NULL;
    }
    return abmfP_get_c_field_name(paramObj);
}


STRING
abmfP_get_cp_submenu_param_type_name(ABObj obj, int whichParam)
{
    return abmfP_str_widget;
}


ABObj
abmfP_get_cp_submenu_param_obj(ABObj obj, int whichParam)
{
    int			curParam = -1;
    ABObj		menu = NULL;
    ABObj		paramObj = NULL;
    AB_TRAVERSAL	trav;
    ABObj		item = NULL;

    if ((menu = obj_get_menu(obj)) != NULL)
    {
	++curParam;
	if (curParam == whichParam)
	{
	    paramObj = menu;
	    goto epilogue;
	}
    }

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	(item = trav_next(&trav)) != NULL; )
    {
	if ((menu = obj_get_menu(item)) != NULL)
	{
	    ++curParam;
	    if (curParam == whichParam)
	    {
		paramObj = menu;
		break;
	    }
	}
    }
    trav_close(&trav);

epilogue:
    return paramObj;
}


BOOL
abmfP_obj_has_menu(ABObj obj)
{
    return (obj_get_menu(obj) != NULL);
}


BOOL
abmfP_obj_has_item_with_menu(ABObj obj)
{
    BOOL		itemHasMenu = FALSE;
    AB_TRAVERSAL	trav;
    ABObj		item;

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	(item = trav_next(&trav)) != NULL; )
    {
	if (abmfP_obj_has_menu(item))
	{
	    itemHasMenu = TRUE;
	    break;
	}
    }
    trav_close(&trav);

    return itemHasMenu;
}


BOOL
abmfP_module_has_menu_struct(ABObj obj)
{
    static ABObj	lastModule = NULL;
    static BOOL		lastModuleHadMenuStruct = FALSE;
    ABObj		module = obj_get_module(obj);
    BOOL		hasMenuStruct = FALSE;

    if (module == lastModule)
    {
	hasMenuStruct = lastModuleHadMenuStruct;
    }
    else
    {
	hasMenuStruct = (trav_count(module, AB_TRAV_MENUS) > 0);
	lastModule = module;
	lastModuleHadMenuStruct = hasMenuStruct;
    }

    return hasMenuStruct;
}



BOOL
abmfP_obj_has_glyph_label(ABObj obj)
{
    return (   (obj_get_label(obj) != NULL)
	    && (obj_get_label_type(obj) == AB_LABEL_GLYPH));
}


BOOL
abmfP_obj_has_item_with_glyph_label(ABObj obj)
{
    AB_TRAVERSAL	trav;
    ABObj		item = NULL;
    BOOL		doesIt = FALSE;

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	(item = trav_next(&trav)) != NULL; )
    {
	if (abmfP_obj_has_glyph_label(item))
	{
	    doesIt = TRUE;
	    break;
	}
    }
    trav_close(&trav);

    return doesIt;
}


BOOL
abmfP_obj_has_string_label(ABObj obj)
{
    return (   (obj_get_label(obj) != NULL)
	    && (obj_get_label_type(obj) == AB_LABEL_STRING));
}


BOOL
abmfP_obj_has_item_with_string_label(ABObj obj)
{
    AB_TRAVERSAL	trav;
    ABObj		item = NULL;
    BOOL		doesIt = FALSE;

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	(item = trav_next(&trav)) != NULL; )
    {
	if (abmfP_obj_has_string_label(item))
	{
	    doesIt = TRUE;
	    break;
	}
    }
    trav_close(&trav);

    return doesIt;
}

BOOL
abmfP_proj_needs_session_save(ABObj proj)
{
    AB_SESSIONING_METHOD	ss_mthd;

    if (!proj)
	return (FALSE);

    if (!obj_is_project(proj))
    {
	proj = obj_get_project(proj);

	if (!proj || !obj_is_project(proj))
	    return (FALSE);
    }

    ss_mthd = obj_get_sessioning_method(proj);

    switch (ss_mthd)  {
    case AB_SESSIONING_UNDEF:
    case AB_SESSIONING_NONE:
	return (FALSE);
    case AB_SESSIONING_CMDLINE:
    case AB_SESSIONING_FILE:
    case AB_SESSIONING_CMDLINE_AND_FILE:
	return (TRUE);
    default:
	return (FALSE);
    }

}


BOOL
abmfP_proj_needs_session_restore(ABObj proj)
{
    AB_SESSIONING_METHOD	ss_mthd;

    if (!proj)
	return (FALSE);

    if (!obj_is_project(proj))
    {
	proj = obj_get_project(proj);

	if (!proj || !obj_is_project(proj))
	    return (FALSE);
    }

    ss_mthd = obj_get_sessioning_method(proj);

    switch (ss_mthd)  {
    case AB_SESSIONING_UNDEF:
    case AB_SESSIONING_NONE:
    case AB_SESSIONING_CMDLINE:
	return (FALSE);
    case AB_SESSIONING_FILE:
    case AB_SESSIONING_CMDLINE_AND_FILE:
	return (TRUE);
    default:
	return (FALSE);
    }
}

BOOL
abmfP_obj_needs_centering_handler(ABObj obj)
{
    AB_ATTACH_TYPE	left_attach_type,
			top_attach_type;

    if (!obj)
	return (FALSE);

    left_attach_type = obj_get_attach_type(obj, AB_CP_WEST);
    top_attach_type = obj_get_attach_type(obj, AB_CP_NORTH);

    if ((left_attach_type == AB_ATTACH_CENTER_GRIDLINE) || 
	(top_attach_type == AB_ATTACH_CENTER_GRIDLINE))
	return (TRUE);
    else
	return (FALSE);
}

STRING
abmfP_obj_get_centering_type(ABObj obj)
{
    AB_ATTACH_TYPE	left_attach_type,
			top_attach_type;

    if (!obj || !abmfP_obj_needs_centering_handler(obj))
	return("DTB_CENTER_NONE");
    
    left_attach_type = obj_get_attach_type(obj, AB_CP_WEST);
    top_attach_type = obj_get_attach_type(obj, AB_CP_NORTH);

    if ((left_attach_type == AB_ATTACH_CENTER_GRIDLINE) && 
	(top_attach_type != AB_ATTACH_CENTER_GRIDLINE))
	return("DTB_CENTER_POSITION_VERT");

    if ((left_attach_type != AB_ATTACH_CENTER_GRIDLINE) && 
	(top_attach_type == AB_ATTACH_CENTER_GRIDLINE))
	return("DTB_CENTER_POSITION_HORIZ");

    if ((left_attach_type == AB_ATTACH_CENTER_GRIDLINE) && 
	(top_attach_type == AB_ATTACH_CENTER_GRIDLINE))
	return("DTB_CENTER_POSITION_BOTH");

    return("DTB_CENTER_NONE");
}

BOOL
abmfP_obj_needs_align_handler(ABObj obj)
{
    AB_GROUP_TYPE	type;

    if (!obj || !obj_is_group(obj))
	return (FALSE);

    type = obj_get_group_type(obj);

    if (type != AB_GROUP_IGNORE)
	return (TRUE);
    else
	return (FALSE);
}

STRING
abmfP_obj_get_group_type(ABObj obj)
{

    AB_GROUP_TYPE	type;

    if (!obj || !abmfP_obj_needs_align_handler(obj))
	return("DTB_GROUP_NONE");
    
    type = obj_get_group_type(obj);

    switch (type)
    {
	case AB_GROUP_IGNORE:
	    return("DTB_GROUP_NONE");
	case AB_GROUP_ROWS:
	    return("DTB_GROUP_ROWS");
	case AB_GROUP_COLUMNS:
	    return("DTB_GROUP_COLUMNS");
	case AB_GROUP_ROWSCOLUMNS:
	    return("DTB_GROUP_ROWSCOLUMNS");
    }

    return("DTB_GROUP_NONE");
}

STRING
abmfP_obj_get_row_align_type(ABObj obj)
{
    AB_GROUP_TYPE	group_type;
    AB_ALIGNMENT	align_type;

    if (!obj)
	return("DTB_ALIGN_NONE");
    
    group_type = obj_get_group_type(obj);

    if ((group_type == AB_GROUP_COLUMNS) || (group_type == AB_GROUP_IGNORE))
	return("DTB_ALIGN_NONE");

    align_type = obj_get_row_align(obj);

    switch (align_type)
    {
	case AB_ALIGN_TOP:
	    return("DTB_ALIGN_TOP");
	case AB_ALIGN_HCENTER:
	    return("DTB_ALIGN_HCENTER");
	case AB_ALIGN_BOTTOM:
	    return("DTB_ALIGN_BOTTOM");
	default:
	    return("DTB_ALIGN_NONE");
    }
}

STRING
abmfP_obj_get_col_align_type(ABObj obj)
{
    AB_GROUP_TYPE	group_type;
    AB_ALIGNMENT	align_type;

    if (!obj)
	return("DTB_ALIGN_NONE");
    
    group_type = obj_get_group_type(obj);

    if ((group_type == AB_GROUP_ROWS) || (group_type == AB_GROUP_IGNORE))
	return("DTB_ALIGN_NONE");

    align_type = obj_get_col_align(obj);

    switch (align_type)
    {
	case AB_ALIGN_LEFT:
	    return("DTB_ALIGN_LEFT");
	case AB_ALIGN_LABELS:
	    return("DTB_ALIGN_LABELS");
	case AB_ALIGN_VCENTER:
	    return("DTB_ALIGN_VCENTER");
	case AB_ALIGN_RIGHT:
	    return("DTB_ALIGN_RIGHT");
	default:
	    return("DTB_ALIGN_NONE");
    }
}


/*
 * Returns the starting index and length of the base file name
 * sans leading path and trailing extension.
 *
 */
static int
find_base_file_name(STRING path, int *startPtr, int *lenPtr)
{
#define start (*startPtr)
#define len (*lenPtr)
    char	*slashPtr = NULL;
    char	*dotPtr = NULL;
    int		slashChar = -1;
    int		dotChar = -1;

    slashPtr = strrchr(path, '/');
    if (slashPtr != NULL)
    {
	/*
	 * handle strings of the form subdir//x.ext
	 */
	while (((*slashPtr) == '/') && ((*slashPtr) != 0))
	{
	    ++slashPtr;
	}
	--slashPtr;		/* point to last slash */
	slashChar = *slashPtr;
	*slashPtr = 0;
    }

    start = 0;
    if (slashPtr != NULL)
    {
	start = ((int)(slashPtr - path)) + 1;
    }

    dotPtr = strrchr(&(path[start]), '.');
    if (dotPtr != NULL)
    {
	dotChar = *dotPtr;
	*dotPtr = 0;
    }

    len = strlen(&path[start]);

    /*
     * Restore the string
     */
    if (slashPtr != NULL)
    {
	*slashPtr = (char)slashChar;
    }
    if (dotPtr != NULL)
    {
	*dotPtr = (char)dotChar;
    }

    return 0;
#undef start
#undef len
}

BOOL
abmfP_proj_has_message(ABObj proj)
{
    ABObj       	obj = (ABObj) NULL;
    AB_TRAVERSAL	trav;
    int			i = 0;
    BOOL		hasMsg = FALSE;

    if (!proj)
        return (FALSE);

    if (!obj_is_project(proj))
    {
        proj = obj_get_project(proj);

        if (!proj || !obj_is_project(proj))
            return (FALSE);
    }

    for (trav_open(&trav, proj, AB_TRAV_SALIENT_UI); 
	((obj = trav_next(&trav)) != NULL) && !hasMsg; )
    {
	if (obj_is_message(obj))
	  hasMsg = TRUE;
    }

    return (hasMsg);
}


/*
** 4/30/95 bfreeman - Temporary. This is obj_get_refs_to's territory! But, I
** wasn't able to get the refs to menu items out of obj_get_refs_to()...the
** objects just aren't there.
**
** Returns an ABObjList containing all of the references to obj
**
** Example use:
**
**    ABObjList refList = abmfP_find_refs_to(obj);
**    ABObj      refObj = NULL;
**    int       numRefs = objlist_get_num_objs(refList);
**    int             i = 0;
**
**    if ((refList != NULL) && (numRefs > 0))
**    {
**	for (i = 0; i < numRefs; ++i)
**	{
**	    refObj = objlist_get_obj(refList, i, NULL);
**
**	    --- use refObj, it refers to obj ---
**		    
**	}
**    }
**    objlist_destroy(refList);
**
**
** Note: May return NULL and it is the responsibility of the caller to
**       destroy the ABObjList.
*/
ABObjList
abmfP_find_refs_to(ABObj obj)
{
    AB_TRAVERSAL  moduleTrav;
    ABObjList     refList = NULL;
    ABObj         refObj  = NULL;
    
    if (obj == NULL)
	return NULL;

    refList = objlist_create();
    
    for (trav_open(&moduleTrav, obj_get_module(obj), AB_TRAV_UI);
	 ((refObj = trav_next(&moduleTrav)) != NULL); )
    {
	if (obj_is_ref(refObj) && (refObj->ref_to == obj))
	{
	    objlist_add_obj(refList, refObj, NULL);
	}
    }
    trav_close(&moduleTrav);

    return refList;
}


