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
#ifndef _MARKINFO_HH_
#define _MARKINFO_HH_

#include "UAS.hh"

class MarkUnitInfo : public UAS_Base
{
  public:

    MarkUnitInfo(unsigned int vcc, unsigned int offset,
		 unsigned int len, void* enclosure);
   ~MarkUnitInfo();

    unsigned int vcc()       { return f_vcc; }
    void vcc(unsigned int n) { f_vcc = n; }

    unsigned int offset()       { return f_offset; }
    void offset(unsigned int n) { f_offset = n; }

    unsigned int length()       { return f_length; }
    void length(unsigned int n) { f_length = n; }

    void* enclosure()       { return f_enclosure; }
    void enclosure(void* v) { f_enclosure = v; }

  private:

    unsigned int f_vcc;    // vcc offset to the segment
    unsigned int f_offset; // internal offset
    unsigned int f_length;

    void* f_enclosure;
};

class MarkInfo : public UAS_List<MarkUnitInfo>
{
  public:
    MarkInfo();
    MarkInfo(UAS_String& name);
    MarkInfo(UAS_String& name, UAS_String& anno);

    UAS_String& name() { return f_name; }
    void name(UAS_String& string) { f_name = string; }

    UAS_String& anno() { return f_annotation; }
    void anno(UAS_String& string) { f_annotation = string; }

  private:
    // I'm not sure how useful these are because this class
    // is just for transient use.
    UAS_String f_name;
    UAS_String f_annotation;
};

#endif
