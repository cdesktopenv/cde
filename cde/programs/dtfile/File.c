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
/* $TOG: File.c /main/22 1999/12/09 13:05:50 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           File.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    File processing functions.
 *
 *   FUNCTIONS: ABS
 *		AddFileIcons
 *		BuildObjectPositions
 *		CommitWorkProcUpdates
 *		CompressObjectList
 *		CopyOrderedEntries
 *		CreateNameChangeDialog
 *		CreateTreeIcons
 *		DeselectAllFiles
 *		DeselectFile
 *		DestroyIconName
 *		DisplaySomeIcons
 *		DisplayWorkProc
 *		DragFinishCB
 *		DrawingAreaRedisplay
 *		DropOnRootCB
 *		EraseTreeLines
 *		EstimateIconSize
 *		FileConvertCB
 *		FileDateAscending
 *		FileDateDescending
 *		FileIconMotion
 *		FileIsSelected
 *		FileNameAscending
 *		FileNameDescending
 *		FileSizeAscending
 *		FileSizeDescending
 *		FileTypeAscending
 *		FileTypeDescending
 *		FilterFiles
 *		FindCurrentPosition
 *		FlattenTree
 *		FmPopup
 *		FreeLayoutData
 *		GetAncestorInfo
 *		GetBottomOfStack
 *		GetDirName
 *		GetDragIcon
 *		GetDragIconMask
 *		GetExtraHeight
 *		GetFullName
 *		GetIconLayoutParms
 *		GetInsertPosition
 *		GetLevel
 *		GetPositionalData
 *		GetSelectedCount
 *		GetStrcollProc
 *		GetTopOfStack
 *		GetTreebtnPixmap
 *		HorizontalScrollbarIsVisible
 *		IconCallback
 *		InMultipleObjectRegion
 *		InitiateIconDrag
 *		IntersectRects
 *		IsButtonOrMotion
 *		IsDesktopPtr
 *		LayoutDesktopIcons
 *		LayoutFileIcons
 *		MakeReuseList
 *		NewConvertDelete
 *		NewConvertFileName
 *		OrderChildrenList
 *		OrderFiles
 *		PositionDesktopIcon
 *		PositionFileView
 *		RedisplayUsingStackingOrder
 *		RedrawOneGadget
 *		RedrawTreeLines
 *		RegisterDesktopHotspots
 *		RelocateDesktopIcon
 *		ReorderChildrenList
 *		RepaintDesktop
 *		RepairFileWindow
 *		RepairStackingPointers
 *		RepositionIcons
 *		RepositionUpInStack
 *		RestorePositionalData
 *		SavePositionalData
 *		SelectAllFiles
 *		SelectFile
 *		SetFileSelected
 *		SetFileUnselected
 *		SetHotRects
 *		SetToNormalColors
 *		SetToSelectColors
 *		SpecialCases
 *		StartDrag
 *		StrCaseCmp
 *		ToBeManaged
 *		TreeBtnCallback
 *		TreeLX
 *		TreeOneWd
 *		TreeWd
 *		TypeToAction
 *		TypeToDropOperations
 *		UnmanageFileIcons
 *		UnpostTextField
 *		UnpostTextPath
 *		UpdateFileIcons
 *		UpdateOneFileIcon
 *		UpdateOneIconLabel
 *		VerticalScrollbarIsVisible
 *		WidgetCmp
 *		YSPACING
 *		_UpdateFileIcons
 *		InputForGadget
 *		InputInGadget
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <fnmatch.h>

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/MwmUtil.h>
#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#ifndef XK_MISCELLANY
#define XK_MISCELLANY
#endif
#include <X11/keysymdef.h>

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif

#include <Xm/DrawingA.h>
#include <Xm/DrawingAP.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/Screen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>

#include <Xm/DragIcon.h>
#include <Xm/DragC.h>
#include <Dt/Dnd.h>

#include <time.h>
#include <utime.h>

#include <Dt/Action.h>
#include <Dt/Connect.h>
#include <Dt/Wsm.h>
#include <Dt/DtNlUtils.h>
#include <Dt/HourGlass.h>
#include <Dt/Dts.h>
#include <Dt/UserMsg.h>
#include <Dt/SharedProcs.h>

#include <Tt/tttk.h>

#include <Xm/XmPrivate.h> /* _XmIsEventUnique _XmSetInDragMode _XmRecordEvent */

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Prefs.h"
#include "Common.h"
#include "Filter.h"
#include "Help.h"
#include "SharedMsgs.h"


extern Widget _DtDuplicateIcon ( Widget, Widget, XmString, String, XtPointer, Boolean );

/* absolute value macro */
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

#define INIT_VALUE 5

/*
 * MAXWINSIZE: maximum width of the file window
 *
 * Note: the file window width and height can't exceed the maximum value
 * for a short (32767), because x and y coords of the icon gadgets placed
 * in the file window are stored as short's by the Xt library (the
 * Position type is a short).
 *   Furthermore, somewhere in the code for registering icon drop sites
 * in libDt or libXm calculations are done that overflow if the window
 * size is too close to the maximum of 32767.  The symptom is observed
 * most easily in tree mode on a directory large enough for icons to
 * overflow into a second column: for values of MAXWINSIZE of 32767,
 * none of the icons are sensitive as drop sites any more.  The value
 * for MAXWINSIZE defined below has been found to be "safe" by experiment.
 */
#define MAXWINSIZE 32600

/********    Static Function Declarations    ********/

static int FileNameAscending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileNameDescending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileTypeAscending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileTypeDescending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileSizeAscending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileSizeDescending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileDateAscending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static int FileDateDescending(
                        FileViewData **t1,
                        FileViewData **t2) ;
static void CompressObjectList (
                        ObjectPosition ** object_positions,
                        int num_objects,
                        int starting_index) ;
static ObjectPosition *GetPositionalData (
                        FileMgrData * file_mgr_data,
                        FileViewData * object,
                        Dimension max_width,
                        Boolean create) ;
static void RedisplayUsingStackingOrder (
                        FileMgrData * file_mgr_data,
                        Widget w,
                        register XEvent *event,
                        Region region) ;
static void ReorderChildrenList (
                        XmManagerWidget file_window,
                        Widget * manage,
                        int manageCount,
                        Widget * unmanage,
                        int unmanageCount) ;
static Boolean IsDesktopPtr (
                        FileViewData * fileViewData,
                        FileMgrData ** fileMgrData,
                        DesktopRec ** desktopRec) ;
static Boolean IntersectRects(
                        Position x1,
                        Position y1,
                        Dimension w1,
                        Dimension h1,
                        Position x2,
                        Position y2,
                        Dimension w2,
                        Dimension h2 ) ;
static void InitiateIconDrag(
                        FileViewData * fileViewData,
                        int rootX,
                        int rootY,
                        XEvent * event );
static void RelocateDesktopIcon(
                        DesktopRec * desktopRec,
                        int root_x,
                        int root_y);
static void BuildObjectPositions(
                        FileMgrData *file_mgr_data);
static void moveCopyLinkCancel(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data );
static void moveCopyLinkOK(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data );
static void DropOnRootCB (
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data);
static void _UpdateFileIcons(
                        FileMgrRec *file_mgr_rec,
                        FileMgrData *file_mgr_data,
                        Boolean new_directory,
                        DirectorySet * add_dir_set );
static void CreateTreeIcons(
                        Widget w);
static void TreeBtnCallback(
                        Widget w,
                        XtPointer clientData,
                        XmAnyCallbackStruct * callData );
static void LayoutDesktopIcons(
                        FileMgrRec *file_mgr_rec,
                        FileMgrData *file_mgr_data,
                        FileViewData ** order_list,
                        int order_count,
                        Boolean turn_off_hourglass);
static XmGadget InputInGadget (
			Widget w,
                        register int x,
			register int y);
static XmGadget InputForGadget (
                        Widget cw,
                        int x,
                        int y);

/********    End Static Function Declarations    ********/

/* cursor for double-click-drag (@@@ should be a resource) */
static Cursor loc_cursor1 = None;
static Cursor loc_cursor2 = None;
#define DBLCLICK_DRAG_THRESHOLD 20

/* layout constants (@@@ could be made resources) */
#define MARGIN          5
#define XSPACING        5
#define YSPACING(fmd)                                                              \
  (fmd->layout_data?                                                               \
    (fmd->view != BY_NAME? - ((IconLayoutData *)fmd->layout_data)->highlight:      \
                           - ((IconLayoutData *)fmd->layout_data)->highlight + 2): \
    0)
#define TreeSep         5
#define TreeOffset      15
#define TreeOneWd(sz)     (TreeOffset + TreeBtnWd[sz]/2)
#define TreeLX(level,sz)  (TreeBtnWd[sz]/2 + ((level) - 1)*TreeOneWd(sz))
#define TreeWd(level,sz)  \
  (TreeLX(level,sz) + TreeOffset + TreeBtnWd[sz] + TreeSep)

/* maximum size of the small, medium, and large tree buttons */
static int TreeBtnWd[3] = { 0, 0, 0 };
static int TreeBtnHt[3] = { 0, 0, 0 };

static int TreeNilWd[3] = { 0, 0, 0 };
static int TreeNilHt[3] = { 0, 0, 0 };

/* tree expansion circle pixmap indices */
typedef enum {
  tpxNotRead,    /* dotted circle */
  tpxError,      /* crossed circle */
  tpxMore,       /* "+" circle */
  tpxLess,       /* "-" circle */
  tpxBoth,       /* "+/-" circle */
  tpxEmpty,      /* empty circle */
  tpxNil,        /* empty-set symbol */
  tpxN
} TreePxId;

/* pixmaps for expand circles in tree mode */
static struct TreePx {
  char *name;            /*   name of pixmap file */
  Pixmap px[3];          /*   small, medium, and large pixmap */
} TreePxTab[tpxN] =
{
  { "Dttvnor", 0 },
  { "Dttverr", 0 },
  { "Dttvmor", 0 },
  { "Dttvlss", 0 },
  { "Dttvbth", 0 },
  { "Dttvemp", 0 },
  { "Dttvnil", 0 },
};

static char *TreePxSuffix[3] = { ".s", ".m", ".l" };

/* value need to to convert drop position to icon placement */
static int dragIconPixmapOffsetX;
static int dragIconPixmapOffsetY;

/*  Local defines  */

#define FILE_MOVE       0
#define FILE_COPY       1
#define FILE_INVALID    2


FileMgrPopup fileMgrPopup = {NULL};


/* Obsolete Motif highlighting and unhighlighting routines */
extern void _XmHighlightBorder(Widget w);
extern void _XmUnhighlightBorder(Widget w);

extern void SelectDTFile (DesktopRec *desktopWindow);

/************************************************************************
 ************************************************************************
 *
 *      Comparison functions used for sorting the files
 *
 ************************************************************************
 ************************************************************************/
/* only use when LANG=C on platforms that don't
provide strcasecmp().   Otherwise, use strcoll() */
static int
StrCaseCmp (
              const char  *s1,
              const char  *s2)
{
  /* Note: This is not really a string case insensitive compare. So don't
     even think of replacing it with any other library routines.
  */
  char *s1a = (char *)s1, *s2a = (char *)s2;

  if (s1 == s2) return  0;

  for (; tolower(*s1) == tolower(*s2); ++s1, ++s2) {
    if (*s1 == '\0' )
    {
      /* File Manager depends heavily on this routine to position the
         icons in its window.
         We want case INSENSITIVE comparision, however when we have
         2 same size and case equivalent strings i.e.   "A" and "a", we
         need this routine to behave like a case SENSITIVE comparision, so
         the position of the icon is constant so "A" is always
         before "a".
      */
      if (*s2 == 0x0)
      {
        for (; *s1a == *s2a; ++s1a, ++s2a) {
          if (*s1 == '\0' )
            return 0;
        }
        return *s1a - *s2a;
      }
      else
        return 0;
    }
  }

  return tolower(*s1) - tolower(*s2);
}

StrcollProc FMStrcoll = NULL;

StrcollProc
GetStrcollProc(void)
{
  int Clang = 0;
#if defined(__hpux)
  struct locale_data * li;
#else
  char * locale;
#endif

#define C_LANG  "C"

   /* if locale is C, use the explicit case insensitive compare */
#if defined(__hpux)
  li = getlocale(LOCALE_STATUS);
  if ( NULL == li->LC_COLLATE_D || strcmp(C_LANG,li->LC_COLLATE_D) == 0 )
    Clang = 1;
#else
  locale = setlocale(LC_COLLATE,NULL); /* put locale in buf */
  if (strcmp(locale,C_LANG) == 0)
    Clang = 1;
#endif

  if (Clang)
    return StrCaseCmp;

  return ((StrcollProc)strcoll);
}

static Boolean
SpecialCases(
             FileViewData **t1,
             FileViewData **t2,
             int *rc )
{
  /* Tree mode */
  if (((FileMgrData *)((DirectorySet *)((*t1)->directory_set))->file_mgr_data)
      ->show_type == MULTIPLE_DIRECTORY)
  {
    if (!(*t1)->file_data->is_subdir && (*t2)->file_data->is_subdir)
      *rc = -1;
    else if ((*t1)->file_data->is_subdir && !(*t2)->file_data->is_subdir)
      *rc = 1;
    else
      return False;
    return True;
  }







  /* Special files */
  if (FMStrcoll ((*t1)->file_data->file_name, "." ) == 0 )
  {
    *rc = -1;
    return True;
  }

  if (FMStrcoll ((*t1)->file_data->file_name, "..") == 0 )
  {
    if ( FMStrcoll ((*t2)->file_data->file_name, ".") == 0 )
      *rc = 1;
    else
      *rc = -1;
    return True;
  }

  if (FMStrcoll ((*t2)->file_data->file_name, ".") == 0 || FMStrcoll ((*t2)->file_data->file_name, "..") == 0)
  {
    *rc = 1;
    return True;
  }








  /* Directories */
  if (!(*t1)->file_data->is_subdir && (*t2)->file_data->is_subdir)
  {
    *rc = 1;
    return True;
  }

  if ((*t1)->file_data->is_subdir && !(*t2)->file_data->is_subdir)
  {
    *rc = -1;
    return True;
  }

  return False;
}

static int
FileNameAscending(
        FileViewData **t1,
        FileViewData **t2 )
{
  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

  if((*t1)->file_data->action_name == NULL)
  {
     if((*t2)->file_data->action_name == NULL)
        rc = FMStrcoll((*t1)->file_data->file_name,
                                              (*t2)->file_data->file_name);
     else
        rc = FMStrcoll((*t1)->file_data->file_name,
                                              (*t2)->file_data->action_name);
  }
  else
  {
     if((*t2)->file_data->action_name != NULL)
        rc = FMStrcoll((*t1)->file_data->action_name,
                                              (*t2)->file_data->action_name);
     else
        rc = FMStrcoll((*t1)->file_data->action_name,
                                              (*t2)->file_data->file_name);
  }

  return rc;
}

static int
FileNameDescending(
        FileViewData **t1,
        FileViewData **t2 )
{
   int rc;

   if( SpecialCases( t1, t2, &rc ) )
     return rc;

   if((*t1)->file_data->action_name == NULL)
   {
      if((*t2)->file_data->action_name == NULL)
         rc = FMStrcoll((*t1)->file_data->file_name,
                                              (*t2)->file_data->file_name);
      else
         rc = FMStrcoll((*t1)->file_data->file_name,
                                              (*t2)->file_data->action_name);
   }
   else
   {
      if((*t2)->file_data->action_name != NULL)
         rc = FMStrcoll((*t1)->file_data->action_name,
                                              (*t2)->file_data->action_name);
      else
         rc = FMStrcoll((*t1)->file_data->action_name,
                                              (*t2)->file_data->file_name);
   }

   if (rc <= -1)
     return 1;

   if (rc >= 1)
     return -1;

   return 0;
}

static int
FileTypeAscending(
        FileViewData **t1,
        FileViewData **t2 )
{
  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

  rc = strcoll( (*t1)->file_data->logical_type, (*t2)->file_data->logical_type );
  if( rc == 0 )
    rc = FileNameAscending( t1, t2 );
  return rc;
}


static int
FileTypeDescending(
        FileViewData **t1,
        FileViewData **t2 )
{
  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

   rc = strcoll( (*t1)->file_data->logical_type, (*t2)->file_data->logical_type );

   if (rc <= -1)
     return 1;

   if (rc >= 1)
     return -1;

   rc = FileNameDescending( t1, t2 );

   return rc;
}

static int
FileSizeAscending(
        FileViewData **t1,
        FileViewData **t2 )
{
  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

  if ((*t1)->file_data->stat.st_size < (*t2)->file_data->stat.st_size)
    return -1;

  else if ((*t1)->file_data->stat.st_size > (*t2)->file_data->stat.st_size)
    return 1;

  return 0;
}

static int
FileSizeDescending(
        FileViewData **t1,
        FileViewData **t2 )
{
  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

  if ((*t1)->file_data->stat.st_size < (*t2)->file_data->stat.st_size)
    return 1;

  if ((*t1)->file_data->stat.st_size > (*t2)->file_data->stat.st_size)
    return -1;

  return 0;
}

static int
FileDateAscending(
        FileViewData **t1,
        FileViewData **t2 )
{
  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

  if ((*t1)->file_data->stat.st_mtime < (*t2)->file_data->stat.st_mtime)
    return -1;

  if ((*t1)->file_data->stat.st_mtime > (*t2)->file_data->stat.st_mtime)
    return 1;

  return 0;
}

static int
FileDateDescending(
        FileViewData **t1,
        FileViewData **t2 )
{

  int rc;

  if( SpecialCases( t1, t2, &rc ) )
    return rc;

  if ((*t1)->file_data->stat.st_mtime < (*t2)->file_data->stat.st_mtime)
    return 1;

  if ((*t1)->file_data->stat.st_mtime > (*t2)->file_data->stat.st_mtime)
    return -1;

  return 0;
}





/************************************************************************
 *
 *  OrderFiles
 *      Sort the file display list according to the ordering data.
 *
 ************************************************************************/

void
OrderFiles(
        FileMgrData *file_mgr_data,
        DirectorySet *directory_set )
{
   FileViewData ** file_view_data;
   int             file_count;
   FileViewData ** order_list;
   int * sort;
   int * sub_sort;
   register int i;
   register int start;

   file_view_data = directory_set->file_view_data;
   file_count = directory_set->file_count;


   /*  Allocate an ordering list  */

   if (directory_set->order_list != NULL)
   {
      XtFree ((char *) directory_set->order_list);
       directory_set->order_list = NULL;
   }

   if (file_count > 0)
      directory_set->order_list = order_list =
         (FileViewData **) XtMalloc (sizeof (FileViewData **) * file_count);
   else
      directory_set->order_list = order_list = NULL;

   /*  Get pointers to all of the file data into the order list  */

   for (i = 0; i < file_count; i++)
      order_list[i] = file_view_data[i];

   /*  Set up the sorting functions according to the order and direction.  */

   sub_sort = NULL;

   if (file_mgr_data->order == ORDER_BY_FILE_TYPE)
   {
      if (file_mgr_data->direction == DIRECTION_ASCENDING)
      {
         sort = (int *) FileTypeAscending;
         sub_sort = (int *) FileNameAscending;
      }
      else
      {
         sort = (int *) FileTypeDescending;
         sub_sort = (int *) FileNameDescending;

      }
   }
   else if (file_mgr_data->order == ORDER_BY_ALPHABETICAL)
   {
      if (file_mgr_data->direction == DIRECTION_ASCENDING)
         sort = (int *) FileNameAscending;
      else
         sort = (int *) FileNameDescending;
   }
   else if (file_mgr_data->order == ORDER_BY_DATE)
   {
      if (file_mgr_data->direction == DIRECTION_ASCENDING)
         sort = (int *) FileDateAscending;
      else
         sort = (int *) FileDateDescending;

   }
   else if (file_mgr_data->order == ORDER_BY_SIZE)
   {
      if (file_mgr_data->direction == DIRECTION_ASCENDING)
         sort = (int *) FileSizeAscending;
      else
         sort = (int *) FileSizeDescending;
   }


   /*  Sort the files and if the sub_sort function is non-null,  */
   /*  sort sets of the files broken according to file type.     */

   qsort (order_list, file_count, sizeof (FileViewData *), (int (*)())sort);

   if (sub_sort != NULL)
   {
      start = 0;
      i = 0;

      while (i < file_count)
      {
         if (order_list[start]->file_data->logical_type !=
             order_list[i]->file_data->logical_type)
         {
            qsort (order_list + start, i - start,
                   sizeof (FileViewData *), (int (*)())sub_sort);
            start = i;
         }

         i++;
      }

      qsort (order_list + start, i - start, sizeof (FileViewData *),
                                                         (int (*)())sub_sort);
   }
}



/************************************************************************
 *
 *  FilterFiles
 *      Filter out the files which do not match the filtering criteria.
 *
 *      The `mustMatch' flag is used to determine whether the user has
 *      requested that the files which match the specification are to
 *      be displayed, or instead, filtered out.
 *
 ************************************************************************/

void
FilterFiles(
        FileMgrData *file_mgr_data,
        DirectorySet *directory_set )
{
   FileViewData **file_view_data;
   FilterData *   filter_data;
   register int   i, j, k;
   Boolean        show_hidden;
   String         filter;
   Boolean        mustMatch, matches;
   int            filterCount = 0;
   int            invisibleCount = 0;
   FileViewData  *sub_root;
#ifdef DT_PERFORMANCE
   struct timeval update_time_s;
   struct timeval update_time_f;

   printf("   Begin FilterFiles\n");
   gettimeofday(&update_time_s, NULL);

    /* added by Rafi */
    _DtPerfChkpntMsgSend("Begin Filtering Files");

#endif

   file_view_data = directory_set->file_view_data;

   filter_data = (FilterData *)(file_mgr_data->filter_active->data);
   show_hidden = filter_data->show_hidden;
   filter = filter_data->filter;
   mustMatch = filter_data->match_flag;

   /* set the show hidden boolean depending on the filter specification */
   if (show_hidden)
      file_mgr_data->show_hid_enabled = True;
   else
      file_mgr_data->show_hid_enabled = False;

   /*  Filter out all files not matching the specifications  */

   for (i = 0; i < directory_set->file_count; i++)
   {
      /* Initially assume the file is not filtered out */

      file_view_data[i]->filtered = False;

      /* If in tree mode, explicitly filter out . and ..  */
      if (file_mgr_data->show_type == MULTIPLE_DIRECTORY &&
         (strcmp(file_view_data[i]->file_data->file_name, ".") == 0 ||
          strcmp(file_view_data[i]->file_data->file_name, "..") == 0))
      {
         filterCount++;
         file_view_data[i]->filtered = True;
         continue;
      }

      /* filter out any files that have their attributes "invisible"   */
      /* field set to false                                            */
      if((_DtCheckForDataTypeProperty(
             file_view_data[i]->file_data->logical_type,
             "invisible")) &&
         (file_mgr_data != trashFileMgrData))
      {
         filterCount++;
         file_view_data[i]->filtered = True;
         ++invisibleCount;
         continue;
      }

      /* Filter hidden files, according to the user setting */

      if (file_view_data[i]->file_data->file_name[0] == '.')
      {
         if(strcmp(file_mgr_data->current_directory, "/") == 0  &&
              strcmp(file_view_data[i]->file_data->file_name, "..") == 0)
         {
            filterCount++;
            file_view_data[i]->filtered = True;
            continue;
         }
         if(file_mgr_data->restricted_directory != NULL)
         {
           if((strcmp(file_mgr_data->restricted_directory,
                            file_mgr_data->current_directory) == 0 &&
                       (strcmp(file_view_data[i]->file_data->file_name, ".") == 0 || strcmp(file_view_data[i]->file_data->file_name, "..") == 0 )) ||
             (strncmp(file_mgr_data->restricted_directory,file_mgr_data->current_directory, strlen(file_mgr_data->restricted_directory)) == 0 &&
               strcmp(file_view_data[i]->file_data->file_name, ".") == 0 ))
           {
             filterCount++;
             file_view_data[i]->filtered = True;
             continue;
           }
         }
      }

      if( restrictMode  &&
           (strcmp(file_view_data[i]->file_data->file_name, "..") == 0 ||
            strcmp(file_view_data[i]->file_data->file_name, ".") == 0))
      {
         char *tempName;

         tempName = (char *)XtMalloc( strlen(file_mgr_data->current_directory) + 3);
         sprintf( tempName, "%s/", file_mgr_data->current_directory );
         if( strcmp(users_home_dir, tempName) == 0 ||
             strcmp(users_home_dir, file_mgr_data->current_directory) == 0)
         {
            filterCount++;
            file_view_data[i]->filtered = True;
            XtFree(tempName);
            continue;
         }
         XtFree(tempName);
      }


      /* if we want to show the hidden files no more should be filtered out */
      if(show_hidden)
         continue;

      /* don't show .trashinfo in the trash directory */
      if(trashFileMgrData == file_mgr_data &&
        (strcmp(file_view_data[i]->file_data->file_name, ".trashinfo") == 0))
      {
         filterCount++;
         file_view_data[i]->filtered = True;
         continue;
      }

      /* Check for a match against the filter expression string, except for
       * files in the trash directory and sub directories in tree mode. */

      matches = False;
      if (strcmp(filter, "") != 0 &&
          file_mgr_data != trashFileMgrData &&
          !(file_mgr_data->show_type == MULTIPLE_DIRECTORY &&
            file_view_data[i]->file_data->is_subdir))
      {
         /* Special case for ".." that need not be filtered */
         if( !strcmp( file_view_data[i]->file_data->file_name, ".." )
             || !strcmp( file_view_data[i]->file_data->file_name, "." ) )
         {
         }
         else if (file_view_data[i]->file_data->action_name)
         {
            if(fnmatch((const char *)filter,
                       (const char *)file_view_data[i]->file_data->action_name,
                       0) == 0)
            {
                if ( mustMatch )
                {
                    filterCount++;
                    file_view_data[i]->filtered = True;
                    continue;
                }
                else
                    matches = True;
            }
         }
         else if (fnmatch((const char *)filter,
             (const char *)file_view_data[i]->file_data->file_name, 0) == 0)
         {
            if ( mustMatch )
            {
               filterCount++;
               file_view_data[i]->filtered = True;
               continue;
            }
            else
               matches = True;
         }
      }

      /* now lets check through the filter filetypes and if the file is  */
      /* filtered, filter it out                                         */
      if(file_mgr_data != trashFileMgrData)
      {
         /* This is for the case of files likes 'action' files which
            do not have a logical_type */

         if(strcmp(file_view_data[i]->file_data->file_name,file_view_data[i]->
                    file_data->logical_type) == 0)
         {
            if(!filter_data->match_flag)
            {
              if(!matches)
              {
               filterCount++;
               file_view_data[i]->filtered = True;
              }
            }
         }
         else
           for(j = 0; j < filter_data->count; j++)
           {
             if(strcmp(filter_data->user_data[j]->filetype,
                       file_view_data[i]->file_data->logical_type) == 0)
             {
                if((filter_data->user_data[j]->selected == True &&
                                                   filter_data->match_flag) ||
                (filter_data->user_data[j]->selected == False &&
                                                   !filter_data->match_flag))
                {
                   if(!matches)
                   {
                      filterCount++;
                      file_view_data[i]->filtered = True;
                   }
                }
                break;
             }
          }
      }
   }

   /* update ndir, nfile counts for this sub directory */
   directory_set->filtered_file_count = filterCount;
   directory_set->invisible_file_count = invisibleCount;
   sub_root = directory_set->sub_root;
   sub_root->ndir = sub_root->nfile = 0;
   sub_root->nnew = 0;

   for (i = 0; i < directory_set->file_count; i++)
   {
     if (!file_view_data[i]->filtered)
     {
       if (file_view_data[i]->file_data->is_subdir)
         sub_root->ndir++;
       else
         sub_root->nfile++;
     }
   }

   UpdateBranchState(file_mgr_data, sub_root, BRANCH_UPDATE, False);

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done FilterFiles, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s.tv_usec);

   /* added by Rafi */
   _DtPerfChkpntMsgSend("Done  Filtering Files");

#endif
}




/************************************************************************
 *
 *  GetAncestorInfo, GetLevel, GetFullName
 *
 ************************************************************************/

void
GetAncestorInfo(
        FileMgrData *file_mgr_data,
        FileViewData *ip,
        int *levelp,
        char *path,
        Bool *morep)
/*
 * Get information related to the ancestory of an entry:
 *  - tree depth level
 *  - full path name
 *  - for each level: flag indicating whether there are additional siblings
 *    to be diplayed after this entry
 */
{
  FileViewData *pp, *pa[256];
  int i, l, level;
  char *p;

  /* determine tree depth level of this entry */
  level = 0;
  for (pp = ip->parent; pp; pp = pp->parent)
    level++;
  if (levelp)
    *levelp = level;

  /* get a list of all ancestors (including ip) in top down order */
  l = level;
  for (pp = ip; pp; pp = pp->parent)
    pa[l--] = pp;

  /* construct path name of this entry */
  if (path) {
    strcpy(path, file_mgr_data->current_directory);
    p = path + strlen(path);
    for (l = 1; l <= level; l++) {
      if (p[-1] != '/')
        *p++ = '/';
      strcpy(p, pa[l]->file_data->file_name);
      p += strlen(p);
    }
  }

  /* compile more array */
  if (morep) {
    for (l = 0; l <= level; l++) {
      DirectorySet *ds = (DirectorySet *)pa[l]->directory_set;
      morep[l] = False;
            if (ds->order_list == NULL)
        continue;
      for (i = 0; i < ds->file_count; i++)
        if (ds->order_list[i] == pa[l])
          break;
      for (i = i + 1; i < ds->file_count; i++) {
        if (ds->order_list[i]->displayed) {
          morep[l] = True;
          break;
        }
      }
    }
  }
}


void
GetFullName(
        FileMgrData *file_mgr_data,
        FileViewData *ip,
        char *path)
{
  GetAncestorInfo(file_mgr_data, ip, NULL, path, NULL);
}


void
GetDirName(
        FileMgrData *file_mgr_data,
        FileViewData *ip,
        char *path)
{
  char *p;

  GetAncestorInfo(file_mgr_data, ip, NULL, path, NULL);
  p = strrchr(path, '/');
  *p = 0;
}


void
GetLevel(
        FileViewData *ip,
        int *levelp)
{
  int level = 0;
  for (ip = ip->parent; ip; ip = ip->parent)
    level++;
  *levelp = level;
}


/************************************************************************
 *
 *  FlattenTree
 *
 ************************************************************************/

static void
CopyOrderedEntries(
        FileViewData *ip,
        FileViewData *fvd_array[],
        int *index)
/*
 * Copy all entries from the tree to an array
 */
{
  DirectorySet *directory_set;
  int i;

  fvd_array[*index] = ip;
  (*index)++;

  if (ip->desc)
  {
    directory_set = (DirectorySet *)ip->desc->directory_set;
    for (i = 0; i < directory_set->file_count; i++)
      CopyOrderedEntries(directory_set->order_list[i], fvd_array, index);
  }
}


void
FlattenTree(
        FileMgrData *file_mgr_data,
        FileViewData ***file_view_data,
        int *file_count)
{
  int i;

  *file_count = 1;
  for (i = 0; i < file_mgr_data->directory_count; i++)
    *file_count += file_mgr_data->directory_set[i]->file_count;

  *file_view_data =
    (FileViewData **) XtMalloc (*file_count * sizeof(FileViewData *));
  i = 0;
  CopyOrderedEntries(file_mgr_data->tree_root, *file_view_data, &i);
}


/************************************************************************
 *
 *  IconCallback
 *      Callback function invoked upon an action occuring on an icon.
 *
 ************************************************************************/

static Bool
IsButtonOrMotion(
  Display *disp,
  XEvent *ep,
  char *arg)
{
  return (ep->type == MotionNotify || ep->type == ButtonRelease);
}


