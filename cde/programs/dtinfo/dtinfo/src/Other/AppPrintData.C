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
/*
 *  $XConsortium: AppPrintData.cc /main/1 1996/04/22 17:41:23 rcs $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#define C_WindowGeometryPref
#define L_Preferences

#define C_WindowSystem
#define L_Other

#define C_ServiceMgr
#define C_MessageMgr
#define L_Managers

#define C_TOC_Element
#define L_Basic

#define USES_OLIAS_FONT

#include "Prelude.h"

#include "Exceptions.hh"

#include <unistd.h>

//--------- Constructors ----------------------------------

AppPrintData::AppPrintData()
{
    f_print_data = (DtPrintSetupData*)XtCalloc(1, sizeof(DtPrintSetupData));
    f_pform = NULL ;
    f_print_dialog = NULL;
    f_print_shell = NULL;
    f_print_only = FALSE;
    f_outline_element = NULL;
    f_print_list = NULL;
    f_pshell_parent = NULL;
};

//--------- Destructors ----------------------------------


AppPrintData::~AppPrintData()
{
    XtFree((char *)f_print_data);
};
