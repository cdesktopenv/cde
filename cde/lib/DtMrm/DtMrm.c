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
 *		     Common Desktop Environment
 *
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc.
 * (c) Copyright 1995 Digital Equipment Corp.
 * (c) Copyright 1995 Fujitsu Limited
 * (c) Copyright 1995 Hitachi, Ltd.
 *                                                                 
 *
 *                   RESTRICTED RIGHTS LEGEND                              
 *
 * Use, duplication, or disclosure by the U.S. Government is subject to
 * restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 * for non-DOD U.S. Government Departments and Agencies are as set forth in
 * FAR 52.227-19(c)(1,2).
 * 
 * Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 * International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 * Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 * Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 * Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 * Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 * Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */
/*                                                                      *
 * (c) Copyright 1996 Hewlett-Packard Company                           *
 * (c) Copyright 1996 International Business Machines Corp.             *
 * (c) Copyright 1996 Sun Microsystems, Inc.                            *
 * (c) Copyright 1996 Novell, Inc.                                      *
 * (c) Copyright 1989, 1990, 1996 Digital Equipment Corporation.	*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: DtMrm.c /main/4 1999/10/14 13:35:44 mgreess $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	This contains only the top-level routine DtMrmIntialize. It can be 
 *	modified as needed to add or remove widgets being initialized for
 *	URM facilities. This routine is normally accessible to
 *	and used by an application at runtime to access URM facilities.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */
#if defined(__hpux)
#include <sys/param.h>
#endif

#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>

#include <Dt/Editor.h>
#include <Dt/EditorP.h>
#include <Dt/Help.h>
#include <Dt/HelpP.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpDialogP.h>
#include <Dt/HelpQuickD.h>
#include <Dt/HelpQuickDP.h>
#include <Dt/MenuButton.h>
#include <Dt/MenuButtonP.h>
#include <Dt/Mrm.h>
#include <Dt/Print.h>
#include <Dt/PrintSetupBP.h>
#include <Dt/PrintSetupBP.h>
#include <Dt/Term.h>
/* work around conflict with <Xm/TextP.h> */
#ifdef PreUnder
#undef PreUnder
#endif
#include <Dt/TermP.h>

/********    Conditionally defined macros for thread_safe DtTerm ******/
#ifdef XTHREADS
#define _MrmProcessLock() XtProcessLock()
#define _MrmProcessUnlock() XtProcessUnlock()
#else /* XTHREADS */
#define _MrmProcessLock()
#define _MrmProcessUnlock()
#endif /* XTHREADS */

/*
 *
 *  TABLE OF CONTENTS
 *
 *	DtMrmInitialize			Initialize URM internals before use
 *
 */

/*
 * The following flag is set to indicate successful URM initialization
 */
static	Boolean	urm__initialize_complete = FALSE;


void DtMrmInitialize (void)

/*
 *++
 *  PROCEDURE DESCRIPTION:
 *
 *	DtMrmInitialize must be called in order to prepare an application to
 *	use URM widget fetching facilities for CDE Dt widgets.  It is 
 *	analogous to the MrmInitialize routine for Motif widgets.
 *
 *	DtMrmInitialize initializes the internal data
 *	structures (creating the mapping from class codes to the creation
 *	routine for each builtin widget class) which URM needs in order to 
 *	successfully perform type conversion on arguments, and successfully 
 *	access widget creation facilities. DtMrmInitialize must be called before
 *	any widgets are	created, whether by URM's fetch mechanisms or directly
 *	by the application. It may be called before or after XtInitialize, and
 *	multiple calls after the first one are benign (no-ops).
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{
    _MrmProcessLock();
    /*
     * Initialize only once
     */
    if (urm__initialize_complete) 
    {
        _MrmProcessUnlock();
        return;
    }
 
    /*
     * Initialize the class descriptors for all the known widgets.
     */
    MrmRegisterClass(
		0, NULL,
		"DtCreateEditor", DtCreateEditor,
        	(WidgetClass) &dtEditorClassRec
		);

    MrmRegisterClass(
		0, NULL,
		"DtCreateHelpDialog", DtCreateHelpDialog,
        	(WidgetClass) &dtHelpDialogWidgetClassRec
		);

    MrmRegisterClass(
		0, NULL,
		"DtCreateHelpQuickDialog", DtCreateHelpQuickDialog,
        	(WidgetClass) &dtHelpQuickDialogWidgetClassRec
		);

    MrmRegisterClass(
		0, NULL,
		"DtCreateMenuButton", DtCreateMenuButton,
        	(WidgetClass) &dtMenuButtonClassRec
		);

    MrmRegisterClass(
		0, NULL,
		"DtCreatePrintSetupBox", DtCreatePrintSetupBox,
        	(WidgetClass) &dtPrintSetupBoxClassRec
		);

    MrmRegisterClass(
		0, NULL,
		"DtCreatePrintSetupDialog", DtCreatePrintSetupDialog,
        	(WidgetClass) &dtPrintSetupBoxClassRec
		);

    MrmRegisterClass(
		0, NULL,
		"DtCreateTerm", DtCreateTerm,
        	(WidgetClass) &dtTermClassRec
		);

    /*
     * Initialization complete
     */
    urm__initialize_complete = TRUE;
    _MrmProcessUnlock();
    return;
}
