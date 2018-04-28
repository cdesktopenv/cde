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
/*  $XConsortium: svc_init.c /main/3 1996/07/09 14:47:28 drk $ */ 

#include <sys/uswitch.h>
#include <errno.h>

/*
 * Activate System V Null pointer treatment.  This allows dereferences
 * of NULL pointers in the CDE executables to work.
 *
 * This procedure is obsolete.  Dereferencing a NULL pointer is
 * an error, and such code should be repaired.
 */

svc_init()
{
    int usw_val;

    usw_val=uswitch(USC_GET,0);
    if ((uswitch(USC_SET, usw_val | USW_NULLP)) == 01) {
	perror("svc_init");
	return(-1);
    }
    return(0);
}