void
IconCallback(
        Widget w,
        XtPointer clientData,
        XtPointer callData )
{
   XmAnyCallbackStruct * callback;
   XButtonEvent * event;
   FileViewData * fileViewData;
   FileMgrData  * fileMgrData;
   static Boolean highlightType = INIT_VALUE;
   static Widget highlightWidget = NULL;
   char * logicalType;
   char * command;
   DesktopRec * desktopRec;
   Boolean dragged;
   Window root;
   XEvent bevent;
   int dx, dy;
   WindowPosition position;

   static int adjustState=-1;
   XrmValue value_return;
   char *str_type_return;


   if( adjustState == -1)
   {
      if (XrmGetResource(XrmGetDatabase(XtDisplay(w)), "*enableBtn1Transfer", "*EnableBtn1Transfer",&str_type_return, &value_return) && !strcmp(value_return.addr,"True") )
        {
             adjustState=True;
        }
      else
             adjustState=False;
   }

   callback = (XmAnyCallbackStruct *) callData;
   fileViewData = (FileViewData *) clientData;
   event = (XButtonEvent *) callback->event;

   if (callback->reason == XmCR_DRAG && event->button == bMenuButton)
      callback->reason = XmCR_POPUP;

   if(callback->reason != XmCR_UNHIGHLIGHT)
   {
      /*  Get the directory data and the file manager data  */
      (void)IsDesktopPtr(fileViewData, &fileMgrData, &desktopRec);
   }

   /*  Process the different callback types  */
   if ((callback->reason == XmCR_ARM) || (callback->reason == XmCR_SELECT))
   {
      /*
       * Both ARM and SELECT are generated using Button1. We pass on these
       * requests to the code responsible for processing button 1 selects
       * and drags.
       */
      FileMgrRec  * fileMgrRec;
      DtIconGadget new = (DtIconGadget)w;

      if( event->type == KeyPress )
      {
        /* if a keypress we only want to select on an SELECT */
        if( callback->reason == XmCR_ARM )
          return;
        else
        {
          XKeyEvent *kevent;
          KeySym keysym;
          int offset;

          kevent = (XKeyEvent *)event;
          if (kevent->state & ShiftMask)
            offset = 1;
          else
            offset = 0;

          keysym = XLookupKeysym((XKeyEvent *)kevent, offset);

          if( keysym == XK_Return )
            goto run_default_action;
        }
      }

      if (callback->reason == XmCR_ARM)
         new->icon.armed = False;

      if (desktopRec)
      {
         B1DragPossible = False;
         B2DragPossible = False;
         XtCallCallbacks(desktopRec->drawA, XmNinputCallback, callData);
      }
      else
      {
         B1DragPossible = False;
         B2DragPossible = False;

         fileMgrRec = (FileMgrRec *)fileMgrData->file_mgr_rec;
         XtCallCallbacks(fileMgrRec->file_window, XmNinputCallback, callData);
      }

      if (callback->reason == XmCR_ARM)
         new->icon.armed = True;
   }
   else if (callback->reason==XmCR_DISARM)
   {
      /*
       * DISARM is generated as the result of a button 1 up event.
       * If we are in the middle of a button 2 drag, then we'll ignore this;
       * otherwise, we now know a drag will not start, so clear all state flags.
       */
      if (B2DragPossible)
         return;

      B1DragPossible = False;
      B2DragPossible = False;
      ProcessBtnUp = False;
   }
   else if (callback->reason == XmCR_DROP)
   {
      /*
       * DROP is generated as the result of a button 2 up event.
       * If we are in the middle of a button 1 drag, then we'll ignore this;
       * otherwise, we now know a drag will not start, so clear all state flags.
       */
      if (B1DragPossible)
         return;

      B1DragPossible = False;
      B2DragPossible = False;
      ProcessBtnUp = False;
   }
   else if (callback->reason == XmCR_DEFAULT_ACTION)
   {
      Boolean Error;

      if(event->type == KeyPress)
      {
         XKeyEvent *kevent;
         KeySym keysym;
         int offset;

         kevent = (XKeyEvent *)event;
         if (kevent->state & ShiftMask)
            offset = 1;
         else
            offset = 0;

         keysym = XLookupKeysym((XKeyEvent *)kevent, offset);

         if (keysym == XK_Escape)
         {
            /* an escape unposts the name change text widget */
            if(desktopRec)
               UnpostDTTextField();
            else
               UnpostTextField(fileMgrData);
            return;
         }
         else
         {
           KeySym SpaceKeySym = XStringToKeysym( "space" );
           if( keysym == SpaceKeySym )
             return;
         }
      }

      run_default_action:

      Error = False;

      /*
       * If DEFAULT_ACTION was generated by button press,
       * wait for the button release
       */
      dragged = False;
      if(event->type == ButtonPress)
      {
        if (loc_cursor1 == None)
          loc_cursor1 = XCreateFontCursor(XtDisplay(w), 40);
        if (loc_cursor2 == None)
          loc_cursor2 = XCreateFontCursor(XtDisplay(w), 34);

        root = RootWindowOfScreen(XtScreen(w));
        XGrabPointer(XtDisplay(w), root,
                     False, ButtonReleaseMask | PointerMotionMask,
                     GrabModeAsync, GrabModeAsync,
                     None, loc_cursor1, CurrentTime);
        do {
          XIfEvent(XtDisplay(w), &bevent, IsButtonOrMotion, NULL);
          if (!dragged && bevent.type == MotionNotify) {
            dx = event->x_root - bevent.xmotion.x_root; if (dx < 0) dx = -dx;
            dy = event->y_root - bevent.xmotion.y_root; if (dy < 0) dy = -dy;
            if (dx > DBLCLICK_DRAG_THRESHOLD || dy > DBLCLICK_DRAG_THRESHOLD) {
              DPRINTF(("dragged!\n"));
              XGrabPointer(XtDisplay(w), root,
                           False, ButtonReleaseMask | PointerMotionMask,
                           GrabModeAsync, GrabModeAsync,
                           None, loc_cursor2, CurrentTime);
              dragged = True;
            }
          }
        } while (bevent.type != ButtonRelease);

        XUngrabPointer(XtDisplay(w), CurrentTime);
        XFlush(XtDisplay(w));

        position.x = bevent.xbutton.x_root;
        position.y = bevent.xbutton.y_root;
      }

      /*
       * DEFAULT_ACTION is generated by a double-click of button 1.
       * We now know a drag will not start, so clear all state flags.
       */
      B1DragPossible = False;
      B2DragPossible = False;
      ProcessBtnUp = False;

      logicalType = fileViewData->file_data->logical_type;
      command = _DtRetrieveDefaultAction(logicalType);

      if (desktopRec)
      {
         /* Any button event unposts the text field */
         UnpostDTTextField();

         if(command)
            RunDTCommand(command, desktopRec, NULL);
         else
            Error = True;
      }
      else
      {
         /* Any button event unposts the text field */
         UnpostTextField(fileMgrData);

         if((openDirType == NEW && strcmp(command, openInPlace) == 0) ||
            (openDirType != NEW && strcmp(command, openNewView) == 0))
         {
            unsigned int modifiers = event->state;

            RunCommand (openNewView, fileMgrData, fileViewData,
                        dragged? &position: NULL, NULL, NULL);
            if((modifiers != 0) && ((modifiers & ControlMask) != 0))
            {
               DialogData  *dialog_data;

               dialog_data = _DtGetInstanceData(fileMgrData->file_mgr_rec);
               CloseView(dialog_data);
            }
         }
         else
         {
            if(command)
            {
               if ((fileMgrData->show_type == MULTIPLE_DIRECTORY || dragged) &&
                   strcmp(command, openInPlace) == 0)
               {
                 RunCommand (openNewView, fileMgrData, fileViewData,
                             dragged? &position: NULL, NULL, NULL);
               }
               else
                 RunCommand (command, fileMgrData, fileViewData,
                             NULL, NULL, NULL);
            }
            else
               Error = True;
         }
      }

      XtFree(command);

      if(Error)
      {
         char * title;
         char msg[512];
         char * tmpStr;

         tmpStr = (GETMESSAGE(9,6, "Action Error"));
         title = (char *)XtMalloc(strlen(tmpStr) + 1);
         strcpy(title, tmpStr);

         (void) sprintf (msg,
              (GETMESSAGE(9,7, "There are no actions defined for %s\n")),
              logicalType);
         if(desktopRec)
            _DtMessage (desktopRec->shell, title, msg, NULL, HelpRequestCB);
         else
            _DtMessage (fileViewData->widget, title, msg, NULL, HelpRequestCB);
         XtFree(title);
      }
   }
   else if (callback->reason == XmCR_DRAG)
   {
      /*
       * DRAG is generated by a button 2 down event. It flags that
       * the user may possible be initiating a drag; we won't know
       * for sure until the drag threshold is surpassed.
       * Ignore this if a Button 1 drag is ramping up.
       */
      if (B1DragPossible)
         return;

      /* Any button event unposts the text field */
      if (desktopRec)
      {
         UnpostDTTextField();
         if(!DTFileIsSelected(desktopRec, fileViewData) && adjustState)
             SelectDTFile(desktopRec);
      }
      else
      {
         if( fileMgrData && !FileIsSelected(fileMgrData,fileViewData) &&
             adjustState
           )
           {
             SelectFile(fileMgrData, fileViewData);
           }
         UnpostTextField(fileMgrData);
      }

      /* Save starting X & Y, in case a drag really starts */
      initialDragX = event->x;
      initialDragY = event->y;
      B2DragPossible = True;
      memcpy((char *) &desktop_data->event, (char *) event,
             (int) sizeof(XButtonEvent));
   }
   else if (callback->reason == XmCR_POPUP)
   {
      if(!desktopRec)
      {
         FmPopup (w, clientData, (XEvent *)event, fileMgrData);
      }
   }
   else if (callback->reason == XmCR_HIGHLIGHT)
   {
      DtIconGadget g = (DtIconGadget)w;

      if (dragActive)
      {
         if ((g->icon.border_type == DtRECTANGLE) || (!g->icon.pixmap))
           _XmUnhighlightBorder(w);

         B1DragPossible = False;
         B2DragPossible = False;
         return;
      }

      if ((g->icon.border_type == DtRECTANGLE) || (!g->icon.pixmap))
         return;

      if ((highlightType != INIT_VALUE) && (highlightWidget))
      {
         if (desktopRec)
         {
            if (highlightType == NOT_DESKTOP)
               DrawUnhighlight(highlightWidget, NOT_DESKTOP);
            else if (highlightWidget != w)
               DrawUnhighlight(highlightWidget, DESKTOP);
         }
         else
         {
            if (highlightType == DESKTOP)
               DrawUnhighlight(highlightWidget, DESKTOP);
            else if (highlightWidget != w)
               DrawUnhighlight(highlightWidget, NOT_DESKTOP);
         }
      }

      if (desktopRec)
      {
         DrawHighlight(w, NULL, NULL, DESKTOP);
         highlightType = DESKTOP;
      }
      else
      {
         DrawHighlight(w, fileViewData, fileMgrData, NOT_DESKTOP);
         highlightType = NOT_DESKTOP;
      }
      highlightWidget = w;
   }
   else if (callback->reason == XmCR_UNHIGHLIGHT)
   {
      DtIconGadget g = (DtIconGadget)w;

      if (dragActive)
      {
        if ((g->icon.border_type == DtRECTANGLE) || (!g->icon.pixmap))
          _XmHighlightBorder(w);

        B1DragPossible = False;
        B2DragPossible = False;
        return;
      }

      if ((g->icon.border_type == DtRECTANGLE) || (!g->icon.pixmap))
        return;

      if (w == highlightWidget)
      {
         if (highlightType == DESKTOP)
            DrawUnhighlight(w, DESKTOP);
         else
            DrawUnhighlight(w, NOT_DESKTOP);

         highlightType = INIT_VALUE;
         highlightWidget = NULL;
      }
   }
   else if (callback->reason == XmCR_SHADOW)
      DrawShadowTh(w, NULL, DESKTOP);

}

static int
GetSelectedCount(
        FileViewData * fileViewData,
        FileMgrData * fileMgrData,
        DesktopRec * desktopRec,
        int * dt)
{
   int selectedCount;
   char * wsName;
   Atom pCurrent;

   /*  If the initiation of the drag occured upon an      */
   /*  already selected icon, check for a multiple drag.  */

   if ((desktopRec == NULL) && FileIsSelected(fileMgrData, fileViewData))
      selectedCount = fileMgrData->selected_file_count;
   else if (desktopRec == NULL)
      selectedCount = 1;
   else
   {
      if (DTFileIsSelected(desktopRec, fileViewData))
      {
         if(DtWsmGetCurrentWorkspace(XtDisplay(desktopRec->shell),
                               RootWindowOfScreen(XtScreen(desktopRec->shell)),
                               &pCurrent) == Success)
         {
            wsName = XGetAtomName(XtDisplay(desktopRec->shell), pCurrent);
            CleanUpWSName(wsName);
         }
         else
            wsName = XtNewString("One");

         for(*dt = 0; *dt < desktop_data->numWorkspaces; (*dt)++)
         {
            if(strcmp(wsName, desktop_data->workspaceData[*dt]->name) == 0)
            {
               selectedCount = desktop_data->workspaceData[*dt]->files_selected;
               break;
            }
         }
         XtFree(wsName);
      }
      else
         selectedCount = 1;
   }

   return(selectedCount);
}


static Pixmap
GetDragIconMask(
        Widget w,
        unsigned int wid,
        unsigned int hei)
{
   Pixmap dragMask;
   Display *dpy = XtDisplay(w);
   unsigned char flags;
   XRectangle pRect, lRect;
   GC fillGC;
   XGCValues values;
   Arg args[8];
   Dimension shadowThickness;
   Dimension marginWidth, marginHeight;
   int minX, minY;
   Boolean minXUndefined, minYUndefined;

   dragMask = XCreatePixmap(dpy, RootWindowOfScreen (XtScreenOfObject(w)),
                            wid, hei, 1);

  /* Create a GC for drawing 0's into the pixmap */
   fillGC = XCreateGC(dpy, dragMask, 0, (XGCValues *) NULL);

   XFillRectangle(dpy, dragMask, fillGC, 0, 0, wid, hei);

   values.foreground = 1;
   XChangeGC(dpy, fillGC, GCForeground, &values);

  /* Create the drag pixmap, and the associated mask bitmap */
   _DtIconGetIconRects((DtIconGadget)w, &flags, &pRect, &lRect);

   minX= 0;
   minY= 0;
   minXUndefined = minYUndefined = True;
   if (flags & XmPIXMAP_RECT)
   {
       minX = pRect.x;
       minY = pRect.y;
       minXUndefined = minYUndefined = False;
   }
   if (flags & XmLABEL_RECT)
   {
      if ((lRect.x < minX) || minXUndefined)
         minX = lRect.x;
      if ((lRect.y < minY) || minYUndefined)
         minY = lRect.y;
   }

    XtSetArg (args[0], XmNshadowThickness, &shadowThickness);
    XtSetArg (args[1], XmNmarginWidth, &marginWidth);
    XtSetArg (args[2], XmNmarginHeight, &marginHeight);
    XtGetValues (w, args, 3);

    if (flags & XmPIXMAP_RECT)
       XFillRectangle(dpy, dragMask, fillGC,
                      pRect.x - minX + shadowThickness + marginWidth,
                      pRect.y - minY + shadowThickness + marginHeight,
                      pRect.width - 2*marginWidth,
                      pRect.height - 2*marginHeight);
    if (flags & XmLABEL_RECT)
    {
      XFillRectangle(dpy, dragMask, fillGC,
                     lRect.x - minX + shadowThickness + marginWidth,
                     lRect.y - minY + shadowThickness + marginHeight,
                     lRect.width - 2*marginWidth,
                     lRect.height - 2*marginHeight);
    }
    XFreeGC(dpy, fillGC);

    return (dragMask);
}

static Widget
GetDragIcon(
        Widget w,
        FileViewData * fileViewData )
{
   XmManagerWidget mw = (XmManagerWidget) XtParent(w);
   Widget screen_object = (Widget) XmGetXmScreen(XtScreenOfObject(w));
   Arg args[11];
   register int n;
   unsigned int wid, hei, d, junk;
   Widget dragIcon;
   Pixmap dragPixmap;
   Pixmap dragMask;

   dragPixmap = _DtIconDraw (w, 0, 0, 0, False);

   XGetGeometry (XtDisplay(w), dragPixmap,
                 (Window *) &junk,             /* returned root window */
                 (int *) &junk, (int *) &junk, /* x, y of pixmap */
                 &wid, &hei,                   /* width, height of pixmap */
                 &junk,                        /* border width */
                 &d);                          /* depth */

   if (initiating_view
       && ((FileMgrData *)initiating_view)->view == BY_ATTRIBUTES)
   {
     XmFontList fontList;
     XmString fileNameString;
     DtIconGadget g = (DtIconGadget)w;


     XtSetArg( args[0], XmNfontList, &fontList );
     XtGetValues( fileViewData->widget, args, 1 );

     fileNameString = XmStringCreateLocalized( fileViewData->file_data->file_name );
     wid = XmStringWidth( fontList, fileNameString )
           + g->icon.pixmap_width
           + g->icon.cache->margin_width
           + g->icon.cache->spacing
           + G_ShadowThickness(g)
           + G_HighlightThickness(g);
     XmStringFree( fileNameString );
   }

   dragMask = GetDragIconMask(w, wid, hei);

   n = 0 ;
   XtSetArg(args[n], XmNhotX, 0);  n++;
   XtSetArg(args[n], XmNhotY, 0);  n++;
   XtSetArg(args[n], XmNwidth, wid);  n++;
   XtSetArg(args[n], XmNheight, hei);  n++;
   XtSetArg(args[n], XmNmaxWidth, wid);  n++;
   XtSetArg(args[n], XmNmaxHeight, hei);  n++;
   XtSetArg(args[n], XmNdepth, d);  n++;
   XtSetArg(args[n], XmNpixmap, dragPixmap);  n++;
   XtSetArg(args[n], XmNmask, dragMask);  n++;
   XtSetArg(args[n], XmNforeground, mw->core.background_pixel);  n++;
   XtSetArg(args[n], XmNbackground, mw->manager.foreground);  n++;
   dragIcon = XtCreateWidget("drag_icon", xmDragIconObjectClass,
                             screen_object, args, n);

   return(dragIcon);
}



/* The following function is called if the completeMove flag is   */
/* set to TRUE. Internally, dtfile sets this to FALSE; thus, only */
/* receiving clients that set this to true will get this function */
/* called for them.                                               */
/* This function will delete the files/dirs that were dropped     */
/* on the receiving client.                                       */

static void
NewConvertDelete(
        Widget w,
        FileViewData * fileViewData,
        char ** fileList,
        int numFiles)
{
      static char *pname = "NewConvertDelete";
      int     i;
      int     child_pid = 0;

      DPRINTF(("%s: Entering function\n", pname));

      /* fork a background child process to honor the Move Completion */
       child_pid = fork();

       if (child_pid == -1)
       {
	    char *msg, *tmpStr, *title;

	    DBGFORK(("%s: Cannot create child process\n", pname));

	    tmpStr = GETMESSAGE(11,59, 
			"Cannot create a child process to delete the dropped files.");
	    msg = XtNewString(tmpStr);
	    title = XtNewString((GETMESSAGE(11,58,"Process Create Error")));

	   /* Display Error message */
	    _DtMessage(toplevel, title, msg, NULL, HelpRequestCB);
	    XtFree(msg);
	    XtFree(title);
	    return;
       }


       /* In the Child Process, we simply erase the files   */
       /* and directories that were dropped on the receiver */
       if (child_pid == 0)
       {
	    DBGFORK(("%s:  child forked\n", pname));

	    for (i = 0; i < numFiles; i++)
	    {
	     DPRINTF(("%s: Erasing file %s\n", pname, fileList[i]));
	     EraseObject(fileList[i]);
	    }

	    DBGFORK(("%s:  child exiting\n", pname));

            exit(0);
       }

       DBGFORK(("%s:  forked child<%d>\n", pname, child_pid));

}


static void
NewConvertFileName(
        Widget w,
        FileViewData * fileViewData,
        char ** fileList,
        Cardinal *numFiles)
{
   FileMgrData * fileMgrData;
   DesktopRec * desktopRec;
   int selectedCount;
   int dt, i, count;
   char * directoryName;
   char * fileName;
   char * path;

   if(fileViewData != NULL)
   {
      (void)IsDesktopPtr(fileViewData, &fileMgrData, &desktopRec);
      selectedCount = GetSelectedCount(fileViewData, fileMgrData, desktopRec, &dt);
   }
   else
   {
      /* If fileViewData is NULL, then the file no long exists */
      fileMgrData = (FileMgrData *)initiating_view;
      desktopRec = NULL;
      if(fileMgrData)
	selectedCount = fileMgrData->selected_file_count;
      else
        selectedCount = 0;
   }

   if (selectedCount > 1)
   {
      for (i = (selectedCount-1); i >= 0; i--)
      {
         if (desktopRec)
         {
            fileViewData = desktop_data->workspaceData[dt]->
                                 selectedDTWindows[i]->file_view_data;
            directoryName = desktop_data->workspaceData[dt]->
                                 selectedDTWindows[i]->dir_linked_to;
            fileName = fileViewData->file_data->file_name;
         }
         else
         {
            fileViewData = fileMgrData->selection_list[i];
            directoryName = ((DirectorySet *)fileViewData->directory_set)->name;
            fileName = fileViewData->file_data->file_name;
         }
         path = (char *)XtMalloc(strlen(directoryName) + strlen(fileName) + 2);
         sprintf(path,"%s/%s", directoryName, fileName);

         if ((!desktopRec) && (fileMgrData->toolbox))
            path = _DtResolveAppManPath(path,
                                        fileMgrData->restricted_directory);

         fileList[i] = DtEliminateDots(path);
      }
   *numFiles = selectedCount;
   }
   else
   {
      if (desktopRec)
      {
         directoryName = desktopRec->dir_linked_to;
         fileName = fileViewData->file_data->file_name;
      }
      else if(fileViewData)
      {
         directoryName = ((DirectorySet *)fileViewData->directory_set)->name;
         fileName = fileViewData->file_data->file_name;
      }
      else
      {
         directoryName = fileMgrData->current_directory;
         fileName = NULL;
      }
      path = (char *)XtMalloc(strlen(directoryName) + strlen(fileName) + 2);
      sprintf(path,"%s/%s", directoryName, fileName);

      if ((!desktopRec) && (fileMgrData->toolbox))
         path = _DtResolveAppManPath(path, fileMgrData->restricted_directory);

      fileList[0] = DtEliminateDots( path );
      *numFiles = 1;
   }

#ifdef DEBUG
   if (debug)
   {
     printf("NewConvertFileName: returns %d files\n", selectedCount);
     for (i = 0; i < selectedCount; i++) {
       printf("\t\"%s\"\n", fileList[i]);
     }
     if (selectedCount == 0)
       printf("\t\"%s\"\n", fileList[0]);
   }
#endif

}

/* ARGSUSED */
static void
FileConvertCB(
        Widget w,
        XtPointer client,
        XtPointer call)
{
    FileViewData * fileViewData = (FileViewData *) client;
    DtDndConvertCallback cb = (DtDndConvertCallback) call;
    FileMgrData *fmd = (FileMgrData *)initiating_view;

    if(fmd)
    {
       fileViewData = fmd->drag_file_view_data;
       fmd->drag_file_view_data = NULL;
    }
    if (cb->reason == DtCR_DND_CONVERT_DATA)
    {
       if(fmd && !fmd->selected_file_count && fileViewData == NULL)
       {
	   cb->status = DtDND_FAILURE;
	   cb->dragData->numItems = 0; /* Just to be on safe side */
	   return;
       }
       NewConvertFileName(w, fileViewData,
                       cb->dragData->data.files, &cb->dragData->numItems);
    } else if (cb->reason == DtCR_DND_CONVERT_DELETE)
    {
       NewConvertDelete(w, fileViewData,
                       cb->dragData->data.files, cb->dragData->numItems);
    }
}


/* This callback procedure removes the icons when the drop is complete */
/* ARGSUSED */
static void
DragFinishCB(Widget w, XtPointer client, XtPointer call)
{
   DtDndDragFinishCallback cb = (DtDndDragFinishCallback) call;
   int                  i;
   static Window root = 0;

   DPRINTF(("DragFinishCB: dragActive -> False\n"));

   dragActive = False;
   initialDragX = -1;
   initialDragY = -1;
   B1DragPossible = False;
   B2DragPossible = False;
   ProcessBtnUp = False;

   if (cb->sourceIcon)
      XtDestroyWidget(cb->sourceIcon);

   DPRINTF (("DragFinishCB: Number of items being freed are %d\n",
              cb->dragData->numItems));
   for (i = 0; i < cb->dragData->numItems; i++) {

      DPRINTF(("DragFinishCB: Freeing %s\n", cb->dragData->data.files[i]));
      XtFree(cb->dragData->data.files[i]);
      cb->dragData->data.files[i] = NULL;
   }

}

/* ARGSUSED */
void
StartDrag(
        Widget w,
        FileViewData * fileViewData,
        XEvent * event)
{
   static XtCallbackRec fileConvertCB[] = { {FileConvertCB, NULL},
                                           {NULL, NULL} };
   static XtCallbackRec dragFinishCB[] = { {DragFinishCB, NULL},
                                           {NULL, NULL} };
   static XtCallbackRec dropOnRootCB[] = { {DropOnRootCB, NULL},
                                           {NULL, NULL} };
   Widget drag_icon;
   Arg                  args[2];
   int                  numArgs;
   FileMgrData *        fileMgrData = NULL;
   DesktopRec *         desktopRec = NULL;
   int                  selectedCount;
   int                  dt;
   Boolean              desktopObj;
   unsigned char        operations;

   if (event->type == INVALID_TYPE) return;

   if (fileViewData == NULL) return;

   fileConvertCB[0].closure = (XtPointer)fileViewData;
   dropOnRootCB[0].closure = (XtPointer)fileViewData;

   desktopObj = IsDesktopPtr(fileViewData, &fileMgrData, &desktopRec);
   selectedCount = GetSelectedCount(fileViewData, fileMgrData,
                                    desktopRec, &dt);

   if (selectedCount > 1)
      drag_icon = NULL;
   else
      drag_icon = GetDragIcon(w, fileViewData);

   numArgs = 0;
   XtSetArg(args[numArgs], DtNsourceIcon, drag_icon);                numArgs++;
   XtSetArg(args[numArgs], DtNdropOnRootCallback, dropOnRootCB);     numArgs++;

   if ((desktopObj && desktopRec->toolbox) ||
       (!desktopObj && fileMgrData->toolbox))
     operations = XmDROP_COPY;
   else
     operations = XmDROP_MOVE | XmDROP_COPY | XmDROP_LINK;

   if (DtDndDragStart(w, event, DtDND_FILENAME_TRANSFER, selectedCount,
                        operations,
                        fileConvertCB, dragFinishCB, args, numArgs) == NULL) {
      DPRINTF(("StartDrag: dragActive -> False\n"));

      dragActive = False;
      initialDragX = -1;
      initialDragY = -1;
      B1DragPossible = False;
      B2DragPossible = False;
      ProcessBtnUp = False;
   }
}


/*
 * This function is capable of initiating either a button 1 or button 2
 * drag operation; which one gets started is dependent upon which of the
 * two state flags (B1DragPossible or B2DragPossible) is set.
 */

static void
InitiateIconDrag(
        FileViewData * fileViewData,
        int rootX,
        int rootY,
        XEvent * event )

{
   FileMgrData * fileMgrData;
   DtIconGadget iconG;
   Widget dragIcon;
   char * typeSet;
   char * fileSet;
   char * directoryName;
   Pixmap dragPixmap;
   XRectangle dragMask[2];
   Pixel  bg;
   XRectangle pRect, lRect;
   unsigned char flags;
   int stringSize;
   register int i;
   Arg args[10];
   Boolean allowDropInInitiatingWindow;
   int rectCount;
   int minX, minY;
   Boolean minXUndefined, minYUndefined;
   DesktopRec * desktopRec;
   int btn;
   char * hostName;
   Boolean spaces = False;
   Boolean trash = False;
   char *tmpStr, *link_path, *ptr;

   /* Don't allow multi-drags to start */
   if (dragActive)
   {
      initialDragX = -1;
      initialDragY = -1;
      B1DragPossible = False;
      B2DragPossible = False;
      ProcessBtnUp = False;
      return;
   }
   else
      dragActive = True;

   DPRINTF(("InitiateIconDrag: dragActive -> True\n"));

   dragIcon = fileViewData->widget;
   (void)IsDesktopPtr(fileViewData, &fileMgrData, &desktopRec);

   /* if what we are dragging is a trash item, we want the objects to be
    * real objects, not their links
    */
   if(fileMgrData == trashFileMgrData && trashFileMgrData != NULL)
      trash = True;

   /*
    * We need to mark the icon which initiated the drag as no longer
    * being 'armed', since it will not receive the button up event,
    * because we will have release the drag.
    */
   iconG = (DtIconGadget)dragIcon;
   iconG->icon.armed = False;

   /*
    * Remember the posistion of the icon pixmap within the drag icon.
    */
   dragIconPixmapOffsetX = G_ShadowThickness(iconG) + G_MarginWidth(iconG);
   dragIconPixmapOffsetY = G_ShadowThickness(iconG) + G_MarginHeight(iconG);

   if(desktopRec)
   {
      initiating_view = (XtPointer) NULL;
      widget_dragged = dragIcon;
   }
   else
   {
      initiating_view = (XtPointer) fileMgrData;
      widget_dragged = NULL;
   }
   StartDrag(dragIcon, fileViewData, event);
}


/*
 * These are replacements for the corresponding libXm functions.  They allow
 * us to treat icon gadgets as non-rectangular, so that input processing
 * is handled correctly.
 */

static XmGadget
InputInGadget (
   Widget w,
   register int x,
   register int y)

{
   CompositeWidget cw = (CompositeWidget)w;
   static Region r = NULL;
   FileMgrData * file_mgr_data;
   Boolean simpleCheck = False;
   register int i;
   unsigned char flags;
   XRectangle pRect, lRect;
   register Widget widget;

   file_mgr_data = ReturnDesktopPtr((Widget)cw);

   /*
    * Tree views and attribute views do not have the non-rectangular
    * hotspots, so we can resort to the standard checking algorithm.
    */
   if ((file_mgr_data == NULL) ||
       (file_mgr_data->show_type != SINGLE_DIRECTORY) ||
       (file_mgr_data->view == BY_ATTRIBUTES))
   {
      simpleCheck = True;
   }

   for (i = 0; i < cw->composite.num_children; i++)
   {
      widget = cw->composite.children[i];

      if (XmIsGadget(widget) && XtIsManaged(widget))
      {
         if (simpleCheck)
         {
            if (x >= widget->core.x && y >= widget->core.y &&
              (Position) x < (Position) (widget->core.x + widget->core.width) &&
              (Position) y < (Position) (widget->core.y + widget->core.height))
            {
               return ((XmGadget) widget);
            }
         }
         else
         {
            /* Initialize the region to be empty */
            if (r == NULL)
               r = XCreateRegion();
            else
               XSubtractRegion(r, r, r);

            _DtIconGetIconRects((DtIconGadget)widget, &flags, &pRect, &lRect);

            if (flags & XmPIXMAP_RECT)
               XUnionRectWithRegion(&pRect, r, r);

            if (flags & XmLABEL_RECT)
               XUnionRectWithRegion(&lRect, r, r);

            if (XPointInRegion(r, x, y))
               return ((XmGadget) widget);
         }
      }
   }

   return(NULL);
}


/*
 * This function is identical to the libXm version; it needs to be here
 * so that it will call our InputInGadget() instead of the libXm
 * version, which was bundled together with it in GadgetUtil.c
 */

static XmGadget
InputForGadget (
   Widget cw,
   int x,
   int y)
{
   XmGadget gadget;

   gadget = InputInGadget (cw, x, y);

   if (!gadget  ||  !XtIsSensitive ((Widget)gadget))
   {
      return ((XmGadget) NULL);
   }

   return (gadget);
}


/*
 * This function processes motion events anytime a B1 or B2 drag operation
 * has the potential of starting for a file icon.  When the drag threshold
 * is surpassed, a drag operation will be started.
 */

void
FileIconMotion(
               Widget w,
               XtPointer clientData,
               XEvent *event)
{
   int diffX, diffY;
   FileViewData * fileViewData = NULL;
   Position rootX, rootY;
   DirectorySet * directoryData = NULL;
   Arg args[10];
   int i;
   Widget dragIcon;

   if ((B1DragPossible && (event->xmotion.state & Button1Mask)) ||
       (B2DragPossible && (event->xmotion.state & Button2Mask)))
   {
      /* Have we passed the drag threshold? */
      diffX = initialDragX - event->xmotion.x;
      diffY = initialDragY - event->xmotion.y;

      if ((ABS(diffX) >= dragThreshold) || (ABS(diffY) >= dragThreshold))
      {
         /* Map the original (x,y) into a gadget Id */
         if (dragIcon =(Widget)InputForGadget(w, initialDragX, initialDragY))
         {
            /* Map the icon into its fileViewData structure */
            /* Check for desktop icon first */
            for (i = 0, fileViewData = NULL; i<desktop_data->numIconsUsed; i++)
            {
               if (desktop_data->desktopWindows[i]->iconGadget == dragIcon)
               {
                 fileViewData = desktop_data->desktopWindows[i]->file_view_data;
                 break;
               }
            }
            if (fileViewData == NULL)
            {
               /* Not a desktop icon */
               XtSetArg(args[0], XmNuserData, (XtPointer) &directoryData);
               XtGetValues(dragIcon, args, 1);
               for (i = 0; i < directoryData->file_count; i++)
               {
                  if (directoryData->file_view_data[i]->widget == dragIcon &&
                             directoryData->file_view_data[i]->displayed)
                  {
                     fileViewData = directoryData->file_view_data[i];
                     break;
                  }
               }
            }


            /* Map to root coordinates */
            XtTranslateCoords(w, (Position)initialDragX, (Position)initialDragY,
                              &rootX, &rootY);

	       if(directoryData)
	       {
	         FileMgrData *fmd;
	         fmd = (FileMgrData *)(directoryData->file_mgr_data);
	         if(fmd)
		      fmd->drag_file_view_data = fileViewData;
	       }
             InitiateIconDrag(fileViewData, (int)rootX, (int)rootY, event);
         }
         else
         {
            /*
             * The file manager view must have changed between the time
             * the user did the button down, and the time they moved
             * enough to pass the drag threshold, because there is now
             * no icon located where the drag initiated.  Therefore,
             * we'll just clean up, because there is much else we can do.
             */
            B1DragPossible = False;
            B2DragPossible = False;
            ProcessBtnUp = False;
         }

         /* Force the button up to be ignored */
         initialDragX = -1;
         initialDragY = -1;
      }
   }
}


