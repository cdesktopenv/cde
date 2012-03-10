
/*
 *      $XConsortium: ab_globals.c /main/3 1995/11/06 17:13:29 rswiston $
 *
 * @(#)ab_globals.c	1.35 19 May 1995      cde_app_builder/src/libABobj
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 ***********************************************************************
 * ab_globals.c - Defines all AB global variables
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <ab_private/obj.h>
#include <ab_private/ab.h>

ABObj		AB_project = NULL;
ABObj		AB_cur_module = NULL;

Widget		AB_toplevel = NULL;
Widget		AB_menubar = NULL;
Widget          AB_generic_chooser = NULL;
Widget		AB_group_dialog = NULL;
Widget		AB_layout_dialog = NULL;
Widget		AB_proj_window = NULL;
Widget		AB_rev_prop_dialog = NULL;
Widget          AB_conn_dialog = NULL;
Widget		AB_dnd_dialog = NULL;
Widget          AB_help_dialog = NULL;
Widget          AB_cgen_win = NULL;
Widget          AB_appfw_dialog = NULL;
Widget          AB_attch_ed_dialog = NULL;
Widget		AB_cgen_prop_dialog = NULL;
Widget		AB_cgen_env_dialog = NULL;
Widget		AB_msgEd_dialog = NULL;

int 		AB_builder_mode = MODE_BUILD;
int		AB_grid_size = 10;
Boolean		AB_btn1_transfer = True;
int		AB_BMenu = Button3;

Vwr		Proj_viewer = NULL;
XrmDatabase	AB_db = NULL;
