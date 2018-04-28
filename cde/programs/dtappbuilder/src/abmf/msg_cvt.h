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
 *	$XConsortium: msg_cvt.h /main/3 1995/11/06 18:10:03 rswiston $
 *
 * @(#)msg_cvt.h	1.1 15 Jul 1994	cde_app_builder/src/abmf
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _ABMF_MSG_CVT_H_
#define _ABMF_MSG_CVT_H_

/*
 * msg_cvt.h - API for message [re]generation
 */

#include <ab/util_types.h>
#include "write_codeP.h"
#include "msg_file.h"

extern BOOL	abmfP_initialize_msg_file(
    GenCodeInfo	genCodeInfo,
    ABObj	project
    );
extern char *	abmfP_catgets_prefix_str(
    GenCodeInfo	genCodeInfo,
    ABObj	obj,
    char	*msg_string
    );

#endif /* !_ABMF_MSG_CVT_H_ */
