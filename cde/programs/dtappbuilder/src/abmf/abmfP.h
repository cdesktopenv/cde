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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: abmfP.h /main/3 1995/11/06 18:01:49 rswiston $
 *
 * @(#)abmfP.h	3.36 30 Apr 1995	cde_app_builder/src/abmf
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
 * abmfP.h - declarations common to all abmf files
 */

#ifndef _ABMF_ABMFP_H_
#define _ABMF_ABMFP_H_

#include <assert.h>		/* assert() */
#include <stdio.h>		/* FILE*, BUFSIZ */
#include <sys/param.h>		/* MAXPATHLEN, ... */
#include <nl_types.h>
#include <X11/Intrinsic.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/strlist.h>
#include "abmf.h"

/*
 * Constants
 */
#define STD_BUF_SIZE	(BUFSIZ)
#define MAX_PATH_SIZE	(MAXPATHLEN+1)
#define MAX_NAME_SIZE	(MAX_PATH_SIZE)

#define CGenFlagsNone			((CGenFlags)0x0000)
#define	CGenFlagCompFieldsUnique	((CGenFlags)0x0001)
#define	CGenFlagSubstructObjValid	((CGenFlags)0x0002)
#define	CGenFlagStructObjValid		((CGenFlags)0x0004)
#define CGenFlagIsStructObjValid	((CGenFlags)0x0008)
#define CGenFlagIsStructObj		((CGenFlags)0x0010)
#define CGenFlagIsSubstructObjValid	((CGenFlags)0x0020)
#define CGenFlagIsSubstructObj		((CGenFlags)0x0040)
#define CGenFlagWriteDefToProjFile	((CGenFlags)0x0080)
#define CGenFlagIsDuplicateDef		((CGenFlags)0x0100)
#define CGenFlagLoadMe			((CGenFlags)0x0200)
#define CGenFlagIsReferenced		((CGenFlags)0x0400)
#define CGenFlagTreatAsAppShell		((CGenFlags)0x0800)

/* specify what type of cgen data is attached to obj */
#define CGenFlagDataIsModule		((CGenFlags)0x2000)
#define CGenFlagDataIsObj		((CGenFlags)0x4000)
#define CGenFlagDataIsProj		((CGenFlags)0x8000)

typedef FILE		*File;		/* convenience */

/*
typedef struct
{
    ISTRING	field_name;
    ABObj	struct_obj;
    ABObj	substruct_obj;
} CGenObjDataRec, *CGenObjData;
*/

/* Proj info */
typedef struct
{
    StringList	callbacks;
    CGenAnyData	children_data;
    BOOL	has_ui_obj;
    BOOL	has_terminal;
    BOOL	has_help;
    ISTRING	exe_name;
} CGenProjDataRec, *CGenProjData;


/* Module info */
typedef struct
{
    StringList	callbacks;
    CGenAnyData	children_data;
} CGenModuleDataRec, *CGenModuleData;


typedef struct CGEN_ANY_DATA_REC	/* name must match obj.h! */
{
    ISTRING	field_name;
    ABObj	struct_obj;
    ABObj	substruct_obj;
    int		num_auto_callbacks;
    union
    {
        CGenProjData	proj;
        CGenModuleData	module;
    } info;
} CGenAnyDataRec;
typedef CGenAnyDataRec	CGenDataRec;
typedef CGenAnyData	CGenData;

#define return_if_err(_return_code, _return_value) \
	   if ((_return_code) < 0) \
	   { \
	       return_value= (_return_value); \
	       goto epilogue; \
	   }

#define return_code(_return_code) \
	    {return_value= (_return_code); goto epilogue;}


#define mfobj_flags(obj) ((obj)->cgen_flags)

#define mfobj_set_flags(obj, _flags)  \
		((obj)->cgen_flags |= ((CGenFlags)(_flags)))

#define mfobj_clear_flags(obj, _flags) \
		(mfobj_flags(obj) &= ~((CGenFlags)(_flags)))

#define mfobj_has_flags(obj, _flags)  \
		((mfobj_flags(obj) & ((CGenFlags)(_flags))) != 0)

/*
 * Get the various types of data from the object
 */
#define mfobj_has_data(obj) \
	((obj)->cgen_data != NULL)

#define mfobj_data(obj) \
	(obj->cgen_data)

#define mfobj_set_proj_data(_obj,_data) \
	( mfobj_set_flags(_obj, CGenFlagDataIsProj), \
	  ((long)(mfobj_data(_obj)->info.proj = (_data))) \
	)

#define mfobj_get_proj_data(obj) \
	(mfobj_has_flags(obj, CGenFlagDataIsProj)? \
	    (mfobj_data(obj)->info.proj) \
	: \
	    ((CGenProjData)NULL) \
	)

#define mfobj_set_module_data(_obj,_data) \
	( mfobj_set_flags(_obj, CGenFlagDataIsModule), \
	  ((int)(mfobj_data(_obj)->info.module = (_data))) \
	)

#define mfobj_get_module_data(obj) \
	(mfobj_has_flags(obj, CGenFlagDataIsModule)? \
	    (mfobj_data(obj)->info.module) \
	: \
	    ((CGenModuleData)NULL) \
	)

/* Internationalization defines and references  */
extern nl_catd	Dtb_project_catd;

#endif /* _ABMF_ABMFP_H_ */
