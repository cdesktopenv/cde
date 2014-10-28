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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: Options.C /main/2 1995/07/17 13:54:17 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "Options.h"
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdio.h>
#include <pwd.h>

/**************************************************************
 *
 * Two options are available for dtappgather:
 *
 *	-v	Verbose    - prints the Search Path environment
 *			     variables to standard output.
 *	-r	Retain     - Retain previous contents of the 
 *			     Application Manager
 *
 **************************************************************/

Options::Options
	(
	unsigned int argc,
	char **      argv
	) : flags(0)
{
CString Usage ("Usage: dtappgather [ -v | -r ]");

    if (argc > 1) {
	for (int i = 1; i < argc; i++) {
	    if (strcmp(argv[i],"-v") == 0)
		flags += 1;
	    else if (strcmp(argv[i],"-r") == 0)
		flags += 2;
	    else {
		fprintf(stderr,"%s\n",Usage.data());
		exit (1);
	    }
	}
    }
}

