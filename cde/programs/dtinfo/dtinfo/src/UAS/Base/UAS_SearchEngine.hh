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
// $XConsortium: UAS_SearchEngine.hh /main/4 1996/06/11 16:39:27 cde-hal $
#ifndef _UAS_SearchEngine_hh_
#define _UAS_SearchEngine_hh_

#include "UAS_Base.hh"
#include "UAS_Pointer.hh"
#include "UAS_String.hh"
#include "UAS_Common.hh"
#include "UAS_SearchResults.hh"
#include "UAS_SearchScope.hh"
#include "UAS_Msgs.hh"

class UAS_OQLParser;

class UAS_SearchEngine : public UAS_Base {
  public:
    UAS_SearchEngine ();

    virtual ~UAS_SearchEngine ();

    virtual UAS_Pointer<UAS_SearchResults> search (UAS_String,
		   UAS_SearchScope &,
		   // NOTE: Might beef this
		   // up later.
		   unsigned int maxDocs = 2000);

    STATIC_SENDER_HH (UAS_SearchMsg);

    unsigned int avail_caps();

  protected:
    // Each implementation must set f_oql_parser, otherwise avail_caps
    // would fail.
    UAS_OQLParser *f_oql_parser;

};

#endif
