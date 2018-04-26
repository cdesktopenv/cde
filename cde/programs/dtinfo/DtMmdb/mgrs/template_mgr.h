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
 * $XConsortium: template_mgr.h /main/4 1996/07/18 14:40:14 drk $
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


#ifndef _template_mgr
#define _template_mgr 1


#include "dstr/memory_pool.h"
#include "object/root.h"
#include "dynhash/imp_die.h"
#include "storage/abs_storage.h"

typedef imp_die template_objs_clt_t; 

#define CDR_FLAG 0x8000


class template_mgr_t 
{
public:
   template_mgr_t() ;
   virtual ~template_mgr_t() ;

   root* look_up(c_code_t c_code);
   Boolean init_obj(abs_storage* store, mmdb_pos_t pos, root*& x);
   Boolean commit_obj(abs_storage* store, root* x);
   Boolean quit_obj(abs_storage* store, root* x);

   Boolean create_obj(abs_storage* store, c_code_t type, root*& new_obj);
   Boolean destroy_obj(abs_storage* store, root* x);

   Boolean insert_template(root* tmt);

// return  0: slot is not an object entry
// return >1: slot is an object entry and the object class code is returned
   c_code_t peek_slot(abs_storage* store, mmdb_pos_t pos);

protected:
// return class code with CDR_FLAG tag
   c_code_t _peek_slot(abs_storage* store, mmdb_pos_t pos, char*& z, int& len);

protected:
   template_objs_clt_t v_template_objs;
   //static memory_pool v_object_copy_space_pool;
};

//extern template_mgr_t* template_mgr;

#endif
