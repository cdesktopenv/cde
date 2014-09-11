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
/* $XConsortium: DataBaseLoad.h /main/7 1996/10/30 11:14:37 drk $ */
/*****************************************************************************
 *
 *   File:         DataBaseLoad.h
 *
 *   Project:	    DT
 *
 *   Description:  This file contains the external defines and function
 *                 declarations for DataBaseLoad.c
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 *
 ****************************************************************************/


#include <Xm/ColorObjP.h>
#include <Dt/Message.h>
#include <Dt/ActionP.h>

#ifndef _databaseload_h
#define _databaseload_h


/************************************************************************
 *
 *  Suffix definition for the front panel file type.
 *
 ************************************************************************/
 
#define FILE_TYPE_SUFFIX  ".fp"



/************************************************************************
 *
 *  Defines for the total number of entry types and an array which
 *  holds the string defining the entrys.
 *
 ************************************************************************/  
 
#define TOTAL_ENTRY_COUNT	5

#define PANEL		0
#define BOX		1
#define SUBPANEL	2
#define SWITCH		3
#define CONTROL		4

extern char * entry_types[];
extern char * control_types[];
extern char * resolution_types[];
extern char * monitor_types[];

#define HIGH		0
#define MEDIUM		1
#define LOW		2
#define MATCH_DISPLAY	3

#define DOUBLE_CLICK	0
#define SINGLE_CLICK	1

#define CONTROL_BLANK		0
#define CONTROL_BUSY		1
#define CONTROL_ICON		2
#define CONTROL_CLIENT		3
#define CONTROL_CLOCK		4
#define CONTROL_DATE		5
#define CONTROL_FILE		6

#define POSITION_FIRST	0
#define POSITION_LAST	100

#define MONITOR_NONE	0
#define MONITOR_MAIL	1
#define MONITOR_FILE	2

#define FP_HELP_VOLUME "FPanel"
#define PANEL_HANDLE "FPOnItemPanelHandle"
#define PANEL_MENU "FPOnItemPanelMenu"
#define PANEL_ICONIFY "FPOnItemPanelIconify"
#define SUBPANEL_ACCESS_AREA "FPOnItemAccessArea"
#define MONITOR_INDICATOR "FPOnItemMonitorIndicator"
#define SWITCH_BUTTON "FPOnItemSwitchButton"
#define INSTALL_ZONE "FPOnItemInstallZone"


/************************************************************************
 *
 *  Main structure definitions for the data of each type of front panel
 *  record.
 *
 ************************************************************************/

typedef struct
{
   char *  record_keyword;
   char ** field_keywords;
   int     num_field_keywords;
} ComponentType;

typedef struct
{
   Boolean   use_default;
   char    * string_value;
   void    * parsed_value;
} ElementValue;

typedef struct
{
   ElementValue * element_values;
} RecordData;

typedef struct
{
    int          flags;
    int          x;
    int          y;
    int          width;
    int          height;
} GeometryData;


typedef struct
{
    char *        action_name;
    char *        action_label;
    DtActionArg * aap;
    int	          count;
} PanelActionData;



typedef struct
{
   char  * file_name;
   char  * component_name;
   int     component_type;
   char  * parent_name;
   int     parent_type;   
   Boolean delete;
} DynamicComponent;



/************************************************************************
 *
 *  Main structure definitions for each type of Front Panel record.
 *  Each start with the set of element values.  This is required inorder
 *  to cast to a generic structure which can then be used to process the
 *  lists of data.
 *
 ************************************************************************/

typedef struct _PanelData     PanelData;
typedef struct _BoxData       BoxData;
typedef struct _SubpanelData  SubpanelData;
typedef struct _SwitchData    SwitchData;
typedef struct _ControlData   ControlData;
typedef struct _PopupData     PopupData;
typedef struct _AnimationData AnimationData;
typedef struct _AnimationItem AnimationItem;

 
/*  Panel record defines and structure declarations  */

#define PANEL_KEYWORD_COUNT	13

#define PANEL_NAME			 0
#define PANEL_GEOMETRY			 1
#define PANEL_DISPLAY_HANDLES		 2
#define PANEL_DISPLAY_MENU		 3
#define PANEL_DISPLAY_MINIMIZE		 4
#define PANEL_RESOLUTION		 5
#define PANEL_CONTROL_BEHAVIOR		 6
#define PANEL_DISPLAY_CONTROL_LABELS	 7
#define PANEL_SUBPANEL_UNPOST		 8
#define PANEL_LOCKED			 9
#define PANEL_HELP_STRING		10
#define PANEL_HELP_VOLUME		11
#define PANEL_HELP_TOPIC		12

