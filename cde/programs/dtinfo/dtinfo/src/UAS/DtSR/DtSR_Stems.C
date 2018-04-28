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
// $XConsortium: DtSR_Stems.cc /main/3 1996/06/11 16:41:47 cde-hal $

#include <string.h>
#include "DtSR_Stems.hh"

DtSR_Stems::DtSR_Stems(int dbn) : f_dbn(dbn)
{
    clear();
}

int
DtSR_Stems::clear()
{
    size_t size = DtSrMAX_STEMCOUNT * DtSrMAXWIDTH_HWORD;

    void* ret = memset(f_stems, 0x00, size);
    if (ret != (void*)f_stems) // failed
	return 0;
    else {
	f_stemcount = 0;
	return 1;
    }
}
