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
 * $XConsortium: node_hd.h /main/4 1996/06/11 17:29:56 cde-hal $
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


#ifndef _node_hd_h
#define _node_hd_h 1

#include "object/tuple.h"
#include "object/pstring.h"
#include "object/compressed_pstring.h"
#include "object/oid.h"
#include "oliasdb/olias_consts.h"
#include "api/smart_ptr.h"

// A16 back compatible
#ifdef A16_BROWSER
#define long_title title
#endif

/*************************************/
// The node class
/*************************************/

class olias_node : public mmdb_tuple
{

public:
   olias_node() : mmdb_tuple(NUM_OLIAS_NODE_FIELDS, OLIAS_NODE_CODE) {};
   virtual ~olias_node() {};

   MMDB_SIGNATURES(olias_node);

protected:
};

typedef olias_node* nodePtr;



class node_smart_ptr : public smart_ptr
{
public:
   node_smart_ptr(info_lib* lib_ptr, const char* info_base_name, const char* locator);
   node_smart_ptr(info_base* base_ptr, const char* locator);

   node_smart_ptr(info_lib* lib_ptr, const char* ibase_name, const oid_t& node_id);
   node_smart_ptr(info_base* ibase_ptr, const oid_t& node_id);
   virtual ~node_smart_ptr() {};

   const char* locator();
   const char* long_title();
   const char* short_title();
   const char* data();
   int data_size();
   const char* toc_node_loc();
   oid_t doc_id();

   oid_t stylesheet_id();
   //const char* stylesheet_name();

/////////////////////////////////////////////////////////////
// this function (update_data()) is for clustering SGML data
// portion to a set of centeralized pages
/////////////////////////////////////////////////////////////
   Boolean update_data(istream&);

/////////////////////////////////////////////////////////////
// this function is for update the doc id field so that
// the node object can be loaded earlier than the doc object.
/////////////////////////////////////////////////////////////
   void update_doc_id(const oid_t& doc_id);
};


typedef node_smart_ptr* node_smart_ptrPtr;

#endif
