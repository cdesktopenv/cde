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
 *	$XConsortium: proj.h /main/3 1995/11/06 17:45:59 rswiston $
 *
 * @(#)proj.h	1.11 16 Feb 1994	cde_app_builder/src/
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
 * proj.h
 * Header file for the project window
 */

#ifndef _PROJ_H_
#define _PROJ_H_

#include <Xm/Xm.h>
#include <ab_private/ui_util.h>
#include "dtb_utils.h"

#define	IMPORT_BY_REF	0
#define IMPORT_BY_COPY	1
#define	IMPORT_AS_BIL	2
#define	IMPORT_AS_UIL	3

typedef enum { 
    AB_OPEN_PROJ_CHOOSER = 0, 
    AB_SAVE_PROJ_AS_CHOOSER,
    AB_IMPORT_CHOOSER, 
    AB_EXPORT_CHOOSER,
    AB_SAVE_MOD_AS_CHOOSER,
    AB_CHOOSER_TYPE_NUM_VALUES
} AB_CHOOSER_TYPE; 

typedef enum
{
    PROJ_STATUS_MOD_PATH,
    PROJ_STATUS_PROJ_PATH, /* number of valid values - MUST BE LAST */
    PROJ_STATUS_NUM_VALUES
        /* ANSI: no comma after last enum item! */
} PROJ_STATUS_INFO;

typedef struct {
    Widget		child;
    AB_CHOOSER_TYPE	chooser_type;
    BOOL		ImportAsBil;
    BOOL		ImportByCopy;
    BOOL		writeEncapsulated;
} ChooserInfoRec, *ChooserInfo;
    

/*
 * This is only used for projects right now, but it can be used for other
 * types of saves, as well.
 */
#define AB_SAVE_CANCELLED	(-1)	/* errors are negative */
#define AB_SAVE_NORMAL		1
#define AB_SAVE_SAVE_AS		2

typedef int ProjSaveCallbackFuncRec(int status);
typedef ProjSaveCallbackFuncRec *ProjSaveCallbackFunc;

int	proj_init(void);

void	proj_add_objects(
	    AB_OBJ	*obj
	);

void	proj_delete_objects(
	    AB_OBJ	*obj
	);

void	proj_register_actions(
	    XtAppContext app
	);

void	proj_select(
	    AB_OBJ	*module
	);

void	proj_deselect(
	    AB_OBJ	*module
	);

void	proj_toggle_select(
	    AB_OBJ	*module
	);

extern int	proj_set_project(
		    ABObj	project
		);

extern int 	proj_destroy_project(
		    ABObj       proj
		);

extern ABObj	proj_get_project(
		);

extern int	proj_set_cur_module(
		    ABObj	module
		);

extern ABObj	proj_get_cur_module(
		);

extern int	proj_rename_module(
		    ABObj    module,
		    STRING   name
		);

extern void	proj_update_node(
		    ABObj	obj
		);

extern int	proj_update_stat_region(
		    PROJ_STATUS_INFO	type,
		    STRING		value
		);

extern BOOL 	proj_check_unsaved_edits(
		    ABObj       root 
		);

extern void 	proj_create_new_proj(
		    STRING      proj_name
		);

extern void 	proj_name_proj( void ); 

extern int	proj_set_files_exploded(ABObj project);

extern int	proj_unset_files_exploded(ABObj project);

extern void	proj_show_save_as_proj_chooser(
		    Widget	widget
		);

extern void 	proj_show_name_dlg(
		    ABObj       module,
		    Widget      parent
		);

extern void 	proj_show_export_bil_chooser(
		    Widget      widget,
		    ABObj       obj
		);

extern BOOL	proj_is_exploded(ABObj project);

extern int	proj_overwrite_existing_file(
	    	    char    		*file,
	    	    ABObj   		obj,
		    BOOL    		encapsulating,
		    BOOL    		Exporting,
		    BOOL    		changed_dir,
		    STRING		old_proj_dir,
		    DTB_MODAL_ANSWER	*answer
		);

extern void	proj_save_needed(
			ProjSaveCallbackFunc saveDoneCB	/* may be NULL */
		);

extern int	proj_save_encapsulated(
		    ABObj	obj,
		    STRING 	file_name
		);

extern int 	proj_save_exploded(	/* one file per module */
		    ABObj       obj,
		    char        *exp_path,
		    BOOL        changed_dir,
		    BOOL	encap_file
		);

extern int 	proj_show_proj_dir(void);


extern void	proj_open_proj_okCB(
		    Widget      		widget,
                    XtPointer   		client_data,
                    XmSelectionBoxCallbackStruct *call_data
		);

extern void	proj_cancelCB(
		    Widget 			widget,
                    XtPointer           	client_data,
                    XmSelectionBoxCallbackStruct *call_data
		);


extern void     proj_unmap_chooserCB(
                    Widget                      widget,
                    XtPointer                   client_data,
                    XtPointer 			call_data
                );

extern void	proj_show_save_proj_as_chooser(
		    Widget              	widget,
		    ProjSaveCallbackFunc	saveDoneCB
		);

extern void 	proj_show_dialog(void);

extern void 	proj_import_okCB(
		    Widget              	widget,
		    XtPointer                	client_data,
		    XmSelectionBoxCallbackStruct *call_data
		);

extern STRING 	proj_cvt_mod_file_to_rel_path(
		    STRING      mod_path,
		    STRING      proj_path
		);

extern void 	proj_set_menus(
		    AB_CHOOSER_TYPE     chooser_type,
		    BOOL                active
		);

extern Boolean 	proj_verify_name(
		    STRING              new_name,
		    BOOL                ObjIsModule,
		    DtbMessageData      msgData,
		    Widget              parent,
		    ABObj               newObj
		);

#endif /* _PROJ_H_ */
