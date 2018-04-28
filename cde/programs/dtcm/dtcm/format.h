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
**  format.h
**
**  static char sccsid[] = "@(#)format.h 1.11 94/11/07 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: format.h /main/3 1995/11/03 10:27:14 rswiston $
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

#ifndef _FORMAT_H
#define _FORMAT_H

#include <csa.h>
#include "ansi_c.h"
#include "props.h"

#define DEFAULT_APPT_LEN	50
#define DEFAULT_GAPPT_LEN	80


extern void format_date		P((Tick, OrderingType, char*, int, int, int));
extern void format_date3	P((Tick, OrderingType, SeparatorType, char*));
extern Boolean format_line	P((Tick, char*, char*, int, Boolean,
				   DisplayType));
extern void format_line2	P((Dtcm_appointment*, char*, char*,
				   DisplayType));
extern void format_appt		P((Dtcm_appointment*, char*, DisplayType, int));
extern void format_abbrev_appt	P((Dtcm_appointment*, char*, Boolean,
				   DisplayType));
extern void format_maxchars	P((Dtcm_appointment*, char*, int, DisplayType));
extern void format_gappt	P((Dtcm_appointment*, char*, char*,
				   DisplayType, int));

#endif
