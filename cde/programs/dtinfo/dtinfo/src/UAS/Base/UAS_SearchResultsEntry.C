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
// $XConsortium: UAS_SearchResultsEntry.cc /main/3 1996/06/11 16:39:43 cde-hal $
# include "UAS_SearchResultsEntry.hh"
# include "UAS_Exceptions.hh"
# include "UAS_Collection.hh"

UAS_SearchResultsEntry::UAS_SearchResultsEntry (const char *id,
                                                const char *book,
                                                const char *section,
                                                unsigned int relevance)
{
    f_matches = NULL;
    f_document = NULL;
    f_id = id;
    f_book = book;
    f_section = section;
    f_relevance = relevance;
}

UAS_SearchResultsEntry::~UAS_SearchResultsEntry ()
{
}

UAS_Pointer<UAS_Common>
UAS_SearchResultsEntry::document ()
{
    return NULL;
}


