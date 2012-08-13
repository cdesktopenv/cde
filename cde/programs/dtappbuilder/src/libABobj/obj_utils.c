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
 * $XConsortium: obj_utils.c /main/4 1996/10/02 15:44:56 drk $
 * 
 *	@(#)obj_utils.c	3.137 01 Feb 1995
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
 * utils.c - general utilities
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/times.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include "objP.h"		/* put objP.h first! */
#include <ab_private/trav.h>
#include <ab_private/util.h>
#include <ab/util_types.h>
#include <ab_private/istr.h>
#include "obj_utils.h"
#include "obj_names_listP.h"

#define AB_TYPE_MODULE	AB_TYPE_MODULE	/* REMIND: can be taken out later */

/*
 * Global variables...
 */
char               *abo_empty_string = "";
char               *abo_null_string = "(nil)";

static struct sigaction	org_sig_segv_handler;
#ifdef SIGBUS
static struct sigaction org_sig_bus_handler;
#endif
static sigjmp_buf	sigjmp_env;

static volatile BOOL	sig_fault_handler_installed = FALSE;
static volatile BOOL	mem_fault_occurred = FALSE;
static int	sig_fault_handler_install(void);
static int	sig_fault_handler_uninstall(void);
static void	sig_fault_handler();
static BOOL	objP_is_accessible(ABObj obj);
static int	build_obj_array(
			ABObj **objArrayInOut, 
			int *objArraySizeInOut, 
			ABObj root
		);
static int	verify_the_silly_index(
			ABObj		nameScopeObj,
			ABObj		*objArray, 
			int		objArraySize
		);


/*************************************************************************
**									**
**		General (non-object) utilities				**
**									**
*************************************************************************/

AB_OBJECT_TYPE
ab_get_proper_subobj_type(AB_OBJECT_TYPE type)
{
    AB_OBJECT_TYPE      subtype = AB_TYPE_UNKNOWN;

    switch (type)
    {
    case AB_TYPE_PROJECT:
	subtype = AB_TYPE_MODULE;
	break;

    case AB_TYPE_CHOICE:
    case AB_TYPE_LIST:
    case AB_TYPE_MENU:
	subtype = AB_TYPE_ITEM;
	break;
    }

    return subtype;
}


/*
 * Returns the item type this object should contain.  If the object is an
 * item, it returns the type of the item.
 */
AB_ITEM_TYPE
ab_get_proper_item_type(AB_OBJECT_TYPE type)
{
    AB_ITEM_TYPE        item_type = AB_ITEM_FOR_UNDEF;

    switch (type)
    {
    case AB_TYPE_MENU:
	item_type = AB_ITEM_FOR_MENU;
	break;

    case AB_TYPE_CHOICE:
	item_type = AB_ITEM_FOR_CHOICE;
	break;

    case AB_TYPE_LIST:
	item_type = AB_ITEM_FOR_LIST;
	break;
    }

    return item_type;
}


int
ab_string_list_count(STRING * list)
{
    int                 i = 0;
    if (list != NULL)
    {
	while (list[i] != NULL)
	{
	    ++i;
	}
    }
    return i;
}


AB_FILE_TYPE
ab_file_type_from_path(STRING path)
{
    AB_FILE_TYPE        file_type = AB_FILE_UNDEF;
    int                 pathlen = strlen(path);

    if (pathlen >= 3)
    {
	char               *ext = &(path[pathlen - 2]);
	if (strcmp(ext, ".P") == 0)
	{
	    file_type = AB_FILE_GIL_PROJECT;
	}
	else if (strcmp(ext, ".G") == 0)
	{
	    file_type = AB_FILE_GIL_INTERFACE;
	}
    }

    return file_type;
}


/*
 * Only ensures that the identifier has no white space in it.
 */
BOOL
ab_ident_is_ok(STRING ident)
{
    char               *cp;
    if (ident == NULL)
    {
	return FALSE;
    }
    for (cp = ident; *cp != 0; ++cp)
    {
	if (isspace(*cp))
	{
	    return FALSE;
	}
    }
    return TRUE;
}


BOOL
ab_c_ident_is_ok(STRING ident)
{
    int                 i;
    int                 len = 0;
    char                ch;

    if (ident == NULL)
    {
	return TRUE;
    }
    for (i = 0, len = strlen(ident); i < len; ++i)
    {
	ch = ident[i];

	/* The first character of a variable name must be a letter.
	 * The underscore character is considered a letter in C. 
	 */
	if (i == 0)
	{
	    if (!(isalpha(ch)) && (ch != '_'))
		return FALSE;
	}
	else
	{
	    if (!((isalnum(ch) || (ch == '_'))))
		return FALSE;
	}
    }

    return TRUE;
}


/*************************************************************************
**									**
**		Object utilities					**
**									**
*************************************************************************/

STRING
obj_get_file(ABObj obj)
{
    if (obj->type == AB_TYPE_FILE)
    {
	return obj_get_name(obj);	/* stored as name */
    }
    while ((obj != NULL) && (obj->type != AB_TYPE_MODULE)
	   && (obj->type != AB_TYPE_PROJECT))
    {
	obj = obj->parent;
    }
    if (obj != NULL)
    {
	if (obj->type == AB_TYPE_MODULE)
	{
	    return istr_string(obj->info.module.file);
	}
	else if (obj->type == AB_TYPE_PROJECT)
	{
	    return istr_string(obj->info.project.file);
	}
    }
    return NULL;
}

/*
 * Gets the project the object belongs to
 */
ABObj
obj_get_project(ABObj obj)
{
    while ((obj != NULL) && (obj->type != AB_TYPE_PROJECT))
    {
	obj = obj->parent;
    }
    return obj;
}

/*
 * Gets the module the object belongs to
 */
ABObj
obj_get_module(ABObj obj)
{
    while ((obj != NULL) && (obj->type != AB_TYPE_MODULE))
    {
	obj = obj->parent;
    }
    return obj;
}

/*
 * Gets the window the object belongs to
 */
ABObj
obj_get_window(ABObj obj)
{
    while ((obj != NULL) && !obj_is_window(obj))
    {
	obj = obj->parent;
    }
    return obj;
}

int
obj_get_num_items(ABObj obj)
{
    return trav_count(obj, AB_TRAV_ITEMS_FOR_OBJ);
}

/*
 * Counts the given object! (i.e., always returns at least 1).
 */
int
obj_get_num_siblings(ABObj obj)
{
    AB_TRAVERSAL        trav;
    int                 num_sibs = 0;
    for (trav_open(&trav, obj, AB_TRAV_SIBLINGS);
	 (obj = trav_next(&trav)) != NULL;)
    {
	++num_sibs;
    }
    return num_sibs;
}

/*
 * Makes sure the object has at least the specified number of children.
 * 
 * Any new children created are appended to the child list, and they are of the
 * type returned from abo_proper_item_type.
 */
int
obj_ensure_num_children(ABObj obj, int requested_num_children)
{
    AB_OBJECT_TYPE      child_type = ab_get_proper_subobj_type(obj->type);
    AB_ITEM_TYPE        child_item_type = ab_get_proper_item_type(obj->type);
    int                 num_children = obj_get_num_children(obj);
    ABObj               child = NULL;

    while (num_children < requested_num_children)
    {
	child = obj_create(child_type, NULL);
	if (child == NULL)
	{
	    return -1;
	}
	if (obj_is_item(child))
	{
	    child->info.item.type = child_item_type;
	}
	obj_append_child(obj, child);
	++num_children;
    }
    return 0;
}


ABObj
obj_find_child_by_label(ABObj root, STRING label)
{
    ABObj               child;

    for (child = root->first_child;
	 child != NULL; child = child->next_sibling)
    {
	if (istr_equalstr(child->label, label))
	{
	    break;
	}
    }
    return child;
}


ABObj
obj_find_child_by_type(ABObj obj, AB_OBJECT_TYPE childType)
{
    ABObj	child;
    for (child = obj->first_child; 
	(child != NULL); child = child->next_sibling)
    {
	if (child->type == childType)
	{
	    break;
	}
    }

    return child;
}


ABObj
obj_find_by_name(ABObj root, STRING name)
{
    StringList		names = objP_get_names_scope_for_children(root);
    ISTRING		istr_name = istr_dup_existing(name);
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    if ((name == NULL) || (istr_name == NULL))
    {
	return NULL;
    }

    if (names != NULL)
    {
	obj = (ABObj)strlist_get_istr_data(names, istr_name);
    }
    else
    {
        for (trav_open(&trav, root, AB_TRAV_ALL);
	     (obj = trav_next(&trav)) != NULL;)
        {
	    if (!(obj_is_project(obj) || obj_is_module(obj)))
	    {
	        if (istr_equal(obj->name, istr_name))
		    break;
	    }
        }
    }

    istr_destroy(istr_name);
    return obj;
}

ABObj
obj_find_by_name_and_type(ABObj root, STRING name, AB_OBJECT_TYPE type)
{
    ABObj               obj;
    AB_TRAVERSAL        trav;

    if (name == NULL)
    {
	return NULL;
    }
    for (trav_open(&trav, root, AB_TRAV_ALL);
	 (obj = trav_next(&trav)) != NULL;)
    {
	if ((obj->type == type)
	    && (istr_equalstr(obj->name, name)))
	{
	    break;
	}
    }
    trav_close(&trav);
    return obj;
}


ABObj
obj_find_by_type(ABObj root, AB_OBJECT_TYPE type)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    for (trav_open(&trav, root, AB_TRAV_UI | AB_TRAV_MOD_PARENTS_FIRST);
	 (obj = trav_next(&trav)) != NULL;)
    {
	if (obj->type == type)
	{
	    break;
	}
    }
    trav_close(&trav);

    return NULL;
}


