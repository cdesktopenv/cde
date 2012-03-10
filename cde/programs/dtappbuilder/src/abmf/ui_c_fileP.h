
/*
 *	$XConsortium: ui_c_fileP.h /main/3 1995/11/06 18:15:26 rswiston $
 *
 * %W% %G%	cde_app_builder/src/abmf
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

#ifndef _ABMF_UI_C_FILEP_H_
#define _ABMF_UI_C_FILEP_H_

#include <ab_private/obj.h>
#include "write_codeP.h"

int	abmfP_write_ui_c_file(
		GenCodeInfo	cGenInfo,
		STRING		codeFileName,
		ABObj		module
		);

#endif /* _ABMF_UI_C_FILEP_H_ */
