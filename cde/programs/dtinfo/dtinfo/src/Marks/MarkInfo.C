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
/* $XConsortium: MarkInfo.C /main/3 1996/11/11 11:09:35 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include "MarkInfo.hh"

MarkUnitInfo::MarkUnitInfo(unsigned int vcc, unsigned int offset,
			   unsigned int len, void* enclosure)
    : f_vcc(vcc), f_offset(offset), f_length(len), f_enclosure(enclosure)
{}

MarkUnitInfo::~MarkUnitInfo()
{
    // do not delete f_enclosure, since it's a property of the caller
}

MarkInfo::MarkInfo()
{}

MarkInfo::MarkInfo(UAS_String& name) : f_name(name)
{}

MarkInfo::MarkInfo(UAS_String& name, UAS_String& anno) : f_name(name),
							 f_annotation(anno)
{}

