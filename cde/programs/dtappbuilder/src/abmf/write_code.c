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
 * $XConsortium: write_code.c /main/3 1995/11/06 18:18:22 rswiston $
 * 
 * @(#)write_code.c	1.18 16 Feb 1994 cde_app_builder/src/abmf
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
 * write_code.c
 * 
 * Writes entire files, projects, et cetera.
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <ab_private/util.h>
#include <ab_private/util_err.h>
#include <ab_private/abio.h>
#include "utilsP.h"
#include "instancesP.h"
#include "write_codeP.h"
#include "msg_file.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)                                  **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)                  **
**                                                                      **
**************************************************************************/



/************************************************************************
**                                                                      **
**       Data                                                           **
**                                                                      **
**************************************************************************/
char *const abmfP_cmt_user_code_start = "DTB_USER_CODE_START";
char *const abmfP_cmt_user_code_end = "DTB_USER_CODE_END";


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/


/*************************************************************************
 **									**
 **			GenCodeInfo type				**
 **									**
 *************************************************************************/

int 
abmfP_gencode_construct(GenCodeInfo gen)
{
    gen->code_file = NULL;
    gen->resource_file = NULL;
    gen->prototype_funcs = TRUE;
    gen->dumped_resources = AB_ARG_CLASS_FLAGS_NONE;
    gen->i18n_method = ABMF_I18N_NONE;
    gen->msg_src_file_name = (STRING)NULL;
    gen->msg_file_obj = (MsgFile)NULL;

    /* init cur_func substructure */
    abmfP_gencode_enter_func(gen);
    gen->writing_func = FALSE;

    return 0;
}


int
abmfP_gencode_destruct(GenCodeInfo gen)
{
    abmfP_gencode_exit_func(gen);
    return 0;
}


/*
 */
int
abmfP_gencode_enter_func(GenCodeInfo genCodeInfo)
{
    genCodeInfo->writing_func= TRUE;
    genCodeInfo->cur_func.ip_obj = NULL;
    genCodeInfo->cur_func.create_obj = NULL;
    genCodeInfo->cur_func.args_var = NULL;
    genCodeInfo->cur_func.args_var_has_value = FALSE;
    genCodeInfo->cur_func.counter_var = NULL;
    genCodeInfo->cur_func.counter_var_has_value = FALSE;
    genCodeInfo->cur_func.arg_counter_var = NULL;
    genCodeInfo->cur_func.arg_counter_var_has_value = FALSE;
    genCodeInfo->cur_func.display_var = NULL;
    genCodeInfo->cur_func.display_var_has_value = FALSE;
    genCodeInfo->cur_func.drawable_var = NULL;
    genCodeInfo->cur_func.drawable_var_has_value = FALSE;
    genCodeInfo->cur_func.icon_pixmap_var = NULL;
    genCodeInfo->cur_func.icon_pixmap_var_has_value = FALSE;
    genCodeInfo->cur_func.icon_mask_pixmap_var = NULL;
    genCodeInfo->cur_func.icon_mask_pixmap_var_has_value = FALSE;
    genCodeInfo->cur_func.label_pixmap_var = NULL;
    genCodeInfo->cur_func.label_pixmap_var_has_value = FALSE;
    genCodeInfo->cur_func.label_str_var = NULL;
    genCodeInfo->cur_func.label_str_var_has_value = FALSE;
    genCodeInfo->cur_func.label_xmstr_var = NULL;
    genCodeInfo->cur_func.label_xmstr_var_has_value = FALSE;
    genCodeInfo->cur_func.acceltext_xmstr_var = NULL;
    genCodeInfo->cur_func.acceltext_xmstr_var_has_value = FALSE;
    genCodeInfo->cur_func.label_array_var = NULL;
    genCodeInfo->cur_func.label_array_has_nulls = FALSE;
    genCodeInfo->cur_func.mnemonic_array_var = NULL;
    genCodeInfo->cur_func.mnemonic_array_var_has_value = FALSE;
    genCodeInfo->cur_func.name_array_var = NULL;
    genCodeInfo->cur_func.name_array_var_has_value = FALSE;
    genCodeInfo->cur_func.name_param = NULL;
    genCodeInfo->cur_func.name_param_has_value = FALSE;
    genCodeInfo->cur_func.parent_param = NULL;
    genCodeInfo->cur_func.parent_param_has_value = FALSE;
    genCodeInfo->cur_func.return_code_var = NULL;
    genCodeInfo->cur_func.return_code_var_has_value = FALSE;

    genCodeInfo->cur_func.title_str_var = NULL;
    genCodeInfo->cur_func.title_str_var_has_value = FALSE;
    genCodeInfo->cur_func.value_str_var = NULL;
    genCodeInfo->cur_func.value_str_var_has_value = FALSE;
    genCodeInfo->cur_func.icon_name_str_var = NULL;
    genCodeInfo->cur_func.icon_name_str_var_has_value = FALSE;
    genCodeInfo->cur_func.ok_label_xmstr_var = NULL;
    genCodeInfo->cur_func.ok_label_xmstr_var_has_value = FALSE;
    genCodeInfo->cur_func.directory_xmstr_var = NULL;
    genCodeInfo->cur_func.directory_xmstr_var_has_value = FALSE;
    genCodeInfo->cur_func.pattern_xmstr_var = NULL;
    genCodeInfo->cur_func.pattern_xmstr_var_has_value = FALSE;

    strlist_construct(&(genCodeInfo->cur_func.submenu_params));
    strlist_set_is_unique(
	&(genCodeInfo->cur_func.submenu_params), FALSE);

    strlist_construct(&(genCodeInfo->cur_func.submenu_param_types));
    strlist_set_is_unique(
	&(genCodeInfo->cur_func.submenu_params), FALSE);

    return 0;
}


/*
 */
int
abmfP_gencode_exit_func(GenCodeInfo genCodeInfo)
{
     genCodeInfo->writing_func= FALSE;
     istr_destroy(genCodeInfo->cur_func.args_var);
     istr_destroy(genCodeInfo->cur_func.counter_var);
     istr_destroy(genCodeInfo->cur_func.arg_counter_var);
     istr_destroy(genCodeInfo->cur_func.display_var);
     istr_destroy(genCodeInfo->cur_func.drawable_var);
     istr_destroy(genCodeInfo->cur_func.icon_pixmap_var);
     istr_destroy(genCodeInfo->cur_func.icon_mask_pixmap_var);
     istr_destroy(genCodeInfo->cur_func.label_pixmap_var);
     istr_destroy(genCodeInfo->cur_func.label_str_var);
     istr_destroy(genCodeInfo->cur_func.label_xmstr_var);
     istr_destroy(genCodeInfo->cur_func.acceltext_xmstr_var);
     istr_destroy(genCodeInfo->cur_func.label_array_var);
     istr_destroy(genCodeInfo->cur_func.mnemonic_array_var);
     istr_destroy(genCodeInfo->cur_func.name_array_var);
     istr_destroy(genCodeInfo->cur_func.name_param);
     istr_destroy(genCodeInfo->cur_func.parent_param);
     istr_destroy(genCodeInfo->cur_func.return_code_var);
     strlist_make_empty(&(genCodeInfo->cur_func.submenu_params));
     strlist_make_empty(&(genCodeInfo->cur_func.submenu_param_types));
     istr_destroy(genCodeInfo->cur_func.title_str_var);
     istr_destroy(genCodeInfo->cur_func.value_str_var);
     istr_destroy(genCodeInfo->cur_func.icon_name_str_var);
     istr_destroy(genCodeInfo->cur_func.ok_label_xmstr_var);
     istr_destroy(genCodeInfo->cur_func.directory_xmstr_var);
     istr_destroy(genCodeInfo->cur_func.pattern_xmstr_var);
     return 0;
}


BOOL
abmfP_obj_is_struct_obj(ABObj obj)
{
    return (obj_is_window(obj) || obj_is_message(obj));
}



BOOL
abmfP_obj_has_struct_field(ABObj obj)
{
    switch (obj_get_type(obj))
    {
	case AB_TYPE_ITEM:
	    return FALSE;	/* all items are substruct fields */
    }

    if (obj_has_flag(obj, NoCodeGenFlag))
    {
	return FALSE;
    }

    switch (obj_get_type(obj))
    {
	case AB_TYPE_MODULE:
	case AB_TYPE_PROJECT:
	case AB_TYPE_UNKNOWN:
	    return FALSE;
    }

    return TRUE;
}

BOOL
abmfP_obj_has_substruct_field(ABObj obj)
{
    return ((obj_is_item(obj)) &&
	    (obj_get_item_type(obj) != AB_ITEM_FOR_LIST) &&
	    (obj_get_item_type(obj) != AB_ITEM_FOR_COMBO_BOX) &&
	    (obj_get_item_type(obj) != AB_ITEM_FOR_SPIN_BOX));
}


BOOL
abmfP_obj_has_field(ABObj obj)
{
    return (   abmfP_obj_has_struct_field(obj) 
	    || abmfP_obj_has_substruct_field(obj) );
}


/*
 * A substruct consists of any items the object may have. List items
 * never have fields, because they are not widgets.
 */
BOOL
abmfP_obj_is_substruct_obj(ABObj obj)
{
    ABObj	item = NULL;
    if (mfobj_has_flags(obj, CGenFlagIsSubstructObjValid))
    {
	goto epilogue;
    }

    mfobj_set_flags(obj, CGenFlagIsSubstructObjValid);
    item = obj_get_item(obj, 0);
    if ((item != NULL) && 
	(obj_get_item_type(item) != AB_ITEM_FOR_LIST) &&
	(obj_get_item_type(item) != AB_ITEM_FOR_COMBO_BOX) &&
	(obj_get_item_type(item) != AB_ITEM_FOR_SPIN_BOX))
    {
        mfobj_set_flags(obj, CGenFlagIsSubstructObj);
    }

epilogue:
    return mfobj_has_flags(obj, CGenFlagIsSubstructObj);
}


/*
 * The substruct consists of our composite subobjects and items
 */
static int
count_possible_substruct_fields(ABObj obj)
{
    return obj_get_num_items(obj);
}


/*
 * A substruct object is one that could have a structure defined to
 * define it (as opposed to being only a widget).
 */
BOOL
abmfP_obj_could_be_substruct_obj(ABObj obj)
{
    return abmfP_obj_is_substruct_obj(obj);
}

int	
abmfP_write_user_start_comment(
			GenCodeInfo genCodeInfo, 
			STRING desc
)
{
    abio_printf(genCodeInfo->code_file, "/*** %s %s ***/\n",
	abmfP_cmt_user_code_start,
	desc);
    return 0;
}


int	
abmfP_write_user_long_start_comment(
			GenCodeInfo	genCodeInfo, 
			STRING		desc
)
{
    File	codeFile = genCodeInfo->code_file;
    if (desc == NULL)
    {
	desc = "Add additional code here";
    }
abio_printf(codeFile, "/**************************************************************************\n");
abio_printf(codeFile, " *** %s\n", abmfP_cmt_user_code_start);
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " *** %s\n", desc);
abio_printf(codeFile, " ***/\n");
    return 0;
}


