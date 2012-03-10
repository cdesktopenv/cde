/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: ensure_end_slash
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************** ENDSLASH.C ****************
 * $XConsortium: endslash.c /main/4 1996/05/07 13:33:42 drk $
 * If passed path name string does not end in a slash, adds a slash.
 * There MUST BE ROOM for the extra slash in the passed string buffer!
 * Does nothing if passed pathstr is completely empty
 * to prevent a presumed 'root path' for files in curr work dir.
 * Returns ptr to zero byte at end of string.
 *
 * $Log$
 * Revision 2.2  1995/10/26  15:23:51  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:52:53  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.1  1995/07/18  22:41:43  miker
 * Initial release as separate module.
 */
#include <string.h>
char	*ensure_end_slash (char *pathstr)
{
    char	*cptr = pathstr + strlen(pathstr) - 1;
    if (*pathstr == 0)
	return pathstr;
    if (*cptr++ != '/') {
	*cptr++ = '/';
	*cptr = 0;
	}
    return cptr;
}

