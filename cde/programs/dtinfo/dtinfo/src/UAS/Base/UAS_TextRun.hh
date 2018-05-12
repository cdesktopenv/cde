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
// $XConsortium: UAS_TextRun.hh /main/3 1996/06/11 16:41:01 cde-hal $
#ifndef _UAS_TextRun_hh_
#define _UAS_TextRun_hh_

#include "UAS_Base.hh"
#include "UAS_Pointer.hh"
#include "UAS_Common.hh"

enum UAS_RangeType { WithTags, WithoutTags };

class UAS_TextRun : public UAS_Base {
  public:
    UAS_TextRun (unsigned int off, unsigned int len);

    virtual ~UAS_TextRun ();

    UAS_Pointer<UAS_Common> document();

    unsigned int offset () const { return f_offset; }
    unsigned int length () const { return f_length; }

    virtual UAS_RangeType  type();

  private:
    UAS_Pointer<UAS_Common> f_document;
    unsigned int f_offset;
    unsigned int f_length;
};

#endif
