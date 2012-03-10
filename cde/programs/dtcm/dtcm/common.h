/*******************************************************************************
**
**  common.h
**
**  static char sccsid[] = "@(#)common.h 1.6 94/11/07 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: common.h /main/3 1995/11/03 10:21:02 rswiston $
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

#ifndef _COMMON_H
#define _COMMON_H

#include "ansi_c.h"

#if LATER
typedef struct {
        int data_version;
        char* target;
} Cdata;

extern Cdata	*get_cdata		P(());
extern void	free_cd_from_blist	P(());
#endif
extern void	reset_values		P(());
extern void	backup_values		P(());
extern void	blist_write_names	P(());
extern Boolean	duplicate_cd		P(());
extern char	*get_appt_str		P(());
extern void	common_update_lists	P(());

#endif
