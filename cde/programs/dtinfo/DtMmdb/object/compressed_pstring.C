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
 * $XConsortium: compressed_pstring.cc /main/4 1996/06/11 17:23:44 cde-hal $
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


#include "object/compressed_pstring.h"

#ifdef C_API
buffer* compressed_pstring::working_buffer_ptr;
buffer* compressed_pstring::v_cp_io_buf_ptr;
#define working_buffer (*working_buffer_ptr)
#define v_cp_io_buf (*v_cp_io_buf_ptr)
// for pstring's buffer
#define v_io_buf (*v_io_buf_ptr) 
#else
buffer compressed_pstring::working_buffer(LBUFSIZ);
buffer compressed_pstring::v_cp_io_buf(LBUFSIZ);
#endif


compressed_pstring::compressed_pstring(c_code_t c_id) : 
   pstring(c_id), agent(0), v_uncompressed_sz(0)
{
}

compressed_pstring::~compressed_pstring()
{
}

MMDB_BODIES(compressed_pstring)

char* compressed_pstring::get(buffer& string_buffer)
{
//MESSAGE(cerr, "compressed_pstring::get()");
//debug(cerr, v_uncompressed_sz);


   string_buffer.reset();
   string_buffer.expand_chunk(v_uncompressed_sz+1);

   pstring::get(working_buffer);

// use cached version of compression agent
//debug(cerr, compress_agent_id);

   if ( (storage_ptr -> my_oid()).ccode() ==  MEM_STORAGE_CODE )
      agent = (compress_agent_handler*)
	(new handler(DICT_AGENT_CODE, storage_ptr));
   else
      agent = new compress_agent_handler(compress_agent_id, storage_ptr);

   (*agent) -> decompress(working_buffer, string_buffer);

   delete agent;

   string_buffer.get_base()[v_uncompressed_sz] = 0;

//MESSAGE(cerr, "compressed_pstring::get() done");
   return string_buffer.get_base();
}

io_status compressed_pstring::asciiIn(istream& in) 
{
   compress_agent_id.asciiIn(in);	// compress agent oid part
//debug(cerr, compress_agent_id);
   return this -> _asciiIn(in);
}

io_status compressed_pstring::asciiIn(const char* buf, int size, const oid_t& id) 
{
   compress_agent_id.become(id);	// compress agent oid part
   return this -> _asciiIn(buf, size);
}

io_status compressed_pstring::asciiIn(const char* buf, int size) 
{
   return pstring::asciiIn(buf, size);
}

io_status compressed_pstring::_asciiIn(istream& in) 
{
// use cached version of compression agent

   if ( (storage_ptr -> my_oid()).ccode() ==  MEM_STORAGE_CODE )
      agent = (compress_agent_handler*)
	(new handler(DICT_AGENT_CODE, storage_ptr));
   else
      agent = new compress_agent_handler(compress_agent_id, storage_ptr);

   pstring::_asciiIn(in);

   _compress();

   return done;
}

io_status compressed_pstring::_asciiIn(const char* buf, int size) 
{
// use cached version of compression agent
   if ( (storage_ptr -> my_oid()).ccode() == MEM_STORAGE_CODE )
      agent = (compress_agent_handler*)
	(new handler(DICT_AGENT_CODE, storage_ptr));
   else
      agent = new compress_agent_handler(compress_agent_id, storage_ptr);

   pstring::asciiIn(buf, size); 	// uncompress data part

   _compress();

   return done;
}

void compressed_pstring::_compress() 
{
   v_uncompressed_sz = v_io_buf.content_sz();

   if ( v_uncompressed_sz > 0 ) {

      working_buffer.reset();

      (*agent) -> compress(v_io_buf, working_buffer);

//debug(cerr, v_uncompressed_sz);
//debug(cerr, working_buffer.content_sz());

      pstring::update(working_buffer.get_base(),  working_buffer.content_sz());
   }

   set_mode(UPDATE, true);

   delete agent;

}

int compressed_pstring::size() const
{
   return v_uncompressed_sz;
}

int compressed_pstring::cdr_sizeof()
{
   return pstring::cdr_sizeof() + sizeof(v_uncompressed_sz) + 
          compress_agent_id.cdr_sizeof(); 
}

io_status compressed_pstring::cdrOut(buffer& buf)
{
//MESSAGE(cerr, "compressed_pstring: cdrOut()");
//debug(cerr, compress_agent_id);

   pstring::cdrOut(buf);
   buf.put(v_uncompressed_sz);
   return compress_agent_id.cdrOut(buf);
}

io_status compressed_pstring::cdrIn(buffer& buf)
{
   pstring::cdrIn(buf);
   buf.get(v_uncompressed_sz);
   return compress_agent_id.cdrIn(buf);
}

HANDLER_BODIES(compressed_pstring)
