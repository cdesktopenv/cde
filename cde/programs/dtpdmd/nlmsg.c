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
/* $XConsortium: nlmsg.c /main/1 1996/10/30 19:10:20 cde-hp $ */
/*
 * dtpdmd/nlmsg.c
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

#define DTPDMD_CAT_NAME "dtpdmd"

#endif /* I18N_MSG */

/*
 * ------------------------------------------------------------------------
 * Default message values
 */

/*** dtpdmd.c ***/
const char DtPdmdMsg_0001[]  = "\
usage: %1$s [options]\n\
\n\
where [options] includes:\n\
   -d <display>        host:display[.screen] of X Print Server\n\
   -a <alt selection>  an alternate selection name (\"%2$s\" default)\n\
   -p <default pdm>    a default pdm to launch (\"%3$s\" default)\n\
   -P <the pdm>        the pdm to launch, overrides all others\n\
   -s                  turn on security protocol\n\
   -f <auth file>      an initial set of authorization cookies\n\
   -l <log file>       a file for the logging of errors, etc";

const char DtPdmdMsg_0002[]  = "%1$s: error, cannot open display %2$s\n";
const char DtPdmdMsg_0003[]  = "%1$s: error, cannot acquire selection %2$s\n";
const char DtPdmdMsg_0004[]  = "%s: error, cannot create Xt Signal Pipe\n";

/*** manager.c ***/

const char DtPdmdMsg_0005[]  = "%s: error, unable to get SelectionRequest property\n";
const char DtPdmdMsg_0006[]  = "%s: error, invalid format for SelectionRequest property\n";
const char DtPdmdMsg_0007[]  = "%s: error, XmbTextPropertyToTestList failed on SelectionRequest property\n";
const char DtPdmdMsg_0008[]  = "%s: error, unable to create message pipe\n";
const char DtPdmdMsg_0009[]  = "%s: error, unable to fork\n";
const char DtPdmdMsg_0010[] = "%1$s: error, cannot execute \"%2$s\" \n";

const char DtPdmdMsg_0011[] = "\
%1$s: exec message logged at %2$s\n\
   exec error message = \"%3$s\"\n\
   exec error code    = %4$s\n\
   pdm                = \"%5$s\"\n\
   -display           = \"%6$s\"\n\
   -vdisplay          = \"%7$s\" \n";

const char DtPdmdMsg_0012[] = "\
%1$s: error, unable to send the final %2$s\n\
message to the requesting client on behalf of\n\
the PDM because of a generated X error\n";

const char DtPdmdMsg_0013[] = "\
%1$s: error, unable to send the final %2$s\n\
message to the requesting client on behalf of\n\
the PDM because of an XIO error\n";

const char DtPdmdMsg_0014[] = "\
%1$s: pdm message logged at %2$s\n\
   pdm       = \"%3$s\"\n\
   -display  = \"%4$s\"\n\
   -vdisplay = \"%5$s\"\n\
   exit code = \"%6$d\"\n\
   stderr    = \"%7$s\" \n";

const char DtPdmdMsg_0015[] = "\
   attached pdmd error= \n\"%s\" \n";

const char DtPdmdMsg_0016[] = "%1$s: error, cannot open log file \"%2$s\". Turning off stderr message logging\n";

#ifdef I18N_MSG

/*
 * ------------------------------------------------------------------------
 * Name: DtPdmdGetMessage
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
DtPdmdGetMessage(
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
	nlmsg_fd = catopen(DTPDMD_CAT_NAME, NL_CAT_LOCALE);
    }
    msg=catgets(nlmsg_fd,set,n,s);
    return (msg);
}
#endif /* I18N_MSG */
