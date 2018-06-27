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
 * $XConsortium: ui_header_file.c /main/3 1995/11/06 18:15:39 rswiston $
 * 
 * @(#)ui_header_file.c 3.51 15 Feb 1994        cde_app_builder/src/abmf
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
 * Creates a Motif application header file containing standard include files,
 * widget specific include files, decls of User Interface Objects
 * (Widgets) decls of User Create Procedures, decls of
 * connection routines, and decls of callback routines.
 * 
 * This file should be included in the application in order for it to reference
 * the user module objects, create procs, and callback routines.
 */

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ab/util_types.h>
#include <ab_private/abio.h>
#include "abmfP.h"
#include "cdefsP.h"
#include "motifdefsP.h"
#include "obj_namesP.h"
#include "utilsP.h"
#include "instancesP.h"
#include "connectP.h"
#include "write_cP.h"
#include "lib_func_stringsP.h"
#include "ui_header_fileP.h"

typedef struct
{
    STRING              include_file;   /* file to include */
}                   IncludeFileRec, *IncludeFile;

/*
 * Widget Class to Widget Specific includes mapping
 */
typedef struct
{
    ISTRING             class_name;
    IncludeFile         include;
}                   IncludeForClassRec, *IncludeForClass;

/*
 * One entry for each include file, so that multiple classes including the
 * same file (e.g., sliders and gauges, control areas and settings) don't
 * cause the file to be included twice.
 */
static IncludeFileRec incStandardLib = {"<stdlib.h>"};
static IncludeFileRec incArrowButton = {"<Xm/ArrowB.h>"};
static IncludeFileRec incBulletinBoard = {"<Xm/BulletinB.h>"};
static IncludeFileRec incCascadeButton = {"<Xm/CascadeB.h>"};
static IncludeFileRec incComboBox= {"<Dt/ComboBox.h>"};
static IncludeFileRec incDialogShell = {"<Xm/DialogS.h>"};
static IncludeFileRec incDrawingArea = {"<Xm/DrawingA.h>"};
static IncludeFileRec incDrawnButton = {"<Xm/DrawnB.h>"};
static IncludeFileRec incodeFileChooser = {"<Xm/FileSB.h>"};
static IncludeFileRec incForm = {"<Xm/Form.h>"};
static IncludeFileRec incFrame = {"<Xm/Frame.h>"};
static IncludeFileRec incIntrinsic=     {"<X11/Intrinsic.h>"};
static IncludeFileRec incLabel = {"<Xm/Label.h>"};
static IncludeFileRec incList = {"<Xm/List.h>"};
static IncludeFileRec incMainWindow = {"<Xm/MainW.h>"};
static IncludeFileRec incMenuShell = {"<Xm/MenuShell.h>"};
static IncludeFileRec incMenuButton = {"<Dt/MenuButton.h>"};
static IncludeFileRec incMwmUtil = {"<Xm/MwmUtil.h>"};
static IncludeFileRec incPanedWindow = {"<Xm/PanedW.h>"};
static IncludeFileRec incPushButton = {"<Xm/PushB.h>"};
static IncludeFileRec incRowColumn = {"<Xm/RowColumn.h>"};
static IncludeFileRec incScale = {"<Xm/Scale.h>"};
static IncludeFileRec incScrollbar = {"<Xm/ScrollBar.h>"};
static IncludeFileRec incScrolledWindow = {"<Xm/ScrolledW.h>"};
static IncludeFileRec incSeparator = {"<Xm/Separator.h>"};
static IncludeFileRec incShell = {"<X11/Shell.h>"};
static IncludeFileRec incSpinBox= {"<Dt/SpinBox.h>"};
static IncludeFileRec incStringDefs = {"<X11/StringDefs.h>"};
static IncludeFileRec incTerm = {"<Dt/Term.h>"};
static IncludeFileRec incTermPrim = {"<Dt/TermPrim.h>"};
static IncludeFileRec incText = {"<Xm/Text.h>"};
static IncludeFileRec incTextField = {"<Xm/TextF.h>"};
static IncludeFileRec incToggleButton = {"<Xm/ToggleB.h>"};
static IncludeFileRec incXm = {"<Xm/Xm.h>"};
static IncludeFileRec incDtbUtils = {"\"dtb_utils.h\""};


/*
 * Note - more than one table entry may exist for a given class!
 */
static IncludeForClass class_includes = NULL;

/*
 * Initialization file includes.
 */
static IncludeFileRec *std_includes[] =
{
    &incStandardLib,
    &incIntrinsic,
    &incDtbUtils,
    NULL,
};

int
abmfP_ui_header_file_init(void)
{
#define add_inc(in_entry, in_class_name, in_include) \
    (in_entry)->class_name= istr_dup(in_class_name); \
    (in_entry)->include= (in_include); \
    ++(in_entry);

    int                 class_includes_size = 22;
    IncludeForClass     include = NULL;

    /*
     * class_includes
     */
    class_includes_size = 35;
    class_includes = (IncludeForClass) abmfP_malloc(
                          class_includes_size * sizeof(IncludeForClassRec));
    include = class_includes;

    /* Make sure ISTRINGS exist for these, so string lists of */
    /* header file names don't actually allocate any memory */
    istr_const(incStandardLib.include_file);
    istr_const(incArrowButton.include_file);
    istr_const(incBulletinBoard.include_file);
    istr_const(incCascadeButton.include_file);
    istr_const(incComboBox.include_file);
    istr_const(incDialogShell.include_file);
    istr_const(incDrawingArea.include_file);
    istr_const(incDrawnButton.include_file);
    istr_const(incodeFileChooser.include_file);
    istr_const(incForm.include_file);
    istr_const(incFrame.include_file);
    istr_const(incIntrinsic.include_file);
    istr_const(incLabel.include_file);
    istr_const(incList.include_file);
    istr_const(incMainWindow.include_file);
    istr_const(incMenuShell.include_file);
    istr_const(incMenuButton.include_file);
    istr_const(incMwmUtil.include_file);
    istr_const(incPanedWindow.include_file);
    istr_const(incPushButton.include_file);
    istr_const(incRowColumn.include_file);
    istr_const(incScale.include_file);
    istr_const(incScrolledWindow.include_file);
    istr_const(incSeparator.include_file);
    istr_const(incShell.include_file);
    istr_const(incSpinBox.include_file);
    istr_const(incStringDefs.include_file);
    istr_const(incTerm.include_file);
    istr_const(incTermPrim.include_file);
    istr_const(incText.include_file);
    istr_const(incTextField.include_file);
    istr_const(incToggleButton.include_file);
    istr_const(incXm.include_file);

    /* build the class<->include_file relationships */
    add_inc(include, abmfP_topLevelShell, &incShell);
    add_inc(include, abmfP_applicationShell, &incShell);
    add_inc(include, abmfP_xmArrowButton, &incArrowButton);
    add_inc(include, abmfP_xmBulletinBoard, &incBulletinBoard);
    add_inc(include, abmfP_xmCascadeButton, &incCascadeButton);
    add_inc(include, abmfP_xmComboBox, &incComboBox);
    add_inc(include, abmfP_xmDialogShell, &incDialogShell);
    add_inc(include, abmfP_xmDrawingArea, &incDrawingArea);
    add_inc(include, abmfP_xmDrawnButton, &incDrawnButton);
    add_inc(include, abmfP_xmFileSelectionBox, &incodeFileChooser);
    add_inc(include, abmfP_xmForm, &incForm);
    add_inc(include, abmfP_xmFrame, &incFrame);
    add_inc(include, abmfP_xmLabel, &incLabel);
    add_inc(include, abmfP_xmList, &incList);
    add_inc(include, abmfP_xmMainWindow, &incMainWindow);
    add_inc(include, abmfP_xmMenuShell, &incMenuShell);
    add_inc(include, abmfP_xmMenuShell, &incRowColumn);
    add_inc(include, abmfP_xmMenuButton, &incMenuButton);
    add_inc(include, abmfP_xmPanedWindow, &incPanedWindow);
    add_inc(include, abmfP_xmPushButton, &incPushButton);
    add_inc(include, abmfP_xmRowColumn, &incRowColumn);
    add_inc(include, abmfP_xmScale, &incScale);
    add_inc(include, abmfP_xmScrolledWindow, &incScrolledWindow);
    add_inc(include, abmfP_xmScrolledWindow, &incList);
    add_inc(include, abmfP_xmSeparator, &incSeparator);
    add_inc(include, abmfP_xmSpinBox, &incSpinBox);
    add_inc(include, abmfP_xmTerm, &incTerm);
    add_inc(include, abmfP_xmTermPrim, &incTermPrim);
    add_inc(include, abmfP_xmText, &incText);
    add_inc(include, abmfP_xmTextField, &incTextField);
    add_inc(include, abmfP_xmToggleButton, &incToggleButton);
    add_inc(include, NULL, NULL);
    assert((include - class_includes) <= class_includes_size);

    return 0;
#undef add_inc
}


static int write_struct_def(GenCodeInfo genCodeInfo, ABObj structObj);
static int write_substruct_def(GenCodeInfo genCodeInfo, ABObj structObj);

int
abmfP_comp_get_widget_specific_includes(StringList includesList, ABObj obj)
{
    int			return_value = 0;
    int			rc = 0;		/* return code */
    AB_TRAVERSAL	trav;
    ABObj		subobj = NULL;

    for (trav_open(&trav, obj, AB_TRAV_COMP_SUBOBJS);
	(subobj = trav_next(&trav)) != NULL; )
    {
	rc = abmfP_obj_get_widget_specific_includes(includesList, subobj);
	return_if_err(rc,rc);
    }
    trav_close(&trav);

epilogue:
    return return_value;
}


/*
 * Puts all the necessary header files for the object in the string
 * list. Normally, the string list should be made to be unique 
 * (via strlist_set_is_unique() before calling this function, in order
 * to avoid duplicates in the list.
 */
int
abmfP_obj_get_widget_specific_includes(StringList includesList, ABObj obj)
{
    int                 	return_value = 0;
    int				num_includes = 0;
    BOOL                	objHasScrollbar = FALSE;
    IncludeForClass	include = class_includes;

    /*
     * Get optional widget parts
     */
    if (obj_has_scrollbar(obj))
    {
        strlist_add_str(includesList, incScrollbar.include_file, NULL);
    }

    if (obj_is_base_win(obj) && !obj_get_resizable(obj))
    {
	strlist_add_str(includesList, incMwmUtil.include_file, NULL);
    }

    /*
     * Save all the includes for this class
     */
    if (obj_get_class_name(obj) != NULL)
    {
        for (include = class_includes; include->class_name != NULL;
             ((include->class_name != NULL) ? ++include : include))
        {
            if (util_streq(obj_get_class_name(obj), 
                        istr_string(include->class_name)))
            {
                /* this include file is needed */
                strlist_add_str(includesList, 
				include->include->include_file, NULL);
            }
        } /* for include */
    } /* class_name != NULL */

    if (return_value >= 0)
    {
	return_value = num_includes;
    }
    return return_value;
}


/*
 * Traverse object list and write out needed widget specific includes.
 * REMIND: move SOMEwhere..
 */
int
abmfP_tree_write_widget_specific_includes(File codeFile, ABObj module)
{
    int			return_value = 0;
    int			num_includes = 0;
    AB_TRAVERSAL	trav;
    ABObj		obj = NULL;
    StringListRec	includesRec;
    StringList		includes = &includesRec;
    strlist_construct(&includesRec);

    strlist_set_is_unique(includes, TRUE);

    /*
     * Get standard includes
     */
    strlist_add_str(includes, incStandardLib.include_file, NULL);
    strlist_add_str(includes, incIntrinsic.include_file, NULL);

    /*
     * Get the list of includes we need
     */
    for (trav_open(&trav, module, AB_TRAV_UI);
         (obj = trav_next(&trav)) != NULL;)
    {
	abmfP_obj_get_widget_specific_includes(includes, obj);
    }
    trav_close(&trav);

    /*
     * Write 'em out!
     */

    num_includes = strlist_get_num_strs(includes);
    if (num_includes > 0)
    {
        int     i = 0;
        STRING  include = NULL;
        abio_puts(codeFile, "\n");
        for (i= 0; i < num_includes; ++i)
        {
            include = strlist_get_str(includes, i, NULL);
            abio_printf(codeFile, "#include %s\n", include);
        }
    }

    strlist_destruct(&includesRec);
    if (return_value >= 0)
    {
	return_value = num_includes;
    }
    return return_value;
}


/*
 * writes one structure definition
 */
static int
write_struct_def(GenCodeInfo genCodeInfo, ABObj structObj)
{
    File                codeFile= genCodeInfo->code_file;
    AB_TRAVERSAL        salientTrav;
    AB_TRAVERSAL        uiTrav;
    ABObj               item = NULL;
    ABObj               fieldObj= NULL;
    ABObj               salientObj = NULL;
    STRING              typeName = NULL;
    STRING              fieldName = NULL;
    int                 numFields = 0;
    int                 thisFieldNum = 0;
    BOOL                objHasItems = FALSE;
    BOOL                lastNumFields = 0;
    BOOL                separateObj = TRUE;
    BOOL                lastSeparateObj = FALSE;
    char                commentBuf[1024] = "";
    int                 fieldsWritten = 0;

    /* Don't want the message structure written out to the
     * _ui.h file.  It belongs in the dtb_utils.h file.
     */
    if (obj_is_message(structObj))
        return 0;

    /*
     * Write out all the substructure data types
     */
    for (trav_open(&uiTrav, structObj, AB_TRAV_UI|AB_TRAV_MOD_PARENTS_FIRST);
        (fieldObj = trav_next(&uiTrav)) != NULL; )
    {
        /* ref objs do not have their own type (they take the menu's type */
        if (    abmfP_obj_is_substruct_obj(fieldObj)
            && (!obj_is_ref(fieldObj)) )
        {
            write_substruct_def(genCodeInfo, fieldObj);
        }
    }
    trav_close(&uiTrav);

    abio_printf(codeFile, "typedef struct\n{\n");
    abio_indent(codeFile);
    abio_printf(codeFile, "Boolean\tinitialized;\n");

    for (trav_open(&salientTrav, 
                structObj, AB_TRAV_UI|AB_TRAV_MOD_PARENTS_FIRST);
        (salientObj= trav_next(&salientTrav)) != NULL;
        lastNumFields = numFields, lastSeparateObj = separateObj)
    {
        if (! (obj_is_salient(salientObj) || obj_is_menu_ref(salientObj)) )
        {
            continue;
        }
        item = obj_get_item(salientObj, 0);
        objHasItems = ((item != NULL) && 
                        !obj_is_list_item(item) &&
                        !obj_is_combo_box_item(item) &&
                        !obj_is_spin_box_item(item));
        numFields = obj_get_num_comp_subobjs(salientObj) + (objHasItems? 1:0);
        thisFieldNum = -1;
        separateObj = (numFields > 1);

        if (separateObj || lastSeparateObj)
        {
            abio_puts(codeFile, "\n");
        }

        /*
         * Handle composite subobjects
         */
        for (trav_open(&uiTrav, salientObj, 
                AB_TRAV_COMP_SUBOBJS|AB_TRAV_MOD_PARENTS_FIRST);
            (fieldObj = trav_next(&uiTrav)) != NULL; ++thisFieldNum)
        {
            typeName = NULL;
            fieldName = NULL;
            if (    abmfP_obj_has_struct_field(fieldObj)
                     && (!obj_was_written(fieldObj)) )
            {
                typeName = abmfP_str_widget;
                fieldName = abmfP_get_c_field_name(fieldObj);
            }
    
            if ((typeName != NULL) && (fieldName != NULL))
            {
                ++thisFieldNum;
                ++fieldsWritten;
                obj_set_was_written(fieldObj, TRUE);
                if (obj_is_menu_ref(fieldObj))
                {
                   /* be sure we don't write the items, since they're */
                   /* in the substructure */
                   abmfP_obj_set_items_written(fieldObj, TRUE);
                }
                abio_printf(codeFile, "%s\t%s;",
                    typeName, fieldName);
                if (separateObj && (thisFieldNum == 0))
                {
                    STRING      userObjName = NULL;
                    if (obj_is_ref(salientObj))
                    {
                        userObjName = 
                            obj_get_name(obj_get_actual_obj(salientObj));
                    }
                    else
                    {
                        userObjName = obj_get_name(salientObj);
                    }
                    abio_puts(codeFile, "\t");
                    sprintf(commentBuf, "object \"%s\"",
                        util_strsafe(userObjName));
                    abmfP_write_c_comment(genCodeInfo, TRUE, commentBuf);
                }
                else
                {
                    abio_puts(codeFile, nlstr);
                }
            }
        }
        trav_close(&uiTrav);

        /*
         * Handle items for this object
         */
        if (objHasItems)
        {
            char        msg[256];
            ++fieldsWritten;
            abio_printf(codeFile, "%s\t%s;\n", 
                abmfP_get_c_substruct_type_name(salientObj),
                abmfP_get_c_substruct_field_name(salientObj));
            abmfP_obj_set_items_written(salientObj, TRUE);
        }
    }
    trav_close(&salientTrav);

    abio_outdent(codeFile);
    abio_printf(codeFile, "} %s, *%s;\n",
        abmfP_get_c_struct_type_name(structObj),
        abmfP_get_c_struct_ptr_type_name(structObj));
    return 0;
}


static int
write_substruct_def(GenCodeInfo genCodeInfo, ABObj structObj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj       fieldObj = NULL;
    AB_TRAVERSAL        trav;

    abio_printf(codeFile, "typedef struct {\n");
    abio_indent(codeFile);

    for (trav_open(&trav, structObj, AB_TRAV_ITEMS_FOR_OBJ);
        (fieldObj = trav_next(&trav)) != NULL; )
    {
        if (abmfP_obj_has_substruct_field(fieldObj))
        {
            obj_set_was_written(fieldObj, TRUE);
            abio_printf(codeFile, "%s\t%s;\n",
                abmfP_str_widget,
                abmfP_get_c_field_name(fieldObj));
        }
    }
    trav_close(&trav);

    abio_outdent(codeFile);
    abio_printf(codeFile, "} %s, *%s;\n",
        abmfP_get_c_substruct_type_name(structObj),
        abmfP_get_c_substruct_ptr_type_name(structObj));
    return 0;
}


/*
 * write the definitions of the structures that hold the widgets
 */
static int
write_all_struct_defs(GenCodeInfo genCodeInfo, ABObj module)
{
    AB_TRAVERSAL        trav;
    ABObj               structObj= NULL;
    ABObj               menuObj;
    int                 numWritten = 0;

    /*
     * Menu creation procs are shared by all menu references
     * in the module, so we write their substructure definitions
     * first.
     *
     * We then mark all the menu refs and their items as written,
     * so they do not get put into each data structure.
     */
    abmfP_write_c_comment(genCodeInfo, FALSE, "Shared data structures");
    for (trav_open(&trav, module, AB_TRAV_MENUS);
        (menuObj = trav_next(&trav)) != NULL; )
    {
        /* refs use the type that they reference */
        if ((!obj_is_ref(menuObj)) && (abmfP_obj_is_substruct_obj(menuObj)))
        {
            ++numWritten;
            write_substruct_def(genCodeInfo, menuObj);
        }
    }
    trav_close(&trav);

    if (numWritten > 0)
    {
        abio_puts(genCodeInfo->code_file, nlstr);
    }

    /*
     * Write all structures
     */
    for (trav_open(&trav, module, AB_TRAV_UI | AB_TRAV_MOD_PARENTS_FIRST);
        (structObj= trav_next(&trav)) != NULL; )
    {
        if (    abmfP_obj_is_struct_obj(structObj))
        {
            abio_printf(genCodeInfo->code_file, nlstr);
            write_struct_def(genCodeInfo, structObj);
        }
    }
    trav_close(&trav);

    return 0;
}


/*
 * Write out the decl of one shared variable
 */
static int
write_shared_var_decl(GenCodeInfo genCodeInfo, ABObj structObj)
{
    File        codeFile = genCodeInfo->code_file;

    if (obj_is_message(structObj))
    {
        abio_printf(codeFile, "extern %s", "DtbMessageDataRec");
    }
    else
    { 
        abio_printf(codeFile, "extern %s",
                abmfP_get_c_struct_type_name(structObj));
    }
    abio_printf(codeFile, " %s;\n",
                abmfP_get_c_struct_global_name(structObj));
    return 0;
}


/*
 * Traverse object list and write out user struct objects
 * decls.
 */
static int
write_all_shared_var_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    File                codeFile= genCodeInfo->code_file;
    AB_TRAVERSAL        trav;
    ABObj               structObj= NULL;;

    abio_printf(codeFile, "\n\n");
    for (trav_open(&trav, module, AB_TRAV_ALL);
        (structObj= trav_next(&trav)) != NULL; )
    {
        if (!abmfP_obj_is_struct_obj(structObj))
        {
            continue;
        }
        write_shared_var_decl(genCodeInfo, structObj);
    }
    trav_close(&trav);

    return 0;
}


