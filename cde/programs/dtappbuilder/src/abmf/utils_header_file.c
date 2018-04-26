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
 *      $XConsortium: utils_header_file.c /main/3 1995/11/06 18:17:19 rswiston $
 *
 *      @(#)utils_header_file.c	1.24 30 Apr 1995 cde_app_builder/src/abmf
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
 *  utils_header_file.c - writes dtb_utils.h
 */

#include <ab_private/abio.h>
#include "lib_func_stringsP.h"
#include "write_cP.h"
#include "utils_header_fileP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)                                  **
**                                                                      i*
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)                  **
**                                                                      **
**************************************************************************/
static int      write_session_typedefs(
                        GenCodeInfo     genCodeInfo, 
                        ABObj   project
                );
static int write_tooltalk_typedefs(GenCodeInfo genCodeInfo, ABObj project);
static int write_help_typedefs(GenCodeInfo genCodeInfo, ABObj project);
static int write_message_typedefs(GenCodeInfo genCodeInfo);
static int write_drag_types(GenCodeInfo genCodeInfo, ABObj project);
static int write_message_struct(GenCodeInfo genCodeInfo);
static int write_lib_func_decls(GenCodeInfo genCodeInfo, ABObj project);
static int write_lib_macros(GenCodeInfo genCodeInfo, ABObj project);
static int write_includes(GenCodeInfo genCodeInfo, ABObj project);
static int write_centering_types(GenCodeInfo genCodeInfo);
static int write_group_struct(GenCodeInfo genCodeInfo);
static int write_group_types(GenCodeInfo genCodeInfo);
static int write_align_types(GenCodeInfo genCodeInfo);


/*************************************************************************
**                                                                      **
**       Data                                                           **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

int 
abmfP_write_utils_header_file(
                        GenCodeInfo     genCodeInfo,
                        STRING          fileName,
                        ABObj           project
)
{
    char        name[1024];

    sprintf(name, "project %s", obj_get_name(project));

    abmfP_write_file_header(
                genCodeInfo, 
                fileName, 
                TRUE,
                name,
                util_get_program_name(), 
                ABMF_MODIFY_NOT,
                " * CDE Application Builder General Utility Functions"
                );

    write_includes(genCodeInfo, project);

    /*
     * data types
     */
    write_session_typedefs(genCodeInfo, project);
    write_tooltalk_typedefs(genCodeInfo, project);
    write_help_typedefs(genCodeInfo, project);
    write_message_typedefs(genCodeInfo);
    write_centering_types(genCodeInfo);
    write_drag_types(genCodeInfo, project);

    /*
     * Write out types used by group layout code
     */
    write_group_types(genCodeInfo);
    write_align_types(genCodeInfo);

    /* 
     * structs
     */
    if (abmfP_proj_has_message(project))
        write_message_struct(genCodeInfo);

    /*
     * Write out structure for group
     * information
     */
    write_group_struct(genCodeInfo);

    /*
     * library function declarations
     */
    abio_puts(genCodeInfo->code_file, nlstr);
    write_lib_func_decls(genCodeInfo, project);
    abio_puts(genCodeInfo->code_file, nlstr);
    write_lib_macros(genCodeInfo, project);

    abmfP_write_file_footer(
                genCodeInfo, 
                fileName, 
                TRUE);
    return 0;
}

static int
write_includes(GenCodeInfo genCodeInfo, ABObj project)
{
    abmfP_write_c_system_include(genCodeInfo, "stdlib.h");
    abmfP_write_c_system_include(genCodeInfo, "X11/Intrinsic.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/Xm.h");
    abmfP_write_c_system_include(genCodeInfo, "Dt/Dnd.h");

    if (obj_get_tooltalk_level(project) != AB_TOOLTALK_NONE)
        abmfP_write_c_system_include(genCodeInfo, "Tt/tttk.h");
 
    return 0;
}

