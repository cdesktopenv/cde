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
 * $XConsortium: pathcollapse.C /main/5 1996/06/21 17:36:32 ageorge $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <codelibs/nl_hack.h>
#include <codelibs/pathutils.h>

#define SEP(P) (CHARAT(P) == '\0' || CHARAT(P) == '/')
#define DOT(P) (CHARAT(P) == '.' && SEP((P) + 1))
#define DOTDOT(P) (CHARAT(P) == '.' && DOT((P) + 1))

#ifdef __cplusplus
extern "C"
#endif
char *
pathcollapse(const char *src, char *dst, boolean show_dir)
{
    int ch;
    const char *srcp;
    char *dstp,  *sep;
    char *components[MAXPATHLEN / 2 + 1];
    char **comp = components;
    int length = src == NULL ? 0 : strlen(src);
    int dir_comp;		/* TRUE if last component was . or .. */
    wchar_t __nlh_char[1];

    if (length == 0 || length > MAXPATHLEN)
    {
	errno = EINVAL;
	return NULL;
    }

    if (dst == NULL)
	if ((dst = (char *)malloc(length + 1)) == NULL)
	    return NULL;

    srcp = src;
    dstp = dst;

    do				/* for each component of src */
    {
	*comp = dstp;

	/* copy the component and trailing separator to dst */
	do
	{
	    ch = (int)CHARAT(srcp);
	    sep = dstp;
	    WCHARADV(ch, dstp);
	    if (ch == '\0')
		break;
	    ADVANCE(srcp);
	} while (ch != '/');

	dir_comp = ch == '/';	/* true if trailing '/' */

	/* skip all adjacent '/' characters [the first is preserved] */
	while (CHARAT(srcp) == '/')
	    ADVANCE(srcp);

	/* remove redundant trailing slash */
	if (CHARAT(srcp) == '\0')
	    if (sep > dst)
		*sep = ch = '\0';

	if (DOT(*comp))
	{
	    dir_comp = 1;
	    dstp = *comp;
	    if (dstp > dst)
	    {
		sep = dstp - 1;
		*dstp = '\0';
	    }
	    continue;
	}
	else if (DOTDOT(*comp))
	{
	    dir_comp = 1;
	    if (*comp > dst)
	    {
	    comp--;
	    if (!DOTDOT(*comp))
	    {
		dstp = *comp;
		if (dstp > dst)
		{
		    sep = dstp - 1;
		    *dstp = '\0';
		}
		else
		{
		    if (CHARAT(dst) == '/')
		    {
			/* /.. is same as / */
			dstp = dst + 1;
			*dstp = '\0';
			comp++;
		    }
		    else
			dst[0] = '.';
		    sep = dst + 1;
		    *sep = '\0';
		}
		continue;
	    }
	    }
	}

	comp++;
    } while (ch != '\0');

    if (show_dir)
    {
	if (dir_comp && (sep > dst + 1 || dst[0] != '/'))
	{
	    *sep++ = '/';
	    *sep = '\0';
	}
    }
    else if (sep > dst)
	*sep = '\0';		/* remove trailing '/' */

    return dst;
}
