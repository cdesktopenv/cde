
/*
 *	$XConsortium: merge_cP.h /main/3 1995/11/06 18:09:04 rswiston $
 *
 *	@(#)merge_cP.h	1.2 01 Sep 1994	cde_app_builder/src/abmf
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

/*
 * merge_cP.h - Merge C files, using magic comments
 */
#ifndef _ABMF_MERGE_CP_H_
#define _ABMF_MERGE_CP_H_

#include "abmfP.h"

extern int	abmfP_merge_c_files(
			File	oldFile,
			STRING	oldFileName,
			File	newFile,
			STRING	newFileName,
			File	*mergedFileOut,
			File	*deltaFileOut
			);


#endif /* _ABMF_MERGE_CP_H_ */