ABObj
obj_find_child_by_name(ABObj obj, STRING name)
{
    ABObj               child;
    if (name == NULL)
    {
	return NULL;
    }
    for (child = obj->first_child;
	 child != NULL; child = child->next_sibling)
    {
	if (istr_equalstr(child->name, name))
	{
	    return child;
	}
    }
    return NULL;
}


ABObj
obj_find_menu_by_name(ABObj root, STRING name)
{
    return obj_find_by_name_and_type(root, name, AB_TYPE_MENU);
}


ABObj
obj_find_module_by_name(ABObj root, STRING name)
{
    AB_TRAVERSAL        trav;
    ABObj               module = NULL;

    for (trav_open(&trav, root, AB_TRAV_MODULES);
	 (module = trav_next(&trav)) != NULL;)
    {
	if (util_streq(obj_get_name(module), name))
	{
	    break;
	}
    }
    trav_close(&trav);

    return module;
}


ABObj
obj_scoped_find_or_create_undef(ABObj parent,
				STRING scopedName, AB_OBJECT_TYPE objType)
{
    ABObj               obj = obj_scoped_find_by_name(parent, scopedName);

    if (obj == NULL)
    {
	STRING              moduleName = obj_scoped_name_get_module_name(
								scopedName);
	STRING              objName = obj_scoped_name_get_obj_name(
								scopedName);
	ABObj               project = NULL;
	ABObj               module = NULL;

	if (moduleName == NULL)
	{
	    /* simple name - create as child of the "parent" object */
	    obj = obj_create(objType, parent);
	}
	else
	{
	    /* complex name - find module to create under */
	    project = obj_get_project(parent);
	    if (project != NULL)
	    {
		module = obj_find_module_by_name(project, moduleName);
		if (module == NULL)
		{
		    /* damn! module don't exist!! */
		    module = obj_create(AB_TYPE_MODULE, project);
		    obj_set_is_defined(module, FALSE);
		    obj_set_name(module, moduleName);
		}
	    }
	    if (module != NULL)
	    {
		/* actually create the object under the correct module */
		obj = obj_create(objType, module);
	    }
	}

	/*
	 * We've (hopefully) created a new object (or more correctly, a
	 * reference to an undefined object). Set the attributes that we know
	 * about the object
	 */
	if (obj != NULL)
	{
	    obj_set_is_defined(obj, FALSE);
	    obj_set_name(obj, objName);
	}
    }				/* obj == NULL */

    return obj;
}


