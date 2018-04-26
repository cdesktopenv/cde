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
 * $XConsortium: obj_names.c /main/3 1995/11/06 18:11:04 rswiston $
 * 
 * @(#)obj_names.c	1.17 15 Feb 1994	cde_app_builder/src/abmf
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
 * obj_names.c - gets various names for objects (C/Widget/Global/Local/...)
 */

#include <ctype.h>
#include <string.h>
#include <ab_private/objxm.h>
#include "utilsP.h"
#include "cdefsP.h"
#include "lib_func_stringsP.h"
#include "instancesP.h"
#include "obj_namesP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/
static char *const typePrefixString = "";
static char *const identPrefixString = "dtb_";

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

static STRING abmfP_build_instance_prefix(ABObj obj, 
			STRING prefixBuf, int prefixBufSize);

static int	ensure_unique_comp_field_names(ABObj obj);

static STRING	get_ident_for_widget_class(ABObj obj);
static STRING	cvt_ident_to_type(STRING ident);
static STRING	cvt_type_to_ident(
			STRING type, STRING identBuf, int identBufSize);

#ifdef DEBUG

static int	assert_c_ident_ok(STRING ident);

#define return_c_ident(ident) \
    assert_c_ident_ok(ident); \
    return (ident)

#else

#define return_c_ident(ident) \
    return (ident)

#endif /* DEBUG */

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

/*************************************************************************
 *************************************************************************
 **									**
 **		Object Names 						**
 **									**
 *************************************************************************
 *************************************************************************/

/*
 * Gets the global variable reference to the object
 */
STRING
abmfP_get_c_name_global(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	structObj = NULL;
    ABObj	substructObj = NULL;
    STRING	structName= NULL;
    STRING	substructName = NULL;
    STRING	fieldName= NULL;

    substructObj = abmfP_obj_get_substruct_obj(obj);
    if (substructObj == NULL)
    {
	structObj = abmfP_obj_get_struct_obj(obj);
    }
    else
    {
	structObj = abmfP_obj_get_struct_obj(substructObj);
    }

    if (structObj == NULL)
    {
	return NULL;
    }
    structName = abmfP_get_c_struct_global_name(structObj);
    if (substructObj != NULL)
    {
	substructName = abmfP_get_c_substruct_field_name(substructObj);
    }
    fieldName = abmfP_get_c_field_name(obj);

    if (substructName != NULL)
    {
	snprintf(name, sizeof(name), "%s.%s.%s", structName, substructName, fieldName);
    } else {
	snprintf(name, sizeof(name), "%s.%s", structName, fieldName);
    }
    return name;
}


/*
 * Gets the name of the field in the structure that represents this obj
 */
STRING
abmfP_get_c_field_name(ABObj obj)
{
    char	fieldNameBuf[MAXPATHLEN+1];
    STRING	fieldName = NULL;
    ABObj	substructObj = NULL;
    ABObj	compRoot = NULL;
    STRING	compRootName = NULL;
    *fieldNameBuf = 0;

    if (mfobj_data(obj)->field_name != NULL)
    {
	goto epilogue;
    }

    /*
     * If this obj is a reference to another obj, then its field name is
     * actually determined by the obj it references. This must be inside
     * a substruct to prevent naming collisions.
     */
    substructObj = abmfP_obj_get_substruct_obj(obj);
    {
	ABObj	fieldNameObj = obj_get_actual_obj(obj);
        if ((substructObj != NULL) && (fieldNameObj != obj))
        {
	    abmfP_get_c_field_name(fieldNameObj);	/* we set field_name */
	    mfobj_data(obj)->field_name = 
	        istr_dup(mfobj_data(fieldNameObj)->field_name);
	        
	    goto epilogue;
	}
    }

    /*
     * We need to make sure all the field names of the containing
     * composite objects are unique. If the name is changed, just return
     * it.
     */
    ensure_unique_comp_field_names(obj);
    if (mfobj_data(obj)->field_name != NULL)
    {
	goto epilogue;
    }

    snprintf(fieldNameBuf, sizeof(fieldNameBuf), "%s", obj_get_name(obj));
    abmfP_uncapitalize_first_char(fieldNameBuf);

    if (substructObj != NULL)
    {
	if ((compRoot = obj_get_root(substructObj)) != NULL)
	{
	    compRootName = obj_get_name(compRoot);
	}
    }

    /* 
     * If this is a field in a substructure, strip of the parent-name
     * prefix, 'cause it's redundant.
     */
    if ((substructObj != NULL) && (compRootName != NULL))
    {
	int	subnamelen = strlen(compRootName);
	int	fieldnamelen = strlen(fieldNameBuf);
	if (   (fieldnamelen > subnamelen)
	    && (strncmp(fieldNameBuf, compRootName, subnamelen) == 0) )
	{
	    fieldnamelen -= subnamelen;
	    while (   (fieldNameBuf[subnamelen] != 0)
		   && (fieldNameBuf[subnamelen] == '_')
		   && (fieldnamelen > 0) )
	    {
		++subnamelen;
		--fieldnamelen;
	    }
	    if (fieldnamelen > 0)
	    {
	        memmove(fieldNameBuf, fieldNameBuf+subnamelen, fieldnamelen+1);
	    }

	    /* we may have created an invalid ident - be sure */
	    fieldName = ab_make_valid_c_ident(fieldNameBuf);
	}
    }
    if (fieldName == NULL)
    {
	fieldName = fieldNameBuf;
    }

    mfobj_data(obj)->field_name = istr_create(fieldName);

epilogue:
    return_c_ident(istr_string(mfobj_data(obj)->field_name));
}


/*
 * Gets the name, relative to a structure instance pointer
 */
STRING
abmfP_get_c_name_in_inst(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	substructObj = NULL;
    STRING	substructName = NULL;
    STRING	fieldName= NULL;

    substructObj = abmfP_obj_get_substruct_obj(obj);
    if (substructObj != NULL)
    {
	substructName = abmfP_get_c_substruct_field_name(substructObj);
    }
    fieldName = abmfP_get_c_field_name(obj);

    if (substructName != NULL)
    {
	snprintf(name, sizeof(name), "%s->%s.%s",
	         abmfP_instance_ptr_var_name,
	         substructName, fieldName);
    } else {
	snprintf(name, sizeof(name), "%s->%s",
	         abmfP_instance_ptr_var_name,
	         fieldName);
    }
    return name;
}


STRING
abmfP_get_c_name_in_subinst(ABObj obj)
{
    static char		name[256];
    STRING		fieldName = abmfP_get_c_field_name(obj);

    if (fieldName == NULL)
    {
	return NULL;
    }

    sprintf(name, "%s->%s",
	abmfP_instance_ptr_var_name, 
	fieldName);
    return name;
}


/*
 * Gets the array of booleans that this objects "selected" value is in.
 */
STRING
abmfP_get_c_array_of_selected_bools_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    sprintf(name, "%s_selected", obj_get_name(abmfP_parent(obj)));
    return_c_ident(name);
}


