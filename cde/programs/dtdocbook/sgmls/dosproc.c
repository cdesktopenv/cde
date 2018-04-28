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
/* $XConsortium: dosproc.c /main/3 1996/06/19 17:14:34 drk $ */
/* dosproc.c -

   MS-DOS implementation of run_process().

     Written by James Clark (jjc@jclark.com).
*/

#include "config.h"

#ifdef SUPPORT_SUBDOC

#include "std.h"
#include "entity.h"
#include "appl.h"

#include <process.h>

int run_process(argv)
char **argv;
{
     int ret;
     fflush(stdout);
     fflush(stderr);
     ret = spawnvp(P_WAIT, argv[0], argv);
     if (ret < 0)
	  appl_error(E_EXEC, argv[0], strerror(errno));
     return ret;
}

#endif /* SUPPORT_SUBDOC */

/*
Local Variables:
c-indent-level: 5
c-continued-statement-offset: 5
c-brace-offset: -5
c-argdecl-indent: 0
c-label-offset: -5
End:
*/
