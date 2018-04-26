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
/* $XConsortium: FileMgr.h /main/6 1996/10/07 13:45:03 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           FileMgr.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the main window of the
 *                   File Manager.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _FileMgr_h
#define _FileMgr_h
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


/*  Resource names  */

#define SELECTION_LIST		"selectionList"
#define BRANCH_LIST		"branchList"

/*  Used in all the dialogs as its userData value for OK and Cancel buttons */

#define FLAG_SET   0x22
#define FLAG_RESET 0x33

/* Menu button sensitivity defines */

#define RENAME         (1 << 1)
#define MOVE           (1 << 2)
#define DUPLICATE      (1 << 3)
#define LINK           (1 << 4)
#define TRASH          (1 << 5)
#define MODIFY         (1 << 6)
#define CHANGEDIR      (1 << 7)
#define PREFERENCES    (1 << 8)
#define FILTER         (1 << 9)
#define FIND           (1 << 10)
#define CREATE_DIR     (1 << 11)
#define CREATE_FILE    (1 << 12)
#define SETTINGS       (1 << 13)
#define PUTBACK        (1 << 14)
#define PUT_ON_DESKTOP (1 << 15)
#define CLEAN_UP       (1 << 16)
#define MOVE_UP        (1 << 17)
#define HOME           (1 << 18)
#define CHANGE_DIR     (1 << 19)
#define TERMINAL       (1 << 20)
/* CLEAN_UP_OP is not really a menu state.
   It's a flag to let GetFileData in FileMgr.c (call when the directory
   is being reread) not to reload icon positions from the .!dt<userid> file.
*/
#define CLEAN_UP_OP    (1 << 21)


#define NOT_DESKTOP         0
#define DESKTOP             1
#define NOT_DESKTOP_DIR     2
#define FM_POPUP            3
#define TRASH_DIRECTORY     4

/* the number of columns standard in the fast change to text widget */
#define FAST_CHANGE_COLUMNS  35

/* Forward structure pointer declaration */
typedef struct _ObjectPosition * ObjectPtr;

/*  Structure for containing the data for 1 file  */

typedef struct _FileData
{
   struct _FileData * next;
   char * file_name;             /* name of the file */
   char * action_name;		 /* if it is an Action, the LABEL name */
   unsigned char physical_type;  /* file, directory, device, pipe, ... */
   char *        logical_type;   /* data type from data typing db */
   int errnum;                   /* error code */
   struct stat stat;             /* file status */
   char * link;                  /* value of symbolic link */
   char * final_link;            /* last of a chain of symbolic links */
   Boolean is_subdir;            /* directory other than "." or ".." */
   Boolean is_broken;            /* broken link */
} FileData;

typedef struct _FileData2
{
   struct _FileData2 * next;
   short file_name;              /* name of the file */
   short action_name;		 /* if it is an Action, the LABEL name */
   unsigned char physical_type;  /* file, directory, device, pipe, ... */
   short logical_type;   /* data type from data typing db */
   int errnum;                   /* error code */
   struct stat stat;             /* file status */
   short link;                   /* value of symbolic link */
   short final_link;             /* last of a chain of symbolic links */
   Boolean is_subdir;            /* directory other than "." or ".." */
   Boolean is_broken;            /* broken link */
   char text[BUFSIZ];
} FileData2;


/* position info read from the position info file */
typedef struct
{
   char *name;
   Position x;
   Position y;
   int stacking_order;
} PositionInfo;


/* tree branch expansion states */
typedef enum {
  tsNotRead,     /* directory not yet read */
  tsError,       /* error reading directory */
  tsReading,     /* directory currently being read for the first time */
  tsNone,        /* no contents shown */
  tsDirs,        /* only sub directories shown */
  tsAll          /* all contents shown */
} TreeShow;

