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
 *	$XConsortium: gil.h /main/3 1995/11/06 18:27:23 rswiston $
 *
 * @(#)gil.h	1.8 02 Feb 1995	cde_app_builder/src/libABobj
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
 * gil.h - gil file defines, et cetera.
 */
#ifndef _ABIL_GIL_H_
#define _ABIL_GIL_H_

#include <ab_private/obj.h>

#define GIL_MAX_NAME_LENGTH	1023
#define GIL_MAX_NAME_SIZE	(GIL_MAX_NAME_LENGTH + 1)/*size includes NULL*/

int		gil_init(void);		/* CALL THIS FIRST! */

extern ABObj	gil_load_file(
			STRING	interfaceFilePath,
			FILE	*inFile,
			ABObj	project
		);
extern int	gil_load_project_file(
			STRING	projectFilePath,
			FILE	*inFile,
			ABObj	*projectOut
		);
extern int	gil_load_project_file_and_resolve_all(
			STRING	projectFilePath,
			FILE	*inFile,
			ABObj	*projectOut
		);

#endif /* _ABIL_GIL_H_ */
