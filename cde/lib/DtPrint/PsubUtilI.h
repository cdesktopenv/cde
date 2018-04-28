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
/* $XConsortium: PsubUtilI.h /main/3 1996/08/12 18:41:41 cde-hp $ */
/*
 * DtPrint/PsubUtilI.h
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
#ifndef _DtPsubUtilI_h
#define _DtPsubUtilI_h

#include <Dt/Print.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ------------------------------------------------------------------------
 * Type Definitions
 *
 */
typedef enum {
    DtPRINT_DEC_NET,
    DtPRINT_TCP_IPC,
    DtPRINT_NET_UNSPECIFIED
} DtPrintSpecNet;

/*
 * ------------------------------------------------------------------------
 * Private Function Declarations
 *
 */
String _DtPrintCreateXPrinterSpecifier(
				       String printer_name,
				       String host_name,
				       DtPrintSpecNet spec_net,
				       int display_num,
				       int screen_num);
void _DtPrintFreeStringList(
			    String* string_list);
String _DtPrintGetDefaultXPrinterName(
				      Widget w);
String* _DtPrintGetXpPrinterList(
				 Widget w);
String* _DtPrintGetXpServerList(
				Widget w);
void _DtPrintParseXDisplaySpecifier(
				    const String display_spec,
				    String* host_name,
				    DtPrintSpecNet* spec_net,
				    int* display_num,
				    int* screen_num);
void _DtPrintParseXPrinterSpecifier(
				    const String specifier,
				    String* printer_name,
				    String* display_spec);
XtEnum _DtPrintVerifyXPrinter(
			      Widget w,
			      String printer_spec,
			      String* new_printer_spec,
			      Display** new_display
#if 0 && defined(PRINTING_SUPPORTED)
                              ,XPContext* new_context
#endif /* PRINTING_SUPPORTED */
                              );



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtPsubUtilI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