/*
 * Gets the array of strings this object's string is in (the string
 * is usually the label).
 */
STRING
abmfP_get_c_array_of_strings_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    sprintf(name, "%s_strings", obj_get_name(abmfP_parent(obj)));
    return_c_ident(name);
}

/*
 * Gets the name of the array of widget names.
 */
STRING
abmfP_get_c_array_of_names_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    sprintf(name, "%s_names", obj_get_name(abmfP_parent(obj)));
    return_c_ident(name);
}


/*
 * Gets the array of mnemonics for this object's item's 
 */
STRING
abmfP_get_c_array_of_mnemonics_name(ABObj obj)
{
    static char name[MAX_NAME_SIZE];
    sprintf(name, "%s_mnemonics", obj_get_name(abmfP_parent(obj)));
    return_c_ident(name);
}


/*
 * Gets the name of the array the object's variable lives in
 */
STRING
abmfP_get_c_array_of_widgets_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    sprintf(name, "%s_items", obj_get_name(abmfP_parent(obj)));
    return_c_ident(name);
}


/*
 * Gets the array of xmstrings this object's string is in (the string
 * is usually the label).
 */
STRING
abmfP_get_c_array_of_xmstrings_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    sprintf(name, "%s_xmstrings", obj_get_name(abmfP_parent(obj)));
    return_c_ident(name);
}


