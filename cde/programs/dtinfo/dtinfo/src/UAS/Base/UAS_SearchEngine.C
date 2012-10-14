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
// $XConsortium: UAS_SearchEngine.cc /main/4 1996/06/11 16:39:22 cde-hal $
# include "UAS_Exceptions.hh"
#include "UAS_SearchEngine.hh"
#include "UAS_Collection.hh"
#include "UAS_OQLParser.hh"

#define CLASS UAS_SearchEngine
STATIC_SENDER_CC (UAS_SearchMsg);

UAS_SearchEngine::UAS_SearchEngine () : f_oql_parser(NULL) {
}

UAS_SearchEngine::~UAS_SearchEngine () {
    if (f_oql_parser != NULL) {
	delete f_oql_parser;
	f_oql_parser = NULL;
    }
}

unsigned int
UAS_SearchEngine::avail_caps()
{
    unsigned int caps;

    if (f_oql_parser) {
	caps = f_oql_parser->determine_caps();
    }
    else // let query editor allow everything
	caps = (unsigned int)-1;

    return caps;
}

UAS_Pointer<UAS_SearchResults>
UAS_SearchEngine::search (UAS_String, UAS_SearchScope &, unsigned int) {
    return (UAS_SearchResults *) 0;
}
