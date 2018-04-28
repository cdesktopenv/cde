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
 * $XConsortium: gil_loadatt.c /main/3 1995/11/06 18:28:12 rswiston $
 * 
 * @(#)gil_loadatt.c    1.26 13 Feb 1994        cde_app_builder/src/libABil
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
 * gil_loadatt.c - Load attributes from GIL file
 * 
 * NOTE: the only exported identifier in this file is abo_load_attribute.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>
#include <ab/util_types.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/istr.h>
#include "../libABobj/obj_utils.h"
#include "gil.h"
#include "gilP.h"
#include "gil_loadattP.h"
#include "load.h"
#include "loadP.h"

#ifndef util_dassert
#ifdef DEBUG
    #define util_dassert(_debugLevel, _boolExpr) \
	    {if (debug_level() >= (_debugLevel)) {assert((_boolExpr));}}
#else
    #define util_dassert(_debugLevel, _boolExpr) \
	    /* ignore this */;
#endif
#endif
#ifndef util_dabort
#ifdef DEBUG
    #define util_dabort(_lvl) {if (debug_level() >= (_lvl)) {abort();}}
#else
    #define util_dabort(_lvl) /* ignore this */;
#endif
#endif


typedef int         LOADATT_FUNC(FILE * inFile, ABObj obj, ABObj root_obj);
typedef LOADATT_FUNC *LOADATT_FUNC_PTR;


/*************************************************************************
**************************************************************************
**                                                                      **
**              LOAD PIECES OF AN ATTRIBUTE                             **
**                                                                      **
**************************************************************************
**************************************************************************/

/*
 * group could be a group or a stack
 */
static int
find_or_create_members(ABObj group, ABObj root, ISTRING_ARRAY * names)
{
    ISTRING             member_name = NULL;
    ABObj 		member = NULL;
    ABObj               memberParent = NULL;
    BOOL                group_reparented = FALSE;
    int                 i;

    for (i = 0; i < names->count; ++i)
    {
        /* move the member under the group */
        member_name = names->strings[i];
        member = obj_scoped_find_or_create_undef(root,
                                 istr_string(member_name), AB_TYPE_UNDEF);
        memberParent = obj_get_parent(member);
        obj_reparent(member, group);
        if ((!group_reparented) && (memberParent != NULL))
        {
            group_reparented = TRUE;
            obj_reparent(group, memberParent);
        }
    }

    return 0;
}

/*
 * Maps from initial state to ABObj attributes
 */
static int
gilP_obj_set_initial_state(ABObj obj, AB_OBJECT_STATE initial_state)
{
    switch (initial_state)
    {
	case AB_STATE_ACTIVE:
	    obj_set_is_initially_active(obj, TRUE);
	break;
	case AB_STATE_ICONIC:
	    obj_set_is_initially_iconic(obj, TRUE);
	break;
	case AB_STATE_INACTIVE:
	    obj_set_is_initially_active(obj, FALSE);
	break;
	case AB_STATE_INVISIBLE:
	    obj_set_is_initially_visible(obj, FALSE);
	break;
	case AB_STATE_NOTSELECTED:
	    obj_set_is_initially_selected(obj, FALSE);
	break;
	case AB_STATE_OPEN:
	    obj_set_is_initially_visible(obj, TRUE);
	break;
	case AB_STATE_SELECTED:
	    obj_set_is_initially_selected(obj, TRUE);
	break;
	case AB_STATE_VISIBLE:
	    obj_set_is_initially_visible(obj, TRUE);
	break;
    }

    return 0;
}

static int
set_children_accelerators(ABObj obj, ISTRING_ARRAY * strings)
{
    int                 return_value = 0;
    int                 rc = 0;
    int                 num_strings = strings->count;
    int                 i;
    ABObj               child_obj;
    AB_TRAVERSAL        trav;

    if (num_strings <= 0)
    {
        return 0;
    }
    if ((rc = obj_ensure_num_children(obj, num_strings)) < 0)
    {
        return rc;
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN), i = 0;
         (child_obj = trav_next(&trav)) != NULL; ++i)
    {
        /* for some reason, missing accelerators are stored as "" */
        if ((int) istr_len(strings->strings[i]) > 0)
        {
            obj_set_accelerator(child_obj, istr_string(strings->strings[i]));
        }
    }
    trav_close(&trav);
    return return_value;
}

static int
set_children_defaults(ABObj obj, BOOL_ARRAY * defaults)
{
    int                 return_value = 0;
    int                 rc = 0;
    int                 num_defaults = defaults->count;
    int                 i;
    ABObj               child_obj;
    AB_TRAVERSAL        trav;
    if (num_defaults <= 0)
    {
        return -1;
    }
    if ((rc = obj_ensure_num_children(obj, num_defaults)) < 0)
    {
        return rc;
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN), i = 0;
         (child_obj = trav_next(&trav)) != NULL; ++i)
    {
        obj_set_is_default(child_obj, defaults->bools[i]);
    }
    return return_value;
}

static int
set_children_fgcolors(ABObj obj, ISTRING_ARRAY * colors)
{
    int                 num_colors = colors->count;
    int                 i;
    ABObj               child_obj;
    AB_TRAVERSAL        trav;
    if (num_colors <= 0)
    {
        return 0;
    }
    if (obj_ensure_num_children(obj, num_colors) < 0)
    {
        return 0;
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN), i = 0;
         (child_obj = trav_next(&trav)) != NULL; ++i)
    {
        obj_set_fg_color(child_obj, istr_string(colors->strings[i]));
        istr_destroy(colors->strings[i]);
    }
    return 0;
}

static int
set_children_initially_selected(ABObj obj, BOOL_ARRAY * selects)
{
    int                 num_selects = selects->count;
    int                 i;
    ABObj               child_obj;
    AB_TRAVERSAL        trav;
    if (num_selects <= 0)
    {
        return 0;
    }
    if (obj_ensure_num_children(obj, num_selects) < 0)
    {
        return 0;
    }
    if (!obj_is_item(obj))
    {
        return 0;
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN), i = 0;
         (child_obj = trav_next(&trav)) != NULL; ++i)
    {
        obj_set_is_initially_selected(child_obj, selects->bools[i]);
    }
    return 0;
}

static int
set_item_labels(ABObj obj, ISTRING_ARRAY * strings)
{
    int                 num_strings = strings->count;
    int                 i = 0;
    ABObj               item = NULL;
    AB_TRAVERSAL        trav;
    STRING		label = NULL;

    if (num_strings <= 0)
    {
        return 0;
    }
    if (obj_ensure_num_children(obj, num_strings) < 0)
    {
        return 0;
    }
    for (trav_open(&trav, obj, AB_TRAV_ITEMS), i = 0;
         (item = trav_next(&trav)) != NULL; ++i)
    {
	label = istr_string(strings->strings[i]);
	if ( (obj_is_menu_item(item)) && (label == NULL) )
	{
	    obj_set_label_type(item, AB_LABEL_SEPARATOR);
	}
	else
	{
	    if (label == NULL)
	    {
		label = Util_empty_string;
	    }
            obj_set_label_type(item, AB_LABEL_STRING);
            obj_set_label(item, label);
	}

        if (obj_is_item(item))
        {
	    char	name[MAXPATHLEN+1];
	    *name = 0;
            obj_set_name_from_label(item, NULL);

	    /*
	     * Append _item - this is what dtbuilder does
	     */
	    util_dassert(1, (obj_get_name(item) != NULL));
	    util_strncpy(name, obj_get_name(item), MAXPATHLEN+1);
	    strcat(name, "_item");
	    obj_set_unique_name(item, name);
        }

	istr_destroy(strings->strings[i]);
    }
    trav_close(&trav);
    return 0;
}