/*
 * Gets the name of the local variable that represents the object. May return
 * a local variable, a reference to the local instance pointer, or a global
 * variable, depending on what is available in the function.
 */
STRING
abmfP_get_c_name(GenCodeInfo genCodeInfo, ABObj obj)
{
    static char         nameBuf[MAX_NAME_SIZE];
    STRING		name= NULL;

    if (genCodeInfo->writing_func)
    {
	    ABObj	ipObj= genCodeInfo->cur_func.ip_obj;
	    ABObj	createObj = genCodeInfo->cur_func.create_obj;

	    if (ipObj == NULL)
	    {
		/* no IP structure pointer */
		if (createObj == obj)
		{
		    sprintf(nameBuf, "(*%s)", abmfP_instance_ptr_var_name);
		    name = nameBuf;
		}
	    }
	    else
	    {
		/* we have an IP structure pointer */
		if (createObj != NULL)
		{
		    /* we're in a create proc.  Create procs take struct ptr */
		    if (abmfP_objs_in_same_struct(ipObj, obj))
		    {
		        name = abmfP_get_c_name_in_inst(obj);
		    }
		    else
		    {
			name = abmfP_get_c_name_global(obj);
		    }
		}
		else 
		{
		    /* the ipObj is a structure object */
		    if (abmfP_objs_in_same_struct(ipObj, obj))
	            {
		        /* we can get the variable from the instance pointer */
		        name= abmfP_get_c_name_in_inst(obj);
		    }
	        }
	    }
	    if (name == NULL)
	    {
		/* must use the global name (yuck!) */
		name= abmfP_get_c_name_global(obj);
	    }
    }
    else
    {
	/* not writing function */
	name= abmfP_get_c_name_global(obj);
    }

    return name;
}


/*************************************************************************
 *************************************************************************
 **									**
 **		Structures						**
 **									**
 *************************************************************************
 *************************************************************************/

/*
 *
 */
static STRING
abmfP_get_c_struct_or_ptr_name(
			GenCodeInfo	genCodeInfo,
			ABObj		obj,
			BOOL		wantPtr
)
{
    static char	nameBuf[MAX_NAME_SIZE] = "";
    STRING	name = NULL;
    ABObj	structObj = abmfP_obj_get_struct_obj(obj);

    if (genCodeInfo->writing_func)
    {
	if (genCodeInfo->cur_func.ip_obj == structObj)
	{
	    name = abmfP_instance_ptr_var_name;
	    if (!wantPtr)
	    {
		sprintf(nameBuf, "(*%s)", name);
		name = nameBuf;
	    }
	}
    }
    
    if (name == NULL)
    {
        name = abmfP_get_c_struct_global_name(structObj);
	if (wantPtr)
	{
	    sprintf(nameBuf, "&(%s)", name);
	    name = nameBuf;
	}
    }

    return name;
}


STRING
abmfP_get_c_struct_name(GenCodeInfo genCodeInfo, ABObj obj)
{
    return abmfP_get_c_struct_or_ptr_name(genCodeInfo, obj, FALSE);
}


STRING
abmfP_get_c_struct_ptr_name(GenCodeInfo genCodeInfo, ABObj obj)
{
    return abmfP_get_c_struct_or_ptr_name(genCodeInfo, obj, TRUE);
}


/*
 * Gets the name of the struct type that this object belongs to
 */
STRING
abmfP_get_c_struct_type_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    STRING	ptrName= abmfP_get_c_struct_ptr_type_name(obj);

    if (ptrName == NULL)
    {
	return NULL;
    }
    snprintf(name, sizeof(name), "%sRec", abmfP_get_c_struct_ptr_type_name(obj));
    return_c_ident(name);
}


STRING
abmfP_get_c_struct_ptr_type_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    STRING	varName = NULL;
    *name = 0;

    varName = abmfP_get_c_struct_global_name(obj);
    if (varName == NULL)
    {
	return NULL;
    }
    snprintf(name, sizeof(name), "%s%sInfo", typePrefixString, abmfP_capitalize_first_char(varName));
    cvt_ident_to_type(name);

    return_c_ident(name);
}


