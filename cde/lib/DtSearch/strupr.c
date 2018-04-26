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
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: strupr
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************************* STRUPR.C ***************************
 * $XConsortium: strupr.c /main/3 1996/05/07 13:48:29 drk $
 * String function that converts all chars to uppercase.
 * Same as DOS function with same name.  Formerly in fzansi.c.
 *
 * $Log$
 * Revision 2.2  1995/10/26  14:31:18  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:10:39  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.1  1995/09/05  17:50:11  miker
 * Initial revision
 */
#include <ctype.h>

char	*strupr (char *s)
{
    char	*t = s;
    while (*t != 0) {
	*t = toupper(*t);
	t++;
    }
    return s;
}

