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
 *	$XConsortium: test_loaduil.c /main/3 1995/11/06 18:31:06 rswiston $
 *
 *	%W% %G%	
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
 * This file contains the unit test program for the uil load
 * component
 */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <ab_private/abuil_load.h>
#include "abuil_print.h"

/*
 * Main program
 */
int
main(
    int		argc,
    char	**argv
)
{
    ABObjPtr		ab_project;
    ABObjPtr		ab_interface;

    if (argc != 2)
    {
	fprintf(stderr, "Usage: test_loaduil <uil_file>\n");
	exit(1);
    }

    ab_project = obj_create(AB_TYPE_PROJECT, NULL);
    if ((ab_interface = abuil_get_uil_file(argv[1], ab_project))
	    == (ABObjPtr) NULL)
    {
	fprintf(stderr, "test_loaduil: loaduil %s failed.\n", argv[1]);
	exit(2);
    }

/*    util_set_verbosity(3); obj_print_tree(ab_project); */

    abuil_obj_print_uil(ab_project);

    obj_destroy(ab_project);
}
