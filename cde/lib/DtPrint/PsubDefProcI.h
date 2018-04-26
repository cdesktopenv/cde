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
/* $XConsortium: PsubDefProcI.h /main/10 1996/11/01 15:00:46 cde-hp $ */
/*
 * DtPrint/PsubDefProcI.h
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _DtPsubDefProcI_h
#define _DtPsubDefProcI_h

#include <Dt/Print.h>
#include <Dt/PrintMsgsP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ------------------------------------------------------------------------
 * Private Function Declarations
 *
 */
extern void _DtPrintDefProcInitialize(Widget w);
extern void _DtPrintDefProcManageErrorBox(DtPrintDefaultProcData* dpd);

/*
 * Procedure Resource Defaults
 */
extern XtEnum _DtPrintSetupBoxXPrinterInfoProc(
					       Widget w,
					       DtPrintSetupData* print_data);
extern XtEnum _DtPrintSetupBoxSelectFileProc(
					     Widget w,
					     DtPrintSetupData* print_data);
extern XtEnum _DtPrintSetupBoxSelectXPrinterProc(
						 Widget w,
						 DtPrintSetupData* print_data);
extern XtEnum _DtPrintSetupBoxVerifyXPrinterProc(
						 Widget w,
						 DtPrintSetupData* print_data);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtPsubDefProcI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
