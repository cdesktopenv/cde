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
/* $XConsortium: help.h /main/3 1995/11/02 14:05:30 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
#include "externals.h"

/* help index flags */
#define HELP_QUERY                      1
#define HELP_ERROR                      2
#define HELP_ADD_HOTSPOT_BUTTON         4
#define HELP_CIRCLE_BUTTON              5
#define HELP_CLEAR_ICON_BUTTON          6
#define HELP_COPY_BUTTON                7
#define HELP_CUT_BUTTON                 8
#define HELP_DELETE_HOTSPOT_BUTTON      9
#define HELP_DYNAMIC_COLOR_BUTTON       10
#define HELP_EDIT_BUTTON                11
#define HELP_ELLIPSE_BUTTON             12
#define HELP_ERASER_BUTTON              13
#define HELP_FILE_BUTTON                14
#define HELP_FLIP_BUTTON                15
#define HELP_FLOOD_BUTTON               16
#define HELP_HELP_BUTTON                17
#define HELP_LINE_BUTTON                19
#define HELP_MAGNIFICATION_BUTTON       20
#define HELP_MENUBAR                    21
#define HELP_NEW_BUTTON                 23
#define HELP_NEW_ICON                   24
#define HELP_OPEN_BUTTON                25
#define HELP_OPTIONS_BUTTON             26
#define HELP_OUTPUT_FORMAT_BUTTON       27
#define HELP_PASTE_BUTTON               29
#define HELP_POINT_BUTTON               30
#define HELP_POLYGON_BUTTON             31
#define HELP_POLYLINE_BUTTON            32
#define HELP_QUIT_BUTTON                33
#define HELP_RECTANGLE_BUTTON           34
#define HELP_RESIZE_ICON_BUTTON         35
#define HELP_ROTATE_BUTTON              36
#define HELP_SAVE_AS_BUTTON             37
#define HELP_SAVE_BUTTON                38
#define HELP_SCALE_BUTTON               39
#define HELP_SELECT_BUTTON              40
#define HELP_STATIC_COLOR_BUTTON        41
#define HELP_STATIC_GREY_BUTTON         42
#define HELP_UNDO_BUTTON                44
#define HELP_VISIBLE_GRID_BUTTON        46
#define HELP_GRAB_BUTTON                47
#define HELP_FILL_BUTTON                48
#define HELP_VIEWPORT                   49
#define HELP_MAIN_WINDOW                50
#define HELP_FILEIO_ICON                51


#define HELP_HELP_INTRODUCTION          60
#define HELP_HELP_TASKS                 61
#define HELP_HELP_REFERENCE             62
#define HELP_HELP_ON_ITEM               63
#define HELP_HELP_USING_HELP            64
#define HELP_HELP_VERSION               65

#define HELP_INTRODUCTION               70
#define HELP_TASKS                      71
#define HELP_REFERENCE                  72
#define HELP_ON_ITEM                    73
#define HELP_USING_HELP                 74
#define HELP_VERSION                    75



/* help index strings */
#define HELP_QUERY_STR                  "query"
#define HELP_ERROR_STR                  "error"
#define HELP_ADD_HOTSPOT_BUTTON_STR     "add-hotspot"
#define HELP_CIRCLE_BUTTON_STR          "circle-tool"
#define HELP_CLEAR_ICON_BUTTON_STR      "clear-icon"
#define HELP_COPY_BUTTON_STR            "copy"
#define HELP_CUT_BUTTON_STR             "cut"
#define HELP_DELETE_HOTSPOT_BUTTON_STR  "delete-hotspot"
#define HELP_DYNAMIC_COLOR_BUTTON_STR   "dymanic-colors"
#define HELP_EDIT_BUTTON_STR            "edit-menu"
#define HELP_ELLIPSE_BUTTON_STR         "ellipse-tool"
#define HELP_ERASER_BUTTON_STR          "eraser-tool"
#define HELP_FILE_BUTTON_STR            "file-menu"
#define HELP_FILL_BUTTON_STR            "fill-tool"
#define HELP_FLIP_BUTTON_STR            "flip"
#define HELP_FLOOD_BUTTON_STR           "flood-tool"
#define HELP_HELP_BUTTON_STR            "help-menu"
#define HELP_LINE_BUTTON_STR            "line-tool"
#define HELP_MAGNIFICATION_BUTTON_STR   "magnification"
#define HELP_MAIN_WINDOW_STR            "main"
#define HELP_FILEIO_ICON_STR            "IEopenDB"
#define HELP_MENUBAR_STR                "menubar"
#define HELP_NEW_ICON_STR               "newIcon"
#define HELP_NEW_BUTTON_STR             "new"
#define HELP_OPEN_BUTTON_STR            "open"
#define HELP_OPTIONS_BUTTON_STR         "options-menu"
#define HELP_OUTPUT_FORMAT_BUTTON_STR   "output-format"
#define HELP_PASTE_BUTTON_STR           "paste"
#define HELP_POINT_BUTTON_STR           "point-tool"
#define HELP_POLYGON_BUTTON_STR         "polygon-tool"
#define HELP_POLYLINE_BUTTON_STR        "polyline-tool"
#define HELP_QUIT_BUTTON_STR            "quit"
#define HELP_RECTANGLE_BUTTON_STR       "rectangle-tool"
#define HELP_RESIZE_ICON_BUTTON_STR     "resize"
#define HELP_ROTATE_BUTTON_STR          "rotate"
#define HELP_SAVE_AS_BUTTON_STR         "save-as"
#define HELP_SAVE_BUTTON_STR            "save"
#define HELP_SCALE_BUTTON_STR           "scale"
#define HELP_SELECT_BUTTON_STR          "select-tool"
#define HELP_STATIC_COLOR_BUTTON_STR    "static-colors"
#define HELP_STATIC_GREY_BUTTON_STR     "static-greys"
#define HELP_UNDO_BUTTON_STR            "undo"
#define HELP_VIEWPORT_STR               "viewport"
#define HELP_VISIBLE_GRID_BUTTON_STR    "visible-grid"
#define HELP_GRAB_BUTTON_STR            "grab-image"

#define HELP_HELP_INTRODUCTION_STR      "help-menu-introduction"
#define HELP_HELP_TASKS_STR             "help-menu-tasks"
#define HELP_HELP_REFERENCE_STR         "help-menu-reference"
#define HELP_HELP_ON_ITEM_STR           "help-menu-onItem"
#define HELP_HELP_USING_HELP_STR        "help-menu-usingHelp"
#define HELP_HELP_VERSION_STR           "help-menu-version"

/* these are for the activate callback on the help menu items */
#define HELP_INTRODUCTION_STR           "_HOMETOPIC"
#define HELP_TASKS_STR                  "tasks"
#define HELP_REFERENCE_STR              "reference"
#define HELP_ON_ITEM_STR                "onItem"
#define HELP_USING_HELP_STR             "_HOMETOPIC"
#define HELP_VERSION_STR                "_COPYRIGHT"

#define HELP_VOLUME                     "Iconed"
#define HELP_HELP_VOLUME                "Help4Help"