static int
write_session_typedefs(
    GenCodeInfo genCodeInfo, 
    ABObj       project
)
{
    File        codeFile;

    if (!genCodeInfo || !project)
        return (0);

    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo, 
        FALSE, "Function type for client session save callback");
    abio_puts(codeFile,"typedef Boolean (*DtbClientSessionSaveCB) (\n");
    abio_puts(codeFile,"    Widget,\n");
    abio_puts(codeFile,"    char *,\n");
    abio_puts(codeFile,"    char ***,\n");
    abio_puts(codeFile,"    int *\n");
    abio_puts(codeFile,");\n");

    if (abmfP_proj_needs_session_restore(project))
    {
        abio_puts(codeFile,"\n");
        abmfP_write_c_comment(genCodeInfo, 
            FALSE, "Function type for client session restore callback");
        abio_puts(codeFile,"typedef Boolean (*DtbClientSessionRestoreCB) (\n");
        abio_puts(codeFile,"    Widget,\n");
        abio_puts(codeFile,"    char *\n");
        abio_puts(codeFile,");\n");
    }
    
    return (0);
}

static int
write_tooltalk_typedefs(
    GenCodeInfo genCodeInfo,
    ABObj       project
)
{
    File        codeFile;

    if (!genCodeInfo || !project ||
        obj_get_tooltalk_level(project) != AB_TOOLTALK_DESKTOP_ADVANCED)
        return (0);

    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Function type for ToolTalk Desktop Message callback");
    abio_puts(codeFile,"typedef Boolean (*DtbTTMsgHandlerCB) (\n");
    abio_puts(codeFile,"    Tt_message,\n");
    abio_puts(codeFile,"    void *\n");
    abio_puts(codeFile,");\n");

    return (0);
}

static int
write_help_typedefs(GenCodeInfo genCodeInfo, ABObj project)
{
    File        codeFile = genCodeInfo->code_file;
    project = project;          /* avoid warning */

    /* 
     * Write declaration for special help data callback structure */
    abio_puts(codeFile,"typedef struct {\n");
    abio_puts(codeFile,"    char\t*help_text;\n");
    abio_puts(codeFile,"    char\t*help_volume;\n");
    abio_puts(codeFile,"    char\t*help_locationID;\n");
    abio_puts(codeFile,"} DtbObjectHelpDataRec, *DtbObjectHelpData;\n");
    return 0;
}


/*
 * This actually writes out the drop types, as well.
 */
static int
write_drag_types(GenCodeInfo genCodeInfo, ABObj project)
{
    project = project;  /* avoid warning */

    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Types for Drag and Drop utilities");
    abio_puts(genCodeInfo->code_file,
        "typedef enum\n"
        "{\n"
        "    DTB_DND_UNDEF = 0,\n"
        "    DTB_DND_ANIMATE,\n"
        "    DTB_DND_CONVERT,\n"
        "    DTB_DND_DELETE,\n"
        "    DTB_DND_DRAG_START,\n"
        "    DTB_DND_DROPPED_ON,\n"
        "    DTB_DND_DROPPED_ON_ROOT_WINDOW,\n"
        "    DTB_DND_FINISH,\n"
        "    DTB_DND_RECEIVE_DATA,\n"
        "    DTB_DND_REGISTER\n"
        "} DTB_DND_REQUEST;\n"
        "\n"
        "typedef struct\n"
        "{\n"
        "    DtDndProtocol      protocol;\n"
        "    unsigned char      operations;     /* XmCopy | XmMove | XmLink */\n"
        "    Boolean            bufferIsText;\n"
        "    Boolean            allowDropOnRootWindow;\n"
        "    int                numItems;\n"
        "    Widget             cursor;         /* from DtDndCreateSourceIcon() */\n"
        "} DtbDndDragStartInfoRec, *DtbDndDragStartInfo;\n"
        "\n"
        "typedef struct\n"
        "{\n"
        "    Boolean    droppedOnRootWindow;\n"
        "} DtbDndDroppedOnRootWindowInfoRec, *DtbDndDroppedOnRootWindowInfo;\n"
        "\n"
        "typedef void   (*DtbDndDragCallback)(\n"
        "                       DTB_DND_REQUEST                 request,\n"
        "                       DtbDndDragStartInfo             dragStart,\n"
        "                       DtDndConvertCallback            convert,\n"
        "                       DtbDndDroppedOnRootWindowInfo   droppedOnRootWindow,\n"
        "                       DtDndConvertCallback            deleteSource,\n"
        "                       DtDndDragFinishCallback         finish\n"
        "               );\n"
        "\n"
        "typedef unsigned long  DtbDragSiteHandle;\n"
	"\n"
        "\n"
	"typedef struct {\n"
    	"DtDndProtocol      	protocols;\n"
    	"unsigned char      	operations;     /* XmCopy | XmMove | XmLink */\n"
    	"Boolean            	textIsBuffer;\n"
    	"Boolean            	preservePreviousRegistration;\n"
    	"Boolean		respondToDropsOnChildren;\n"
	"} DtbDndDropRegisterInfoRec, *DtbDndDropRegisterInfo;\n"
	"\n"
	"typedef void	(*DtbDndDropCallback)(\n"
	"			Widget				widget,\n"
	"			DTB_DND_REQUEST			request,\n"
	"			DtbDndDropRegisterInfo		registerInfo,\n"
	"			DtDndTransferCallback		receiveInfo,\n"
	"			DtDndDropAnimateCallback	animateInfo\n"
	"		);\n"
	"\n"
	"typedef unsigned long	DtbDropSiteHandle;\n"
	"\n"
        );

    return 0;
}


