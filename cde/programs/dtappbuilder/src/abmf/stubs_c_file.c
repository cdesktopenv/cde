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
 * $XConsortium: stubs_c_file.c /main/3 1995/11/06 18:13:48 rswiston $
 * 
 * @(#)stubs_c_file.c	3.48 11 Feb 1994	cde_app_builder/src/abmf
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
 * Create a file containing callback proc stubs. These are example notify
 * procedures that will serve as a template for the actual notify procedures
 * in the application.
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ab/util_types.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include "abmfP.h"
#include "ui_header_fileP.h"
#include "cdefsP.h"
#include "motifdefsP.h"
#include "obj_namesP.h"
#include "utilsP.h"
#include "instancesP.h"
#include "argsP.h"
#include "connectP.h"
#include "write_cP.h"
#include "lib_func_stringsP.h"
#include "stubs_c_fileP.h"

typedef void *VoidPtr;

/*
 * Action argument type.
 */
typedef enum
{
    IMMED_TYPE,
    INT_TYPE,
    STRING_TYPE
}                   ARG_TYPES;

/*
 * REMIND: some time in the future, "instance" should be "dtbTarget"
 */
static STRING	fromVarName = "dtbSource";
static STRING	toVarName = "dtbTarget";
static STRING	instanceVarName = "instance";

/*
 * Data
 */
static STRING	begin_tt_callback_body =
"\nBoolean\n\
%s(Tt_message msg, void *calldata)\n";

static STRING	begin_ss_save_callback_body =
"\nBoolean\n\
%s(Widget widget, String session_file, char ***argv, int *argc)\n";

static STRING	begin_ss_restore_callback_body =
"\nBoolean\n\
%s(Widget widget, char *session_file)\n";

/*
 * Local functions.
 */
static int	printf_setval(GenCodeInfo, ABObj, ...);
static int	printf_getval(GenCodeInfo, ABObj, int nres, ...);
static int write_tooltalk_cb_vars(GenCodeInfo genCodeInfo, ABObj action);
static int write_tooltalk_cb_body1(GenCodeInfo genCodeInfo, ABObj action);
static int write_tooltalk_cb_body2(GenCodeInfo genCodeInfo, ABObj action);
static int write_tooltalk_msg_reply(File codeFile);
static int write_ss_cb_vars(GenCodeInfo genCodeInfo, ABObj action);
static int write_ss_cb_body1(GenCodeInfo genCodeInfo, ABObj action);
static int write_ss_cb_body2(GenCodeInfo genCodeInfo, ABObj action);
static int	write_instance_ptr_var(
			GenCodeInfo	genCodeInfo,  
			ABObj		ipObj,
			STRING		instanceVarName,
			STRING		paramName,
			BOOL		castValue,
			STRING		comment
		);

static int write_builtin_action_for_ref(
                        GenCodeInfo       genCodeInfo, 
                        ABObj             action,
                        ABObj             toObj,
			BOOL              setUpVars,
                        AB_BUILTIN_ACTION builtin_action,
                        STRING            actionTKResource,
                        STRING            actionResource
);

static int write_builtin_action(
                        GenCodeInfo       genCodeInfo, 
                        ABObj             action,
                        ABObj             toObj,
                        AB_BUILTIN_ACTION builtin_action,
                        STRING            actionTKResource,
                        STRING            actionResource
);

/*
 * Gets the object that the connection is "actually" from. If write_conn*
 * is called on an object that has references to it, it's actually the
 * references that have the connections, not the object, itself.
 * (e.g., menus never have connections written, it's the menu references
 * that do).
 */
static ABObj
get_actual_from_obj(ABObj actionOrObj)
{
    static ABObj	lastFromObj = NULL;
    static ABObj	lastActualFromObj = NULL;
    AB_TRAVERSAL	refTrav;
    ABObj		refObj = NULL;
    ABObj		module = NULL;
    ABObj		fromObj = NULL;
    ABObj		actualFromObj = NULL;

    if (actionOrObj == NULL)
    {
	return NULL;
    }

    /*
     * Get the "from" obj
     */
    if (obj_is_action(actionOrObj))
    {
	fromObj = obj_get_from(actionOrObj);
    }
    else if (obj_is_ui(actionOrObj))
    {
	fromObj = actionOrObj;
    }

    /*
     * Shortcut, to avoid a lengthy traversal
     */
    if (fromObj == lastFromObj)
    {
	goto epilogue;
    }

    /*
     * See if we can do anything
     */
    if (fromObj == NULL)
    {
	return NULL;
    }
    module = obj_get_module(fromObj);

    /*
     * Actually find it!
     */
    if (   (!obj_is_ref(fromObj))
        && (   (obj_get_type(fromObj) == AB_TYPE_MENU)
            || (obj_get_parent_of_type(fromObj, AB_TYPE_MENU) != NULL)) )
    {
        /* find an object that references this one, to
         * get the appropriate type (only menu references are 
         * supported, currently).
         */
        for (trav_open(&refTrav, module, AB_TRAV_UI);
	        (refObj = trav_next(&refTrav)) != NULL; )
        {
            if (refObj->ref_to == fromObj)
            {
	        break;
            }
        }
        trav_close(&refTrav);
    }

    if (actualFromObj == NULL)
    {
	if (refObj != NULL)
	{
	    actualFromObj = refObj;
	}
	else
	{
	    actualFromObj = fromObj;
	}
    }

    lastFromObj = fromObj;
    lastActualFromObj = actualFromObj;

epilogue:
    return lastActualFromObj;
}


static BOOL
cwd_is_dtbuilder_src_dir(void)
{
    static BOOL		initialized = FALSE;
    static BOOL		isSrcDir = FALSE;

    if (!initialized)
    {
	initialized = TRUE;
        isSrcDir = (util_paths_are_same_file(".", "../../src/ab"));
    }

    return isSrcDir;
}


static STRING
get_to_var_name(void)
{
    return (cwd_is_dtbuilder_src_dir()? instanceVarName:toVarName);
}


static STRING
get_from_var_name(void)
{
    return fromVarName;
}


/*
 * Generate XtVaSetvalues calls in callbacks.
 * obj is the object that will have it's resources set.
 *
 * Resource list must be name,value pairs, NULL-terminated
 * All resource values must be of type (void *)
 */
static int
printf_setval(GenCodeInfo genCodeInfo, ABObj obj, ...)
{
    int			return_value = 0;
    File		codeFile = genCodeInfo->code_file;
#if defined(__osf__) || defined(linux) || defined(CSRG_BASED)
/* OSF/1 define va_list in <va_list.h> as structure of char ** and int
 * Sun define va_list as void * */
    va_list             paramList = { NULL, 0 };
#else
    va_list             paramList = NULL;
#endif /* __osf__ */
    STRING              resName = NULL;
    void		*resValue = NULL;
    STRING		objCName = NULL;
    ISTRING		istrValue = NULL;

    va_start(paramList, obj);
    
    objCName = abmfP_get_c_name(genCodeInfo, obj);
    abio_printf(codeFile, "\tXtVaSetValues(%s,", objCName);
    abio_printf(codeFile, "\n\t\t");

    while ((resName = va_arg(paramList, STRING)) != NULL)
    {
	resValue = va_arg(paramList, VoidPtr);
	switch (abmfP_get_res_type(resName, obj))
	{
	    case AB_ARG_LITERAL:
	    case AB_ARG_PIXEL:
	    case AB_ARG_PIXMAP:
	    case AB_ARG_XMSTRING:
		istrValue = istr_create((STRING)resValue);
		resValue = (VoidPtr)istrValue;
	    break;

	    case AB_ARG_WIDGET:
		util_dprintf(1, 
		    "Ignoring unsupported connection resource type: WIDGET\n");
	    break;
	}
	abmfP_write_arg_val(genCodeInfo, FALSE, resName, resValue, obj);
	abio_printf(codeFile, ",\n\t\t");

	istr_destroy(istrValue);
	resValue = NULL;
    }
    abio_printf(codeFile, "NULL);");

    va_end(paramList);
    return return_value;
}


/*
 * printf_getval - can only get one resource.  This can be changed at a later
 * date if need be. NOTE: We need to declare local variables in one pass,
 * Then write out an XtVaGetValues on a second pass, if we ever want to get
 * more that one value back.
 */
static int
printf_getval(GenCodeInfo genCodeInfo, ABObj obj, int nres,...)
{
    int			return_value = 0;
    File		codeFile = genCodeInfo->code_file;
    va_list             ap;
    STRING              resource;
    ARG_TYPES           type;
    STRING		objCName = NULL;
    nres = nres;		/* avoid warning */

    va_start(ap, nres);

    objCName = abmfP_get_c_name(genCodeInfo, obj);
    resource = va_arg(ap, STRING);
    type = va_arg(ap, ARG_TYPES);

    /* Print the declaration */
    switch (type)
    {
    case IMMED_TYPE:
    case INT_TYPE:
	abio_printf(codeFile, "\tint i;\n\n");
	break;
    case STRING_TYPE:
	abio_printf(codeFile, "\tchar *str;\n\n");
	break;
    }

    abio_printf(codeFile, "\tXtVaGetValues(%s,\n\t\t%s, ", objCName, resource);

    /* Print the local variable reference */
    switch (type)
    {
    case IMMED_TYPE:
    case INT_TYPE:
	abio_printf(codeFile, "&i");
	break;
    case STRING_TYPE:
	abio_printf(codeFile, "&str");
	break;
    }
    abio_printf(codeFile, ",\n\t\tNULL);\n");

    va_end(ap);
    return return_value;
}

static void
set_up_user_type_variables(GenCodeInfo genCodeInfo, ABObj toObj)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               structObj = abmfP_obj_get_struct_obj(toObj);
    ABObj		winParent = NULL;
    char		winParentName[1024];
    *winParentName = 0;

    /*
     * Determine window parent of this popup
     */
    winParent = obj_get_win_parent(structObj);
    if (winParent == NULL)
    {
	sprintf(winParentName, "%s()", abmfP_lib_get_toplevel_widget->name);
    }
    else
    {
        strcpy(winParentName, abmfP_get_c_name(genCodeInfo, winParent));
    }

    /*
     * REMIND: This is obsolete and should be removed!
     */
    if (cwd_is_dtbuilder_src_dir())
    {
        write_instance_ptr_var(genCodeInfo, toObj,
	    instanceVarName, abmfP_client_data_var_name, TRUE, NULL);
	abio_puts(genCodeInfo->code_file, nlstr);
    }
    else
    {
        write_instance_ptr_var(genCodeInfo, toObj,
	    toVarName, abmfP_client_data_var_name, TRUE, NULL);
        write_instance_ptr_var(genCodeInfo, toObj,
	    instanceVarName, toVarName, FALSE, "/* obsolete */");
	abio_puts(genCodeInfo->code_file, nlstr);
    }
    abio_printf(codeFile, "if (!(%s->initialized))\n", get_to_var_name());
    abmfP_write_c_block_begin(genCodeInfo);
    abio_printf(codeFile, "%s(%s, %s);\n",
		abmfP_get_init_proc_name(structObj),
		get_to_var_name(),
		winParentName);
    abmfP_write_c_block_end(genCodeInfo);
}


