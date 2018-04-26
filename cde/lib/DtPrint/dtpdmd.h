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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: dtpdmd.h /main/4 1996/08/12 18:41:45 cde-hp $ */
/******************************************************************************
 ******************************************************************************
 **
 ** File:         dtpdmd.h
 **
 ** Description:  Header file for the "dtpdmd/dtpdm protocol" which
 **               consists of a standard command line dtpdmd->dtpdm
 **               and starndard exit codes dtpdm->dtpdmd.
 **
 **               Other dtpdmd header stuff lives here too.
 **
 ** (c) Copyright 1995, 1996, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/******************************************************************************
 *
 * Standard command line parameters for any PDM which
 * will become a child of dtpdmd via fork/exec.
 *
 * generic_pdm [dt-pdm-command options] [dtpdmd options]
 *
 * where [dt-pdm-command options] is:
 *    - any additional options that were specified along with
 *      the dt-pdm-command attribute from the X Print Server.
 *
 * where [dtpdmd options] is any of:
 *    -display  dpy     Specified the display connection to the Video X-Server.
 *    -window   vwid    Specified the window id on dpy.
 *    -pdisplay pdpy    Specified the display connection to the Print X-Server.
 *    -pcontext pcid    Specified the print context id on pdpy.
 */

/******************************************************************************
 *
 * Standard exit() codes for any PDM which is a child of dtpdmd.
 */
#define PDM_EXIT_NONE           None
#define PDM_EXIT_OK             191     /* "OK" */
#define PDM_EXIT_CANCEL         192     /* "CANCEL" */
#define PDM_EXIT_VXAUTH         193     /* no print display authorization */
#define PDM_EXIT_PXAUTH         194     /* no video display authorization */
#define PDM_EXIT_ERROR          195     /* all other error reasons */

/*
 * Misc information.
 */
#define DEFAULT_PDM_EXECUTABLE  "dtpdm"