/*
 * In the future, this should only declare functions that get used
 */
static int
write_lib_func_decls(GenCodeInfo genCodeInfo, ABObj project)
{
    File        codeFile = genCodeInfo->code_file;
    project = project;  /* avoid warnings */

    abio_puts(codeFile, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE,
                "Application Builder utility functions");
    abio_printf(codeFile, "%s\n", abmfP_lib_cvt_file_to_pixmap->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_set_label_pixmaps->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_set_label_from_bitmap_data->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_file_has_extension->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_cvt_filebase_to_pixmap->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_cvt_image_file_to_pixmap->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_set_label_from_image_file->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_cvt_resource_from_string->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_create_greyed_pixmap->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_save_toplevel_widget->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_toplevel_widget->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_remove_sash_focus->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_save_command->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_command->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_exe_dir->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_help_dispatch->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_more_help_dispatch->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_help_back_hdlr->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_do_onitem_help->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_show_help_volume_info->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_call_help_callback->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_popup_menu_register->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_drag_site_register->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_drop_site_register->proto);

    /*
     * Write out centering routines
     */
    abio_printf(codeFile, "%s\n", abmfP_lib_children_center->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_children_uncenter->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_center->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_uncenter->proto);

    /*
     * Write out align routine
     */
    abio_printf(codeFile, "%s\n", abmfP_lib_children_align->proto);

    /*
     * The session save callback is always generated
     */
    abio_printf(codeFile, "%s\n", abmfP_lib_session_save->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_client_session_saveCB->proto);

    if (abmfP_proj_needs_session_save(project))
    {
        abio_printf(codeFile, "%s\n", abmfP_lib_set_client_session_saveCB->proto);
    }

    if (abmfP_proj_needs_session_restore(project))
    {
        abio_printf(codeFile, "%s\n", abmfP_lib_session_restore->proto);
        abio_printf(codeFile, "%s\n", abmfP_lib_set_client_session_restoreCB->proto);
        abio_printf(codeFile, "%s\n", abmfP_lib_get_client_session_restoreCB->proto);
    }

    /* 
     * Write out ToolTalk prototypes if needed
     */
    switch(obj_get_tooltalk_level(project)) 
    {
        case AB_TOOLTALK_DESKTOP_ADVANCED: 
            abio_printf(codeFile, "%s\n", abmfP_lib_set_tt_msg_quitCB->proto);
            abio_printf(codeFile, "%s\n", abmfP_lib_set_tt_msg_do_commandCB->proto);
            abio_printf(codeFile, "%s\n", abmfP_lib_set_tt_msg_get_statusCB->proto);
            abio_printf(codeFile, "%s\n", abmfP_lib_set_tt_msg_pause_resumeCB->proto);
            abio_printf(codeFile, "%s\n", abmfP_lib_tt_contractCB->proto);
            /* fall through */
        case AB_TOOLTALK_DESKTOP_BASIC:
            abio_printf(codeFile, "%s\n", abmfP_lib_tt_close->proto);
            break; 
        default: 
            break; 
    } 

    /* Write out message creation and posting routines if the
     * project contains message objects.
     */
    if (abmfP_proj_has_message(project))
    {
        abio_printf(codeFile, "%s\n", abmfP_lib_create_message_dlg->proto);
        abio_printf(codeFile, "%s\n", abmfP_lib_MessageBoxGetActionButton->proto);
        abio_printf(codeFile, "%s\n", abmfP_lib_show_message->proto);
        abio_printf(codeFile, "%s\n", abmfP_lib_show_modal_message->proto);
    }

    abio_printf(codeFile, "\n");

    return 0;
}

