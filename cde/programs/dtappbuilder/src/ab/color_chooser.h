/*
 *      $XConsortium: color_chooser.h /main/3 1995/11/06 17:24:25 rswiston $
 *
 * @(#)color_chooser.h	1.1 21 Feb 1994      cde_app_builder/src/ab
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
** color_chooser.h -- declarations associated with the color chooser
*/

/* 
** Routine to display the color chooser and return the name of the
** color selected.  Returns an empty string if no color is selected.
*/ 
char *		display_color_chooser();