/*
 * Traverse object list and write out callback decls, for those
 * connections with the proper value of auto_named.
 */
static int
write_action_decls(
                        GenCodeInfo genCodeInfo, 
                        ABObj module, 
                        BOOL auto_named)
{
    AB_TRAVERSAL        trav;
    ABObj               action = NULL;

    for (trav_open(&trav, module, AB_TRAV_ACTIONS_FOR_OBJ);
         (action = trav_next(&trav)) != NULL;)
    {
        if (   mfobj_has_flags(action, CGenFlagIsDuplicateDef)
            || mfobj_has_flags(action, CGenFlagWriteDefToProjFile))
        {
            continue;
        }

        if (!util_xor(action->info.action.auto_named, auto_named))
	{
            abmfP_write_action_func_decl(genCodeInfo, action);
        }
    }
    trav_close(&trav);
    return 0;
}


/*
 * Traverse object list and write out connection decls. (Functions
 * that have been automatically generated and named)
 */
static int
write_connection_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    return write_action_decls(genCodeInfo, module, TRUE);
}


/*
 * Traverse object list and write out callback decls. (Functions that
 * have been named by the user)
 */
static int
write_callback_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    return write_action_decls(genCodeInfo, module, FALSE);
}


/*
 * Write out decls of structure clear procs
 */
