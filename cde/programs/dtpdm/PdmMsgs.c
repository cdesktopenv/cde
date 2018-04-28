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
/* $XConsortium: PdmMsgs.c /main/3 1996/08/12 18:42:25 cde-hp $ */
/*
 * dtpdm/PdmMsgs.c
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifdef I18N_MSG

#include <nl_types.h>

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif 

#define DTPDM_CAT_NAME "dtpdm"

#endif /* I18N_MSG */

/*
 * ------------------------------------------------------------------------
 * Default message values
 */
const char DtPdmMsg_0001[] = "Locale unsupported by X";
const char DtPdmMsg_0002[] = "Auto-select";
const char DtPdmMsg_0003[] = "Default";
const char DtPdmMsg_0004[] = "Syntax error parsing medium-source-sizes";
const char DtPdmMsg_0005[] = "Syntax error parsing input-trays-medium";
const char DtPdmMsg_0006[] = "Syntax error parsing document format";


#ifdef I18N_MSG

/*
 * ------------------------------------------------------------------------
 * Name: DtPdmGetMessage
 *
 * Parameters:
 *
 *   int	set -		The message catalog set number.
 *
 *   int	n - 		The message number.
 *
 *   char	*s -		The default message if the message is not
 * 				retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 */
const char *
DtPdmGetMessage(
		int set,
		int n,
		char * s)
{
    char *msg;
    nl_catd catopen();
    char *catgets();
    static int first = 1;
    static nl_catd nlmsg_fd;
    
    if(set == -1 || n == -1)
	return s;
    
    if(first) 
    {
	first = 0;
	nlmsg_fd = catopen(DTPDM_CAT_NAME, NL_CAT_LOCALE);
    }
    msg=catgets(nlmsg_fd,set,n,s);
    return (msg);
}
#endif /* I18N_MSG */
