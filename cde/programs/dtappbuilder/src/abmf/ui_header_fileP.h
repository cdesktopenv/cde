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
 *	$XConsortium: ui_header_fileP.h /main/3 1995/11/06 18:15:59 rswiston $
 *
 * @(#)ui_header_fileP.h	3.14 14 Jan 1995	cde_app_builder/src/abmf
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

#ifndef _ABMF_UI_HEADER_FILEP_H_ 
#define _ABMF_UI_HEADER_FILEP_H_ 

#include "write_codeP.h"

int	abmfP_ui_header_file_init(void);	/* call this first !! */

int	abmfP_write_ui_header_file(
		GenCodeInfo	genCodeInfo,
		ABObj		module, 
		STRING		headerName,
		STRING		headerDefine
		);

int	abmfP_write_action_func_decl(GenCodeInfo genCodeInfo, ABObj action);

int 	abmfP_comp_get_widget_specific_includes(
			StringList	includes, 
			ABObj 		compObj
	);
int 	abmfP_obj_get_widget_specific_includes(
			StringList	includes, 
			ABObj		obj
	);
int 	abmfP_tree_write_widget_specific_includes(
			File	codeFile, 
			ABObj	tree
	);

#endif /* _ABMF_UI_HEADER_FILEP_H_ */
