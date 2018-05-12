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
// $XConsortium: DtSR_Stems.hh /main/3 1996/06/11 16:41:52 cde-hal $
#ifndef _DTSR_STEMS_HH_
#define _DTSR_STEMS_HH_

extern "C" {
#include "Search.h"
}

#include "UAS_Base.hh"

typedef char Stems[DtSrMAX_STEMCOUNT][DtSrMAXWIDTH_HWORD];

class DtSR_Stems : public UAS_Base
{
  public:
    DtSR_Stems(int dbn);

    Stems& stems() { return f_stems; }
    int& count() { return f_stemcount; }

    int clear();

    int dbn() { return f_dbn; }

  private:
    Stems f_stems;    
    int   f_stemcount;

    int   f_dbn;

};

#endif
