
/*
 *	$XConsortium: abmf.h /main/3 1995/11/06 18:01:36 rswiston $
 *
 *	@(#)abmf.h	1.3 15 Jul 1994	cde_app_builder/src/abmf
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
 * abmf.h - public entry points to the code generator
 */
#ifndef _ABMF_ABMF_H_
#define _ABMF_ABMF_H_

#include <ab_private/obj.h>

typedef enum
{
    ABMF_I18N_UNDEF= 0,
    ABMF_I18N_NONE,			/* no i18n */
    ABMF_I18N_XPG4_API,			/* generated code conforms to
					 * XPG4 API
					 */
    ABMF_I18N_METHOD_NUM_VALUES		/* must be last */
} ABMF_I18N_METHOD;


typedef enum
{
    ABMF_CGEN_UNDEF,
    ABMF_CGEN_ALL,
    ABMF_CGEN_BY_DATE,
    ABMF_CGEN_BY_DATE_AND_CONTENTS,
    ABMF_CGEN_RESTRICTION_NUM_VALUES	/* must be last */
} ABMF_CGEN_RESTRICTION;


/*
 * "dumped" resources are written to a resource file, rather than a C file
 */
int	abmf_generate_code(
		ABObj			project, 
		ABMF_CGEN_RESTRICTION	restriction,
		BOOL			merge_files,
		ABMF_I18N_METHOD	i18nMethod,
		BOOL			prototype_functions,
		AB_ARG_CLASS_FLAGS	dumped_resources
	);

#endif /* _ABMF_ABMF_H_ */