static int
write_instance_ptr_var(
			GenCodeInfo	genCodeInfo,  
			ABObj		ipObj,
			STRING		instanceVarName,
			STRING		paramName,
			BOOL		castValue,
			STRING		comment
)
{
    FILE	*codeFile = genCodeInfo->code_file;

    if (ipObj == NULL)
    {
	return 0;
    }
    abio_printf(genCodeInfo->code_file, "%s\t%s = ",
		abmfP_get_c_struct_ptr_type_name(ipObj),
		instanceVarName);
    if (castValue)
    {
	abio_printf(codeFile, "(%s)", abmfP_get_c_struct_ptr_type_name(ipObj));
    }

    abio_printf(codeFile, "%s;", paramName);

    if (comment != NULL)
    {
	abio_printf(genCodeInfo->code_file, "\t%s", comment);
    }
    abio_puts(genCodeInfo->code_file, nlstr);
    return 0;
}


static int
write_tooltalk_cb_vars(
    GenCodeInfo genCodeInfo,
    ABObj action 
)
{
    File                codeFile;

    if (!genCodeInfo || !action)
        return 0;

    codeFile = genCodeInfo->code_file;

    switch(obj_get_when(action))
    {
	case AB_WHEN_TOOLTALK_QUIT:
	    abio_puts(codeFile, "int\t\tsilent = 0;\n");
	    abio_puts(codeFile, "int\t\tforce = 0;\n");
	    abio_puts(codeFile, "Boolean\t\tcancel = False;\n");
	    break;
	case AB_WHEN_TOOLTALK_GET_STATUS:
	    abio_puts(codeFile, "char\t\t*status_string = \"\";\n");
	    break;
	case AB_WHEN_TOOLTALK_PAUSE_RESUME:
	    abio_puts(codeFile, "Boolean\t\tsensitive = (Bool)calldata;\n");
	    abio_puts(codeFile, "Boolean\t\talready_set = False;\n");
	    break;
	case AB_WHEN_TOOLTALK_DO_COMMAND:
	    abio_puts(codeFile, "Tt_status\t\tstatus = TT_DESKTOP_ENOTSUP;\n");
	    abio_puts(codeFile, "char\t\t*command;\n");
	    abio_puts(codeFile, "char\t\t*result;\n");
	    break;
	default:
	    break;
    }
    return 0;
}

