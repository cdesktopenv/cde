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
/* $XConsortium: PrintOptionsP.h /main/2 1996/03/12 09:28:04 drk $ */

#ifndef _DT_PRINT_OPTIONSP_H
#define _DT_PRINT_OPTIONSP_H

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
                                                                   
		     Common Desktop Environment

   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
   (c) Copyright 1993, 1994, 1995 Novell, Inc.
   (c) Copyright 1995 Digital Equipment Corp.
   (c) Copyright 1995 Fujitsu Limited
   (c) Copyright 1995 Hitachi, Ltd.
                                                                   

                     RESTRICTED RIGHTS LEGEND                              

Use, duplication, or disclosure by the U.S. Government is subject to
restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
for non-DOD U.S. Government Departments and Agencies are as set forth in
FAR 52.227-19(c)(1,2).

Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
Digital Equipment Corp., 111 Powdermill Road, Maynard, Massachusetts 01754, U.S.A.
Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Message Catalog Lookup
 */
#define DTPO_MARGIN_FRAME_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_MARGINS, _DtPrMsgPrintOption_0000)
#define DTPO_MARGIN_FRAME_TOP_MARGIN_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_TOP, _DtPrMsgPrintOption_0001)
#define DTPO_MARGIN_FRAME_RIGHT_MARGIN_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_RIGHT, _DtPrMsgPrintOption_0002)
#define DTPO_MARGIN_FRAME_BOTTOM_MARGIN_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_BOTTOM, _DtPrMsgPrintOption_0003)
#define DTPO_MARGIN_FRAME_LEFT_MARGIN_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_LEFT, _DtPrMsgPrintOption_0004)

#define DTPO_HEADERFOOTER_FRAME_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_HEADERS_N_FOOTERS, _DtPrMsgPrintOption_0005)
#define DTPO_HEADERFOOTER_FRAME_TOP_LEFT_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_TOP_LEFT, _DtPrMsgPrintOption_0006)
#define DTPO_HEADERFOOTER_FRAME_TOP_RIGHT_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_TOP_RIGHT, _DtPrMsgPrintOption_0007)
#define DTPO_HEADERFOOTER_FRAME_BOTTOM_LEFT_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_BOTTOM_LEFT, _DtPrMsgPrintOption_0008)
#define DTPO_HEADERFOOTER_FRAME_BOTTOM_RIGHT_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintOptionsSet, DTPO_BOTTOM_RIGHT, _DtPrMsgPrintOption_0009)
    

/*
 * Header/Footer Frame API
 */
typedef enum {
    DTPRINT_OPTION_HEADER_LEFT,
    DTPRINT_OPTION_HEADER_RIGHT,
    DTPRINT_OPTION_FOOTER_LEFT,
    DTPRINT_OPTION_FOOTER_RIGHT
} _DtPrintHdrFtrEnum;

extern Widget _DtPrintCreateHdrFtrFrame(
	Widget			parent,
	int			nspecs,
	char			**spec_strings,
	void			**spec_data
	);

extern Widget _DtPrintHdrFtrFrameEnumToWidget(
	Widget			frame,
	_DtPrintHdrFtrEnum	which
	);

extern void _DtPrintHdrFtrFrameMenuWidgets(
	Widget			frame,
	Widget			*menu,
	int			*nmenu_buttons,
	Widget			**menu_buttons
	);

extern void *_DtPrintGetHdrFtrData(
	Widget			widget
	);

extern int _DtPrintGetHdrFtrIndex(
	Widget			widget
	);

extern char *_DtPrintGetHdrFtrString(
	Widget			widget
	);

extern void _DtPrintSetHdrFtrByData(
	Widget			widget,
	void			*data
	);

extern void _DtPrintSetHdrFtrByIndex(
	Widget			widget,
	int			index
	);

extern void _DtPrintSetHdrFtrByString(
	Widget			widget,
	char			*string
	);


/*
 * Margin Frame API
 */
typedef enum {
    DTPRINT_OPTION_MARGIN_TOP,
    DTPRINT_OPTION_MARGIN_RIGHT,
    DTPRINT_OPTION_MARGIN_BOTTOM,
    DTPRINT_OPTION_MARGIN_LEFT
} _DtPrintMarginEnum;

extern Widget _DtPrintCreateMarginFrame(
	Widget			parent
	);

extern Widget _DtPrintMarginFrameEnumToWidget(
	Widget			frame,
	_DtPrintMarginEnum	which
	);

extern char* _DtPrintGetMarginSpec(
	Widget			margin
	);

extern void _DtPrintSetMarginSpec(
	Widget			margin,
	char			*spec
	);



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DT_PRINT_OPTIONSP_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
