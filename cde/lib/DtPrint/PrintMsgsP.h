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
/* $XConsortium: PrintMsgsP.h /main/9 1996/08/19 17:12:12 cde-hp $ */
/*
 * DtPrint/PrintMsgsP.h
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
#ifndef _DtPrintMsgsP_h
#define _DtPrintMsgsP_h

#ifdef I18N_MSG
#include <nl_types.h>
#define DTPRINT_GETMESSAGE(set, number, string) \
    _DtPrintGetMessage(set, number, string)

#else /* I18N_MSG */
#define DTPRINT_GETMESSAGE(set, number, string) string
#endif /* I18N_MSG */

/*** const causes the HP compiler to complain.  Remove reference until   ***
 *** the build and integration people can figure out how to get this to  ***
 *** work.                                                               ***/

/* #define CONST	const */
#define CONST


/* 
 * extern declarations for PrintSetupBox widget messages
 */
extern CONST char _DtPrMsgPrintSetupBox_0000[];
extern CONST char _DtPrMsgPrintSetupBox_0001[];
extern CONST char _DtPrMsgPrintSetupBox_0002[];
extern CONST char _DtPrMsgPrintSetupBox_0003[];
extern CONST char _DtPrMsgPrintSetupBox_0004[];
extern CONST char _DtPrMsgPrintSetupBox_0005[];
extern CONST char _DtPrMsgPrintSetupBox_0006[];
extern CONST char _DtPrMsgPrintSetupBox_0007[];
extern CONST char _DtPrMsgPrintSetupBox_0008[];
extern CONST char _DtPrMsgPrintSetupBox_0009[];
extern CONST char _DtPrMsgPrintSetupBox_0010[];
extern CONST char _DtPrMsgPrintSetupBox_0011[];
extern CONST char _DtPrMsgPrintSetupBox_0012[];
extern CONST char _DtPrMsgPrintSetupBox_0013[];
extern CONST char _DtPrMsgPrintSetupBox_0014[];
extern CONST char _DtPrMsgPrintSetupBox_0015[];
extern CONST char _DtPrMsgPrintSetupBox_0016[];
extern CONST char _DtPrMsgPrintSetupBox_0017[];
extern CONST char _DtPrMsgPrintSetupBox_0018[];
extern CONST char _DtPrMsgPrintSetupBox_0019[];
extern CONST char _DtPrMsgPrintSetupBox_0020[];
extern CONST char _DtPrMsgPrintSetupBox_0021[];
extern CONST char _DtPrMsgPrintSetupBox_0022[];
extern CONST char _DtPrMsgPrintSetupBox_0023[];
extern CONST char _DtPrMsgPrintSetupBox_0024[];
extern CONST char _DtPrMsgPrintSetupBox_0025[];
extern CONST char _DtPrMsgPrintSetupBox_0026[];
extern CONST char _DtPrMsgPrintSetupBox_0027[];
extern CONST char _DtPrMsgPrintSetupBox_0028[];
extern CONST char _DtPrMsgPrintSetupBox_0029[];
extern CONST char _DtPrMsgPrintSetupBox_0030[];
extern CONST char _DtPrMsgPrintSetupBox_0031[];
extern CONST char _DtPrMsgPrintSetupBox_0032[];
extern CONST char _DtPrMsgPrintSetupBox_0033[];
extern CONST char _DtPrMsgPrintSetupBox_0034[];
extern CONST char _DtPrMsgPrintSetupBox_0035[];
extern CONST char _DtPrMsgPrintSetupBox_0036[];

/* 
 * Message set for PrintSetupBox widget
 */
#define	MS_PrintSetupBox		1