static int
write_tooltalk_cb_body1(
    GenCodeInfo genCodeInfo,
    ABObj action
)
{
    File                codeFile;

    if (!genCodeInfo || !action)
        return 0;
 
    codeFile = genCodeInfo->code_file; 

    switch(obj_get_when(action))
    {
        case AB_WHEN_TOOLTALK_QUIT:
	    abio_puts(codeFile, "tt_message_arg_ival(msg, 0, &silent);\n");
	    abio_puts(codeFile, "tt_message_arg_ival(msg, 1, &force);\n");
	    abio_puts(codeFile,"\n");
            abio_puts(codeFile, abmfP_comment_begin);
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
              "Process Quit request->\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "if \"silent\" == 1, then the Quit should occur without\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
		"notifying the user.  If \"force\" == 1, then the user may\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "be notified, but the Quit should not be cancellable.\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "If \"silent\" == 0 and \"force\" == 0, then this Quit\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "request may be cancelled, in which case \"cancel\" should\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "be set to True in order send the \"failed\" reply.\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile, 
		"If the Quit request is honored (not cancelled), the\n");
            abio_puts(codeFile, abmfP_comment_continue);  
            abio_puts(codeFile,  
		"application exit should not occur until after the request\n");
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,  
		"has been replied-to.\n");
            abio_puts(codeFile, abmfP_comment_end);
            break;

        case AB_WHEN_TOOLTALK_GET_STATUS:
            abio_puts(codeFile, abmfP_comment_begin);
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
              "Process Get Status request->\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "The application should assign \"status_string\" an appropriate\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,
                "string value reflecting the application's status.\n");
            abio_puts(codeFile, abmfP_comment_end);
            break;
        case AB_WHEN_TOOLTALK_PAUSE_RESUME:
            abio_puts(codeFile, abmfP_comment_begin); 
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile, 
              "Process Pause/Resume request->\n"); 
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,  
                "If \"sensitive\" == False (Pause), then the application should\n"); 
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
                "set the sensitivity on all mapped shells to False.  If\n");  
            abio_puts(codeFile, abmfP_comment_continue);  
            abio_puts(codeFile,    
                "\"sensitive\" == True (Resume), then the application should\n"); 
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "set the sensitivity on all mapped shells to True.n"); 
            abio_puts(codeFile, abmfP_comment_continue);    
            abio_puts(codeFile,      
                "If the application is already in the requested state of\n"); 
            abio_puts(codeFile, abmfP_comment_continue);     
            abio_puts(codeFile,       
		"sensitivity, then \"already_set\" should be set to True.\n");
            abio_puts(codeFile, abmfP_comment_end);
            break;
        case AB_WHEN_TOOLTALK_DO_COMMAND:
            abio_puts(codeFile, "command = tt_message_arg_val(msg, 0);\n");
            abio_puts(codeFile, "\n"); 
    	    abio_puts(codeFile, abmfP_comment_begin);
    	    abio_puts(codeFile, abmfP_comment_continue);
    	    abio_puts(codeFile,
              "Process Do_Command request->\n");
    	    abio_puts(codeFile, abmfP_comment_continue);
	    abio_puts(codeFile, 
		"The application should execute the code defined by \"command\".\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,  
                "If execution is successful, then \"status\" should be set to TT_OK,\n"); 
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"else set to the appropriate Tt_status value.\n");
            abio_puts(codeFile, abmfP_comment_continue);  
            abio_puts(codeFile,    
		"If the command returns a result, then \"result\" should be set\n");
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "to the appropriate result string.\n"); 
    	    abio_puts(codeFile, abmfP_comment_end);
            break;
        default:
            break;
    }
    abio_puts(codeFile, "\n");
    return 0;
}