/* Compute position of desktop icon shell for given a drop position */
static void
PositionDesktopIcon(
   int drop_x,
   int drop_y,
   int *root_x,
   int *root_y)

{
  int pixmap_offset_x;
  int pixmap_offset_y;
  DtIconGadget g;

  /*
   * We want to position the shell, so that the icon pixmap in the icon
   * gadget will appear at the same spot that the icon pixmap of the drag
   * cursor was when the drop occurred.
   */

  /* First we caculate the top left corner of the drag pixmap by adding
   * the offset of the pixmap within the drag icon to the drop position. */
  drop_x += dragIconPixmapOffsetX;
  drop_y += dragIconPixmapOffsetY;

  /* In order to calculate the correct position of the dektop icon shell
   * such that the pixmap within the icon gadget will end up at the desired
   * position, we need to know the offset of the gadget's icon pixmap relative
   * to the desktop icon shell.
   *   Since a desktop icon consists a frame widget, a drawing area and an
   * icon gadget (all within a popup shell), this offset is computed by adding
   * the frame width and and drawing area margins to the icon gadget's
   * highlight thickness.
   *   The frame width is 3 and and and the drawing area margin width is 1
   * (both hardcoded in Desktop.c).  We determine the remaining components
   * by looking at one of the existing desktop icon gadgets. */
  if (desktop_data != NULL &&
      desktop_data->numIconsUsed + desktop_data->numCachedIcons > 0)
  {
     g = (DtIconGadget) desktop_data->desktopWindows[0]->iconGadget;
     pixmap_offset_x =
      pixmap_offset_y = 3 + 1 + G_HighlightThickness(g) + G_ShadowThickness(g);
     pixmap_offset_x += G_MarginWidth(g);
     pixmap_offset_y += G_MarginHeight(g);
  }
  else
  {
     /* don't have an icon gadget; assume default values */
     pixmap_offset_x = pixmap_offset_y = 3 + 1 + 2 + 2 + 2;
  }

  /* Finally, calculate the position of the dektop icon shell by
   * subtracting the offset of the gadget's icon pixmap from the
   * desired pixmap position */
  *root_x = drop_x - pixmap_offset_x;
  *root_y = drop_y - pixmap_offset_y;
}


/* Code to reposition a desktop icon after a drag */

static void
RelocateDesktopIcon(
   DesktopRec * desktopRec,
   int root_x,
   int root_y)

{
   Widget pu_shell;
   Dimension width, height;
   Arg args[10];
   XSizeHints wmSizeHints;

   pu_shell = desktopRec->shell;


   XtSetArg (args[0], XmNwidth, &width);
   XtSetArg (args[1], XmNheight, &height);
   XtGetValues(pu_shell, args, 2);

   RegisterInGrid((int)width, (int)height,
                  desktopRec->root_x,
                  desktopRec->root_y,
                  desktopRec->workspace_num,
                  False);

   XtSetArg (args[0], XmNx, root_x);
   XtSetArg (args[1], XmNy, root_y);

   XtSetValues (pu_shell, args, 2);
   XRaiseWindow(XtDisplay(pu_shell), XtWindow(pu_shell));
   XSync(XtDisplay(pu_shell), False);

   RegisterInGrid((int)width, (int)height, root_x, root_y,
                  desktopRec->workspace_num, True);

   desktopRec->root_x = root_x;
   desktopRec->root_y = root_y;
   SaveDesktopInfo(NORMAL_RESTORE);
}


/************************************************************************
 *
 *  FileIsSelected
 *      Loop through the file selection list to see if the specified
 *      file is selected.
 *
 ************************************************************************/

Boolean
FileIsSelected(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data )
{
   return file_view_data->selected;
}




/************************************************************************
 *
 *  SetFileSelected
 *      Display the icon representing file_data as selected.
 *
 ************************************************************************/