int	
abmfP_write_user_end_comment(
			GenCodeInfo	genCodeInfo, 
			STRING desc
)
{
    abio_printf(genCodeInfo->code_file, "/*** %s   %s ***/\n",
	abmfP_cmt_user_code_end,
	desc);
    return 0;
}


int	
abmfP_write_user_long_end_comment(
			GenCodeInfo	genCodeInfo, 
			STRING		desc
)
{
    File	codeFile = genCodeInfo->code_file;
    if (desc == NULL)
    {
	desc = "End of user code section";
    }

abio_printf(codeFile, "/*** %s\n", abmfP_cmt_user_code_end);
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " *** %s\n", desc);
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " **************************************************************************/\n");
    return 0;
}


int	
abmfP_write_user_seg(GenCodeInfo genCodeInfo, STRING desc, STRING contents)
{
    File	codeFile = genCodeInfo->code_file;

    /* start comment */
    abio_printf(codeFile, "/*** %s vvv %s below vvv ***/\n",
	abmfP_cmt_user_code_start,
	desc);

    /* contents */
    if (contents != NULL)
    {
        abio_puts(codeFile, contents);
    }

    /* end comment */
    abio_printf(codeFile, "/*** %s   ^^^ %s above ^^^ ***/\n",
	abmfP_cmt_user_code_end,
	desc);
   
    return 0;
}


