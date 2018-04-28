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
/* $XConsortium: VolSelectI.h /main/5 1995/10/26 12:34:32 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        VolSelectI.h
 **
 **   Project:     DtHelp Project
 **
 **   Description: File locating and handling utilities
 ** 
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelp_VolSelectI_h
#define _DtHelp_VolSelectI_h

#include <Xm/Xm.h>

/* collects all the dlg children together */
/* sort of a pseudo widget rec */
typedef struct _DtHelpFileDlgChildren {
          Widget        shell;
          Widget        form;
          Widget        label;
          Widget        list;
          Widget        prompt;
          Widget        separator;
          Widget        closeBtn;
          Widget        helpBtn;
} _DtHelpFileDlgChildren;

Widget _DtHelpFileListCreateSelectionDialog (
        DtHelpDialogWidget hw,
        Widget         parent,
        Boolean        modalDialog,
        char *         dlgTitle,
        XmFontList *   io_titlesFontList,
        _DtHelpFileList   in_out_list,
        _DtHelpFileDlgChildren * out_struct);

#endif /* _DtHelp_VolSelectI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

