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
/*******************************************************************************
**
**  resource.h
**
**  static char sccsid[] = "@(#)resource.h 1.5 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: resource.h /main/3 1995/11/03 10:38:43 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _RESOURCE_H
#define _RESOURCE_H
 
#include "ansi_c.h"
#include <sys/types.h>

/*
**  Resource structure
*/
struct Resource {
	char		*resource_name;
	char		*resource_value;
	struct Resource	*next;
};
typedef struct Resource Resource;

/*
**  Function declarations
*/
extern void		free_resources		P((Resource*));
extern char	    	*get_resource		P((Resource*, char *, char*,
							char*, char*));
extern char		*get_resource_by_val	P((Resource*, char*, char*));
extern boolean_t	load_resources		P((Resource**, char*));
extern boolean_t	save_resources		P((Resource*, char*));
extern boolean_t	set_resource		P((Resource**, char*, char*,
							char*, char*));
extern boolean_t	set_resource_by_val	P((Resource**, char*, char*));

#endif