typedef struct _FileViewData
{
   XtPointer directory_set;
   FileData * file_data;
   struct _FileViewData *parent;    /* ptr to dir containing this entry */
   struct _FileViewData *next;      /* ptr to next entry in the same dir */
   struct _FileViewData *desc;      /* if this is a dir: list of entries */
   Boolean filtered;             /* True if file is filtered */
   Boolean selected;             /* True if file is selected */
   Boolean displayed;            /* False if filtered or collapsed tree branch*/
   TreeShow ts;                  /* how much to show in tree mode */
   int ndir;                     /* no of sub directories */
   int nfile;                    /* no of non-dir files */
   int nnew;                     /* no of new files */
   Widget widget;                /* icon gadget for this entry */
   Widget treebtn;               /* button gadget for tree branch expand */
   Boolean need_update;          /* False until widget created and up-to-date */
   Boolean registered;           /* True if drop zone registered */
   Position x, y;                /* bottom left corner of icon gadget */
   char *label;                  /* icon label string */
   long icon_mtime;              /* modified time of instance icon */
   ObjectPtr position_info;
} FileViewData;


/*  Structure for containing a set of directories  */

typedef struct
{
   char          * name;
   FileViewData  * sub_root;           /* root of the sub-tree */
   int             file_count;
   FileViewData ** file_view_data;
   FileViewData ** order_list;
   int             filtered_file_count;
   int             invisible_file_count;
   XtPointer       file_mgr_data;
} DirectorySet;


/* Structure for containing desktop object position information */

typedef struct _ObjectPosition {
   char * name;
   Position x;
   Position y;
   Boolean in_use;
   Boolean late_bind;
   int stacking_order;
   FileViewData * file_view_data;
   ObjectPtr next;
   ObjectPtr prev;
} ObjectPosition;

/*  External definitions for the file manager dialog class  */

extern DialogClass * fileMgrClass;

typedef enum
{
  not_busy = 0,        /* not busy */
  busy_readdir,        /* busy reading directory */
  initiating_readdir   /* inside call to ReadDir */
} BusyStatus;

typedef struct
{
   /* first 5 fields must be identical to DialogInstanceData struct */
   Boolean   displayed;
   Position  x;
   Position  y;
   Dimension width;
   Dimension height;

   Boolean   mapped;
   short busy_status;
   int busy_detail;

   unsigned char show_type;     /* single directory or directory tree? */
   unsigned char tree_files;    /* show only subdirs or files as well? */
   short tree_preread_level;    /* how many levels to pre-read */
   short tree_show_level;       /* how many levels to show initially */

   int           restoreKind;

   char * host;
   char * current_directory;
   char ** branch_list;

   unsigned char view;              /* current view */
   unsigned char view_single;       /* view for singlge directory */
   unsigned char view_tree;         /* view for tree mode */
   unsigned char order;             /* sort order */
   unsigned char direction;         /* sort direction */
   unsigned char positionEnabled;   /* random positioning vs sorted grid */

   Boolean newSize;
   char * restricted_directory;
   char * title;
   char * helpVol;
   Boolean toolbox;
   Boolean dropSite;
   Boolean IsTrashCan;

   int             directory_count;
   DirectorySet ** directory_set;
   FileViewData  * tree_root;           /* root of the tree */

   FileViewData  * renaming;

   FileViewData ** selection_list;
   char ** selection_table;
   int selected_file_count;

   Boolean       show_iconic_path;
   Boolean       show_current_dir;
   Boolean       show_status_line;
   Boolean       fast_cd_enabled;
   Boolean       show_hid_enabled;

   char *special_msg;
   XtIntervalId msg_timer_id;

   Boolean       cd_inited;
   XFontStruct * cd_font;
   XFontSet      cd_fontset;
   XmFontType    cd_fonttype;
   GC            cd_normal_gc;
   GC            cd_select_gc;
   char          *cd_select;

   GC            tree_solid_thin_gc;
   GC            tree_solid_thick_gc;
   GC            tree_dash_thin_gc;
   GC            tree_dash_thick_gc;

   XtPointer file_mgr_rec;
   FileViewData * popup_menu_icon;
   FileViewData * drag_file_view_data;

   DialogData * find;
   DialogData * attribute;
   DialogData * change_dir;
   DialogData * preferences;
   DialogData * filter_active;
   DialogData * filter_edit;
   DialogData ** attr_dialog_list;
   int           attr_dialog_count;
   DialogData ** secondaryHelpDialogList;
   int           secondaryHelpDialogCount;
   DialogData *  primaryHelpDialog;

   ObjectPosition ** object_positions;
   int              num_objects;
   Dimension        grid_height;
   Dimension        grid_width;

   /* When user creates new file or directory, we want to store
      away the new file info so we can scroll to it later.
   */
   char * scrollToThisFile;
   char * scrollToThisDirectory;

   XtPointer layout_data;
   char * desktop_file;
} FileMgrData, * FileMgrDataPtr;