static void
SetFileSelected(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data )
{
   /* mark selected */
   file_view_data->selected = True;

   /* if this file has an up-to-date gadget, change its colors */
   if (!file_view_data->need_update)
   {
      if (file_view_data->file_data->link == NULL)
         SetToSelectColors (file_view_data->widget,
              (Widget)((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window,
              0);
      else
         SetToSelectColors (file_view_data->widget,
              (Widget)((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window,
              LINK_FILE);
      if (PositioningEnabledInView(file_mgr_data))
         RedrawOneGadget(file_view_data->widget, NULL, NULL);
   }
}

/************************************************************************
 *
 *  SetFileUnselected
 *      Display the icon representing file_data as not selected.
 *
 ************************************************************************/

static void
SetFileUnselected(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data )
{
   /* mark selected */
   file_view_data->selected = False;

   /* if this file has an up-to-date gadget, change its colors */
   if (!file_view_data->need_update)
   {
      if (file_view_data->file_data->link != NULL)
         SetToNormalColors (file_view_data->widget,
                    ((FileMgrRec *)(file_mgr_data->file_mgr_rec))->file_window,
                    ((FileMgrRec *)(file_mgr_data->file_mgr_rec))->main,
                    LINK_FILE);
      else
         SetToNormalColors (file_view_data->widget,
                    ((FileMgrRec *)(file_mgr_data->file_mgr_rec))->file_window,
                    ((FileMgrRec *)(file_mgr_data->file_mgr_rec))->main,
                    0);

      if (PositioningEnabledInView(file_mgr_data))
         RedrawOneGadget(file_view_data->widget, NULL, NULL);
   }
}

/************************************************************************
 *
 *  SelectFile
 *      Add the file to the selection list.
 *
 ************************************************************************/

void
SelectFile(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data )
{
   int selection_count;
   int i;

   /* Add to the front of the selection list */
   selection_count = file_mgr_data->selected_file_count;
   file_mgr_data->selected_file_count++;

   file_mgr_data->selection_list = (FileViewData **)
      XtRealloc ((char *) file_mgr_data->selection_list,
                 sizeof(FileViewData *) * (selection_count + 2));

   for (i = file_mgr_data->selected_file_count; i > 0; i--)
      file_mgr_data->selection_list[i] = file_mgr_data->selection_list[i-1];

   file_mgr_data->selection_list[0] = file_view_data;

   /* mark selected */
   SetFileSelected(file_mgr_data, file_view_data);
}




/************************************************************************
 *
 *  DeselectFile
 *      Remove the file from the selection list.
 *
 ************************************************************************/

void
DeselectFile(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data,
        Boolean valid)
{
   int selection_count;
   register int i;
   register int j;


   selection_count = file_mgr_data->selected_file_count;
   file_mgr_data->selected_file_count--;

   for (i = 0; i < selection_count; i++)
      if (file_mgr_data->selection_list[i] == file_view_data)
         break;

   for (j = i; j < selection_count - 1; j++)
      file_mgr_data->selection_list[j] = file_mgr_data->selection_list[j + 1];

   file_mgr_data->selection_list = (FileViewData **)
      XtRealloc ((char *) file_mgr_data->selection_list,
                  sizeof(FileViewData *) * selection_count);
   file_mgr_data->selection_list[selection_count - 1] = NULL;

   if (valid)
     SetFileUnselected(file_mgr_data, file_view_data);
   else
     file_view_data->selected = False;
}




/************************************************************************
 *
 *  DeselectAllFiles
 *      Unselect all of the selected icons within the file manager data
 *
 ************************************************************************/

void
DeselectAllFiles(
        FileMgrData *file_mgr_data )
{
   FileViewData * file_view_data;
   register int i, j, k;
   ObjectPtr bottom;
   FileViewData ** repaint_list;


   /*  Get the color to reset the icons.  */

   /*  Loop through the selection set, resetting the visuals for  */
   /*  each selected icon.                                        */

   if (!PositioningEnabledInView(file_mgr_data))
   {
      for (i = 0; i < file_mgr_data->selected_file_count; i++)
      {
         file_view_data = file_mgr_data->selection_list[i];
         SetFileUnselected(file_mgr_data, file_view_data);
      }
   }
   else if (file_mgr_data->selected_file_count > 0)
   {
      /*
       * We can't simply redraw the selected files; we must also redraw
       * any unselected files which are higher on the stacking order.
       */

      repaint_list = (FileViewData **)XtMalloc(sizeof(FileViewData *) *
                     file_mgr_data->selected_file_count);

      /* Order the objects to be unselected */
      for (i = 0; i < file_mgr_data->selected_file_count; i++)
      {
         file_view_data = file_mgr_data->selection_list[i];
         for (j = 0; j < i; j++)
         {
            if (file_view_data->position_info->stacking_order <
                repaint_list[j]->position_info->stacking_order)
            {
               /* Insert here, pushing down all lower entries */
               for (k = file_mgr_data->selected_file_count - 1; k > j; k--)
                  repaint_list[k] = repaint_list[k-1];

               repaint_list[j] = file_view_data;
               break;
            }
         }

         /* Insert at end, if necessary */
         if (j >= i)
            repaint_list[i] = file_view_data;
      }

      /* Start the redraw process */
      i = file_mgr_data->selected_file_count - 1;
      bottom = repaint_list[i]->position_info;

      while(bottom)
      {
         if ((i >= 0) && (bottom == repaint_list[i]->position_info))
         {
            /* Unselect this object */
            SetFileUnselected(file_mgr_data, bottom->file_view_data);
            i--;
         }
         if(bottom->file_view_data != NULL &&
            !bottom->file_view_data->need_update)
         {
            RedrawOneGadget(bottom->file_view_data->widget, NULL, NULL);
         }
         bottom = bottom->prev;
      }

      XtFree((char *)repaint_list);
      repaint_list = NULL;
   }

   if (file_mgr_data->selection_list != NULL)
   {
      XtFree ((char *) file_mgr_data->selection_list);
      file_mgr_data->selection_list = NULL;
   }

   file_mgr_data->selection_list =
      (FileViewData **) XtMalloc (sizeof (FileViewData *));
   file_mgr_data->selection_list[0] = NULL;
   file_mgr_data->selected_file_count = 0;

}




/************************************************************************
 *
 *  SelectAllFiles
 *      Select all of the files within the file manager data.
 *
 ************************************************************************/

void
SelectAllFiles(
        FileMgrData *file_mgr_data )
{
   DirectorySet * directory_data;
   FileViewData ** order_list;
   int directory_count;
   int selection_count;
   register int i;
   register int j;
   ObjectPtr top;
   FileViewData ** selection_list;


   if (PositioningEnabledInView(file_mgr_data))
   {
      /* Force selection list order to match stacking order */
      selection_list = (FileViewData **)XtMalloc(sizeof(FileViewData *) *
             (file_mgr_data->num_objects));

      top = GetTopOfStack(file_mgr_data);
      selection_count = 0;
      for (i = 0; i < file_mgr_data->num_objects; i++)
      {
         /* If the there isn't file_view_data for it
            Or if it's a parent folder (..go up)
            Don't bother to select them
         */
         if( top->file_view_data != NULL
             && strcmp( top->name, ".." ) != 0 )
         {
           selection_list[selection_count] = top->file_view_data;
           ++selection_count;
         }
         top = top->next;
      }
      selection_list[selection_count] = NULL;

      /* Force redraw in bottom to top order */
      for (i = selection_count - 1; i >= 0; i--)
      {
         if (!FileIsSelected(file_mgr_data, selection_list[i]))
            SetFileSelected(file_mgr_data, selection_list[i]);
         }

      /* Free old selection list, and save new one */
      XtFree ((char *) file_mgr_data->selection_list);
      file_mgr_data->selection_list = selection_list;
   }
   else
   {
      /*  Free up any current selection and get the selection color.  */

      if (file_mgr_data->selection_list != NULL)
      {
        DeselectAllFiles( file_mgr_data );
      }

      if (file_mgr_data->show_type == SINGLE_DIRECTORY)
         directory_count = 1;
      else
         directory_count = file_mgr_data->directory_count;


      /*  Loop through the set of directories checking each file view   */
      /*  structure to see if the icon is filtered.  If not, select it  */
      /*  and increment the selection count.                            */

      selection_count = 0;

      /* For tree mode the index has to be -1 */

      i = (file_mgr_data->show_type == MULTIPLE_DIRECTORY)?-1:0;
      for (; i < directory_count; i++)
      {
         directory_data = file_mgr_data->directory_set[i];
         order_list = directory_data->order_list;

         for (j = 0; j < directory_data->file_count; j++)
         {
            /* If the file is being filtered out
               Or if it's a parent folder (..go up)
               Don't bother to select them
            */
            if (order_list[j]->filtered == True
                || strcmp( order_list[j]->file_data->file_name, ".." ) == 0 )
               continue;

            selection_count++;

            file_mgr_data->selection_list = (FileViewData **)
                XtRealloc ((char *) file_mgr_data->selection_list,
                           sizeof(FileViewData *) * (selection_count + 1));

            file_mgr_data->selection_list[selection_count] = NULL;
            file_mgr_data->selection_list[selection_count - 1] = order_list[j];
            SetFileSelected(file_mgr_data, order_list[j]);
         }
      }
   }

   file_mgr_data->selected_file_count = selection_count;

   if(file_mgr_data != trashFileMgrData)
   {
      if (selection_count == 0)
         ActivateNoSelect ((FileMgrRec *)file_mgr_data->file_mgr_rec);
      else if (selection_count == 1)
      {
         ActivateSingleSelect ((FileMgrRec *)file_mgr_data->file_mgr_rec,
                    file_mgr_data->selection_list[0]->file_data->logical_type);
      }
      else
         ActivateMultipleSelect ((FileMgrRec *)file_mgr_data->file_mgr_rec);
   }
}




/************************************************************************
 *
 *  SetToSelectColors
 *      Set a single icon widget to selected colors.
 *
 ************************************************************************/

void
SetToSelectColors(
        Widget widget,
        Widget file_window,
        int type)
{
   int j;
   Pixel background_color;
   Arg args[3];


   /*  Get the select color to be used as the background of  */
   /*  the icon gadgets.                                     */

   j = 0;
   XtSetArg (args[j], XmNbackground, &background_color); j++;
   XtGetValues (file_window, args, j);

   j = 0;
   if (background_color == white_pixel)
   {
      XtSetArg (args[j], XmNbackground, black_pixel); j++;
      XtSetArg (args[j], XmNforeground, white_pixel); j++;
   }
   else if (background_color == black_pixel)
   {
      XtSetArg (args[j], XmNbackground, white_pixel); j++;
      XtSetArg (args[j], XmNforeground, black_pixel); j++;
   }
   else
   {
      XtSetArg (args[j], XmNbackground, white_pixel); j++;
      XtSetArg (args[j], XmNforeground, black_pixel); j++;
   }

   XtSetValues (widget, args, j);
}

/************************************************************************
 *
 *  SetToNormalColors
 *      Set a single icon widget to normal colors.
 *
 ************************************************************************/

void
SetToNormalColors(
        Widget widget,
        Widget bg_fg_colors,
        Widget top_shadow_colors,
        int type)
{
   int j;
   Pixel  background_color;
   Pixel  foreground_color;
   Pixel  pixmap_background;
   DtIconGadget new = (DtIconGadget)widget;
   Arg args[5];

   /*  Get the colors to be used for drawing the icons  */

   j = 0;
   XtSetArg (args[j], XmNbackground, &background_color); j++;
   XtSetArg (args[j], XmNforeground, &foreground_color); j++;
   XtGetValues (bg_fg_colors, args, j);

   j = 0;
   XtSetArg (args[j], XmNtopShadowColor, &pixmap_background); j++;
   XtGetValues (top_shadow_colors, args, j);

   j = 0;
#ifdef _SHOW_LINK
   if(type == LINK_FILE)
      XtSetArg (args[j], XmNforeground, pixmap_background); j++;
   else
#endif
      XtSetArg (args[j], XmNforeground, foreground_color); j++;

   if (background_color == white_pixel)
   {
       XtSetArg (args[j], XmNbackground, white_pixel); j++;
   }
   else if (background_color == black_pixel)
   {
       XtSetArg (args[j], XmNbackground, black_pixel); j++;
   }
   else
   {
       XtSetArg (args[j], XmNbackground, background_color); j++;
   }

   /* we want to make sure the armed value is off so that it will get
      the correct GC */
   if(new->icon.armed == True)
   {
      new->icon.armed = False;
   }

   XtSetValues (widget, args, j);
}


/*
 * When a text widget is destroyed, we need to free up the string we
 * attached as userData.
 */

void
DestroyIconName (
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   char * str;
   Arg args[1];

   XtSetArg(args[0], XmNuserData, &str);
   XtGetValues(w, args, 1);
   XtFree(str);
}


int
#ifdef _NO_PROTO
GetInsertPosition( x1, x2, fontList, name )
   int x1;
   int x2;
   XmFontList fontList;
   char * name;
#else
GetInsertPosition( int x1, int x2, XmFontList fontList, char * name )
#endif
{
  int i, width, stringWidth;
  char * tmp;
  char savedChar;
#ifdef MULTIBYTE
  int len;
#endif /* MULTIBYTE */

  width = x2 - x1;

#ifdef MULTIBYTE
  i = 0;
  tmp = name;
  while ((len = mblen(tmp, MB_CUR_MAX)) > 0)
#else /* MULTIBYTE */
  for( tmp = name + 1, i = 0;
       *tmp != 0x0;
       ++tmp, ++i )
#endif /* MULTIBYTE */
  {
    XmString string;

#ifdef MULTIBYTE
    tmp += len;
#endif /* MULTIBYTE */

    savedChar = *tmp;
    *tmp = 0x0;
    string = XmStringCreateLocalized( name );
    stringWidth = XmStringWidth( fontList, string );
    XmStringFree( string );
    *tmp = savedChar;
    if( stringWidth > width )
      break;
#ifdef MULTIBYTE
    else
      i++;
#endif /* MULTIBYTE */
  }

  return( i );
}

void
CreateNameChangeDialog (
   Widget w,
   FileViewData *file_view_data,
   XtPointer client_data,
   int type)
{
   XRectangle textExtent;
   FileMgrData * file_mgr_data;
   DesktopRec * desktopWindow;
   Widget parent = XtParent(w);
   Widget text;
   Arg args[8];
   int n;
   Position x, y;
   Widget frame, shell;
   Dimension fHeight, sHeight;
#ifdef SHAPE
   Dimension tWidth, tHeight;
#endif
   char * name;
   XtTranslations trans_table;
   XmFontList fontList;
   Dimension stringWidth;
   XmString fileNameString;
   char tmpBuf[MAX_PATH];

   _DtIconGetTextExtent_r(w, &textExtent);

   if(type == DESKTOP)
   {
      char buf[MAXPATHLEN];

      desktopWindow = (DesktopRec *)client_data;
      if(desktopWindow->text != NULL)
         return;

      sprintf( buf, "%s:%s", home_host_name, root_title );

      /*
       * If the object is on the DESKTOP and its name is root_title, then
       * the user can't rename it.
       */
      if( strcmp( buf, desktopWindow->title ) == 0
          && strcmp( desktopWindow->dir_linked_to, "/" ) == 0
          && strcmp( desktopWindow->file_name, "." ) == 0 )
        return;
   }
   else
      file_mgr_data = (FileMgrData *)client_data;

   /* if the object is in the trash, can't rename it */
   if( type != DESKTOP
       && file_mgr_data == trashFileMgrData )
      return;

   /* if the object is an action, can't rename it */
   if( DtDtsDataTypeIsAction( file_view_data->file_data->logical_type ) )
   {
     char *tmpStr, *title, *msg;

     tmpStr = GETMESSAGE(10, 39, "Rename error");
     title = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(10, 40, "This object is an Action.\nAction icon labels cannot be directly renamed." );
     msg = XtNewString(tmpStr);

     _DtMessage(toplevel, title, msg, NULL, HelpRequestCB);
     XtFree(title);
     XtFree(msg);
     return;
   }
   else if(file_view_data->file_data->action_name)
   {
     char *tmpStr, *title, *msg;

     tmpStr = GETMESSAGE(10, 39, "Rename error");
     title = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(11, 32, "Cannot rename %s");
     msg = XtMalloc(strlen(tmpStr)+strlen(file_view_data->file_data->
                  action_name) +1);
     sprintf(msg,tmpStr,file_view_data->file_data->action_name);

     _DtMessage(toplevel, title, msg, NULL, HelpRequestCB);
     XtFree(title);
     XtFree(msg);
     return;
   }

   /*
    * The selected objects name is attached as 'userData' to text field,
    * to aid us in mapping back to the original object later.
    */
   if( type == DESKTOP &&
       ( (strcmp(".", file_view_data->file_data->file_name) == 0)
         || strcmp("..", file_view_data->file_data->file_name) == 0) )
   {
      name = XtNewString(desktopWindow->file_name);
   }
   else if ( strcmp(".", file_view_data->file_data->file_name) == 0 ||
             strcmp("..", file_view_data->file_data->file_name) == 0 )
   {
      return;
   }
   else
      name = XtNewString(file_view_data->file_data->file_name);

   /* set up translations in main edit widget */
   trans_table = XtParseTranslationTable(translations_escape);

   /* We need to set the width of the text widget.
      Can't use XmNcolumns because of the double-byte.
   */
   {
     XtSetArg( args[0], XmNfontList, &fontList );
     XtGetValues( file_view_data->widget, args, 1 );

     sprintf( tmpBuf, "%s    ", file_view_data->file_data->file_name );
     fileNameString = XmStringCreateLocalized( tmpBuf );
     stringWidth = XmStringWidth( fontList, fileNameString );
     XmStringFree( fileNameString );

     n = 0;
     XtSetArg(args[n], XmNuserData, name);                   n++;
     XtSetArg(args[n], XmNmarginHeight, 0);                  n++;
     XtSetArg(args[n], XmNmarginWidth, 0);                   n++;
     XtSetArg(args[n], XmNvalue, name);                      n++;
     XtSetArg(args[n], XmNwidth, stringWidth);               n++;
   }

   if(type == DESKTOP)
   {
      text = XmCreateTextField(parent, "nameChangeT_DT", args, n);
      XtAddCallback (text, XmNactivateCallback, ChangeIconNameDT,
                                                 (XtPointer)desktopWindow);
      XtAddCallback(text, XmNhelpCallback, (XtCallbackProc)DTHelpRequestCB,
                    HELP_NAMECHANGE_DIALOG_STR);
      desktopWindow->text = text;

      frame = XtParent(parent);
      shell = XtParent(frame);
      XtSetArg(args[0], XmNheight, &fHeight);
      XtGetValues(frame, args, 1);
      XtSetArg(args[0], XmNheight, &sHeight);
      XtGetValues(shell, args, 1);
   }
   else
   {
      text = XmCreateTextField(parent, "nameChangeT", args, n);
      file_mgr_data->renaming = file_view_data;
      XtAddCallback (text, XmNmotionVerifyCallback,
                     (XtCallbackProc)ChangeIconName,
                     (XtPointer)file_mgr_data);
      XtAddCallback (text, XmNmodifyVerifyCallback,
                     (XtCallbackProc)ChangeIconName,
                     (XtPointer)file_mgr_data);
      XtAddCallback (text, XmNactivateCallback,
                     (XtCallbackProc)ChangeIconName,
                     (XtPointer)file_mgr_data);
      XtAddCallback(text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_NAMECHANGE_DIALOG_STR);
   }

   XtAddCallback (text, XmNdestroyCallback, DestroyIconName, (XtPointer)NULL);

   /* set up translations in main edit widget */
   XtOverrideTranslations(text, trans_table);

   if(type == DESKTOP && sHeight > fHeight) {
       /* status area has been attached in multi-byte case */
       x = 0;
       y = textExtent.y;
   }
   else {
       x = textExtent.x;
       y = textExtent.y - (Dimension)(text->core.height - textExtent.height);
   }

   {
     Window rootWindow, childWindow;
     int pX, pY, rootX, rootY, insertPosition;
     unsigned int mask;

     XQueryPointer( XtDisplay( parent ), XtWindow( file_view_data->widget ),
                    &rootWindow, &childWindow, &rootX, &rootY, &pX, &pY,
                    &mask );
     if( pX == 0 )
       insertPosition = strlen( name );
     else
     {
       if( type != DESKTOP )
       {
         if( pX > (int) x && pX < (int) x + (int) stringWidth )
           insertPosition = GetInsertPosition( x, pX, fontList, name );
         else
           insertPosition = strlen( name );
       }
       else
         insertPosition = GetInsertPosition( x, x + pX, fontList, name );
     }
     n = 0;
     XtSetArg( args[n], XmNcursorPosition, insertPosition);     n++;
     XtSetArg( args[n], XmNx, x);                               n++;
     XtSetArg( args[n], XmNy, y);                               n++;
     XtSetValues (text, args, n);
   }

#ifdef SHAPE
   if(shapeExtension)
      if(type == DESKTOP)
      {
         Dimension shadowThickness;
         XRectangle rect[2];
         unsigned char flags;

         XtSetArg (args[0], XmNwidth, &tWidth);
         XtSetArg (args[1], XmNheight, &tHeight);
         XtGetValues (text, args, 2);

         XtSetArg (args[0], XmNshadowThickness, &shadowThickness);
         XtGetValues (desktopWindow->iconGadget, args, 1);

         _DtIconGetIconRects((DtIconGadget)desktopWindow->iconGadget,
                                                &flags, &rect[0], &rect[1]);
        /* 1 */
         rect[0].x += 1;
         rect[0].y += 1;
         rect[0].width += 2*shadowThickness;
         rect[0].height += 2*shadowThickness;

        /* 2 */
         if(type == DESKTOP && sHeight > fHeight) {
             /* status area has been attached in multi-byte case */
             rect[1].x = x + 1;
             rect[1].y = y + 1;
             rect[1].width = 2*shadowThickness + tWidth;
             rect[1].height = sHeight - y - 1;
         }
         else {
             rect[1].x += 1;
             rect[1].y += 1;
             rect[1].width = shadowThickness + tWidth;
             rect[1].height = tHeight;
         }

         if(rect[0].width > rect[1].width)
           rect[1].width = rect[0].width;

         XShapeCombineRectangles(XtDisplay(desktopWindow->shell),
                                 XtWindow(desktopWindow->shell),
                                 ShapeBounding, 0, 0, &rect[0], 2,
                                 ShapeSet, Unsorted);
      }
#endif
   XtManageChild(text);
   XmProcessTraversal(text, XmTRAVERSE_CURRENT);

   if(type != DESKTOP)
   {
      FileMgrRec *file_mgr_rec;

      file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
      file_mgr_rec->menuStates &= ~RENAME;
   }
}

/************************************************************************
 *
 *  SavePositionalData
 *      Save the desktop icon positional data.
 *
 ************************************************************************/

void
SavePositionalData (
   int fd,
   FileMgrData * file_mgr_data,
   char ** name_list,
   char * name)
{
   int i;
   FILE * fd_stream = fdopen(fd, "w");
   ObjectPosition * ptr;


   if(file_mgr_data->positionEnabled == RANDOM_ON &&
                  file_mgr_data->object_positions &&
                  file_mgr_data->show_type == SINGLE_DIRECTORY &&
                  file_mgr_data->host != NULL)
   {
      /* Number of object positions */
      fprintf(fd_stream, "*%s.%s.%s.%s:   %d\n#\n", name_list[0], name_list[1],
               name, "num_positions", file_mgr_data->num_objects);

      for (i = 0; i < file_mgr_data->num_objects; i++)
      {
         ptr = file_mgr_data->object_positions[i];
         fprintf(fd_stream, "*%s.%s.%s.%s%d:   %s %d %d %d\n",
                   name_list[0], name_list[1], name, "object", i,
                   ptr->name,
                   ptr->x,
                   ptr->y,
                   ptr->stacking_order);
      }
   }
   else
   {
      /* Number of object positions */
      fprintf(fd_stream, "*%s.%s.%s.%s:   %d\n#\n", name_list[0], name_list[1],
               name, "num_positions", 0);
   }

   fflush(fd_stream);
}



/************************************************************************
 *
 *  RestorePositionalData
 *      Restore the desktop icon positional data.
 *
 ************************************************************************/

void
RestorePositionalData (
   XrmDatabase db,
   char ** name_list,
   FileMgrData * file_mgr_data,
   char * name)
{
   XrmName xrm_name[10];
   int i = 0;
   int j;
   char objectName[20];
   XrmRepresentation rep_type;
   XrmValue value;
   int num_objects;
   char * str;
   char * obj_name;
   ObjectPosition * ptr;
   int x, y;
   int s_order;

   while (name_list[i])
   {
      xrm_name[i] = XrmStringToQuark(name_list[i]);
      i++;
   }
   xrm_name[i++] = XrmStringToQuark(name);
   xrm_name[i] = XrmStringToQuark("num_positions");
   xrm_name[i+1] = '\0';

   /* Find out how many objects there are to be loaded */
   file_mgr_data->object_positions = NULL;
   num_objects = 0;
   if (XrmQGetResource (db, xrm_name, NULL, &rep_type, &value))
   {
      if ((num_objects = atoi (value.addr)) <= 0)
         num_objects = 0;
      else
      {
         file_mgr_data->object_positions = (ObjectPosition **)XtMalloc(
                             sizeof(ObjectPosition *) * num_objects);
      }
   }

   file_mgr_data->num_objects = num_objects;

   for (j = 0; j < num_objects; j++)
   {
      sprintf(objectName, "object%d", j);
      xrm_name[i] = XrmStringToQuark(objectName);
      ptr = file_mgr_data->object_positions[j] = (ObjectPosition *)
                                     XtMalloc(sizeof(ObjectPosition));

      XrmQGetResource (db, xrm_name, NULL, &rep_type, &value);
      str = (char *)value.addr;
      obj_name = XtMalloc(strlen(str) + 1);
      sscanf((char *)value.addr, "%s %d %d %d", obj_name, &x, &y, &s_order);
      ptr->name = obj_name;
      ptr->x = x;
      ptr->y = y;
      ptr->stacking_order = s_order;
      ptr->in_use = False;
      ptr->late_bind = False;
      ptr->file_view_data = NULL;
      ptr->next = NULL;
      ptr->prev = NULL;
   }

   /* Repair all of the next and prev pointers */
   RepairStackingPointers(file_mgr_data);
}


void
UnpostTextField (
   FileMgrData * file_mgr_data)

{
   XmManagerWidget file_window;
   int i;
   FileMgrRec * file_mgr_rec;

   file_mgr_data->renaming = NULL;

   file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   for (i = 0; i < file_window->composite.num_children; i++)
   {
      if (XmIsTextField(file_window->composite.children[i]) &&
          !file_window->composite.children[i]->core.being_destroyed)
      {
         XtUnmanageChild(file_window->composite.children[i]);
         XtDestroyWidget(file_window->composite.children[i]);
         return;
      }
   }
}


void
UnpostTextPath (
   FileMgrData * file_mgr_data)

{
  FileMgrRec * file_mgr_rec;
  Arg args[2];

  file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;

  XtSetArg (args[0], XmNallowShellResize, False);
  XtSetValues(file_mgr_rec->shell, args, 1);

  XtUnmanageChild(file_mgr_rec->current_directory_text);

  XtSetArg (args[0], XmNallowShellResize, True);
  XtSetValues(file_mgr_rec->shell, args, 1);

  file_mgr_data->fast_cd_enabled = False;
}


/*
 * Function to force a redraw of a single gadget.
 */

void
RedrawOneGadget (
   Widget child,
   XEvent * event,
   Region region)

{
   if (child->core.widget_class->core_class.expose)
      (*(child->core.widget_class->core_class.expose))(child, event, region);
}


/*
 * Given a region describing the area to be repainted, repaint all icons,
 * in bottom to top order, which fall into this region.
 */

void
RepaintDesktop (
   FileMgrData * file_mgr_data,
   XEvent * event,
   Region region)

{
   Widget child;
   ObjectPosition * bottom = GetBottomOfStack(file_mgr_data);

   /* Redraw all affected gadgets */
   while(bottom)
   {
      if (!bottom->late_bind)
      {
         if (bottom->file_view_data != NULL &&
             !bottom->file_view_data->need_update)
         {
            child = bottom->file_view_data->widget;
            if (XRectInRegion(region, child->core.x, child->core.y,
                              child->core.width, child->core.height))
            {
               RedrawOneGadget(child, event, region);
            }
         }
      }

      bottom = bottom->prev;
   }
}


/*
 * This function will extract all exposure events intended for this file
 * window, combine them into a single region, and then determine which of
 * the file icons need to be redrawn.  It is a two step process, as
 * described by the comments in the code.
 */

static void
RedisplayUsingStackingOrder (
   FileMgrData * file_mgr_data,
   Widget w,
   register XEvent *event,
   Region region)

{
   register Widget child;
   FileViewData * file_view_data;
   ObjectPosition * bottom;
   ObjectPosition * top;
   Region redrawRegion = XCreateRegion();
   XRectangle rect;
   XEvent expEvent;
   int numChildren = 0;
   Widget * children = NULL;
   Region widget_region;
   Region tmp_region;

   /* Get the initial region to redraw */
   if (region)
      XUnionRegion(redrawRegion, region, redrawRegion);
   else if (event)
   {
      rect.x = event->xexpose.x;
      rect.y = event->xexpose.y;
      rect.height = event->xexpose.height;
      rect.width = event->xexpose.width;

      XUnionRectWithRegion(&rect, redrawRegion, redrawRegion);
   }

   /*
    * Capture and encompass any other exposure events which are destined for
    * us, but are waiting in the wings.
    */
   while (XCheckWindowEvent(XtDisplay(w), XtWindow(w), ExposureMask, &expEvent))
   {
      rect.x = expEvent.xexpose.x;
      rect.y = expEvent.xexpose.y;
      rect.height = expEvent.xexpose.height;
      rect.width = expEvent.xexpose.width;

      XUnionRectWithRegion(&rect, redrawRegion, redrawRegion);
   }

   /* Nothing to do if the redraw region is empty */
   if (XEmptyRegion(redrawRegion))
   {
      XDestroyRegion(redrawRegion);
      return;
   }

   /*
    * Starting at the top of the stack, find any items which fall into
    * the redraw region.  As items are found which need to be redrawn,
    * subtract them from the redraw region, so that any items lower on the
    * stack which might be under these gadgets, but are within the region,
    * are not redrawn.
    *
    * The second phase is to start at the bottom of the stack, and start
    * redrawing the gadgets which fell into the region, along with any
    * other gadgets on top of these (since they too now need to be redrawn.
    */
   top = GetTopOfStack(file_mgr_data);
   children = NULL;
   numChildren = 0;

   while (top)
   {
      if (!top->late_bind)
      {
         file_view_data = top->file_view_data;
         if(file_view_data != NULL)
         {
            child = file_view_data->widget;

            if (child && XmIsGadget(child) && XtIsManaged(child))
            {
               widget_region = XCreateRegion();
               WidgetRectToRegion(file_mgr_data, child, widget_region);
               XIntersectRegion(redrawRegion, widget_region, widget_region);
               if (!XEmptyRegion(widget_region))
               {
                  XSubtractRegion(redrawRegion, widget_region, redrawRegion);
                  children = (Widget *)XtRealloc((char *)children,
                                         (numChildren + 1) * sizeof(Widget));
                  children[numChildren] = child;
                  numChildren++;
               }
               XDestroyRegion(widget_region);
            }
         }
      }
      top = (ObjectPosition *)top->next;
   }

   /* Now, start redrawing, in bottom to top order */
   bottom = GetBottomOfStack(file_mgr_data);
   numChildren--;
   while (bottom)
   {
      if (!bottom->late_bind)
      {
         file_view_data = bottom->file_view_data;
         if(file_view_data != NULL)
         {
            child = file_view_data->widget;

            if (child && XmIsGadget(child) && XtIsManaged(child))
            {
               widget_region = XCreateRegion();
               WidgetRectToRegion(file_mgr_data, child, widget_region);
               if ((numChildren >= 0) && (children[numChildren] == child))
               {
                  XUnionRegion(redrawRegion, widget_region, redrawRegion);
                  RedrawOneGadget(child, event, redrawRegion);
                  numChildren--;
               }
               else
               {
                  /* Do we overlap something which was previously redrawn? */
                  tmp_region = XCreateRegion();
                  XIntersectRegion(redrawRegion, widget_region, tmp_region);
                  if (!XEmptyRegion(tmp_region))
                  {
                     XUnionRegion(redrawRegion, widget_region, redrawRegion);
                     RedrawOneGadget(child, event, redrawRegion);
                  }
                  XDestroyRegion(tmp_region);
               }
               XDestroyRegion(widget_region);
            }
         }
      }
      bottom = (ObjectPosition *)bottom->prev;
   }

   XDestroyRegion(redrawRegion);
   XtFree((char *)children);
   children = NULL;
}


/*
 * This is the function which we use to override the class expose function
 * for the drawing area widget.  It allows us to catch exposure events
 * intended for the desktop, so that we can force the redrawing of the
 * gadgets to occur occording to the stacking order.
 */

void
DrawingAreaRedisplay (
   Widget wid,
   XEvent *event,
   Region region)

{
   XmDrawingAreaWidget da = (XmDrawingAreaWidget) wid;
   XmDrawingAreaCallbackStruct cb;
   FileMgrData * file_mgr_data;

   cb.reason = XmCR_EXPOSE;
   cb.event = event;
   cb.window = XtWindow (da);

   DPRINTF2(("DrawingAreaRedisplay: event %d, x/y %d/%d, wd/ht %d/%d\n",
             event->xany.type,
             event->xexpose.x, event->xexpose.y,
             event->xexpose.width, event->xexpose.height));

   file_mgr_data = ReturnDesktopPtr(wid);

   if (file_mgr_data && PositioningEnabledInView(file_mgr_data))
      RedisplayUsingStackingOrder(file_mgr_data, (Widget)da, event, region);
   else
      XmeRedisplayGadgets((Widget)da, event, region);

   XtCallCallbackList ((Widget)da, da->drawing_area.expose_callback, &cb);
}


/*
 * Return the bottom of the stacking order list.
 */

ObjectPtr
GetBottomOfStack (
   FileMgrData * file_mgr_data)

{
   int i;
   ObjectPtr bottom;

   for (i = 0, bottom = NULL; i < file_mgr_data->num_objects; i++)
   {
      if (file_mgr_data->object_positions[i]->next == NULL)
      {
         bottom = file_mgr_data->object_positions[i];
         break;
      }
   }

   return(bottom);
}


/*
 * Return the top of the stacking order list.
 */

ObjectPtr
GetTopOfStack (
   FileMgrData * file_mgr_data)
{
   int i;
   ObjectPtr top;

   for (i = 0, top = NULL; i < file_mgr_data->num_objects; i++)
   {
      if (file_mgr_data->object_positions[i]->prev == NULL)
      {
         top = file_mgr_data->object_positions[i];
         break;
      }
   }

   return(top);
}


/*
 * Move an object up in the stacking order.  Will not work for moving an
 * object further down in the stacking order, but we currently have no
 * need for doing that.
 *
 * NOTE: The top of the stack is '1', not '0'!!
 */

void
RepositionUpInStack (
   FileMgrData * file_mgr_data,
   int oldPos,
   int newPos)

{
   ObjectPtr ptr, ptr2, savePtr;
   register int i, j;
   XmManagerWidget file_window;

   if (oldPos == newPos)
      return;

   ptr = GetTopOfStack(file_mgr_data);

   /* Find where item is to be inserted */
   while (ptr->stacking_order != newPos)
      ptr = ptr->next;

   /* Find where item currently is */
   ptr2 = ptr;
   while (ptr2->stacking_order != oldPos)
      ptr2 = ptr2->next;

   savePtr = ptr2;

   if(savePtr->file_view_data == NULL)
      return;

   /* Remove from its current location */
   if (ptr2->prev)
      ptr2->prev->next = ptr2->next;
   if (ptr2->next)
      ptr2->next->prev = ptr2->prev;

   /* Link into new position */
   ptr2->prev = ptr->prev;
   ptr2->next = ptr;
   if (ptr->prev)
      ptr2->prev->next = ptr2;
   ptr->prev = ptr2;

   /* Update the stacking order value */
   for (i = newPos; i <= oldPos; i++)
   {
      ptr2->stacking_order = i;
      ptr2 = ptr2->next;
   }

   /* Update the children's list for the file window */
   file_window = (XmManagerWidget)
                  (((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window);

   /* Find the affect child */
   for (i = 0; (i < file_window->composite.num_children) &&
       (file_window->composite.children[i] != savePtr->file_view_data->widget);
       i++);

   /* Push intervening entries down */
   for (j = i; j >= newPos; j--)
      file_window->composite.children[j] = file_window->composite.children[j-1];

   /* Insert into new position in children list */
   file_window->composite.children[newPos-1] = savePtr->file_view_data->widget;
}


/*
 * Reorder the file window's children list so that it matches the stacking
 * order.
 */

static void
ReorderChildrenList (
   XmManagerWidget file_window,
   Widget * manage,
   int manageCount,
   Widget * unmanage,
   int unmanageCount)

{
   Widget * children = file_window->composite.children;
   register int i, j;

   for (i = 0; i < manageCount; i++)
      children[i] = manage[i];

   for (j = 0; j < unmanageCount; j++)
      children[i++] = unmanage[j];
}


/*
 * If positional information is available for the indicated file, then
 * return it; if not, then return NULL.
 */

ObjectPtr
FindCurrentPosition (
   FileMgrData * file_mgr_data,
   char * file_name)

{
   int i;

   for (i = 0; i < file_mgr_data->num_objects; i++)
   {
      if (strcmp(file_name, file_mgr_data->object_positions[i]->name) == 0)
      {
         /* Found a match */
         file_mgr_data->object_positions[i]->in_use = True;
         return(file_mgr_data->object_positions[i]);
      }
   }

   return(NULL);
}


void
RepairStackingPointers (
   FileMgrData * file_mgr_data)

{
   int i;
   int j;
   ObjectPosition * ptr;
   ObjectPosition * prev;

   for (i = 1, prev = NULL; i <= file_mgr_data->num_objects; i++)
   {
      for (j = 0; j < file_mgr_data->num_objects; j++)
      {
         ptr = file_mgr_data->object_positions[j];
         if (ptr->stacking_order == i)
         {
            if (prev)
            {
               ptr->prev = prev;
               prev->next = ptr;
            }
            prev = ptr;
            break;
         }
      }
   }
}

/*
 * Reorder the children's list for the file_window, so that it matches
 * the stacking order.  Also, set up all next and previous pointers.
 */

void
OrderChildrenList (
   FileMgrData * file_mgr_data)

{
   FileMgrRec * file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;
   XmManagerWidget file_window;
   Widget * managed;
   Widget * unmanaged;
   int num_managed;
   int num_unmanaged;
   ObjectPosition * top;
   int i, j;

   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   managed = (Widget *)XtMalloc(sizeof(Widget *) *
                        file_window->composite.num_children);
   unmanaged = (Widget *)XtMalloc(sizeof(Widget *) *
                        file_window->composite.num_children);
   num_managed = num_unmanaged = 0;

   top = GetTopOfStack(file_mgr_data);
   while(top)
   {
      if (top->file_view_data != NULL && top->file_view_data->widget != NULL)
         managed[num_managed++] = top->file_view_data->widget;
      top = top->next;
   }

   /* All the rest get put at the end of the children's list */
   for (i = 0; i < file_window->composite.num_children; i++)
   {
      for (j = 0; j < num_managed; j++)
      {
         if (managed[j] == file_window->composite.children[i])
            break;
      }

      if (j >= num_managed)
         unmanaged[num_unmanaged++] = file_window->composite.children[i];
   }

   ReorderChildrenList(file_window, managed, num_managed, unmanaged,
                       num_unmanaged);
   XtFree( (char *)managed );
   XtFree( (char *)unmanaged );
}


/*
 * SetHotRects
 */

void
SetHotRects (
   FileViewData  * file_view_data,
   XtCallbackProc callback,
   XtPointer callback_data)

{
   Arg args[3];

   if (file_view_data->displayed)
   {
      /*************************/
      /* icon gadget displayed */
      /*************************/
      DtIconGadget g = (DtIconGadget) file_view_data->widget;
      unsigned char operations;

         /* find defined operations (M/C/L) for file type */
      operations = TypeToDropOperations(
                             file_view_data->file_data->logical_type);

      /* if icon gadget not yet registered as a drop site, do so now */
      if (!file_view_data->registered)
      {
         /* register drop site for MCL but make drop site inactive */
         XtSetArg (args[0], XmNdropSiteOperations,
                   XmDROP_COPY | XmDROP_MOVE | XmDROP_LINK);
         XtSetValues (file_view_data->widget, args, 1);

         file_view_data->registered = True;
      }

      if (operations)
      {
         /***********************************/
         /* file has associated MCL actions */
         /***********************************/
         XRectangle rects[2];
         unsigned char flags;
         int numRects = 0;
         Cardinal n = 0;
         Dimension s_t, adj_xy, adj_size;

         /* ensure drop site operations and drop area are correct */
         _DtIconGetIconRects(g, &flags, &rects[0], &rects[1]);

         /*
          * Compute adjustments to the drop area:
          *
          *  - Add shadowThickness to the drop area:
          *    The icon gadget leaves space of width shadowThickness around
          *    the pixmap and label; use this space to make the drop zone
          *    larger.
          *
          *  - Compensate for bug in drop zone registration/drawing:
          *    For some reason the drop zone displayed on the screen is
          *    actually smaller than the rectangle we register:
          *    The drag&drop library seems to reduce the size of the drop
          *    area by the value of highlightThickness at the top and left
          *    border and by highlightThickness-1 at the bottom and right.
          *    Is this a Motif drag&drop bug?
          *    We compensate by registering a larger rectangle.
          */
         s_t = G_ShadowThickness(g);
         adj_xy = s_t + G_HighlightThickness(g);
         adj_size = adj_xy + s_t + G_HighlightThickness(g) - 1;

         if (flags & XmPIXMAP_RECT)
         {
            rects[0].x -= g->rectangle.x + adj_xy;
            rects[0].y -= g->rectangle.y + adj_xy;
            rects[0].width += adj_size;
            rects[0].height += adj_size;
            numRects++;
          }

         if (flags & XmLABEL_RECT)
         {
            rects[1].x -= g->rectangle.x + adj_xy;
            rects[1].y -= g->rectangle.y + adj_xy;
            rects[1].width += adj_size;
            rects[1].height += adj_size;
            if (!numRects) rects[0] = rects[1];
               numRects++;
          }

          if (numRects)
          {
             XtSetArg(args[n], XmNdropRectangles, rects);        n++;
             XtSetArg(args[n], XmNnumDropRectangles, numRects);  n++;
          }
          XtSetArg (args[n], XmNdropSiteOperations, operations); n++;

          XmDropSiteUpdate (file_view_data->widget, args, n);
          g->icon.operations = operations;

          /* add client data */
          XtRemoveAllCallbacks(file_view_data->widget, XmNdropCallback);
          XtAddCallback(file_view_data->widget, XmNdropCallback,
                      callback, callback_data);
      }
      else
      {
         /*********************************************/
         /* file does not have associated MCL actions */
         /*********************************************/
         /* make drop site inactive */
         XtSetArg (args[0], XmNdropSiteOperations, XmDROP_NOOP);
         XmDropSiteUpdate (file_view_data->widget, args, 1);
         XtRemoveAllCallbacks(file_view_data->widget, XmNdropCallback);
      }
    }
    else
    {
      /*****************************/
      /* icon gadget not displayed */
      /*****************************/
      if (file_view_data->registered)
      {
         /***************************************/
         /* icon gadget registered as drop site */
         /***************************************/
         /* make drop site inactive */
         XtSetArg (args[0], XmNdropSiteOperations, XmDROP_NOOP);
         XmDropSiteUpdate (file_view_data->widget, args, 1);
         XtRemoveAllCallbacks(file_view_data->widget, XmNdropCallback);
      }
   }
}



/*
 * TypeToDropOperations
 */

unsigned char
TypeToDropOperations (
   char * file_type)

{
   unsigned char operations = 0L;
   char *action;

   /* does object have MOVE, COPY, and/or LINK actions */
   /*    -- or no actions at all                       */
   if (action = DtDtsDataTypeToAttributeValue(file_type,
                                              DtDTS_DA_MOVE_TO_ACTION,
                                              NULL))
   {
      operations = operations | XmDROP_MOVE;
      DtDtsFreeAttributeValue(action);
   }
   if (action = DtDtsDataTypeToAttributeValue(file_type,
                                              DtDTS_DA_COPY_TO_ACTION,
                                              NULL))
   {
      operations = operations | XmDROP_COPY;
      DtDtsFreeAttributeValue(action);
   }
   if (action = DtDtsDataTypeToAttributeValue(file_type,
                                              DtDTS_DA_LINK_TO_ACTION,
                                              NULL))
   {
      operations = operations | XmDROP_LINK;
      DtDtsFreeAttributeValue(action);
   }

   return(operations);
}


/*
 * TypeToAction
 */

char *
TypeToAction (
   unsigned char dropOperation,
   char * logical_type)
{
   char * action;

   /* retrieve action from database based on dropOperation */
   switch(dropOperation)
   {
      case XmDROP_MOVE:
         action = DtDtsDataTypeToAttributeValue(logical_type,
                                                DtDTS_DA_MOVE_TO_ACTION,
                                                NULL);
         break;
      case XmDROP_COPY:
         action = DtDtsDataTypeToAttributeValue(logical_type,
                                                DtDTS_DA_COPY_TO_ACTION,
                                                NULL);
         break;
      case XmDROP_LINK:
         action = DtDtsDataTypeToAttributeValue(logical_type,
                                                DtDTS_DA_LINK_TO_ACTION,
                                                NULL);
         break;
      default:
         action = NULL;
         break;
   }

   return(action);
}


/*
 * Given a fileViewData pointer, determine if it is part of a FileMgrData,
 * or a DesktopRec; return the appropriate pointer.
 */

static
Boolean
IsDesktopPtr (
   FileViewData * fileViewData,
   FileMgrData ** fileMgrData,
   DesktopRec ** desktopRec)
{
   int i;

   *desktopRec = NULL;
   *fileMgrData = NULL;

   for (i = 0; i < desktop_data->numIconsUsed; i++)
   {
      if (desktop_data->desktopWindows[i]->file_view_data == fileViewData)
      {
         *desktopRec = desktop_data->desktopWindows[i];
         return(True);
      }
   }

   *fileMgrData = (FileMgrData *)
                ((DirectorySet *)fileViewData->directory_set)->file_mgr_data;
   return(False);
}


/******************************************************************
 *
 *  PositionFileView -
 *        Given a particular file, position the file view that file
 *        is in on that particular file.
 *
 ********************************************************************/
void
PositionFileView(
     FileViewData *file_view_data,
     FileMgrData *file_mgr_data)
{
   Position x, y;
   Arg args[1];
   int value, size, increment, page, max;
   FileMgrRec * file_mgr_rec = (FileMgrRec *)(file_mgr_data->file_mgr_rec);
   Widget p;

   /* if the file is filtered we can't highlight it! */
   if(!file_view_data->displayed)
      return;

   /* Get the y location of the icon_gadget */
   y = file_view_data->y - file_mgr_data->grid_height;

   /* Two things:
    * 
    * 1) not sure exactly how slow this is, but there seems to be no other
    *    way to do this. (CDExc23427)
    * 2) XmScrollVisible does not work if the work window is not managed...
    *    so, we call it, and then get the appropriate x,y back from the
    *    scroll bars and set them (CDExc23428) */
   p = XtVaCreateManagedWidget("positionicon", xmGadgetClass, 
			       file_mgr_rec->file_window,
			       XmNmappedWhenManaged, False,
			       XmNx, file_view_data->x,
			       XmNy, y, 
			       XmNwidth, file_mgr_data->grid_width,
			       XmNheight, file_mgr_data->grid_height,
			       NULL);

   XmScrollVisible(file_mgr_rec->scroll_window, p,
		   XSPACING, YSPACING(file_mgr_data));

   XtDestroyWidget(p);

   XmScrollBarGetValues(file_mgr_rec->horizontal_scroll_bar,
			&value, &size, &increment, &page);
   x = -((Position) value);

   XmScrollBarGetValues(file_mgr_rec->vertical_scroll_bar,
			&value, &size, &increment, &page);
   y = -((Position) value);

   XtVaSetValues(file_mgr_rec->file_window, XmNx, x, XmNy, y, NULL);
}


static Boolean
InMultipleObjectRegion (FileMgrData * file_mgr_data, FileViewData * fileViewData)
{
  register int i;
  for( i = 0; i < file_mgr_data->selected_file_count; ++i )
    if( strcmp( file_mgr_data->selection_list[i]->file_data->file_name, fileViewData->file_data->file_name ) == 0 )
      return True;
  return False;
}

void
FmPopup (
      Widget w,
      XtPointer client_data,
      XEvent *event,
      FileMgrData *file_mgr_data)
{
   FileMgrRec      *file_mgr_rec;
   Arg             args[2];
   FileViewData    *fileViewData = NULL;
   int             i, num_of_children, obj_btns;
   XmManagerWidget action_pane;
   XmString        label_string;


   /* set number of popup children based on annotation */
   num_of_children = FM_POPUP_CHILDREN_NA;
   obj_btns = OBJ_BTNS_NA;

   /* attach the popup widget info to the menu */
   file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
   XtSetArg(args[0], XmNuserData, file_mgr_rec);
   XtSetValues(fileMgrPopup.menu, args, 1);

   if(file_mgr_data)
     file_mgr_data->popup_menu_icon = NULL;

   /* we are dealing with a white space popup */
   if((w == NULL)
      && (client_data == NULL)
/*
      && (file_mgr_data->selected_file_count == 0)
*/
      )
   {
      DirectorySet *directory_set;

      /* retrieve the fileViewData for the current directory */
      directory_set = file_mgr_data->directory_set[0];
      for (i = 0; i < directory_set->file_count; i++)
      {
         if(strcmp(directory_set->order_list[i]->file_data->file_name, ".")
            == 0)
         {
            fileViewData = directory_set->order_list[i];
            break;
         }
      }

      /* manage the white space buttons and unmanage the object buttons */
      XtManageChildren(fileMgrPopup.wsPopup, WS_BTNS);
      XtUnmanageChildren(fileMgrPopup.objPopup, obj_btns);
      XtUnmanageChildren(fileMgrPopup.trash_objPopup, TRASH_OBJ_BTNS);

      XtUnmanageChild(fileMgrPopup.wsPopup[BTN_UNSELECTALL]);

      if(file_mgr_data == trashFileMgrData)
      {
         /* Set popup menu label */
         label_string = XmStringCreateLocalized ((GETMESSAGE(33, 2, "Current Folder")));
         XtSetArg (args[0], XmNlabelString, label_string);
         XtSetValues (fileMgrPopup.title, args, 1);
         XtManageChild(fileMgrPopup.title);
         XmStringFree (label_string);

         /* trash white space popup -- unmanage the properties and show */
         /* hidden files buttons */
         XtUnmanageChild(fileMgrPopup.wsPopup[BTN_PROPERTIES]);
         XtUnmanageChild(fileMgrPopup.wsPopup[BTN_FIND]);
         XtUnmanageChild(fileMgrPopup.wsPopup[BTN_SHOWHIDDEN]);

         /* align the remaining buttons */
         XtSetArg(args[0], XmNmarginLeft, 0);
         if( PositioningEnabledInView( file_mgr_data ) )
         {
           file_mgr_rec->menuStates |= CLEAN_UP;
           XtSetValues(fileMgrPopup.wsPopup[BTN_CLEANUP], args, 1);
         }
         else
         {
           file_mgr_rec->menuStates &= ~(CLEAN_UP);
           XtUnmanageChild( fileMgrPopup.wsPopup[BTN_CLEANUP] );
         }
         XtSetValues(fileMgrPopup.wsPopup[BTN_SELECTALL], args, 1);

         /* unmanage the action portion of the popup menu */
         XtUnmanageChild(fileMgrPopup.action_separator);

         action_pane = (XmManagerWidget) fileMgrPopup.menu;
         for(i=num_of_children; i<action_pane->composite.num_children; i++)
            XtUnmanageChild(action_pane->composite.children[i]);
      }
      else
      {
         Dimension margin;

         /* manage the properties and show hidden files buttons */
         if( file_mgr_data->toolbox )
           XtUnmanageChild(fileMgrPopup.wsPopup[BTN_PROPERTIES]);
         else
           XtManageChild(fileMgrPopup.wsPopup[BTN_PROPERTIES]);
         XtManageChild(fileMgrPopup.wsPopup[BTN_FIND]);
         XtManageChild(fileMgrPopup.wsPopup[BTN_SHOWHIDDEN]);

         /* Set popup menu label */
         label_string = XmStringCreateLocalized ((GETMESSAGE(33, 2, "Current Folder")));
         XtSetArg (args[0], XmNlabelString, label_string);
         XtSetValues (fileMgrPopup.title, args, 1);
         XtManageChild(fileMgrPopup.title);
         XmStringFree (label_string);

         /* align the menu buttons */
         action_pane = (XmManagerWidget) fileMgrPopup.menu;
         XtSetArg(args[0], XmNmarginLeft, &margin);
         XtGetValues(fileMgrPopup.wsPopup[BTN_SHOWHIDDEN], args, 1);
         XtSetArg(args[0], XmNmarginLeft, margin);

         if( PositioningEnabledInView( file_mgr_data ) )
         {
           file_mgr_rec->menuStates |= CLEAN_UP;
           XtSetValues(fileMgrPopup.wsPopup[BTN_CLEANUP], args, 1);
         }
         else
         {
           file_mgr_rec->menuStates &= ~(CLEAN_UP);
           XtUnmanageChild( fileMgrPopup.wsPopup[BTN_CLEANUP] );
         }

         XtSetValues(fileMgrPopup.wsPopup[BTN_SELECTALL], args, 1);

         if(showFilesystem )
         {
           if (file_mgr_data->show_hid_enabled)
             XmToggleButtonGadgetSetState(fileMgrPopup.wsPopup[BTN_SHOWHIDDEN], True, False);
           else
             XmToggleButtonGadgetSetState(fileMgrPopup.wsPopup[BTN_SHOWHIDDEN], False, False);
         }

         /* ensure that there was fileViewData for the current directory */
         if(fileViewData != NULL)
         {
            /* attach the current fileViewData to the 'Properties' button */
           if( ! file_mgr_data->toolbox )
           {
             XtRemoveAllCallbacks(fileMgrPopup.wsPopup[BTN_PROPERTIES],
                                  XmNactivateCallback);
             XtAddCallback (fileMgrPopup.wsPopup[BTN_PROPERTIES],
                            XmNactivateCallback,
                            ShowModAttrDialog, (XtPointer) fileViewData);

             /* sensitize the 'Properties' option */
             XtSetSensitive(fileMgrPopup.wsPopup[BTN_PROPERTIES], True);
           }
            /* update the actions portion of the popup menu */
            XtManageChild(fileMgrPopup.action_separator);

            XtFree(fileMgrPopup.action_pane_file_type);
            fileMgrPopup.action_pane_file_type =
                XtNewString(fileViewData->file_data->logical_type);

            UpdateActionMenuPane ((XtPointer)fileViewData, file_mgr_rec,
                                  fileViewData->file_data->logical_type,
                                  FM_POPUP, num_of_children,
                                  fileMgrPopup.menu,
                                  fileViewData->file_data->physical_type);

            /* align actions portion of the popup menu */
            for(i=num_of_children; i<action_pane->composite.num_children; i++)
              XtSetValues(action_pane->composite.children[i], args, 1);
         }
         else
         {
           if( ! file_mgr_data->toolbox )
           {
             /* remove callback from 'Properties' button */
             XtRemoveAllCallbacks(fileMgrPopup.wsPopup[BTN_PROPERTIES],
                                  XmNactivateCallback);

             /* desensitize the 'Properties' button */
             XtSetSensitive(fileMgrPopup.wsPopup[BTN_PROPERTIES], False);
           }

            /* unmanage the action portion of the popup menu */
            XtUnmanageChild(fileMgrPopup.action_separator);

            for(i=num_of_children; i<action_pane->composite.num_children; i++)
               XtUnmanageChild(action_pane->composite.children[i]);
         }
      }
   }

   /* we are dealing with an object popup */
   else
   {
      char label[MAX_PATH];

      /* retrieve the fileViewData for the selected icon */
      if (client_data)
         fileViewData = (FileViewData *) client_data;
      else if (file_mgr_data->selected_file_count != 0)
         fileViewData = file_mgr_data->selection_list[0];


      /* unmanage the white space buttons */
      XtUnmanageChildren(fileMgrPopup.wsPopup, WS_BTNS);

      if(file_mgr_data == trashFileMgrData)
      {
          file_mgr_data->popup_menu_icon = fileViewData;

         /* Set popup menu label */
         if( file_mgr_data->selected_file_count > 1
             && InMultipleObjectRegion(file_mgr_data, fileViewData))
         {

           label_string = XmStringCreateLocalized ((GETMESSAGE(33,1, "Multiple Objects")));
         }
         else
         {
           char *tmp_label;

           if (fileViewData->file_data->action_name)
              tmp_label = fileViewData->file_data->action_name;
           else
              tmp_label = fileViewData->file_data->file_name;

           if( strlen( tmp_label ) > 20 )
             sprintf( label, "%-20.20s...", tmp_label );
           else
             sprintf( label, "%s", tmp_label );

           label_string = XmStringCreateLocalized (label);
         }
         XtSetArg (args[0], XmNlabelString, label_string);
         XtSetValues (fileMgrPopup.title, args, 1);
         XtManageChild(fileMgrPopup.title);
         XmStringFree (label_string);

         /* trash popup--unmanage the non-trash buttons, manage the trash */
         XtUnmanageChildren(fileMgrPopup.objPopup, obj_btns);
         XtManageChildren(fileMgrPopup.trash_objPopup, TRASH_OBJ_BTNS);

         /* adjust callbacks */
         XtRemoveAllCallbacks(fileMgrPopup.trash_objPopup[BTN_RESTORE],
                              XmNactivateCallback);
         XtAddCallback(fileMgrPopup.trash_objPopup[BTN_RESTORE],
                       XmNactivateCallback, Restore, (XtPointer) fileViewData);

         XtRemoveAllCallbacks(fileMgrPopup.trash_objPopup[BTN_REMOVE],
                              XmNactivateCallback);
         XtAddCallback(fileMgrPopup.trash_objPopup[BTN_REMOVE],
                       XmNactivateCallback, ConfirmRemove,
                       (XtPointer) fileViewData);

         /* unmanage the action portion of the popup menu */
         XtUnmanageChild(fileMgrPopup.action_separator);

         action_pane = (XmManagerWidget) fileMgrPopup.menu;
         for(i=num_of_children; i<action_pane->composite.num_children; i++)
            XtUnmanageChild(action_pane->composite.children[i]);
      }
      else
      {
         /* non-trash popup--manage the non-trash buttons, unmanage the trash */
         XtManageChildren(fileMgrPopup.objPopup, obj_btns);
         XtUnmanageChildren(fileMgrPopup.trash_objPopup, TRASH_OBJ_BTNS);
         if( file_mgr_data->toolbox )
           XtUnmanageChild(fileMgrPopup.objPopup[BTN_PROPERTIES]);

         if(file_mgr_data->selected_file_count > 1
            && InMultipleObjectRegion(file_mgr_data, fileViewData))
         {
            /* we have many files selected; remove callbacks from the  */
            /* 'Properties', 'Put on Desktop', and 'Review' buttons    */
            /* and attach the selection list to the remaining buttons; */
            /* desensitize the 3 buttons listed above; unmanage the    */
            /* actions part of the popup menu                          */

           /* adjust callbacks */
           if( ! file_mgr_data->toolbox )
             XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_PROPERTIES],
                                  XmNactivateCallback);

           XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_PUTON],
                                XmNactivateCallback);
           XtAddCallback(fileMgrPopup.objPopup[BTN_PUTON], XmNactivateCallback,
                         PutOnDTCB, (XtPointer) fileViewData);

           XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_TRASH],
                                XmNactivateCallback);
           XtAddCallback(fileMgrPopup.objPopup[BTN_TRASH], XmNactivateCallback,
                         TrashFiles, (XtPointer) fileViewData);

           if( file_mgr_data->toolbox && geteuid() != root_user &&
                   access(file_mgr_data->current_directory,W_OK|X_OK) != 0)
             XtSetSensitive(fileMgrPopup.objPopup[BTN_TRASH], False);


           /* sensitize buttons */
           if( !file_mgr_data->toolbox )
             XtSetSensitive(fileMgrPopup.objPopup[BTN_PROPERTIES], False);

           XtSetSensitive(fileMgrPopup.objPopup[BTN_HELP], False);

           /* Set popup menu label */
           label_string = XmStringCreateLocalized ((GETMESSAGE(33, 1, "Multiple Objects")));
           XtSetArg (args[0], XmNlabelString, label_string);
           XtSetValues (fileMgrPopup.title, args, 1);
           XtManageChild(fileMgrPopup.title);
           XmStringFree (label_string);


           /* unmanage actions */
           XtUnmanageChild(fileMgrPopup.action_separator);
           action_pane = (XmManagerWidget) fileMgrPopup.menu;
           for(i = num_of_children;
               i < action_pane->composite.num_children;
               i++)
             XtUnmanageChild(action_pane->composite.children[i]);
         }
         else
         {
           char *tmp_label;

           /* we have one file selected; attach fileViewData to buttons; */
           /* sensitize any desensitized buttons; update the actions     */
           /* part of the popup menu                                     */

           /* adjust callbacks */
           if( ! file_mgr_data->toolbox )
           {
             XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_PROPERTIES],
                                  XmNactivateCallback);
             XtAddCallback(fileMgrPopup.objPopup[BTN_PROPERTIES],
                           XmNactivateCallback, ShowModAttrDialog, (XtPointer) fileViewData);
           }

            XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_PUTON],
                                 XmNactivateCallback);
            XtAddCallback(fileMgrPopup.objPopup[BTN_PUTON],
                 XmNactivateCallback, PutOnDTCB, (XtPointer) fileViewData);

            XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_TRASH],
                                 XmNactivateCallback);
            XtAddCallback(fileMgrPopup.objPopup[BTN_TRASH],
                 XmNactivateCallback, TrashFiles, (XtPointer) fileViewData);

           if ( file_mgr_data->toolbox && geteuid() != root_user &&
                   access(file_mgr_data->current_directory,W_OK|X_OK) != 0 )
             XtSetSensitive(fileMgrPopup.objPopup[BTN_TRASH], False);
           else
             XtSetSensitive(fileMgrPopup.objPopup[BTN_TRASH], True);


            XtRemoveAllCallbacks(fileMgrPopup.objPopup[BTN_HELP],
                                 XmNactivateCallback);
            XtAddCallback(fileMgrPopup.objPopup[BTN_HELP],
                 XmNactivateCallback, ObjectHelp, (XtPointer) fileViewData->widget);

            /* sensitize buttons */
            if( ! file_mgr_data->toolbox )
              XtSetSensitive(fileMgrPopup.objPopup[BTN_PROPERTIES], True);

            XtSetSensitive(fileMgrPopup.objPopup[BTN_PUTON], True);
            XtSetSensitive(fileMgrPopup.objPopup[BTN_HELP], True);

            if (fileViewData->file_data->action_name)
               tmp_label = fileViewData->file_data->action_name;
            else
               tmp_label = fileViewData->file_data->file_name;

            if( strlen( tmp_label ) > 20 )
              sprintf( label, "%-20.20s...", tmp_label );
            else
              sprintf( label, "%s", tmp_label );

            /* Set popup menu label */
            label_string = XmStringCreateLocalized (label);
            XtSetArg (args[0], XmNlabelString, label_string);
            XtSetValues (fileMgrPopup.title, args, 1);
            XtManageChild(fileMgrPopup.title);
            XmStringFree (label_string);


            /* update actions */
            XtManageChild(fileMgrPopup.action_separator);

            XtFree(fileMgrPopup.action_pane_file_type);
            fileMgrPopup.action_pane_file_type =
                XtNewString(fileViewData->file_data->logical_type);

            UpdateActionMenuPane ((XtPointer)fileViewData, file_mgr_rec,
                                  fileViewData->file_data->logical_type,
                                  FM_POPUP, num_of_children,
                                  fileMgrPopup.menu,
                                  fileViewData->file_data->physical_type);

            /* align actions */
            action_pane = (XmManagerWidget) fileMgrPopup.menu;
            XtSetArg(args[0], XmNmarginLeft, 0);
            for(i = num_of_children; i < action_pane->composite.num_children; i++)
              XtSetValues(action_pane->composite.children[i], args, 1);
         }
      }
   }


   /* position and manage popup menu */
   if(event == NULL)
   {
      Position x, y;
      Dimension width, height;
      Dimension gWidth, gHeight;
      int displayWidth, displayHeight;
      DtIconGadget  g = (DtIconGadget)w;

      XtSetArg (args[0], XmNwidth, &width);
      XtSetArg (args[1], XmNheight, &height);
      XtGetValues(XtParent(fileMgrPopup.menu), args, 2);

      gWidth = g->icon.pixmap_width;
      gHeight = g->icon.pixmap_height;

      displayWidth = DisplayWidth(XtDisplay(w), DefaultScreen(XtDisplay(w)));
      displayHeight =DisplayHeight(XtDisplay(w), DefaultScreen(XtDisplay(w)));

      x = file_mgr_rec->shell->core.x +
          file_mgr_rec->scroll_window->core.x +
          w->core.x + gWidth/2;;

      y = file_mgr_rec->shell->core.y +
          file_mgr_rec->header_frame->core.y +
          file_mgr_rec->header_frame->core.height +
          file_mgr_rec->scroll_window->core.y +
          w->core.y + gHeight/2;;

      if((Dimension)(x + width) > (Dimension)displayWidth)
         x = displayWidth - width - 4;
      if((Dimension)(y + height) > (Dimension)displayHeight)
         y = displayHeight - height - 4;

      XtSetArg (args[0], XmNx, x);
      XtSetArg (args[1], XmNy, y);
      XtSetValues(XtParent(fileMgrPopup.menu), args, 2);
   }
   else
   {
     if(( event->type == ButtonPress || event->type == ButtonRelease) ||
        ( ( event->type == KeyPress || event->type == KeyRelease) &&
          _XmIsEventUnique(event) ) )
     {
       XmMenuPosition(fileMgrPopup.menu, (XButtonPressedEvent *)event);
     }
   }

   XtManageChild(fileMgrPopup.menu);

   if( event->type == KeyPress || event->type == KeyRelease )
   {
     /* Specify that the focus is being moved by key, NOT mouse
     */
     _XmSetInDragMode(fileMgrPopup.menu,False);

     XmProcessTraversal(fileMgrPopup.menu,XmTRAVERSE_CURRENT);

     /* This function is being called twice.
        Record this event so the above check _XmIsEventUnique will work
     */
     _XmRecordEvent(event);
   }
}


