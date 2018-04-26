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
 * $XConsortium: stylesheet_hd.h /main/4 1996/06/11 17:30:53 cde-hal $
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


#ifndef _stylesheet_hd_h
#define _stylesheet_hd_h 1

#include "object/tuple.h"
#include "object/pstring.h"
#include "object/compressed_pstring.h"
#include "object/oid.h"
#include "oliasdb/olias_consts.h"
#include "api/smart_ptr.h"

/*************************************/
// The stylesheet class
/*************************************/

class stylesheet : public mmdb_tuple
{

public:
   stylesheet() : mmdb_tuple(NUM_STYLESHEET_FIELDS, STYLESHEET_CODE) {};
   virtual ~stylesheet() {};

   MMDB_SIGNATURES(stylesheet);

protected:
};

typedef stylesheet* stylesheetPtr;



class stylesheet_smart_ptr : public smart_ptr
{
private:
   char* get_data(int index);
   int get_data_size(int index);

// transaction guarded version
   void update_data(istream& in, int index);
   void update_data(const char* buf, int size, int index);

public:
   stylesheet_smart_ptr(info_lib* lib_ptr, const char* info_base_name, const char* locator);
   stylesheet_smart_ptr(info_base* base_ptr, const char* locator);

   stylesheet_smart_ptr(info_lib* lib_ptr, const char* ibase_name, const oid_t& stylesheet_id);
   stylesheet_smart_ptr(info_base* ibase_ptr, const oid_t& stylesheet_id);
   virtual ~stylesheet_smart_ptr() {};

   const char* name();

   const char* online_data(); // online style sheet
   int online_data_size();

   const char* hardcopy_data(); // hardcopy style sheet
   int hardcopy_data_size();

   void update_hardcopy_data(istream& in);
   void update_online_data(istream& in);

   void update_hardcopy_data(const char* buf, int size) ;
   void update_online_data(const char* buf, int size);
};


typedef stylesheet_smart_ptr* stylesheet_smart_ptrPtr;

#endif