STRING
abmfP_get_c_struct_global_name(ABObj obj)
{
    static char	varName[MAX_NAME_SIZE];
    ABObj	structObj = NULL;
    
    structObj = abmfP_obj_get_struct_obj(obj);
    if (structObj == NULL)
    {
	return NULL;
    }

    abmfP_build_instance_prefix(structObj, varName, MAX_NAME_SIZE);

    return_c_ident(varName);
}


/*************************************************************************
 *************************************************************************
 **									**
 **		Substructures						**
 **									**
 *************************************************************************
 *************************************************************************/

STRING
abmfP_get_c_substruct_name(GenCodeInfo genCodeInfo, ABObj obj)
{
    static char	nameBuf[1024] = "";
    STRING	name = NULL;
    ABObj	structObj = NULL;
    ABObj	substructObj = NULL;
    ABObj	createObj = NULL;
    ABObj	ipObj = NULL;
    BOOL	ipObjIsSubstruct = FALSE;
    STRING	substructName = NULL;

    if (genCodeInfo->writing_func)
    {
	ipObj = genCodeInfo->cur_func.ip_obj;
	createObj = genCodeInfo->cur_func.create_obj;
	ipObjIsSubstruct = ((createObj != NULL) && (createObj == ipObj));
    }

    if (ipObj != NULL)
    {
	/* try to get a value relative to ip */
        substructObj = abmfP_obj_get_substruct_obj(obj);
        if (substructObj == NULL)
        {
	    return NULL;
        }
        structObj = abmfP_obj_get_struct_obj(substructObj);
        if (structObj == NULL)
        {
	    return NULL;
        }

        if (ipObjIsSubstruct && (substructObj == ipObj))
        {
	    /* the substructure is the instance */
            strcpy(nameBuf, abmfP_instance_ptr_var_name);
	    name = nameBuf;
        }
        else if (structObj == ipObj)
        {
	    /* The structure is the instance */
	    strcpy(nameBuf, abmfP_instance_ptr_var_name);
            substructName = abmfP_get_c_substruct_field_name(obj);
	    if (substructName != NULL)
	    {
	        strcat(nameBuf, "->");
	        strcat(nameBuf, substructName);
	        name = nameBuf;
	    }
        }
    }

    if (name == NULL)
    {
	name = abmfP_get_c_substruct_global_name(substructObj);
    }

    return name;
}


/*
 *
 */
STRING
abmfP_get_c_substruct_global_name(ABObj obj)
{
    static char	name[256];
    ABObj	subsObj = abmfP_obj_get_substruct_obj(obj);
    ABObj	structObj = NULL;
    STRING	structVar = NULL;
    STRING	structField = NULL;
    
    *name = 0;
    if (subsObj != NULL)
    {
	structObj = abmfP_obj_get_struct_obj(subsObj);
    }
    if (structObj == NULL)
    {
	return NULL;
    }

    structVar = abmfP_get_c_struct_global_name(structObj);
    structField = abmfP_get_c_substruct_field_name(subsObj);
    if ((structVar == NULL) || (structField == NULL))
    {
	return NULL;
    }
    sprintf(name, "%s.%s", structVar, structField);
    return name;
}

/*
 *
 */
STRING
abmfP_get_c_substruct_field_name(ABObj obj)
{
    static char		fieldName[MAX_NAME_SIZE];
    snprintf(fieldName, sizeof(fieldName), "%s_items", abmfP_uncapitalize_first_char(obj_get_name(obj)));
    return_c_ident(fieldName);
}


STRING
abmfP_get_c_substruct_type_name(ABObj obj)
{
    static char		typeName[256];
    STRING		ptrTypeName = abmfP_get_c_substruct_ptr_type_name(obj);

    if (ptrTypeName == NULL)
    {
	return NULL;
    }
    snprintf(typeName, sizeof(typeName), "%sRec", ptrTypeName);
    return_c_ident(typeName);
}