static int
write_lib_macros(GenCodeInfo genCodeInfo, ABObj project)
{
    File        codeFile = genCodeInfo->code_file;
    project = project;  /* avoid warning */

    abio_printf(codeFile, 
"#define dtb_cvt_string_to_pixel(parent, str) \\\n"
"        ((Pixel)dtb_cvt_resource_from_string( \\\n"
"                   (parent), XtRPixel, sizeof(Pixel), (str), 0))\n"
"\n");

    return 0;
}

static int
write_message_typedefs(
    GenCodeInfo genCodeInfo
)
{
    File        codeFile;
 
    if (!genCodeInfo)
        return (0);
 
    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Returns answer value for modal MessageBox");
    abio_puts(codeFile,"typedef enum {\n");
    abio_puts(codeFile,"    DTB_ANSWER_NONE,\n");
    abio_puts(codeFile,"    DTB_ANSWER_ACTION1,\n");
    abio_puts(codeFile,"    DTB_ANSWER_ACTION2,\n");
    abio_puts(codeFile,"    DTB_ANSWER_ACTION3,\n");
    abio_puts(codeFile,"    DTB_ANSWER_CANCEL,\n");
    abio_puts(codeFile,"    DTB_ANSWER_HELP\n");
    abio_puts(codeFile,"} DTB_MODAL_ANSWER;\n");
 
    abio_puts(codeFile,"\n");                    
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Values for MessageBox default button");
    abio_puts(codeFile,"typedef enum {\n");
    abio_puts(codeFile,"    DTB_ACTION1_BUTTON,\n");
    abio_puts(codeFile,"    DTB_ACTION2_BUTTON,\n");
    abio_puts(codeFile,"    DTB_ACTION3_BUTTON,\n");
    abio_puts(codeFile,"    DTB_CANCEL_BUTTON,\n");
    abio_puts(codeFile,"    DTB_NONE\n");
    abio_puts(codeFile,"} DTB_BUTTON;\n");
 
    return (0);
}


static int
write_centering_types(
    GenCodeInfo genCodeInfo
)
{
    File        codeFile;
 
    if (!genCodeInfo)
        return (0);
 
    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Types/ways of centering an object");
    abio_puts(codeFile,"typedef enum {\n");
    abio_puts(codeFile,"    DTB_CENTER_NONE,\n");
    abio_puts(codeFile,"    DTB_CENTER_POSITION_VERT,\n");
    abio_puts(codeFile,"    DTB_CENTER_POSITION_HORIZ,\n");
    abio_puts(codeFile,"    DTB_CENTER_POSITION_BOTH\n");
    abio_puts(codeFile,"} DTB_CENTERING_TYPES;\n");
    abio_puts(codeFile,"\n\n");
 
    return (0);
}