static void
DropOnRootCB (
     Widget w,
     XtPointer client_data,
     XtPointer call_data)
{
   DtDndDropCallbackStruct *fileList = (DtDndDropCallbackStruct *)call_data;
   char * ptr;
   char ** file_set = NULL;
   char ** host_set = NULL;
   FileMgrRec * file_mgr_rec;
   FileMgrData * file_mgr_data;
   char   *next;
   int i, j;
   int wsNum;
   String end;
   String tmpStr;
   int numFiles;
   int effScreenWidth;
   int effScreenHeight;
   int start_x, start_y;
   int root_x, root_y;
   int step, dstep;

   /* Check the protocol, set to failure if not the */
   /* File Transfer protocol                        */
   if (fileList->dropData->protocol != DtDND_FILENAME_TRANSFER)
     {
       fileList->status = DtDND_FAILURE;
       return;
     }


   numFiles = fileList->dropData->numItems;

   DPRINTF (("DropOnRoot: Number of files dropped are %d\n", numFiles));

   if(fileList->reason != DtCR_DND_DROP_ANIMATE)
   {
      /* set the complete move flag to False since it is not required to be called */
      /* In case when the drag is from non File manager client */
      if(!dragActive)
	initiating_view = NULL;

      fileList->completeMove = False;

      _DtSetDroppedFileInfo(fileList, &file_set, &host_set);

      if(initiating_view != NULL)
      {
         file_mgr_rec = (FileMgrRec *)
                     (((FileMgrData *) initiating_view)->file_mgr_rec);
         file_mgr_data = (FileMgrData *) initiating_view;
      }
      else
         file_mgr_data = NULL;

      /* get the correct position for the desktop icon shell */
      PositionDesktopIcon(fileList->x, fileList->y, &root_x, &root_y);

      if (numFiles > 1)
      {
         /*
          * We will place the icons by marching down a diagonal starting
          * at the drop point.  But we don't want to march off-screen
          * if there are too many icons.  So ...
          *  - We choose a step width of 20, 10, 5, or 2 depending
          *    on the number of icons dropped.
          *  - When we run into the edge of the screen, we start a new
          *    diagonal shifted one step to the right.
          *  - If all icons still won't fit, we move the starting point
          *    closer to the top left corner of the screen.
          */

         /* compute effective screen size (largest x,y where an icon
          * can be placed without going off-screen) */
         effScreenWidth = WidthOfScreen(XtScreen(w)) - (4 + 4+48+4 + 5);
         effScreenHeight = HeightOfScreen(XtScreen(w)) - (4 + 4+48+3+15+4 + 5);

         /* chosee step depending on number of icons */
         if (numFiles <= 200)
           step = 20;
         else if (numFiles <= 400)
           step = 10;
         else  if (numFiles <= 1000)
           step = 5;
         else
           step = 2;
         dstep = 40 + 4+48+4 + 5;

         /* choose starting point so at least two icons will fit */
         if (root_x <= effScreenWidth - step)
           start_x = root_x;
         else
           start_x = effScreenWidth - step;

         if (root_y <= effScreenWidth - step)
           start_y = root_y;
         else
           start_y = effScreenWidth - step;

         DPRINTF(("DropOnRootCB: numFiles %d, root_x %d, root_y %d, step %d\n",
                  numFiles, root_x, root_y, step));

         /* check if all icons will fit */
         for (;;)
         {
           /* cacluclate how many will fit (add up icons on all diagonals) */
           int n = 0, nd = 0;
           for (root_x = start_x; root_x <= effScreenWidth; root_x += dstep)
           {
             int nx = (effScreenWidth - root_x)/step + 1;
             int ny = (effScreenHeight - start_y)/step + 1;
             n += (nx <= ny)? nx: ny;
             nd++;
           }

           DPRINTF(("start_x %d, start_y %d, nd %d, n %d\n",
                    start_x, start_y, nd, n));

           /* if everything fits - great! */
           if (n >= numFiles)
             break;

           /* move the start point closer to the top left corner */
           if (effScreenWidth - start_x < effScreenHeight - start_y &&
               start_x >= step)
           {
             /* move left so more icons will fit */
             start_x -= step;
           }
           else if (effScreenWidth - start_x > effScreenHeight - start_y &&
                    start_y >= step)
           {
             /* move up so one more icon will fit in each diagonal */
             start_y -= step;
           }
           else if (start_x > 0 || start_y > 0)
           {
             /* move left and up */
             if (start_x >= step)
               start_x -= step;
             else
               start_x = 0;
             if (start_y >= step)
               start_y -= step;
             else
               start_y = 0;
           }
           else
             /* ran out of space - too bad! */
             break;
         }
         root_x = start_x;
         root_y = start_y;
      }

      /* the icon that was dropped on the root window was an
         icon that was already on the root window */
      if (file_mgr_data == NULL)
      {
         DPRINTF(("DropOnRoot: Object already on Desktop\n"));

         /* loop through the existing desktop icons to determine which
            ones are being dragged, then change their location */
         for(i=0; i < desktop_data->numIconsUsed; i++)
         {
            if((Widget)desktop_data->desktopWindows[i]->iconGadget ==
                                                   (Widget)widget_dragged)
            {
               if (DTFileIsSelected(desktop_data->desktopWindows[i],
                      desktop_data->desktopWindows[i]->file_view_data))
               {
                 for(j=0;j< desktop_data->numWorkspaces;j++)
                   if(desktop_data->workspaceData[j]->number ==
                      desktop_data->desktopWindows[i]->workspace_num)
                     break;
                 wsNum = j;
                 for (j = 0;
                      j < desktop_data->workspaceData[wsNum]->files_selected;
                      j++)
                 {
                   RelocateDesktopIcon(desktop_data->workspaceData[wsNum]->
                                       selectedDTWindows[j],
                                       root_x, root_y);
                   root_x += step;
                   root_y += step;
                   if (root_x > effScreenWidth || root_y > effScreenHeight)
                   {
                     start_x += dstep;
                     if (start_x > effScreenWidth)
                       start_x = 0;
                     root_x = start_x;
                     root_y = start_y;
                   }
                 }
               }
               else
               {
                  RelocateDesktopIcon(desktop_data->desktopWindows[i],
                                      root_x, root_y);
                  break;
               }
            }
         }
      }
      else if( file_mgr_data == trashFileMgrData )
      {
         /* if the file manager data is the trash, we want to tell the
            user that they can't drop trash on the desktop
         */
         char * msg;
         char * tmpStr;

         DPRINTF(("DropOnRoot: Attempting to Drag Trash Object to Desktop\n"));
         file_mgr_rec = (FileMgrRec *)trashFileMgrData->file_mgr_rec;
         tmpStr = (GETMESSAGE(11,37, "You can't drop files from\nthe Trash Can on to the Workspace.\nTo remove an object from Trash,\n   -Select the object, and\n   -Choose 'Put Back'\nfrom the File menu or the object's popup menu.\nYou can then drag the object out of File Manager and drop it on\nthe Workspace backdrop."));
         msg = XtNewString(tmpStr);
         FileOperationError (file_mgr_rec->main, msg, NULL);
         XtFree(msg);
         return;
      }
      else /* it was dropped on the root window so lets process it */
      {
        int EndIndex = desktop_data->numIconsUsed;

        for(i = 0; i < numFiles; i++)
        {
          ptr = strrchr(file_set[i], '/');
          if( strcmp(file_mgr_data->current_directory, file_set[i]) == 0)
          {
            SetupDesktopWindow(XtDisplay(file_mgr_rec->shell),
                               file_mgr_data, file_mgr_rec,
                               ".", host_set[i], file_set[i],
                               root_x, root_y,
                               NULL,EndIndex);
          }
          else if(strncmp(file_mgr_data->current_directory, file_set[i], strlen(file_set[i]) ) == 0 )
          {
            SetupDesktopWindow(XtDisplay(file_mgr_rec->shell),
                               file_mgr_data, file_mgr_rec,
                               "..", host_set[i],file_mgr_data->current_directory,
                               root_x, root_y,
                               NULL,EndIndex);
          }
          else
          {
            *ptr = '\0';
            if(*(file_set[i]) == 0)
            {
              SetupDesktopWindow(XtDisplay(file_mgr_rec->shell),
                                 file_mgr_data, file_mgr_rec,
                                 ptr + 1, host_set[i], "/",
                                 root_x, root_y,
                                 NULL,EndIndex);
            }
            else
            {
              SetupDesktopWindow(XtDisplay(file_mgr_rec->shell),
                                 file_mgr_data, file_mgr_rec,
                                 ptr + 1, host_set[i], file_set[i],
                                 root_x, root_y,
                                 NULL,EndIndex);
            }
          }
          root_x += step;
          root_y += step;
          if (root_x > effScreenWidth || root_y > effScreenHeight)
          {
            start_x += dstep;
            if (start_x > effScreenWidth)
              start_x = 0;
            root_x = start_x;
            root_y = start_y;
          }
        }
        initiating_view = (XtPointer)NULL;
      }

      _DtFreeDroppedFileInfo(numFiles, file_set, host_set);

      return;
   }
}

/************************************************************************
 *
 *  FreeLayoutData
 *
 ************************************************************************/

void
FreeLayoutData(XtPointer p)
{
   IconLayoutData *layout_data;

   if (p == NULL)
      return;

   layout_data = (IconLayoutData *)p;

   if (layout_data->work_id != 0)
   {
      DPRINTF(("FreeLayoutData: removing workproc\n"));
      XtRemoveWorkProc(layout_data->work_id);
      XmDropSiteEndUpdate(layout_data->drop_site_w);
   }

   XtFree((char *)layout_data->order_list);
   layout_data->order_list = NULL;
   XtFree((char *)layout_data->reuse_icons);
   layout_data->reuse_icons = NULL;
   XtFree((char *)layout_data->reuse_btns);
   layout_data->reuse_btns = NULL;
   XtFree((char *)layout_data->manage);
   layout_data->manage = NULL;

   XtFree((char *)layout_data);
   layout_data = NULL;
}

/************************************************************************
 *
 *  UnmanageFileIcons
 *      Unmanage a subset of the file icons.
 *
 ************************************************************************/

void UnmanageFileIcons(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data)
{
   XmManagerWidget file_window;
   FileViewData **order_list;
   int order_count;
   int i, n;
   Widget *unmanage;
   Widget child;
   Arg args[20];


   /*  Set the size of the file window BIG so that it does not  */
   /*  try to force positioning on its children.                */

   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   XtResizeWidget ((Widget)file_window, 32767, 32767, 0);


   /*  Set the scrolled window and file window appropriately  */
   /*  to prevent a lot of gyrations.                         */

   XtSetArg (args[0], XmNscrollBarDisplayPolicy, XmSTATIC);
   XtSetValues (file_mgr_rec->scroll_window, args, 1);

   order_list = ((IconLayoutData *)file_mgr_data->layout_data)->order_list;
   order_count = ((IconLayoutData *)file_mgr_data->layout_data)->order_count;

   unmanage = (Widget *)XtMalloc(2*order_count*sizeof(Widget));
   n = 0;
   for (i = 0; i < order_count; i++)
   {
     if (order_list[i]->filtered)
       continue;

     if ((n > 0 || order_list[i] == file_view_data) &&
         !order_list[i]->need_update)
     {
       unmanage[n++] = order_list[i]->widget;
       if (order_list[i]->treebtn)
         unmanage[n++] = order_list[i]->treebtn;
     }
   }

   /* remember which icon currently has the focus */
   if (XtIsManaged(file_mgr_rec->file_window))
   {
      /* see if the focus is inside the file window */
      child = XmGetFocusWidget(file_mgr_rec->file_window);
      if (child != NULL && XtParent(child) == file_mgr_rec->file_window)
         file_mgr_rec->focus_widget = child;
   }

   /* unmanage the selected children */
   if (n > 0)
      XtUnmanageChildren(unmanage, n);

   XtFree((char *)unmanage);
}


/************************************************************************
 *
 *  UpdateFileIcons
 *      Create or reuse a set of file icons used to get the files
 *      displayed.  This is never called for the desktop.
 *
 ************************************************************************/

void
UpdateFileIcons(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        Boolean new_directory)
{
  _UpdateFileIcons(file_mgr_rec, file_mgr_data, new_directory, NULL);
}

void
AddFileIcons(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        DirectorySet * add_dir_set)
{
  _UpdateFileIcons(file_mgr_rec, file_mgr_data, False, add_dir_set);
}


/*--------------------------------------------------------------------
 * MakeReuseList
 *
 *   Background information:
 *     After a refresh on a directory we want to reuse old icon and tree
 *   button widgets instead of destroying the old widgets and creating
 *   new ones.  Fourthermore, we want to reuse the same widgets for the
 *   same files, so that if the icon and/or label didn't change, less
 *   work needs to be done in _UpdateFileIcons().
 *     For this reason, after a refresh the GetFileData() routine copies
 *   Widgets from the old FileViewData list to the new FileViewData list
 *   for files that are both on the old and new list (i.e., files that
 *   already existed before the refresh and are still there after the
 *   refresh).  This allows _UpdateFileIcons() to reuse the old
 *   widget.
 *
 *   The purpose of MakeReuseList() is to find widgets that are no
 *   longer found in the new FileViewData list.  These are widgets
 *   from files that disappeared after the refresh (either because the
 *   file was deleted or because file is now filtered out).  These
 *   widgets can then be reused by _UpdateFileIcons()for new files that
 *   just appeared after the refresh.
 *
 *------------------------------------------------------------------*/

/* compare function for qsort and bsearch */
static int
WidgetCmp(Widget *w1, Widget *w2)
{
  return *w1 - *w2;
}

static void
MakeReuseList(
        Widget *children,
        int num_children,
        FileViewData **order_list,
        int order_count,
        Widget **reuse_icons,
        Widget **reuse_btns)
{
#ifdef DEBUG
   int n_old, n_filtered, del_icon, del_btn;
#endif
   Widget *sorted_chilren = NULL;
   Boolean *reuse = NULL;
   int icon_count;
   int btn_count;
   int i;
   Widget *p;
   Widget w;

   /* allocate widget arrays */
   *reuse_icons = (Widget *)XtMalloc((num_children + 1)*sizeof(Widget));
   *reuse_btns = (Widget *)XtMalloc((num_children + 1)*sizeof(Widget));
   icon_count = btn_count = 0;

   /* only figure things out if we already have children */
   if (num_children > 0) {
     /* create a sorted list of children */
     sorted_chilren = (Widget *)XtMalloc(num_children * sizeof(Widget));
     memcpy(sorted_chilren, children, num_children * sizeof(Widget));
     qsort(sorted_chilren, num_children, sizeof(Widget), (int (*)())WidgetCmp);

     /* create reuse flags; initially assume all children can be reused */
     reuse = (Boolean *)XtMalloc(num_children * sizeof(Boolean));
     for (i = 0; i < num_children; i++)
       reuse[i] = True;

   /* reset reuse flag for all widgets found in order_list */
#ifdef DEBUG
     n_old = n_filtered = del_icon = del_btn = 0;
#endif
     for (i = 0; i < order_count; i++)
       {
	 if (order_list[i]->filtered &&
	     strcmp(order_list[i]->file_data->file_name, ".") != 0)
	   {
	     /* don't reuse this widget later */
#ifdef DEBUG
	     n_filtered++;
	     if (order_list[i]->widget)
	       del_icon++;
	     if (order_list[i]->treebtn)
	       del_btn++;
#endif
	     order_list[i]->widget =
	       order_list[i]->treebtn = NULL;
	   }
	 else
	   {
	     if (order_list[i]->widget)
	       {
		 p = bsearch(&order_list[i]->widget,
			     sorted_chilren, num_children, sizeof(Widget),
			     (int (*)())WidgetCmp);
		 if (p)
		   {
		     /* don't reuse this widget for any other file */
		     reuse[p - sorted_chilren] = False;
#ifdef DEBUG
		     n_old++;
#endif
		   }
		 else
		   {
		     /* don't reuse this widget later */
		     order_list[i]->widget = NULL;
#ifdef DEBUG
		     del_icon++;
#endif
		   }
	       }

	     if (order_list[i]->treebtn)
	       {
		 p = bsearch(&order_list[i]->treebtn,
			     sorted_chilren, num_children, sizeof(Widget),
			     (int (*)())WidgetCmp);
		 if (p)
		   {
		     /* don't reuse this widget for any other file */
		     reuse[p - sorted_chilren] = False;
		   }
		 else
		   {
		     /* don't reuse this widget later */
		     order_list[i]->treebtn = NULL;
#ifdef DEBUG
		     del_btn++;
#endif
		   }
	       }
	   }
       }

     /* copy reusable widgets into widget arrays */
     for (i = 0; i < num_children; i++)
       {
	 if (reuse[i])
	   {
	     /* this widget can be reused for new files */
	     w = sorted_chilren[i];
	     if (XtClass(w) == dtIconGadgetClass)
	       {
#ifndef DELAYED_UNREGISTER
		 Arg args[1];

		 XtSetArg (args[0], XmNdropSiteOperations, XmDROP_NOOP);
		 XtRemoveAllCallbacks(w, XmNdropCallback);
		 XtSetValues (w, args, 1);
#endif
		 (*reuse_icons)[icon_count++] = w;
	       }
	     else if (XtClass(w) == xmPushButtonGadgetClass)
	       (*reuse_btns)[btn_count++] = w;
	   }
       }
   }

   /* null-terminate the arrays */
   (*reuse_icons)[icon_count] = NULL;
   (*reuse_btns)[btn_count] = NULL;

   /* free storage */
   XtFree((char *)sorted_chilren);
   XtFree((char *)reuse);

   DPRINTF(("MakeReuseList: count %d (%d new, %d old, %d filtered)\n",
            order_count, order_count - n_filtered - n_old, n_old, n_filtered));
   DPRINTF(("               reuse %d + %d, del %d + %d\n",
            icon_count, btn_count, del_icon, del_btn));
}


/*--------------------------------------------------------------------
 * UpdateOneIconLabel
 *------------------------------------------------------------------*/

static void
UpdateOneIconLabel(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data)
{
   char *label;
   char *s;

   /*  Get the label and icon to be used for the widget  */
   if (file_mgr_data->view != BY_ATTRIBUTES)
   {
      if (strcmp(file_view_data->file_data->file_name, "..") == 0)
      {
         /* label = ".. (go up)" */
         s = GetSharedMessage(UP_ONE_LEVEL_LABEL);
         label = (char *)XtMalloc(2 + strlen(s) + 1);
         strcpy(label, "..");
         strcat(label, s);
      }
      else if (file_mgr_data->view == BY_NAME &&
               file_view_data->file_data->physical_type == DtDIRECTORY &&
               file_mgr_data->show_type != MULTIPLE_DIRECTORY)
      {
         /* label = "name/" */
         label = (char *)XtMalloc(
                             strlen(file_view_data->file_data->file_name) + 2);
         strcpy(label, file_view_data->file_data->file_name);
         strcat(label, "/");
      }
      else if (file_mgr_data->view == BY_NAME &&
               file_view_data->file_data->physical_type == DtEXECUTABLE)
      {
         /* label = "name*" */
         label = (char *)XtMalloc(
                             strlen(file_view_data->file_data->file_name) + 2);
         strcpy(label, file_view_data->file_data->file_name);
         strcat(label, "*");
      }
      else if(file_view_data->file_data->action_name != NULL)
         /* label = action name */
         label = XtNewString(file_view_data->file_data->action_name);
      else
         /* label = file name */
         label = XtNewString(file_view_data->file_data->file_name);
   }
   else /* file_mgr_data->view == BY_ATTRIBUTES */
   {
      /* label = file name + attributes */
      label = GetLongName(file_view_data->file_data);
      if (strcmp(file_view_data->file_data->file_name, "..") == 0)
      {
         s = GetSharedMessage(UP_ONE_LEVEL_LABEL);
         label = (char *)XtRealloc(label, strlen(label) + strlen(s) + 1);
         strcat(label, s);
      }
   }

   /* store new label */
   XtFree(file_view_data->label);
   file_view_data->label = label;
}


/*--------------------------------------------------------------------
 * UpdateOneFileIcon
 *------------------------------------------------------------------*/

static void
UpdateOneFileIcon(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data)
{
   XmString icon_label;
   char *logical_type;
   PixmapData *pixmapData;
   Widget icon_widget;
   Widget btn_widget;
   Boolean is_instance_icon;
   Boolean instance_icon_changed;
   Arg args[35];
   int n_color_args;
   int argi_imageName;
   int n;

   XmManagerWidget file_window = (XmManagerWidget) file_mgr_rec->file_window;
   DirectorySet *directory_set = (DirectorySet *)file_view_data->directory_set;
   IconLayoutData *layout_data = (IconLayoutData *)file_mgr_data->layout_data;

   /* Get the label and icon to be used for the widget */
   if (!file_view_data->label)
     UpdateOneIconLabel(file_mgr_data, file_view_data);

   icon_label = XmStringCreateLocalized(file_view_data->label);

   /*  Get the icon name based on the file type  */

   logical_type = file_view_data->file_data->logical_type;

   if (file_mgr_data->view == BY_NAME)
      pixmapData = NULL;
   else if (openDirType == NEW &&
               file_view_data->file_data->physical_type == DtDIRECTORY)
   {
      pixmapData = CheckForOpenDirectory(file_view_data,
                                         directory_set,
                                         file_mgr_data,
                                         logical_type);
   }
   else
   {
      if (file_mgr_data->view == BY_NAME_AND_ICON)
         pixmapData = _DtRetrievePixmapData(
                         logical_type,
                         file_view_data->file_data->file_name,
                         directory_set->name,
                         (Widget) file_window,
                         LARGE);
      else
         pixmapData = _DtRetrievePixmapData(
                         logical_type,
                         file_view_data->file_data->file_name,
                         directory_set->name,
                         (Widget) file_window,
                         SMALL);
   }

   /* check if this is an instance icon */
   is_instance_icon = False;
   if (pixmapData != NULL)
   {
     char tmp[1024];

     strcpy(tmp, directory_set->name);
     strcat(tmp, "/");
     strcat(tmp, file_view_data->file_data->file_name);
     if (strcmp(pixmapData->iconFileName, tmp) == 0)
        is_instance_icon = True;
   }

   /* check if instance icon was modified */
   instance_icon_changed = False;
   if (is_instance_icon)
   {
     if (file_view_data->icon_mtime != file_view_data->file_data->stat.st_mtime)
     {
        if (file_view_data->icon_mtime != 0)
           instance_icon_changed = True;
        file_view_data->icon_mtime = file_view_data->file_data->stat.st_mtime;
     }
   }
   else
     file_view_data->icon_mtime = 0;

   /* Build the arg list for color resources.  */
   n = 0;
   XtSetArg (args[n], XmNarmColor, white_pixel);                     n++;

   if (layout_data->background == white_pixel)
   {
      if (file_view_data->selected)
      {
         XtSetArg (args[n], XmNbackground, black_pixel);             n++;
         XtSetArg (args[n], XmNforeground, white_pixel);             n++;
      }
      else
      {
         XtSetArg (args[n], XmNbackground, white_pixel);             n++;
         XtSetArg (args[n], XmNforeground, layout_data->foreground); n++;
      }
      XtSetArg (args[n], XmNpixmapBackground, white_pixel);          n++;
      XtSetArg (args[n], XmNpixmapForeground, black_pixel);          n++;
   }
   else if (layout_data->background == black_pixel)
   {
      if (file_view_data->selected)
      {
         XtSetArg (args[n], XmNbackground, white_pixel);             n++;
         XtSetArg (args[n], XmNforeground, black_pixel);             n++;
      }
      else
      {
         XtSetArg (args[n], XmNbackground, black_pixel);             n++;
         XtSetArg (args[n], XmNforeground, layout_data->foreground); n++;
      }
      XtSetArg (args[n], XmNpixmapBackground, white_pixel);          n++;
      XtSetArg (args[n], XmNpixmapForeground, black_pixel);          n++;
   }
   else
   {
      if (file_view_data->selected)
      {
         XtSetArg (args[n], XmNbackground, white_pixel);             n++;
         XtSetArg (args[n], XmNforeground, black_pixel);             n++;
         XtSetArg (args[n], XmNpixmapBackground, white_pixel);       n++;
         XtSetArg (args[n], XmNpixmapForeground, black_pixel);       n++;
      }
      else
      {
         XtSetArg (args[n], XmNbackground, layout_data->background); n++;
         XtSetArg (args[n], XmNforeground, layout_data->foreground); n++;
         XtSetArg (args[n], XmNpixmapBackground, layout_data->pixmap_back); n++;
         XtSetArg (args[n], XmNpixmapForeground, layout_data->pixmap_fore); n++;
      }
   }

   n_color_args = n;

   /* Build the rest of the arg list and either create or reuse the widget. */

   XtSetArg (args[n], XmNstring, icon_label);                        n++;
   argi_imageName = n;
   if (pixmapData)
     XtSetArg (args[n], XmNimageName, pixmapData->iconFileName);
   else
     XtSetArg (args[n], XmNimageName, NULL);
   n++;
   XtSetArg (args[n], XmNmaxPixmapWidth, layout_data->pixmap_width);   n++;
   XtSetArg (args[n], XmNmaxPixmapHeight, layout_data->pixmap_height); n++;
   XtSetArg (args[n], XmNuserData, directory_set);                     n++;
   XtSetArg (args[n], XmNunderline, False);                            n++;
   XtSetArg (args[n], XmNfillMode, XmFILL_TRANSPARENT);                n++;
   if (file_mgr_data->view == BY_NAME_AND_ICON &&
       file_mgr_data->show_type != MULTIPLE_DIRECTORY)
      XtSetArg (args[n], XmNpixmapPosition, XmPIXMAP_TOP);
   else
      XtSetArg (args[n], XmNpixmapPosition, XmPIXMAP_LEFT);            n++;

   /* See if we can re-use the same or some other icon gadget */
   if (file_view_data->widget)
      icon_widget = file_view_data->widget;
   else if (*layout_data->next_icon_to_use)
      icon_widget = *layout_data->next_icon_to_use++;
   else
      icon_widget = NULL;

   /* See if we found an available icon gadget */
   if (icon_widget)
   {
      /* reuse the icon gadget */
      if (icon_widget != file_view_data->widget || file_mgr_data->newSize)
      {
         XtSetArg (args[n], XmNdropSiteOperations, XmDROP_NOOP);n++;
         XtRemoveAllCallbacks(icon_widget, XmNdropCallback);
         file_view_data->registered = False;
      }
      XtRemoveAllCallbacks (icon_widget, XmNcallback);

      /* if instance_icon_changed, force destroy of old pixmap */
      if (instance_icon_changed)
         XtSetArg (args[argi_imageName], XmNimageName, NULL);

      /*
       * Move the gadget off the visible area; this avoids unnecessary
       * redraw events at the old position when the gadget is moved to
       * the correct position once it is determined in LayoutFileIcons.
       */
      icon_widget->core.x = -999;
      icon_widget->core.y = -999;
      XtSetValues (icon_widget, args, n);

      if (instance_icon_changed && pixmapData)
      {
         XtSetArg (args[0], XmNimageName, pixmapData->iconFileName);
         XtSetValues (icon_widget, args, 1);
      }
   }
   else
   {
      /* create a new or duplicate an existing widget */
      XtSetArg (args[n], XmNshadowThickness, 2);             n++;
      XtSetArg (args[n], XmNdropSiteOperations, XmDROP_NOOP);n++;
      XtSetArg (args[n], XmNfontList, user_font);            n++;
      if( keybdFocusPolicy == XmEXPLICIT)
      {
         XtSetArg (args[n], XmNtraversalOn, True);           n++;
      }
      else
      {
         XtSetArg (args[n], XmNtraversalOn, False);          n++;
         XtSetArg (args[n], XmNhighlightThickness, 0);       n++;
      }
      XtSetArg (args[n], XmNborderType, DtNON_RECTANGLE);   n++;

      if (layout_data->dup_icon_widget == NULL)
      {
#ifdef HARDCODED_ICON_MARGINS
         XtSetArg (args[n], XmNmarginWidth, 0);  n++;
         XtSetArg (args[n], XmNmarginHeight, 0); n++;
#endif
         XtSetArg (args[n], XmNx, -999);  n++;
         XtSetArg (args[n], XmNy, -999);  n++;
         icon_widget = layout_data->dup_icon_widget =
                   _DtCreateIcon ((Widget)file_window, "icon", args, n);
      }
      else
      {
         DtIconGadget g;
         int i = n_color_args;

         icon_widget = _DtDuplicateIcon ((Widget)file_window,
                       layout_data->dup_icon_widget,
                       icon_label,
                       (pixmapData? pixmapData->iconFileName: NULL),
                       (XtPointer)directory_set, /* userData */
                       False);                   /* underline */
         g = (DtIconGadget)icon_widget;
         g->gadget.highlighted = False;
         g->gadget.highlight_drawn = False;

         /*
          * Move the gadget off the visible area; this avoids unnecessary
          * redraw events at the old position when the gadget is moved to
          * the correct position once it is determined in LayoutFileIcons.
          */
         icon_widget->core.x = -999;
         icon_widget->core.y = -999;

         /* make sure colors, drop operations, and clipping are right */
         XtSetArg(args[i], XmNdropSiteOperations, XmDROP_NOOP);             i++;
         XtSetArg(args[i], XmNmaxPixmapWidth, layout_data->pixmap_width);   i++;
         XtSetArg(args[i], XmNmaxPixmapHeight, layout_data->pixmap_height); i++;
         XtSetValues (icon_widget, args, i);
      }
      XtAddCallback(icon_widget, XmNhelpCallback,
                    (XtCallbackProc)HelpRequestCB, NULL);
      file_view_data->registered = False;
   }

   if (file_mgr_data->view != BY_NAME)
      _DtCheckAndFreePixmapData(logical_type,
                                (Widget) file_window,
                                (DtIconGadget) icon_widget,
                                pixmapData);

#ifdef _SHOW_LINK
   if (file_view_data->file_data->link != 0)
   {
      XtSetArg (args[0], XmNforeground, layout_data->topshadow);
      XtSetValues (icon_widget, args, 1);
   }
#endif

   /*
    * If viewing by attributes, adjust spacing between the icon pixmap and
    * the file name so that all file names are aligned.
    */
   if (file_mgr_data->view != BY_NAME_AND_ICON ||
       file_mgr_data->show_type == MULTIPLE_DIRECTORY)
   {
     Dimension pixmap_width = ((DtIconGadget)icon_widget)->icon.pixmap_width;

     if (pixmap_width < layout_data->pixmap_width)
     {
        XtSetArg (args[0], XmNspacing,
              layout_data->spacing + layout_data->pixmap_width - pixmap_width);
        XtSetValues (icon_widget, args, 1);
     }
   }


   file_view_data->file_data->is_broken = False;
   file_view_data->widget = icon_widget;
   XtAddCallback (icon_widget, XmNcallback, (XtCallbackProc)IconCallback,
                  file_view_data);

   XmStringFree (icon_label);

   /* Check if we need a button for tree branch expand */

   if (file_mgr_data->show_type != MULTIPLE_DIRECTORY ||
       !file_view_data->file_data->is_subdir)
   {
     /* no tree branch expand button needed */
     file_view_data->treebtn = NULL;
   }
   else
   {
     /* create a tree branch expand button */
     Pixmap px = GetTreebtnPixmap(file_mgr_data, file_view_data);

     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP);                 n++;
     XtSetArg(args[n], XmNlabelPixmap, px);                     n++;
     XtSetArg(args[n], XmNbackground, layout_data->background); n++;
     XtSetArg(args[n], XmNtraversalOn, False);                  n++;
     XtSetArg(args[n], XmNhighlightThickness, 0);               n++;
     XtSetArg(args[n], XmNshadowThickness, 0);                  n++;
     XtSetArg(args[n], XmNmarginWidth, 0);                      n++;
     XtSetArg(args[n], XmNmarginHeight, 0);                     n++;
     XtSetArg(args[n], XmNuserData, file_mgr_data);             n++;
     XtSetArg(args[n], XmNx, -999);                             n++;
     XtSetArg(args[n], XmNy, -999);                             n++;

     /* See if we can re-use the same or some other button gadget */
     if (file_view_data->treebtn)
        btn_widget = file_view_data->treebtn;
     else if (*layout_data->next_btn_to_use)
        btn_widget = *(layout_data->next_btn_to_use)++;
     else
        btn_widget = NULL;

     /* See if we found an available button gadget */
     if (btn_widget) {
        XtRemoveAllCallbacks (btn_widget, XmNactivateCallback);
        XtSetValues (btn_widget, args, n);
     }
     else
     {
       btn_widget = XmCreatePushButtonGadget((Widget)file_window,
                                             "tree_button", args, n);
     }
     XtAddCallback(btn_widget, XmNactivateCallback,
                    (XtCallbackProc)TreeBtnCallback, file_view_data);

     file_view_data->treebtn = btn_widget;
   }

   /* this entry is now up-to-date */
   file_view_data->need_update = False;
}


