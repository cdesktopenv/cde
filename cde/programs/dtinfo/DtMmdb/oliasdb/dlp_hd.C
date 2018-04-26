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
 * $XConsortium: dlp_hd.cc /main/4 1996/06/11 17:28:37 cde-hal $
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


#include "oliasdb/dlp_hd.h"

handler* dlp::get_component(int index) 
{
   if ( index !=  1 )
      throw(boundaryException(1, 1, index));

   return new handler(/*c,*/ f_oid, storage_ptr);
}

io_status dlp::asciiOut(ostream& out)
{
//////////////////////////////////////////////
// qfc: cause dlp trouble to find end nodes.
//////////////////////////////////////////////
/*
   if ( BIT_TEST(pos_status, BEG) )
      out << '+';
*/

   io_status ok = node_oid.asciiOut(out);

/*
   if ( BIT_TEST(pos_status, TERM) )
      out << '-';
*/

   return ok;
}

// format for a book section:
//  1004 \n
//  +x.x \n
//  ...
//  1004 \n
//  x.x \n
//  - \n
// 
// a dlp stream is (book_section)+

io_status dlp::asciiIn(istream& in)
{
   set_mode(UPDATE, true);

   pos_status = NOT_BEG_TERM;

   int plus = in.get();

   if ( plus == '+' ) {
      pos_status |= BEG;
   } else
      in.putback(char(plus));

   io_status ok = node_oid.asciiIn(in);

   int minus = in.get();

   if ( minus == '-' ) {
      pos_status |= TERM;
      if ( in.get() != '\n' )
      throw(stringException("should be a newline"));
   } else
      in.putback(char(minus));

//debug(cerr, int(BEG));
//debug(cerr, int(TERM));
//debug(cerr, int(pos_status));
//debug(cerr, node_oid);

   return ok;
}

int dlp::cdr_sizeof()
{
   return dl_list_cell::cdr_sizeof() + node_oid.cdr_sizeof() + sizeof(pos_status);
}

io_status dlp::cdrOut(buffer& buf)
{
   dl_list_cell::cdrOut(buf);
   node_oid.cdrOut(buf);
   buf.put(pos_status);
   return done;
}

io_status dlp::cdrIn(buffer& buf)
{
   dl_list_cell::cdrIn(buf);
   node_oid.cdrIn(buf, OLIAS_NODE_CODE);
   buf.get(pos_status);
   return done;
}


MMDB_BODIES(dlp)

///////////////////////////////////
//
///////////////////////////////////

dlp_smart_ptr::
dlp_smart_ptr(info_lib* lib_ptr, 
              const char* info_base_name, const oid_t& id): 
smart_ptr(lib_ptr, info_base_name, DLP_LIST_POS, 
          managers::query_mgr -> form_oid_handler(
                    oid_t(OLIAS_NODE_CODE, id.icode())
                                                 ),
////////////////////////////////////////////////////////////
// to keep backward compatibility
// up to 2.2: position is specified by BASE_COMPONENT_INDEX
// 2.3 and higher: position is specified by THIS
////////////////////////////////////////////////////////////
(lib_ptr->get_info_base(info_base_name)->
get_list(DLP_LIST_POS)->its_store()->
get_db_version() < mm_version(2,3)) ? BASE_COMPONENT_INDEX : THIS, 
smart_ptr::LIST
         )
{
}

dlp_smart_ptr::
dlp_smart_ptr(info_base* bs_ptr, const oid_t& id) : 
smart_ptr(bs_ptr, DLP_LIST_POS, 
          managers::query_mgr -> form_oid_handler(
                    oid_t(OLIAS_NODE_CODE, id.icode())
                                                 ),
////////////////////////////////////////////////////////////
// to keep backward compatibility
// up to 2.2: position is specified by BASE_COMPONENT_INDEX
// 2.3 and higher: position is specified by THIS
////////////////////////////////////////////////////////////
(bs_ptr-> get_list(DLP_LIST_POS)->its_store()->
get_db_version() < mm_version(2,3)) ? BASE_COMPONENT_INDEX : THIS, 
           smart_ptr::LIST
         )
{
}

dlp_smart_ptr::
dlp_smart_ptr(dlp_smart_ptr& dsp, const oid_t& dlp_id) 
{
   smart_ptr::_init(dlp_id, dsp.its_store());
}

///////////////////////////////////
//
///////////////////////////////////
const oid_t dlp_smart_ptr::node_id()
{
   dlp* x = (dlp*)(handler::operator->());
   return oid_t( x -> node_oid );
}

dlp_smart_ptr* dlp_smart_ptr::prev()
{
   oid_t back_cell_oid(prev_dlp_oid());

   if ( back_cell_oid.icode() == 0 )
      return 0;
   else {

      dl_list_cell_handler* cell_hd = (dl_list_cell_handler*)this;

      dlp_smart_ptr* back_cell_hd = (dlp_smart_ptr*)
          new smart_ptr(cell_hd->its_store(), back_cell_oid); 

      if ( back_cell_hd -> node_id().icode() == 0 ) {
         delete back_cell_hd;
         return 0;
      } else
         return back_cell_hd;
   }
}

dlp_smart_ptr* dlp_smart_ptr::next()
{
   oid_t next_cell_oid(next_dlp_oid());

   if ( next_cell_oid.icode() == 0 )
      return 0;
   else {

      dl_list_cell_handler* cell_hd = (dl_list_cell_handler*)this;

      dlp_smart_ptr* next_cell_hd = (dlp_smart_ptr*)
          new smart_ptr(cell_hd->its_store(), next_cell_oid);

      if ( next_cell_hd -> node_id().icode() == 0 ) {
         delete next_cell_hd;
         return 0;
      } else
         return next_cell_hd;
   }
}
        
oid_t dlp_smart_ptr::next_node_oid()
{
   dlp_smart_ptr* x = next();

   if ( x ) {
      //return oid_t(x -> node_id());
      oid_t z(x -> node_id());
      delete x;
      return z;
   } else {
      return ground;
   }
}
        
oid_t dlp_smart_ptr::prev_node_oid()
{
   dlp_smart_ptr* x = prev();

   if (x) {
      oid_t z(x -> node_id());
      delete x;
      return z;
   } else {
      return ground;
   }
}

oid_t dlp_smart_ptr::prev_dlp_oid()
{
   if ( BIT_TEST(((dlp*)(handler::operator->())) -> pos_status, dlp::BEG) ) {
      return ground;
   } else {
      dl_list_cell_handler* cell_hd = (dl_list_cell_handler*)this;
      return oid_t((*cell_hd) -> get_backward_ptr());
   }
}

oid_t dlp_smart_ptr::next_dlp_oid()
{
//debug(cerr, *((dlp*)(handler::operator->())));
   if ( BIT_TEST(((dlp*)(handler::operator->())) -> pos_status, dlp::TERM) )
      return ground;
   else {
      dl_list_cell_handler* cell_hd = (dl_list_cell_handler*)this;
      return oid_t((*cell_hd) -> get_forward_ptr());
   }
}