static int
write_clear_proc_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    for (trav_open(&trav, module, AB_TRAV_ALL);
         (obj = trav_next(&trav)) != NULL;)
    {
        if (!abmfP_obj_has_clear_proc(obj)) 
        {
            continue;
        }
        if (obj_is_window(obj))
        {
            abmfP_write_clear_proc_decl(genCodeInfo, obj);
        }
    }
    trav_close(&trav);

    return 0;
}


/*
 * Write out decls structure initialization procs
 */
static int
write_init_proc_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    for (trav_open(&trav, module, AB_TRAV_SALIENT_UI);
         (obj = trav_next(&trav)) != NULL;)
    {
        if (!abmfP_obj_has_init_proc(obj))
        {
            continue;
        }
        abmfP_write_init_proc_decl(genCodeInfo, obj);
    }
    trav_close(&trav);
    return 0;
}


/*
 * Returns a copy of the input string with dots replaced with underscores.
 */
static              STRING
remove_dots(STRING s)
{
    static char         name[MAXPATHLEN];
    STRING              n = (STRING) name;

    for (; *s; ++s, ++n)
        if (*s == '.')
            *n = '_';
        else
            *n = *s;

    *n = '\0';

    return name;
}

/*
 * Open the UI header file, write some comments, write the includes, write
 * the decls, and close the header file.
 */