static int
write_tooltalk_cb_body2(
    GenCodeInfo genCodeInfo,
    ABObj action
)
{
    File                codeFile;

    if (!genCodeInfo || !action)
        return 0;

    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, "\n");

    switch(obj_get_when(action))
    {
        case AB_WHEN_TOOLTALK_QUIT:
	    abio_puts(codeFile, "if (cancel == True)\n");
	    abio_indent(codeFile);
	    abio_puts(codeFile, "tttk_message_fail(msg, TT_DESKTOP_ECANCELED, 0, 1);\n");
	    abio_outdent(codeFile);
	    abio_puts(codeFile, "else\n");
	    abio_puts(codeFile, "{\n");
	    abio_indent(codeFile);
	    write_tooltalk_msg_reply(codeFile);
	    abio_puts(codeFile, "\n");

    	    abmfP_write_c_comment(genCodeInfo, FALSE,
        	"Now that the Message has been replied, process Quit operation");
            abmfP_write_user_code_seg(genCodeInfo, NULL);
            abio_puts(codeFile, nlstr);

	    abio_outdent(codeFile);
	    abio_puts(codeFile, "}\n");
            break;
        case AB_WHEN_TOOLTALK_GET_STATUS:
	    abio_puts(codeFile, "tt_message_arg_val_set(msg, 0, status_string);\n");
            write_tooltalk_msg_reply(codeFile);
            break;
        case AB_WHEN_TOOLTALK_PAUSE_RESUME:
	    abio_puts(codeFile, "if (already_set == True)\n");
	    abio_indent(codeFile);
	    abio_puts(codeFile, "tt_message_status_set(msg, TT_DESKTOP_EALREADY);\n");
	    abio_outdent(codeFile);
            write_tooltalk_msg_reply(codeFile);
            break;
        case AB_WHEN_TOOLTALK_DO_COMMAND:
    	    abio_puts(codeFile, "tt_free(command);\n");
    	    abio_puts(codeFile, "tt_message_status_set(msg, status);\n");
    	    abio_puts(codeFile, "if (tt_is_err(status))\n");
	    abio_indent(codeFile);
	    abio_puts(codeFile, "tttk_message_fail(msg, status, 0, 1);\n");
	    abio_outdent(codeFile);
            abio_puts(codeFile, "else\n");
            abio_puts(codeFile, "{\n");
            abio_indent(codeFile);
	    abio_puts(codeFile, "if (result != NULL)\n");
    	    abio_indent(codeFile);
	    abio_puts(codeFile, "tt_message_arg_val_set(msg, 1, result);\n");
	    abio_outdent(codeFile);
            write_tooltalk_msg_reply(codeFile);
	    abio_outdent(codeFile);
            abio_puts(codeFile, "}\n");
            break;
        default:
            break;
    }

    abio_puts(codeFile, nlstr);
    abmfP_write_user_code_seg(genCodeInfo, NULL);
    abio_puts(codeFile, nlstr);
    abio_puts(codeFile, "return True;\n");
    return 0;
}


static int
write_tooltalk_msg_reply(
    File	codeFile
)
{
     abio_puts(codeFile, "tt_message_reply(msg);\n"); 
     abio_puts(codeFile, "tttk_message_destroy(msg);\n"); 

     return 0;
}


static int
write_ss_cb_vars(
    GenCodeInfo genCodeInfo,
    ABObj action 
)
{
    File                codeFile;

    if (!genCodeInfo || !action)
        return 0;

    codeFile = genCodeInfo->code_file;

    switch(obj_get_when(action))
    {
	case AB_WHEN_SESSION_SAVE:
	case AB_WHEN_SESSION_RESTORE:
	    abio_puts(codeFile, "Boolean\t\tret_value = False;\n");
	    break;
	default:
	    break;
    }
    return 0;
}

