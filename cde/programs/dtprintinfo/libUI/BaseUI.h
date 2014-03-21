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
/* $TOG: BaseUI.h /main/5 1998/04/06 13:31:37 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef BASEUI_H
#define BASEUI_H

#include <stdio.h>

#ifndef _BOOLEAN_
#define _BOOLEAN_
#if (defined(sun) && OSMAJORVERSION <= 5 && OSMINORVERSION <= 3)|| defined(USL) || defined(__uxp__)
  #include <sys/types.h>
  #define boolean boolean_t
  #define true B_TRUE
  #define false B_FALSE
#elif defined(sun)
  #include <sys/types.h>
  #define boolean boolean_t
#if defined(__XOPEN_OR_POSIX)
  #define true _B_TRUE
  #define false _B_FALSE
#else
  #define true B_TRUE
  #define false B_FALSE
#endif
#elif defined(linux)
  #define false 0
  #define true 1
  #define boolean int
#elif defined(CSRG_BASED)
#include <stdbool.h>
#define boolean bool
#else
  typedef enum
  {
     false = 0,
     true = 1
  } boolean;
#endif
#endif

#ifndef STRDUP
#define STRDUP(string) (string ? strdup(string) : (char*) NULL)
#define STRCMP(s1, s2) (s1 && s2 ? strcmp(s1, s2) : (s1 ? 1 : -1))
#define STRLEN(string) (string ? strlen(string) : 0)
#endif

typedef enum
{
   BASE_UI,
   APPLICATION,
   BUTTON,
   COMBO_BOX,
   CONTAINER,
   DIALOG,
   LABEL,
   LIST,
   GROUP,
   HELP_SYSTEM,
   ICON,
   MAIN_WINDOW,
   MENU,
   MENU_BAR,
   PROMPT,
   SEPARATOR,
   SPIN_BUTTON,
   SCALE,
   TEXT
} UI_Class;

typedef enum
{
   MODELESS,
   MODAL,
   INFORMATION,
   ERROR,
   WORK_IN_PROGRESS,
   FILE_SELECTION,           // This type is always modal
   DIRECTORY_SELECTION,      // This type is always modal
   FILE_DIRECTORY_SELECTION, // This type is always modal
   QUESTION,                 // This type is always modal
   WARNING,                  // This type is always modal
   PROMPT_DIALOG,            // This type is always modal
   MODAL_WORK_IN_PROGRESS,
   MODAL_ERROR,
   MODAL_INFORMATION
} DialogType;

typedef enum
{
   PULLDOWN_MENU,
   OPTION_MENU,
   POPUP_MENU
} MenuType;

typedef enum
{
   LEFT_JUSTIFIED,
   CENTERED,
   RIGHT_JUSTIFIED
} LabelType;

typedef enum
{
   VERTICAL_SCALE,
   HORIZONTAL_SCALE
} ScaleType;

typedef enum
{
   FIXED_LIST,
   SCROLLED_LIST
} ListType;

typedef enum
{
   READ_ONLY,
   EDITABLE,
   SCROLLED_READ_ONLY,
   SCROLLED_EDITABLE
} TextType;

typedef enum
{
   SINGLE_SELECT,
   MULTIPLE_SELECT, 
   EXTENDED_SELECT,
   BROWSE_SELECT
} SelectionType;

typedef enum
{
   STRING_PROMPT, 
   MULTI_LINE_STRING_PROMPT, 
   INTEGER_PROMPT,
   REAL_PROMPT
} PromptType;

typedef enum
{
   CHECK_BOX, 
   RADIO_GROUP,
   HORIZONTAL_CHECK_BOX, 
   HORIZONTAL_RADIO_GROUP,
   FORM_BOX
} GroupType;

typedef enum
{
   CANVAS,
   FORM,
   VERTICAL_ROW_COLUMN,
   HORIZONTAL_ROW_COLUMN,
   PANE,
   WORK_AREA,
   ICON_LIST,
   SCROLLED_CANVAS,
   SCROLLED_FORM,
   SCROLLED_VERTICAL_ROW_COLUMN,
   SCROLLED_HORIZONTAL_ROW_COLUMN,
   SCROLLED_PANE,
   SCROLLED_WORK_AREA,
   SCROLLED_ICON_LIST
} ContainerType;

typedef enum
{
   PUSH_BUTTON, 
   TOGGLE_BUTTON, 
   UP_ARROW_BUTTON, 
   DOWN_ARROW_BUTTON, 
   LEFT_ARROW_BUTTON, 
   RIGHT_ARROW_BUTTON
} ButtonType;

typedef enum
{
   NO_LINE, 
   SINGLE_LINE, 
   DOUBLE_LINE, 
   SINGLE_DASHED_LINE, 
   DOUBLE_DASHED_LINE, 
   SHADOW_ETCHED_IN, 
   SHADOW_ETCHED_OUT, 
   SHADOW_ETCHED_IN_DASH, 
   SHADOW_ETCHED_OUT_DASH
} SeparatorStyle;

typedef enum
{
   AS_PLACED, 
   GRID, 
   BROWSER,
   TREE, 
   PROPERTIES
} ViewStyle;

typedef enum
{
   NAME_ONLY, 
   VERY_LARGE_ICON, 
   LARGE_ICON, 
   MEDIUM_ICON, 
   SMALL_ICON, 
   TINY_ICON, 
   DETAILS
} IconStyle;

typedef enum
{
   LEFT_SLANTED_ARROW_CURSOR, 
   RIGHT_SLANTED_ARROW_CURSOR, 
   WATCH_CURSOR, 
   HOUR_GLASS_CURSOR, 
   IBEAM_CURSOR, 
   CROSS_HAIRS_CURSOR
} PointerCursor;

class BaseUI;

typedef boolean (*SelectProc) (BaseUI *object);
typedef boolean (*CompareProc) (BaseUI **first, BaseUI **second);
typedef void (*TimeOutCallback) (BaseUI *object, void *callback_data);
typedef void (*ButtonCallback) (void *);
typedef void (*DialogCallback) (void *);
typedef boolean (*ValidationCallback) (void *client_data, void *call_data);
typedef void (*ThreadCallback) (BaseUI *obj, char *output, int rc);

const int NO_SUBCLASS = -1;

class BaseUI {

 public:

#ifdef DEBUG
   static int indent;
   static boolean trace_calls;
   static void EnterFunction(char *message);
   static void LeaveFunction(char *message);
#define TraceFunctionCalls(flag) trace_calls = flag
#else
#define EnterFunction(message)
#define LeaveFunction(message)
#define TraceFunctionCalls(flag)
#endif 


 protected:

   static int _UniqueID;

   char *_name;
   char *_category;
   int _id;
   BaseUI *_parent;
   BaseUI **_children;
   int _numChildren;
   BaseUI **_allChildren;
   int _numAllChildren;
   boolean _has_been_opened;
   boolean _opened;
   boolean _visible;
   boolean _active;
   boolean _selected;
   boolean _update;
   char *_update_message;
   ViewStyle _viewStyle;
   IconStyle _iconStyle;

   BaseUI(BaseUI *parent,
	  const char *name,
	  const char *category = NULL);

   // Derived classes must define these functions
   virtual boolean SetVisiblity(boolean) = 0;
   virtual boolean SetActivity(boolean) = 0;
   virtual boolean SetSelected(boolean) = 0;
   virtual boolean SetName(char *) = 0;
   virtual boolean SetCategory(char *) = 0;
   virtual boolean SetOpen(boolean) = 0;
   virtual boolean SetView(ViewStyle) = 0;
   virtual boolean SetIcon(IconStyle) = 0;
   virtual boolean SetParent(BaseUI *new_parent) = 0;
   virtual void SetAddTimeOut(TimeOutCallback, void *, long interval) = 0;
   virtual void DoRefresh() = 0;
   virtual void DoToFront() = 0;
   virtual void DoContextualHelp() = 0;
   virtual void DoBeginUpdate() = 0;
   virtual void DoEndUpdate() = 0;
   virtual void DoMakeVisible() = 0;
   virtual boolean DoIsVisible() { return true; }
   virtual void DoUpdateMessage(char *message) = 0;
   virtual boolean SetOrder(int) = 0;

   void SetVisible(boolean);
   void AddToParent();
   void DeleteFromParent();
   void AddToParentContainer();
   void DeleteFromParentContainer();
   void _Find(void *, int, int *, BaseUI ***, SelectProc, boolean,
	      boolean, boolean, int);
   BaseUI *_FindBy(char *, int, int *, BaseUI ***, SelectProc, boolean,
		   boolean, boolean);

   // These messages are sent to all parents
   // up to and including the parent container
   virtual void NotifyCreate(BaseUI *) { } ;
   virtual void NotifyDelete(BaseUI *) { } ;
   virtual void NotifyVisiblity(BaseUI *) { } ;
   virtual void NotifySelected(BaseUI *) { } ;
   virtual void NotifyOpen(BaseUI *) { } ;

 public:

   // Not Used By Subclasses, for application programmers only
   void *ApplicationData;              

   virtual ~BaseUI();                  // destructor

   virtual boolean HandleHelpRequest();
   void ContextualHelp();
   int UniqueID() { return _id; }
   boolean ObjectExists(int unique_id);

   // Set functions
   void Visible(boolean);              // Set visiblity, calls SetVisiblity
   void Active(boolean);               // Set sensitivity, calls SetActivity
   void Selected(boolean);             // Set selected status, calls SetSelected
   void Open(boolean);                 // Set opened status, calls SetOpen
   void Name(char *);                  // Set name, calls SetName
   void Category(char *);              // Set category, calls SetCategory
   void ContainerView(ViewStyle);      // Set container view, calls SetView
   void IconView(IconStyle);           // Set icon view, calls SetIcon
   void Parent(BaseUI *new_parent);    // Set parent, calls SetParent
   virtual void Details(char *) { }

   // Access functions
   const boolean Visible()          { return _visible; }
   const boolean Active()           { return _active; }
   const boolean Selected()         { return _selected; }
   const boolean Open()             { return _opened; }
   const boolean HasBeenOpened()    { return _has_been_opened; }
   const char * Name()              { return _name; }
   const char * Category()          { return _category; }
   const ViewStyle ContainerView()  { return _viewStyle; }
   const IconStyle IconView()       { return _iconStyle; }
   virtual char * Details()         { return NULL; }

   BaseUI * Parent()                { return _parent; }
   BaseUI ** Children()             { return _children; }
   int NumChildren()                { return _numChildren; }
   BaseUI ** AllChildren()          { return _allChildren; }
   int NumAllChildren()             { return _numAllChildren; }

   // These are for children
   BaseUI ** Siblings();
   int NumSiblings();
   BaseUI * ContainerParent();
   int NumContainerChildren();
   BaseUI ** ContainerChildren();
   void DeleteChildren();

   virtual void PointerShape(PointerCursor) { }
   virtual PointerCursor PointerShape() { return LEFT_SLANTED_ARROW_CURSOR; }
   virtual void AttachAll(int /*offset*/ = 0) { }
   virtual void AttachTop(int /*offset*/ = 0) { }
   virtual void AttachBottom(int /*offset */= 0) { }
   virtual void AttachLeft(int /*offset */= 0) { }
   virtual void AttachRight(int /*offset */= 0) { }
   virtual void AttachTop(   BaseUI *,
			     int /*offset*/ = 0, boolean /*opposite*/ = false) { }
   virtual void AttachBottom(BaseUI *,
			     int /*offset*/ = 0, boolean /*opposite*/ = false) { }
   virtual void AttachLeft(  BaseUI *,
			     int /*offset*/ = 0, boolean /*opposite*/ = false) { }
   virtual void AttachRight( BaseUI *,
			     int /*offset*/ = 0, boolean /*opposite*/ = false) { }
   virtual void WidthHeight(int /*width*/, int /*height*/) { }
   virtual void WidthHeight(int * /*width*/, int * /*height*/) { }
   virtual void Width(int /*width*/)   { }
   virtual int Width()             { return 0; }
   virtual void Height(int /*height*/) { }
   virtual int Height()            { return 0; }
   virtual void StringWidthHeight(const char * /*string*/, int * /*w*/,
				  int * /*h*/) { }
   virtual int StringWidth(const char * /*string*/)  { return 0; }
   virtual int StringHeight(const char * /*string*/) { return 0; }

   virtual void Thread(const char * /*cmd*/, ThreadCallback,
		       int /*buf_len*/ = 512) { }
   virtual void Thread(int /*pid*/, int /*fd*/, ThreadCallback,
		       int /*buf_len*/ = 512) { }
   virtual void Thread(int /*socket*/, ThreadCallback,
		       int /*buf_len*/ = 512)      { }

   // Add timeouts to object (interval is in terms of milliseconds)
   void AddTimeOut(TimeOutCallback, void *callback_data, long interval);
   virtual int MicroSleep(long milliseconds);
   void Refresh();
   void ToFront();
   void BeginUpdate();
   void EndUpdate();
   void MakeVisible();
   boolean IsVisible();
   void UpdateMessage(const char *message);

   BaseUI *FindByName(char *pattern,
		      int depth = 1,
		      int *n_mactches = NULL,
		      BaseUI ***matches = NULL,
		      SelectProc select_proc = NULL,
		      boolean regular_expression = false,
		      boolean case_sensitive = true);
   BaseUI *FindByCategory(char *pattern,
		          int depth = 1,
			  int *n_mactches = NULL,
			  BaseUI ***matches = NULL,
		          SelectProc select_proc = NULL,
		          boolean regular_expression = false,
		          boolean case_sensitive = true);
   void OrderByName(boolean flag);       // Calls SetOrder
   void GroupByCategory(boolean flag);   // Calls SetOrder
   int Order();                          // Returns order
   void Order(int new_position);         // Calls SetOrder

   // Dumps object to stdout
   virtual void Dump(boolean verbose = false,
		     int level = 0);
   // Dumps object heirarchy to stdout
   void DumpHierarchy(boolean verbose = false,
		      int level = 0);

   // Select/UnSelected all immediate children
   virtual void SelectAll(boolean select_status = true);

   // Select/UnSelected all children, and their children, etc...
   virtual void SelectAllDescendants(boolean select_status = true);

   // Open/Close all children, and their children, etc...
   virtual void OpenAllDescendants(boolean opened = true);

   // Get selected items
   virtual void Selection(int *n_items,
			  BaseUI ***items = NULL);

   virtual const UI_Class UIClass() { return BASE_UI; }
   virtual const int UISubClass() { return NO_SUBCLASS; }

   virtual const char *const UIClassName() { return "BaseUI"; }

};

#endif // BASEUI_H
