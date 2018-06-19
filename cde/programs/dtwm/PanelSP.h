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

#ifndef _DtPanelShellP_h
#define _DtPanelShellP_h

#include <Xm/XmP.h>
#include <X11/ShellP.h>
#include "PanelS.h"


typedef struct _DtPanelShellClassPart
    {
    XtPointer       	extension;
    }	DtPanelShellClassPart;

typedef struct _DtPanelShellClassRec
    {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ShellClassPart	shell_class;
    WMShellClassPart	wm_shell_class;
    VendorShellClassPart	vendor_shell_class;
    TopLevelShellClassPart	top_level_shell_class;
    DtPanelShellClassPart	application_shell_class;
    }	DtPanelShellClassRec;

extern DtPanelShellClassRec dtPanelShellClassRec;


typedef struct _DtPanelShellPart
    {
    XmFontList		high_res_font_list;
    XmFontList		medium_res_font_list;
    XmFontList		low_res_font_list;
    Dimension		resolution;
    }	DtPanelShellPart;

typedef struct _DtPanelShellRec
    {
    CorePart	 	core;
    CompositePart 	composite;
    ShellPart 		shell;
    WMShellPart		wm;
    VendorShellPart	vendor;
    TopLevelShellPart	topLevel;
    DtPanelShellPart	panel;
    }	DtPanelShellRec;


/*-------------------------------------------------------------
**	Class and Instance Macros
*/

/*	DtPanelShell Class Macros
*/	

/*	DtPanelShell Instance Macros
*/
#define S_Resolution(s)		(s -> panel.resolution)
#define S_HighResFontList(s)	(s -> panel.high_res_font_list)
#define S_MediumResFontList(s)	(s -> panel.medium_res_font_list)
#define S_LowResFontList(s)	(s -> panel.low_res_font_list)


#endif /* _DtPanelShellP_h */