static int
write_ss_cb_body1(
    GenCodeInfo genCodeInfo,
    ABObj action
)
{
    File                codeFile;

    if (!genCodeInfo || !action)
        return 0;
 
    codeFile = genCodeInfo->code_file; 

    switch(obj_get_when(action))
    {
	case AB_WHEN_SESSION_RESTORE:
    	    abio_puts(codeFile, abmfP_comment_begin);
    	    abio_puts(codeFile, abmfP_comment_continue);
    	    abio_puts(codeFile,
              "Restore application state via session file.\n");
    	    abio_puts(codeFile, abmfP_comment_continue);
    	    abio_puts(codeFile,
              "Use contents of \"session file\" to bring the application\n");
    	    abio_puts(codeFile, abmfP_comment_continue);
    	    abio_puts(codeFile,
              "to the desired state.\n");
    	    abio_puts(codeFile, abmfP_comment_continue);
    	    abio_puts(codeFile,
              "Set \"ret_value\" to True to indicate success.\n");
    	    abio_puts(codeFile, abmfP_comment_end);
            break;
	case AB_WHEN_SESSION_SAVE:
    	    abio_puts(codeFile, abmfP_comment_begin);
    	    abio_puts(codeFile, abmfP_comment_continue);
    	    abio_puts(codeFile,
              "Save application state in either session file and/or\n");
    	    abio_puts(codeFile, abmfP_comment_continue);
	    abio_puts(codeFile, 
		"command line vector.\n");
            abio_puts(codeFile, abmfP_comment_continue);
	    abio_puts(codeFile, "\n");
            abio_puts(codeFile, abmfP_comment_continue);
            abio_puts(codeFile,  
                "Session File:\n"); 
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    Open the file \"session_file\"\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    Write application state information into it\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    Close the file\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    Set \"ret_value\" to True\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    (Setting \"ret_value\" to True is important if you are using\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    session files! Do not set it to True if you are not using\n");
            abio_puts(codeFile, abmfP_comment_continue); 
            abio_puts(codeFile,   
		"    session files.)\n");
            abio_puts(codeFile, abmfP_comment_continue);  
            abio_puts(codeFile,   "\n");
            abio_puts(codeFile, abmfP_comment_continue);  
            abio_puts(codeFile,    
		"Command line vector:\n");
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "    Allocate an argv vector\n"); 
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "    Fill it up with the appropriate strings\n"); 
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "    Return the argv vector in \"argv\" i.e.\n"); 
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "    \t*argv = my_new_argv;\n"); 
            abio_puts(codeFile, abmfP_comment_continue);   
            abio_puts(codeFile,     
                "    Return the size of the vector in argc\n"); 
    	    abio_puts(codeFile, abmfP_comment_end);
            break;
        default:
            break;
    }
    abio_puts(codeFile, "\n");
    return 0;
}


static int
write_ss_cb_body2(
    GenCodeInfo genCodeInfo,
    ABObj action
)
{
    File                codeFile;

    if (!genCodeInfo || !action)
        return 0;

    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, "\n");
    abio_puts(codeFile, "return ret_value;\n");
    return 0;
}


static int
write_action_functions(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    static char         msg[256],
                       *s;
    AB_TRAVERSAL        trav;
    ABObj               action = NULL;
    ABObj               fromObj = NULL;	/* for error reports */
    int                 i = 0;

    /*
     * Auto-named functions
     */
    for (trav_open(&trav, obj, AB_TRAV_ACTIONS_FOR_OBJ | AB_TRAV_MOD_SAFE), i = 0;
	 (action = trav_next(&trav)) != NULL; ++i)
    {
	if (   mfobj_has_flags(action, CGenFlagIsDuplicateDef)
	    || mfobj_has_flags(action, CGenFlagWriteDefToProjFile))
	{
	    continue;
	}

	if (action->info.action.auto_named)
	{
	    abmfP_write_action_function(genCodeInfo, action);
	    abio_puts(genCodeInfo->code_file, "\n");
	}
    }
    /* don't close traversal, yet */

    /*
     * User-named functions
     */
    for (trav_reset(&trav);
	 (action = trav_next(&trav)) != NULL; ++i)
    {
	if (   mfobj_has_flags(action, CGenFlagIsDuplicateDef)
	    || mfobj_has_flags(action, CGenFlagWriteDefToProjFile))
	{
	    continue;
	}

	if (!(action->info.action.auto_named))
	{
	    abmfP_write_action_function(genCodeInfo, action);
	    abio_puts(genCodeInfo->code_file, "\n");
	}
    }
    trav_close(&trav);

    return OK;
}


/*
 * Write the callback proc stub file.
 */
int
abmfP_write_stubs_c_file(
			 GenCodeInfo genCodeInfo,
			 STRING codeFileName,
			 ABObj module
)
{
    File                codeFile = genCodeInfo->code_file;
    STRING              errmsg = NULL;
    ABObj               win_obj = NULL;
    ABObj               project = obj_get_project(module);
    char		moduleHeaderFileName[MAX_PATH_SIZE];
    char		moduleName[MAX_PATH_SIZE];

    /*
     * Write file header.
     */

    abmfP_write_user_header_seg(genCodeInfo);
    abio_puts(codeFile, nlstr);

    sprintf(moduleName, "module %s", obj_get_name(module));
    abmfP_write_file_header(
		genCodeInfo, 
		codeFileName, 
		FALSE,
		moduleName,
		util_get_program_name(), 
		ABMF_MODIFY_USER_SEGS,
	   " * Contains: Module callbacks and connection functions"
		);


    /*
     * Write includes.
     */
    strcpy(moduleHeaderFileName, abmfP_get_ui_header_file_name(module));
    abmfP_write_c_system_include(genCodeInfo, "stdio.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/Xm.h");
    abmfP_write_c_local_include(genCodeInfo, 
			abmfP_get_utils_header_file_name(module));

    /* 
     * Include project file if i18n is enabled. This file
     * is needed for the message catalog stuff
     */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
        abmfP_write_c_local_include(genCodeInfo,
	    abmfP_get_project_header_file_name(project));
    abmfP_write_c_local_include(genCodeInfo, moduleHeaderFileName);
		    

    /*
     * Write out includes for modules with connection targets
     */
    {
	StringListRec       connIncludes;
	int                 i = 0;
	int                 num_strings = 0;
	strlist_construct(&connIncludes);

	strlist_add_str(&connIncludes, moduleHeaderFileName, NULL);
	abmfP_get_connect_includes(&connIncludes, module);
	num_strings = strlist_get_num_strs(&connIncludes);
	if (num_strings > 1)		/* start at 1 - skip this_ui.h */
	{
            abio_puts(codeFile, nlstr);
	    abmfP_write_c_comment(genCodeInfo, FALSE,
		"Header files for cross-module connections");
	}
	for (i = 1; i < num_strings; ++i)
	{
	    abio_printf(codeFile, "#include %s\n",
				  strlist_get_str(&connIncludes, i, NULL));
	}

	strlist_destruct(&connIncludes);
    }
    abio_puts(codeFile, nlstr);

    abmfP_write_user_file_top_seg(genCodeInfo);
    abio_puts(codeFile, nlstr);

    if (write_action_functions(genCodeInfo, module) != OK)
	return ERROR;

    abmfP_write_user_file_bottom_seg(genCodeInfo);
    return OK;
}


