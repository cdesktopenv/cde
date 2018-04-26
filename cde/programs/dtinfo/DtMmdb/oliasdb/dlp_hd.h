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
 * $XConsortium: dlp_hd.h /main/4 1996/06/11 17:28:42 cde-hal $
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


#ifndef _dlp_hd_h
#define _dlp_hd_h 1

#include "object/oid_t.h"
#include "object/dl_list_cell.h"
#include "oliasdb/olias_consts.h"
#include "api/smart_ptr.h"

/*************************************/
// The dlp class
/*************************************/

class dlp : public dl_list_cell
{
public:
   dlp() : dl_list_cell(DLP_CODE), pos_status(NOT_BEG_TERM) {};
   virtual ~dlp() {};

   MMDB_SIGNATURES(dlp);

   handler* get_component(int) ;

// in/out functions
   io_status asciiOut(ostream&);
   io_status asciiIn(istream&);

   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);

protected:
   enum POS_STATUS { BEG=1, TERM=2, NOT_BEG_TERM=0 };

   oid_t node_oid;
   char  pos_status; // 1: beginning cell  
                     // 2: terminating cell
                     // 3: both beginning and terminating cell
                     // 0: neither beginning nor terminating cell

   friend class dlp_smart_ptr;
};

typedef dlp* dlpPtr;




class dlp_smart_ptr : public smart_ptr
{
public:
// x_id is not the oid of the dlp_hd, rather it is the
// content of the dlp_hd. 
   dlp_smart_ptr(info_lib* lib_ptr, const char* ibase_name, const oid_t& node_id);
   dlp_smart_ptr(info_base* ibase_ptr, const oid_t& node_id);

   dlp_smart_ptr(dlp_smart_ptr&, const oid_t& dlp_id);

   virtual ~dlp_smart_ptr() {};


// id of the node that this dlp represents
   const oid_t node_id();

// the prev and next dlp of this dlp
   dlp_smart_ptr* prev();
   dlp_smart_ptr* next();

   oid_t prev_node_oid();
   oid_t next_node_oid();

   oid_t prev_dlp_oid();
   oid_t next_dlp_oid();
};



#endif