int
abmfP_write_user_long_seg(GenCodeInfo genCodeInfo, STRING desc)
{
    abio_puts(genCodeInfo->code_file, nlstr);
    abmfP_write_user_long_start_comment(genCodeInfo, desc);
    abio_puts(genCodeInfo->code_file, nlstr);
    abmfP_write_user_long_end_comment(genCodeInfo, NULL);
    abio_puts(genCodeInfo->code_file, "\n\n");
    return 0;
}



int	
abmfP_write_user_type_and_var_seg(GenCodeInfo genCodeInfo)
{
    return abmfP_write_user_seg(genCodeInfo, 
		"Add type and variable definitions", NULL);
}


int	
abmfP_write_user_struct_fields_seg(GenCodeInfo genCodeInfo)
{
    return abmfP_write_user_seg(genCodeInfo, "Add structure fields", NULL);
}


int
abmfP_write_user_funcs_seg(GenCodeInfo genCodeInfo)
{
    return abmfP_write_user_seg(genCodeInfo, "Add user-defined functions", NULL);
}


int
abmfP_write_user_code_seg(GenCodeInfo genCodeInfo, STRING contents)
{
    return abmfP_write_user_seg(genCodeInfo, "Add C code", contents);
}


int	
abmfP_write_user_var_seg(GenCodeInfo genCodeInfo)
{
    return abmfP_write_user_seg(genCodeInfo, "Add C variables", NULL);
}