int
abmfP_write_action_function(
			GenCodeInfo	genCodeInfo, 
			ABObj		action
)
{
    int			rc = 0;			/* return code */
    BOOL		isTTCB = FALSE;
    BOOL		ss_cb = FALSE;
    File                codeFile = genCodeInfo->code_file;
    BOOL		topUserSegWritten = FALSE;
    BOOL		bottomUserSegWritten = FALSE;
    BOOL		funcBodyWritten = FALSE;
    BOOL		funcEndWritten = FALSE;
    BOOL		actionPrintfWritten = FALSE;
    int                 return_value = 0;
    ABObj               fromObj = obj_get_from(action);
    ABObj		actualFromObj = NULL;
    ABObj		toObj = obj_get_to(action);
    ABObj		module = NULL;
    char		actionName[1024];
    char		actionPrintf[1024];

    abmfP_gencode_enter_func(genCodeInfo);
    abmfP_ip_obj(genCodeInfo) = obj_get_to(action);
    util_strncpy(actionName, abmfP_get_action_name(action), 1024);
    sprintf(actionPrintf, "printf(\"action: %s()\\n\");\n", actionName);

    /***
     *** START OF FUNCTION
     ***/

    switch (obj_get_when(action))
    {
    case AB_WHEN_AFTER_CREATED:
	/* 
	 * post-create procs have the signature of an Xt Callback,
	 * although they are called as conventional functions.
	 */
        fromObj = obj_get_from(action);
	actualFromObj = get_actual_from_obj(action);
        abmfP_write_xm_callback_begin(genCodeInfo, FALSE, actionName);
        write_instance_ptr_var(genCodeInfo, actualFromObj,
	    get_from_var_name(), "callData", TRUE, NULL);
	abio_puts(genCodeInfo->code_file, nlstr);

    break;

    case AB_WHEN_DRAGGED_FROM:
    {
	abio_printf(genCodeInfo->code_file, 
	    abmfP_lib_default_dragCB->def,	/* this is a format string */
		actionName,actionName,actionName,
		actionName,actionName, actionName);
	abio_puts(genCodeInfo->code_file, "\n\n");

	/* these are all in the "library" definition */
	topUserSegWritten = TRUE;
	bottomUserSegWritten = TRUE;
	funcBodyWritten = TRUE;
	funcEndWritten = TRUE;
	actionPrintfWritten = TRUE;
    }
    break;

    case AB_WHEN_DROPPED_ON:
    {
	abio_printf(genCodeInfo->code_file, 
	    abmfP_lib_default_dropCB->def,	/* this is a format string */
		actionName,actionName,actionName,actionName);
	abio_puts(genCodeInfo->code_file, "\n\n");

	/* these are all in the "library" definition */
	topUserSegWritten = TRUE;
	bottomUserSegWritten = TRUE;
	funcBodyWritten = TRUE;
	funcEndWritten = TRUE;
	actionPrintfWritten = TRUE;
    }
    break;

    case AB_WHEN_TOOLTALK_QUIT:
    case AB_WHEN_TOOLTALK_DO_COMMAND:
    case AB_WHEN_TOOLTALK_GET_STATUS:
    case AB_WHEN_TOOLTALK_PAUSE_RESUME:
	isTTCB = TRUE;
	    abio_printf(codeFile, begin_tt_callback_body, actionName);
            abmfP_write_c_block_begin(genCodeInfo);
            write_tooltalk_cb_vars(genCodeInfo, action);
    break;

    case AB_WHEN_SESSION_RESTORE:
	ss_cb = TRUE;
	    abio_printf(codeFile, begin_ss_restore_callback_body,
		abmfP_get_action_name(action));
            abmfP_write_c_block_begin(genCodeInfo);
            write_ss_cb_vars(genCodeInfo, action);
    break;

    case AB_WHEN_SESSION_SAVE:
	ss_cb = TRUE;
	    abio_printf(codeFile, begin_ss_save_callback_body,
		abmfP_get_action_name(action));
            abmfP_write_c_block_begin(genCodeInfo);
            write_ss_cb_vars(genCodeInfo, action);
    break;

    default:
        abmfP_write_xm_callback_begin(genCodeInfo, FALSE, actionName);
    break;

    } /* switch obj_get_when() */


    /***** 
     ***** TOP USER SEGMENT
     *****/

    if (!topUserSegWritten)
    {
	STRING	contents = 
	    	    (actionPrintfWritten? NULL:(isTTCB? actionPrintf:NULL));
        abmfP_write_user_var_and_code_seg(genCodeInfo, contents);
        abio_puts(codeFile, nlstr);
	topUserSegWritten = TRUE;
	if (contents != NULL)
	{
	    actionPrintfWritten = TRUE;
	}
    }

    /***
     *** FUNCTION BODY
     ***/

    if (isTTCB)
    {
        write_tooltalk_cb_body1(genCodeInfo, action);
        abmfP_write_user_code_seg(genCodeInfo, NULL);
        write_tooltalk_cb_body2(genCodeInfo, action);
	funcBodyWritten = TRUE;
	bottomUserSegWritten = TRUE; 
    }
    else if (ss_cb)
    {
        write_ss_cb_body1(genCodeInfo, action);
        abmfP_write_user_code_seg(genCodeInfo, NULL);
        write_ss_cb_body2(genCodeInfo, action);
	funcBodyWritten = TRUE;
	bottomUserSegWritten = TRUE;
    }
    else if (!funcBodyWritten) switch (obj_get_func_type(action))
    {
    case AB_FUNC_BUILTIN:
	rc = abmfP_write_builtin_action(genCodeInfo, action, TRUE);
	return_if_err(rc,rc);
	funcBodyWritten = TRUE;
	break;

    case AB_FUNC_USER_DEF:
	abmfP_write_user_start_comment(genCodeInfo, "vvv Add C code below vvv");
	abmfP_write_user_end_comment(genCodeInfo, "^^^ Add C code above ^^^");
	bottomUserSegWritten = TRUE;
	funcBodyWritten = TRUE;
	break;

    case AB_FUNC_CODE_FRAG:
	abio_puts(codeFile, obj_get_func_code(action));
	funcBodyWritten = TRUE;
	break;

    case AB_FUNC_ON_ITEM_HELP:
	abio_printf(codeFile, "dtb_do_onitem_help();\n");
	funcBodyWritten = TRUE;
	break;

    case AB_FUNC_HELP_VOLUME:
	abio_printf(codeFile, 
		"dtb_show_help_volume_info(\"%s\", \"%s\");\n",
		istr_string(action->info.action.volume_id), 
		istr_string(action->info.action.location));
	funcBodyWritten = TRUE;
	break;

    default:
	{
	    char *obj_name_string = obj_get_name(fromObj);
	    util_printf_err(catgets(Dtb_project_catd, 1, 78,
		    "unknown function type for action from object, %s"),
		    obj_name_string);
	    return_code(ERR);
	}
	break;
    }

    /***** 
     ***** BOTTOM USER SEGMENT
     *****/

    if (!bottomUserSegWritten)
    {
	STRING	contents = (actionPrintfWritten? NULL:actionPrintf);
	abmfP_write_user_code_seg(genCodeInfo, contents);
	bottomUserSegWritten = TRUE;
	if (contents != NULL)
	{
	    actionPrintfWritten = TRUE;
	}
    }

    /***** 
     ***** FUNCTION END 
     *****/

    if (!funcEndWritten)
    {
        abmfP_write_c_func_end(genCodeInfo, NULL);
	funcEndWritten = TRUE;
    }

epilogue:
    abmfP_gencode_exit_func(genCodeInfo);
    return return_value;
}