static int
set_item_label_types(ABObj obj, LABEL_TYPE_ARRAY * label_types)
{
    int                 num_types = label_types->count;
    int                 i = 0;
    ABObj               item = NULL;
    AB_TRAVERSAL        trav;
    AB_LABEL_TYPE	label_type = AB_LABEL_UNDEF;

    if (num_types <= 0)
    {
        return 0;
    }
    if (obj_ensure_num_children(obj, num_types) < 0)
    {
        return 0;
    }
    for (trav_open(&trav, obj, AB_TRAV_ITEMS), i = 0;
         (item = trav_next(&trav)) != NULL; ++i)
    {
	label_type = label_types->label_types[i];

	if (obj_get_label_type(item) != AB_LABEL_SEPARATOR)
	{
	    /* if it's already a separator, it has been set elsewhere */
            obj_set_label_type(item, label_type);
	}
    }
    trav_close(&trav);

    return 0;
}

static int
set_children_menu_names(ABObj obj, ISTRING_ARRAY * strings)
{
    int                 num_strings = strings->count;
    int                 i;
    ABObj               child_obj;
    AB_TRAVERSAL        trav;

    if (num_strings <= 0)
    {
        return 0;
    }
    if (obj_ensure_num_children(obj, num_strings) < 0)
    {
        return 0;
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN), i = 0;
         (child_obj = trav_next(&trav)) != NULL; ++i)
    {
        obj_set_menu_name(child_obj, istr_string(strings->strings[i]));
        istr_destroy(strings->strings[i]);
    }
    trav_close(&trav);
    return 0;
}

/*
 * Load an array of booleans from the input file into an array terminated by
 * -1.  Returns NULL if successful, otherwise an error message.
 */
int
gilP_load_bools(FILE * inFile, BOOL_ARRAY * bools)
{
    int                 i;
    BOOL		boolValue;

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN); ++i)
    {
        if (!abio_gil_get_boolean(inFile, &boolValue))
        {
            abil_print_load_err(ERR_WANT_BOOLEAN);
            return -1;
        }
        bools->bools[i] = boolValue;
    }
    bools->count = i;

    return 0;
}

/*
 * Load an array of event types from the input file into an array. Terminate
 * the array with -1.  Return NULL if successful, otherwise an error message.
 */
static int
load_events(FILE * inFile, AB_WHEN ** p)
{
#define INC     10              /* array buffer increment */
#define SIZE    (sizeof (AB_WHEN*))     /* array element size */
    int                 lth = 0;/* array length */
    int                 c;      /* array count */
    ISTRING             s = NULL;       /* work string */

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    *p = (AB_WHEN *) util_malloc((lth += INC) * SIZE);

    for (c = 0; !abio_get_list_end(inFile); c++)
    {
        if (c + 1 == lth)
            *p = (AB_WHEN *) realloc(*p, (lth += INC) * SIZE);
        if (!abio_get_keyword(inFile, &s))
        {
            abil_print_load_err(ERR_WANT_KEYWORD);
            return -1;
        }
        (*p)[c] = gilP_string_to_when(istr_string(s));
    }

    (*p)[c] = (AB_WHEN) - 1;
    return 0;
#undef  INC
#undef  SIZE
}


/*
 * Load one handler name
 */
int
gilP_load_handler(FILE * inFile, ISTRING * handler)
{
    return gilP_load_name(inFile, handler);
}


/*
 * Load a null-terminated array of names from the input file into an array.
 * Returns NULL if successful, otherwise an error message.
 * 
 * Checks strings with abo_ident_is_ok
 */
int
gilP_load_handlers(FILE * inFile, ISTRING_ARRAY * handlers)
{
    int                 rc = 0;
    int                 return_value = 0;
    int                 i;

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN); ++i)
    {
        if ((rc = gilP_load_handler(inFile, &(handlers->strings[i]))) < 0)
        {
            return_value = rc;
            break;
        }
    }
    handlers->count = i;

    if (return_value < 0)
    {
        /** free the list of strings */
        for (i = 0; i < handlers->count; ++i)
        {
            istr_destroy(handlers->strings[i]);
        }
        handlers->count = 0;
    }

    return return_value;
}

/*
 * Loads a name. Checks the value with abo_ident_is_ok
 */
int
gilP_load_name(FILE * inFile, ISTRING * name)
{
    ISTRING             string_name = NULL;

    if (!abio_gil_get_name(inFile, &string_name))
    {
        abil_print_load_err(ERR_WANT_NAME);
        return -1;
    }
    if ((string_name != NULL) && (!ab_c_ident_is_ok(istr_string(string_name))))
    {
        abil_print_load_err(ERR_BAD_IDENTIFIER);
        return -1;
    }
    *name = istr_dup(string_name);
    istr_destroy(string_name);
    return 0;
}


/*
 * Load a null-terminated array of names from the input file into an array.
 * Returns NULL if successful, otherwise an error message.
 */
int
gilP_load_names(FILE * inFile, ISTRING_ARRAY * names)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    int                 i;

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN); ++i)
    {
        if ((rc = gilP_load_name(inFile, &(names->strings[i]))) < 0)
        {
            return_value = rc;
            break;
        }
    }
    names->count = i;

    if (return_value < 0)
    {
        for (i = 0; i < names->count; ++i)
        {
            istr_destroy(names->strings[i]);
        }
        names->count = 0;
    }
    return return_value;
}

/*
 * Loads a name, without checking to see if it is valid
 */
int
gilP_load_file_name(FILE * inFile, ISTRING * name)
{
    ISTRING             charname;
    if (!abio_gil_get_name(inFile, &charname))
    {
        abil_print_load_err(ERR_WANT_NAME);
        return -1;
    }
    *name = istr_dup(charname);
    return 0;
}

/*
 * Load a null-terminated array of names from the input file into an array.
 * Returns NULL if successful, otherwise an error message.
 */
int
gilP_load_file_names(FILE * inFile, ISTRING_ARRAY * names)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    int                 i;

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN); ++i)
    {
        if ((rc = gilP_load_file_name(inFile, &(names->strings[i]))) < 0)
        {
            return_value = rc;
            break;
        }
    }
    names->count = i;

    if (return_value < 0)
    {
        istr_array_uninit(names);
    }
    return return_value;
}

/*
 * Load an array of label types from the input file into an array. Terminate
 * the array with -1.  Return NULL if successsful, otherwise an error
 * message.
 */
int
gilP_load_label_types(FILE * inFile, LABEL_TYPE_ARRAY * label_types)
{
    int                 i;      /* array count */
    ISTRING             keyword;/* work string */

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN);
         ++i)
    {
        if (!abio_get_keyword(inFile, &keyword))
        {
            abil_print_load_err(ERR_WANT_KEYWORD);
            return -1;
        }
        label_types->label_types[i] =
            gilP_string_to_label_type(istr_string(keyword));
    }
    label_types->count = i;

    return 0;
}

/*
 * Load an array of initial states from the input file into an array.
 * Terminate the array with -1.  Return NULL if successsful, otherwise an
 * error message.
 */