struct _PanelData
{
   ElementValue * element_values;

   BoxData ** box_data;
   int        box_data_count;

   Widget shell;
   Widget form;
   Widget left_handle;
   Widget right_handle;
   Widget menu;
   Widget iconify;

   int resolution;
   unsigned int main_icon_size;
   unsigned int sub_icon_size;
   unsigned int switch_icon_size;
   XmFontList font_list;
   XmFontList date_font_list;
   int color_use;

   XmPixelSet * pixel_set;
   XmPixelSet * active_pixel_set;
   XmPixelSet * inactive_pixel_set;
   XmPixelSet * primary_pixel_set;
   XmPixelSet * secondary_pixel_set;
   int          pixel_set_count;

   Cursor busy_cursor;
   ControlData * busy_light_data;
   XtPointer global_data;
   XtPointer screen_data;
   PopupData * popup_data;

   XtPointer push_recall_list;
   int       push_recall_count;
   int       max_push_recall_count;

   XtPointer embedded_client_list;
   int       embedded_client_count;
   int       max_embedded_client_count;

   AnimationData * animation_data;
   int animation_count;

   DynamicComponent ** dynamic_data_list;
   int dynamic_data_count;
   int max_dynamic_data_count;

   Boolean use_color_icons;

   int switch_row_count;
   char * app_name;
};



/************************************************************************
 *
 *  panel -- This global contains the top of the frontpanel data, widgets,
 *           and any other data defining the layout/content/processing.
 *
 ************************************************************************/

extern PanelData panel;

#define O_Shell(o)	(panel.shell)


/*  Box record defines and structure declarations  */

#define BOX_KEYWORD_COUNT	8

#define BOX_NAME		0
#define BOX_CONTAINER_NAME	1
#define BOX_POSITION_HINTS	2
#define BOX_LOCKED		3
#define BOX_DELETE		4
#define BOX_HELP_STRING		4
#define BOX_HELP_VOLUME		6
#define BOX_HELP_TOPIC		7

struct _BoxData
{
   ElementValue * element_values;

   ControlData ** control_data;
   int            control_data_count;

   Widget form;
   Widget left_arrow_form;
   Widget left_control_form;
   Widget right_arrow_form;
   Widget right_control_form;

   SwitchData * switch_data;
   Widget switch_form;
   Widget switch_edit;

   int subpanel_count;
};



/*  Subpanel record defines and structure declarations  */

#define SUBPANEL_KEYWORD_COUNT	9

#define SUBPANEL_NAME			0
#define SUBPANEL_CONTAINER_NAME		1
#define SUBPANEL_CONTROL_INSTALL	2
#define SUBPANEL_TITLE			3
#define SUBPANEL_LOCKED			4
#define SUBPANEL_DELETE			5
#define SUBPANEL_HELP_STRING		6
#define SUBPANEL_HELP_VOLUME		7
#define SUBPANEL_HELP_TOPIC		8

struct _SubpanelData
{
   ElementValue * element_values;

   ControlData ** control_data;
   int            control_data_count;

   ControlData * parent_control_data;
   ControlData * default_control;

   Widget shell;
   Widget form;
   Widget dropzone;
   Widget separator;

   Widget main_panel_icon_copy;

   Boolean  torn;
   Position posted_x;
};


/*  Switch record defines and structure declarations  */

#define SWITCH_KEYWORD_COUNT	9

#define SWITCH_NAME		0
#define SWITCH_CONTAINER_NAME	1
#define SWITCH_POSITION_HINTS	2
#define SWITCH_NUMBER_OF_ROWS	3
#define SWITCH_LOCKED           4
#define SWITCH_DELETE           5
#define SWITCH_HELP_STRING	6
#define SWITCH_HELP_VOLUME	7
#define SWITCH_HELP_TOPIC	8

struct _SwitchData
{
   ElementValue * element_values;

   ControlData ** control_data;
   int            control_data_count;

   BoxData * box_data;

   Widget   rc;
   Widget * buttons;

   Atom  * atom_names;
   char ** switch_names;
   int     switch_count;
   int     active_switch;

   PopupData * popup_data;

   Time    time_stamp;
};


