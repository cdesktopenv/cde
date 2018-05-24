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
/* $XConsortium: utility.c /main/4 1995/11/09 12:54:25 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#if defined(SunOS)
#include <netdb.h>
#include <sys/systeminfo.h>
#endif

#include "utility.h"
#include "lutil.h"

#if !defined(__linux__)
extern char * strdup(const char *);
#endif

/*
 * calendar_name@host[.domain] -> calendar_name
 */
extern char *
_DtCmsTarget2Name(char *target)
{
        return(_DtCmGetPrefix(target, '@'));
}
 
/*
 * calendar_name@host[.domain] -> host[.domain]
 */
extern char *
_DtCmsTarget2Location(char *target)
{
	char *ptr;

	if (target == NULL)
		return (NULL);

	if (ptr = strchr(target, '@')) {
		return (strdup(++ptr));
	} else
		return (NULL);
}
 
/*
 * calendar_name@host[.domain] -> host
 */
extern char *
_DtCmsTarget2Host(char *target)
{
        char *location, *host;
 
        if ((location = _DtCmsTarget2Location(target)) != NULL) {
                host = _DtCmGetPrefix(location, '.');
                free(location);
                return(host);
        } else
                return(NULL);
}

/*
 * calendar_name@host[.domain] -> domain
 */
extern char *
_DtCmsTarget2Domain(char *target)
{
        char *location, *domain, *ptr;
 
        if ((location = _DtCmsTarget2Location(target)) != NULL) {
		if (ptr = strchr(location, '.'))
			domain = strdup(++ptr);
		else
			domain = NULL;
                free(location);
                return(domain);
        } else
                return(NULL);
}