/*************************************************************************
**									**
**		Functions private to libgobj				**
**									**
**************************************************************************/

int
abo_error(STRING message)
{
    util_printf_err("%s\n", message);
    return 0;
}

static int
indent(int spaces)
{
    int                 i;
    for (i = 0; i < spaces; ++i)
    {
	util_dputs(0, " ");
    }
    return 0;
}

/*
 * Returns TRUE if the object is the target of a project connection (i.e. one
 * that is written into the project .c file, and not into a module.  Formerly
 * known as cross-module connections).
 */
BOOL
obj_is_project_action_target(ABObj obj)
{
    ABObj               project = NULL;
    BOOL                is_project_target = FALSE;
    ABObj               action = NULL;
    AB_TRAVERSAL        trav;

    project = obj_get_project(obj);
    for (trav_open(&trav, project, AB_TRAV_ACTIONS_FOR_OBJ);
	 (action = trav_next(&trav)) != NULL;)
    {
	if (action->info.action.to == obj)
	{
	    is_project_target = TRUE;
	    break;
	}
    }
    trav_close(&trav);
    return is_project_target;
}


/*
 * Return a valid C name given a string (normally, a label)
 */
STRING
ab_make_valid_c_ident(STRING label)
{
    static char         name[MAXPATHLEN];
    static int          ith = 0;
    int                 lastchar = -1;
    register char      *oldPtr = label;
    register char      *newPtr = name;

    if (isdigit(*oldPtr))
    {

	/*
	 * If the label starts with a number, prepend 'dtb'
	 */
	*newPtr++ = 'd';
	*newPtr++ = 't';
	*newPtr++ = 'b';
	lastchar = *(newPtr-1);
    }

    while ((*oldPtr) != '\0')
    {
	if (isalnum(*oldPtr))
	{
	    lastchar = *newPtr++ = *oldPtr++;
	}
	else if (isprint(*oldPtr) && (lastchar != '_'))
	{
	    /* turn illegal printable chars into _ (e.g. / .) */
	    lastchar = *newPtr++ = '_';
	    oldPtr++;
	}
	else
	{
	    /*
	     * Skip the illegal character.
	     */
	    oldPtr++;
	}
    }

    /*
     * Strip off trailing underscore (there will be no more than one).
     */
    if (lastchar == '_')
    {
	--newPtr;
    }
    *newPtr = 0;	/* terminate the string */

    if (newPtr == name)
    {

	/*
	 * The entire label is composed of illegal characters, in that case,
	 * return a generated name.
	 */
	sprintf(name, "dtb_name_%d", ith++);
    }

    return name;
}


/*
 * Given a unix file name, strip off the path and the suffix.
 */
STRING
ab_ident_from_file_name(STRING filename)
{
    static char         buf[MAXPATHLEN];
    char               *p;

    if (filename != NULL)
    {
	if (p = (char *) strrchr(filename, '/'))
	    strcpy(buf, p + 1);
	else
	    strcpy(buf, filename);

	if (p = (char *) strrchr(buf, '.'))
	    *p = '\0';
    }
    return buf;
}


/*
 * Assumes: name, label not NULL.
 */
STRING
ab_ident_from_name_and_label(STRING name, STRING label)
{
    static char         new_name[256];
    sprintf(new_name, "%s_%s", name, label);
    return ab_make_valid_c_ident(new_name);
}


/*
 * Gets the module and object name from a complex name (i.e., a name of the
 * form module.name or module::name
 */
int
obj_scoped_name_split(
		      STRING complex_name,
		      STRING module_name_buf,
		      int module_name_buf_size,
		      STRING obj_name_buf,
		      int obj_name_buf_size
)
{
    int                 return_value = 0;
    char               *dot_ptr = NULL;
    char               *module_name_ptr = NULL;
    char               *obj_name_ptr = complex_name;
    char               *module_name_end_ptr = NULL;
    int                 moduleNameEndChar = -1;

    if ((dot_ptr = strchr(complex_name, '.')) != NULL)
    {
	module_name_ptr = complex_name;
	obj_name_ptr = dot_ptr + 1;
    }
    else if ((dot_ptr = strstr(complex_name, "::")) != NULL)
    {
	module_name_ptr = complex_name;
	obj_name_ptr = dot_ptr + 2;
    }

    /*
     * Remove white space on either side of the separator
     */
    if (dot_ptr != NULL)
    {
	/* put a 0 at the end of the module name */
	for (module_name_end_ptr = dot_ptr - 1;
	     ((module_name_end_ptr > complex_name)
	      && isspace(*module_name_end_ptr));)
	{
	    --module_name_end_ptr;
	}
	if (module_name_end_ptr < dot_ptr)
	{
	    ++module_name_end_ptr;	/* point one *after* last char in
					 * name */
	}
	moduleNameEndChar = *module_name_end_ptr;
	*module_name_end_ptr = 0;
    }
    while (((*obj_name_ptr) != 0) && (isspace(*obj_name_ptr)))
    {
	++obj_name_ptr;
    }

    /*
     * We now now the substrings' locations. grab them
     */
    if (module_name_buf != NULL)
    {
	if (module_name_ptr == NULL)
	{
	    /* no module name! */
	    *module_name_buf = 0;
	}
	else
	{
	    strncpy(module_name_buf, complex_name, module_name_buf_size);
	    module_name_buf[module_name_buf_size - 1] = 0;
	}
    }
    if (obj_name_buf != NULL)
    {
	strncpy(obj_name_buf, obj_name_ptr, obj_name_buf_size);
	obj_name_buf[obj_name_buf_size - 1] = 0;
    }

/* epilogue: */
    /* replace the 0 we inserted with the char that was there */
    if (module_name_end_ptr != NULL)
    {
	*module_name_end_ptr = moduleNameEndChar;
    }
    return return_value;
}


