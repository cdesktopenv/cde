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
 *	$XConsortium: write_c.c /main/3 1995/11/06 18:17:49 rswiston $
 *
 *	@(#)write_c.c	1.12 16 Feb 1994	cde_app_builder/src/abmf
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
 *  write_c.c - generic functions to write out C code
 */

#include <stdarg.h>
#include <ab_private/abio.h>
#include "cdefsP.h"
#include "obj_namesP.h"
#include "utilsP.h"
#include "write_cP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/
static int write_func_def_params( 
			File	file, 
			BOOL	use_prototypes,
			va_list	va_params);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

extern STRING	abmfP_str_bool= "Boolean";
extern STRING	abmfP_str_int= "int";
extern STRING	abmfP_str_string= "String";
extern STRING	abmfP_str_void= "void";
extern STRING	abmfP_str_widget = "Widget";
extern STRING	abmfP_str_xtpointer = "XtPointer";
extern STRING	abmfP_str_tt_msg= "Tt_message";
extern STRING	abmfP_str_voidpointer= "void *";
extern STRING	abmfP_str_intpointer= "int *";
extern STRING	abmfP_str_argvpointer= "char ***";

extern STRING	abmfP_str_empty = "";
extern STRING	abmfP_str_null = "NULL";
extern STRING	abmfP_str_zero = "0";


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

int 
abmfP_write_c_block_begin(GenCodeInfo genCodeInfo)
{
    abio_puts(genCodeInfo->code_file, "{\n");
    abio_indent(genCodeInfo->code_file);
    return 0;
}


int 
abmfP_write_c_block_end(GenCodeInfo genCodeInfo)
{
    abio_outdent(genCodeInfo->code_file);
    abio_puts(genCodeInfo->code_file, "}\n");
    return 0;
}


int 
abmfP_write_c_comment(
		GenCodeInfo	genCodeInfo,
		BOOL		oneLiner,
		STRING		comment
)
{
    File	file= genCodeInfo->code_file;
    
    if (oneLiner)
    {
	abio_printf(file, "/* %s */\n", comment);
    }
    else
    {
        abio_puts(file, abmfP_comment_begin);
        abio_puts(file, abmfP_comment_continue);
        abio_puts(file, comment);
        abio_puts(file, "\n");
        abio_puts(file, abmfP_comment_end);
    }
    return 0;
}


int 
abmfP_write_c_func_decl(
		GenCodeInfo	genCodeInfo,
		BOOL	is_static,
		STRING	return_type,
		STRING	func_name,
		...
)
{
#define va_start_params() (va_start(params, func_name))
    va_list	params;
    File	file= genCodeInfo->code_file;
    int		i= 0;
    int		num_params = 0;
    int		num_params_written= 0;
    BOOL	list_params= TRUE;	/* use multiline format for params */

    va_start_params();

    /*
     * Count pairs of strings
     */
    num_params= 0;
    va_start_params();
    while (   (va_arg(params, STRING) != NULL) 
	   && (va_arg(params, STRING) != NULL))
    {
	++num_params;
    }

    list_params= (num_params > 1);

    if (is_static)
    {
	abio_puts(file, "static ");
    }
    if (return_type != NULL)
    {
	abio_puts(file, return_type);
    }
    if (func_name != NULL)
    {
	abio_printf(file, " %s(", func_name);
    }

    if ((genCodeInfo->prototype_funcs) && (num_params > 0))
    {
	BOOL	paramsDone = FALSE;
	STRING	curParamType =  NULL;
	STRING	curParamName = NULL;

	num_params_written= 0;
	if (list_params)
	{
	    abio_puts(file, "\n");
	    abio_indent(file);
	}
	va_start_params();
        while (!paramsDone)
        {
	    curParamType = va_arg(params, STRING);
	    if (curParamType != NULL)
	    {
	        curParamName = va_arg(params, STRING);
	    }
	    if ((curParamType == NULL) || (curParamName == NULL))
	    {
		paramsDone = TRUE;
		continue;
	    }

	    abio_puts(file, curParamType);
	    abio_puts(file, (list_params? "\t":" "));
	    abio_puts(file, curParamName);
	    ++num_params_written;
	    if (num_params_written < num_params)
	    {
	        abio_puts(file, ",");
		if (list_params)
		{
		    abio_puts(file, "\n");
		}
	    }
	}
	if (list_params)
	{
	    abio_outdent(file);
	    abio_puts(file, "\n");
	}
    }
    abio_puts(file, ");\n");

    va_end(params);
    return 0;
#undef va_start_params
}


int 
abmfP_write_c_func_begin(
		GenCodeInfo	genCodeInfo,
		BOOL	is_static,
		STRING	return_type,
		STRING	func_name,
		...
)
{
#define va_start_params() (va_start(params, func_name))
    va_list	params;
    File	file= genCodeInfo->code_file;

    va_start_params();

    abio_set_indent(file, 0);
    if (is_static)
    {
	abio_puts(file, "static ");
    }
    if (return_type != NULL)
    {
	abio_puts(file, return_type);
	abio_puts(file, " ");
    }
    abio_puts(file, "\n");
    if (func_name != NULL)
    {
	abio_puts(file, func_name);
	abio_puts(file, "(");
    }
    va_start_params();
    write_func_def_params(file, genCodeInfo->prototype_funcs, params);
    abio_puts(file, "{\n"); /* } let vi match braces */

    abio_indent(file);

    va_end(params);
    return 0;
#undef va_start_params
}


/*
 * If return_value is null, no return is written
 */
int
abmfP_write_c_func_end(
		GenCodeInfo	genCodeInfo,
		STRING		return_value
)
{
    File	file= genCodeInfo->code_file;
    if (return_value != NULL)
    {
	abio_printf(file, "return %s;\n", return_value);
    }
    abio_set_indent(file, 0); /* { vi hack */
    abio_puts(file, "}\n\n");
    return 0;
}


/*
 * writes #include X
 */
int
abmfP_write_c_include(GenCodeInfo genCodeInfo, STRING fileName)
{
    File	codeFile = genCodeInfo->code_file;
    abio_puts(codeFile, "#include ");
    abio_put_string(codeFile, fileName);
    abio_puts(codeFile, "\n");
    return 0;
}


/*
 * writes #include "blah.h"
 */
int
abmfP_write_c_local_include(GenCodeInfo genCodeInfo, STRING fileName)
{
    File	codeFile = genCodeInfo->code_file;
    abio_puts(codeFile, "#include ");
    abio_put_string(codeFile, fileName);
    abio_puts(codeFile, "\n");
    return 0;
}


/*
 * writes #include <blah.h>
 */
int
abmfP_write_c_system_include(GenCodeInfo genCodeInfo, STRING fileName)
{
    File	codeFile = genCodeInfo->code_file;
    abio_printf(codeFile, "#include <%s>\n", fileName);
    return 0;
}


/*
 * Assumes: function name and opening "(" have been written
 */
static int
write_func_def_params(
	File	file,
	BOOL	use_prototypes,
	va_list	va_params
)
{
#if defined(__ppc) || defined(linux)
#define va_start_params() __va_copy(params, va_params)
#elif defined(CSRG_BASED)
#define va_start_params() va_copy(params, va_params)
#else
#define va_start_params() (params = va_params)
#endif
#define va_end_params() (0)
    va_list	params;
    int		num_params_written= 0;
    int		num_params= 0;
    BOOL	list_params= FALSE;
    int		i= 0;
    STRING	curParamType = NULL;
    STRING	curParamName = NULL;
    BOOL	paramsDone = FALSE;

    va_start_params();

    /*
     * Count non-NULL string pairs
     */
    num_params = 0;
    va_start_params();
    while (   (va_arg(params, STRING) != NULL) 
	   && (va_arg(params, STRING) != NULL))
    {
	++num_params;
    }

    list_params= (num_params > 1);	/* use "list" format */
    if (num_params > 0)
    {
	if (list_params)
	{
	    abio_puts(file, "\n");
	    abio_indent(file);
	}

	paramsDone = FALSE;
	va_start_params();
	while (!paramsDone)
	{
	    curParamType = va_arg(params, STRING);
	    if (curParamType != NULL)
	    {
		curParamName = va_arg(params, STRING);
	    }
	    if ((curParamType == NULL) || (curParamName == NULL))
	    {
		paramsDone = TRUE;
		continue;
	    }

	    if (use_prototypes)
	    {
	        abio_puts(file, curParamType);
	        abio_puts(file, " ");
	    }
	    abio_puts(file, curParamName);
	    ++num_params_written;
	    if (num_params_written < num_params)
	    {
		abio_puts(file, ",");
	    }
	    if (list_params)
	    {
	        abio_puts(file, "\n");
	    }
	}
	if (list_params)
	{
	    abio_outdent(file);
	}
    }
    abio_puts(file, ")\n");

    /*
     * Write out old-style parameter types
     */
    num_params_written= 0;
    if (!use_prototypes)
    {
	paramsDone = FALSE;
	va_start_params();
	while (!paramsDone)
	{
	    curParamType = va_arg(params, STRING);
	    if (curParamType != NULL)
	    {
		curParamName = va_arg(params, STRING);
	    }
	    if ((curParamType == NULL) || (curParamName == NULL))
	    {
		paramsDone = TRUE;
		continue;
	    }

	    abio_printf(file, "\t%s %s", curParamType, curParamName);
	    ++num_params_written;
	    if (num_params_written < num_params)
	    {
		abio_puts(file, ",");
	    }
	    abio_puts(file, "\n");
	}
    }

    va_end_params();
    return 0;
#undef va_start_params
#undef va_end_params
}


int
abmfP_write_xm_callback_decl(
			GenCodeInfo genCodeInfo, 
			BOOL	is_static,
			STRING	func_name
)
{
    return abmfP_write_c_func_decl(
		    genCodeInfo,
		    is_static,
		    abmfP_str_void,
		    func_name,
		    abmfP_str_widget, abmfP_widget_var_name,
		    abmfP_str_xtpointer, abmfP_client_data_var_name,
		    abmfP_str_xtpointer, abmfP_call_data_var_name,
		    NULL
		);
}




int
abmfP_write_xm_callback_begin(
			GenCodeInfo genCodeInfo, 
			BOOL	is_static,
			STRING func_name
)
{
    return abmfP_write_c_func_begin(
		    genCodeInfo,
		    is_static,
		    abmfP_str_void,
		    func_name,
		    abmfP_str_widget, abmfP_widget_var_name,
		    abmfP_str_xtpointer, abmfP_client_data_var_name,
		    abmfP_str_xtpointer, abmfP_call_data_var_name,
		    NULL
		);
}

int
abmfP_write_tooltalk_callback_decl(
                        GenCodeInfo genCodeInfo,
                        BOOL    is_static,
                        STRING  func_name
)
{
    return abmfP_write_c_func_decl(
                    genCodeInfo,
                    is_static,
                    abmfP_str_bool,
                    func_name,
                    abmfP_str_tt_msg, abmfP_tt_msg_var_name,
                    abmfP_str_voidpointer, abmfP_call_data_var_name,
                    NULL
                );
}

int
abmfP_write_session_save_callback_decl(
                        GenCodeInfo genCodeInfo,
                        BOOL    is_static,
                        STRING  func_name
)
{
    return abmfP_write_c_func_decl(
                    genCodeInfo,
                    is_static,
                    abmfP_str_bool,
                    func_name,
                    abmfP_str_widget, abmfP_widget_var_name,
                    abmfP_str_string, abmfP_ss_session_file_var_name,
                    abmfP_str_argvpointer, "argv",
                    abmfP_str_intpointer, "argc",
                    NULL
                );
}

int
abmfP_write_session_restore_callback_decl(
                        GenCodeInfo genCodeInfo,
                        BOOL    is_static,
                        STRING  func_name
)
{
    return abmfP_write_c_func_decl(
                    genCodeInfo,
                    is_static,
                    abmfP_str_bool,
                    func_name,
                    abmfP_str_widget, abmfP_widget_var_name,
                    abmfP_str_string, abmfP_ss_session_file_var_name,
                    NULL
                );
}

int
abmfP_write_tooltalk_callback_begin(
                        GenCodeInfo genCodeInfo,
                        BOOL    is_static,
                        STRING func_name
)
{
    return abmfP_write_c_func_begin(
                    genCodeInfo,
                    is_static,
                    abmfP_str_bool,
                    func_name,
                    abmfP_str_tt_msg, abmfP_tt_msg_var_name,
                    abmfP_str_voidpointer, abmfP_call_data_var_name,
                    NULL
                );
}




abmfP_write_clear_proc_decl(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
)
{
    return abmfP_write_c_func_decl(
	    genCodeInfo,
	    FALSE,				    /* Is static */
	    abmfP_str_int,			    /* return type */
	    abmfP_get_clear_proc_name(obj),	    /* function name */
	    abmfP_get_c_struct_ptr_type_name(obj),  /* Argument type */
	    abmfP_instance_ptr_var_name,	    /* Argument name */
	    NULL
	   );
}


int
abmfP_write_create_proc_begin_or_decl(
		GenCodeInfo	genCodeInfo,
		ABObj		obj,
		BOOL		declaration
)
{
#define MAX_CREATE_PARAMS 11		/* need lots for widget return params*/
    int		return_value = 0;
    STRING	param_types[MAX_CREATE_PARAMS];
    STRING	param_names[MAX_CREATE_PARAMS];
    int		num_params = 0;
    int		i = 0;
    int		num_widgets = 0;
    int		first_widget_return_param = -1;
    int		num_submenu_params = 0;
    int		first_submenu_param = -1;

    for (num_params = 0; num_params < MAX_CREATE_PARAMS; ++num_params)
    {
	param_types[num_params] = NULL;
	param_names[num_params] = NULL;
    }

    /*
     * Set up parameters, in order : instance, name, parent, widget*
     */
    num_params = 0;

    /*
     * Instance pointer
     */
    if (abmfP_obj_create_proc_has_instance_param(obj))
    {
	param_types[num_params]= 
	    abmfP_obj_get_create_proc_instance_param_type(obj);
	param_names[num_params]= abmfP_instance_ptr_var_name;
	++num_params;
	if (!declaration)
	{
	    /* 	The ipObj field is set, so we don't need to save the var. */
	}
    }

    /*
     * Name
     */
    if (abmfP_obj_create_proc_has_name_param(obj))
    {
	param_types[num_params] = abmfP_str_string;
	param_names[num_params] = "name";
	++num_params;
	if (!declaration)
	{
	    abmfP_name_param(genCodeInfo)= istr_const("name");
	    abmfP_name_param_has_value(genCodeInfo)= TRUE;
	}
    }

    /*
     * Parent
     */
    if (abmfP_obj_create_proc_has_parent_param(obj))
    {
	param_types[num_params]= abmfP_str_widget;
	param_names[num_params]= "parent";
	++num_params;
	if (!declaration)
	{
	    abmfP_name_param(genCodeInfo)= istr_const("parent");
	    abmfP_name_param_has_value(genCodeInfo)= TRUE;
	}
    }

    /*
     * Widget* for returning widget values
     */
    num_widgets = abmfP_obj_get_num_create_proc_return_widgets(obj);
    if (num_widgets > 0)
    {
	first_widget_return_param = num_params;
	for (i = 0; (i < num_widgets) && (num_params < MAX_CREATE_PARAMS); ++i)
	{
	    param_types[num_params]= "Widget *";
	    param_names[num_params]= 
		strdup(abmfP_obj_get_create_proc_return_widget_name(obj, i));
	    ++num_params;
	}
    }

    /*
     * submenu id params
     */
    num_submenu_params = abmfP_get_num_cp_submenu_params(obj);
    if (num_submenu_params > 0)
    {
	StringList	submenu_param_list = 
				&(abmfP_submenu_params(genCodeInfo));
	StringList	submenu_param_type_list =
				&(abmfP_submenu_param_types(genCodeInfo));
	int		num_strings = 0;

	strlist_make_empty(submenu_param_list);
	strlist_make_empty(submenu_param_type_list);

	for (i = 0; i < num_submenu_params; ++i)
	{
	    strlist_add_str(submenu_param_type_list,
			abmfP_get_cp_submenu_param_type_name(obj, i), NULL);
	    strlist_add_str(submenu_param_list,
			abmfP_get_cp_submenu_param_name(obj, i), NULL);
	}

	num_strings = strlist_get_num_strs(submenu_param_list);
	for (i = 0; (i < num_strings) && (num_params < MAX_CREATE_PARAMS); ++i)
	{
	    param_types[num_params] = 
		strlist_get_str(submenu_param_type_list, i, NULL);
	    param_names[num_params] = 
		strlist_get_str(submenu_param_list, i, NULL);
	    ++num_params;
	}
    }

    /*
     * Check for max # params
     */
    assert(num_params < MAX_CREATE_PARAMS);

    if (declaration)
    {
        return_value = 
	    abmfP_write_c_func_decl(
	        genCodeInfo,
	        TRUE,
	        abmfP_str_int,		abmfP_get_create_proc_name(obj),
	        param_types[0],	param_names[0],
	        param_types[1],	param_names[1],
	        param_types[2],	param_names[2],
	        param_types[3],	param_names[3],
	        param_types[4],	param_names[4],
	        param_types[5],	param_names[5],
	        param_types[6],	param_names[6],
	        param_types[7],	param_names[7],
	        param_types[8],	param_names[8],
	        param_types[9],	param_names[9],
		NULL
	       );
    }
    else
    {
        return_value = 
	    abmfP_write_c_func_begin(
	        genCodeInfo,
	        TRUE,
	        abmfP_str_int,		abmfP_get_create_proc_name(obj),
	        param_types[0],	param_names[0],
	        param_types[1],	param_names[1],
	        param_types[2],	param_names[2],
	        param_types[3],	param_names[3],
	        param_types[4],	param_names[4],
	        param_types[5],	param_names[5],
	        param_types[6],	param_names[6],
	        param_types[7],	param_names[7],
	        param_types[8],	param_names[8],
	        param_types[9],	param_names[9],
		NULL
	       );
    }

    if (first_widget_return_param >= 0)
    {
    for (i= first_widget_return_param; i < MAX_CREATE_PARAMS; ++i)
    {
	if (param_names[i] != NULL)
	{
	    util_free(param_names[i]); param_names[i] = NULL;
	}
    }
    }


    return return_value;
#undef MAX_CREATE_PARAMS
}


int
abmfP_write_create_proc_decl(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
)
{
    return abmfP_write_create_proc_begin_or_decl(
		genCodeInfo, obj, TRUE);
}


int
abmfP_write_create_proc_begin(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
)
{
    return abmfP_write_create_proc_begin_or_decl(
		genCodeInfo, obj, FALSE);
}



abmfP_write_clear_proc_begin(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
)
{
    return abmfP_write_c_func_begin(
	    genCodeInfo,
	    FALSE,				   /* Is static     */
	    abmfP_str_int,			   /* return type  */
	    abmfP_get_clear_proc_name(obj),	   /* function name */
	    abmfP_get_c_struct_ptr_type_name(obj), /* Argument type */
	    abmfP_instance_ptr_var_name, 	   /* Argument name */
	    NULL
	   );
}


abmfP_write_init_proc_decl(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
)
{
    if (obj_is_message(obj))
    {
        return abmfP_write_c_func_decl(
            genCodeInfo,
            FALSE,                              /* Is static */
            abmfP_str_int,                      /* return type */
            abmfP_get_init_proc_name(obj),      /* function name */
            "DtbMessageData",			/* Argument type */
	    abmfP_instance_ptr_var_name,	/* Argument name */
            NULL
	   );   
    }
    else if (obj_is_window(obj))
    {
	return abmfP_write_c_func_decl(
	    genCodeInfo,
	    FALSE,				   /* Is static */
	    abmfP_str_int,			   /* return type */
	    abmfP_get_init_proc_name(obj),	   /* function name */
	    abmfP_get_c_struct_ptr_type_name(obj), /* Argument type */
	    abmfP_instance_ptr_var_name,	   /* Argument name */
	    abmfP_str_widget,	"parent",	   /* Argument type */
	    NULL				   /* Argument name */
	   );
    }
}


abmfP_write_init_proc_begin(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
)
{
    if (obj_is_message(obj))
    {
	return abmfP_write_c_func_begin(
            genCodeInfo,
            FALSE,                           /* Is static */
            abmfP_str_int,                   /* return type */
            abmfP_get_init_proc_name(obj),   /* function name */
            "DtbMessageData",                /* Argument type */
            abmfP_instance_ptr_var_name,     /* Argument name */
            NULL 
           );
    }
    else
    {
	return abmfP_write_c_func_begin(
	    genCodeInfo,
	    FALSE,				   /* Is static */
	    abmfP_str_int,			   /* return type */
	    abmfP_get_init_proc_name(obj),	   /* function name */
	    abmfP_get_c_struct_ptr_type_name(obj), /* Argument type */
	    abmfP_instance_ptr_var_name,	   /* Argument name */
	    abmfP_str_widget,			   /* Argument type */
	    "parent",				   /* Argument name */
	    NULL
	   );
    }
}


int
abmfP_write_file_header(
		GenCodeInfo		genCodeInfo,
		STRING			fileName,
		BOOL			openIfdef,
		STRING			gennedFrom,
		STRING			gennedBy,
		ABMF_MODIFY_TYPE	modifyType,
		STRING			description
)
{
    File	codeFile = genCodeInfo->code_file;
    STRING	comment = NULL;

    abio_printf(codeFile, 
        "/*\n"
	" * File: %s\n",
	    fileName);
    
    if (description != NULL)
    {
	abio_printf(codeFile, "%s\n", description);
    }

    abio_puts(codeFile, " *\n");
    abio_printf(codeFile, " * This file was generated by %s, from %s\n",
	gennedBy, gennedFrom);
    abio_puts(codeFile, " *\n");

    comment = NULL;
    switch (modifyType)
    {
	case ABMF_MODIFY_NOT:
	    comment = 
	" *    ** DO NOT MODIFY BY HAND - ALL MODIFICATIONS WILL BE LOST **\n";
	break;

	case ABMF_MODIFY_USER_SEGS:
	    comment =
" * Any text may be added between the DTB_USER_CODE_START and\n"
" * DTB_USER_CODE_END comments (even non-C code). Descriptive comments\n"
" * are provided only as an aid.\n"
" *\n"
" *  ** EDIT ONLY WITHIN SECTIONS MARKED WITH DTB_USER_CODE COMMENTS.  **\n"
" *  ** ALL OTHER MODIFICATIONS WILL BE OVERWRITTEN. DO NOT MODIFY OR  **\n"
" *  ** DELETE THE GENERATED COMMENTS!                                 **\n";
	break;
    }

    if (comment != NULL)
    {
	abio_puts(codeFile, comment);
    }
    abio_puts(codeFile, " */\n");

    if (openIfdef)
    {
	STRING	defineToken = abmfP_get_define_from_file_name(fileName);
	abio_printf(codeFile, "#ifndef %s\n", defineToken);
	abio_printf(codeFile, "#define %s\n", defineToken);
    }

    abio_puts(codeFile, "\n");

    return 0;
}


int
abmfP_write_file_footer(
			GenCodeInfo	genCodeInfo,
			STRING		fileName,
			BOOL		closeIfdef
)
{
    if (closeIfdef)
    {
	abio_printf(genCodeInfo->code_file, "#endif /* %s */\n",
		abmfP_get_define_from_file_name(fileName));
    }
    return 0;
}

abmfP_write_msg_clear_proc_decl(
                GenCodeInfo     genCodeInfo,
                ABObj           module
)
{
    return abmfP_write_c_func_decl(
            genCodeInfo,
            FALSE,                                 /* Is static */
            abmfP_str_int,                         /* return type */
            abmfP_get_msg_clear_proc_name(module), /* function name */
            "DtbMessageData",  		   	   /* Argument type */
            abmfP_instance_ptr_var_name,           /* Argument name */
            NULL);   
}