int
abmfP_write_builtin_action(
			GenCodeInfo	genCodeInfo, 
			ABObj 		action,
			BOOL		setUpVars
)
{
    int			return_value     = 0;
    File                codeFile         = genCodeInfo->code_file;
    ABObj               toObj            = NULL;
    STRING		actionTKResource = NULL;	/* in toolkit */
    STRING		actionResource   = NULL;	/* in source code */
    ISTRING		istr_resource    = NULL;
    AB_BUILTIN_ACTION	builtin_action   = AB_STDACT_UNDEF;

    toObj          = obj_get_to(action);
    builtin_action = obj_get_func_builtin(action);
    if (toObj != NULL)
    {
        toObj = objxm_comp_get_target_for_builtin_action(toObj, builtin_action);
    }
    if (toObj == NULL)
    {
	abmfP_write_c_comment(genCodeInfo, FALSE, "Invalid action ignored.");
	abio_puts(codeFile, nlstr);
	return 0;
    }

    /*
     * We need to convert the resource into a string that can be put
     * in the .c file.
     */
    actionTKResource = objxm_get_resource_for_builtin_action(
				toObj, builtin_action);
    istr_resource    = objxm_get_res_strname(actionTKResource);
    actionResource   = istr_string(istr_resource);

    if (abmfP_get_c_name(genCodeInfo, toObj) == NULL)
    {
	return_value = write_builtin_action_for_ref(genCodeInfo,
	    action, toObj, setUpVars, builtin_action, actionTKResource,
	    actionResource);
    }
    else
    {
	if (setUpVars)
	{
	    set_up_user_type_variables(genCodeInfo, toObj);
	}
	
	return_value = write_builtin_action(genCodeInfo, action, toObj,
	    builtin_action, actionTKResource, actionResource);
    }

    if (return_value == 0)
	abio_printf(codeFile, nlstr);
	
epiloge:
    abio_printf(codeFile, nlstr);
    return return_value;
}