int
abmfP_write_ui_header_file(
                           GenCodeInfo genCodeInfo,
                           ABObj module,
                           STRING headerName,
                           STRING headerDefine
)
{
    File                codeFile = genCodeInfo->code_file;
    char                moduleName[1024];
    STRING              errmsg = NULL;
    int                 i = 0;
    *moduleName = 0;

    sprintf(moduleName, "module %s", util_strsafe(obj_get_name(module)));
    abmfP_write_file_header(
                genCodeInfo, 
                headerName,
                TRUE,
                moduleName,
                util_get_program_name(), 
                ABMF_MODIFY_NOT,
" * Contains: Declarations of module objects, user create procedures,\n"
" *           and callbacks."
                );

    /*
     * Write standard includes.
     */
    for (i = 0; std_includes[i] != NULL; ++i)
    {
        abio_printf(codeFile,
                    "#include %s\n", std_includes[i]->include_file);
        /* REMIND : only write out necessary include files! */
        /* std_includes[i]->written = TRUE; */
    }

    /*
     * Write UI object decls.
     */
    abmfP_tree_set_written(module, FALSE);
    abio_puts(codeFile, "\n");
    abio_puts(codeFile, abmfP_comment_begin);
    abio_puts(codeFile, abmfP_comment_continue);
    abio_printf(codeFile, "User Interface Objects\n");
    abio_puts(codeFile, abmfP_comment_end);
    write_all_struct_defs(genCodeInfo, module);
    write_all_shared_var_decls(genCodeInfo, module);

    /*
     * Write structure clear proc decls
     */
    abio_puts(codeFile, "\n");
    abio_puts(codeFile, abmfP_comment_begin);
    abio_puts(codeFile, abmfP_comment_continue);
    abio_printf(codeFile, 
        "Structure Clear Procedures: These set the fields to NULL\n");
    abio_puts(codeFile, abmfP_comment_end);
    write_clear_proc_decls(genCodeInfo, module);

    /*
     * Write structure init proc decls.
     */
    abio_puts(codeFile, "\n");
    abio_puts(codeFile, abmfP_comment_begin);
    abio_puts(codeFile, abmfP_comment_continue);
    abio_printf(codeFile, 
        "Structure Initialization Procedures: These create the widgets\n");
    abio_puts(codeFile, abmfP_comment_end);
    write_init_proc_decls(genCodeInfo, module);

    /*
     * Write Callback decls.
     */
    abio_puts(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE, "User Callbacks");
    write_callback_decls(genCodeInfo, module);

    /*
     * Write Connection decls.
     */
    abio_puts(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE, "Connections");
    write_connection_decls(genCodeInfo, module);

    /*
     * Write file footer.
     */
    abmfP_write_file_footer(genCodeInfo, headerName, TRUE);

    return OK;
}


