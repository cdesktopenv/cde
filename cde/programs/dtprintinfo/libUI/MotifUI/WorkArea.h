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
/* $XConsortium: WorkArea.h /main/3 1995/11/06 09:45:18 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef WORKAREA_H
#define WORKAREA_H

#ifdef __cplusplus
extern "C" {
#endif
 
externalref WidgetClass  workAreaWidgetClass;

typedef struct _WorkAreaClassRec *WorkAreaWidgetClass;
typedef struct _WorkAreaRec      *WorkAreaWidget;

enum {
    GuiATTACH_TOP, 
    GuiATTACH_BOTTOM, 
    GuiATTACH_LEFT, 
    GuiATTACH_RIGHT
};

#define GuiIsWorkArea(w) XtIsSubclass((w), workAreaWidgetClass)

extern const char gui_workarea_strings[];

#define GuiNisList ((char*)&gui_workarea_strings[0])
#define GuiNattachment ((char*)&gui_workarea_strings[7])
#define GuiCAttachment ((char*)&gui_workarea_strings[18])
#define GuiRAttachment ((char*)&gui_workarea_strings[29])
#define GuiNisWorkArea ((char*)&gui_workarea_strings[40])
#define GuiNautoResizeWidth ((char*)&gui_workarea_strings[51])
#define GuiNisOpened ((char*)&gui_workarea_strings[67])
#define GuiNisTransient ((char*)&gui_workarea_strings[76])
#define GuiNlineThickness ((char*)&gui_workarea_strings[88])
#define GuiCLineThickness ((char*)&gui_workarea_strings[102])
#define GuiNlineOffset ((char*)&gui_workarea_strings[116])
#define GuiCLineOffset ((char*)&gui_workarea_strings[127])
#define GuiNnodeLineLength ((char*)&gui_workarea_strings[138])
#define GuiCNodeLineLength ((char*)&gui_workarea_strings[153])
#define GuiNnumberSubNodes ((char*)&gui_workarea_strings[168])
#define GuiCNumberSubNodes ((char*)&gui_workarea_strings[183])
#define GuiNnumberColumns ((char*)&gui_workarea_strings[198])
#define GuiCNumberColumns ((char*)&gui_workarea_strings[212])
#define GuiNsubNodes ((char*)&gui_workarea_strings[226])
#define GuiCSubNodes ((char*)&gui_workarea_strings[235])
#define GuiNhorizontalSpace ((char*)&gui_workarea_strings[244])
#define GuiNverticalSpace ((char*)&gui_workarea_strings[260])
#define GuiCSpace ((char*)&gui_workarea_strings[274])
#define GuiNsuperNode ((char*)&gui_workarea_strings[280])
#define GuiCSuperNode ((char*)&gui_workarea_strings[290])
extern Widget GuiCreateWorkArea(
    Widget parent, 
    char *name, 
    ArgList arglist, 
    Cardinal argcount);

extern Widget GuiCreateScrolledWorkArea(
    Widget parent, 
    char *name, 
    ArgList arglist, 
    Cardinal argcount);

/*
 * Use the following two functions to improve performance when managing and
 * unmanaging many children all at once.
 */
 
extern void GuiWorkAreaEnableRedisplay(Widget /* workArea widget */);
extern void GuiWorkAreaDisableRedisplay(Widget /* workArea widget */);

/*
 * GuiWorkAreaReorderChildren reorders a parent's subnodes.  Can be use to sort 
 * or move subnodes.  The subnode list can be a subset of the parents's
 * subnodes and it can be moved relative to another position (widget).
 * The default position is to start the reorder as the first subnode.
 *
 * If position == parent_subnode, start reorder as first subnode.  
 * If position == NULL, subnode_list[n_subnodes] will be the last sub_node
 * of parent_subnode.  If position is a widget in the parent's subnodes,
 * start the reorder after it.
 */

extern void GuiWorkAreaReorderChildren(
    Widget parent_subnode, 
    WidgetList subnode_list,
    int n_subnodes, 
    Widget position);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* WORKAREA_H */