/*
 * Gets the module name from the given object name, if there is one (i.e, the
 * name is of the for module::name or module.name).
 */
STRING
obj_scoped_name_get_module_name(STRING complexName)
{
    static char         module_name[256] = "";
    int                 rc;
    rc = obj_scoped_name_split(complexName, module_name, 256, NULL, 0);
    if ((rc < 0) || (*module_name == 0))
    {
	return NULL;
    }
    return module_name;
}


/*
 * Gets the simple object name from the complex name (a name of the form
 * module::name or module.name).
 */
STRING
obj_scoped_name_get_obj_name(STRING complexName)
{
    static char         simple_name[256] = "";
    int                 rc;
    rc = obj_scoped_name_split(complexName, NULL, 0, simple_name, 256);
    if ((rc < 0) || (*simple_name == 0))
    {
	return NULL;
    }
    return simple_name;
}


/*************************************************************************
**									**
**		Debugging functions					**
**									**
**************************************************************************/

static int
obj_tree_print_indented(ABObj obj,
			int spaces, int verbosity);
static int          indent(int spaces);

int
obj_print(ABObj obj)
{
    int                 spaces = 0;
    int                 verbosity = util_get_verbosity();
    return obj_print_indented(obj, spaces, verbosity);
}

int
obj_print_indented(ABObj obj, int spaces, int verbosity)
{
#define print_flag(flag, flag_str) \
	(obj_has_flag(obj, flag)? \
	    util_dprintf(0, " %s", flag_str) \
        : \
	    0)

    char                namebuf[256];

    *namebuf = 0;
    if (obj == NULL)
    {
	util_dprintf(0, "NULL Object\n");
	return 0;
    }
    if (obj_is_action(obj))
    {
	switch (obj->info.action.func_type)
	{
	case AB_FUNC_USER_DEF:
	    sprintf(namebuf, "func:%s",
		    istr_string_safe(obj->info.action.func_value.func_name));
	    break;

	case AB_FUNC_BUILTIN:	/* builtin */
	    sprintf(namebuf, "builtin:%s",
		    util_builtin_action_to_string(
				      obj->info.action.func_value.builtin));
	    break;
	}
    }
    else
    {
	sprintf(namebuf, "name:'%s'", istr_string_safe(obj->name));
    }
    indent(spaces);
    if ((obj != NULL) && (obj_has_impl_flags(obj, ObjFlagDestroyed)))
    {
	util_dprintf(1, "**DESTROYED** ");
    }
    if ((obj != NULL) && (!obj_is_defined(obj)))
    {
	util_dprintf(0, "**UNDEF** ");
    }
    util_dprintf(0, "0x%08lx: %s  type:%s\n",
		 obj, namebuf,
		 util_object_type_to_string(obj->type));
    if (verbosity >= 4)
    {
        indent(spaces);
        util_dprintf(1, "file: '%s'\n", util_strsafe(obj_get_file(obj)));
	if (obj->flags != NoFlags)
	{
	    indent(spaces);
	    util_dprintf(0, "flags:");
	    print_flag(NoCodeGenFlag, "NoCodeGen");
	    print_flag(XmConfiguredFlag, "XmConfiged");
	    print_flag(XmCfgForCodeFlag, "CfgForCode");
	    print_flag(XmCfgForBuildFlag, "CfgForBuild");
	    print_flag(InstantiatedFlag, "Instantiated");
	    print_flag(BuildActionsFlag, "BuildActions");
	    print_flag(MappedFlag, "Mapped");
	    print_flag(AttrChangedFlag, "AttrChanged");
	    print_flag(SaveNeededFlag, "SaveNeeded");
	    print_flag(BeingDestroyedFlag, "BeingDestroyed");
	    util_dprintf(0, "\n");
	}
	if (obj_is_action(obj))
	{
	    ABObj               thisModule = NULL;
	    ABObj               thatModule = NULL;
	    ABObj               thatObj = NULL;
	    AB_WHEN		when = AB_WHEN_UNDEF;

	    if (obj != NULL)
	    {
		thisModule = obj_get_module(obj);
	    }

	    /*
	     * To value
	     */
	    thatObj = obj->info.action.to;
	    if (thatObj != NULL)
	    {
		thatModule = obj_get_module(thatObj);
	    }
	    indent(spaces);
	    util_dprintf(0, "to: ");
	    if ((thatModule != NULL) && (thatModule != thisModule))
	    {
		util_dprintf(0, "%s.", obj_get_safe_name(thatModule, namebuf, 256));
	    }
	    util_dprintf(0, "(%#08lx)%s\n",
			 thatObj, obj_get_safe_name(thatObj, namebuf, 256));

	    /*
	     * From value
	     */
	    thatObj = obj->info.action.from;
	    if (thatObj != NULL)
	    {
		thatModule = obj_get_module(thatObj);
	    }
	    indent(spaces);
	    util_dprintf(0, "from: ");
	    if ((thatModule != NULL) && (thatModule != thisModule))
	    {
		util_dprintf(0, "%s.", obj_get_safe_name(thatModule, namebuf, 256));
	    }
	    util_dprintf(0, "(%#08lx)%s\n",
			 thatObj, obj_get_safe_name(thatObj, namebuf, 256));

	    when = obj_get_when(obj);
	    indent(spaces);
	    util_dprintf(0, 
		"when: %s\n", util_strsafe(util_when_to_string(when)));
	}
	else
	{
	    indent(spaces);
	    util_dprintf(0, "class:%s\n",
			 istr_string_safe(obj->class_name));
	    indent(spaces);
	    util_dprintf(0, "[parent: %s]\n",
			 obj_get_safe_name(obj->parent, namebuf, 256));
	    if (obj->ref_to != NULL)
	    {
	        indent(spaces);
	        util_dprintf(0, "[ref_to: %s]\n",
			 obj_get_safe_name(obj->ref_to, namebuf, 256));
	    }
	    if (obj->part_of != NULL)
	    {
	        indent(spaces);
	        util_dprintf(0, "[part_of: %s]\n",
			 obj_get_safe_name(obj->part_of, namebuf, 256));
	    }
	    indent(spaces);
	    util_dprintf(0, "x: %d\n", obj->x);
	    indent(spaces);
	    util_dprintf(0, "y: %d\n", obj->y);
	    indent(spaces);
	    util_dprintf(0, "width: %d\n", obj->width);
	    indent(spaces);
	    util_dprintf(0, "height: %d\n", obj->height);
	    indent(spaces);
	    util_dprintf(0, "label: '%s'\n",
			 istr_string_safe(obj->label));
	}			/* if is_action */
    }
    return 0;
#undef print_flag
}				/* obj_print_indented */


