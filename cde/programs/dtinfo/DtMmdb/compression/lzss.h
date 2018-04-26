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
/* $XConsortium: lzss.h /main/3 1996/06/11 17:15:21 cde-hal $ */


#ifndef _lzss_h
#define _lzss_h 1

#include "compression/abs_agent.h"

class lzss : public compress_agent
{

public:
   lzss() : compress_agent(DICT_AGENT_CODE) {};
   virtual ~lzss() {};

   virtual void compress(const buffer& uncompressed, buffer& compressed) ;
   virtual void decompress(buffer& compressed, buffer& uncompressed) ;

   MMDB_SIGNATURES(lzss);

   virtual io_status build_dict(lex_func_t f_lex, getchar_func_t f_getchar);
};

extern lzss g_lzss_agent;

#endif
