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
 *	$XConsortium: cdefs.c /main/3 1995/11/06 18:02:58 rswiston $
 *
 *	@(#)cdefs.c	1.11 17 Nov 1994	cde_app_builder/src/abmf
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
 *  cdefs.c - general C defines, for spitting out code
 */

#include "cdefsP.h"

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

STRING	abmfP_comment_begin= "/*\n";
STRING	abmfP_comment_continue= " * ";
STRING	abmfP_comment_end= " */\n";

STRING	abmfP_call_data_var_name = "callData";
STRING	abmfP_client_data_var_name = "clientData";
STRING	abmfP_instance_ptr_var_name= "instance";
STRING	abmfP_parent_param_name= "parent";
STRING	abmfP_toplevel_var_name= "toplevel";
STRING	abmfP_widget_var_name = "widget";
STRING  abmfP_tt_msg_var_name = "msg";
STRING  abmfP_ss_session_file_var_name = "session_file";
STRING  abmfP_argv_var_name = "argv";
STRING  abmfP_argc_var_name = "argc";