static int
write_group_types(
    GenCodeInfo genCodeInfo
)
{
    File        codeFile;
 
    if (!genCodeInfo)
        return (0);
 
    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Types of group layout");
    abio_puts(codeFile,"typedef enum {\n");
    abio_puts(codeFile,"    DTB_GROUP_NONE,\n");
    abio_puts(codeFile,"    DTB_GROUP_ROWS,\n");
    abio_puts(codeFile,"    DTB_GROUP_COLUMNS,\n");
    abio_puts(codeFile,"    DTB_GROUP_ROWSCOLUMNS\n");
    abio_puts(codeFile,"} DTB_GROUP_TYPES;\n");
 
    return (0);
}

static int
write_align_types(
    GenCodeInfo genCodeInfo
)
{
    File        codeFile;
 
    if (!genCodeInfo)
        return (0);
 
    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Types/ways of aligning the children of a group");
    abio_puts(codeFile,"typedef enum {\n");
    abio_puts(codeFile,"    DTB_ALIGN_NONE,\n");
    abio_puts(codeFile,"    DTB_ALIGN_TOP,\n");
    abio_puts(codeFile,"    DTB_ALIGN_RIGHT,\n");
    abio_puts(codeFile,"    DTB_ALIGN_BOTTOM,\n");
    abio_puts(codeFile,"    DTB_ALIGN_LEFT,\n");
    abio_puts(codeFile,"    DTB_ALIGN_VCENTER,\n");
    abio_puts(codeFile,"    DTB_ALIGN_HCENTER,\n");
    abio_puts(codeFile,"    DTB_ALIGN_LABELS\n");
    abio_puts(codeFile,"} DTB_ALIGN_TYPES;\n");
 
    return (0);
}



static int
write_message_struct(
    GenCodeInfo genCodeInfo
)
{
    File        codeFile;
 
    if (!genCodeInfo)
        return (0);
 
    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Structure to store values for Messages");
    abio_puts(codeFile,"typedef struct {\n");
    abio_puts(codeFile,"    Boolean              initialized;\n");
    abio_puts(codeFile,"    unsigned char        type;\n");
    abio_puts(codeFile,"    XmString             title;\n");
    abio_puts(codeFile,"    XmString             message;\n");
    abio_puts(codeFile,"    XmString             action1_label;\n");
    abio_puts(codeFile,"    XtCallbackProc       action1_callback;\n");
    abio_puts(codeFile,"    XmString             action2_label;\n");
    abio_puts(codeFile,"    XtCallbackProc       action2_callback;\n");
    abio_puts(codeFile,"    XmString             action3_label;\n");
    abio_puts(codeFile,"    XtCallbackProc       action3_callback;\n");
    abio_puts(codeFile,"    Boolean              cancel_button;\n");
    abio_puts(codeFile,"    XtCallbackProc       cancel_callback;\n");
    abio_puts(codeFile,"    Boolean              help_button;\n");
    abio_puts(codeFile,"    DtbObjectHelpDataRec help_data;\n");
    abio_puts(codeFile,"    DTB_BUTTON           default_button;\n");
    abio_puts(codeFile,"} DtbMessageDataRec, *DtbMessageData;\n");
 
    abio_puts(codeFile,"\n");
 
    return (0);
}

static int
write_group_struct(
    GenCodeInfo genCodeInfo
)
{
    File        codeFile;
 
    if (!genCodeInfo)
        return (0);
 
    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo,
        FALSE, "Structure to store attributes of a group");
    abio_puts(codeFile,"typedef struct {\n");
    abio_puts(codeFile,"    DTB_GROUP_TYPES     group_type;\n");
    abio_puts(codeFile,"    DTB_ALIGN_TYPES     row_align;\n");
    abio_puts(codeFile,"    DTB_ALIGN_TYPES     col_align;\n");
    abio_puts(codeFile,"    int                 margin;\n");
    abio_puts(codeFile,"    int                 num_rows;\n");
    abio_puts(codeFile,"    int                 num_cols;\n");
    abio_puts(codeFile,"    int                 hoffset;\n");
    abio_puts(codeFile,"    int                 voffset;\n");
    abio_puts(codeFile,"    Widget              ref_widget;\n");
    abio_puts(codeFile,"} DtbGroupInfo;\n");
 
    abio_puts(codeFile,"\n");
 
    return (0);
}
