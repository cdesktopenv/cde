
/*
 *	$XConsortium: ab_bil.h /main/3 1995/11/06 17:12:53 rswiston $
 *
 * @(#)ab_bil.h	1.13 02 Apr 1995
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
 * File: ab_bil.h
 */

#ifndef _ab_ab_bil_h
#define _ab_ab_bil_h

#include <ab_private/obj.h>

/* Defines for data types returned by DtDtsBufferToDataType()
 * and DtDtsFileToDataType().
 */
#define	BIL	"BIL"
#define BIP	"BIP"
#define BIX	"BIX"

/* Loads entire project. Sets/replaces current project.
 */
int 	ab_load_bil_file(
	    STRING	fileName,
	    FILE	*inFile,
	    BOOL	BufferDrop
	);

/* Loads one bil file, containing one or more modules, into
 * the current project.
 */
int 	ab_import_bil_file(
	    STRING	fileName,
	    FILE	*inFile,
	    BOOL	import_by_ref
	);

/* Calls ab_load_bil_file().  Called by the File->Open Project
 * callback and the Project->Open callback.  Also called when
 * a project is loaded from the command-line.
 */
int 	ab_check_and_open_bip(
	    STRING      fileName
	);

/* Calls ab_import_bil_file().  Called by the File->Import->Module
 * callback and the Module->Import callback.  Also called when
 * a project or module is loaded from the command-line. 
 */ 
int 	ab_check_and_import_bil(
	    STRING      fileName,
	    BOOL        ImportByCopy
	);


int 	ab_load_project(
	    STRING      fileName,
	    FILE        *inFile,
	    BOOL	BufferDrop
	);

int 	ab_import_module(
	    STRING      fileName,
	    FILE        *inFile,
	    BOOL	ImportByCopy
	);


#endif /* _ab_ab_bil_h */
