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
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: DtSearchExit
 *		DtSearchAddUserExit
 *		DtSearchRemoveUserExit
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* AUSEXIT.C ************************
 * $XConsortium: ausexit.c /main/6 1996/06/23 16:47:24 cde-ibm $
 * August 1993.
 * Performs generic exit function for AusText system ensuring
 * graceful shutdown for vista database, curses lib, etc.
 * Declares several global function pointers.
 * Austext_exit has been renamed to DtSearchExit to support
 * it's public visibility in DtSearch.
 *
 * Replaces both exitop.c and exitopv.c and changes the name
 * of the function from opera_exit() to austext_exit().
 * The name change ensures that the linker will catch all
 * the places we have to set global 'austext_exit_dbms' to d_close().
 * These were the former exitopV.c links; the plain exitop.c
 * links were non-vista programs so only the name has to be changed.
 *
 * If austext_exit_endwin is not NULL, then a windows-oriented UI
 * is executing and austext_exit_endwin should be called before
 * shutting down to ensure restoration of normal terminal mode.
 * For curses, austext_exit_endwin is set to the curses endwin()
 * function immediately after curses initialization (initscr() call).
 *
 * If austext_exit_dbms is not NULL, then current process
 * is linked to database api and function should be called
 * to flush and close databases.  For vista, austext_exit_dbms
 * is set to d_close() at beginning of vista-using program.
 *
 * If austext_exit_mem is not NULL, then current process
 * is using shared memory and function should be called
 * to release it to the system.
 *
 * If austext_exit_comm is not NULL, then current process
 * is communicating to a remote process over a network.
 * The function is in a local communications package and
 * should be called to gracefully shutdown local child/parent 
 * processes and daemons and allow a message to be sent to
 * the remote process to do the same.
 *
 * Austext_exit_first and last are miscellaneous private
 * exits which get called first and last respectively.
 * Austext_exit_user is called just before 'last',
 * but 'user' is public, ie known to customer developers
 * using the DtSearch/ausapi interface, whereas 'first' and 'last'
 * are private, reserved for the ausapi developers themselves.
 *
 * $Log$
 * Revision 2.3  1996/04/10  19:45:58  miker
 * Added DtSearchAddUserExit and DtSearchRemoveUserExit.
 *
 * Revision 2.2  1995/10/25  22:20:53  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  18:51:11  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.4  1995/08/31  21:54:23  miker
 * Rename austext_exit() to DtSearchExit().
 */
#include <stdlib.h>
#include "Search.h"

void	(*austext_exit_first) (int) =	NULL;
void	(*austext_exit_dbms) (int) =	NULL;
void	(*austext_exit_comm) (int) =	NULL;
void	(*austext_exit_endwin) (int) =	NULL;
void	(*austext_exit_mem) (int) =	NULL;
void	(*austext_exit_user) (int) =	NULL;
void	(*austext_exit_last) (int) =	NULL;


/****************************************/
/*					*/
/*		DtSearchExit		*/
/*					*/
/****************************************/
void	DtSearchExit (int return_code)
{
    if (austext_exit_first != NULL)
	austext_exit_first (return_code);
    if (austext_exit_dbms != NULL)
	austext_exit_dbms (return_code);
    if (austext_exit_comm != NULL)
	austext_exit_comm (return_code);
    if (austext_exit_endwin != NULL)
	austext_exit_endwin (return_code);
    if (austext_exit_mem != NULL)
	austext_exit_mem (return_code);
    if (austext_exit_user != NULL)
	austext_exit_user (return_code);
    if (austext_exit_last != NULL)
	austext_exit_last (return_code);
    exit (return_code);
}

/****************************************/
/*					*/
/*	    DtSearchAddUserExit		*/
/*					*/
/****************************************/
void    DtSearchAddUserExit (void (*user_exit)(int))
{ austext_exit_user = user_exit; }


/****************************************/
/*					*/
/*	  DtSearchRemoveUserExit	*/
/*					*/
/****************************************/
void    DtSearchRemoveUserExit (void)
{ austext_exit_user = NULL; }

/********************* AUSEXIT.C ************************/