int
gilP_load_initial_states(FILE * inFile, INITIAL_STATE_ARRAY * states)
{
    int                 i;
    ISTRING             keyword;

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN); ++i)
    {
        if (!abio_get_keyword(inFile, &keyword))
        {
            abil_print_load_err(ERR_WANT_KEYWORD);
            return -1;
        }
        states->states[i] = gilP_string_to_object_state(istr_string(keyword));
    }
    states->count = i;

    return 0;
}


/*
 * Loads in one string.  Spaces is allocated for the new string.
 */
int
gilP_load_string(FILE * inFile, ISTRING * string)
{
    if (!abio_get_string(inFile, string))
    {
        abil_print_load_err(ERR_WANT_STRING);
        return -1;
    }
    return 0;
}

/*
 * Load a null-terminated array of strings from the input file into an array.
 * Returns NULL if successful, otherwise an error message.  Null strings are
 * turned into empty strings instead of being left NULL.
 */
int
gilP_load_strings(FILE * inFile, ISTRING_ARRAY * strings)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    int                 i;      /* array count */
    ISTRING             string;

    if (!abio_get_list_begin(inFile))
    {
        abil_print_load_err(ERR_WANT_LIST);
        return -1;
    }

    for (i = 0; (!abio_get_list_end(inFile)) && (i < AB_OBJ_MAX_CHILDREN);
         ++i)
    {
        if ((rc = gilP_load_string(inFile, &string)) < 0)
        {
            return rc;
        }
        strings->strings[i] = (string == NULL ? istr_create("") : string);
    }
    strings->count = i;

    return return_value;
}



/*************************************************************************
**************************************************************************
**                                                                      **
**              LOAD SPECIFIC ATTRIBUTES                                **
**                                                                      **
**      load_attribute calls these functions.                           **
**************************************************************************
**************************************************************************/

/*
 * */
static int
load_att_(ABObj obj, ABObj root_obj)
{

    return 0;
}

static int
load_att_anchor_object(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             anchor = NULL;

    if ((rc = gilP_load_name(inFile, &anchor)) < 0)
    {
        return_value = rc;
    }
    /* obj->info.group.anchor_obj= anchor; REMIND: what about anchors? */
    istr_destroy(anchor);
    return return_value;
}

static int
load_att_anchor_point(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             compass_str;
    if (!abio_gil_get_name(inFile, &compass_str))
    {
        abil_print_load_err(ERR_WANT_NAME);
        return -1;
    }
    /** REMIND: implement anchors
    if ((obj->info.group.anchor_point=
            gilP_string_to_compass_point(istr_string(compass_str)))
                    == ERROR)
    {
        abil_print_load_err(ERR_UNKNOWN);
        return -1;
    }
    **/
    return 0;
}

static int
load_att_background_color(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             bg_color;
    if (!abio_get_string(inFile, &bg_color))
    {
        abil_print_load_err(ERR_WANT_STRING);
        return -1;
    }
    if (bg_color != NULL)
    {
        obj_set_bg_color(obj, istr_string(bg_color));
        istr_destroy(bg_color);
    }
    return 0;
}

/*
 * */
static int
load_att_busy_drop_glyph(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 rc = 0;
    ISTRING             glyph = NULL;

    if ((rc = gilP_load_string(inFile, &glyph)) < 0)
    {
        return rc;
    }

    return 0;
}

static int
load_att_button_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             button_type_string;
    AB_BUTTON_TYPE      button_type;

    if (!abio_get_keyword(inFile, &button_type_string))
    {
        abil_print_load_err(ERR_WANT_KEYWORD);
        return -1;
    }
    if ((button_type = gilP_string_to_button_type(istr_string(button_type_string)))
        == ERROR)
    {
        abil_print_load_err(ERR_UNKNOWN);
        return -1;
    }
    else
    {
        obj->info.button.type = button_type;
    }
    return 0;
}

/*
 * for choice labels
 */
static int
load_att_choices(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       choices;

    if ((rc = gilP_load_strings(inFile, &choices)) < 0)
    {
        return rc;
    }
    return_value = rc = set_item_labels(obj, &choices);

    return return_value;
}

/*
 * for choice colors
 */
static int
load_att_choice_colors(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       colors;

    if ((rc = gilP_load_strings(inFile, &colors)) < 0)
    {
        return rc;
    }
    return_value = rc = set_children_fgcolors(obj, &colors);
    return return_value;
}

/*
 * */
static int
load_att_choice_defaults(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    BOOL_ARRAY          defaults;

    if ((rc = gilP_load_bools(inFile, &defaults)) < 0)
    {
        return rc;
    }
    return_value = rc = set_children_defaults(obj, &defaults);

    return return_value;
}

/*
 * for choice type - glyph or string ?
 */
static int
load_att_choice_label_types(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    LABEL_TYPE_ARRAY    label_types;

    if ((rc = gilP_load_label_types(inFile, &label_types)) < 0)
    {
        return rc;
    }
    return_value = rc = set_item_label_types(obj, &label_types);
    return return_value;
}

static int
load_att_col_alignment(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             align_string;

    if (!abio_get_keyword(inFile, &align_string))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((obj->info.container.col_align =
         gilP_string_to_alignment(istr_string(align_string)))
        == ERROR)
    {
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }

    return 0;
}

/*
 * only for menus and choices
 */
static int
load_att_columns(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 num_columns;

    if (!abio_get_integer(inFile, &num_columns))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    /***** jjd
    if (!obj->info.group)
            NewLayoutInfo(obj);
    *****/

    switch (obj->type)
    {
    case AB_TYPE_MENU:
        if (num_columns > 0)
        {

            /*
             * value is # of items
             */
        }
        break;

    default:
        obj_set_num_columns(obj, num_columns);
        break;
    }

    return 0;
}

/*
 * buttons - use width & height ?
 */
static int
load_att_constant_width(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL	constant_width = FALSE;

    if (!abio_gil_get_boolean(inFile, &constant_width))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    obj_set_resizable(obj, constant_width);

    return 0;
}