/*--------------------------------------------------------------------
 * _UpdateFileIcons
 *------------------------------------------------------------------*/

static void
_UpdateFileIcons(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        Boolean new_directory,
        DirectorySet * add_dir_set)
{
   XmManagerWidget file_window;
   FileViewData **order_list;
   int order_count;
   Widget child;
   Arg args[5];
   IconLayoutData *layout_data;
   int i;

#ifdef DT_PERFORMANCE
   struct timeval update_time_s;
   struct timeval update_time_f;
#endif

   DPRINTF(("_UpdateFileIcons(\"%s\", new_dir %c, add_dir_set %p) ...\n",
            file_mgr_data->current_directory,
            new_directory? 'T': 'F', add_dir_set));

   /*  Set the size of the file window BIG so that it does not  */
   /*  try to force positioning on its children.                */
   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   XtResizeWidget ((Widget)file_window, 32767, 32767, 0);

   /*  Set the scrolled window and file window appropriately  */
   /*  to prevent a lot of gyrations.                         */

   XtSetArg (args[0], XmNscrollBarDisplayPolicy, XmSTATIC);
   XtSetValues (file_mgr_rec->scroll_window, args, 1);

   /* For faster updates, unmanage all the icons */
   if (XtIsManaged(file_mgr_rec->file_window))
   {
      /* see if the focus is inside the file window */
      child = XmGetFocusWidget(file_mgr_rec->file_window);
      if (child != NULL)
      {
        if (new_directory)
        {
          file_mgr_rec->focus_widget = file_mgr_rec->file_window;
        }
        else if( XtParent(child) == file_mgr_rec->file_window )
        {
          /* remember which widget had the focus */
          file_mgr_rec->focus_widget = child;
        }
      }

      DPRINTF(("  focus_widget = %p (%s)\n",
               file_mgr_rec->focus_widget,
               file_mgr_rec->focus_widget?
                  XtName(file_mgr_rec->focus_widget): "nil"));
      XtUnmanageChild(file_mgr_rec->file_window);
   }

   XtUnmanageChildren(file_window->composite.children,
                      file_window->composite.num_children);

   /* if this is a new directory, scroll to the top */
   if (new_directory)
   {
      XtSetArg (args[0], XmNx, 0);
      XtSetArg (args[1], XmNy, 0);
      XtSetValues ((Widget)file_window, args, 2);
   }

   /*
    * Don't leave the view in a munged state for too long.
    * Only do if we are not creating a new view.  This is because the
    * view is not yet in the 'view_list', and so the redisplay code
    * may not use the correct redisplay function.
    */
   if (ReturnDesktopPtr(file_mgr_rec->file_window))
   {
      UpdateHeaders(file_mgr_rec, file_mgr_data, False);
      XFlush (XtDisplay (file_window));
      XmUpdateDisplay ((Widget)file_window);
   }

   /* free any old layout data */
   FreeLayoutData(file_mgr_data->layout_data);
   file_mgr_data->layout_data = NULL;

   /* if directory-read still in progress, don't do anything more now */
   if (file_mgr_data->busy_status != not_busy)
   {
      DPRINTF(("done (busy)\n"));
      return;
   }

#ifdef DT_PERFORMANCE
   printf("   Beginning UpdateFileIcons\n");
   gettimeofday(&update_time_s, NULL);

   /* added by Rafi */
   _DtPerfChkpntMsgSend("Begin Update Icons");

#endif

   /* set up new layout data */
   layout_data = (IconLayoutData *)XtCalloc(1, sizeof(IconLayoutData));
   file_mgr_data->layout_data = (XtPointer)layout_data;

   FlattenTree(file_mgr_data, &order_list, &order_count);
   layout_data->order_list = order_list;
   layout_data->order_count = order_count;

   MakeReuseList(file_window->composite.children,
                 file_window->composite.num_children,
                 order_list, order_count,
                 &layout_data->reuse_icons,
                 &layout_data->reuse_btns);
   layout_data->next_icon_to_use = layout_data->reuse_icons;
   layout_data->next_btn_to_use = layout_data->reuse_btns;

   layout_data->manage = (Widget *)XtMalloc(2*order_count*sizeof(Widget));
   layout_data->manage_count = 0;

   layout_data->i_do_next_vis = 0;
   layout_data->i_do_next_all = 0;

   /*
    * Iterate through the file list and mark all entries to be in
    * need of update.  We also construct icon labels at this time,
    * since they are need by LayoutFileIcons to estimate icon gadget
    * sizes.
    */

   if (add_dir_set)
   {
     /* only need to iterate throught the new entries */;
     order_list = add_dir_set->order_list;
     order_count = add_dir_set->file_count;
   }

   for (i = 0; i < order_count; i++)
   {
      order_list[i]->need_update = True;
      order_list[i]->selected = False;
      UpdateOneIconLabel(file_mgr_data, order_list[i]);
   }

   /* set selected flag on all files in selection_list */
   for (i = 0; i < file_mgr_data->selected_file_count; i++)
      file_mgr_data->selection_list[i]->selected = True;

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done UpdateFileIcons, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s.tv_usec);

   /* added by Rafi */
   _DtPerfChkpntMsgSend("Done  Update Icons");
#endif

   DPRINTF(("done\n"));
}




/************************************************************************
 *
 *  CreateTreeIcons
 *      Create icons for tree-branch-expand buttons.
 *
 ************************************************************************/

static void
CreateTreeIcons(Widget w)
{
  Arg args[20];
  Pixel  background_color = 0;
  Pixel  foreground_color = 0;
  int i, j;
  TreePxId px;
  char pxname[128];
  unsigned int width, height, dummy;

  XtSetArg (args[0], XmNbackground, &background_color);
  XtSetArg (args[1], XmNforeground, &foreground_color);
  XtGetValues (w, args, 2);

  for (i = 0; i < 3; i++)
  {
    TreeBtnWd[i] = 5;
    TreeBtnHt[i] = 5;
    for (j = 0; j < tpxN; j++)
    {
      strcpy(pxname, TreePxTab[j].name);
      strcat(pxname, TreePxSuffix[i]);
      TreePxTab[j].px[i] = _DtGetPixmap(XtScreen(w), pxname,
                                     foreground_color, background_color);
      width = height = 0;
      XGetGeometry(XtDisplay(w), TreePxTab[j].px[i],
                   (Window *) &dummy,               /* returned root window */
                   (int *) &dummy, (int *) &dummy,  /* x, y of pixmap */
                   &width, &height,                 /* pixmap width, height */
                   &dummy, &dummy);                 /* border width, depth */
      if (j == tpxNil)
      {
         TreeNilWd[i] = width;
         TreeNilHt[i] = height;
      }
      else
      {
         if (width > TreeBtnWd[i])
           TreeBtnWd[i] = width;
         if (height > TreeBtnHt[i])
           TreeBtnHt[i] = height;
      }
    }
  }
}


/************************************************************************
 *
 *  GetTreebtnPixmap
 *      Get icon for tree-branch-expand buttons.
 *
 ************************************************************************/