STRING
abmfP_get_c_substruct_ptr_type_name(ABObj obj)
{
    static char		ptrTypeName[MAX_NAME_SIZE];
    char		ptrTypeNameTmp[sizeof(ptrTypeName)];
    STRING		varName = NULL;
    ABObj		module = NULL;
    
    if (obj_is_ref(obj))
    {
	/* refs use the type of the obj they actually are */
	ABObj	actualObj = obj_get_actual_obj(obj);
	if (actualObj == NULL)
	{
	    return NULL;
	}
	varName = obj_get_name(actualObj);
    }
    else
    {
        varName = obj_get_name(obj);
    }

    if (varName == NULL)
    {
	return NULL;
    }

    if ((module = obj_get_module(obj)) == NULL)
	return NULL;
    else
    {
	/* Warning: Due to abmfP_capitalize_first_char() returning a pointer
	 * to static data this cannot be one snprintf() */
	snprintf(ptrTypeNameTmp, sizeof(ptrTypeNameTmp), "%s%s",
	         typePrefixString, abmfP_capitalize_first_char(obj_get_name(module)));
	snprintf(ptrTypeName, sizeof(ptrTypeName), "%s%sItems",
	         ptrTypeNameTmp, abmfP_capitalize_first_char(varName));
	cvt_ident_to_type(ptrTypeName);
    }

    return_c_ident(ptrTypeName);
}



STRING
abmfP_get_clear_proc_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    STRING	ptrTypeName = abmfP_get_c_struct_ptr_type_name(obj);
    /* abmfP_build_instance_prefix(obj, name, MAX_NAME_SIZE); */

    if (ptrTypeName == NULL)
    {
	return NULL;
    }
    snprintf(name, sizeof(name), "%s_clear", ptrTypeName);

    return_c_ident(abmfP_uncapitalize_first_char(name));
}


STRING
abmfP_get_create_proc_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    abmfP_build_instance_prefix(obj, name, MAX_NAME_SIZE);
    strcat(name, "_");
    strcat(name, "create");

    return_c_ident(name);
}


STRING
abmfP_get_init_proc_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    abmfP_build_instance_prefix(obj, name, MAX_NAME_SIZE);
    strcat(name, "_");
    strcat(name, "initialize");

    return_c_ident(name);
}


/*
 * "Why" we use this naming scheme is explained in the comment for
 *  abmfP_get_widget_name_for_res_file().
 *
 *  The C field names in the structures match the widget names.
 *
 * The scheme is:
 *	       shell: dtb_module_object
 * topmost container: dtb_module_object
 *             other: object
 */
STRING
abmfP_get_widget_name(ABObj obj)
{
    static char	nameBuf[MAX_NAME_SIZE];
    char	*name = nameBuf;
    ABObj	project = NULL;
    *nameBuf = 0;
    assert(abmfP_parent(obj) != NULL);
    assert(obj_get_name(obj_get_module(obj)) != NULL);

    if (   ObjWClassIsShellSubclass(obj)
	|| ObjWClassIsShellSubclass(abmfP_parent(obj)))
    {
	sprintf(nameBuf, "dtb_%s_%s", 
	    obj_get_name(obj_get_module(obj)),
	    abmfP_get_c_field_name(obj));
	name = nameBuf;
    }
    else if (   ObjWClassIsScrolledWindow(obj)
	     && (   obj_is_list(obj_get_root(obj))
		 || obj_is_text(obj_get_root(obj)) )
	    )
    {
	/* this widget gets named for us by an XmCreate...() function */
	ABObj	compRoot = obj_get_root(obj);
	ABObj	objectObj = objxm_comp_get_subobj(compRoot, AB_CFG_OBJECT_OBJ);
	name =abmfP_get_widget_name(objectObj);
	if ((name != NULL) && (name != nameBuf))
	{
	    snprintf(nameBuf, sizeof(nameBuf), "%s", name);
	}
	if ((*nameBuf) != 0)
	{
	    strcat(nameBuf, "SW");
	}
	name = nameBuf;
    }
    else
    {
	name = abmfP_get_c_field_name(obj);
    }
    return (((*name) == 0)? NULL:name);
}


