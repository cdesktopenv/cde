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
/* $XConsortium: ilversion.c /main/3 1995/10/23 16:02:54 rswiston $ */
/*  /ilc/ilversion.c : Contains the IL version string
    (which should really be dynamically generated).
*/

#include "ilint.h"

/*
   Image Library version <external>[ <baselevel>], for <OS> <OS version> '<library name>'
   WARNING: FIXED FORMAT EXPECTED BY /ilc/iltiffwrite.c !!!
   See notes below "ilVersionName".
*/

#ifdef __hpux 
    IL_PRIVATE 
   char _ilVersionString[] = "HP IL BETA v 23.  bl08";
/*                                nnnnn vvvvvv bbbb       see below
                                  xxxxxxxxxxxxxxxxx       total size = 17 chars

        The above version string *must* maintain the syntax and position of all fields
        before the ";".  The strings have the following format; each string is 5 chars:
            nnnnn       A 5 char fixed string equal to "HP IL ".
            vvvvvv      A 6 char string in the format "v!!.??", where !! is the two-digit
                        major version number (" n" if less than 10), and ?? is the minor
                        version number ("n " if less than 10).  Examples: "v10.12",
                        "v 9.6 ".
            bbbbb       A 4 char string in the format "bl??", where ?? is the baselevel
                        number (e.g. "bl01", "bl56").

*/

#else

    IL_PRIVATE
   char _ilVersionString[] = "@(#)HP IL v 2.1  bl06; Image Library for Domain/OS SR10.X 'il'";
/*                                nnnnn vvvvvv bbbb       see above
                                  xxxxxxxxxxxxxxxxx       total size = 17 chars
*/

#endif