typedef struct
{
   Widget shell;
   Widget main;
   Widget menu;
   Widget header_frame;
   Widget header_separator;
   Widget iconic_path_da;
   Widget current_directory_frame;
   Widget current_directory_drop;
   Widget current_directory_icon;
   Widget current_directory;
   Widget current_directory_text;
   Widget status_line;
   Widget scroll_window;
   Widget horizontal_scroll_bar;
   Widget vertical_scroll_bar;
   Widget file_window;
   Widget actions;
   Widget action_pane;
   Widget directoryBarBtn;
   Dimension iconic_path_width;
   char * action_pane_file_type;
   unsigned int menuStates;
   Widget focus_widget;
   Widget create_directoryBtn_child;
   Widget create_dataBtn_child;
   Widget findBtn_child;
   Widget change_directoryBtn_child;
   Widget preferencesBtn_child;
   Widget defaultEnvBtn_child;
   Widget filterBtn_child;
} FileMgrRec;

typedef struct
{
   char * action_pane_file_type;
   Widget menu;
   Widget title;
   Widget wsPopup[6];
   Widget objPopup[6];
   Widget trash_objPopup[2];
   Widget action_separator;
} FileMgrPopup;

typedef struct
{
   Widget dialog_widget;
   Widget menu_widget;
   Widget dir_text_widget;
   Widget file_text_widget;
   Widget file_type_widget;
   Widget Ok;
   Widget Cancel;
   Widget Apply;
   Widget Help;
   Widget Misc;
   int    original_physical_type;
   char * original_logical_type;
   char * file_name;
   char * directory_name;
   char * host_name;
   int mode;
   FileMgrRec * file_mgr_rec;
   char * previous_file_type;
   FileMgrData * file_mgr_data;
} DialogCallbackStruct;

/*  Local structure used for passing callback data  */

typedef struct
{
   Widget w;
   DialogCallbackStruct *call_struct;
   unsigned char type;
   unsigned int btnMask;
   char *new_name;
} MakeFileDoneData;


typedef struct
{
   Widget w;
   DialogCallbackStruct *call_struct;
   int mode;
   unsigned int btnMask;
} RenameDoneData;

/* data for doing icon update and layout */
typedef struct
{
  /* ordered list of all files */
  FileViewData ** order_list;
  int order_count;

  /* icon layout parameters */
  Dimension width;
  Dimension height;
  Dimension pixmap_width;
  Dimension pixmap_height;
  Dimension char_width;
  Dimension margin;
  Dimension spacing;
  Dimension highlight;
  unsigned char alignment;
  unsigned char pixmap_position;
  Boolean icon_top;
  int treebtn_size;

  /* colors to use */
  Pixel background;
  Pixel foreground;
  Pixel pixmap_back;
  Pixel pixmap_fore;
#ifdef _SHOW_LINK
  Pixel topshadow;
#endif

  /* widgets to be re-used */
  Widget *reuse_icons;
  Widget *reuse_btns;
  Widget *next_icon_to_use;
  Widget *next_btn_to_use;
  Widget dup_icon_widget;
  Widget *manage;
  int manage_count;

  /* DisplayWorkProc */
  XtWorkProcId work_id;
  Widget drop_site_w;
  int ex, ey;
  Boolean focus_done;
  Boolean visible_done;
  Boolean all_done;
  int i_do_next_vis;
  int i_do_next_all;
} IconLayoutData;




#define FM_POPUP_CHILDREN    17
#define FM_POPUP_CHILDREN_NA 15

#define WS_BTNS              6
#define OBJ_BTNS             6
#define OBJ_BTNS_NA          4
#define TRASH_OBJ_BTNS       2

#define BTN_PROPERTIES       0

/* white space buttons */
#define BTN_FIND             1
#define BTN_CLEANUP          2
#define BTN_SELECTALL        3
#define BTN_UNSELECTALL      4
#define BTN_SHOWHIDDEN       5

/* object buttons */
#define BTN_PUTON            1
#define BTN_TRASH            2
#define BTN_HELP             3

/* trash object buttons */
#define BTN_RESTORE          0
#define BTN_REMOVE           1

#endif /* _FileMgr_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
