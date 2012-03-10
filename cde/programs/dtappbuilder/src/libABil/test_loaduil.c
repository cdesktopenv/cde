
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