/*
** we are on a virtual object and need to write the actual builtin action
** for each ref to it.
*/
static int
write_builtin_action_for_ref(
    GenCodeInfo	      genCodeInfo, 
    ABObj 	      action,
    ABObj             toObj,
    BOOL              setUpVars,
    AB_BUILTIN_ACTION builtin_action,
    STRING            actionTKResource,		       /* in toolkit */
    STRING	      actionResource		       /* in source code */
)
{
    int                        i = 0;
    int                  numRefs = 0;
    int             return_value = 0;
    ABObj                 refObj = NULL;
    ABObjList            refList = NULL;
	
    if (!genCodeInfo || !action || !toObj)
	return -1;
    
    refList = abmfP_find_refs_to(toObj);
    numRefs = objlist_get_num_objs(refList);
    
    if ((refList != NULL) && (numRefs > 0))
    {
	if (setUpVars)
	{
	    refObj = objlist_get_obj(refList, 0, NULL);
	    set_up_user_type_variables(genCodeInfo, refObj);
	}
	    
	/* write the builtin actions */
	for (i = 0; i < numRefs; ++i)
	{
	    refObj  = objlist_get_obj(refList, i, NULL);

	    if (abmfP_get_c_name(genCodeInfo, refObj) != NULL)
	    {
		return_value = write_builtin_action(genCodeInfo,
		    action, refObj, builtin_action, actionTKResource,
		    actionResource);
	    }
	}
    }
    else
    {
	util_dprintf(0, "ERROR: write_builtin_action_for_ref(): ");
	util_dprintf(0, "Unable to obtain references to the object.\n");
    }

epiloge:
    objlist_destroy(refList);
    return return_value;
}


/*
** determine which builtin action and write the appropriate code.
*/
static int
write_builtin_action(
    GenCodeInfo	      genCodeInfo, 
    ABObj 	      action,
    ABObj             toObj,
    AB_BUILTIN_ACTION builtin_action,
    STRING            actionTKResource,		       /* in toolkit */
    STRING	      actionResource		       /* in source code */
)
{
#define	 IS(type)	(obj_get_func_type(action) ==(type))
#define	 SVAL(action)	((STRING)obj_get_arg_string(action))
#define	 IVAL(action)	((int)obj_get_arg_int(action))

    File                codeFile     = genCodeInfo->code_file;
    int			return_value = 0;

    if (!codeFile || !action || !toObj || !builtin_action)
	return return_value;

    switch (builtin_action)
    {
	case AB_STDACT_ENABLE:
	    abio_print_line(codeFile, NULL);
	    abio_print_line(codeFile, "XtSetSensitive(%s, True);",
		abmfP_get_c_name(genCodeInfo, toObj));
	    break;

	case AB_STDACT_DISABLE:
	    abio_print_line(codeFile, NULL);
	    abio_print_line(codeFile, "XtSetSensitive(%s, False);",
		abmfP_get_c_name(genCodeInfo, toObj));
	    break;

	case AB_STDACT_SHOW:
	    switch((obj_get_root(toObj))->type)
	    {
		case AB_TYPE_BASE_WINDOW:
		    abio_print_line(codeFile, "XtPopup(%s, XtGrabNone);",
			abmfP_get_c_name(genCodeInfo, toObj));
		    break;
			
		case AB_TYPE_DIALOG: 
		case AB_TYPE_FILE_CHOOSER: 
		default:
		    abio_print_line(codeFile, "XtManageChild(%s);",
			abmfP_get_c_name(genCodeInfo, toObj));
		    break;
	    }
	    break;

	case AB_STDACT_HIDE:
	    switch((obj_get_root(toObj))->type) 
	    { 
		case AB_TYPE_BASE_WINDOW: 
		    abio_print_line(codeFile, "XtPopdown(%s);",
			abmfP_get_c_name(genCodeInfo, toObj));
		    break;

		case AB_TYPE_DIALOG:  
		case AB_TYPE_FILE_CHOOSER:  
		default:
		    abio_print_line(codeFile, "XtUnmanageChild(%s);",
			abmfP_get_c_name(genCodeInfo, toObj));
		    break;
	    }
	    break;

	case AB_STDACT_SET_LABEL:
	    /* check toObj for root */
	    if ( obj_get_label_type(obj_get_root(toObj)) == AB_LABEL_GLYPH)
	    {
		abio_printf(codeFile, "%s(%s, ",
		    abmfP_lib_set_label_from_image_file->name,
		    abmfP_get_c_name(genCodeInfo, toObj));
		abio_put_string(codeFile, SVAL(action));
		abio_puts(codeFile, ");\n");
	    }
	    else
	    {
		assert(actionTKResource != NULL);
		printf_setval(genCodeInfo, toObj,
		    actionTKResource, SVAL(action),
		    NULL);
	    }
	    break;

	case AB_STDACT_SET_VALUE:
	    printf_setval(genCodeInfo, toObj, 
		actionTKResource, IVAL(action), NULL);
	    break;

	case AB_STDACT_SET_TEXT:
	    printf_setval(genCodeInfo, toObj, 
		actionTKResource, SVAL(action),
		NULL);
	    break;

/* 	case AB_STDACT_SHOW_HELP: */
/* 	    abio_print_line(codeFile, NULL); */
/* 	    abio_print_line(codeFile,  */
/* 		"XtCallCallbacks(%s,XmNhelpCallback,(XtPointer)NULL);", */
/* 		abmfP_get_c_name(genCodeInfo, toObj)); */
/* 	    break; */

	default:
	    {
		static char         msg[255];

		if (obj_get_name(action) != NULL)
		{
		    char *action_name = obj_get_name(action);
		    sprintf(msg, catgets(Dtb_project_catd, 1, 76,
			"Unknown action name, %s"), action_name);
		}
		else
		{
		    int action_type = obj_get_func_builtin(action);
		    sprintf(msg, catgets(Dtb_project_catd, 1, 77,
			"Unknown action type, %d"), action_type);
		}
		util_error(msg);
		return_value = ERR_INTERNAL;

		/*
		 * return msg; Just print message and go on - JT
		 */
	    }
	    break;
    }			/* switch func.value.builtin */

    return return_value;
#undef IS
#undef SVAL
#undef IVAL
}