/*
 * In the resource file, we use the conventions:
 *
 * shell from XtVaAppInitialize: Class
 *                        shell: Class*shell
 *            topmost container: Class*topmostcontainer
 *			   item: Class*topmostcontainer*widget.item
 *                    all other: Class*topmostcontainer*widget
 *
 * This means that wisely choosing widget names gives us something
 * like this (see abmfP_get_widget_name()):
 *
 * shell from XtVaAppInitialize: Project
 *               shell: Project*dtb_module_mainwindow
 *   topmost container: Project*dtb_module_mainwindow_form
 *		  item: Project*dtb_module_mainwindow_form*radiobox.itemA_item
 *           all other: Project*dtb_module_mainwindow_form*object
 */
STRING	
abmfP_get_widget_name_for_res_file(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    char	nameTemp[sizeof(name)];
    *name = 0;
    assert(abmfP_parent(obj) != NULL);

    if (mfobj_has_flags(obj, CGenFlagTreatAsAppShell))
    {
	/* The application shell is always provided for the application,
	 * from XtVaAppInitialize(). To actually set a resource on just that
	 * widget, the class name (only!) is used in the resource file.
	 */
	/* name=Class */
	assert(obj_is_base_win(obj));
	util_strncpy(name, abmfP_get_app_class_name(obj), MAX_NAME_SIZE);
    }
    else if (   (ObjWClassIsShellSubclass(obj))
             || (ObjWClassIsShellSubclass(abmfP_parent(obj))) )
    {
	/* name=Class*widget */
	sprintf(name, "%s*%s",
		abmfP_get_app_class_name(obj),
		abmfP_get_widget_name(obj));
    }
    else if (obj_is_item(obj))
    {
	/* name = Class*topmostcontainer*widget.item */

	ABObj	parent = abmfP_parent(obj);
	STRING	parentName = NULL;

	*name = 0;
	abmfP_get_widget_name_for_res_file(parent);
	if ((*name) != 0)
	{
	    strcat(name, ".");
	    strcat(name, abmfP_get_widget_name(obj));
	}
    }
    else
    {
	/* name=Class*topcontainer*widget */

	ABObj	container = obj;
	ABObj	shell = abmfP_parent(container);
	while ((shell != NULL) && (!ObjWClassIsShellSubclass(shell)))
	{
	    container = shell;
	    shell = abmfP_parent(container);
	}

	/* we can't use abmfP_get_widget_name twice in the same printf,
	 * because of the static string buffer.
	 */
	snprintf(nameTemp, sizeof(nameTemp), "%s*%s*",
	         abmfP_get_app_class_name(obj),
	         abmfP_get_widget_name(container));
	snprintf(name, sizeof(name), "%s%s",
	         nameTemp,
	         abmfP_get_widget_name(obj));
    }
    return name;
}


/*************************************************************************
 **									**
 **		FILE NAMES						**
 **									**
 *************************************************************************/

STRING
abmfP_get_project_c_file_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	project= obj_get_project(obj);
    sprintf(name, "%s.c", obj_get_name(project));
    return name;
}


STRING
abmfP_get_project_header_file_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	project= obj_get_project(obj);
    sprintf(name, "%s.h", obj_get_name(project));
    return name;
}


STRING	
abmfP_get_utils_c_file_name(ABObj obj)
{
    obj = obj;	/* avoid warning */
    return "dtb_utils.c";
}


STRING	
abmfP_get_utils_header_file_name(ABObj obj)
{
    obj = obj;	/* avoid warning */
    return "dtb_utils.h";
}


STRING abmfP_get_stubs_c_file_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	module= obj_get_module(obj);
    sprintf(name, "%s_stubs.c", obj_get_name(module));
    return name;
}


STRING
abmfP_get_ui_c_file_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	module= obj_get_module(obj);
    sprintf(name, "%s_ui.c", obj_get_name(module));
    return name;
}


STRING
abmfP_get_ui_header_file_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	module= obj_get_module(obj);
    sprintf(name, "%s_ui.h", obj_get_name(module));
    return name;
}


/*
 * app class name is project name, capitalized
 */
STRING
abmfP_get_app_class_name(ABObj obj)
{
    static char	name[MAX_NAME_SIZE];
    ABObj	project = obj_get_project(obj);
    *name = 0;
    util_strncpy(name, obj_get_name(project), MAX_NAME_SIZE);
    name[0] = toupper(name[0]);
    return name;
}