static int
load_att_default_drop_site(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL	default_drop = FALSE;

    if (!abio_gil_get_boolean(inFile, &default_drop))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

static int
load_att_dnd_accept_cursor(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             accept_cursor;

    if (!abio_get_string(inFile, &accept_cursor))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    istr_destroy(accept_cursor);
    return 0;
}

static int
load_att_dnd_accept_cursor_xhot(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 xhot;

    if (!abio_get_integer(inFile, &xhot))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

static int
load_att_dnd_accept_cursor_yhot(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 yhot;

    if (!abio_get_integer(inFile, &yhot))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

static int
load_att_dnd_cursor(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             dnd_cursor;
    if (!abio_get_string(inFile, &dnd_cursor))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    istr_destroy(dnd_cursor);

    return 0;
}

static int
load_att_dnd_cursor_xhot(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 xhot;
    if (!abio_get_integer(inFile, &xhot))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    return 0;
}

static int
load_att_dnd_cursor_yhot(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 yhot;
    if (!abio_get_integer(inFile, &yhot))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    return 0;
}

static int
load_att_draggable(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL	draggable = FALSE;

    if (!abio_gil_get_boolean(inFile, &draggable))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

static int
load_att_drawing_model(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             drawing_model_string;

    if (!abio_get_keyword(inFile, &drawing_model_string))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);

    return 0;
}

static int
load_att_drop_target_width(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 width;
    if (!abio_get_integer(inFile, &width))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

static int
load_att_droppable(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL	tmp_bool = FALSE;
    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

static int
load_att_events(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    AB_WHEN            *events = NULL;

    /* do later - currently returns (AB_WHEN *) */
    if ((rc = load_events(inFile, &events)) < 0)
        return rc;
    util_free(events);
    return return_value;
}

static int
load_att_file_chooser_filter_pattern(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);

    if (tmp_str != NULL)
    {
        obj_set_filter_pattern(obj, istr_string(tmp_str));
    }
    istr_destroy(tmp_str);

    return 0;
}

static int
load_att_file_chooser_match_glyph(FILE * inFile, ABObj obj, ABObj root_obj) 
{
    ISTRING              tmp_str = NULL; 
    if (!abio_get_string(inFile, &tmp_str)) 
	return (abil_print_load_err(ERR_WANT_STRING),-1);
	
    abil_print_load_err(ERR_NOT_IMPL);
    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_file_chooser_match_glyph_mask(
			FILE * inFile, ABObj obj, ABObj root_obj) 
{
    ISTRING              tmp_str = NULL; 
    if (!abio_get_string(inFile, &tmp_str)) 
	return (abil_print_load_err(ERR_WANT_STRING),-1);
	
    abil_print_load_err(ERR_NOT_IMPL);
    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_file_chooser_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);

    return 0;
}

static int
load_att_foreground_color(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    if (tmp_str != NULL)
    {
        if ((obj_is_control(obj) && obj->parent) ||
            obj->type == AB_TYPE_TEXT_PANE)
        {
            if (obj->type == AB_TYPE_SCALE)
            {
                obj_set_bg_color(obj, istr_string(tmp_str));
            }
        }
        else if (obj_is_window(obj) || obj_is_pane(obj))
        {
            obj_set_fg_color(obj, istr_string(tmp_str));
        }
    }
    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_group_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((obj->info.container.group_type =
         gilP_string_to_group_type(istr_string(tmp_str)))
        == ERROR)
    {
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }

    return 0;
}

static int
load_att_height(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    obj->height = tmp_int;

    return 0;
}

static int
load_att_help(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    /* REMIND: what about help? */
    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    istr_destroy(tmp_str);

    return 0;
}

static int
load_att_hoffset(FILE * inFile, ABObj obj, ABObj root_obj)
{
    if (!abio_get_integer(inFile, &obj->info.container.hoffset))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

static int
load_att_hscroll(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL	tmp_bool = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    obj->info.drawing_area.hscrollbar =
        (tmp_bool ? AB_SCROLLBAR_WHEN_NEEDED : AB_SCROLLBAR_NEVER);

    return 0;
}

static int
load_att_hspacing(FILE * inFile, ABObj obj, ABObj root_obj)
{
    if (!abio_get_integer(inFile, &obj->info.container.hspacing))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);


    return 0;
}

/*
 * for base window - icon file
 */
static int
load_att_icon_file(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    istr_destroy(tmp_str);

    return 0;
}

static int
load_att_icon_label(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    if (tmp_str != NULL)
    {
        obj_set_icon_label(obj, istr_string(tmp_str));
    }
    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_icon_mask_file(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    /* for base window - icon mask file */
    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    istr_destroy(tmp_str);


    return 0;
}

static int
load_att_initial_list_glyphs(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ABObj               child = NULL;

    if ((rc = load_att_choices(inFile, obj, root_obj)) < 0)
    {
        return rc;
    }
    for (child = obj->first_child; child != NULL; child = child->next_sibling)
    {
        child->label_type = AB_LABEL_GLYPH;
    }

    return return_value;
}

static int
load_att_initial_list_values(FILE * inFile, ABObj obj, ABObj root_obj)
{
    return load_att_choices(inFile, obj, root_obj);
}

static int
load_att_initial_selections(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    BOOL_ARRAY          bools;

    if ((rc = gilP_load_bools(inFile, &bools)) < 0)
    {
        return rc;
    }
    return_value = rc = set_children_initially_selected(obj, &bools);
    return return_value;
}

static int
load_att_initial_state(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;
    AB_OBJECT_STATE	initial_state = AB_STATE_UNDEF;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    initial_state = gilP_string_to_object_state(istr_string(tmp_str));
    if (initial_state == AB_STATE_UNDEF)
    {
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    }
    gilP_obj_set_initial_state(obj, initial_state);


    return 0;
}


/*
 * Loads in a list interface file names for a project
 */
static int
load_att_interfaces(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       interfaces;
    AB_TRAVERSAL        trav;
    ABObj               child;
    int                 i;
    char		objName[GIL_MAX_NAME_SIZE];
    int			objNameLen = 0;
    *objName = 0;

    istr_array_init(&interfaces);
    if ((rc = gilP_load_file_names(inFile, &interfaces)) < 0)
    {
        return rc;
    }
    if (obj_ensure_num_children(obj, interfaces.count) < 0)
    {
        return (abil_print_load_err(ERR_WANT_LIST), -1);
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN), i = 0;
         (child = trav_next(&trav)) != NULL; ++i)
    {
	obj_set_is_defined(child, FALSE);	/* not read in, yet */
	obj_set_type(child, AB_TYPE_MODULE);
        obj_set_file(child, istr_string(interfaces.strings[i]));
	util_strncpy(objName, obj_get_file(child), GIL_MAX_NAME_SIZE);
	objNameLen = strlen(objName);
	if (   (objNameLen >= 2)
	    && (   (util_streq(&(objName[objNameLen-2]), ".P"))
	        || (util_streq(&(objName[objNameLen-2]), ".G"))))
	{
	    objName[objNameLen-2] = 0;
	}
        obj_set_name(child, objName);
    }
    trav_close(&trav);

    istr_array_uninit(&interfaces);
    return return_value;
}


/*
 * Load in initial value. Can be a number or a string.
 */
static int
load_att_initial_value(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;
    ISTRING             tmp_str;

    switch (obj->type)
    {
    case AB_TYPE_SCALE:
        if (!abio_get_integer(inFile, &tmp_int))
            return (abil_print_load_err(ERR_WANT_INTEGER), -1);
        obj->info.scale.initial_value = tmp_int;
        break;

    case AB_TYPE_TEXT_FIELD:
        switch (obj->info.text.type)
        {
        case AB_TEXT_NUMERIC:
            abio_get_integer(inFile, &tmp_int);
            obj->info.text.initial_value_int = tmp_int;
            break;

        default:
            abio_get_string(inFile, &tmp_str);
            obj_set_initial_value_string(obj, istr_string(tmp_str));
            break;
        }
        break;
    default:
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }

    istr_destroy(tmp_str);
    return 0;
}



static int
load_att_label(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    obj_set_label(obj, istr_string(tmp_str));
    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_label_bold(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    /* only for static text widget - do later */
    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

static int
load_att_label_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    AB_LABEL_TYPE       label_type;
    ISTRING             tmp_str = NULL;

    /* do later - ignore for now */
    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((label_type = gilP_string_to_label_type(istr_string(tmp_str))) == ERROR)
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    obj->label_type = label_type;

    istr_destroy(tmp_str);
    return 0;
}

/*
 * What GIL called layout-type is actually orientation (horizontal, vertical)
 */
static int
load_att_layout_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    AB_ORIENTATION      orientation;
    ISTRING             tmp_str = NULL;

    /*
     * for gauge, scrolling-list, choice, scale text-fields
     */
    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((orientation = gilP_string_to_orientation(istr_string(tmp_str)))
        == ERROR)
    {
        return ERR_UNKNOWN;
    }
    obj_set_orientation(obj, orientation);

    switch (obj_get_orientation(obj))
    {
    case AB_ORIENT_HORIZONTAL:
        obj->info.container.row_align = AB_ALIGN_TOP;
        break;

    case AB_ORIENT_VERTICAL:
        obj->info.container.col_align = AB_ALIGN_LEFT;
        break;

    case ERROR:
    default:
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_mapped(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    /* for base Windows (shells) and popups iconic ?? */
    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

static int
load_att_max_tick_string(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_max_value(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    /* for gauge, scale and numeric text-field widget */
    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    switch (obj->type)
    {
    case AB_TYPE_SCALE:
        if (tmp_int > 0)
        {
            obj->info.scale.max_value = tmp_int;
        }
        break;

    case AB_TYPE_TEXT_FIELD:
        break;

    default:
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }

    return 0;
}

static int
load_att_max_value_string(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_members(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       name_list;

    istr_array_init(&name_list);
    if ((rc = gilP_load_names(inFile, &name_list)) < 0)
    {
        return rc;
    }

    if (!((obj->type == AB_TYPE_LAYERS) || (obj->type == AB_TYPE_CONTAINER)))
    {
	abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
    }
    else 
    {
	if (name_list.count == 0)
        {
            abil_print_load_err(WARN_NO_MEMBERS);
        }
        else
        {
            find_or_create_members(obj, root_obj, &name_list);
        }
    }

    istr_array_uninit(&name_list);
    return return_value;
}

/*
 * 
 */
static int
load_att_menu_item_accelerators(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       accels;

    if ((rc = gilP_load_strings(inFile, &accels)) < 0)
    {
        return rc;
    }
    set_children_accelerators(obj, &accels);

    return 0;
}

static int
load_att_menu_item_colors(FILE * inFile, ABObj obj, ABObj root_obj)
{
    return load_att_choice_colors(inFile, obj, root_obj);
}

/*
 * */
static int
load_att_menu_item_defaults(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    BOOL_ARRAY          bools;
    int                 i;
    ABObj               child = NULL;

    if ((rc = gilP_load_bools(inFile, &bools)) < 0)
    {
        return rc;
    }
    obj_ensure_num_children(obj, bools.count);
    for (i = 0, child = obj->first_child; child != NULL;
         ++i, child = child->next_sibling)
    {
        obj_set_is_initially_selected(child, bools.bools[i]);
    }

    return 0;
}

/*
 * */
static int
load_att_menu_item_labels(FILE * inFile, ABObj obj, ABObj root_obj)
{
    return load_att_choices(inFile, obj, root_obj);
}

/*
 * */
static int
load_att_menu_item_label_types(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    LABEL_TYPE_ARRAY    ltypes;
    int                 i;
    ABObj               child;

    if ((rc = gilP_load_label_types(inFile, &ltypes)) < 0)
    {
        return rc;
    }
    obj_ensure_num_children(obj, ltypes.count);
    for (child = obj->first_child, i = 0; child != NULL;
         child = child->next_sibling, ++i)
    {
        child->label_type = ltypes.label_types[i];
    }

    return 0;
}

/*
 * */
static int
load_att_menu_item_states(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    INITIAL_STATE_ARRAY states;
    int                 i;
    ABObj               child;

    if ((rc = gilP_load_initial_states(inFile, &states)) < 0)
    {
        return rc;
    }
    if (obj_ensure_num_children(obj, states.count) < 0)
    {
        return ERR_NO_MEMORY;
    }
    for (i = 0, child = obj->first_child; child != NULL;
         ++i, child = child->next_sibling)
    {
	gilP_obj_set_initial_state(child, states.states[i]);
    }

    return 0;
}

/*
 * */
static int
load_att_menu_item_menus(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       names;

    if ((rc = gilP_load_names(inFile, &names)) < 0)
    {
        return rc;
    }
    return set_children_menu_names(obj, &names);
}

/*
 * for button, drawing_area, control-area and scrolling- lists - name of menu
 * to be attached
 */
static int
load_att_menu(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             menu_name = NULL;

    if ((rc = gilP_load_name(inFile, &menu_name)) < 0)
    {
        return rc;
    }
    obj_set_menu_name(obj, istr_string(menu_name));

    istr_destroy(menu_name);
    return 0;
}

static int
load_att_menu_title(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    obj_set_label(obj, istr_string(tmp_str));
    obj->label_type = AB_LABEL_STRING;

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_menu_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    AB_MENU_TYPE        tmp_menu_type;
    ISTRING             tmp_str = NULL;

    /* can be command_menu, exclusive_menu and nonexclusive_menu */
    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((tmp_menu_type = gilP_string_to_menu_type(istr_string(tmp_str))) < 0)
    {
        abil_print_load_err(ERR_UNKNOWN_MENU_TYPE);
        tmp_menu_type = AB_MENU_PULLDOWN;
    }
    obj->info.menu.type = tmp_menu_type;

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_min_tick_string(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_min_value(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    obj_set_min_value(obj, tmp_int);
    return 0;
}

static int
load_att_min_value_string(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_multiple_selections(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		multiSelect = FALSE;

    /* for exclusive and non-exclusive in scrolling-list */
    if (!abio_gil_get_boolean(inFile, &multiSelect))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    if (obj->type == AB_TYPE_LIST)
    {
	if (multiSelect)
	    obj_set_selection_mode(obj, AB_SELECT_BROWSE_MULTIPLE);
	else
	    obj_set_selection_mode(obj, AB_SELECT_SINGLE);
    }
    else
    {
	abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
    }

    return 0;
}

/*
 * If an object with the given name exists, we're gonna grab it in place of
 * this one.
 */
static int
load_att_name(FILE * inFile, ABObj *pobj, ABObj root_obj)
{
#define obj (*pobj)
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             name = NULL;
    ABObj               old_obj = NULL;

    if ((rc = gilP_load_name(inFile, &name)) < 0)
    {
        return rc;
    }
    abil_loadmsg_set_object(istr_string(name));

    old_obj = obj_find_by_name(root_obj, istr_string(name));
    if ((old_obj != NULL) && (old_obj != obj))
    {
        if (   (!obj_is_defined(old_obj)) 
	    && (   (obj_get_type(old_obj) == AB_TYPE_UNDEF)
	        || (obj_get_type(old_obj) == obj_get_type(obj)) )
	   )
        {
            /* An undefined object with this name exists - it was */
            /* forward-referenced */
            util_dprintf(3, "Resolving forward reference: %s\n", 
					istr_string_safe(name));
            obj_replace(old_obj, obj);
            obj_destroy(old_obj);
            obj_set_name_istr(obj, name);
        }
        else
        {
            /* a duplicate name! */
            obj_set_unique_name_istr(obj, name);
            util_printf_err("Duplicate name.  Renaming %s -> %s.\n", 
		istr_string(name), obj_get_name(obj));
        }
    }
    else
    {
        /* brand new object! */
        obj_set_name(obj, istr_string(name));
    }
    abil_loadmsg_set_object(obj_get_name(obj));

    istr_destroy(name);
    return 0;
#undef obj
}

static int
load_att_normal_drop_glyph(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    istr_destroy(tmp_str);
    tmp_str = NULL;

    return 0;
}

/*
 * used for scales and gauges
 */
static int
load_att_orientation(FILE * inFile, ABObj obj, ABObj root_obj)
{
    AB_ORIENTATION      orientation;
    ISTRING             tmp_str = NULL;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    orientation = gilP_string_to_orientation(istr_string(tmp_str));
    if (obj_set_orientation(obj, orientation) < 0)
    {
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }

    istr_destroy(tmp_str);
    return 0;
}

static int
load_att_owner(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             owner_name = NULL;
    ABObj               parent_obj = NULL;
    char                real_name[256];

    if ((rc = gilP_load_name(inFile, &owner_name)) < 0)
    {
        return rc;
    }

    if (owner_name == NULL)
        return 0;               /* do nothing */

    parent_obj =
        obj_scoped_find_or_create_undef(root_obj,
                                  istr_string(owner_name), AB_TYPE_UNDEF);
    obj_append_child(parent_obj, obj);

    istr_destroy(owner_name);
    return 0;
}

/*
 * for menus
 */
static int
load_att_pinnable(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		pinnable = FALSE;

    /* REMIND: handle tear-off/pinnable menus */
    if (!abio_gil_get_boolean(inFile, &pinnable))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

/*
 * for pop-up windows
 */
static int
load_att_pinned(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

/*
 * for text-field, text and scrolling-list
 */
static int
load_att_read_only(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool  = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    obj_set_read_only(obj, tmp_bool);

    return 0;
}

static int
load_att_reference_point(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING              tmp_str = NULL;

    if (!abio_get_keyword(inFile, &tmp_str)) return
        abil_print_load_err(ERR_WANT_KEYWORD); 
	
    return 0;
}

static int
load_att_resizable(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return ERR_WANT_BOOLEAN;
    obj_set_resizable(obj, tmp_bool);

    return 0;
}

static int
load_att_row_alignment(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((obj->info.container.row_align =
         gilP_string_to_alignment(istr_string(tmp_str)))
        == ERROR)
        return (abil_print_load_err(ERR_UNKNOWN), -1);

    istr_destroy(tmp_str);
    return 0;
}

/*
 * for projects
 */
static int
load_att_root_window(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    ISTRING             root_window_name = NULL;
    ABObj               root_window = NULL;

    gilP_load_name(inFile, &root_window_name);

    if (root_window_name != NULL)
    {
        root_window = obj_scoped_find_or_create_undef(
                  root_obj, istr_string(root_window_name), AB_TYPE_BASE_WINDOW);
        if (root_window == NULL)
        {
            abil_print_load_err(ERR_WANT_NAME);
            return_value = -1;
	    goto epilogue;
        }
    }

    obj_set_root_window(obj, root_window);

epilogue:
    istr_destroy(root_window_name);
    return return_value;
}

/*
 * only for scrolling_list and choices
 */
static int
load_att_rows(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 rows = -1;

    if (!abio_get_integer(inFile, &rows))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    switch (obj->type)
    {
    case AB_TYPE_CONTAINER:
    case AB_TYPE_LIST:
    case AB_TYPE_CHOICE:
    case AB_TYPE_LAYERS:        /* special */
        obj_set_num_rows(obj, rows);
        break;
    }
    if (obj_is_text(obj))
    {
        obj_set_num_rows(obj, rows);
    }

    return 0;
}

/*
 * this is for stub widget - drawing area scrollbar - skip at the moment
 */
static int
load_att_scrollable_height(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

/*
 * this is for stub widget - drawing area scrollbar - skip at the moment
 */
static int
load_att_scrollable_width(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

static int
load_att_selection_required(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    /* for choices & scrolling-list */
    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    obj_set_selection_required(obj, tmp_bool);

    return 0;
}

static int
load_att_setting_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    AB_CHOICE_TYPE      choice_type;
    ISTRING             tmp_str;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    if ((choice_type = gilP_string_to_choice_type(istr_string(tmp_str))) == ERROR)
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    if (choice_type == ERROR)
    {
        return (abil_print_load_err(ERR_UNKNOWN), -1);
    }
    else
    {
        obj->info.choice.type = choice_type;
    }

    return 0;
}

/*
 * for control-area, term and text panes - Xview puts a 1 pixel border OL
 * says don't
 */
static int
load_att_show_border(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    obj_set_has_border(obj, tmp_bool);

    return 0;
}

/*
 * for scale only - Xview ??
 */
static int
load_att_show_endboxes(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    return 0;
}

/*
 * for base and popup windows
 */
static int
load_att_show_footer(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		footer_present = FALSE;

    /* REMIND: handle footer */
    if (!abio_gil_get_boolean(inFile, &footer_present))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);

    return 0;
}

static int
load_att_show_range(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		footer_present = FALSE;

    if (!abio_gil_get_boolean(inFile, &footer_present))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    return 0;
}

static int
load_att_show_value(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    /* for scale show value in text-field - Appl. job */
    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    obj->info.scale.show_value = tmp_bool;

    return 0;
}

static int
load_att_scale_width(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    /* scale's width */
    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    if (tmp_int > 0)
    {
        obj->width = tmp_int;
    }

    return 0;
}

static int
load_att_stored_length(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int		tmp_int = 0;

    if (!abio_get_integer(inFile, &tmp_int))
    {
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    }
    if (tmp_int > 0)
    {
	obj_set_max_length(obj, tmp_int);
    }
    
    return 0;
}

/*
 * for text-field
 */
static int
load_att_max_length(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    if (tmp_int > 0)
    {
        obj->info.text.max_length = tmp_int;
    }

    return 0;
}

static int
load_att_text_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;
    AB_TEXT_TYPE        text_type;

    /* for alpha/numeric text-field */
    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    text_type = gilP_string_to_text_type(istr_string(tmp_str));
    obj->info.text.type = text_type;

    istr_destroy(tmp_str);
    return 0;
}

/*
 * Motif doesn't support slider ticks
 */
static int
load_att_ticks(FILE * inFile, ABObj obj, ABObj root_obj)
{

    int                 tmp_int = 0;
    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    return 0;
}

static int
load_att_title(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    /* for list title - internal */
    if (!abio_get_string(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_STRING), -1);
    obj_set_label(obj, istr_string(tmp_str));
    istr_destroy(tmp_str);

    return 0;
}

/*
 * */
static int
load_att_type(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;
    AB_OBJECT_TYPE      obj_type = AB_TYPE_UNDEF;

    if (!abio_get_keyword(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    obj_type = gilP_string_to_object_type(istr_string(tmp_str));

    abil_loadmsg_set_object(istr_string(tmp_str));
    obj_set_type(obj, obj_type);

    /*
     * Need to save more information about this object type
     */
    switch (obj_get_type(obj))
    {
	case AB_TYPE_CONTAINER:
	    if (istr_equalstr(tmp_str, ":control-area"))
	    {
		obj_set_container_type(obj, AB_CONT_RELATIVE);
	    }
	    else if (istr_equalstr(tmp_str, ":group"))
	    {
		obj_set_container_type(obj, AB_CONT_GROUP);
	    }
	break;

	case AB_TYPE_MESSAGE:
	    obj_set_msg_type(obj, AB_MSG_INFORMATION);
	    obj_set_default_btn(obj, AB_DEFAULT_BTN_ACTION1);
	break;

	case AB_TYPE_TEXT_FIELD:
	    if (istr_equalstr(tmp_str, ":message"))
	    {
		/* guide message is static text */
		obj_set_read_only(obj, TRUE);
	    }
	break;
    }

    istr_destroy(tmp_str);
    return 0;
}

/*
 * Currently, all generations of guide have ignored user-data.
 */
static int
load_att_user_data(FILE * inFile, ABObj obj, ABObj root_obj)
{
    ISTRING             tmp_str = NULL;

    if (!abio_get_list(inFile, &tmp_str))
        return (abil_print_load_err(ERR_WANT_LIST), -1);
    istr_destroy(tmp_str);      /* ignore */

    return 0;
}

/*
 * for text-field # of chars visible
 */
static int
load_att_value_length(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    if (obj_is_text(obj))
    {
        obj_set_num_columns(obj, tmp_int);
    }

    return 0;
}

static int
load_att_value_underlined(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 return_value = 0;
    BOOL		is_underlined = FALSE;

    if (!abio_gil_get_boolean(inFile, &is_underlined))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    if (is_underlined)
    {
        abil_print_load_err(ERR_NOT_IMPL);
    }

    return return_value;
}

static int
load_att_value_x(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 valx;

    if (!abio_get_integer(inFile, &valx))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    obj_set_value_x(obj, valx);

    return 0;
}

static int
load_att_value_y(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 valy;

    if (!abio_get_integer(inFile, &valy))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    obj_set_value_y(obj, valy);


    return 0;
}

static int
load_att_voffset(FILE * inFile, ABObj obj, ABObj root_obj)
{
    if (!abio_get_integer(inFile, &obj->info.container.voffset))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

static int
load_att_vscroll(FILE * inFile, ABObj obj, ABObj root_obj)
{
    BOOL		tmp_bool = FALSE;

    if (!abio_gil_get_boolean(inFile, &tmp_bool))
        return (abil_print_load_err(ERR_WANT_BOOLEAN), -1);
    obj->info.drawing_area.vscrollbar =
        (tmp_bool ? AB_SCROLLBAR_WHEN_NEEDED : AB_SCROLLBAR_NEVER);

    return 0;
}

static int
load_att_vspacing(FILE * inFile, ABObj obj, ABObj root_obj)
{
    if (!abio_get_integer(inFile, &obj->info.container.vspacing))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);

    return 0;
}

/*
 * CURRENT
 */
static int
load_att_width(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int = 0;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    if (tmp_int >= 0)
    {
        obj->width = tmp_int;
    }

    return 0;
}

static int
load_att_x(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int = 0;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    obj->x = tmp_int;

    return 0;
}

static int
load_att_y(FILE * inFile, ABObj obj, ABObj root_obj)
{
    int                 tmp_int = 0;

    if (!abio_get_integer(inFile, &tmp_int))
        return (abil_print_load_err(ERR_WANT_INTEGER), -1);
    obj->y = tmp_int;

    return 0;
}

/*************************************************************************
**************************************************************************
**                                                                      **
**              LOAD ONE ATTRIBUTE OF ANY TYPE                          **
**                                                                      **
**************************************************************************
**************************************************************************/

/*
 * Load one attribute.  Returns NULL if successful, otherwise an error
 * message.
 * 
 * THIS IS THE ONLY EXPORTED IDENTIFIER IN THIS FILE **
 */
int
gilP_load_attribute_value(
                          FILE * inFile,
                          ABObj obj,
                          AB_GIL_ATTRIBUTE g_attr,
                          ABObj root_obj
)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    LOADATT_FUNC       *func = NULL;

    if (abio_get_eof(inFile))
        return (abil_print_load_err(ERR_EOF), -1);

    switch (g_attr)
    {
    case AB_GIL_ACTIONS:
        func = gilP_load_att_actions;
        break;
    case AB_GIL_ANCHOR_OBJECT:
        func = load_att_anchor_object;
        break;
    case AB_GIL_ANCHOR_POINT:
        func = load_att_anchor_point;
        break;
    case AB_GIL_BACKGROUND_COLOR:
        func = load_att_background_color;
        break;
    case AB_GIL_BUSY_DROP_GLYPH:
        func = load_att_busy_drop_glyph;
        break;
    case AB_GIL_BUTTON_TYPE:
        func = load_att_button_type;
        break;
    case AB_GIL_CHOICES:
        func = load_att_choices;
        break;
    case AB_GIL_CHOICE_COLORS:
        func = load_att_choice_colors;
        break;
    case AB_GIL_CHOICE_DEFAULTS:
        func = load_att_choice_defaults;
        break;
    case AB_GIL_CHOICE_LABEL_TYPES:
        func = load_att_choice_label_types;
        break;
    case AB_GIL_COLUMNS:
        func = load_att_columns;
        break;
    case AB_GIL_COL_ALIGNMENT:
        func = load_att_col_alignment;
        break;
     case AB_GIL_CONSTANT_WIDTH:
        func = load_att_constant_width;
        break;
    case AB_GIL_DEFAULT_DROP_SITE:
        func = load_att_default_drop_site;
        break;
    case AB_GIL_DONE_HANDLER:
        func = gilP_load_att_done_handler;
        break;
    case AB_GIL_DND_ACCEPT_CURSOR:
        func = load_att_dnd_accept_cursor;
        break;
    case AB_GIL_DND_ACCEPT_CURSOR_XHOT:
        func = load_att_dnd_accept_cursor_xhot;
        break;
    case AB_GIL_DND_ACCEPT_CURSOR_YHOT:
        func = load_att_dnd_accept_cursor_yhot;
        break;
    case AB_GIL_DND_CURSOR:
        func = load_att_dnd_cursor;
        break;
    case AB_GIL_DND_CURSOR_XHOT:
        func = load_att_dnd_cursor_xhot;
        break;
    case AB_GIL_DND_CURSOR_YHOT:
        func = load_att_dnd_cursor_yhot;
        break;
    case AB_GIL_DRAGGABLE:
        func = load_att_draggable;
        break;
    case AB_GIL_DRAWING_MODEL:
        func = load_att_drawing_model;
        break;                  /* don't need - we have just xwindows */
    case AB_GIL_DROPPABLE:
        func = load_att_droppable;
        break;
    case AB_GIL_DROP_TARGET_WIDTH:
        func = load_att_drop_target_width;
        break;
    case AB_GIL_EVENTS:
        func = load_att_events;
        break;
    case AB_GIL_EVENT_HANDLER:
        func = gilP_load_att_event_handler;
        break;
    case AB_GIL_FILE_CHOOSER_TYPE:
        func = load_att_file_chooser_type;
        break;
    case AB_GIL_FILE_CHOOSER_FILTER_PATTERN:
        func = load_att_file_chooser_filter_pattern;
        break;
    case AB_GIL_FILE_CHOOSER_MATCH_GLYPH:
       func = load_att_file_chooser_match_glyph;
       break; 
    case AB_GIL_FILE_CHOOSER_MATCH_GLYPH_MASK: 
       func = load_att_file_chooser_match_glyph_mask;
       break;
    case AB_GIL_FOREGROUND_COLOR:
        func = load_att_foreground_color;
        break;
    case AB_GIL_GROUP_TYPE:
        func = load_att_group_type;
        break;
    case AB_GIL_HEIGHT:
        func = load_att_height;
        break;
    case AB_GIL_HELP:
        func = load_att_help;
        break;
    case AB_GIL_HOFFSET:
        func = load_att_hoffset;
        break;
    case AB_GIL_HSCROLL:
        func = load_att_hscroll;
        break;
    case AB_GIL_HSPACING:
        func = load_att_hspacing;
        break;
    case AB_GIL_ICON_FILE:
        func = load_att_icon_file;
        break;
    case AB_GIL_ICON_LABEL:
        func = load_att_icon_label;
        break;
    case AB_GIL_ICON_MASK_FILE:
        func = load_att_icon_mask_file;
        break;
    case AB_GIL_INITIAL_LIST_GLYPHS:
        func = load_att_initial_list_glyphs;
        break;
    case AB_GIL_INITIAL_LIST_VALUES:
        func = load_att_initial_list_values;
        break;
    case AB_GIL_INITIAL_SELECTIONS:
        func = load_att_initial_selections;
        break;
    case AB_GIL_INITIAL_VALUE:
        func = load_att_initial_value;
        break;
    case AB_GIL_INITIAL_STATE:
        func = load_att_initial_state;
        break;
    case AB_GIL_INTERFACES:
        func = load_att_interfaces;
        break;
    case AB_GIL_LABEL:
        func = load_att_label;
        break;
    case AB_GIL_LABEL_BOLD:
        func = load_att_label_bold;
        break;
    case AB_GIL_LABEL_TYPE:
        func = load_att_label_type;
        break;
    case AB_GIL_LAYOUT_TYPE:
        func = load_att_layout_type;
        break;
    case AB_GIL_MAPPED:
        func = load_att_mapped;
        break;
    case AB_GIL_MAX_TICK_STRING:
        func = load_att_max_tick_string;
        break;
    case AB_GIL_MAX_VALUE:
        func = load_att_max_value;
        break;
    case AB_GIL_MAX_VALUE_STRING:
        func = load_att_max_value_string;
        break;
    case AB_GIL_MEMBERS:
        func = load_att_members;
        break;
    case AB_GIL_MENU_HANDLER:
        func = gilP_load_att_menu_handler;
        break;
    case AB_GIL_MENU_ITEM_ACCELERATORS:
        func = load_att_menu_item_accelerators;
        break;
    case AB_GIL_MENU_ITEM_COLORS:
        func = load_att_menu_item_colors;
        break;
    case AB_GIL_MENU_ITEM_DEFAULTS:
        func = load_att_menu_item_defaults;
        break;
    case AB_GIL_MENU_ITEM_HANDLERS:
        func = gilP_load_att_menu_item_handlers;
        break;
    case AB_GIL_MENU_ITEM_LABELS:
        func = load_att_menu_item_labels;
        break;
    case AB_GIL_MENU_ITEM_LABEL_TYPES:
        func = load_att_menu_item_label_types;
        break;
    case AB_GIL_MENU_ITEM_STATES:
        func = load_att_menu_item_states;
        break;
    case AB_GIL_MENU_ITEM_MENUS:
        func = load_att_menu_item_menus;
        break;
    case AB_GIL_MENU:
        func = load_att_menu;
        break;
    case AB_GIL_MENU_TITLE:
        func = load_att_menu_title;
        break;
    case AB_GIL_MENU_TYPE:
        func = load_att_menu_type;
        break;
    case AB_GIL_MIN_TICK_STRING:
        func = load_att_min_tick_string;
        break;
    case AB_GIL_MIN_VALUE:
        func = load_att_min_value;
        break;
    case AB_GIL_MIN_VALUE_STRING:
        func = load_att_min_value_string;
        break;
    case AB_GIL_MULTIPLE_SELECTIONS:
        func = load_att_multiple_selections;
        break;
    case AB_GIL_NAME:
        func = NULL;
        rc = load_att_name(inFile, &obj, root_obj);     /* special case */
        /* takes obj** */
        break;
    case AB_GIL_NORMAL_DROP_GLYPH:
        func = load_att_normal_drop_glyph;
        break;
    case AB_GIL_ACTIVATE_HANDLER:
        func = gilP_load_att_notify_handler;
        break;
    case AB_GIL_ORIENTATION:
        func = load_att_orientation;
        break;
    case AB_GIL_OWNER:
        func = load_att_owner;
        break;
    case AB_GIL_PINNABLE:
        func = load_att_pinnable;
        break;
    case AB_GIL_PINNED:
        func = load_att_pinned;
        break;
    case AB_GIL_READ_ONLY:
        func = load_att_read_only;
        break;
    case AB_GIL_REFERENCE_POINT:
        func = load_att_reference_point;
        break;
    case AB_GIL_REPAINT_PROC:
        func = gilP_load_att_repaint_proc;
        break;
    case AB_GIL_RESIZABLE:
        func = load_att_resizable;
        break;
    case AB_GIL_ATT_ROOT_WINDOW:
        func = load_att_root_window;
        break;
    case AB_GIL_ROWS:
        func = load_att_rows;
        break;
    case AB_GIL_ROW_ALIGNMENT:
        func = load_att_row_alignment;
        break;
    case AB_GIL_SCROLLABLE_HEIGHT:
        func = load_att_scrollable_height;
        break;
    case AB_GIL_SCROLLABLE_WIDTH:
        func = load_att_scrollable_width;
        break;
    case AB_GIL_SELECTION_REQUIRED:
        func = load_att_selection_required;
        break;
    case AB_GIL_SETTING_TYPE:
        func = load_att_setting_type;
        break;
    case AB_GIL_SHOW_BORDER:
        func = load_att_show_border;
        break;
    case AB_GIL_SHOW_ENDBOXES:
        func = load_att_show_endboxes;
        break;
    case AB_GIL_SHOW_FOOTER:
        func = load_att_show_footer;
        break;
    case AB_GIL_SHOW_RANGE:
        func = load_att_show_range;
        break;
    case AB_GIL_SHOW_VALUE:
        func = load_att_show_value;
        break;
    case AB_GIL_SCALE_WIDTH:
        func = load_att_scale_width;
        break;
    case AB_GIL_STORED_LENGTH:
        func = load_att_stored_length;
        break;
    case AB_GIL_MAX_LENGTH:
        func = load_att_max_length;
        break;
    case AB_GIL_TEXT_TYPE:
        func = load_att_text_type;
        break;
    case AB_GIL_TICKS:
        func = load_att_ticks;
        break;
    case AB_GIL_TITLE:
        func = load_att_title;
        break;
    case AB_GIL_TYPE:
        func = load_att_type;
        break;
    case AB_GIL_USER_DATA:
        func = load_att_user_data;
        break;
    case AB_GIL_VALUE_LENGTH:
        func = load_att_value_length;
        break;
    case AB_GIL_VALUE_UNDERLINED:
        func = load_att_value_underlined;
        break;
    case AB_GIL_VALUE_X:
        func = load_att_value_x;
        break;
    case AB_GIL_VALUE_Y:
        func = load_att_value_y;
        break;
    case AB_GIL_VOFFSET:
        func = load_att_voffset;
        break;
    case AB_GIL_VSCROLL:
        func = load_att_vscroll;
        break;
    case AB_GIL_VSPACING:
        func = load_att_vspacing;
        break;
    case AB_GIL_WIDTH:
        func = load_att_width;
        break;
    case AB_GIL_X:
        func = load_att_x;
        break;
    case AB_GIL_Y:
        func = load_att_y;
        break;
    default:
        abil_print_load_err(ERR_UNKNOWN_ATTR);
        return -1;
    }

    if (func != NULL)
    {
        rc = func(inFile, obj, root_obj);
    }

    return rc;
}                               /* gilP_load_attribute */

int
istr_array_init(ISTRING_ARRAY * array)
{
    array->count = 0;
    return 0;
}

int
istr_array_uninit(ISTRING_ARRAY * array)
{
    int                 i;
    for (i = 0; i < array->count; ++i)
    {
        istr_destroy(array->strings[i]);
    }
    array->count = 0;
    return 0;
}
