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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tt_ldpath.C /main/7 1999/10/14 18:42:03 mgreess $ 			 				
/*
 *
 * tt_ldpath.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "util/tt_string.h"
#include "util/tt_port.h"
#include <stdio.h>
#include <string.h>     

/* Included after "util/tt_string.h" to avoid index/strchr conflicts. */
#define X_INCLUDE_DIRENT_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

static int
find_lib(_Tt_string &cpath, _Tt_string &libname, _Tt_string &fullpath)
{
	class stat	st;
	int 		result;
	DIR		*cdir;
	dirent		*dentry;
	_Tt_string	cname;
	_Tt_string	prefix;
	_Tt_string	vinfo;
	int		vmajor, vminor;
	int		max_vmajor, max_vminor;
	_Tt_string	best_version;
	int		dlen, llen;
	_Xreaddirparams	dir_buf;


	memset((char*) &dir_buf, 0, sizeof(_Xreaddirparams));
	result = 0;
	fullpath = cpath.cat("/").cat(libname);
	if (stat((char *)fullpath, &st) == 0) {
		result = 1;
	} else if (!(cdir = opendir((char *)cpath))) {
		// need to find a version in cpath if possible
		max_vmajor = -1;
		max_vminor = -1;
		llen = libname.len();
		while ((dentry = _XReaddir(cdir, dir_buf)) != (dirent *)0) {
			dlen = strlen(dentry->d_name);
			if (dlen >= llen) {
				cname = dentry->d_name;
				prefix = cname.left(llen);
				if (libname == prefix) {
					vinfo = cname.mid(llen + 1,
							  cname.len());
					sscanf((char *)vinfo,"%d.%d",
					       &vmajor, &vminor);
					if (vmajor >= max_vmajor &&
					    vminor > max_vminor) {
						max_vmajor = vmajor;
						max_vminor = vminor;
						best_version = cname;
					}
				}
			}
		}
		closedir(cdir);
		if (max_vmajor == -1) {
			result = 0;
		} else {
			fullpath = cpath.cat("/").cat(best_version);
			result = 1;
		}
	}
	return result;
}

// Returns:
//	1	success
//	0	failure; diagnostic logged
int
tt_ldpath(_Tt_string libname, _Tt_string &path)
{
	// iterate through the directories specified in
	// LD_LIBRARY_PATH with the addition of the hardcoded
	// directories /lib, /usr/lib, and /usr/local/lib to find a
	// version of libname. For each directory we try find libname
	// first. If that fails then we stat the directory and find
	// all the instances of libname plus major and minor versions.
	_Tt_string	cpath;
	_Tt_string	rpath;
	_Tt_string	ldpath;
	_Tt_string	tried;

	if (getenv("LD_LIBRARY_PATH") != (char *)0) {
		ldpath = getenv("LD_LIBRARY_PATH");
		ldpath = ldpath.cat(":/lib:/usr/lib:/usr/local/lib");
	} else {
		ldpath = "/lib:/usr/lib:/usr/local/lib";
	}
	tried = ldpath;
	while (ldpath.len()) {
		rpath = ldpath.split(':', cpath);
		if (cpath.len() == 0) {
			cpath = rpath;
		}
		if (find_lib(cpath, libname, path)) {
			return(1);
		}
		if (ldpath == rpath) {
			break;
		}
		ldpath = rpath;
	}
	_tt_syslog( 0, LOG_ERR,
		    "%s: %s ($LD_LIBRARY_PATH: %s)",
		    libname.operator const char *(), strerror(ENOENT),
		    tried.operator const char *() );
	return(0);
}


#ifdef TEST_MAIN
main(int argc, char **argv)
{
	_Tt_string	lib = argv[1];
	_Tt_string	path;
	
	if (_tt_ldpath(lib, path)) {
		printf("full_ldpath: %s\n", (char *)path);
	}
}
#endif				// TEST_MAIN
