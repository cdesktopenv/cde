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
 * $XConsortium: zip.cc /main/3 1996/06/11 17:15:36 cde-hal $
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


#include "compression/zip.h"

//zip g_zip_agent;

#define UNCOMPRESSED	"/tmp/uncomp"
#define COMPRESSED	"/tmp/comp"

void zip::compress(const buffer& uncompressed, buffer& compressed) 
{
////////////////////////////////////////
// code for testing. I know it is slow. 
////////////////////////////////////////

   int ret;
   fstream out(UNCOMPRESSED, ios::out|ios::trunc);

   if ( !out )
      throw(streamException(out.rdstate()));

   if ( out.write(uncompressed.get_base(), uncompressed.content_sz()) == 0 )
      throw(streamException(out.rdstate()));

   out.close();

   ret = system(form("gzip -c %s > %s", (char*)UNCOMPRESSED,(char*)COMPRESSED));

   fstream in(COMPRESSED, ios::in);

   if ( !in )
      throw(streamException(in.rdstate()));

   int x = bytes((char*)COMPRESSED);

   compressed.expand_chunk(x);

   if ( in.read(compressed.get_base(), x) == 0 || x != in.gcount() )
      throw(streamException(in.rdstate()));

   compressed.set_content_sz(x);

   in.close();

   return;
}

void zip::decompress(buffer& compressed, buffer& uncompressed) 
{
   int ret;
   fstream out(COMPRESSED, ios::out|ios::trunc);

   if ( !out )
      throw(streamException(out.rdstate()));

   if ( out.write(compressed.get_base(), compressed.content_sz()) == 0 )
      throw(streamException(out.rdstate()));

   out.close();

   ret = system(form("gzip -cd %s > %s",(char*)COMPRESSED,(char*)UNCOMPRESSED));

   fstream in(UNCOMPRESSED, ios::in);

   if ( !in )
      throw(streamException(in.rdstate()));

   int x = bytes((char*)UNCOMPRESSED);

   uncompressed.expand_chunk(x);

   if ( in.read(uncompressed.get_base(), x) == 0 || x != in.gcount() )
      throw(streamException(in.rdstate()));

   uncompressed.set_content_sz(x);

   in.close();

   return;
}

io_status zip::build_dict(lex_func_t, getchar_func_t)
{
   return done;
}

MMDB_BODIES(zip)

int zip::cdr_sizeof()
{
   return 0;
}

io_status zip::cdrOut(buffer&)
{
   return done;
}

io_status zip::cdrIn(buffer&)
{
   return done;
}


