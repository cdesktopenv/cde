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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: UI.h /main/4 1995/11/01 11:31:06 rswiston $ */
/*****************************************************************************
 *
 *   File:         UI.h
 *
 *   Project:	   CDE
 *
 *   Description:  This file contains defines and declarations needed
 *                 by UI.c
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 *
 ****************************************************************************/


#ifndef _ui_h
#define _ui_h

#define SWITCH_HIGH_BUTTON_WIDTH	124
#define SWITCH_HIGH_BUTTON_HEIGHT	27
#define SWITCH_LOW_BUTTON_WIDTH		66
#define SWITCH_LOW_BUTTON_HEIGHT	22

#define ICON_HIGH_WIDTH			64
#define ICON_MEDIUM_WIDTH		58
#define ICON_LOW_WIDTH			40


#define UP_ARROW_IMAGE_NAME		"Fpup"
#define DOWN_ARROW_IMAGE_NAME		"Fpdown"
#define UP_MONITOR_ARROW_IMAGE_NAME	"FpupY"
#define DOWN_MONITOR_ARROW_IMAGE_NAME	"FpdownY"
#define BLANK_ARROW_IMAGE_NAME		"FpblnkA"
#define DROPZONE_IMAGE_NAME		"Fpdropz"
#define INDICATOR_OFF_IMAGE_NAME	"Fpindc"
#define INDICATOR_ON_IMAGE_NAME		"FpindcY"
#define MINIMIZE_NORMAL_IMAGE_NAME	"Fpmin"
#define MINIMIZE_SELECTED_IMAGE_NAME	"FpminY"
#define MENU_NORMAL_IMAGE_NAME		"Fpmenu"
#define MENU_SELECTED_IMAGE_NAME	"FpmenuY"
#define HANDLE_IMAGE_NAME		"Fphandl"
#define DEFAULT_IMAGE_NAME		"Fpdeflt"



extern void ToggleDefaultControl (ControlData *, SubpanelData *, ControlData *);
extern String GetIconName (String, unsigned int);
extern void AddSubpanel (ControlData * control_data);
extern void DeleteSubpanelControl(SubpanelData *subpanel, ControlData *control_data);
extern void DeleteSubpanel(ControlData *control_data);
extern void UpdateSwitchGeometry (BoxData * box_data);
extern void FrontPanelCreate (Widget toplevel);


#endif /* _ui_h */
/*  DON"T ADD ANYTHING AFTER THIS #endif  */