Pixmap GetTreebtnPixmap(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data)
{
   TreePxId pxid;

   /* if not yet done, create tree button icons */
   /* @@@ do this earlier from main? */
   if (TreeBtnWd[0] == 0)
     CreateTreeIcons(((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window);

   if (file_view_data->ts == tsNotRead)
     pxid = tpxNotRead;
   else if (file_view_data->ts == tsError)
     pxid = tpxError;
   else if (file_view_data->ndir == 0 &&
            (file_view_data->nfile == 0 ||
             file_mgr_data->tree_files == TREE_FILES_NEVER)
             && (file_view_data->ts == tsNone || !showEmptySet))
     pxid = tpxEmpty;
   else if (file_view_data->ts == tsNone)
     pxid = tpxMore;
   else if (file_view_data->ts == tsDirs)
     pxid = (file_view_data->nfile == 0 ||
             file_mgr_data->tree_files == TREE_FILES_NEVER)? tpxLess:
            (file_view_data->ndir == 0)?  tpxMore:
                                          tpxBoth;
   else if (file_view_data->ts == tsAll)
     pxid = tpxLess;

   if (file_mgr_data->view == BY_NAME)
     return TreePxTab[pxid].px[0];   /* small pixmap */
   else if (file_mgr_data->view == BY_NAME_AND_ICON)
     return TreePxTab[pxid].px[2];   /* large pixmap */
   else
     return TreePxTab[pxid].px[1];   /* medium  pixmap */
}


/************************************************************************
 *
 *  GetIconSize
 *      Compute maximum icon size.
 *
 ************************************************************************/

static void
GetIconLayoutParms(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        IconLayoutData *ld)
{
   DirectorySet *directory_set;
   int file_count;
   FileViewData **file_list;
   int i;
   DtIconGadget g;
   Arg args[10];
   Dimension shadowThickness;
   Dimension marginWidth;
   Dimension maxWidth = ld->pixmap_width;
   Dimension gadgetWidth;

   /* determine pixmap size */
   if (file_mgr_data->view == BY_NAME)
   {
      /* no pixmap */
      ld->pixmap_width = 0;
      ld->pixmap_height = 0;
   }
   else if (file_mgr_data->view == BY_NAME_AND_ICON)
   {
      /* large pixmap */
      ld->pixmap_width = largeIconWidth;
      ld->pixmap_height = largeIconHeight;
   }
   else
   {
      /* small pixmap */
      ld->pixmap_width = smallIconWidth;
      ld->pixmap_height = smallIconHeight;
   }

   /* find the icon gadget for "." */
   directory_set = file_mgr_data->directory_set[0];
   file_count = directory_set->file_count;
   file_list = directory_set->file_view_data;

   g = NULL;
   for (i = 0; i < file_count; i++)
   {
      if (strcmp(file_list[i]->file_data->file_name, ".") == 0)
      {
         g = (DtIconGadget)file_list[i]->widget;
         break;
      }
   }

   /* get layout parameters from "." */
   if (g)
   {
      XtSetArg(args[0], XmNhighlightThickness, &ld->highlight);
      XtSetArg(args[1], XmNshadowThickness, &shadowThickness);
      XtSetArg(args[2], XmNmarginWidth, &marginWidth);
      XtSetArg(args[3], XmNspacing, &ld->spacing);
      XtSetArg(args[4], XmNalignment, &ld->alignment);
      XtSetArg(args[5], XmNpixmapPosition, &ld->pixmap_position);
      XtGetValues((Widget)g, args, 6);

      if (g->icon.pixmap_width < maxWidth)
         ld->spacing = ld->spacing - (maxWidth - g->icon.pixmap_width);

      ld->width = ((Widget)g)->core.width +
                                     ld->pixmap_width  - g->icon.pixmap_width;
      ld->height = ((Widget)g)->core.height +
                                     ld->pixmap_height - g->icon.pixmap_height;

      ld->char_width = (g->icon.string_width)/strlen(file_list[i]->label);
      if (ld->pixmap_position != XmPIXMAP_TOP)
         ld->width -= g->icon.string_width;
      ld->margin = ld->highlight + shadowThickness + marginWidth;
   }
   else
   {
      /* No icon gadget for "." found: strange!  Guess some defaults.  */
      ld->char_width = 8;
      ld->margin = 2 + 1 + 1;
      ld->spacing = 2;
      ld->highlight = (keybdFocusPolicy == XmEXPLICIT)? 2: 0;
      ld->alignment = XmALIGNMENT_END;
      if (file_mgr_data->view == BY_NAME_AND_ICON &&
          file_mgr_data->show_type != MULTIPLE_DIRECTORY)
      {
         ld->pixmap_position = XmPIXMAP_TOP;
         ld->width = 2*ld->margin + ld->pixmap_width;
         ld->height = 2*ld->margin + ld->pixmap_height + ld->spacing + 14;
      }
      else
      {
         ld->pixmap_position = XmPIXMAP_LEFT;
         ld->width = 2*ld->margin + ld->pixmap_width +
                                                 ld->spacing + ld->char_width;
         ld->height = 2*ld->margin + ld->pixmap_height;
      }
   }

   /* determine which size tree buttons to use */
   if (file_mgr_data->show_type != MULTIPLE_DIRECTORY)
      ld->treebtn_size = 0;   /* no tree buttons needed */
   else if (file_mgr_data->view == BY_NAME)
      ld->treebtn_size = 0;   /* small size */
   else if (file_mgr_data->view == BY_NAME_AND_ICON)
      ld->treebtn_size = 2;   /* large size */
   else
      ld->treebtn_size = 1;   /* medium size */
}


static void
EstimateIconSize(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        IconLayoutData *layout_data,
        FileViewData *file_view_data,
        Dimension *width,
        Dimension *height)
{
   int label_len;
   int label_width;

   if (file_view_data == NULL)
      label_len = 1;
   else if (file_view_data->label == NULL)
      label_len = strlen(file_view_data->file_data->file_name);
   else
      label_len = strlen(file_view_data->label);

   if (layout_data->pixmap_position == XmPIXMAP_TOP)
   {
      label_width = 2*layout_data->margin + label_len*layout_data->char_width;
      if ((Dimension)label_width > layout_data->width)
         *width = label_width;
      else
         *width = layout_data->width;
   }
   else
        *width = layout_data->width + (label_len) * layout_data->char_width;
   *height = layout_data->height;
}


/************************************************************************
 *
 *  GetExtraHeight
 *    Compute extra height for drawing nil symbol for empty tree branches.
 *
 ************************************************************************/

static Dimension
GetExtraHeight(
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data,
        int treebtn_size)
{
   if (showEmptySet &&
       file_view_data->file_data->is_subdir &&
       file_view_data->ts >= tsDirs &&
       file_view_data->ndir == 0 &&
       (file_view_data->nfile == 0 ||
        file_mgr_data->tree_files == TREE_FILES_NEVER))
   {
      return YSPACING(file_mgr_data) + TreeNilHt[treebtn_size];
   }
   else
      return 0;
}


/************************************************************************
 *
 *  EraseTreeLines
 *    Erase connecting lines in tree mode from a specified icon widget
 *    on down; called when a tree branch is expanded or collapsed to
 *    erase previous tree lines before drawing new icons and tree lines.
 *
 ************************************************************************/

void
EraseTreeLines(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data)
{
   XmManagerWidget file_window;
   Dimension fw_width, fw_height;
   Position x, y;

   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   if (!XtIsManaged((Widget)file_window))
      return;

   /* get upper left corner of grid space for the icon widget */
   x = file_view_data->x;
   y = file_view_data->y - file_mgr_data->grid_height;

   /*  Get file window width and height */
   fw_width = file_window->core.width;
   fw_height = file_window->core.height;

   DPRINTF2(("EraseTreeLines(\"%s\"): x/y %d/%d (widget %d/%d)\n",
             file_view_data->file_data->file_name, x, y,
             file_view_data->widget->core.x,
             file_view_data->widget->core.y));

   /* clear area from icon widget to bottom of file window */
   XClearArea(XtDisplay(file_window), XtWindow(file_window),
              x, y, fw_width - x, fw_height - y, False);

   /*
    * clear area to the right and above the icon widget
    * (necessary if there are multiple columns, which happens if
    *  there are more icons than fit into a single column of
    *  maximum height 32767)
    */
   if (y > 0  &&  (Dimension)(x + file_mgr_data->grid_width) < fw_width)
   {
     x += file_mgr_data->grid_width;
     XClearArea(XtDisplay(file_window), XtWindow(file_window),
                x, 0, fw_width - x, y, False);
   }
}


/************************************************************************
 *
 *  RedrawTreeLines
 *      Redraw connecting lines in tree mode.
 *
 ************************************************************************/

void
RedrawTreeLines(
        Widget w,
        int ex, int ey, int ewidth, int eheight, int ecount,
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data)
{
   static char *empty_msg = NULL;

   FileViewData *file_view_data;
   IconLayoutData *layout_data;
   Dimension grid_width, grid_height;
   Dimension extra_height, e_height;
   int sz;
   FileViewData **order_list;
   int order_count;
   GC solid_gc, dash_gc;
   int i, k;
   Position x, xl;
   Position y, y0, y1;
   int level;
   Bool more[256];
   XFontSetExtents *extents;
   int font_height;
   int font_yoffset;
   int tmp;

   if (!XtIsManaged(w))
      return;

   /*  get layout parameters  */
   layout_data = (IconLayoutData *)file_mgr_data->layout_data;
   order_list = layout_data->order_list;
   order_count = layout_data->order_count;
   grid_width = file_mgr_data->grid_width;
   grid_height = file_mgr_data->grid_height;
   sz = layout_data->treebtn_size;

   DPRINTF2(("RedrawTreeLines(x %d, y %d, wd %d, ht %d, count %d)\n",
             ex, ey, ewidth, eheight, ecount));

   if (grid_width == 0 || grid_height == 0)
      /* layout probably not yet done */
      return;

   /* if not yet done, create tree button icons */
   /* @@@ do this earlier from main? */
   if (TreeBtnWd[0] == 0)
     CreateTreeIcons(((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window);

   /* select line styles */
   if (file_mgr_data->view == BY_NAME) {
     solid_gc = file_mgr_data->tree_solid_thin_gc;
     dash_gc = file_mgr_data->tree_dash_thin_gc;
   } else {
     solid_gc = file_mgr_data->tree_solid_thick_gc;
     dash_gc = file_mgr_data->tree_dash_thick_gc;
   }

   x = MARGIN;
   y = MARGIN;

   for (k = 0; k < order_count; k++)
   {
      if (!order_list[k]->displayed)
         continue;

      /* determine the height of this item */
      file_view_data = order_list[k];
      extra_height = GetExtraHeight(file_mgr_data, file_view_data, sz);
      GetLevel(file_view_data, &level);

      /* check if we need to go to the next column */
      tmp = (int)y + grid_height + extra_height + YSPACING(file_mgr_data);
      if (tmp + MARGIN > MAXWINSIZE)
      {
        /* window would exceed height limit; go to the next column */
        x += grid_width + XSPACING;
        y = MARGIN;
        tmp = (int)y + grid_height + extra_height + YSPACING(file_mgr_data);
      }

      /* check if current item intersects the exposed region */
      y0 = y - YSPACING(file_mgr_data)
             - grid_height
             + (Dimension)(grid_height - TreeBtnHt[sz])/(Dimension)2
             + TreeBtnHt[sz];
      y1 = y + grid_height + extra_height + YSPACING(file_mgr_data);
      if (x  <= ex + ewidth  && x + TreeWd(level, sz) > ex &&
          y0 <= ey + eheight && y1 > ey)
      {
         GetAncestorInfo(file_mgr_data, file_view_data, NULL, NULL, more);

         /* draw vertical connecting lines for upper tree levels */
         for (i = 0; i < level; i++) {
           if (more[i])
             XDrawLine(XtDisplay(w), XtWindow(w), solid_gc,
                       x + TreeLX(i, sz), y0,
                       x + TreeLX(i, sz), y1);
         }

         /* draw vertical connecting line for this tree level */
         xl = x + TreeLX(level, sz);
         if (level > 0) {
           XDrawLine(XtDisplay(w), XtWindow(w), solid_gc,
                     xl, y0,
                     xl, more[level]? y1: y + grid_height/2);

           if (file_view_data->file_data->is_subdir || !more[level]) {
             /* draw horizontal line */
             XDrawLine(XtDisplay(w), XtWindow(w),
                       (file_view_data->file_data->is_subdir &&
                        file_view_data->ts == tsNotRead)? dash_gc: solid_gc,
                       xl, y + grid_height/2,
                       xl + TreeOffset, y + grid_height/2);
           }
         }

         /* draw nil symbol for empty subdirs */
         if (extra_height)
         {
            xl += TreeOneWd(sz);
            y0 += grid_height + YSPACING(file_mgr_data);
            y1 = y + grid_height + YSPACING(file_mgr_data);
            e_height = extra_height - YSPACING(file_mgr_data);

            XDrawLine(XtDisplay(w), XtWindow(w), solid_gc,
                      xl, y0,
                      xl, y1 + e_height/2);

            XDrawLine(XtDisplay(w), XtWindow(w), solid_gc,
                      xl, y1 + e_height/2,
                      xl + TreeOffset, y1 + e_height/2);

            xl = x + TreeWd(level, sz) + TreeBtnWd[sz];
            XCopyArea(XtDisplay(w), TreePxTab[tpxNil].px[sz],
                      XtWindow(w), solid_gc,
                      0, 0, TreeNilWd[sz], TreeNilHt[sz],
                      xl, y1);
/*
            if (empty_msg == NULL)
               empty_msg = XtNewString("(empty)");

            if (file_mgr_data->cd_fonttype == XmFONT_IS_FONTSET)
            {
                extents = XExtentsOfFontSet(file_mgr_data->cd_fontset);
                font_yoffset = extents->max_logical_extent.y;
                font_height = extents->max_logical_extent.height;
            }
            else
            {
                font_yoffset = file_mgr_data->cd_font->ascent;
                font_height = file_mgr_data->cd_font->ascent +
                                file_mgr_data->cd_font->descent;
            }

            XDrawImageString(XtDisplay(w), XtWindow(w), solid_gc,
                      xl + TreeNilWd[sz] + 2,
                      y1 + (TreeNilHt[sz] - font_height)/2 + font_yoffset,
                      empty_msg, strlen(empty_msg));
*/
         }
      }

      /* goto next item */
      y = (Position)tmp;
   }
}


/************************************************************************
 *
 *  DisplaySomeIcons
 *    Check if any incons or widgets in the area given by ex, ey, ewd,
 *    and eht need to be updated.
 *
 ************************************************************************/

#ifdef DEBUG
static int g_workCount1 = 0;
static int g_workCount2 = 0;
static int g_callCount = 0;
#endif

static Boolean
ToBeManaged(
        IconLayoutData *layout_data,
        FileViewData *file_view_data)
{
   int i;

   for (i = 0; i < layout_data->manage_count; i++)
      if (layout_data->manage[i] == file_view_data->widget)
         return True;

   return False;
}


static int
DisplaySomeIcons(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        int ex, int ey, int ewd, int eht,
        int workLimit,
        Boolean doAll)
{
#ifdef DT_PERFORMANCE
   struct timeval update_time_s;
   struct timeval update_time_f;
#endif
   XmManagerWidget file_window;
   FileViewData **order_list;
   int order_count;
   Arg args[10];
   Arg args_dso_get[1];
   Arg args_dso_set[1];
   IconLayoutData *layout_data;
   FileViewData **change;
   int changeCount;
   Widget *manage;
   int manageCount;
   int workCount1;
   int workCount2;
   Dimension grid_width, grid_height;
   Dimension icon_width, icon_height;
   Dimension extra_height;
   int i, k;
   Position x, y;
   FileViewData  *file_view_data;
   Boolean changed;
   Widget child;
   DtIconGadget g;
   int level;
   unsigned char operations;
   Widget *wp;
   ObjectPosition *position_data;
   XRectangle textExtent;

   /* Get a list of icon and button widgets we can re-use */
   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   layout_data = (IconLayoutData *)file_mgr_data->layout_data;
   order_list = layout_data->order_list;
   order_count = layout_data->order_count;
   manage = layout_data->manage + layout_data->manage_count;

   /* allocate storage for list of changed icons */
   change = (FileViewData **)XtMalloc(order_count * sizeof(FileViewData *));

   /*  Find the maximum values for the icon heights and widths  */
   grid_width = file_mgr_data->grid_width;
   grid_height = file_mgr_data->grid_height;


#ifdef DT_PERFORMANCE
   printf("   Begin Part 1, DisplaySomeIcons (update icons)\n");
   gettimeofday(&update_time_s, NULL);

   /* added by Rafi */
   _DtPerfChkpntMsgSend("Begin Display Icons");
#endif

   DPRINTF2((
     "DisplaySomeIcons(\"%s\", x/y %d/%d, wd/ht %d/%d, i %d:%d) ...\n",
     file_mgr_data->current_directory, ex, ey, ewd, eht,
     layout_data->i_do_next_vis,
     layout_data->i_do_next_all));

   /* set up args for querying/unregistering drop sites */
   XtSetArg (args_dso_get[0], XmNdropSiteOperations, &operations);
   XtSetArg (args_dso_set[0], XmNdropSiteOperations, XmDROP_NOOP);

   /*
    * Iterate through the list of files and create/update and position
    * all visible icon gadgets for which this work hasn't been done yet
    * (need_update flag set).
    */

   changeCount = 0;
   manageCount = 0;
   workCount1 = 0;

   if (doAll)
      k = layout_data->i_do_next_all;
   else
      k = layout_data->i_do_next_vis;
   for ( ; k < order_count && workCount1 < workLimit; k++)
   {
      /*
       * Process focus widget before everything else so that we can restore
       * the focus when we manage the file_window again.  If a file was
       * being renamed, we do that one first.
       */
      if (!layout_data->focus_done)
      {
         /* search for focus widget in order_list and see if still displayed */
        file_view_data = NULL;
        if (file_mgr_rec->focus_widget != NULL)
        {
          if (file_mgr_rec->focus_widget == file_mgr_rec->file_window)
          {
            for (i = 0; i < order_count; i++)
              if (order_list[i]->displayed)
              {
                file_view_data = order_list[i];
                break;
              }
          }
          else
          {
            for (i = 0; i < order_count; i++)
              if (order_list[i]->widget == file_mgr_rec->focus_widget)
              {
                if (order_list[i]->displayed)
                  file_view_data = order_list[i];
                break;
              }
          }
        }

         /* if not found, focus could be on a rename text widget */
        if (file_view_data == NULL)
          file_view_data = file_mgr_data->renaming;

        layout_data->focus_done = True;

        if (file_view_data)
        {
          k--;
          /* decrement loop index, so that the entry that was supposed to be
             processed here will be looked at again in the next iteration
          */
          goto do_this_entry;
        }
      }


      /* ignore files that are filtered */
      file_view_data = order_list[k];
      if (file_view_data->filtered &&
          strcmp(file_view_data->file_data->file_name, ".") != 0)
      {
         continue;
      }

      /*
       * If the file is not currenly displayed (collapsed tree branch) or
       * is currenly scrolled out of view, we don't need to do anything
       * at this time.  Except ...
       */
      if (!file_view_data->displayed ||
          ((int)file_view_data->x + (int)grid_width) < ex  ||
          file_view_data->x >= ex + ewd ||
          file_view_data->y < ey ||
          ((int)file_view_data->y - (int)grid_height) >= (ey + eht))
      {
         /*
          * ... if this file still has an old icon widget, AND if the
          * old icon position is visible, AND if the icon was registered
          * as a drop site, we will have a "ghost drop zone" showing up where
          * the icon used to be.  The old icon gadget is unmanaged and will
          * remain unmanaged until later when we update it and move it to the
          * correct positon (happens only when the user scrolls to the new
          * position).  In the meantime, even though the gadget is unmanged,
          * its old position still remains registered as a drop site,
          * so we have to either unregister it or move the gadget to
          * its new position now.
          */
         child = file_view_data->widget;
         if (child != NULL &&
             (Position)(child->core.x + child->core.width) >= (Position)ex  &&
             child->core.x < ex + ewd &&
             (Position)(child->core.y + child->core.height) >= (Position)ey &&
             child->core.y < ey + eht)
         {
            if (!file_view_data->displayed)
            {
               XtGetValues (child, args_dso_get, 1);
               if (operations != XmDROP_NOOP)
               {
                  XtSetValues (child, args_dso_set, 1);
                  workCount1++;
                  file_view_data->registered = False;
               }
               continue;
            }

         } else
           continue;
      }

do_this_entry:
      /*
       * If not yet done, create/update the icon gadget for this file
       */
      if (file_view_data->need_update)
      {
         UpdateOneFileIcon(file_mgr_rec, file_mgr_data, file_view_data);
         child = file_view_data->widget;

         /*
          * We may need to adjust the icon position based on the difference
          * between estimated size and actual size.
          */
         if (layout_data->alignment == XmALIGNMENT_CENTER &&
             file_mgr_data->view == BY_NAME_AND_ICON)
         {
            EstimateIconSize(file_mgr_rec, file_mgr_data, layout_data,
                             file_view_data, &icon_width, &icon_height);

            if (child->core.width != icon_width)
            {
               file_view_data->x = file_view_data->x
                 - (Dimension)(grid_width - icon_width)/(Dimension)2
                 + (Dimension)(grid_width - child->core.width)/(Dimension)2;
            }
         }

         if (PositioningEnabledInView(file_mgr_data))
         {
            position_data = file_view_data->position_info;
            if (position_data->late_bind)
            {
               /*
                * When new objects are dropped on the random placement window,
                * they do not yet have a widget, so the drop_y could not be
                * adjusted to take into account the height of the icon;
                * this must be done now, after the object has an icon.
                */
/*
               position_data->y -= child->core.height/2;
*/
               position_data->late_bind = False;
               file_view_data->y = position_data->y + child->core.height;
            }
            else if (child->core.height != grid_height)
            {
               /* @@@ ??? @@@
                * Not quite right: have to distinguish two cases:
                *  (1) position_data->y read from .!xxx file
                *      file_view_data->y computed by adding grid_height
                *  (2) file_view_data->y from layout
                *      position_data->y computed by subtracting grid_height
                * In case (1) have to recompute file_view_data->y; in case
                * (2) have to recompute position_data->y.  Code below only
                * works for case (2).
                */
/*
               position_data->y = file_view_data->y - child->core.height;
*/
            }
         }

         changed = True;
      }
      else
         changed = False;


      /* if focus is supposed to be in the file window, focus on this widget */
      if (file_mgr_rec->focus_widget == file_mgr_rec->file_window)
         file_mgr_rec->focus_widget = file_view_data->widget;


      /* determine desired icon postition */
      child = file_view_data->widget;

      if (PositioningEnabledInView(file_mgr_data))
      {
         x = file_view_data->position_info->x;
         y = file_view_data->position_info->y;
      }
      else if (file_mgr_data->show_type != MULTIPLE_DIRECTORY)
      {
         x = file_view_data->x;
         y = file_view_data->y - child->core.height;
      }
      else /* file_mgr_data->show_type == MULTIPLE_DIRECTORY */
      {
         GetLevel(file_view_data, &level);
         extra_height = GetExtraHeight(file_mgr_data, file_view_data,
                                       layout_data->treebtn_size);

         /* check position of tree button, if any */
         if (file_view_data->treebtn)
         {
           child = file_view_data->treebtn;

           x = file_view_data->x
                 + TreeLX(level, layout_data->treebtn_size) + TreeOffset;
           y = file_view_data->y
               - grid_height
               + (Dimension)(grid_height - child->core.height)/(Dimension)2;

           if (child->core.x != x || child->core.y != y)
              XmeConfigureObject(child, x, y, child->core.width,
				 child->core.height, child->core.border_width);

           child = file_view_data->widget;
         }

         if (child->core.height < grid_height)
         {
            x = file_view_data->x + TreeWd(level, layout_data->treebtn_size);
            y = file_view_data->y
                - grid_height
                +(Dimension)(grid_height - child->core.height)/(Dimension)2;
         }
         else
         {
            x = file_view_data->x + TreeWd(level, layout_data->treebtn_size);
            y = file_view_data->y - grid_height;
         }
      }

      if (child->core.x != x || child->core.y != y)
      {
	  XmeConfigureObject(child, x, y, child->core.width,
			     child->core.height, child->core.border_width);
	  changed = True;
      }

      /* make sure the icon gadget and tree button, if any, are managed */
      if (!XtIsManaged(child) && !ToBeManaged(layout_data, file_view_data))
      {
         manage[manageCount++] = child;
         if (file_view_data->treebtn)
            manage[manageCount++] = file_view_data->treebtn;
         changed = True;
      }

      if (changed)
      {
         /* remember which icons were changed */
         change[changeCount++] = file_view_data;
         workCount1++;

         /*
          * If the icon we just changed was being renamed, make sure
          * the corresponding text widget is positioned correctly.
          */
         if (file_view_data == file_mgr_data->renaming)
         {
            for (i = 0; i < file_window->composite.num_children; i++)
            {
               child = file_window->composite.children[i];
               if (XmIsTextField(child))
               {
                  /* Check if the text field is still needed */
                  if (!child->core.being_destroyed)
                  {
                     /* move to the correct position */
                     _DtIconGetTextExtent_r(file_view_data->widget,
					    &textExtent);
                     x = textExtent.x;
                     y = textExtent.y -
                         (Dimension)(child->core.height - textExtent.height)/(Dimension)2;
		     XmeConfigureObject(child, x, y, child->core.width,
					child->core.height,
					child->core.border_width);

                     /* manage it */
                     manage[manageCount++] = child;
                  }
                  break;
               }
            }
         }
      }
   }

   /* remember where we left off ... */
   if (doAll)
      layout_data->i_do_next_all = k;
   else
      layout_data->i_do_next_vis = k;

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done Part 1, DisplaySomeIcons, time: %ld.%ld\n\n",
          update_time_f.tv_sec - update_time_s.tv_sec,
          update_time_f.tv_usec - update_time_s.tv_usec);

   /* the following message send call added by Rafi */
   _DtPerfChkpntMsgSend("Done  Display Icons");
#endif

#ifdef DT_PERFORMANCE
   printf("   Begin Part 2, DisplaySomeIcons (register drop sites)\n");
   gettimeofday(&update_time_s, NULL);

   /* added by Rafi  */
   _DtPerfChkpntMsgSend("Begin Register drop sites");
#endif

   /* unregister drop sites of unused icon gadgets */
   workCount2 = 0;
#ifdef DELAYED_UNREGISTER
   for (wp = layout_data->next_icon_to_use;
        (child = *wp) != NULL;
        wp++)
   {
      if ((Position)(child->core.x + child->core.width) >= (Position)ex  &&
          child->core.x < ex + ewd &&
          (Position)(child->core.y + child->core.height) >= (Position)ey &&
          child->core.y < ey + eht)
      {
         XtGetValues (child, args_dso_get, 1);
         if (operations != XmDROP_NOOP)
         {
            XtSetValues (child, args_dso_set, 1);
            workCount2++;

            if (workCount2/2 >= workLimit)
               break;
         }
      }
   }
#endif

   /*
    * Register drop sites
    *
    * Note: in "as placed" mode, we defer this work and do it in
    * CommitWorkProcUpdates instead.  Reason: need to re-register
    * all drop sites in top-to-bottom stacking order at that time.
    */
   if (! PositioningEnabledInView(file_mgr_data) &&
       file_mgr_data != trashFileMgrData)
   {
      for (k = 0; k < changeCount; k++)
      {
         SetHotRects(change[k],
                     (XtCallbackProc) DropOnObject,
                     (XtPointer) change[k]);
      }
   }

   /* free storage */
   XtFree((char *)change);
   change = NULL;

   /* update count of children to be managed */
   layout_data->manage_count += manageCount;


#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done Part 2 DisplaySomeIcons, time: %ld.%ld\n\n",
          update_time_f.tv_sec - update_time_s.tv_sec,
          update_time_f.tv_usec - update_time_s.tv_usec);

   /* the following message send call added by Rafi */
   _DtPerfChkpntMsgSend("Done  Register drop sites");
#endif

   DPRINTF2((" ... %d+%d changed (%d managed)\n",
             workCount1, workCount2, manageCount));

#ifdef DEBUG
   g_workCount1 += workCount1;
   g_workCount2 += workCount2;
   g_callCount++;
#endif

   return (workCount1 >= workCount2)? workCount1: workCount2;
}


/************************************************************************
 *
 *  Display work procedure
 *
 ************************************************************************/

static void
CommitWorkProcUpdates(
  FileMgrRec *file_mgr_rec,
  FileMgrData * file_mgr_data,
  Boolean reset)
{
   XmManagerWidget file_window;
   IconLayoutData *layout_data;
   Widget w;
   int i;

   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   layout_data = (IconLayoutData *)file_mgr_data->layout_data;

   if (layout_data == NULL)
     return;

   DPRINTF((
    "CommitWorkProcUpdates: %d+%d updates (m %d, n %d, i %d:%d, reset %d)\n",
            g_workCount1,
            g_workCount2,
            layout_data->manage_count,
            g_callCount,
            layout_data->i_do_next_vis,
            layout_data->i_do_next_all,
            reset));

#ifdef DEBUG
   g_workCount1 = 0;
   g_workCount2 = 0;
   g_callCount = 0;
#endif

   if (reset)
   {
      layout_data->i_do_next_vis = 0;
      layout_data->i_do_next_all = 0;
   }

   /* if work proc no longer active, everything should already be commited */
   if (layout_data->work_id == 0)
     return;

   /* manage new children */
   if (layout_data->manage_count > 0)
   {
      if (PositioningEnabledInView(file_mgr_data))
         OrderChildrenList(file_mgr_data);
      XtManageChildren(layout_data->manage, layout_data->manage_count);
   }

   /*
    * In "as placed" mode, need to register drop sites now.
    * (If not "as placed" mode, this was already done in DisplaySomeIcons.)
    */
   if (PositioningEnabledInView(file_mgr_data))
      RegisterDesktopHotspots(file_mgr_data, file_mgr_rec);

   /* commit drop site updates */
   XmDropSiteEndUpdate(layout_data->drop_site_w);

   /* If not managed yet, manage the file window again */
   if (!XtIsManaged((Widget)file_window))
   {
     XtArgVal incr;
     Arg args[2];

     XtManageChild ((Widget)file_window);
     XtVaGetValues(file_mgr_rec->vertical_scroll_bar,XmNuserData,&incr,NULL);
     if (VerticalScrollbarIsVisible(file_mgr_rec->vertical_scroll_bar,
                                    file_mgr_rec->scroll_window)
         && incr > 0 )
     {
        XtSetArg (args[0], XmNincrement,incr);
        XtSetValues (file_mgr_rec->vertical_scroll_bar, args, 1);
     }
     XmUpdateDisplay ((Widget)file_window);
   }

   /* Try to preserve the focus */
   if (file_mgr_rec->focus_widget)
   {
      /* see if the widget that previously had the focus was just managed */
      w = NULL;
      for (i = 0; i < layout_data->manage_count; i++)
      {
         if (layout_data->manage[i] == file_mgr_rec->focus_widget)
         {
            w = file_mgr_rec->focus_widget;
            break;
         }
      }

      /* if focus widget not found, set focus on file_window */
      if (w == NULL)
         w = (Widget)file_window;

      XmProcessTraversal(w, XmTRAVERSE_CURRENT);

      file_mgr_rec->focus_widget = NULL;
   }

   layout_data->manage_count = 0;

   /* start new update */
   XmDropSiteStartUpdate(layout_data->drop_site_w);
}


static Boolean
DisplayWorkProc(
  XtPointer client_data)
{
   FileMgrRec *file_mgr_rec = (FileMgrRec *)client_data;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   XmManagerWidget file_window;
   IconLayoutData *layout_data;
   int ex, ey, ewd, eht;
   ObjectPosition *bottom;
   Widget child;
   int n;
   Boolean commit_updates = False;

   int n1 = 1;
   int n2 = 1;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   if (dialog_data == NULL)
      return False;

   file_mgr_data = (FileMgrData *) dialog_data->data;
   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   layout_data = (IconLayoutData *)file_mgr_data->layout_data;

   /* get the position and size of the currently visible area */
   ex = -file_window->core.x;
   ey = -file_window->core.y;
   ewd = XtParent(file_window)->core.width;
   eht = XtParent(file_window)->core.height;

   /* check if the window scrolled */
   if (ex != layout_data->ex || ey != layout_data->ey)
   {
      layout_data->i_do_next_vis = layout_data->i_do_next_all;
      layout_data->visible_done = False;
   }

   /* first work on icons in the currently visible area */
   if (layout_data->visible_done)
      n = 0;
   else
   {
     n = DisplaySomeIcons(file_mgr_rec, file_mgr_data, ex, ey, ewd, eht, n1, False);
     if (n == 0)
     {
       /* we just finished updating all visible icons */
       DPRINTF(("DisplayWorkProc: visible done.\n"));
       layout_data->visible_done = True;
       commit_updates = True;
     }
   }

   /* if we still have some time left, work on other icons */
   if (layout_data->visible_done && n < n2)
   {
      n = DisplaySomeIcons(file_mgr_rec, file_mgr_data,
                           0, 0, 32767, 32767, n2 - n, True);

      /* check if we are done */
      if (n == 0)
      {
         layout_data->all_done = True;
         commit_updates = True;
      }
      else if (layout_data->manage_count >= 100)
         commit_updates = True;
   }

   if (commit_updates)
   {
      /* manage new children and commit drop site updates */
      CommitWorkProcUpdates(file_mgr_rec, file_mgr_data, False);

      /*
       * In "as placed" mode, icons may overlap.
       * Force redraw in bottom to top stacking order.
       */
      if (PositioningEnabledInView(file_mgr_data))
      {
         for (bottom = GetBottomOfStack(file_mgr_data);
              bottom;
              bottom = (ObjectPosition *)bottom->prev)
         {
            if (bottom->file_view_data != NULL &&
                bottom->file_view_data->displayed &&
                !bottom->file_view_data->need_update)
            {
               child = bottom->file_view_data->widget;
               if ((Position)(child->core.x + child->core.width) >= (Position)ex  &&
                   child->core.x < ex + ewd &&
                   (Position)(child->core.y + child->core.height) >= (Position)ey &&
                   child->core.y < ey + eht)
               {
                  RedrawOneGadget(child, NULL, NULL);
               }
            }
         }
      }
   }

   if (layout_data->all_done)
   {
      /* all work is done; all icons are up-to-date */
      file_mgr_data->newSize = False;
      layout_data->work_id = 0;
      XmDropSiteEndUpdate(layout_data->drop_site_w);

      DPRINTF(("DisplayWorkProc: all done.\n"));

      /* returning True will end the work proc */
      return True;
   }
   else
   {
      /* remember current scroll position */
      layout_data->ex = ex;
      layout_data->ey = ey;
      return False;
   }
}


/************************************************************************
 *
 *  LayoutFileIcons
 *      Position and size the full set of icons for the file mgr data.
 *
 ************************************************************************/

void
LayoutFileIcons(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        Boolean update_scrolling_position,
        Boolean turn_off_hourglass )
{
#ifdef DT_PERFORMANCE
   struct timeval update_time_s;
   struct timeval update_time_f;
#endif
   XmManagerWidget file_window;
   FileViewData ** order_list;
   int order_count;
   int total_icon_count;
   IconLayoutData *layout_data;
   Dimension file_window_width, file_window_height;
   Dimension scrolled_window_width, scrolled_window_height;
   Dimension vert_scrollbar_width, horiz_scrollbar_height;
   Dimension grid_width, grid_height;
   Dimension icon_width, icon_height;
   Dimension extra_height;
   Dimension sw_shadow_thickness, sb_highlight_thickness, space;
   int len, max_len;
   int level, max_level;
   FileViewData *file_view_data;
   Boolean overflow = False;
   int i, j, k;
   Position x, y;
   int row_count;
   int column_count;
   int tmp;
   Arg args[10];
   char *fileType;

   unsigned char operations = 0L;
   static XtCallbackRec dropCB[] = { {DropOnFileWindow, NULL}, {NULL, NULL} };


   /* if directory-read still in progress, don't do anything now */
   if (file_mgr_data->busy_status != not_busy)
      return;

   DPRINTF(("LayoutFileIcons(\"%s\", u_s_p %c, t_o_h %c) ...\n",
            file_mgr_data->current_directory,
            update_scrolling_position? 'T': 'F',
            turn_off_hourglass? 'T': 'F'));

#ifdef DT_PERFORMANCE
   printf("   Begin LayoutFileIcons\n");
   gettimeofday(&update_time_s, NULL);

   /* added by Rafi */
   _DtPerfChkpntMsgSend("Begin Layout Icons");
#endif

   /*
    * Just in case a previous update work proc is still active,
    * we commit pending updates made by the work proc before we
    * start making changes.
    */
   CommitWorkProcUpdates(file_mgr_rec, file_mgr_data, True);

   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   layout_data = (IconLayoutData *)file_mgr_data->layout_data;

   fileType = GetDirectoryLogicalType(file_mgr_data,
                                        file_mgr_data->current_directory);
   operations = TypeToDropOperations(fileType);

   if (!file_mgr_data->dropSite)
   {
       dropCB[0].closure = (XtPointer)file_mgr_data;

       DtDndVaDropRegister((Widget)file_mgr_rec->file_window,
                           DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
                           operations,
                           dropCB,
                           DtNdropAnimateCallback, dropCB,
                           DtNregisterChildren, True,
                           DtNtextIsBuffer,	True,
                           XmNanimationStyle,	XmDRAG_UNDER_SHADOW_IN,
                           NULL);
       file_mgr_data->dropSite = True;

       if (!operations)
       {
           XtSetArg (args[0], XmNdropSiteOperations, XmDROP_NOOP);
           XmDropSiteUpdate((Widget)file_mgr_rec->file_window, args, 1);
       }
   }
   else
   {
       if (operations)
           XtSetArg (args[0], XmNdropSiteOperations, operations);
       else
           XtSetArg (args[0], XmNdropSiteOperations, XmDROP_NOOP);
       XmDropSiteUpdate((Widget)file_mgr_rec->file_window, args, 1);
   }

   /*  Get the count of the total icon to be displayed.  */
   order_list = layout_data->order_list;
   order_count = layout_data->order_count;
   total_icon_count = 0;
   for (i = 0; i < order_count; i++)
   {
      if (order_list[i]->displayed)
         total_icon_count++;
   }

   /*  Get the colors to be used for drawing the icons  */
   XtSetArg (args[0], XmNbackground, &layout_data->background);
   XtSetArg (args[1], XmNforeground, &layout_data->foreground);
#ifdef _SHOW_LINK
   XtSetArg (args[2], XmNtopShadowColor, &layout_data->topshadow);
   XtGetValues (file_mgr_rec->file_window, args, 3);
#else
   XtGetValues (file_mgr_rec->file_window, args, 2);
#endif
   XtSetArg (args[0], XmNtopShadowColor, &layout_data->pixmap_back);
   XtSetArg (args[1], XmNbottomShadowColor, &layout_data->pixmap_fore);
   XtGetValues (file_mgr_rec->main, args, 2);

   /*
    * Create/update icon gadget for ".".
    * This gadget may not actually be displayed (depends of filter settings;
    * by default it is filtered out), but we need at least one gadget
    * created here so we can figure out fonts and margins used by icon
    * gadgets.  This is necessary to compute the grid spacing.
    */
   for (i = 0; i < order_count; i++)
      if (strcmp(order_list[i]->file_data->file_name, ".") == 0)
      {
         UpdateOneFileIcon(file_mgr_rec, file_mgr_data, order_list[i]);
         order_list[i]->need_update = True;
         break;
      }

   /* Find the icon with the longest label */
   max_len = 0;
   file_view_data = NULL;
   for (i = 0; i < order_count; i++)
   {
      if (!order_list[i]->filtered &&
          (len = strlen(order_list[i]->label)) > max_len)
      {
         file_view_data = order_list[i];
         max_len = len;
      }
   }

   /* get the size of the icon with the longest label */
   GetIconLayoutParms(file_mgr_rec, file_mgr_data, layout_data);
   EstimateIconSize(file_mgr_rec, file_mgr_data, layout_data, file_view_data,
                    &grid_width, &grid_height);

   /* for tree view add space for tree lines and buttons */
   max_level = 0;
   if (file_mgr_data->show_type == MULTIPLE_DIRECTORY)
   {
      for (i = 0; i < order_count; i++)
      {
         if (order_list[i]->displayed)
         {
            GetLevel(order_list[i], &level);
            if (level > max_level)
               max_level = level;
         }
      }
      grid_width += TreeWd(max_level, layout_data->treebtn_size);
   }

   file_mgr_data->grid_height = grid_height;
   file_mgr_data->grid_width = grid_width;

   /*
    * If positioning is enabled in this view, then we need to use the other
    * layout function, which is capable of handling overlapping icons and
    * stacking order.
    */
   if (PositioningEnabledInView(file_mgr_data))
   {
      if (file_mgr_data->object_positions)
      {
         LayoutDesktopIcons(file_mgr_rec, file_mgr_data,
			    order_list, order_count, turn_off_hourglass);
         /* RepairFileWindow(file_mgr_data); *OBSOLETE* */
         goto layout_done;
      }
   }

   /*
    * Position and size the icons according to the show and view types.
    */

   scrolled_window_width = file_mgr_rec->scroll_window->core.width;
   scrolled_window_height = file_mgr_rec->scroll_window->core.height;
   vert_scrollbar_width = file_mgr_rec->vertical_scroll_bar->core.width;
   horiz_scrollbar_height = file_mgr_rec->horizontal_scroll_bar->core.height;

   if (file_mgr_data->show_type == MULTIPLE_DIRECTORY)
   {
     /* if not yet done, create tree button icons */
     if (TreeBtnWd[0] == 0)
       CreateTreeIcons(file_mgr_rec->file_window);

     /* layout for tree mode */
     x = MARGIN;
     y = MARGIN;
     file_window_width = MARGIN + grid_width - TreeOffset + XSPACING;

     for (k = 0; k < order_count; k++)
     {
       if (!order_list[k]->displayed)
         continue;

       /* make sure window height won't exceed limit */
       extra_height = GetExtraHeight(file_mgr_data, order_list[k],
                                     layout_data->treebtn_size);
       tmp = (int)y + grid_height + extra_height + YSPACING(file_mgr_data);
       if (tmp + MARGIN > MAXWINSIZE)
       {
         /* window would exceed height limit; start a new column */
         x += grid_width + XSPACING;
         y = MARGIN;
         file_window_width += grid_width + XSPACING;
         tmp = (int)y + grid_height + extra_height + YSPACING(file_mgr_data);
         overflow = True;
       }

       order_list[k]->x = x;
       order_list[k]->y = y + grid_height;

       /* update y */
       y = (Position)tmp;
     }

     if (overflow)
       file_window_height = MAXWINSIZE;
     else
       file_window_height = y + MARGIN;
   }
   else if (file_mgr_data->view == BY_ATTRIBUTES)
   {
      /* layout for details view, no tree mode */
      x = MARGIN;
      y = MARGIN;
      file_window_width = 2*MARGIN + grid_width;

      for (k = 0; k < order_count; k++)
      {
         if (!order_list[k]->displayed)
            continue;

         /* make sure window height won't exceed limit */
         tmp = (int)y + grid_height + YSPACING(file_mgr_data);
         if (tmp + MARGIN > MAXWINSIZE)
         {
            /* window would exceed height limit; start a new column */
            x += grid_width + XSPACING;
            y = MARGIN;
            file_window_width += grid_width + XSPACING;
            tmp = (int)y + grid_height + YSPACING(file_mgr_data);
            overflow = True;
         }

         order_list[k]->x = x;
         order_list[k]->y = y + grid_height;

         /* update y */
         y = (Position)tmp;
      }

      if (overflow)
         file_window_height = MAXWINSIZE;
      else
         file_window_height = y + MARGIN;
   }
   else  /* show_type == SINGLE_DIRECTORY, view != BY_ATTRIBUTES */
   {
      /* layout for "normal" views (no tree mode, no details) */

      /* calculate how many columns fit in the window width */
     column_count = ((int)scrolled_window_width - 4 - 2*MARGIN + XSPACING) /
       ((int)grid_width + XSPACING);
     if (column_count == 0)
       column_count = 1;  /* need at least one column */

      /*
       * Calculate the window height.  Need to do calculation in int's
       * rather than short (Dimension) because of possible overflow.
       */
     row_count = (total_icon_count + column_count - 1) / column_count;
     tmp = 2*MARGIN - YSPACING(file_mgr_data) +
       row_count * ((int)grid_height + YSPACING(file_mgr_data));

     /* check if the height is larger than the scrolled window */
     if (tmp >= (int)scrolled_window_height - 2*MARGIN)
     {
       /* need to recompute everything because of space for vert scrollbar */
       column_count = ((int)scrolled_window_width - 4 - 2*MARGIN + XSPACING
                       - (int)vert_scrollbar_width)
         / ((int)grid_width + XSPACING);
       if (column_count == 0)
         column_count = 1;
       row_count = (total_icon_count + column_count - 1) / column_count;
       tmp = 2*MARGIN - YSPACING(file_mgr_data) +
         row_count * ((int)grid_height + YSPACING(file_mgr_data));
     }

     /* check if the window height is within the limit */
     if (tmp <= MAXWINSIZE)
       file_window_height = (Dimension)tmp;
     else
     {
       /* window height too large; use more columns */
       overflow = True;
       row_count = (int)(MAXWINSIZE - 2*MARGIN + YSPACING(file_mgr_data)) /
         (int)((int)grid_height + YSPACING(file_mgr_data));
       column_count = (total_icon_count  + row_count - 1) / row_count;
       row_count = (total_icon_count + column_count - 1) / column_count;
       file_window_height = (Dimension) (2*MARGIN - YSPACING(file_mgr_data) +
                                         row_count * ((int)grid_height + YSPACING(file_mgr_data)));
     }

     DPRINTF(("  %d columns, %d rows (scroll_window width %d)\n",
              column_count, row_count,
              scrolled_window_width));

     /* assign positions to all icons */
     y = MARGIN;
     file_window_width = (Dimension)
       (2*MARGIN - XSPACING + column_count*((int)grid_width + XSPACING));

     k = 0;
     for (i = 0; i < row_count; i++)
     {
       x = MARGIN;

       for (j = 0; j < column_count && k < order_count;)
       {
         /* find the next icon to display */
         while (k < order_count && !order_list[k]->displayed)
           k++;
         if (k == order_count)
           break;

         if (layout_data->alignment == XmALIGNMENT_CENTER &&
             file_mgr_data->view == BY_NAME_AND_ICON)
         {
           EstimateIconSize(file_mgr_rec, file_mgr_data, layout_data,
                            order_list[k], &icon_width, &icon_height);
           order_list[k]->x = x +
             (Dimension)(grid_width - icon_width)/(Dimension)2;
         }
         else
           order_list[k]->x = x;
         order_list[k]->y = y + grid_height;

         x += grid_width + XSPACING;
         j++;
         k++;
       }

       y += grid_height + YSPACING(file_mgr_data);
     }
   }

   DPRINTF(("  file_window size: width %d, height %d\n",
            file_window_width, file_window_height));

   /*  Manage/unmanage the scrollbars as needed.  */

   XtSetArg (args[0], XmNwidth, file_window_width);
   XtSetArg (args[1], XmNheight, file_window_height);
   XtSetValues ((Widget)file_window, args, 2);

   XtSetArg (args[0], XmNscrollBarDisplayPolicy, XmAS_NEEDED);
   XtSetValues (file_mgr_rec->scroll_window, args, 1);


   /*  Unmanage the horizontal scrollbar if it is not needed   */
   if (VerticalScrollbarIsVisible(file_mgr_rec->vertical_scroll_bar,
                                  file_mgr_rec->scroll_window))
   {
      if (file_window_width >= (Dimension)(scrolled_window_width - 4 -
                                                         vert_scrollbar_width))
         XtManageChild (file_mgr_rec->horizontal_scroll_bar);
      else
      {
         XtUnmanageChild (file_mgr_rec->horizontal_scroll_bar);
         XtSetArg (args[0], XmNx, 0);
         XtSetValues ((Widget)file_window, args, 1);
      }
   }
   else if (file_window_width >= (Dimension)(scrolled_window_width - 4))
      XtManageChild (file_mgr_rec->horizontal_scroll_bar);
   else
   {
      XtUnmanageChild (file_mgr_rec->horizontal_scroll_bar);
      XtSetArg (args[0], XmNx, 0);
      XtSetValues ((Widget)file_window, args, 1);
   }

   XSync (XtDisplay (file_window), False);

   /*  Set the file window width and height to be at least  */
   /*  the size of the scrolled window.                     */

   if (VerticalScrollbarIsVisible(file_mgr_rec->vertical_scroll_bar,
                                  file_mgr_rec->scroll_window))
   {
      if(file_window_width <= (Dimension)(scrolled_window_width -
                           (vert_scrollbar_width + 4)))

         file_window_width = scrolled_window_width -
                           (vert_scrollbar_width + 4);
   }
   else
   {
      if(file_window_width < (Dimension)(scrolled_window_width - 4))
         file_window_width = scrolled_window_width - 4;
   }

   if (HorizontalScrollbarIsVisible(file_mgr_rec->horizontal_scroll_bar,
                                    file_mgr_rec->scroll_window))
   {
      int pad;

      XtSetArg (args[0], XmNhighlightThickness, &sb_highlight_thickness);
      XtGetValues ((Widget)file_mgr_rec->horizontal_scroll_bar, args, 1);

      XtSetArg (args[0], XmNshadowThickness, &sw_shadow_thickness);
      XtSetArg (args[1], XmNspacing, &space);
      XtGetValues ((Widget)file_mgr_rec->scroll_window, args, 2);

      pad = (int)(((sb_highlight_thickness + sw_shadow_thickness) * 2) + space);

      if(file_window_height < (Dimension)(scrolled_window_height -
                        (horiz_scrollbar_height + pad)))
         file_window_height = scrolled_window_height -
                        (horiz_scrollbar_height + pad);
   }
   else
   {
      if (file_window_height < (Dimension)(scrolled_window_height - 4))
         file_window_height = scrolled_window_height - 4;
   }


   XtSetArg (args[0], XmNwidth, file_window_width);
   XtSetArg (args[1], XmNheight, file_window_height);
   XtSetValues ((Widget)file_window, args, 2);

   if( file_mgr_data->scrollToThisFile == NULL )
   {
     if (update_scrolling_position)
     {
       XtSetArg (args[0], XmNx, 0);
       XtSetArg (args[1], XmNy, 0);
       XtSetValues ((Widget)file_window, args, 2);
     }

     /*  Set the vertical scrollbar's increment to icon height  */
     XtSetArg (args[0], XmNincrement, grid_height + YSPACING(file_mgr_data));
     XtSetArg (args[1], XmNuserData, grid_height + YSPACING(file_mgr_data));
     XtSetValues (file_mgr_rec->vertical_scroll_bar, args, 2);
   }

   if (PositioningEnabledInView(file_mgr_data))
   {
     BuildObjectPositions(file_mgr_data);
     /* RepairFileWindow(file_mgr_data); *OBSOLETE* */
   }

   /*
    * Don't keep up the hourglass; this helps the user to get the impression
    * that all of the work is done.
    */
   if (turn_off_hourglass && !overflow)
      _DtTurnOffHourGlass(file_mgr_rec->shell);


layout_done:


   /*
    * Start up a work proc that will update the display.
    */
   layout_data->visible_done = False;
   layout_data->all_done = False;
   if (layout_data->work_id == 0)
   {
      XtAppContext app_context =
        XtWidgetToApplicationContext(file_mgr_rec->shell);

      DPRINTF(("LayoutFileIcons: starting workproc\n"));
      layout_data->drop_site_w = (Widget)file_mgr_rec->shell;
      XmDropSiteStartUpdate(layout_data->drop_site_w);

      layout_data->work_id =
        XtAppAddWorkProc(app_context, DisplayWorkProc, file_mgr_rec);
   }


   if( file_mgr_data->scrollToThisFile )
   {
     int i;
     DirectorySet * directory_set = NULL;

     file_view_data = NULL;

     for( i = 0; i < file_mgr_data->directory_count; ++i)
     {
       if( strcmp( ((DirectorySet *)file_mgr_data->directory_set[i])->name, file_mgr_data->scrollToThisDirectory ) == 0 )
       {
         directory_set = (DirectorySet *)file_mgr_data->directory_set[i];
         break;
       }
     }

     if( directory_set )
     {
       for( i = 0; i < directory_set->file_count; ++i )
       {
         if( strcmp( directory_set->order_list[i]->file_data->file_name,
                     file_mgr_data->scrollToThisFile ) == 0 )
         {
           file_view_data = directory_set->order_list[i];
           break;
         }
       }
     }

     if( file_view_data
         && file_view_data->filtered == False )
     {
       FileMgrRec * file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;

       DeselectAllFiles( file_mgr_data );
       SelectFile( file_mgr_data, file_view_data );
       ActivateSingleSelect( file_mgr_rec,
             file_mgr_data->selection_list[0]->file_data->logical_type );

       PositionFileView(file_view_data, file_mgr_data);
     }
     else
     {
       if (update_scrolling_position)
       {
         XtSetArg (args[0], XmNx, 0);
         XtSetArg (args[1], XmNy, 0);
         XtSetValues ((Widget)file_window, args, 2);
       }

       /* Set the vertical scrollbar's increment to icon height  */
       XtSetArg (args[0], XmNincrement, grid_height + YSPACING(file_mgr_data));
       XtSetArg (args[1], XmNuserData, grid_height + YSPACING(file_mgr_data));
       XtSetValues (file_mgr_rec->vertical_scroll_bar, args, 2);
     }

     XtFree( file_mgr_data->scrollToThisFile );
     file_mgr_data->scrollToThisFile = NULL;
     XtFree( file_mgr_data->scrollToThisDirectory );
     file_mgr_data->scrollToThisDirectory = NULL;
   }

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done LayoutFileIcons, time: %ld.%ld\n\n",
          update_time_f.tv_sec - update_time_s.tv_sec,
          update_time_f.tv_usec - update_time_s.tv_usec);

   /* the following message send call added by Rafi */
   _DtPerfChkpntMsgSend("Done LayoutFileIcons");
#endif

   DPRINTF(("... done\n"));
}



/************************************************************************
 *
 *  TreeBtnCallback
 *      Callback function invoked upon clicking the tree-branch-expand button.
 *
 ************************************************************************/

static void
TreeBtnCallback(
        Widget w,
        XtPointer clientData,
        XmAnyCallbackStruct *callData )
{
  FileMgrRec *file_mgr_rec;
  FileMgrData *file_mgr_data;
  FileViewData *file_view_data = (FileViewData *)clientData;
  Arg args[20];
  Bool expand;

  /* check which mouse button was pressed */
  if ((callData->event->type == ButtonPress ||
       callData->event->type == ButtonRelease) &&
      ((XButtonEvent *)callData->event)->button != Button1)
  {
    return;
  }
  expand = True;

  /* get file mgr data and record */
  XtSetArg (args[0], XmNuserData, &file_mgr_data);
  XtGetValues (w, args, 1);
  file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;

  XmDropSiteStartUpdate(file_mgr_rec->file_window);
  CommitWorkProcUpdates(file_mgr_rec, file_mgr_data, True);
  DirTreeExpand(file_mgr_data, file_view_data, expand);
  DrawCurrentDirectory (file_mgr_rec->current_directory,
                        file_mgr_rec, file_mgr_data);
  LayoutFileIcons(file_mgr_rec, file_mgr_data, False, True);
  XmDropSiteEndUpdate(file_mgr_rec->file_window);
  RedrawTreeLines(file_mgr_rec->file_window,
                  -file_mgr_rec->file_window->core.x,
                  -file_mgr_rec->file_window->core.y,
                  XtParent(file_mgr_rec->file_window)->core.width,
                  XtParent(file_mgr_rec->file_window)->core.height,
                  0, file_mgr_rec, file_mgr_data);
}


/*
 * When a drop occurs on a File Manger window, which now
 * support random placement, what is dropped may not ultimately be what
 * is displayed.  Since the dissolve transition effect has been disabled
 * for drops on the desktop, we can sometimes end up with garbage left on
 * the desktop.  This function will attempt to clear up the leftover garbage,
 * by resetting all areas of the desktop which are not covered by an icon
 * to the background color for the file window.
 *
 * This function is OBSOLETE (?).
 */

void
RepairFileWindow (
   FileMgrData * file_mgr_data)

{
   FileMgrRec * file_mgr_rec;
   Region clipList;
   Region hotspot;
   Region redrawRegion;
   int i;
   XRectangle rect;
   int num_children;
   Widget * children;
   XmManagerWidget file_window;
   GC gc;
   unsigned long mask;
   XGCValues values;

   file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;
   file_window = (XmManagerWidget)file_mgr_rec->file_window;

   /* Initialize the clip region to that of the file window */
   rect.x = 0;
   rect.y = 0;
   rect.height = file_window->core.height;
   rect.width = file_window->core.width;
   clipList = XCreateRegion();
   XUnionRectWithRegion(&rect, clipList, clipList);

   /* Subtract out the hotspots associated with each icon */
   num_children = file_window->composite.num_children;
   children = file_window->composite.children;
   hotspot = XCreateRegion();

   for (i = 0; i < num_children; i++)
   {
      if (XtIsManaged(children[i]) &&
          XtIsSubclass(children[i], dtIconGadgetClass))
      {
         WidgetRectToRegion(file_mgr_data, children[i], hotspot);
         XSubtractRegion(clipList, hotspot, clipList);
      }
   }

   /* Create a GC for doing our drawing */
   mask = GCForeground;
   values.foreground = file_window->core.background_pixel;
   gc = XCreateGC(XtDisplay(file_window), XtWindow(file_window), mask, &values);
   XSetRegion(XtDisplay(file_window), gc, clipList);

   /* Restore the window */
   XFillRectangle(XtDisplay(file_window), XtWindow(file_window), gc, 0, 0,
                  file_window->core.width, file_window->core.height);

   /*
    * Force all icons to redraw, since we are only able to repair the
    * areas where icons are not; the drop image may have extended onto
    * some of the icons.
    */
   rect.x = 0;
   rect.y = 0;
   rect.height = file_window->core.height;
   rect.width = file_window->core.width;
   redrawRegion = XCreateRegion();
   XUnionRectWithRegion(&rect, redrawRegion, redrawRegion);
   (*file_window->core.widget_class->core_class.expose)
             ((Widget)file_window, NULL, redrawRegion);

   /* Clean up */
   XFreeGC(XtDisplay(file_window), gc);
   XDestroyRegion(clipList);
   XDestroyRegion(hotspot);
   XDestroyRegion(redrawRegion);
}




/*
 * Compress the stacking order values, anytime an item is removed.
 */

static void
CompressObjectList (
   ObjectPosition ** object_positions,
   int num_objects,
   int starting_index)
{
   int i;

   for (i = 0; i < num_objects; i++)
   {
      if (object_positions[i]->stacking_order > starting_index)
         object_positions[i]->stacking_order--;
   }
}



/************************************************************************
 *
 *  LayoutDesktopIcons
 *      Position and size the full set of icons for the file mgr data.
 *
 ************************************************************************/

static void
LayoutDesktopIcons (
     FileMgrRec  * file_mgr_rec,
     FileMgrData * file_mgr_data,
     FileViewData ** order_list,
     int order_count,
     Boolean turn_off_hourglass )

{
   XmManagerWidget file_window;
   int directory_count, largest_x, largest_y;
   int value, size, increment, page;
   Dimension current_wd;
   Dimension current_ht;
   Dimension file_window_width;
   Dimension grid_height;
   Dimension grid_width;
   FileViewData * object;
   ObjectPosition * position_data;
   ObjectPosition * bottom;
   ObjectPtr top;
   int i, j, k;
   Boolean set_size = False;
   char * edit_name;
   Arg args[2];
   int sorder;


   /*  Get the grid block size */
   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   grid_height = file_mgr_data->grid_height;
   grid_width = file_mgr_data->grid_width;

   file_window_width = file_mgr_rec->scroll_window->core.width - 4;
   current_ht = file_mgr_rec->file_window->core.height;
   current_wd = file_mgr_rec->file_window->core.width;


   /* Before positioning, mark all position entries as 'not used' */
   for (i = 0; i < file_mgr_data->num_objects; i++)
   {
      file_mgr_data->object_positions[i]->in_use = False;
      file_mgr_data->object_positions[i]->file_view_data = NULL;
   }

   /*
    * Before attempting to place new icons, we need to make sure that
    * all of the existing object_positions entries have had their
    * file_view_data field filled in.  This is so that during placement,
    * we can get the height and width of the associated icons, to help
    * prevent overlap.
    */
   for (i = 0; i < order_count; i++)
   {
      object = order_list[i];
      if (object->displayed)
      {
         if (position_data = GetPositionalData(file_mgr_data, object, 0, False))
         {
            /* Save; used later during redraw */
            position_data->file_view_data = object;
            object->position_info = (ObjectPtr)position_data;
         }
      }
      else if (position_data = GetPositionalData(file_mgr_data, object, 0,
                                                 False))
      {
         /*
          * If an object has position information, but is currently
          * filtered, don't discard its position info; mark it as in-use.
          */
         position_data->file_view_data = object;
         object->position_info = (ObjectPtr)position_data;
      }
   }

   /* Remove any unused position data entries */
   for (i = 0; i < file_mgr_data->num_objects; )
   {
      if (!file_mgr_data->object_positions[i]->in_use)
      {
         /* If this object had a text field, delete it */
         for (k = 0; k < file_window->composite.num_children; k++)
         {
            if (XmIsTextField(file_window->composite.children[k]) &&
                !file_window->composite.children[k]->core.being_destroyed)
            {
               XtSetArg(args[0], XmNuserData, &edit_name);
               XtGetValues(file_window->composite.children[k], args, 1);
               if (strcmp(edit_name,file_mgr_data->object_positions[i]->name)
                        == 0)
               {
                  /* Match */
                  XtUnmanageChild(file_window->composite.children[k]);
                  XtDestroyWidget(file_window->composite.children[k]);
                  break;
               }
            }
         }

         /* Free up the entry; bump up the other array entries */
         /* Update the linked list */
         if (file_mgr_data->object_positions[i]->prev)
         {
            file_mgr_data->object_positions[i]->prev->next = (ObjectPtr)
                file_mgr_data->object_positions[i]->next;
         }
         if (file_mgr_data->object_positions[i]->next)
         {
            file_mgr_data->object_positions[i]->next->prev = (ObjectPtr)
                file_mgr_data->object_positions[i]->prev;
         }
         sorder = file_mgr_data->object_positions[i]->stacking_order;
         XtFree(file_mgr_data->object_positions[i]->name);
         file_mgr_data->object_positions[i]->name = NULL;
         XtFree((char *)file_mgr_data->object_positions[i]);
         file_mgr_data->object_positions[i] = NULL;
         for (j = i; j < file_mgr_data->num_objects - 1; j++)
         {
            file_mgr_data->object_positions[j] =
                                   file_mgr_data->object_positions[j+1];
         }
         file_mgr_data->num_objects--;
         file_mgr_data->object_positions = (ObjectPosition **) XtRealloc(
               (char *)file_mgr_data->object_positions,
               sizeof(ObjectPosition *) * file_mgr_data->num_objects);
         CompressObjectList(file_mgr_data->object_positions,
                            file_mgr_data->num_objects, sorder);
      }
      else
         i++;
   }

   /* Now, it is safe to position any unplaced objects */
   for (i = 0; i < order_count; i++)
   {
      object = order_list[i];
      if (object->displayed)
      {
         position_data = GetPositionalData(file_mgr_data, object,
                                           file_window_width, True);

         /* Save; used later during redraw */
         position_data->file_view_data = object;
         object->position_info = (ObjectPtr)position_data;

         /* record position of bottom left corner for DisplaySomeIcons */
         object->x = position_data->x;
         object->y = position_data->y +
               (object->need_update? grid_height: object->widget->core.height);
      }
   }

   /* Get largest x value */
   largest_x = 0;
   largest_y = 0;
   bottom = GetBottomOfStack(file_mgr_data);
   while (bottom)
   {
      if (bottom->file_view_data != NULL && bottom->file_view_data->displayed)
      {
         if (bottom->file_view_data->position_info->x > largest_x)
            largest_x = bottom->file_view_data->position_info->x;
         if (bottom->file_view_data->position_info->y > largest_y)
            largest_y = bottom->file_view_data->position_info->y;
      }
      bottom = (ObjectPosition *)bottom->prev;
   }
   largest_x += grid_width;
   largest_y += grid_height;

   /* if necessary, shrink the width & height to what we need */
   if (current_wd == 32767)
   {
      current_wd = largest_x;
      set_size = True;
   }

   if (current_ht == 32767)
   {
      current_ht = largest_y;
      set_size = True;
   }

   /*  Get the horizontal and vertical scrollbars.  */
   XtSetArg (args[0], XmNscrollBarDisplayPolicy, XmAS_NEEDED);
   XtSetValues (file_mgr_rec->scroll_window, args, 1);

   /*
    * Typically, dtfile does everything possible to prevent a horizontal
    * scrollbar from being displayed.  However, when random placement is
    * enabled, we don't mind having a horizontal scrollbar, when needed.
    * We need to manually manage this, since the normal dtfile layout
    * code may have forcably unmanaged the scrollbar earlier.
    */
   if (XtIsManaged(file_mgr_rec->horizontal_scroll_bar))
   {
      /* get scroll bar values */
      (void)XmScrollBarGetValues(file_mgr_rec->horizontal_scroll_bar,
                                 &value, &size, &increment, &page);
   }

   if (VerticalScrollbarIsVisible(file_mgr_rec->vertical_scroll_bar,
                                  file_mgr_rec->scroll_window))
   {
      if ((Dimension)largest_x >= (Dimension)(file_mgr_rec->scroll_window->core.width -
                          (file_mgr_rec->vertical_scroll_bar->core.width + 4)))
      {
         XtManageChild(file_mgr_rec->horizontal_scroll_bar);
      }
      else
      {
         /* set scroll bar values changing its position */
         if(value != 0 && XtIsManaged(file_mgr_rec->horizontal_scroll_bar))
            XtManageChild(file_mgr_rec->horizontal_scroll_bar);
         else
         {
            XtUnmanageChild (file_mgr_rec->horizontal_scroll_bar);
            XtSetArg (args[0], XmNx, 0);
            XtSetValues ((Widget)file_window, args, 1);
         }
      }
   }
   else if ((Dimension)largest_x >= (Dimension)(file_mgr_rec->scroll_window->core.width - 4))
      XtManageChild(file_mgr_rec->horizontal_scroll_bar);
   else
   {
      /* set scroll bar values changing its position */
      if(value != 0 && XtIsManaged(file_mgr_rec->horizontal_scroll_bar))
         XtManageChild(file_mgr_rec->horizontal_scroll_bar);
      else
      {
         XtUnmanageChild (file_mgr_rec->horizontal_scroll_bar);
         XtSetArg (args[0], XmNx, 0);
         XtSetValues ((Widget)file_window, args, 1);
      }
   }

   /*  Set the file window width and height to be at least  */
   /*  the size of the scrolled window.                     */
   if ((Dimension)(current_wd) < (Dimension)(file_mgr_rec->scroll_window->core.width - 4))
   {
      if (VerticalScrollbarIsVisible(file_mgr_rec->vertical_scroll_bar,
                                     file_mgr_rec->scroll_window))
         current_wd = file_mgr_rec->scroll_window->core.width -
                           (file_mgr_rec->vertical_scroll_bar->core.width + 4);
      else
         current_wd = file_mgr_rec->scroll_window->core.width - 4;
      set_size = True;
   }

   if ((Dimension)(current_ht) < (Dimension)(file_mgr_rec->scroll_window->core.height - 4))
   {
      current_ht = file_mgr_rec->scroll_window->core.height - 4;
      set_size = True;
   }

   if (set_size)
   {
      XtSetArg (args[0], XmNwidth, current_wd);
      XtSetArg (args[1], XmNheight, current_ht);
      XtSetValues (file_mgr_rec->file_window, args, 2);
   }

   /*  Set the vertical scrollbar's increment to icon height  */
   XtSetArg (args[0], XmNincrement, file_mgr_data->grid_height + 2);
   XtSetValues (file_mgr_rec->vertical_scroll_bar, args, 1);

   /*
    * Don't keep up the hourglass; this helps the user to get the impression
    * that all of the work is done.
    */
   if (turn_off_hourglass)
      _DtTurnOffHourGlass(file_mgr_rec->shell);
}


/***************************************************
 *
 * Given a desktop object, see if it has already been assigned a position;
 * if not, then try to fit it into the next available grid position.
 *
 **************************************************/

static Boolean
IntersectRects(
        Position x1,
        Position y1,
        Dimension w1,
        Dimension h1,
        Position x2,
        Position y2,
        Dimension w2,
        Dimension h2 )

{
   int srcABot, srcBBot;
   int srcARight, srcBRight;
   int dw, dh;
   int dx, dy;

   srcABot = y1 + h1 - 1;
   srcBBot = y2 + h2 - 1;
   srcARight = x1 + w1 - 1;
   srcBRight = x2 + w2 - 1;

   if (x1 >= x2)
      dx = x1;
   else
      dx = x2;

   if (y1 > y2)
      dy = y1;
   else
      dy = y2;

   if (srcARight >= srcBRight)
      dw = srcBRight - dx + 1;
   else
      dw = srcARight - dx + 1;

   if (srcABot > srcBBot)
      dh = srcBBot - dy + 1;
   else
      dh = srcABot - dy + 1;

   if (dw <= 0 || dh <= 0)
      return(FALSE);

   return(TRUE);
}

static ObjectPosition *
GetPositionalData (
   FileMgrData * file_mgr_data,
   FileViewData * object,
   Dimension max_width,
   Boolean create)
{
   int i;
   int k;
   ObjectPosition *entry, *top;
   Position x;
   Position y;
   Dimension grid_height;
   Dimension grid_width;
   Boolean hit;
   ObjectPosition * oP;
   Dimension objWidth, objHeight;
   Dimension oWidth, oHeight;

   /* If object already has positional data, then use it */
   for (i = 0; i < file_mgr_data->num_objects; i++)
   {
      if (strcmp(object->file_data->file_name,
                 file_mgr_data->object_positions[i]->name) == 0)
      {
         /* Found a match */
         file_mgr_data->object_positions[i]->in_use = True;
         return(file_mgr_data->object_positions[i]);
      }
   }

   if (!create)
      return(NULL);

   /* Create a new entry, and place into the grid on the top of the stack */
   entry = (ObjectPosition *) XtMalloc(sizeof(ObjectPosition));
   entry->name = XtNewString(object->file_data->file_name);
   entry->in_use = True;
   entry->late_bind = False;
   entry->stacking_order = 2;
   entry->prev = NULL;
   entry->next = NULL;
   entry->file_view_data = NULL;

   if(file_mgr_data->num_objects == 1)
   {
      top = file_mgr_data->object_positions[0];
      top->next = (ObjectPtr)entry;
      entry->prev = (ObjectPtr)top;
   }
   else
   {
      top = NULL;
      /* Push all other objects down in the stack */
      for (i = 0; i < file_mgr_data->num_objects; i++)
      {
         /* Find the previous top of the stack */
         if (file_mgr_data->object_positions[i]->stacking_order == 1)
         {
            top = file_mgr_data->object_positions[i];
            continue;
         }
         else if (file_mgr_data->object_positions[i]->stacking_order == 2)
         {
            if(top == NULL)
               top = GetTopOfStack(file_mgr_data);
            top->next = (ObjectPtr)entry;
            entry->next = (ObjectPtr)file_mgr_data->object_positions[i];
            entry->prev = (ObjectPtr)top;
            file_mgr_data->object_positions[i]->prev = (ObjectPtr)entry;
         }
         file_mgr_data->object_positions[i]->stacking_order++;
      }
   }

   grid_height = file_mgr_data->grid_height;
   grid_width = file_mgr_data->grid_width;
   x = MARGIN;
   y = MARGIN;

   if (object->widget && !object->need_update)
   {
      objWidth = object->widget->core.width;
      objHeight = object->widget->core.height;
   }
   else
   {
      objWidth = grid_width;
      objHeight = grid_height;
   }

   /* Find the first open spot, which will not cause any overlap */
   do
   {
      hit = False;

      for (i = 0; i < file_mgr_data->num_objects; i++)
      {
         oP = file_mgr_data->object_positions[i];
         if (oP->file_view_data->widget && !oP->file_view_data->need_update)
         {
            oWidth = oP->file_view_data->widget->core.width;
            oHeight = oP->file_view_data->widget->core.height;
         }
         else
         {
            oWidth = grid_width;
            oHeight = grid_height;
         }
         if (oP->file_view_data->displayed &&
             IntersectRects(x, y, objWidth, objHeight,
                            oP->x, oP->y, oWidth, oHeight))
         {
            /* Try next grid spot */
            x += grid_width + XSPACING;

            if ((Dimension)(x + objWidth) >= max_width)
            {
               /* Go to next row */
               y += grid_height + YSPACING(file_mgr_data);
               x = MARGIN;
            }
            hit = True;
            break;
         }
      }
   } while (hit);

   /* Add to the end of the list */
   entry->x = x;
   entry->y = y;
   file_mgr_data->num_objects++;
   file_mgr_data->object_positions = (ObjectPosition **) XtRealloc(
                   (char *)file_mgr_data->object_positions,
                   sizeof(ObjectPosition *) * file_mgr_data->num_objects);

   /* Force the ordered list to be maintained */
   for (i = 0; i < file_mgr_data->num_objects - 1; i++)
   {
      if ((entry->y < file_mgr_data->object_positions[i]->y) ||
          ((entry->y == file_mgr_data->object_positions[i]->y) &&
           (entry->x < file_mgr_data->object_positions[i]->x)))
      {
         /* Fits here; slide later entries down */
         for (k = file_mgr_data->num_objects - 1; k > i; k--)
         {
            file_mgr_data->object_positions[k] =
                file_mgr_data->object_positions[k-1];
         }

         break;
      }
   }

   file_mgr_data->object_positions[i] = entry;
   return(file_mgr_data->object_positions[i]);
}


/*******************************************************************
 *
 *  BuildObjectPositons - builds up the object positions for directories
 *    which have now object position information.
 *
 *********************************************************************/
static void
BuildObjectPositions(
   FileMgrData *file_mgr_data)
{
   int i, j, k, l;
   FileViewData * file_view_data;
   ObjectPosition * ptr;

   file_mgr_data->object_positions = (ObjectPosition **)XtMalloc(
               sizeof(ObjectPosition *) *
               file_mgr_data->directory_set[0]->file_count);

   for (i = 0, j = 0; i < file_mgr_data->directory_set[0]->file_count; i++)
   {
      file_view_data = file_mgr_data->directory_set[0]->order_list[i];

      if (file_view_data->filtered)
         continue;

      ptr = (ObjectPosition *) XtMalloc(sizeof(ObjectPosition));
      ptr->name = XtNewString(file_view_data->file_data->file_name);
      ptr->x = file_view_data->x;
      ptr->y = file_view_data->y -
            (file_view_data->need_update? file_mgr_data->grid_height:
                                          file_view_data->widget->core.height);
      ptr->in_use = True;
      ptr->late_bind = False;
      ptr->stacking_order = j+1;
      ptr->file_view_data = file_view_data;
      file_view_data->position_info = (ObjectPtr)ptr;
      ptr->next = NULL;
      ptr->prev = NULL;

      /* Sort according to left-to-right, top-to-bottom */
      for (k = 0; k < j; k++)
      {
         if ((ptr->y < file_mgr_data->object_positions[k]->y) ||
             ((ptr->y == file_mgr_data->object_positions[k]->y) &&
              (ptr->x < file_mgr_data->object_positions[k]->x)))
         {
            /* Shift others down, to open up a spot */
            for (l = j; l > k; l--)
            {
               file_mgr_data->object_positions[l] =
                     file_mgr_data->object_positions[l - 1];
            }
            break;
         }
      }
      file_mgr_data->object_positions[k] = ptr;
      j++;
   }

   file_mgr_data->num_objects = j;

   /* Repair all of the next and prev pointers */
   RepairStackingPointers(file_mgr_data);
   OrderChildrenList(file_mgr_data);
}


/***********************************
 *
 * When items are dragged around on a random placement window, they will be
 * repositioned at the point where they were dropped.  Multiple drop items
 * will be positioned 'around' where the drop occurred.
 *
 **********************************/

void
RepositionIcons (
   FileMgrData * file_mgr_data,
   char ** file_list,
   int file_count,
   Position drop_x,
   Position drop_y,
   Boolean late_binding_needed)

{
   Position x;
   Position y;
   int i, i2, j, k, l;
   int adj;
   Position orig_x;
   Position orig_y;
   ObjectPosition * save_object;
   char * name;
   FileMgrRec * file_mgr_rec;
   XmManagerWidget file_window;
   Widget textWidget;
   char * edit_name;
   Arg args[10];
   Boolean newEntry;
   int newEntryCount = 0;
   char dot_dir[3] = ".";
   char dotdot_dir[3] = "..";

   /* if random placement is not enabled, positioning doesn't matter */
   if(file_mgr_data->positionEnabled == RANDOM_OFF)
      return;

   file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;

   /*
    * For directory views, no positional data may be present if positioning
    * was disabled for this view.  So ... we must create some now.
    */
   if ((file_mgr_data->num_objects == 0) &&
       (file_mgr_data->directory_set[0]->file_count > 0))
   {
         BuildObjectPositions(file_mgr_data);
   }

   /*
    * Update to the new position.
    * The position needs to be adjusted so that the icon actually ends
    * up where the user dropped it.  This is because the drag icon includes
    * margins and shadow area, but not the highlight area of the icon gadget
    * that was dragged.  Coordinates drop_x, drop_y are the upper left corner
    * of the drag icon when it was dropped.  To convert to icon gadget
    * coordinates we need to subtract the highlight thickness.
    */
   if (file_mgr_data->layout_data != NULL)
     adj = ((IconLayoutData *)file_mgr_data->layout_data)->highlight;
   else
     adj = 0;
   x = drop_x - adj;
   y = drop_y - adj;
   file_window = (XmManagerWidget) file_mgr_rec->file_window;

   /* Process each of the dropped files */
   /* Go from bottom to top, to retain stacking order */
   for (i = file_count - 1; i >= 0; i--)
   {
      textWidget = NULL;

      /*
       * The names typically come in as complete paths; we only want
       * the filename portion.
       * fdt: this check will break for root (/)
       */
      if( strcmp(file_mgr_data->current_directory, file_list[i]) == 0)
         name = dot_dir;
      else if(strncmp(file_mgr_data->current_directory, file_list[i], strlen(file_list[i]) ) == 0 )
        name = dotdot_dir;
      else
      {
        if ((name = strrchr(file_list[i], '/')) == NULL)
          name = file_list[i];
        else
        {
          name++;

         /* Try to gracefully handle root (/) */
          if (*name == '\0')
            name--;
        }
      }

      newEntry = True;

      /* Find the positional data for this object, if any */
      for (j = 0; j < file_mgr_data->num_objects; j++)
      {
         if (strcmp(name, file_mgr_data->object_positions[j]->name) == 0)
         {
            /* Find the associated widget */
            for (k = 0; strcmp(name,
             file_mgr_data->directory_set[0]->file_view_data[k]->file_data->                    file_name); k++);

            newEntry = False;

            /* If this object has a text field, then move it also */
            for (l = 0; l < file_window->composite.num_children; l++)
            {
               if (XmIsTextField(file_window->composite.children[l]) &&
                   !file_window->composite.children[l]->core.being_destroyed)
               {
                  XtSetArg(args[0], XmNuserData, &edit_name);
                  XtGetValues(file_window->composite.children[l], args, 1);
                  if (strcmp(edit_name, name) == 0)
                  {
                     textWidget = file_window->composite.children[l];
                     break;
                  }
               }
            }
            break;
         }
      }

      if (newEntry)
      {
         ObjectPosition * data;

         /* Create positional data at the top of the ordered array */
         newEntryCount++;
         j = 0;
         data = (ObjectPosition *) XtMalloc(sizeof(ObjectPosition));
         data->name = XtNewString(name);
         data->x = 0;
         data->y = 0;
         data->in_use = True;
         data->late_bind = True;
         data->stacking_order = 1;
         data->file_view_data = NULL;
         data->prev = NULL;
         data->next = NULL;

         /* Push all other objects down in the stack */
         for (i2 = 0; i2 < file_mgr_data->num_objects; i2++)
         {
            /* Find the previous top of the stack */
            if (file_mgr_data->object_positions[i2]->stacking_order == 1)
            {
               data->next = (ObjectPtr)file_mgr_data->object_positions[i2];
               file_mgr_data->object_positions[i2]->prev = (ObjectPtr)data;
            }
            file_mgr_data->object_positions[i2]->stacking_order++;
         }

         file_mgr_data->num_objects++;
         file_mgr_data->object_positions = (ObjectPosition **) XtRealloc(
               (char *)file_mgr_data->object_positions,
               sizeof(ObjectPosition *) * file_mgr_data->num_objects);

         /* Add entry to the top of the ordered array */
         for (j = file_mgr_data->num_objects - 1; j > 0; j--)
         {
            file_mgr_data->object_positions[j] =
                    file_mgr_data->object_positions[j - 1];
         }

         file_mgr_data->object_positions[0] = data;
      }
      else
      {
         /* Move item upto top of the stack */
         RepositionUpInStack(file_mgr_data,
                     file_mgr_data->object_positions[j]->stacking_order, 1);
      }

      orig_x = file_mgr_data->object_positions[j]->x;
      orig_y = file_mgr_data->object_positions[j]->y;
      file_mgr_data->object_positions[j]->x = x;
      file_mgr_data->object_positions[j]->y = y;
      save_object = file_mgr_data->object_positions[j];

      /* Move the object */
      if (!newEntry)
      {
         /* We need to force a geometry request */
         XtSetArg(args[0], XmNx, x);
         XtSetArg(args[1], XmNy, y);
         XtSetValues(file_mgr_data->directory_set[0]->file_view_data[k]->widget,
                     args, 2);

         /* Move associated text widget, if there is one */
         if (textWidget)
         {
            Position t_x, t_y;

            t_x = textWidget->core.x + x - orig_x;
            t_y = textWidget->core.y + y - orig_y;

            /* We need to force a geometry request */
            XtSetArg(args[0], XmNx, t_x);
            XtSetArg(args[1], XmNy, t_y);
            XtSetValues(textWidget, args, 2);
         }
      }

      /*
       * Reorder the positional array, so that it remains ordered.
       * Bubble down toward the end of the list if the object has moved
       * farther from the origin; bubble up if it has moved closer to
       * the origin.
       */
      if ((y > orig_y) || ((y == orig_y) && (x > orig_x)))
      {
         /* Bubble down */
         for (k = j + 1; k < file_mgr_data->num_objects; k++)
         {
            if ((y < file_mgr_data->object_positions[k]->y) ||
                ((y == file_mgr_data->object_positions[k]->y) &&
                 (x < file_mgr_data->object_positions[k]->x)))
            {
               /* We fit right here */
               file_mgr_data->object_positions[k - 1] = save_object;
               break;
            }
            else
            {
               /* Bubble */
               file_mgr_data->object_positions[k - 1] =
                     file_mgr_data->object_positions[k];
            }
         }

         /* See if it goes at the end */
         if (k == file_mgr_data->num_objects)
            file_mgr_data->object_positions[k - 1] = save_object;
      }
      else
      {
         /* Bubble up */
         for (k = j - 1; k >= 0; k--)
         {
            if ((y > file_mgr_data->object_positions[k]->y) ||
                ((y == file_mgr_data->object_positions[k]->y) &&
                 (x > file_mgr_data->object_positions[k]->x)))
            {
               /* We fit right here */
               file_mgr_data->object_positions[k + 1] = save_object;
               break;
            }
            else
            {
               /* Bubble */
               file_mgr_data->object_positions[k + 1] =
                     file_mgr_data->object_positions[k];
            }
         }

         /* See if it goes at the end */
         if (k < 0)
            file_mgr_data->object_positions[0] = save_object;
      }

      /* Create position for the next file to be processed */
      x += 20;
      y += 20;
   }
   /*
    * Reregister the desktop hotspots.
    * Even if the caller told us that late binding was needed, if no new
    * objects were specified, then we need to register hotspots now, because
    * the layout function will never be called because the directory never
    * really changed.  This situation can occur when an icon is dropped on
    * the desktop from a regular dtfile view, but that icon is already on
    * the desktop (it thus is just a reposition).
    */
   if (!late_binding_needed ||
       (late_binding_needed && (newEntryCount == 0)))
   {
      RegisterDesktopHotspots(file_mgr_data, file_mgr_rec);
   }
}

void
RegisterDesktopHotspots (
   FileMgrData * file_mgr_data,
   FileMgrRec  * file_mgr_rec)
{
   XmManagerWidget file_window;
   FileViewData  * file_view_data;
   int ex, ey, ewd, eht;
   ObjectPtr top;
   Widget icon;


   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   ex = -file_window->core.x;
   ey = -file_window->core.y;
   ewd = XtParent(file_window)->core.width;
   eht = XtParent(file_window)->core.height;

   /* Set up the icon location information with the drag handler  */
   /* Register in top to bottom stacking order */
   top = GetTopOfStack(file_mgr_data);

   while (top)
   {
      file_view_data = top->file_view_data;

      if (file_view_data != NULL &&
          file_view_data->displayed &&
          !file_view_data->need_update)
      {
         icon = file_view_data->widget;
         if ((Position)(icon->core.x + icon->core.width) >= (Position)ex  &&
             icon->core.x < ex + ewd &&
             (Position)(icon->core.y + icon->core.height) >= (Position)ey &&
             icon->core.y < ey + eht)
         {
            SetHotRects(file_view_data,
                        (XtCallbackProc) DropOnObject,
                        (XtPointer) file_view_data);
         }
      }

      top = top->next;
   }
}


/*
 * Dtfile used to try to determine if the vertical scrollbar was visible
 * by checking to see if it was managed; the assumption here was that the
 * scrolled window widget unmanaged the scrollbar, when it was not needed.
 * Unfortunately, it turns out that instead of unmanaging the scrollbar,
 * the scrolled window simply moves the scrollbar out of sight; it is moved
 * such that the X for the scrollbar is the same as the width of the
 * scrolled window.  So ... in order for us to really determine if the
 * scrollbar is visible, we need to see whether or not its X falls within
 * the visible area of the scrolled window.
 */

Boolean
VerticalScrollbarIsVisible(
        Widget vertSB,
        Widget scrolledWin)

{
   if (vertSB && XtIsManaged(vertSB) &&
       (vertSB->core.x < (Position)scrolledWin->core.width))
   {
      return(True);
   }

   return(False);
}

Boolean
HorizontalScrollbarIsVisible(
        Widget hortSB,
        Widget scrolledWin)

{
   if (hortSB && XtIsManaged(hortSB) &&
       (hortSB->core.y <= (Position)scrolledWin->core.height))
   {
      return(True);
   }

   return(False);
}
