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
 * $XConsortium: pm_random.cc /main/3 1996/06/11 17:38:16 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

/////////////////////////////////////////////////////////////////
// In CACM July. 1993, pp 109, Park and Miller said that
// a=48271 is little better. In that case, q=44488 and r=3399.
// -qfc, July 2, 1993
/////////////////////////////////////////////////////////////////

#include "utility/pm_random.h"

pm_random::pm_random(int sd) 
{
   seed(sd);
}

void 
pm_random::seed(int seed)  
{
   if ( !INRANGE( seed, 1, 2147483646) ) {
      MESSAGE(cerr, 
              "pm_random::seed(): seed should be in [1, 2147483646]"
             );
      throw(boundaryException(1, 2147483646, seed));
   }

   
   v_i_seed = seed;
   int test = 16807 * (seed % 127773) - 2836 * (seed / 127773);
   v_new_seed = ( test > 0 ) ? test : test + 2147483647;
}

