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
 * $XConsortium: locator_hd.h /main/5 1996/10/26 18:18:57 cde-hal $
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


#ifndef _locator_hd_h
#define _locator_hd_h 1

#include "object/short_list.h"
#include "object/tuple.h"
#include "object/oid.h"
#include "object/pstring.h"
#include "object/integer.h"
#include "oliasdb/olias_consts.h"
#include "api/smart_ptr.h"

/*************************************/
// The locator class
/*************************************/

class olias_locator : public mmdb_tuple
{

public:
   olias_locator();
   virtual ~olias_locator() {};

   MMDB_SIGNATURES(olias_locator);

protected:
};

typedef olias_locator* locatorPtr;

class locator_smart_ptr : public smart_ptr
{
public:
   locator_smart_ptr(info_lib* lib_ptr,
                     const char* info_base_name, const char* locator);
   locator_smart_ptr(info_base* base_ptr, const char* locator);
   locator_smart_ptr(info_base* base_ptr, const oid_t& id);

   virtual ~locator_smart_ptr() {};


   const char* inside_node_locator_str();
   oid_t node_id();
   const char* label();
   //int vcc_num();
};

typedef locator_smart_ptr* locator_smart_ptrPtr;



#endif