/* Message IDs for PrintSetupBox */
#define PSUB_WARN_WA_LOCATION		1
#define PSUB_SELECT_PRINTER		2
#define PSUB_PRINT_TO_FILE		3
#define PSUB_SELECT_FILE		4
#define PSUB_CANCEL			5
#define PSUB_COPIES			6
#define PSUB_HELP			7
#define PSUB_PRINT			8
#define PSUB_PRINTER_NAME		9
#define PSUB_SETUP			10
#define PSUB_DESCRIPTION		11
#define PSUB_WARN_COPY_COUNT		12
#define PSUB_WARN_SETUP_MODE		13
#define PSUB_WARN_SETUP_MODE_CHANGE	14
#define PSUB_PRINTER_INFO		15
#define PSUB_INVALID_PRINTER_TITLE	16
#define PSUB_INVALID_PRINTER_MESSAGE	17
#define PSUB_INVALID_DISPLAY_MESSAGE	18
#define PSUB_NOT_XP_DISPLAY_MESSAGE	19
#define PSUB_NO_DEFAULT_MESSAGE		20
#define PSUB_NO_DEFAULT_DISPLAY_MESSAGE	21
#define PSUB_PRINTER_MISSING_MESSAGE	22
#define PSUB_WARN_PRINT_DESTINATION	23
#define PSUB_WARN_CT_CONVERSION		24
#define PSUB_WARN_CONVERSION_ARGS	25
#define PSUB_PRLIST_ERROR_TITLE		26
#define PSUB_NO_PRINTERS_MESSAGE	27
#define PSUB_DESC_UNAVAILABLE		28
#define PSUB_SELECT_FILE_TITLE		29
#define PSUB_PRINTER_INFO_TITLE		30
#define PSUB_FORMAT_LABEL		31
#define PSUB_MODEL_LABEL		32
#define PSUB_PRINTER_LIST_LABEL		33
#define PSUB_MORE_PRINTERS_TITLE	34
#define PSUB_PRINT_TO_PRINTER		35
#define PSUB_FILE_NAME                  36
#define PSUB_HELP_DLG_TITLE             37

/* 
 * extern declarations for _DtPrint frame widgets
 */
extern CONST char _DtPrMsgPrintOption_0000[];
extern CONST char _DtPrMsgPrintOption_0001[];
extern CONST char _DtPrMsgPrintOption_0002[];
extern CONST char _DtPrMsgPrintOption_0003[];
extern CONST char _DtPrMsgPrintOption_0004[];
extern CONST char _DtPrMsgPrintOption_0005[];
extern CONST char _DtPrMsgPrintOption_0006[];
extern CONST char _DtPrMsgPrintOption_0007[];
extern CONST char _DtPrMsgPrintOption_0008[];
extern CONST char _DtPrMsgPrintOption_0009[];
 
/*
 * Message set for _DtPrint frame widgets
 */
#define MS_PrintOptionsSet      2
 
/*
 * Message IDs for _DtPrint frame widgets
 */
#define DTPO_MARGINS            1
#define DTPO_TOP                2
#define DTPO_RIGHT              3
#define DTPO_BOTTOM             4
#define DTPO_LEFT               5
 
#define DTPO_HEADERS_N_FOOTERS  6
#define DTPO_TOP_LEFT           7
#define DTPO_TOP_RIGHT          8
#define DTPO_BOTTOM_LEFT        9
#define DTPO_BOTTOM_RIGHT       10
 
/* 
 * extern declarations for PrintDlgMgr module
 */
extern CONST char _DtPrMsgPrintDlgMgr_0000[];
extern CONST char _DtPrMsgPrintDlgMgr_0001[];
extern CONST char _DtPrMsgPrintDlgMgr_0002[];
 
/*
 * Message set for PrintDlgMgr module
 */
#define MS_PrintDlgMgrSet      3
 
/*
 * Message IDs for PrintDlgMgr module
 */
#define DTPDM_SEND_MAIL_LABEL     1
#define DTPDM_BANNER_LABEL        2
#define DTPDM_SPOOL_OPTS_LABEL    3
 

#ifdef I18N_MSG
extern char * _DtPrintGetMessage(
				 int set,
				 int n,
				 char * s);

#endif /* I18N_MSG */

#endif /* _DtPrintMsgsP_h */
