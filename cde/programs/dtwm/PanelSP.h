/* $XConsortium: PanelSP.h /main/3 1995/11/01 11:27:59 rswiston $ */

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
