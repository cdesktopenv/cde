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
/* $TOG: DmxPrintOutput.h /main/3 1997/07/07 13:58:26 mgreess $ */

#ifndef _DMX_PRINT_OUTPUT_H
#define _DMX_PRINT_OUTPUT_H

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

#include <Dt/PrintOptionsP.h>
#include "DtMailTypes.h"
#include "UIComponent.h"

class DmxPrintOutput : public UIComponent
{
public:

    DmxPrintOutput ( Widget );
    ~DmxPrintOutput (void);

    void		hideFooters (void);
    void		showFooters (void);
    void		hideHeaders (void);
    void		showHeaders (void);
    void		setHdrFtrString (_DtPrintHdrFtrEnum, char*);
    void		setHdrFtrStrings (char *, char *, char *, char *);

    void		setPageMargin (
					_DtPrintMarginEnum,
					const char*,
					DtMailBoolean*
					);
    void		setPageMargin (_DtPrintMarginEnum, int);
    void		setPageMargins (int, int, int, int);
    void		setPageMargins (
					const char*,
					const char*,
					const char*,
					const char*,
					DtMailBoolean*
					 );

    void		setWrapToFit (DtMailBoolean);

    int			getCharactersPerLine(void);
    inline int		getLinesPerPage(void) { return _lines_per_page; }
    int			getNumLines(void);
    int			getLastPosition(void);
    int			getTopPosition(void);
    DtMailBoolean	pageUp(void);
    DtMailBoolean	pageDown(void);
    void		setTopPosition(int);

    static void		appendContents(void*, char*);
    void		appendNewLine();
    void		appendPageBreak();
    void		clearContents(void);

private:
    static const char* const
			_default_margin;

    Widget		_pshell;
    Widget		_form;
    Widget		_inner_form;
    Widget		_header_left;
    Widget		_header_right;
    Widget		_editor;
    Widget		_footer_left;
    Widget		_footer_right;

    int			_lines_per_page;
    Dimension		_margin_top;
    Dimension		_margin_right;
    Dimension		_margin_bottom;
    Dimension		_margin_left;

    int			doGetLinesPerPage();
    void		setInnerPageDimensions(
						Dimension,
						Dimension,
						Dimension,
						Dimension
						);
};

#endif // _DMX_PRINT_OUTPUT_H
