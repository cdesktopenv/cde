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
/*
 * File:         Connect.h $XConsortium: Connect.h /main/3 1995/10/26 15:19:09 rswiston $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_connect_h
#define _Dt_connect_h

#include <Dt/DtP.h>
#include <bms/connect.h>
#include <codelibs/pathutils.h>

#define DtGetShortHostname Xegetshorthostname
#define DtGetHostname      Xegethostname
/*
    DESCRIPTION:
    
    	These functions are similiar to gethostname(2), however
	DtGetHostname always returns a full domain-qualified name 
	and DtGetShortHostname returns a simple name.
	
    SYNOPSIS:
    
    	status = DtGet[Short]Hostname (hostname, size)
	
	int status;		Returns 0 on success, -1 on failure.
				[Actually it returns what gethostname(2)
				returns, which is ambiguous in the
				HP-UX manual.]
				
	char *hostname;		The hostname is returned here.
	
	int size;		The name is truncated to "size - 1" and
				is null-terminated.
*/

#define DtGetcwd       Xegetcwd
/*
    DESCRIPTION:
    
    	This function is similar to getcwd except it first checks $PWD.
	It only calls getcwd if $PWD is not set.

    SYNOPSIS:
    
    	cwd = DtGetcwd (buf, size)

	char *cwd;		Pointer to the returned value.  (Typically 
				the same value as 'buf' that is passed in.)

	char *buf;		Pointer to memory allocated by the caller.
				Buf must be large enough to hold the string.

	int size;		Size of buf in bytes.
*/

#define DtIsLocalHostP XeIsLocalHostP
/*
    DESCRIPTION:
    
    	Tests whether a passed-in hostname identifies the host on which
	the function is being executed.  This handles all combinations of
	simple and domain-qualified names for either the hostname passed
	in or the one defined on the local host.
	
	WARNING: Returns BOOLEAN, not INT.  DONT TREAT IT AS AN INT!
	
    SYNOPSIS:
    
    	status = XeIsLocalHostP (hostname);
	
	Boolean status;		Returns TRUE if "hostname" identifies the
				local host, FALSE otherwise.
				
	char *hostname;		The hostname (either simple or domain-
				qualified) to test.
*/

#define DtIsSameHostP XeIsSameHostP
/*
    DESCRIPTION:
    
    	Compares two hostnames to see if they specify the same host.
	This handles combinations of simple and domain-qualified names.
	This function canonicalizes both names and then compares them.
	
	WARNING: Returns BOOLEAN, not INT.  DONT TREAT IT AS AN INT!
	
    SYNOPSIS:
    
    	status = DtIsSameHostP (host1, host2);
	
	Boolean status;		Returns TRUE if host1 and host2 identify
				the same host, FALSE otherwise.
				
	char *host1, *host2;	The two hostnames (either simple or
				domain-qualified) to compare.
*/

#define DtCreateContextString XeCreateContextString
/*
    DESCRIPTION:
    
        DtCreateContextString takes the three parts of a context and
	puts them into a single string, in the form "host:/dir/file".

        A NEW STRING, OWNED BY THE CALLER, is returned.
	
    SYNOPSIS:
    
    	context_string = DtCreateContextString (host, dir, file);
	
	DtString context_string;  The returned context.  The memory is owned
				  by the caller.  "NULL" is returned if the
				  context cannot be created.
				
	DtString host;	          The name of the host.
	
	DtString dir;		  The directory.
	
	DtString file;		  The name of the file.
*/	
	
#define DtEliminateDots	XeEliminateDots
/*
    DESCRIPTION:
    
    	This routine removes /./'s and /../'s from a path.  It will 
	OVERWRITE the path IT WAS PASSED.  If there are too many /../'s
	in the path this function will return NULL, so you better keep
	a pointer to the path if you hope to reclaim it.
	
	This function does not handle "host:/directory/file", shell
	variables, or other exotic animals.
	
    SYNOPSIS:
    
    	fixed_path = DtEliminateDots (path);
	
	DtString fixed_path;	A pointer to the same path that was passed in
				(though now it is fixed up) or NULL if problems
				were encountered.
				
	DtString path;		The path that needs fixing up.
*/	
	
#define DtParseFileString XeParseFileString

#endif /* _Dt_connect_h */
/* Do not add anything after this endif. */