int
obj_tree_print(ABObj obj)
{
    int                 iRet = 0;
    util_dprintf(0, "***** Object tree *****\n");
    iRet = obj_tree_print_indented(obj, 0, util_get_verbosity());
    iRet = obj_tree_verify(obj);
    if (iRet < 0)
    {
	util_dprintf(0, "\n***\n***  TREE IS CORRUPT!\n***");

    }
    util_dprintf(0, "******* Tree End ******\n");
    return iRet;
}


static int
obj_tree_print_indented(ABObj obj, int spaces, int verbosity)
{
    AB_TRAVERSAL        trav;
    ABObj               child = NULL;

    if (obj == NULL)
    {
	util_dprintf(0, "NULL Tree\n");
	return 0;
    }
    obj_print_indented(obj, spaces, verbosity);
    if (verbosity >= 4)
    {
	util_dprintf(0, "\n");
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	 (child = trav_next(&trav)) != NULL;)
    {
	obj_tree_print_indented(child, spaces + 4, verbosity);
    }
    trav_close(&trav);

    return 0;
}


/*
 * Perform regular checks of object integrity.
 * Tries not to overuse CPU, so this may or may not actually verify anything.
 *
 * Note that the debug_last_verify_time field doesn't exist in non-debugging
 * builds.
 */
int
objP_update_verify(ABObj obj)
{
#ifndef DEBUG
    return 0;
#else
    int			return_value = 0;
    static ABObj	lastObj = NULL;
    static int		lastObjCount = 0;
    time_t		curTime = 0;

    /*
     * time() can be rather slow, so let multiple references to the 
     * same object slide...
     */
    if (   (obj != NULL)
        && (   (debug_level() >= 5)
	    || ((   (obj != lastObj) || (++lastObjCount > 10))
	         && (curTime > (obj->debug_last_verify_time+1))) )
       )
    {
	return_value = obj_verify(obj);
	lastObj = obj;
	lastObjCount = 0;
    }
    return return_value;
#endif /* DEBUG */
}


/*
 * This function does not call any functions outside of this file.
 * This is because many functions call this one, and we don't want
 * to recurse.
 *
 * This function can be EXTREMELY SLOW! Calling it, for instance,
 * one million times inside a loop would not be the right thing to do.
 */
