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
 *	$XConsortium: load.h /main/3 1995/11/06 18:30:12 rswiston $
 *
 *	@(#)load.h	1.13 09 Sep 1994	cde_app_builder/src/libABil
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
 * load.h - public functions to load a file.
 */
#ifndef _ABIL_BIL_LOADFILE_H_
#define _ABIL_BIL_LOADFILE_H_

#include <ab_private/istr.h>

/*
 * Functions
 */
extern STRING   abil_loadmsg(STRING);   /* output error message */
extern int      abil_loadmsg_clear(void);
extern int      abil_loadmsg_set_object(STRING objname);
extern int      abil_loadmsg_set_att(STRING attname);
extern int      abil_loadmsg_set_action_att(STRING actattname);
extern int      abil_loadmsg_set_file(STRING bilfile);
extern int 	abil_print_load_err(int error_num);
extern int 	abil_print_custom_load_err(STRING error_msg);
extern int	abil_loadmsg_set_err_printed(BOOL); 
extern BOOL 	abil_loadmsg_err_printed(void);

#endif /* _ABIL_BIL_LOADFILE_H_ */

