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
/* $XConsortium: AusText.h /main/2 1996/07/18 16:38:54 drk $ */
// AusText.h - Specific implementation for AusText search engine

#ifndef AUSTEXT_HEADER
#define AUSTEXT_HEADER

#include "SearchEng.h"

class Token;
class AusTextStore;
class NodeData;
class DataRepository;

class AusText : public SearchEngine {

private:
  DataRepository *store;
  AusTextStore   *f_search_store;

protected:
  void write_start_tag ( const Token &);
  void write_end_tag ( const Token & );
  void write_terms ( FlexBuffer *termsbuf );
  void write_buffer();

public:
  void markup( const Token & );
  void data( const char *, size_t );

public:
  AusText(NodeData *, const Token &);
  ~AusText();

};

#endif
