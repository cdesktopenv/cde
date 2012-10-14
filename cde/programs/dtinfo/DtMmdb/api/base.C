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
 * $XConsortium: base.cc /main/4 1996/06/11 17:10:55 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#include "api/base.h"

base::base(object_dict* obj_dict) :
   info_base_set_names(0), info_base_list_names(0),
   num_cset_ptrs(0), num_list_ptrs(0),
   f_obj_dict(obj_dict)
{
   base_path[0] = 0;
   base_name[0] = 0;
   base_desc[0] = 0;
   base_uid[0] = 0;
}

base::base(object_dict* obj_dict, 
      char** set_nms, char** list_nms,
      const char* base_dir, const char* base_nm, const char* base_ds,
      const char* base_uid_str
          ) :
   info_base_set_names(set_nms), info_base_list_names(list_nms),
   num_cset_ptrs(0), num_list_ptrs(0),
   f_obj_dict(obj_dict)
{
   if ( base_dir )
      strcpy(base_path, base_dir);
   else
      base_path[0] = 0;

   if ( base_nm )
      strcpy(base_name, base_nm);
   else
      base_name[0] = 0;

   if ( base_ds )
      strcpy(base_desc, base_ds);
   else
      base_desc[0] = 0;

   if ( base_uid_str )
      strcpy(base_uid, base_uid_str);
   else
      base_uid[0] = 0;

   if ( set_nms )
      while ( info_base_set_names[num_cset_ptrs] )
         num_cset_ptrs++;

   if ( list_nms ) {
      while ( info_base_list_names[num_list_ptrs] )
         num_list_ptrs++;
   }
}
   
base::~base() 
{
   ;
}