STRING
abmfP_get_exe_file_name(ABObj obj)
{
    ABObj	project = obj_get_project(obj);
    assert((project != NULL) && (obj_is_project(project)));

    if (mfobj_get_proj_data(project)->exe_name == NULL)
    {
	char	exe_name[1024];
	*exe_name = 0;
	cvt_type_to_ident(obj_get_name(project), exe_name, 1024);
        mfobj_get_proj_data(project)->exe_name  = istr_create(exe_name);
    }

    return istr_string(mfobj_get_proj_data(project)->exe_name);
}


/*************************************************************************
 **									**
 **		STATIC FUNCTIONS					**
 **									**
 *************************************************************************/


/*
 * Builds "modulename_objectname"
 *
 * First char is always lower case
 */
static STRING
abmfP_build_instance_prefix(ABObj obj, STRING prefixBuf, int prefixBufSize)
{
    char	tmpBuf[1024] = "";
    char	tmpBuf2[1024] = "";
    ABObj	module= obj_get_module(obj);

    if (module == NULL)
    {
	cvt_type_to_ident(obj_get_name(obj), prefixBuf, prefixBufSize);
    }
    else
    {
	/* Warning: Due to obj_get_name() returning a pointer to
	 * static data this cannot be one snprintf() */
	snprintf(tmpBuf2, sizeof(tmpBuf2), "%s%s_",
	         identPrefixString, obj_get_name(module));
	snprintf(tmpBuf, sizeof(tmpBuf), "%s%s",
	         tmpBuf2, obj_get_name(obj));
	cvt_type_to_ident(tmpBuf, prefixBuf, prefixBufSize);
    }

    assert(strlen(prefixBuf) < (size_t)prefixBufSize);
    return prefixBuf;
}


STRING	
abmfP_get_c_app_root_win_name(ABObj obj)
{
    static char	root_widget_name[256];
    ABObj	project= obj_get_project(obj);
    ABObj	root_window= abmfP_get_root_window(project);
    *root_widget_name = 0;

    snprintf(root_widget_name, sizeof(root_widget_name), "%s()", abmfP_lib_get_toplevel_widget->name);

    return root_widget_name;
}


static int	
ensure_unique_comp_field_names(ABObj obj)
{
    ABObj	compRoot = obj_get_root(obj);
    ABObj	objectObj = NULL;
    BOOL	objectObjRenamed = FALSE;
    STRING	compRootName = NULL;
    STRING	ext = NULL;

    if (   (compRoot == NULL) 
	|| mfobj_has_flags(compRoot, CGenFlagCompFieldsUnique))
    {
	return 0;
    }

    /*
     * The "object" obj is the one that most closely represents the 
     * purpose of this composite (e.g., the textfield widget for a textfield
     * composite).
     *
     * Give it exactly the name the user has given to the composite
     */
    compRootName = obj_get_name(compRoot);
    objectObj = objxm_comp_get_subobj(compRoot, AB_CFG_OBJECT_OBJ);
    if ((objectObj != NULL) && (objectObj != compRoot))
    {
	util_dprintf(2, "changing field name %s -> %s\n",
   	    util_strsafe(obj_get_name(objectObj)), compRootName);
	objectObjRenamed = TRUE;
	mfobj_data(objectObj)->field_name = istr_create(compRootName);
    }

    /*
     * XmConfig gives the composite name to the root of the composite.
     * See if we need to give it a better "widget" name.
     */
    if ((objectObjRenamed) && (compRoot != objectObj))
    {
	ext = get_ident_for_widget_class(compRoot);
	if (ext == NULL)
	{
	    ext = "container";
	}
        {
	    char	newObjName[1024];
	    if (compRootName != NULL)
	    {
	        snprintf(newObjName, sizeof(newObjName), "%s_%s", compRootName, ext);
	    } else {
	        snprintf(newObjName, sizeof(newObjName), "_%s", ext);
	    }
    
	    util_dprintf(2, "changing field name %s -> %s\n",
		    util_strsafe(obj_get_name(compRoot)), newObjName);
	    mfobj_data(compRoot)->field_name = istr_create(newObjName);
        }
    } /* compRoot != objectObj */
    mfobj_set_flags(compRoot, CGenFlagCompFieldsUnique);

    return 0;
}


