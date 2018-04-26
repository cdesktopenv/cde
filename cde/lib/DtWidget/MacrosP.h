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
/* $XConsortium: MacrosP.h /main/5 1996/03/27 20:16:40 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990,1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/**---------------------------------------------------------------------
***	
***	file:		MacrosP.h
***
***	project:	MotifPlus Widgets
***
***	description:	Private include file with macros for MotifPlus
***			widgets and gadgets.
***	
***-------------------------------------------------------------------*/


#ifndef _DtMacrosP_h
#define _DtMacrosP_h

#define XmUNSPECIFIED_STRING	(XmString) 3


/*-------------------------------------------------------------
**	Widget Class Macros
*/

/*	Core Class Macros
*/
/*** WARNING: These macros are not thread-safe! ***/
#define C_WidgetClass(wc)	(wc -> core_class.widget_class)
#define C_Superclass(wc)	(wc -> core_class.superclass)
#define C_ClassName(wc)		(wc -> core_class.class_name)
#define C_ClassInited(wc)	(wc -> core_class.class_inited)
#define C_NumResources(wc)	(wc -> core_class.num_resources)

/*	Composite Class Macros
*/	
#ifdef _XtCompositeP_h
#define C_ChangeManaged(wc)	(wc -> composite_class.change_managed)
#endif /* _XtCompositeP_h */

/*	Constraint Class Macros
*/	
#ifdef _XtConstraintP_h
#define C_ConstraintResources(wc) (wc -> constraint_class.resources)
#define C_ConstraintNumResources(wc) (wc -> constraint_class.num_resources)
#endif /* _XtConstraintP_h */

/*	XmGadget Class Macros
*/
#define XmInheritBorderHighlight	((XtWidgetProc) _XtInherit)
#define XmInheritBorderUnhighlight	((XtWidgetProc) _XtInherit)
#define XmInheritVisualChange		((XmVisualChangeProc) _XtInherit)



/*-------------------------------------------------------------
**	Primitive Macros
*/

#define P_X(w)			(w -> core.x)
#define P_Y(w)			(w -> core.y)
#define P_Width(w)		(w -> core.width)
#define P_Height(w)		(w -> core.height)
#define P_BorderWidth(w)	(w -> core.border_width)
#define P_XrmName(w)		(w -> core.xrm_name)
#define P_PopupList(w)		(w -> core.popup_list)
#define P_NumPopups(w)		(w -> core.num_popups)



/*-------------------------------------------------------------
**	Gadget Macros
*/

/*	Object Macros
*/
#define G_XrmName(r)		(r -> object.xrm_name)
#define G_Class(r)		(r -> object.widget_class)

/*	RectObject Macros
*/
#define G_X(r)			(r -> rectangle.x)
#define G_Y(r)			(r -> rectangle.y)
#define G_Width(r)		(r -> rectangle.width)
#define G_Height(r)		(r -> rectangle.height)
#define G_BorderWidth(r)	(r -> rectangle.border_width)

/*	XmGadget Macros
*/
#define G_HelpCB(g)		(g -> gadget.help_callback)
#define G_EventMask(g)		(g -> gadget.event_mask)
#define G_Highlighted(g)	(g -> gadget.highlighted)
#define G_UnitType(g)		(g -> gadget.unit_type)



/*-------------------------------------------------------------
**	Manager Macros
*/

/*	Core Macros
*/
#define M_X(w)			(w -> core.x)
#define M_Y(w)			(w -> core.y)
#define M_Width(w)		(w -> core.width)
#define M_Height(w)		(w -> core.height)
#define M_BorderWidth(w)	(w -> core.border_width)
#define M_Background(w)		(w -> core.background_pixel)
#define M_Sensitive(w)		(w->core.sensitive && w->core.ancestor_sensitive) 
#define M_Name(w)		(w -> core.name)
#define M_Accelerators(w)	(w -> core.accelerators)
#define M_TmTranslations(w)	(w -> core.tm.translations)
#define M_PopupList(w)		(w -> core.popup_list)
#define M_NumPopups(w)		(w -> core.num_popups)

/*	Composite and Constraint Macros
*/
#define M_Children(m)		(m -> composite.children)
#define M_NumChildren(m)	(m -> composite.num_children)

/*	XmManager Macros
*/
#ifdef _XmManagerP_h
#define M_Foreground(m)		(m -> manager.foreground)
#define M_HighlightThickness(m)	(m -> manager.highlight_thickness)
#define M_ShadowThickness(m)	(m -> manager.shadow_thickness)
#define M_TopShadowGC(m)	(m -> manager.top_shadow_GC)
#define M_BottomShadowGC(m)	(m -> manager.bottom_shadow_GC)
#define M_BackgroundGC(m)	(m -> manager.background_GC)
#define M_SelectedGadget(m)	(m -> manager.selected_gadget)
#define M_HelpCB(m)		(m -> manager.help_callback)
#define M_UnitType(m)		(m -> manager.unit_type)
#endif /* _XmManagerP_h */

/*	XmDrawingArea Macros
*/
#ifdef _XmDrawingAreaP_h
#define M_MarginWidth(m)	(m -> drawing_area.margin_width)
#define M_MarginHeight(m)	(m -> drawing_area.margin_height)
#endif /* _XmDrawingAreaP_h */

/*	XmBulletinBoard Macros
*/
#ifdef _XmBulletinBoardP_h
#define M_ButtonFontList(m)	(m -> bulletin_board.button_font_list)
#define M_LabelFontList(m)	(m -> bulletin_board.label_font_list)
#define M_TextFontList(m)	(m -> bulletin_board.text_font_list)
#define M_MarginWidth(m)	(m -> bulletin_board.margin_width)
#define M_MarginHeight(m)	(m -> bulletin_board.margin_height)
#endif /* _XmBulletinBoardP_h */

#ifdef MAYBE
/*	DtIconBox Macros
*/
#ifdef _DtIconBoxP_h
#define M_Timer(m)		(m -> icon_box.timer)
#define M_ShellX(m)		(m -> icon_box.shell_x)
#define M_ShellY(m)		(m -> icon_box.shell_y)
#define M_DragShell(m)		(m -> icon_box.drag_shell)
#define M_DragWidget(m)		(m -> icon_box.drag_widget)
#define M_DragIcon(m)		(m -> icon_box.drag_icon)
#define M_XCursor(m)		(m -> icon_box.x_cursor)
#define M_CrossCursor(m)	(m -> icon_box.cross_cursor)
#endif /* _DtIconBoxP_h */
#endif /* MAYBE */

#endif /* _DtMacrosP_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
