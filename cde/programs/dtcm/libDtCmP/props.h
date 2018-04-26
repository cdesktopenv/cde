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
**  props.h
**
**  static char sccsid[] = "@(#)props.h 1.6 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: props.h /main/5 1996/03/29 17:28:13 rswiston $
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

#ifndef _PROPS_H
#define _PROPS_H

#include <sys/types.h>
#include "ansi_c.h"
#include "resource.h"

/*******************************************************************************
**
**  Typedefs and supporting structures
**
*******************************************************************************/
typedef enum {
        HOUR12,
	HOUR24
} DisplayType;

typedef enum {
        YEAR_VIEW,
	MONTH_VIEW,
	WEEK_VIEW,
	DAY_VIEW
} ViewType;

typedef struct props_entry {
	char			*property_name;
	char			*property_value;
	boolean_t		update;
	struct props_entry	*next;
} Props_entry;

/*
**  Enumerated type to access properties array.  If you add a member to this
**  enumeration, be sure to add it's value to the character array in props.c
*/
typedef enum {
	CP_BEGOP,		/* Don't put anything before this */
	CP_BEEPON,
	CP_BEEPADV,
	CP_BEEPUNIT,
	CP_FLASHON,
	CP_FLASHADV,
	CP_FLASHUNIT,
	CP_OPENON,
	CP_OPENADV,
	CP_OPENUNIT,
	CP_MAILON,
	CP_MAILADV,
	CP_MAILUNIT,
	CP_MAILTO,
	CP_UNIXON,
	CP_UNIXADV,
	CP_UNIXCOMMAND,
	CP_DAYBEGIN,
	CP_DAYEND,
	CP_DAYCALLIST,
	CP_DEFAULTVIEW,
	CP_DEFAULTDISP,
	CP_PRINTDEST,
	CP_PRINTPRIVACY,
	CP_PRINTERNAME,
	CP_PRINTOPTIONS,
	CP_PRINTDIRNAME,
	CP_PRINTFILENAME,
	CP_PRINTRMARGIN,
	CP_PRINTBMARGIN,
	CP_PRINTLMARGIN,
	CP_PRINTTMARGIN,
	CP_PRINTLHEADER,
	CP_PRINTRHEADER,
	CP_PRINTLFOOTER,
	CP_PRINTRFOOTER,
	CP_PRINTMONTHS,
	CP_PRINTCOPIES,
	CP_DEFAULTCAL,
	CP_CALLOC,
	CP_DATEORDERING,
	CP_DATESEPARATOR,
	CP_PRIVACY,
	CP_USEFNS,
	CP_APPTBEGIN,
	CP_APPTDURATION,
	CP_ENDOP		/* Don't put anything after this */
} Props_op;

/*******************************************************************************
**
**  The not-so-mighty props structure is simply an X resource database and the
**  array of Props_entry's which contain the property information.
**
*******************************************************************************/
typedef struct {
	Props_entry	*p_list;
	Resource	*rdb;
} Props;

/*******************************************************************************
**
**  External function declarations
**
*******************************************************************************/
extern boolean_t	cal_convert_cmrc	P((Props*));
extern char		*get_char_prop		P((Props*, Props_op));
extern char		*get_char_prop_default	P((Props_op));
extern int		get_int_prop		P((Props*, Props_op));
extern void		props_clean_up		P((Props*));
extern boolean_t	read_props		P((Props*));
extern boolean_t	save_props		P((Props*));
extern boolean_t	set_char_prop		P((Props*, Props_op, char*));
extern boolean_t	set_int_prop		P((Props*, Props_op, int));

#endif