int
abmfP_write_action_func_decl(GenCodeInfo genCodeInfo, ABObj action)
{
    STRING		funcName = abmfP_get_action_name(action);
    ABObj		fromObj = NULL;
    AB_TRAVERSAL        refTrav;
    ABObj               refObj = NULL;
    ABObj               actualFromObj = NULL;
    ABObj		module = NULL;

    fromObj = obj_get_from(action);
    module = obj_get_module(fromObj);

    switch (obj_get_when(action))
    {
    case AB_WHEN_DRAGGED_FROM:
        abio_printf(genCodeInfo->code_file, 
                abmfP_lib_default_dragCB->proto, /* fmt string */
                    funcName);
        abio_puts(genCodeInfo->code_file, nlstr);
    break;

    case AB_WHEN_DROPPED_ON:
        abio_printf(genCodeInfo->code_file, 
                abmfP_lib_default_dropCB->proto, /* fmt string */
                    funcName);
        abio_puts(genCodeInfo->code_file, nlstr);
    break;

    case AB_WHEN_TOOLTALK_QUIT:
    case AB_WHEN_TOOLTALK_DO_COMMAND:
    case AB_WHEN_TOOLTALK_GET_STATUS:
    case AB_WHEN_TOOLTALK_PAUSE_RESUME:
        abmfP_write_tooltalk_callback_decl(genCodeInfo, FALSE, funcName);
    break;

    case AB_WHEN_SESSION_SAVE:
        abmfP_write_session_save_callback_decl(genCodeInfo, 
						FALSE, funcName);
    break;
    case AB_WHEN_SESSION_RESTORE:
        abmfP_write_session_restore_callback_decl(genCodeInfo, 
						FALSE, funcName);
    break;

    default:
        abmfP_write_xm_callback_decl(genCodeInfo, FALSE, funcName);
    break;
    } /* switch obj_get_when() */

    return 0;
}

