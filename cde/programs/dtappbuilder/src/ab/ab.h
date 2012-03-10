
/*
 *	$XConsortium: ab.h /main/3 1995/11/06 17:12:23 rswiston $
 *
 * @(#)ab.h	1.49 94/07/31 Copyright 1993 Sun Microsystems
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
 * ab.h 
 */
#ifndef _AB_H_
#define _AB_H_

#include <Xm/Xm.h>
#include <nl_types.h>		/* Necessary for message I18N */
#include <ab_private/obj.h>
#include <ab_private/vwr.h>
#include <ab_private/ab_utils.h>
#include <Dt/Dt.h>              /* for version information */


#define MODE_BUILD	1
#define MODE_TEST_SHOWN	2
#define MODE_TEST_PROJ	3

/*
 * Determine the version string
 */
#ifdef SDtVERSION_STRING
    #define AbVERSION_STRING SDtVERSION_STRING
#elif defined DtVERSION_STRING
    #define AbVERSION_STRING DtVERSION_STRING
#else
    #define AbVERSION_STRING "CDE Version Unknown"
#endif


extern ABObj		AB_project;
extern ABObj		AB_cur_module;

extern int		AB_builder_mode;

extern Widget		AB_toplevel;

extern Widget           AB_cgen_win;
extern Widget		AB_proj_window;

extern Widget		AB_generic_chooser;

extern Widget		AB_appfw_dialog;
extern Widget		AB_attch_ed_dialog;
extern Widget           AB_cgen_env_dialog;
extern Widget           AB_cgen_prop_dialog;
extern Widget           AB_conn_dialog;
extern Widget		AB_dnd_dialog;
extern Widget		AB_help_dialog;
extern Widget		AB_layout_dialog;
extern Widget		AB_msgEd_dialog;
extern Widget		AB_rev_prop_dialog;

extern Vwr		Proj_viewer;
extern XrmDatabase	AB_db;
extern Boolean          AB_btn1_transfer; 
extern int              AB_BMenu; 

/* Needed for message I18N */
extern nl_catd          Dtb_project_catd;

#endif /* _AB_H_ */
