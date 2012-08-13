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
/*
 *+SNOTICE
 *
 *
 *	$XConsortium: NotDynamic.C /main/4 1996/04/21 19:48:35 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdio.h>
#include <unistd.h>

#include "NotDynamic.hh"

NotDynamic::NotDynamic(const char * path)
: DynamicLib(path)
{
}

NotDynamic::~NotDynamic(void)
{
}

struct SymTable {
    const char *	symbol;
    void *		entry;
};

#include <DtMail/DtMail.hh>

extern "C" void * RFCMetaFactory(const char * op);
extern "C" void * V3MetaFactory(const char * op);

static const SymTable symbol_table[] = {
{ "RFCMetaFactory",	(void *)RFCMetaFactory },
{ "V3MetaFactory",	(void *)V3MetaFactory },
{ NULL,			NULL }
};

void *
NotDynamic::getSym(const char * sym)
{
    for (int i = 0; symbol_table[i].symbol; i++) {
	if (strcmp(symbol_table[i].symbol, sym) == 0) {
	    return(symbol_table[i].entry);
	}
    }

    return(NULL);
}
