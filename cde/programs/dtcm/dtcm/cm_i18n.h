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
/*******************************************************************************
**
**  cm_i18n.h
**
**  static char sccsid[] = "@(#)cm_i18n.h 1.7 94/12/22 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: cm_i18n.h /main/4 1996/11/07 17:00:36 drk $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _FILENAME
#define _FILENAME
#endif

/* CM font file description:
 *
 * Each line cannot exceed MAX_LINE_LEN. 
 * File format is as follows: 
 * <Locale name> <fontset1_fontname1> <fontset2_fontname1>
 *               <fontset1_fontname2> <fontset2_fontname2>
 * Line starts at left most margin.
 * Categories separated by space(s).
 * Comment lines start with an '%'
 */


#define MAX_LINE_LEN    128
#define MAX_FONT_LEN    40
#define COMMENT_SYMBOL  "%"
#define CHAR_SIZE       sizeof(char)
#define isEUC(c)    ((c) & 0x80 ? TRUE : FALSE)

/* cm_get_fonts()'s return values */
#define OPEN_FAIL         -1
#define NO_LOCALE_ENTRY   -2 
#define EXTRACT_FAIL      -3
#define OKAY               1
#define NO_I18N_HEADER     2

extern char *fontset1[];
extern char *fontset2[];
extern int use_default_fonts;
extern int use_octal;

extern int cm_get_fonts(/* char * */); 
extern void ps_i18n_header(/* FILE *, Frame */);
extern char *euc_to_octal(/* char * */);
extern char *cm_get_i18n_date(/* Frame, char * */);
extern char *cm_printf();

int is_comment();
int match_locale();

extern int	cm_get_fonts		P((char *));
extern int	is_comment		P((char[MAX_LINE_LEN]));
extern int	match_locale		P((char *, char[MAX_LINE_LEN]));
extern void	ps_i18n_header		P((FILE *, Widget));
extern char*	euc_to_octal		P((char *));
extern char*	cm_get_i18n_date	P((Widget, char *));
extern char*	cm_printf		P((double, int));
