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

