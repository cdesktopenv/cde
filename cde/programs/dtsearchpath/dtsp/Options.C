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
/* $XConsortium: Options.C /main/4 1996/01/25 15:55:11 rswiston $ */
/*******************************************************************
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 *
 ********************************************************************
 ****************************<+>*************************************/

#include "Options.h"
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdio.h>
#include <pwd.h>

/**************************************************************
 *
 * Two options are available for dtsearchpath:
 *
 *	-v	Verbose    - prints the Search Path environment
 *			     variables to standard output.
 *	-u	Check User - determines the Search Path for a 
 *			     particular user.
 *      -a      AutoMount  - remove the autmounter's mount point
 *
 *      -csh    force Csh style environment set commands
 *
 *      -ksh    force Ksh style environment set commands
 **************************************************************/

Options::Options
	(
	unsigned int argc,
	char **      argv
	) : flags(0), user_id(0), home_dir(0), automountpoint(0)
{
CString Usage ("Usage: dtsearchpath [ -v | -T | -o | -a | -csh | -ksh ] "
	       "[ -u <login-name> ]");

    if (argc > 1) {
	for (int i = 1; i < argc; i++) {
	    if (strcmp(argv[i],"-v") == 0)
		flags |= 1;
	    else if (strcmp(argv[i],"-T") == 0)
		flags |= 4;
	    else if (strcmp(argv[i],"-o") == 0)
		flags |= 8;
	    else if (strcmp(argv[i],"-a") == 0)
		flags |= 16;
	    else if (strcmp(argv[i],"-csh") == 0)
		flags |= 32;
	    else if (strcmp(argv[i],"-c") == 0)	// for bc with sun version
		flags |= 32;
	    else if (strcmp(argv[i],"-ksh") == 0)
		flags |= 64;
	    else if (strcmp(argv[i],"-u") == 0) {
		flags |= 2;
		if (++i < argc && argv[i][0] != '-') {
		    user_id = new CString(argv[i]);
		    struct passwd * pwd = getpwnam(user_id->data());
		    home_dir = new CString(pwd->pw_dir);
		}
		else {
		    fprintf(stderr, "%s\n",Usage.data());
		    exit(1);
		}
	    }
	    else {
		fprintf(stderr, "%s\n",Usage.data());
		exit(1);
	    }
	}
    }
#if defined(hpux) || defined(hpV4)
    flags |= 16;
#endif
}


Options::~Options()
{
    delete user_id;
    delete automountpoint;
    delete home_dir;
}


void Options::setUserID
	(
	const CString & login
	)
{
    user_id = new CString(login);
}

void Options::setAutoMountPoint
	(
	const CString & mountpoint
	)
{
    if (mountpoint.isNull())
	automountpoint = new CString("/tmp_mnt");
    else
        automountpoint = new CString(mountpoint);
}