static STRING
get_ident_for_widget_class(ABObj obj)
{
    STRING	ident = NULL;

    if (ObjWClassIsMenuShell(obj))
	ident = "menu";
    else if (ObjWClassIsShellSubclass(obj))
	ident = "shell";
    else if (ObjWClassIsBulletinBoard(obj))
	ident = "bulletin";
    else if (ObjWClassIsDrawingArea(obj))
	ident = "drawarea";
    else if (ObjWClassIsForm(obj))
	ident = "form";
    else if (ObjWClassIsFrame(obj))
	ident = "frame";
    else if (ObjWClassIsLabel(obj))
	ident = "label";
    else if (ObjWClassIsList(obj))
	ident = "list";
    else if (ObjWClassIsMainWindow(obj))
	ident = "mainwin";
    else if (ObjWClassIsPanedWindow(obj))
	ident = "panedwin";
    else if (ObjWClassIsPushButton(obj))
	ident = "button";
    else if (ObjWClassIsRowColumn(obj))
	ident = "rowcolumn";
    else if (ObjWClassIsScrolledWindow(obj))
	ident = "scrolledwin";
    else if (ObjWClassIsText(obj))
	ident = "text";
    else if (ObjWClassIsTextField(obj))
	ident = "textfield";

    return ident;
}


static STRING	
cvt_ident_to_type(STRING ident)
{
    int		prevCharOff = 0;
    BOOL	capitalizeNext = 0;
    int		nextCharOff = 0;
    char	nextChar = 0;
    int		identLen = util_strlen(ident);

    if (identLen < 0)
    {
	return NULL;
    }
    ident[0] = toupper(ident[0]);

    for (nextCharOff = 0; nextCharOff < identLen; ++nextCharOff)
    {
	nextChar = ident[nextCharOff];
	if (nextChar == '_')
	{
	    capitalizeNext = TRUE;
	}
	else
	{
	    ident[prevCharOff++] = capitalizeNext? toupper(nextChar):nextChar;
	    capitalizeNext = FALSE;
	}
    }
    ident[prevCharOff] = 0;

    return ident;
}


/*
 * This routine must be identical in:
 *
 *	src/abmf/obj_names.c
 *	src/ab/cgen_utils.c
 *
 * If either this one or the other one is changed, copy the routine to the
 * other file.
 */
static STRING
cvt_type_to_ident(STRING type, STRING identBuf, int identBufSize)
{
    int		typeOff = 0;
    int		identOff = 0;
    int		typeChar = -1;
    int		typeLen = util_strlen(type);
    int		identMaxLen = identBufSize-1;
    int		lastIdentChar = -1;
    BOOL	lastTypeCharWasUpper = FALSE;

    for (typeOff = 0; 
	    (typeOff < typeLen) && (identOff < (identMaxLen-1)); ++typeOff)
    {
	typeChar = type[typeOff];
	if (isupper(typeChar))
	{
	    if (   (lastIdentChar != '_') 
		&& (!lastTypeCharWasUpper)
		&& (lastIdentChar != -1)
	       )
	    {
		lastIdentChar = identBuf[identOff++] = '_';
	    }
	    if (identOff < (identMaxLen-1))
	    {
	        lastIdentChar = identBuf[identOff++] = tolower(typeChar);
	    }
	    lastTypeCharWasUpper = TRUE;
	}
	else
	{
	    lastIdentChar = identBuf[identOff++] = typeChar;
	    lastTypeCharWasUpper = FALSE;
	}
    }
    identBuf[identOff] = 0;

    return identBuf;
}


/*
 * This is in a function, so that if the assert fails the message is
 * meaningful. Otherwise, any macros in the assert() get expanded
 * and can be several lines long.
 */
#ifdef DEBUG
static int
assert_c_ident_ok(STRING ident)
{
    assert((ident == NULL) || (ab_c_ident_is_ok(ident)));
    return 0;
}
#endif /* DEBUG */

STRING
abmfP_get_msg_clear_proc_name(ABObj msgObj)
{
    static char name[MAX_NAME_SIZE];

    snprintf(name, sizeof(name), "%s_initialize", abmfP_get_c_struct_global_name(msgObj));

    return_c_ident(name);
}



