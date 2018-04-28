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
 * $XConsortium: mark_base.h /main/4 1996/07/18 14:47:51 drk $
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


#ifndef _mark_base_h
#define _mark_base_h 1

#include "oliasdb/user_base.h"

/*************************************/
// The mark class
/*************************************/

class mark_base : public user_base
{

public:
   mark_base(const char* base_dir, 
             const char* base_name, 
             const char* base_desc,
             user_base::rw_flag_t = user_base::READ
            );
   mark_base(user_base::rw_flag_t = user_base::READ);
   virtual ~mark_base();

// return user marks associated with a node locator
   oid_list_handler* get_mark_list(const char* node_locator);

// iterate over all user marks 
   mmdb_pos_t first();
   oid_t get_mark_oid(mmdb_pos_t& ind);
   void next(mmdb_pos_t& ind);

protected:

protected:
   cset_handler* mark_set_hd;

//a16 back compatible
   friend class user_mark_smart_ptr;

   friend class mark_smart_ptr;
   friend class mark_hd;
};

typedef mark_base* mark_basePtr;


#endif
