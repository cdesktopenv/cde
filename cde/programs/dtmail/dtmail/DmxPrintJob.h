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
/* $XConsortium: DmxPrintJob.h /main/3 1996/04/12 14:12:32 mgreess $ */

#ifndef _DMX_PRINT_JOB_H
#define _DMX_PRINT_JOB_H

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
#include "Dmx.h"
#include "DmxPrintOptions.h"
#include "DmxPrintOutput.h"
#include "DmxPrintSetup.h"
#include "DtMailTypes.h"
#include "MainWindow.h"
#include "OptCmd.h"
#include "UIComponent.h"

typedef struct _dmx_msg_info 
{
    int		end_position;
    DmxMsg	*msg;
} DmxMsgInfo;

class DmxPrintJob : public UIComponent
{
    private:
	DmxMsg			*_next_msg;
        char			*_filename;
	DmxMailbox		*_mailbox;
        MainWindow		*_parent;
        DmxPrintOutput		*_print_output;
        DmxPrintSetup		*_print_setup;
	DtPrintSetupData	*_print_data;
	Widget			_pshell;
        DtMailBoolean		_silent;

	DmxMsgInfo		*_spool_msg_info;
        int			_spool_nmsgs_done;
        int			_spool_nmsgs_total;
        int			_spool_npages_done;
        int			_spool_npages_total;

#ifndef USE_XP_SERVER
    	Widget			_nextpage_shell;
    	Widget			_nextpage_button;
#endif
	static void	cancelCB (Widget, XtPointer, XtPointer);
	static void	closeDisplayCB (Widget, XtPointer, XtPointer);
	static void	pdmNotificationCB (Widget, XtPointer, XtPointer);
	static void	pdmSetupCB (Widget, XtPointer, XtPointer);
	static void	printCB (Widget, XtPointer, XtPointer);
	static void	printOnePageCB (Widget, XtPointer, XtPointer);

	void		createPrintShell (void);
	void		createOutputWidgets (void);
	void		doPrint (void);
#if 0 && defined(PRINTING_SUPPORTED)
	static void	finishedPrintToFile(
					Display*,
					XPContext,
					XPGetDocStatus,
					XPointer);
#endif /* PRINTING_SUPPORTED */
	char *		getPageHeaderString( DmxMsg*, DmxStringTypeEnum);
	Boolean		loadOutputWidgets (void);
	void		updatePageHeaders(
					DmxMsg*,
					DmxStringTypeEnum,
					DmxStringTypeEnum,
					DmxStringTypeEnum,
					DmxStringTypeEnum);
    public:
	DmxPrintJob (char*, DtMailBoolean, MainWindow*);
	~DmxPrintJob (void);

	void		cancel (void);
	void		execute (void);
};

#endif // _DMX_PRINT_JOB_H
