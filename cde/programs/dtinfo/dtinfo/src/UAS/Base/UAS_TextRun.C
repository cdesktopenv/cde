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
// $XConsortium: UAS_TextRun.cc /main/3 1996/06/11 16:40:55 cde-hal $
#include "UAS_TextRun.hh"
#include "UAS_Exceptions.hh"
#include "UAS_SearchEngine.hh"
#include "UAS_Collection.hh"

UAS_TextRun::UAS_TextRun (unsigned int off, unsigned int len)
:f_offset(off), f_length(len)
{
}

UAS_TextRun::~UAS_TextRun ()
{
}

UAS_Pointer<UAS_Common>
UAS_TextRun::document()
{
    return NULL;
}

UAS_RangeType
UAS_TextRun::type()
{
    return WithoutTags;
}