/*  Control record defines and structure declarations  */

#define CONTROL_KEYWORD_COUNT	23

#define CONTROL_NAME		 0
#define CONTROL_TYPE		 1
#define CONTROL_CONTAINER_TYPE	 2
#define CONTROL_CONTAINER_NAME	 3
#define CONTROL_POSITION_HINTS	 4
#define CONTROL_NORMAL_ICON	 5
#define CONTROL_ALTERNATE_ICON	 6
#define CONTROL_LABEL		 7
#define CONTROL_PUSH_ACTION	 8
#define CONTROL_PUSH_ANIMATION	 9
#define CONTROL_DROP_ACTION	10
#define CONTROL_DROP_ANIMATION	11
#define CONTROL_PUSH_RECALL	12
#define CONTROL_MONITOR_TYPE	13
#define CONTROL_CLIENT_NAME	14
#define CONTROL_CLIENT_GEOMETRY	15
#define CONTROL_FILE_NAME	16
#define CONTROL_DATE_FORMAT	17
#define CONTROL_LOCKED		18
#define CONTROL_DELETE		19
#define CONTROL_HELP_STRING	20
#define CONTROL_HELP_VOLUME	21
#define CONTROL_HELP_TOPIC	22

struct _ControlData
{
   ElementValue * element_values;

   XtPointer      parent_data;
   char           parent_type;
   SubpanelData * subpanel_data;

   Widget icon;
   Widget arrow;
   Widget arrow_separator;
   Widget indicator;

   PanelActionData ** actions;

   Boolean is_action;

   char * move_action;
   char * copy_action;
   char * link_action;

   unsigned char operation;
};



/*  Popup structure declarations  */

struct _PopupData
{
   Widget popup;
   Widget popup_title;
   Widget separator1;
   Widget separator2;
   Widget deinstall_item;
   Widget toggle_item;
   Widget modify_subpanel_item;
   Widget add_ws_item;
   Widget help_item;
   Widget delete_ws_item;
   Widget rename_ws_item;
   Widget * action_item;
   long action_count;
   Boolean subpanel_add_state;
};


/*  Animation structure declarations  */

struct _AnimationData
{
   String name;
   int item_count;
   AnimationItem * items;
};

struct _AnimationItem
{
   String image_name;
   int    delay;
};



/************************************************************************
 *
 *  External function declarations.
 *
 ************************************************************************/


extern Boolean FrontPanelReadDatabases (void);
extern void InitParse(char *, ElementValue **);
extern void InitializeControlFields( ElementValue *element_values, char *);
extern void InitializeSubpanelFields (ElementValue * element_values);
extern void _WriteControlElementValues(ElementValue *);
extern void WriteControlComponentFile(ControlData *);
extern void WriteSubpanelComponentFile(SubpanelData *);
extern void RemoveEntry (RecordData * record_data, int record_type);
extern void RemoveControlComponentFile(ControlData *);
extern void RemoveSubpanelComponentFile(SubpanelData *);
extern void    SessionAddFileData (char *, char *, int, char *, int, Boolean);
extern void    SessionDeleteFileData (char *);
extern void    SessionDeleteAll (void);
extern char *  SessionFileNameLookup (char *, int, char *, int);
extern void PanelTopicHelpCB(Widget, XtPointer, XtPointer);
extern void BoxTopicHelpCB(Widget, XtPointer, XtPointer);
extern void SubpanelTopicHelpCB(Widget, XtPointer, XtPointer);
extern void SwitchTopicHelpCB(Widget, XtPointer, XtPointer);
extern void ControlTopicHelpCB(Widget, XtPointer, XtPointer);
extern void GeneralTopicHelpCB(Widget, XtPointer, XtPointer);




/*
 * macro to get message catalog strings
 */

extern char *_DtGetMessage(char *filename, int set, int n, char *s);

#ifndef NO_MESSAGE_CATALOG
# ifdef __ultrix
#  define _FP_CLIENT_CAT_NAME "fp.cat"
# else  /* __ultrix */
#  define _FP_CLIENT_CAT_NAME "fp"
# endif /* __ultrix */
# define FPGETMESSAGE(set, number, string)\
    _DtGetMessage(_FP_CLIENT_CAT_NAME, set, number, string)
#else
# define FPGETMESSAGE(set, number, string)\
    string
#endif


#endif /* _databaseload_h */
/*  DON"T ADD ANYTHING AFTER THIS #endif  */
