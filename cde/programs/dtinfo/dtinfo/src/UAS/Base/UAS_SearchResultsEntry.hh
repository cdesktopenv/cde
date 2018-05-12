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
// $XConsortium: UAS_SearchResultsEntry.hh /main/3 1996/06/11 16:39:58 cde-hal $
#ifndef _UAS_SearchResultsEntry_hh_
#define _UAS_SearchResultsEntry_hh_

#include "UAS_Base.hh"
#include "UAS_Pointer.hh"
#include "UAS_Common.hh"
#include "UAS_List.hh"
#include "UAS_TextRun.hh"

class UAS_SearchResultsEntry : public UAS_Base {
  public:
    UAS_SearchResultsEntry (const char *, const char *, const char *,
			    unsigned int);

    virtual ~UAS_SearchResultsEntry ();

    virtual UAS_Pointer<UAS_List<UAS_TextRun> > matches () const
        { return f_matches; }
    virtual UAS_Pointer<UAS_List<UAS_TextRun> > create_matches ()
        { return NULL; }

    UAS_Pointer<UAS_Common> document ();

    UAS_String id() const		{ return f_id; }
    UAS_String book() const		{ return f_book; }
    UAS_String section() const		{ return f_section; }
    unsigned int relevance () const	{ return f_relevance; }

  protected:
    UAS_Pointer<UAS_Common> f_document;
    UAS_Pointer<UAS_List<UAS_TextRun> > f_matches;

    UAS_String		f_id;
    UAS_String		f_book;
    UAS_String		f_section;
    unsigned int	f_relevance;
};

#endif
