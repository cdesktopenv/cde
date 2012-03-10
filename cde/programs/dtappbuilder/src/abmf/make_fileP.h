
/*
 *	$XConsortium: make_fileP.h /main/3 1995/11/06 18:08:34 rswiston $
 *
 * @(#)make_fileP.h	3.13 24 Mar 1994	cde_app_builder/src/abmf
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

#include "write_codeP.h"

int		abmfP_write_make_file(
			GenCodeInfo	genCodeInfo,
			ABObj		project,
			AB_OS_TYPE	osType,
			BOOL		useSourceBrowser
			);

