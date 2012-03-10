
/*
 *	$XConsortium: ab_dnd.h /main/3 1995/11/06 17:13:18 rswiston $
 *
 * %W% %G%	cde_app_builder/src/
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
 * ab_dnd.h
 * Declarations for drag and drop functions in the app builder.
 */
#ifndef _ab_dnd_h
#define _ab_dnd_h

extern void	ab_dnd_register_drop_file(
		    Widget	dropFile
		);

extern void	ab_dnd_unregister_drop(
		    Widget	dropFile
		);

#endif /* _ab_dnd_h */
