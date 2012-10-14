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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: olias_consts.h /main/6 1996/10/26 18:19:11 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#ifndef _DTMMDB_OLIAS_CONSTS_HH_
#define _DTMMDB_OLIAS_CONSTS_HH_

#define DOC_CODE 		1000
#define OLIAS_NODE_CODE 	1001
#define LOCATOR_CODE 		1002
#define TOC_CODE 		1003
#define DLP_CODE 		1004
#define GRAPHIC_CODE 		1009
#define SGML_CONTENT_CODE	1010
#define STYLESHEET_CODE 	1011
#define XREF_CODE 		1012

#define NUM_DOC_FIELDS 		7
#define NUM_OLIAS_NODE_FIELDS 	7
#define NUM_LOCATOR_FIELDS 	3
#define NUM_TOC_FIELDS 		4
#define NUM_GRAPHIC_FIELDS 	6
#define NUM_STYLESHEET_FIELDS 	3

#define USER_MARK_CODE 		1006
#define USER_CONFIG_CODE 	1007

#define DOC_SET_NAME		(char*)"doc"
#define NODE_SET_NAME		(char*)"node"
#define TOC_SET_NAME		(char*)"toc"
#define LOCATOR_SET_NAME	(char*)"loc"
#define GRAPHIC_SET_NAME	(char*)"graphic"
#define STYLESHEET_SET_NAME	(char*)"stylesheet"

#define DLP_LIST_NAME		(char*)"dlp"

#define USER_MARK_SET_NAME	(char*)"mark"
#define USER_CONFIG_SET_NAME	(char*)"config"

#define MARK_SPEC		(char*)"mmdb.mark.spec"
#define PREF_SPEC		(char*)"mmdb.pref.spec"

#define NODE_SET_POS    	0
#define TOC_SET_POS     	1
#define LOCATOR_SET_POS 	2
#define GRAPHIC_SET_POS 	3
#define DOC_SET_POS     	4
#define STYLESHEET_SET_POS      5

#define DLP_LIST_POS    	0

#ifndef GRAPHIC_TYPE_ENUM
#define GRAPHIC_TYPE_ENUM
typedef enum {
  GR_TYPE_UNKNOWN,
  GR_TYPE_CGM,
  GR_TYPE_GIF,
  GR_TYPE_JPEG,
  GR_TYPE_PBM,
  GR_TYPE_PGM,
  GR_TYPE_POSTSCRIPT,
  GR_TYPE_PPM,
  GR_TYPE_TIFF,
  GR_TYPE_XBM,
  GR_TYPE_XPM,
  GR_TYPE_XWD
}  GR_TYPE;
#endif

#endif
