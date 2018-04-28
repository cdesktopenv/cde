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
 *	$XConsortium: util_err.c /main/4 1995/11/06 18:53:42 rswiston $
 *
 * @(#)util_err.c	1.7 09 Jan 1995	cde_app_builder/src/libAButil
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
 * File: util_err.c
 */
#include <stdio.h>
#include <ab_private/util_err.h>
#include "utilP.h"

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
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

STRING
util_err_string(int errCode)
{
    STRING	err = NULL;

    switch (errCode)
    {
	case ERR_INTERNAL:	
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 4, "Internal error");
	break;
	case ERR_RECURSION:	
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 5, "Recursive function calls");
	break;
	case ERR_NOT_ALLOWED:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 6, "Permission denied");
	break;
	case ERR_DATA_SPACE_FULL:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 8, "Data space is full");
	break;
	case ERR_BUFFER_TOO_SMALL:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 9, "Buffer is too small");
	break;
	case ERR_NOT_IMPLEMENTED: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 10, "Functionality is not implemented");
	break;
	case ERR_NOT_INITIALIZED: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 11, "Not initialized");
	break;
	case ERR_CLIENT: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 12, "Client error");
	break;
	case ERR_NOT_FOUND: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 13, "Not found");
	break;
	case ERR_DUPLICATE_KEY: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 14, "Duplicate key");
	break;
	case ERR_BAD_PARAM: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 15, "Bad parameter");
	break;
	case ERR_IO: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 16, "I/O error");
	break;
	case ERR_FILE_NOT_FOUND: 
	err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 17, "File not found");
	break;
	case ERR_OPEN:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 18, "Could not open file");
	break;
	case ERR_WRITE:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 19, "Could not write to file");
	break;
	case ERR_READ:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 20, "Could not read file");
	break;
	case ERR_EOF:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 21, "End of file");
	break;
	case ERR_BAD_FILE_FORMAT:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 22, "Bad file format");
	break;
	case ERR_RENAME:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 23, "Could not rename file");
	break;
	case ERR_MEMORY:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 24, "Memory error");
	break;
	case ERR_NO_MEMORY:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 25, "No memory");
	break;
	case ERR_MEMORY_CORRUPT:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 26, "Memory is corrupt");
	break;
	case ERR_MULTIPLE_FREE:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 27, "Multiple free of memory");
	break;
	case ERR_READ_ONLY:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 28, "Read-only file");
	break;
	case ERR_BACKUP:
	    err= catgets(UTIL_MESSAGE_CATD, UTIL_MESSAGE_SET, 29, "Could not backup file");
	break;
    }

    /*
     * An error has already occurred. Don't return NULL and cause a 
     * core dump!
     */
    return err == NULL?"":err;
}

int
util_print_error(
    int		errCode, 
    STRING 	string
)
{
    STRING	errmsg = NULL;

    errmsg = util_err_string(errCode);
    if (string)
	util_printf_err("%s: %s\n", string, errmsg);
    else
	util_printf_err("%s\n", errmsg);

    return 0;
}