int
obj_verify(ABObj obj)
{
#define safe_data_access(_expr) \
    mem_fault_occurred = FALSE; \
    if (sigsetjmp(sigjmp_env, TRUE) == 0) \
    { \
	(_expr); \
    } \
    data_access_ok = (!mem_fault_occurred);

#define field_err(_fieldName) \
	(return_value = -1, \
	util_dprintf(0, \
            "ERROR: Obj %s, bad value in field: %s\n", \
		obj_name, (_fieldName)))

#define check_str(obj, _field) \
    ( ((long)(last_field = #_field)), \
      (istr_verify(obj->_field) >= 0)? \
	(0) \
    : \
	(field_err(#_field)))

    volatile BOOL	objIsAccessible = FALSE;
    volatile int	return_value = 0;
    volatile char	obj_name[1024] = "";
    volatile STRING	obj_str_ptr_name = NULL;
    volatile ABObj	parent = NULL;
    volatile ABObj	next_sibling = NULL;
    volatile ABObj	prev_sibling = NULL;
    volatile BOOL	ok = FALSE;
    volatile BOOL	data_access_ok = TRUE;
    volatile STRING	last_field = NULL;
    volatile ABObj	tmpObj = NULL;
    volatile StringList	namesList = NULL;

    sig_fault_handler_install();

    if (obj == NULL)
    {
	return_value = -1;
	goto epilogue;
    }

    /*
     * See if this object even exists
     */
    if (!objP_is_accessible(obj))
    {
	objIsAccessible = FALSE;
	return_value = -1;
	util_dprintf(0,
	    "ERROR - bad object ptr in obj_verify(): %#lx\n", obj);
	goto epilogue;
    }
    objIsAccessible = TRUE;	/* don't forget to set this!! */

    /*
     * Get the name of the object
     */
    mem_fault_occurred = FALSE;
    if (sigsetjmp(sigjmp_env, TRUE) == 0)
    {
	obj_str_ptr_name = NULL;
	ok = (istr_verify(obj->name) >= 0);
	if (ok)
	{
	    obj_str_ptr_name = istr_string(obj->name);
	    sprintf((STRING)obj_name, "(ABObj %#lx", obj);
	    if (obj_str_ptr_name != NULL)
	    {
	        strcat((STRING)obj_name, " = ");
	        strcat((STRING)obj_name, obj_str_ptr_name);
	    }
	    strcat((STRING)obj_name, ")");
	    if (!ok)
	    {
	        field_err("name");
	    }
	} /* ok */
    }
    else
    {
	sprintf((STRING)obj_name, "(ABObj %#lx)", obj);
	field_err("name");
    }

    if (obj->impl_flags == ObjFlagAlreadyFreedValue)
    {
	util_dprintf(0, "\n\n");
	util_dprintf(0, "\nDANGER WILL ROBINSON! COSMIC STORM APPROACHES!!!\n");
	util_dprintf(0, "\n");
	sleep(5);
	util_dprintf(0, "        (Actually, it's just a reference to a previously destroyed object)\n");
	util_dprintf(0, "        (Object is %s)\n", obj_name);
	util_dputs(0, "\n\n");
	return_value = -1;
    }

    /*
     * next_sibling
     */
    next_sibling = obj->next_sibling;
    if ((next_sibling != NULL) && (!objP_is_accessible(next_sibling)))
    {
	next_sibling = NULL;
	field_err("next_sibling");
    }
    if (next_sibling != NULL)
    {
	safe_data_access(ok = (next_sibling->prev_sibling == obj));
	if ((!data_access_ok) || (!ok))
	{
	    return_value = -1;
	    field_err("next_sibling");
	}
    }

    /*
     * parent
     */
    prev_sibling = obj->prev_sibling;
    if ((prev_sibling != NULL) && (!objP_is_accessible(prev_sibling)))
    {
	prev_sibling = NULL;
	field_err("prev_sibling");
    }
    if (prev_sibling != NULL)
    {
	safe_data_access(ok = (prev_sibling->next_sibling == obj));
	if ((!data_access_ok) || (!ok))
	{
	    return_value = -1;
	    field_err("next_sibling");
	}
    }

    /*
     * parent
     */
    parent = obj->parent;
    if ((parent != NULL) && (!objP_is_accessible(parent)))
    {
	parent = NULL;
	field_err("parent");
    }
    if (parent != NULL)
    {
	volatile ABObj	child;
	volatile BOOL	found = FALSE;

	ok = TRUE;
	safe_data_access(child = parent->first_child);
	if (!data_access_ok)
	{
	   field_err("parent");
	}
	else
	{
	    while ((!found) && (child != NULL))
	    {
	        if (child == obj)
	        {
		    found = TRUE;
	        }
		safe_data_access(child = child->next_sibling);
		if (!data_access_ok)
		{
		    field_err("parent");
		    break;
		}
	    }
	}
	if (!found)
	{
	    return_value = -1;
	    field_err("parent");
	}
    }

    /*
     * first_child
     */
    
    /*
     * Still should check: first_child, part_of
     */

    /*
     * Check all the ISTRING fields
     */

    /* we shouldn't mem fault here, but we're going to prepare, */
    /* just in case */
    mem_fault_occurred = FALSE;
    if (sigsetjmp(sigjmp_env, TRUE) != 0)
    {
	return_value = -1;
	if (last_field != NULL)
	{
	    field_err(last_field);
	}
	goto epilogue;
    }
    check_str(obj,user_data);
    check_str(obj,help_volume);
    check_str(obj,help_location);
    check_str(obj,help_text);
    check_str(obj,bg_color);
    check_str(obj,fg_color);
    check_str(obj,label);
    check_str(obj,menu_name);
    check_str(obj,class_name);

    /*
     * Check type-specific info
     */
    switch (obj->type)
    {
	case AB_TYPE_ACTION:
	    switch (obj->info.action.func_type)
	    {
	        case AB_FUNC_CODE_FRAG:
		    check_str(obj,info.action.func_value.code_frag);
	        break;
    
	        case AB_FUNC_USER_DEF:
		    check_str(obj,info.action.func_value.func_name);
	        break;
	    }
	    switch (obj->info.action.arg_type)
	    {
	        case AB_ARG_STRING:
		    check_str(obj,info.action.arg_value.sval);
	        break;
	    }
	    check_str(obj,info.action.func_name_suffix);
	break;

        case AB_TYPE_FILE_CHOOSER:
	    check_str(obj,info.file_chooser.filter_pattern);
	    check_str(obj,info.file_chooser.ok_label);
	    check_str(obj,info.file_chooser.directory);
	break;

        case AB_TYPE_MESSAGE:
	    check_str(obj,info.message.msg_string);
	    check_str(obj,info.message.action1_label);
	    check_str(obj,info.message.action2_label);
	break;

        case AB_TYPE_DIALOG:
        case AB_TYPE_BASE_WINDOW:
        {
	    check_str(obj,info.window.icon);
	    check_str(obj,info.window.icon_label);
        }
	break;

        case AB_TYPE_TEXT_FIELD:
        case AB_TYPE_TEXT_PANE:
        {
	    check_str(obj,info.text.initial_value_string);
        }
	break;
    } /* switch obj->type */
    if (obj->type == AB_TYPE_MODULE)
    {
	check_str(obj,info.module.file);
	check_str(obj,info.module.stubs_file);
	check_str(obj,info.module.ui_file);
    }
    if (obj->type == AB_TYPE_ITEM)
    {
	check_str(obj,info.item.accelerator);
    }
    if (obj->type == AB_TYPE_PROJECT)
    {
	check_str(obj,info.project.file);
	check_str(obj,info.project.stubs_file);
    }
    if (obj->type == AB_TYPE_TERM_PANE)
    {
	check_str(obj,info.term.process_string);
    }


    /*
     * Check names index (we know the strings are valid, now)
     * project name doesn't go in an index
     */
    if ((obj->type != AB_TYPE_PROJECT) && (obj->name != NULL))
    {
        namesList = NULL;
        for (parent = obj->parent; parent != NULL; parent = parent->parent)
        {
	    if (   (parent->type == AB_TYPE_MODULE)
		|| (parent->type == AB_TYPE_PROJECT) )
	    {
		break;
	    }
        }
	if (parent != NULL)
	{
	    switch (parent->type)
	    {
	        case AB_TYPE_MODULE: 
		    namesList = parent->info.module.obj_names_list;
	        break;
	        case AB_TYPE_PROJECT: 
		    namesList = parent->info.project.obj_names_list;
	        break;
	    }

	    if (namesList == NULL)
	    {
	        util_dprintf(1, 
		    "No names index found containing %s\n", obj_name);
	        return_value = -1;
	        goto epilogue;
	    }
	    else
	    {
		/*
		 * A destroyed object must *not* be in the index,
		 * but any other object *must* be in the index.
		 */
	        tmpObj = (ABObj)strlist_get_istr_data(namesList, obj->name);
		if (obj_has_impl_flags(obj, ObjFlagDestroyed))
		{
		    if (tmpObj != NULL)
		    {
			util_dprintf(1, 
			    "Destroyed object is in names index: %s\n",
			    obj_name);
			return_value = -1;
			goto epilogue;
		    }
		}
		else
		{
		    if (tmpObj != obj)
		    {
		        util_dprintf(1, "Object does not exist in index: %s\n",
			    obj_name);
	                return_value = -1;
		        goto epilogue;
		    }
	        }
	    }
	} /* parent != NULL */
    } /* obj->name != NULL */

epilogue:
#ifdef DEBUG
    if (objIsAccessible)
    {
        obj->debug_last_verify_time = time(NULL); /*only exists in debug build*/
    }
#endif /* DEBUG */
    sig_fault_handler_uninstall();
    return return_value;
#undef check_str
#undef field_err
#undef safe_data_access
}


/*
 * Returns whether or not we can actually examine this object without
 * causing a memory fault
 *
 * Assumes: memory fault handler is installed
 * Modifies: sigjmp_env global var
 */
static BOOL 
objP_is_accessible(ABObj obj)
{
    volatile BOOL		isIt = TRUE;
    volatile unsigned char	*volatile objData = (unsigned char *)obj;
    volatile int		i;
    volatile unsigned char	oneByte = 0;

    mem_fault_occurred = FALSE;
    if (sigsetjmp(sigjmp_env, TRUE) != 0)
    {
	isIt = FALSE;
	goto epilogue;
    }

    for (i = 0 ; i < sizeof(*obj); ++i)
    {
	oneByte = objData[i];
    }

epilogue:
    return isIt;
}


static int
sig_fault_handler_install(void)
{
    struct sigaction	new_action;

    mem_fault_occurred = FALSE;
    if (sig_fault_handler_installed)
    {
	return 0;
    }

    if (sigaction(SIGSEGV, NULL, &org_sig_segv_handler) < 0)
    {
	return -1;
    }
    new_action = org_sig_segv_handler;
    new_action.sa_handler = sig_fault_handler;
    sigemptyset(&(new_action.sa_mask));
    new_action.sa_flags = 0;
    if (sigaction(SIGSEGV, &new_action, NULL) < 0)
    {
	return -1;
    }

    sig_fault_handler_installed = TRUE;

#ifdef SIGBUS
    if (sigaction(SIGBUS, NULL, &org_sig_bus_handler) < 0)
    {
	return -1;
    }
    new_action = org_sig_bus_handler;
    new_action.sa_handler = sig_fault_handler;
    sigemptyset(&(new_action.sa_mask));
    new_action.sa_flags = 0;
    if (sigaction(SIGBUS, &new_action, NULL) < 0)
    {
	return -1;
    }
#endif /* SIGBUS */

    return 0;
}


static int
sig_fault_handler_uninstall(void)
{
    mem_fault_occurred = FALSE;
    if (!sig_fault_handler_installed)
    {
	return 0;
    }

    if (sigaction(SIGSEGV, &org_sig_segv_handler, NULL) < 0)
    {
	return -1;
    }
    sig_fault_handler_installed = FALSE;

#ifdef SIGBUS
    if (sigaction(SIGBUS, &org_sig_bus_handler, NULL) < 0)
    {
	return -1;
    }
#endif /* SIGBUS */

    return 0;
}


static void
sig_fault_handler()
{
    if (mem_fault_occurred)
    {
	/* We should have cleared this to prepare for a mem fault */
	/* If not, then something has gone haywire */
        static STRING msg = NULL;
	msg = "MEMORY ACCESS VIOLATION OCCURED. ABORTING.\n";
	write(2, msg, strlen(msg));
	abort();
    }
    mem_fault_occurred = TRUE;
    siglongjmp(sigjmp_env, 1);
}


int
obj_tree_verify(ABObj root)
{
    int                 return_value = 0;
    int			rc = 0;		/* return code */
    ABObj		*objArray = NULL;
    int			objArraySize = 0;
    ABObj		module = NULL;
    AB_TRAVERSAL	moduleTrav;
    int			i = 0;

    if (root == NULL)
    {
	/* empty tree is valid (I guess) */
	return 0;
    }

    rc = build_obj_array(&objArray, &objArraySize, root);
    if (rc < 0)
    {
        return_value = rc;
        goto epilogue;
    }

    for (i = 0; i < objArraySize; ++i)
    {
	rc = obj_verify(objArray[i]);
	if (rc < 0)
	{
	    return_value = rc;
	    goto epilogue;
	}
    }

    /* 
     * Check the project names list
     */
    if (obj_is_project(root))
    {
        rc = verify_the_silly_index(root, objArray, objArraySize);
	if (rc < 0)
	{
	    return_value = rc;
	    goto epilogue;
	}
    }

    /* 
     * check the modules' name lists
     */
    for (trav_open(&moduleTrav, root, AB_TRAV_MODULES);
	(module = trav_next(&moduleTrav)) != NULL; )
    {
	rc = verify_the_silly_index(module, objArray, objArraySize);
	if (rc < 0)
	{
	    return_value = rc;
	    break;
	}
    }
    trav_close(&moduleTrav);

epilogue:
    util_free(objArray);
    return return_value;
}


static int	
verify_the_silly_index(
			ABObj		nameScopeObj,
			ABObj		*objArray, 
			int		objArraySize
)
{
    int		return_value = 0;
    StringList	names = NULL;
    int		numNames = 0;
    int		nameCount = 0;
    int		objCount = 0;
    ABObj	namedObj = NULL;
    ABObj	namesObj = NULL;
    ISTRING	curName = NULL;
    ABObj	curObj = NULL;
    ABObj	curScopeObj = NULL;
    StringList	curNames = NULL;
    BOOL	namedObjFound = FALSE;
    char	nameBuf1[1024];
    char	nameBuf2[1024];
    char	nameBuf3[1024];
    *nameBuf1 = 0;
    *nameBuf2 = 0;
    *nameBuf3 = 0;


    names = objP_get_names_list(nameScopeObj);
    if (names == NULL)
    {
	return 0;
    }
    numNames = strlist_get_num_strs(names);

    for (nameCount = 0; nameCount < numNames; ++nameCount)
    {
	namedObjFound = FALSE;
	curName = strlist_get_istr(names, nameCount, (void **)&namedObj);
	for (objCount = 0; 
	     (!namedObjFound) && (objCount < objArraySize); ++objCount)
	{
	    /*
	     * See if we've found the object
	     */
	    curObj = objArray[objCount];
	    if ((curObj != namedObj) || obj_is_project(curObj))
	    {
		/* projects don't go in list, anywhere */
		continue;
	    }

	    /*
	     * We've found the object that goes with this name. Check
	     * to see if it is in the right scope.
	     */
	    curNames = NULL;
	    curScopeObj = objP_get_names_scope_obj(curObj);
	    if (curScopeObj != NULL)
	    {
	        curNames = objP_get_names_list(curScopeObj);
	    }
	    if (curNames == NULL)
	    {
		util_dprintf(1, 
		    "No names list found for object %s in module %s!\n",
		    obj_get_safe_name(curObj, nameBuf1, 1024),
		    obj_get_safe_name(obj_get_module(curObj), nameBuf2, 1024));
		return_value = ERR_INTERNAL;
		goto epilogue;
	    }
	    else if (curNames != names)
	    {
		util_dprintf(1,
	     "Object %s should be in scope for %s, but is in scope for %s\n",
		    obj_get_safe_name(curObj, nameBuf1, 1024),
		    obj_get_safe_name(curScopeObj, nameBuf2, 1024),
		    obj_get_safe_name(nameScopeObj, nameBuf3, 1024));
		return_value = ERR_INTERNAL;
		goto epilogue;
	    }
	    else if (!istr_equal(curName, curObj->name))
	    {
		util_dprintf(1,
		    "Object %s has incorrect reference in index!\n",
			obj_get_safe_name(curObj, nameBuf1, 1024));
	    }
	    else
	    {
		namedObjFound = TRUE;
	    }
	} /* for objCount */

	if (!namedObjFound)
	{
#ifdef DEBUG
	    util_dprintf(1,
		"Name is in index '%s', but no such object exists!\n",
		istr_string_safe(curName));
#endif /* DEBUG */
	    return_value = ERR_INTERNAL;
	    goto epilogue;
	}
    } /* for nameCount */

epilogue:
    return return_value;
}


static int
build_obj_array(ABObj **objArrayPtr, int *objArraySizePtr, ABObj root)
{
#define objArray (*objArrayPtr)
#define objArraySize (*objArraySizePtr)
    int		rc = 0;			/* return code */
    ABObj	*newObjArray = NULL;
    ABObj	child = NULL;

    /*
     * If this object is already in the list, we've detected some sort
     * of cycle.
     */
    {
	int	i = 0;
	char	name[1024];
	*name = 0;
        for (i = 0; i < objArraySize; ++i)
	{
	    if (objArray[i] == root)
	    {
		util_dprintf(1, 
		 "INTERNAL ERROR: some sort of cycle detected involving %s\n",
		 obj_get_safe_name(root, name, 1024));
		return -1;
	    }
	}
    }

    /*
     * Add root obj to list
     */
    ++objArraySize;
    newObjArray = (ABObj*)realloc(objArray, objArraySize*sizeof(ABObj*));
    if (newObjArray == NULL)
    {
	util_dprintf(1, "Out of memory in build_obj_array\n");
	return ERR_NO_MEMORY;
    }
    objArray = newObjArray;
    objArray[objArraySize-1] = root;

    /*
     * Add the children to the list
     */
    for (child = root->first_child; child != NULL; child = child->next_sibling)
    {
	if ((rc = build_obj_array(&objArray, &objArraySize, child)) < 0)
	{
	    return rc;
	}
    }

    return 0;

#undef objArrayPtr
#undef objArray
}

ABObj
obj_get_parent_of_type(ABObj obj, AB_OBJECT_TYPE type)
{
    ABObj               ancestor = obj->parent;
    while ((ancestor != NULL) && (obj_get_type(ancestor) != type))
    {
	ancestor = ancestor->parent;
    }
    return ancestor;
}
