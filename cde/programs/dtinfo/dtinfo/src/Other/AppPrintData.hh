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
// $XConsortium: AppPrintData.hh /main/7 1996/07/26 11:33:03 rcs $

/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _AppPrintData_hh
#define _AppPrintData_hh

#include <Xm/XmAll.h>
#if 0 && defined(PRINTING_SUPPORTED)
#include <Xm/Print.h>
#endif /* PRINTING_SUPPORTED */
#include <Dt/Print.h>

#include "UAS.hh"

template <class T> class xList;

class AppPrintData
{
  public:  // member data
    Widget f_print_dialog;
    Widget f_print_shell;
    Boolean f_print_only;
    DtPrintSetupData *f_print_data;
    Widget f_pform;
    xList<UAS_Pointer<UAS_Common> > *f_print_list;
    OutlineElement *f_outline_element;  
    Widget f_pshell_parent;  // widget to parent print shell to

  public:  // functions
  
    // Constructing
    AppPrintData();
  
    // Destructing 
    ~AppPrintData();		
  
    // Accessing

    Widget print_dialog() { return f_print_dialog; };
    Widget print_shell() { return f_print_shell; };
    Boolean  	print_only() { return f_print_only; } 
    DtPrintSetupData * print_data() { return f_print_data; };
    xList<UAS_Pointer<UAS_Common> > * print_list() { return f_print_list; };

};

#endif
