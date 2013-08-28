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
 * $XConsortium: params.cc /main/4 1996/07/18 14:33:30 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#include "hmphf/params.h"

struct partition_t {
   int upper_bound;
   float bits;
   float p1;
   float p2;
} partition_tbl [] = {
   {100, 3.2, 0.52, 0.31}, {1000, 3.4, 0.55, 0.3}, {10000, 3.4, 0.57, 0.29},
   {MAXINT, 4.2, 0.58, 0.3} 
};

void params::select_value(float bts)  
{
   unsigned int i;
   for ( i=0; ; i++ ) {
      if ( (int)v_n <= partition_tbl[i].upper_bound )
         break;
   }

   if ( bts == 0.0 )
      v_bits = partition_tbl[i].bits;
   else
      v_bits = bts;

   v_r = 0;
   v_b = ( v_n > 0 ) ? (int)( float(v_n*v_bits) / (1.0+flog2(v_n)) ) : 0;

   v_p1 = (int)( partition_tbl[i].p1 * v_n);
   v_p2 = (int)( partition_tbl[i].p2 * v_b);

   if ( v_p1 == 0 ) v_p1++;
   if ( v_p2 == 0 ) v_p2++;

   v_seed = 1;
}

void params::re_select_value()  
{
   select_value(v_bits+0.2);
}

ostream& operator<<(ostream& out, params& pms)
{
   debug(out, pms.v_n);
   debug(out, pms.v_b);
   debug(out, pms.v_p1);
   debug(out, pms.v_p2);
   debug(out, (int)pms.v_bits);
   debug(out, pms.v_r);
   debug(out, pms.v_seed);
   return out;
}