int	
abmfP_write_user_var_and_code_seg(GenCodeInfo genCodeInfo, STRING contents)
{
    return abmfP_write_user_seg(genCodeInfo, 
		"Add C variables and code", contents);
}


int	
abmfP_write_user_header_seg(GenCodeInfo genCodeInfo)
{
    return abmfP_write_user_seg(genCodeInfo, "Add file header", NULL);
}

int	
abmfP_write_user_file_top_seg(GenCodeInfo genCodeInfo)
{
    File	codeFile = genCodeInfo->code_file;

abio_printf(codeFile, nlstr);
abio_printf(codeFile, "/**************************************************************************\n");
abio_printf(codeFile, " *** %s\n", abmfP_cmt_user_code_start);
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " *** All necessary header files have been included.\n");
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " *** Add include files, types, macros, externs, and user functions here.\n");
abio_printf(codeFile, " ***/\n");

    abio_puts(codeFile, nlstr);
    abmfP_write_user_long_end_comment(genCodeInfo, NULL);
    abio_puts(codeFile, "\n\n");
    return 0;
}


int	
abmfP_write_user_file_bottom_seg(GenCodeInfo genCodeInfo)
{
    File	codeFile = genCodeInfo->code_file;

abio_printf(codeFile, nlstr);
abio_printf(codeFile, "/**************************************************************************\n");
abio_printf(codeFile, " *** %s\n", abmfP_cmt_user_code_start);
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " *** All automatically-generated data and functions have been defined.\n");
abio_printf(codeFile, " ***\n");
abio_printf(codeFile, " *** Add new functions here, or at the top of the file.\n");
abio_printf(codeFile, " ***/\n");
abio_printf(codeFile, nlstr);
    abmfP_write_user_long_end_comment(genCodeInfo, NULL);
    abio_puts(codeFile, "\n\n");
    return 0;
}

