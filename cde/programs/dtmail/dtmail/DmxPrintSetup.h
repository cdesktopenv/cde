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
/* $TOG: DmxPrintSetup.h /main/9 1997/08/14 15:53:59 mgreess $ */

#ifndef _DMX_PRINT_SETUP_H
#define _DMX_PRINT_SETUP_H

/*
 *+SNOTICE
 *
 *	$:$
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <Dt/Print.h>
#include "UIComponent.h"

class DmxPrintSetup
{
private:
typedef struct print_setup_widgets
{
    //
    // Widgets from the dtmail-specific portion of the PrintSetup Dialog.
    //
    Widget		dtprint_setup;
    Widget		form;
    Widget		print_separately_tb;
    Widget		use_word_wrap_tb;
    Widget		more_options_pb;

    //
    // Widgets from the generic portion of the PrintSetup Dialog.
    //
    Widget		printer_name_tf;
    Widget		checkbox_rc;
    Widget		checkbox_tb;
    Widget		filename_tf;
} PrintSetupWidgets;

    Widget		_parent;
    Widget		_dtprint_setup;
    PrintSetupWidgets	*_widgets;

    XtCallbackProc	_printCB;
    XtCallbackProc	_cancelCB;
    XtCallbackProc	_closeDisplayCB;
    XtCallbackProc	_pdmSetupCB;

    XtPointer		_printClosure;
    XtPointer		_cancelClosure;
    XtPointer		_closeDisplayClosure;
    XtPointer		_pdmSetupClosure;

    // Print options specific to this print job.
    DtMailBoolean	_print_separately;
    DtMailBoolean	_use_word_wrap;
    DtMailBoolean	_print_to_file;
    char		*_printer_name;
    char		*_filename;

    void		attachPrintSetupDialog(void);
    Widget		createPrintSetupDialog(Widget);
    void		detachPrintSetupDialog(void);
    void		savePrintSetupOptions(DtPrintSetupData*);

    static void		destinationChangedCB(Widget, XtPointer, XtPointer);
    static void		destroyPrintSetupDialogCB(Widget, XtPointer, XtPointer);
    static void		moreOptionsCB(Widget, XtPointer, XtPointer);
    static void		printCB(Widget, XtPointer, XtPointer);

public:
    DmxPrintSetup (
			Widget,
			XtCallbackProc, XtPointer,
			XtCallbackProc, XtPointer,
			XtCallbackProc, XtPointer,
			XtCallbackProc, XtPointer
	          );
    ~DmxPrintSetup (void);

    void		setPrintToFileName (char*);
    void		display(void);
    DtMailBoolean	getDefaultPrintData(DtPrintSetupData*);
    DtMailBoolean	printSeparately(void);
    DtMailBoolean	useWordWrap(void);
};

#endif // _DMX_PRINT_SETUP_H
