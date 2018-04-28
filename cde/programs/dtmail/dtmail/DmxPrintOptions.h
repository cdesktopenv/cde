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
/* $XConsortium: DmxPrintOptions.h /main/6 1996/09/23 13:15:43 mgreess $ */

#ifndef _DMX_PRINT_OPTIONS_H
#define _DMX_PRINT_OPTIONS_H

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
#include <DtMail/PropUi.hh>
#include "Dmx.h"
#include "IndexedOptionMenu.h"
#include "UIComponent.h"

#define DMX_PROPKEY_HEADER_LEFT		"headerleft"
#define DMX_PROPKEY_HEADER_RIGHT	"headerright"
#define DMX_PROPKEY_FOOTER_LEFT		"footerleft"
#define DMX_PROPKEY_FOOTER_RIGHT	"footerright"

#define DMX_PROPKEY_MARGIN_TOP		"margintop"
#define DMX_PROPKEY_MARGIN_RIGHT	"marginright"
#define DMX_PROPKEY_MARGIN_BOTTOM	"marginbottom"
#define DMX_PROPKEY_MARGIN_LEFT		"marginleft"

#define DMX_PROPKEY_PRINT_HEADERS	"printheaders"
#define DMX_PROPKEY_MESSAGE_SEPARATOR	"messageseparator"
#define DMX_PROPKEY_SEPARATOR_STRING	"separatorstring"

#define DMX_PROPVAL_EMPTY		"Empty"
#define DMX_PROPVAL_CC_HEADER		"CCHeader"
#define DMX_PROPVAL_DATE_HEADER		"DateHeader"
#define DMX_PROPVAL_FROM_HEADER		"FromHeader"
#define DMX_PROPVAL_SUBJECT_HEADER	"SubjectHeader"
#define DMX_PROPVAL_TO_HEADER		"ToHeader"
#define DMX_PROPVAL_PAGE_NUMBER		"PageNumber"
#define DMX_PROPVAL_USER_NAME		"UserName"

#define DMX_PROPVAL_DFLT_MARGIN		"1.0 in"

#define DMX_PROPVAL_NONE		"None"
#define DMX_PROPVAL_STANDARD		"Standard"
#define DMX_PROPVAL_ABBREVIATED		"Abbreviated"
#define DMX_PROPVAL_ALL			"All"

#define DMX_PROPVAL_NEW_LINE		"NewLine"
#define DMX_PROPVAL_BLANK_LINE		"BlankLine"
#define DMX_PROPVAL_CHARACTER_LINE	"CharacterLine"
#define DMX_PROPVAL_PAGE_BREAK		"PageBreak"
#define DMX_PROPVAL_NEW_JOB		"NewJob"

struct DmxpoPropKey
{
        int		which;
        char		*key;
        char		*dflt_prop_string;
};

struct DmxpoPropValue
{
        int		which;
        char		*prop_string;
        int		set_id;
        int		msg_id;
        char		*dflt_string;
};

class DmxPrintOptions : public UIComponent
{

private:
    DtVirtArray<IndexedOptionMenu *>
			*_iom_array;
    DtVirtArray<PropUiItem *>
			*_propui_array;
    int			_propui_array_iterator;

    Widget		_parent;
    Widget		_form;

    Widget		_hdrftr_frame;
    Widget		_margin_frame;

    IndexedOptionMenu	*_msgsep_iom;
    IndexedOptionMenu	*_prthdr_iom;

    PropSource		*_prop_source;

    PropUiItem			*propKeyToPropItem(char*);
    static DmxpoPropKey		*enumToPropKey(int, int, DmxpoPropKey*);
    static DmxpoPropValue	*stringToPropValue(
						const char*,
						int,
						DmxpoPropValue*);
    static char			*isValidMarginSpec(
						PropUiItem*,
						void*);
public:

    DmxPrintOptions ( Widget );
    ~DmxPrintOptions (void);

    PropUiItem			*getFirstProp(void);
    PropUiItem			*getNextProp(void);
    int				getNumProps(void);

    static DmxStringTypeEnum	getHdrFtrSpec(_DtPrintHdrFtrEnum which);
    static const char		*getMarginSpec(_DtPrintMarginEnum which);
    static DmxMsgSeparatorEnum	getMessageSeparator(void);
    static DmxPrintHeadersEnum	getPrintedHeaders(void);
    static const char		*getSeparatorString(void);
};

#endif // _DMX_PRINT_OPTIONS_H
