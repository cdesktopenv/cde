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
/* $XConsortium: SharedProcs.c /main/7 1996/10/03 14:01:42 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           SharedProcs.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Contains the set of functions which are of general
 *                   use to all DT clients.
 *
 *   FUNCTIONS: BuildBufferFileName
 *		RenameCollisions
 *		RenameEntry
 *		RetrieveAndUseNameTemplateInfo
 *		SetBufferFileNames
 *		_DtAddOneSubdialog
 *		_DtBuildActionArgsWithDroppedBuffers
 *		_DtBuildActionArgsWithDroppedFiles
 *		_DtBuildActionArgsWithSelectedFiles
 *		_DtBuildActionArgsWithDTSelectedFiles
 *		_DtBuildFMTitle
 *		_DtBuildPath
 *		_DtChangeTildeToHome
 *		_DtCheckAndFreePixmapData
 *		_DtCheckForDataTypeProperty
 *		_DtCompileActionVector
 *		_DtCopyDroppedFileInfo
 *		_DtDestroySubdialog
 *		_DtDestroySubdialogArray
 *		_DtDuplicateDialogNameList
 *		_DtFollowLink
 *		_DtFreeActionArgs
 *		_DtFreeDroppedBufferInfo
 *		_DtFreeDroppedFileInfo
 *		_DtGenericDestroy
 *		_DtGenericMapWindow
 *		_DtGenericUpdateWindowPosition
 *		_DtGetSelectedFilePath
 *		_DtHideOneSubdialog
 *		_DtIsBufferExecutable
 *		_DtLoadSubdialogArray
 *		_DtMappedCB
 *		_DtPName
 *		_DtPathFromInput
 *		_DtResolveAppManPath
 *		_DtRetrieveDefaultAction
 *		_DtRetrievePixmapData
 *		_DtSaveSubdialogArray
 *		_DtSetDroppedBufferInfo
 *		_DtSetDroppedFileInfo
 *		_DtSpacesInFileNames
 *		_DtStringsAreEquivalent
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <ctype.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/VendorSEP.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
/* Copied from Xm/BaseClassI.h */
extern XmWidgetExtData _XmGetWidgetExtData( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */

#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Dts.h>
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>
#include <Dt/Action.h>
#include <Dt/Dnd.h>

#include <Tt/tttk.h>

#include "Encaps.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Common.h"
#include "Main.h"
#include "SharedProcs.h"

extern char *pathcollapse();


/* Defines */
#define RW_ALL S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH


/* Global controlling whether auto-positioning is enabled */
Boolean disableDialogAutoPlacement = False;


/********    Static Function Declarations    ********/

static void SetBufferFileNames (
                        char **file_set,
                        DtDndBuffer *buffers,
                        int num_of_buffers) ;
static char * BuildBufferFileName (
                        char *file_name,
                        int postfix_index,
                        void *buffer,
                        int buffer_size) ;
static char * RetrieveAndUseNameTemplateInfo(
                        void *buffer,
                        int buffer_size,
                        char *template_input) ;
static void RenameEntry(
                        char **name,
                        unsigned int addIndex) ;
static void RenameCollisions(
                        char **list,
                        int count) ;

/********    End Static Function Declarations    ********/


/************************************************************************
 *
 *  _DtPName
 *      Returns the parent directory of its argument.
 *      Does this by looking for the last "/" in the name, and
 *      NULL'ing it out.
 *
 ************************************************************************/

char *
_DtPName(
        register char *name )

{
   register char * q;
   static char pnam[MAXPATHLEN];
   static char dot[] = ".";

   strcpy(pnam, name);
   q = strrchr(pnam, '/');

   if (q == NULL)
      q = pnam;
   else if ((q == pnam) &&
#ifdef NLS16
            (mblen(q, MB_CUR_MAX) == 1) &&
#endif
            (*q == '/'))
   {
      q++;
   }

   *q = '\0';

   return(pnam[0] ? pnam : dot);
}


/*
 * Given a filename, follow it to the end of its link.  Returns NULL if
 * a recursive symbolic link is found.
 *
 * Note, the path returned is a static buffer, and should not be freed by
 * the calling function.
 */

char *
_DtFollowLink (
   char * path)
{
   char link_path[MAXPATHLEN];
   static char file[MAXPATHLEN];
   int link_len;
   char * end;
   int history_count;
   int history_size;
   char ** history;
   int i;

   /* Used to check for symbolic link loops */
   history_count = 0;
   history_size = 100;
   history = (char **)XtMalloc(sizeof(char *) * history_size);

   strcpy(file, path);

   while ((link_len = readlink(file, link_path, MAXPATHLEN)) > 0)
   {
      link_path[link_len] = '\0';

      /* Force the link to be an absolute path, if necessary */
      if (link_path[0] != '/')
      {
         /* Relative paths are relative to the current directory */
         end = strrchr(file, '/') + 1;
         *end = '\0';
         strcat(file, link_path);
      }
      else
         strcpy(file, link_path);

      /* Check for a recursive loop; abort if found */
      for (i = 0; i < history_count; i++)
      {
         if (strcmp(file, history[i]) == 0)
         {
            for (i = 0; i < history_count; i++)
               XtFree(history[i]);
            XtFree((char *)history);
            history = NULL;
            return(NULL);
         }
      }

      /* Add to the history list */
      if (history_count >= history_size)
      {
         history_size += 100;
         history = (char **)XtRealloc((char *)history,
                                      sizeof(char *) * history_size);
      }
      history[history_count++] = XtNewString(file);
   }

   /* Free up the history list */
   for (i = 0; i < history_count; i++)
      XtFree(history[i]);
   XtFree((char *)history);
   history = NULL;

   return(file);
}


/************************************************************************
 *
 *  _DtStringsAreEquivalent
 *      Compare two strings and return true if equal.
 *      The comparison is on lower cased strings.  It is the callers
 *      responsibility to ensure that test_str is already lower cased.
 *
 ************************************************************************/

Boolean
_DtStringsAreEquivalent(
        register char *in_str,
        register char *test_str )
{
#ifdef NLS16
   wchar_t c1;
   wchar_t c2;
   int c1size, c2size;
#endif
   register int i;
   register int j;

#ifdef NLS16
   if (is_multibyte)
   {
      for (;;)
      {
         /* Convert each character from multibyte to wide format */
         c1size = mbtowc(&c1, in_str, MB_CUR_MAX);
         c2size = mbtowc(&c2, test_str, MB_CUR_MAX);

         /* No match, if the two characters have different byte counts */
         if (c1size != c2size)
            return(False);

         /* Do case conversion only for single byte characters */
         if (c1size == 1)
         {
            if (isupper((int) c1))
               c1 = tolower((int) c1);
         }

         /* See if the two wide characters match */
         if (c1 != c2)
            return(False);

         /* Are we at the end of the string? */
         if (c1 == '\0')
            return(True);

         /* Keep comparing */
         in_str += c1size;
         test_str += c2size;
      }
   }
   else
#endif
   {
      for (;;)
      {
         i = *in_str;
         j = *test_str;

         if (isupper (i)) i = tolower (i);
         if (i != j) return (False);
         if (i == 0) return (True);

         in_str++;
         test_str++;
      }
   }
}



void
_DtDuplicateDialogNameList (
        char ** nameList,
        char ** newNameList,
        int newNameListSize,
        int * nameCount )

{
   int i;

   /*  Get a copy of the name list to be used to build new name lists.  */
   *nameCount = 0;
   while (nameList[*nameCount] != NULL)
   {
      newNameList[*nameCount] = nameList[*nameCount];
      (*nameCount)++;
   }

   /* NULL out any remaining array entries */
   for (i = (*nameCount); i < newNameListSize; i++)
      newNameList[i] = NULL;
}


void
_DtLoadSubdialogArray (
        char ** nameList,
        int    nameCount,
        DialogData *** dialogArray,
        int    dialogCount,
        int    dialogId,
        XrmDatabase dataBase,
        int    firstId )

{
   int i;
   char number[10];

   /* Load sub-dialogs */
   nameList[nameCount] = number;
   nameList[nameCount + 1] = NULL;

   /* Get text annotation dialogs */
   *dialogArray = (DialogData **) XtMalloc(sizeof(DialogData *) * dialogCount);

   for (i = 0; i < dialogCount; i++)
   {
      sprintf(number, "%d", firstId);
      (*dialogArray)[i] = _DtGetResourceDialogData(dialogId, dataBase, nameList);
      firstId++;
   }
}



void
_DtSaveSubdialogArray (
        char ** nameList,
        int    nameCount,
        DialogData ** dialogArray,
        int    dialogCount,
        int    fd,
        int    firstId )

{
   int i;
   char number[10];

   nameList[nameCount] = number;
   nameList[nameCount + 1] = NULL;

   for (i = 0; i < dialogCount; i++)
   {
      sprintf(number, "%d", firstId);
      _DtWriteDialogData(dialogArray[i], fd, nameList);
      firstId++;
   }
}


void
_DtDestroySubdialogArray (
        DialogData ** dialogArray,
        int    dialogCount )

{
   int i;

   for (i = 0; i < dialogCount; i++)
   {
      if (_DtIsDialogShowing(dialogArray[i]))
         _DtHideDialog(dialogArray[i], False);

      _DtFreeDialogData(dialogArray[i]);
   }
   XtFree((char *)dialogArray);
}


void
_DtDestroySubdialog (
        DialogData * dialogData )

{
   if (dialogData)
   {
      if (_DtIsDialogShowing(dialogData))
         _DtHideDialog(dialogData, False);
      _DtFreeDialogData(dialogData);
   }
}


void
_DtHideOneSubdialog (
        DialogData * dialogData,
        DialogData *** dialogArray,
        int         * dialogCountPtr )

{
   int i, j;

   for (i = 0; i < *dialogCountPtr; i++)
   {
      if (dialogData == (*dialogArray)[i])
      {
         for (j = i; j < (*dialogCountPtr) - 1; j++)
            (*dialogArray)[j] = (*dialogArray)[j + 1];
         break;
      }
   }

   /*  Free or decrease the size of the dialog data list  */
   (*dialogCountPtr)--;
   if (*dialogCountPtr == 0)
   {
      XtFree((char *)*dialogArray);
      *dialogArray = NULL;
   }
   else
   {
      *dialogArray = (DialogData **)
         XtRealloc((char *)*dialogArray,
                   sizeof(DialogData *) * (*dialogCountPtr));
   }
}


void
_DtAddOneSubdialog (
        DialogData * dialogData,
        DialogData *** dialogArray,
        int         * dialogCountPtr )

{
   int count;

   count = *dialogCountPtr;
   (*dialogCountPtr)++;
   (*dialogArray) = (DialogData **) XtRealloc((char *)(*dialogArray),
                                  sizeof(DialogData *) * (*dialogCountPtr));
   (*dialogArray)[count] = dialogData;
}


/*
 * This function acts as a frontend to the encapsulator's map callback
 * function.  If we are in the middle of a restore session, then we don't
 * want the map callback to alter the placement of the dialog, so we will
 * not call the encapsulator's function.
 */

void
_DtMappedCB(
         Widget w,
         XtPointer client_data,
         XtPointer call_data )
{
   if (!disableDialogAutoPlacement)
      _DtmapCB(w,client_data,call_data);

   XtRemoveCallback(w, XmNpopupCallback, (XtCallbackProc)_DtMappedCB, client_data);
}


/*
 * This is the generic function for registering the map callback.
 */

void
_DtGenericMapWindow (
     Widget parent,
     XtPointer recordPtr)
{
   GenericRecord * genericRecord = (GenericRecord *) recordPtr;

   XtAddCallback (genericRecord->shell, XmNpopupCallback,
                  (XtCallbackProc)_DtMappedCB, (XtPointer) parent);
}


/*
 * This is the generic function for destroying a dialog widget hierarchy.
 */

void
_DtGenericDestroy(
        XtPointer recordPtr )

{
   GenericRecord * genericRecord = (GenericRecord *) recordPtr;

   XtDestroyWidget(genericRecord->shell);
   XtFree((char *) genericRecord);
}


/*
 * This is the generic function for updating the shell's x and y, to
 * take the window manager border into consideration.
 */

void
_DtGenericUpdateWindowPosition(
        DialogData * dataPtr )

{
   DialogInstanceData * genericData = (DialogInstanceData *) dataPtr->data;
   GenericRecord * genericRecord;

   /* Do nothing, if the dialog is not displayed */
   if (genericData->displayed == True)
   {
      XmVendorShellExtObject vendorExt;
      XmWidgetExtData        extData;
      Window                 junkWindow;
      int                    t_x, t_y;
      Arg                    args[5];

      genericRecord = (GenericRecord *)_DtGetDialogInstance(dataPtr);

      XtSetArg(args[0], XmNwidth, &genericData->width);
      XtSetArg(args[1], XmNheight, &genericData->height);
      XtGetValues(genericRecord->shell, args, 2);

      XTranslateCoordinates(XtDisplay(genericRecord->shell),
                       XtWindow(genericRecord->shell),
                       RootWindowOfScreen(XtScreen(genericRecord->shell)),
                       0, 0, &t_x, &t_y,
                       &junkWindow);
      genericData->x = (Position) t_x;
      genericData->y = (Position) t_y;

      /* Modify x & y to take into account window mgr frames */
      extData=_XmGetWidgetExtData(genericRecord->shell, XmSHELL_EXTENSION);
      vendorExt = (XmVendorShellExtObject)extData->widget;
      genericData->x -= vendorExt->vendor.xOffset;
      genericData->y -= vendorExt->vendor.yOffset;
   }
}


/*
 * This is a function for building a path from directory and filename
 * parameters.
 */

void
_DtBuildPath(
        char         *path,
        char         *directory,
        char         *fileName)

{
  if (directory)
  {
    if (fileName)
      sprintf(path, "%s/%s", directory, fileName);
    else
      sprintf(path, "%s", directory);
  }
  else
  {
    if (fileName)
      sprintf(path, "%s", fileName);
    else
      sprintf(path, "%s", "");
  }
}


/*
 * This is a function for retrieving the pixmap data for a data type.
 */

PixmapData *
_DtRetrievePixmapData(
        char         *dataType,
        char         *fileName,
        char         *directory,
        Widget        shell,
        int           size)

{
  PixmapData *pixmapData;
  char path[MAXPATHLEN];
  char *iconName;

  pixmapData = (PixmapData *) XtMalloc(sizeof(PixmapData));
  if (!pixmapData)
    return NULL;

  path[0] = 0x0;
  _DtBuildPath(path, directory, fileName);

  pixmapData->size = size;

  /* retrieve host name */
  pixmapData->hostPrefix = DtDtsDataTypeToAttributeValue(dataType,
                                                         DtDTS_DA_DATA_HOST,
                                                         NULL);

  /*
     retrieve instance icon name if one exists; otherwise, retrieve class
     icon name
  */
  if (path[0] != 0x0)
  {
    pixmapData->instanceIconName = DtDtsDataTypeToAttributeValue(dataType,
                                            DtDTS_DA_INSTANCE_ICON,
                                            path);
  }
  else
    pixmapData->instanceIconName = NULL;

  if (pixmapData->instanceIconName == NULL)
  {
    pixmapData->iconName = DtDtsDataTypeToAttributeValue( dataType, 
                                                          DtDTS_DA_ICON, NULL);
    if( pixmapData->iconName == NULL )
    {
      if( strcmp( dataType, LT_DIRECTORY ) == 0 )
        pixmapData->iconName = XtNewString( "DtdirB" );
      else
        pixmapData->iconName = XtNewString( "Dtdeflt" );
    }
  }
  else
  {
    pixmapData->iconName = NULL;
  }

  /* retrieve icon file name */
  if (pixmapData->size == LARGE)
     pixmapData->iconFileName = _DtGetIconFileName(XtScreen(shell),
                                               pixmapData->instanceIconName,
                                               pixmapData->iconName,
                                               pixmapData->hostPrefix,
                                               DtMEDIUM);
  else
     pixmapData->iconFileName = _DtGetIconFileName(XtScreen(shell),
                                               pixmapData->instanceIconName,
                                               pixmapData->iconName,
                                               pixmapData->hostPrefix,
                                               DtTINY);

  /* return pixmap data */
  return(pixmapData);
}


/*
 * This is a function for checking the size, etc of an icon pixmap and
 * freeing the pixmap data for an icon.
 */

void
_DtCheckAndFreePixmapData(
        char         *dataType,
        Widget        shell,
        DtIconGadget  iconGadget,
        PixmapData   *pixmapData)

{
  Arg   args[1];

  if (!pixmapData)
    return;

  /*
     if there was an instance icon name; verify that the pixmap parameters
     do not exceed system limits; if system limits are exceeded, retrieve
     class icon name and set icon gadget image resource to this value
  */
  if (pixmapData->instanceIconName)
  {

    if(! pixmapData->iconFileName && iconGadget->icon.pixmap == 0)
    { /* Try to get pixmap name
      */
      char * tmp, * ptr;

      tmp = XtNewString( pixmapData->instanceIconName );
      if( ptr = strrchr( tmp,'/' ) )
        *(ptr) = 0;
      XmeFlushIconFileCache( tmp );

      if (pixmapData->iconFileName != NULL)
         XtFree(pixmapData->iconFileName);
      if (pixmapData->size == LARGE)
        pixmapData->iconFileName = _DtGetIconFileName( XtScreen(shell),
                                              pixmapData->instanceIconName,
                                              NULL,
                                              pixmapData->hostPrefix,
                                              DtMEDIUM );
      else
        pixmapData->iconFileName = _DtGetIconFileName(XtScreen(shell),
                                              pixmapData->instanceIconName,
                                              NULL,
                                              pixmapData->hostPrefix,
                                              DtTINY);



      XtSetArg(args[0], XmNimageName, pixmapData->iconFileName);
      XtSetValues((Widget) iconGadget, args, 1);

      XtFree( tmp );
    }

	

    DtDtsFreeAttributeValue(pixmapData->instanceIconName);

    if (iconGadget->icon.pixmap == 0                     ||
        iconGadget->icon.pixmap_width == 0               ||
        iconGadget->icon.pixmap_height == 0              ||
        (Dimension)iconGadget->icon.pixmap_width > (Dimension)instanceWidth    ||
        (Dimension)iconGadget->icon.pixmap_height > (Dimension)instanceHeight)
    {

      pixmapData->iconName = DtDtsDataTypeToAttributeValue(
                                             dataType, 
                                             DtDTS_DA_ICON, NULL);

      /* retrieve icon file name */
      if (pixmapData->iconFileName != NULL)
      {
         XtFree(pixmapData->iconFileName);
         pixmapData->iconFileName = NULL;
      }

      if (pixmapData->size == LARGE)
            pixmapData->iconFileName = _DtGetIconFileName(XtScreen(shell),
                                             pixmapData->instanceIconName,
                                             pixmapData->iconName,
                                             pixmapData->hostPrefix,
                                             DtMEDIUM);
      else
            pixmapData->iconFileName = _DtGetIconFileName(XtScreen(shell),
                                             pixmapData->instanceIconName,
                                             pixmapData->iconName,
                                             pixmapData->hostPrefix,
                                             DtTINY);



       XtSetArg(args[0], XmNimageName, pixmapData->iconFileName);
       XtSetValues((Widget) iconGadget, args, 1);

       DtDtsFreeAttributeValue(pixmapData->iconName);
    }
  }
  else
  {
     DtDtsFreeAttributeValue(pixmapData->iconName);
  }

  DtDtsFreeAttributeValue(pixmapData->hostPrefix);
  if (pixmapData->iconFileName)
     XtFree(pixmapData->iconFileName);

  XtFree((char *) pixmapData);
}


/*
 * This is a function for checking for a datatype property.
 */

Boolean
_DtCheckForDataTypeProperty(
        char *dataType,
        char *property)

{
   char *properties;
   char *prop;
   char *props;
   Boolean found = False;


   properties = DtDtsDataTypeToAttributeValue(dataType,
                                              DtDTS_DA_PROPERTIES,
                                              NULL);

   if (properties)
   {
     props = properties;
     prop = props;
     while (props = DtStrchr(props, ','))
     {
       *props = '\0';
       if (strcmp(prop, property) == 0)
       {
         found = True;
         break;
       }
       *props = ','; 
       props++;
       prop = props;
     }

     if (!props)
     {
       if (strcmp(prop, property) == 0)
         found = True;
     }

     DtDtsFreeAttributeValue(properties);
   }

   return found;
}


/*
 * This is a function for compiling a vectorized action list for a data type.
 */

char **
_DtCompileActionVector(
        char *dataType)

{
   char *actions;
   char **vector = NULL;

   actions = DtDtsDataTypeToAttributeValue(dataType,
                                           DtDTS_DA_ACTION_LIST,
                                           NULL);
   if (actions)
     vector = (char **) _DtVectorizeInPlace(actions, ',');

   return(vector);
}


/*
 * This is a generic function for retrieving the default action for a data
 * type.
 */

char *
_DtRetrieveDefaultAction(
        char *dataType)

{
   char *actions;
   char *acts;
   char *default_action = NULL;

   actions = DtDtsDataTypeToAttributeValue(dataType,
                                           DtDTS_DA_ACTION_LIST,
                                           NULL);
   if (actions)
   {
     if (acts = DtStrchr(actions, ','))
       *acts = '\0';
     default_action = XtNewString(actions);

     DtDtsFreeAttributeValue(actions);
   }

   return(default_action);
}


/*
 * This is a function for building a title for a File Manager view.
 */

char *
_DtBuildFMTitle(
        FileMgrData *file_mgr_data )

{
   char *title, *ptr, *fileLabel, *fileName;

   if (fileLabel = DtDtsFileToAttributeValue(file_mgr_data->current_directory,
                                             DtDTS_DA_LABEL))
      ptr = fileLabel;
   else if (fileName = strrchr(file_mgr_data->current_directory, '/'))
      ptr = fileName + 1;
   else
      ptr = "";

   if (file_mgr_data->title)
   {
      if (file_mgr_data->toolbox &&
          strcmp(file_mgr_data->current_directory,
                 file_mgr_data->restricted_directory) != 0)
      {
         title = (char *)XtMalloc(strlen(file_mgr_data->title) +
                                  strlen(ptr) +
                                  4); /* Need for blank dash blank NULL */
         sprintf(title, "%s - %s", file_mgr_data->title, ptr);
      }
      else
      {
         title = XtNewString(file_mgr_data->title);
      }
   }
   else
   {
      if (strcmp(file_mgr_data->current_directory, "/") == 0 && !fileLabel)
      {
         title = (char *)XtMalloc(strlen((GETMESSAGE(12, 7, "File Manager"))) +
                                  strlen(file_mgr_data->host) +
                                  strlen(root_title) +
                                  5); /* Need for blank dash blank colon NULL */
         sprintf( title, "%s - %s:%s", (GETMESSAGE(12, 7, "File Manager")),
                                       file_mgr_data->host,
                                       root_title );
      }
      else
      {
         title = (char *)XtMalloc(strlen((GETMESSAGE(12, 7, "File Manager"))) +
                                  strlen(ptr) +
                                  4); /* Need for blank dash blank NULL */
         sprintf( title, "%s - %s", (GETMESSAGE(12, 7, "File Manager")), ptr);
      }
   }

   DtDtsFreeAttributeValue(fileLabel);
   return(title);
}


/*
 * This is a function for building a path from the directory name
 * and file name pieces of a FileViewData structure.
 */

char *
_DtGetSelectedFilePath(
        FileViewData *selected_file )

{
   char *directory;
   char *file;
   char *path;

   directory = ((DirectorySet *)selected_file->directory_set)->name;
   file = selected_file->file_data->file_name;

   if (strcmp(directory, "/") == 0)
   {
      path = XtMalloc(strlen(directory) + strlen(file) + 1);
      sprintf(path, "%s%s", directory, file);
   }
   else
   {
      path = XtMalloc(strlen(directory) + strlen(file) + 2);
      sprintf(path, "%s/%s", directory, file);
   }

   return(path);
}


/*
 * This is a generic function for building action parameters from an array of
 * file view data structures.
 */

void
_DtBuildActionArgsWithSelectedFiles(
        FileViewData **selection_list,
        int selected_count,
        DtActionArg **action_args,
        int *arg_count )

{
   *arg_count = 0;

   *action_args = (DtActionArg *)
                     XtCalloc(1, sizeof(DtActionArg) * selected_count);

   if (*action_args)
   {
      int i;

      for(i = 0; i < selected_count; i++)
      {
         ((*action_args)[(*arg_count)]).argClass = DtACTION_FILE;
         ((*action_args)[(*arg_count)++]).u.file.name =
                         _DtGetSelectedFilePath(selection_list[i]);
      }
   }
}


void
_DtBuildActionArgsWithDTSelectedFiles(
        DesktopRec **selection_list,
        int selected_count,
        DtActionArg **action_args,
        int *arg_count )

{
   *arg_count = 0;

   *action_args = (DtActionArg *)
                     XtCalloc(1, sizeof(DtActionArg) * selected_count);

   if (*action_args)
   {
      int i;

      for(i = 0; i < selected_count; i++)
      {
         ((*action_args)[(*arg_count)]).argClass = DtACTION_FILE;
         ((*action_args)[(*arg_count)++]).u.file.name =
                  _DtGetSelectedFilePath(selection_list[i]->file_view_data);
      }
   }
}


/*
 * This is a generic function for building action parameters from drag and drop
 * file information.
 */

void
_DtBuildActionArgsWithDroppedFiles(
        FileViewData *dropped_on_obj,
        DtDndDropCallbackStruct *drop_parameters,
        DtActionArg **action_args,
        int *arg_count )

{
   *arg_count = 0;

   if (dropped_on_obj)
      *action_args = (DtActionArg *) XtCalloc
        (1, sizeof(DtActionArg) * (drop_parameters->dropData->numItems + 1));
   else
      *action_args = (DtActionArg *) XtCalloc
        (1, sizeof(DtActionArg) * drop_parameters->dropData->numItems);

   if (*action_args)
   {
      int i;

      if (dropped_on_obj)
      {
         ((*action_args)[(*arg_count)]).argClass = DtACTION_FILE;
         ((*action_args)[(*arg_count)++]).u.file.name =
                         _DtGetSelectedFilePath(dropped_on_obj);
      }

      for(i = 0; i < drop_parameters->dropData->numItems; i++)
      {
         ((*action_args)[(*arg_count)]).argClass = DtACTION_FILE;
         ((*action_args)[(*arg_count)++]).u.file.name =
                         XtNewString(drop_parameters->dropData->data.files[i]);
      }
   }
}


/*
 * This is a generic function for building action parameters from drag and drop
 * buffer information.
 */


void
_DtBuildActionArgsWithDroppedBuffers(
        FileViewData *dropped_on_obj,
        DtDndDropCallbackStruct *drop_parameters,
        DtActionArg **action_args,
        int *arg_count )

{
   *arg_count = 0;

   if (dropped_on_obj)
      *action_args = (DtActionArg *) XtCalloc
        (1, sizeof(DtActionArg) * (drop_parameters->dropData->numItems + 1));
   else
      *action_args = (DtActionArg *) XtCalloc
        (1, sizeof(DtActionArg) * drop_parameters->dropData->numItems);

   if (*action_args)
   {
      int i;
      DtActionBuffer buffer_arg = {NULL, 0, NULL, NULL, False};

      if (dropped_on_obj)
      {
         ((*action_args)[(*arg_count)]).argClass = DtACTION_FILE;
         ((*action_args)[(*arg_count)++]).u.file.name =
                         _DtGetSelectedFilePath(dropped_on_obj);
      }

      for(i = 0; i < drop_parameters->dropData->numItems; i++)
      {
         buffer_arg.bp = drop_parameters->dropData->data.buffers[i].bp;
         buffer_arg.size = drop_parameters->dropData->data.buffers[i].size;
         buffer_arg.name = drop_parameters->dropData->data.buffers[i].name;

         ((*action_args)[(*arg_count)]).argClass = DtACTION_BUFFER;
         ((*action_args)[(*arg_count)++]).u.buffer = buffer_arg;
      }
   }
}


/*
 * This is a generic function for freeing action parameters.
 */

void
_DtFreeActionArgs(
        DtActionArg *action_args,
        int arg_count )

{
   int i;

   for (i = 0; i < arg_count; i++)
   {
      if (action_args[i].argClass == DtACTION_FILE)
      {
         XtFree(action_args[i].u.file.name);
         action_args[i].u.file.name = NULL;
      }
   }
   
   XtFree((char *) action_args);
}


/************************************************************************
 * The following functions deal with Buffer Manipulation and Naming
 ************************************************************************/

/*
 * This is a generic function for extracting buffer information from
 * drop input.
 */

void
_DtSetDroppedBufferInfo(char **file_set,
              BufferInfo *buffer_set,
              char **host_set,
              DtDndDropCallbackStruct *drop_parameters)

{

 int num_of_buffers = drop_parameters->dropData->numItems;
 int i;
 DtDndBuffer *buffers = drop_parameters->dropData->data.buffers;


 DPRINTF (("Executing..._DtSetDroppedBufferInfo\n"));

 /* Initialize file_set and ensure of unique file names for unamed buffers*/
 SetBufferFileNames(file_set, buffers, num_of_buffers);

 for (i= 0; i < num_of_buffers; i++)
  {

     (buffer_set[i]).buf_ptr = buffers[i].bp;
     (buffer_set[i]).size    = buffers[i].size;

     host_set[i] = XtNewString(home_host_name);

     DPRINTF(("_DtSetDroppedBufferInfo:\n host_set[%d]=%s,\
              buffer_set[%d].buf_ptr=%p, buffer_set[%d].size=%d\n",
              i, host_set[i], i, buffer_set[i].buf_ptr, i, buffer_set[i].size));
  }
}


/*
 * This is a function for creating file names for a set of buffers.
 */

static void
SetBufferFileNames (char **file_set,
                    DtDndBuffer *buffers,
                    int num_of_buffers)
{
 int null_filenames_count = 0;
 int i;
 int first_nullfile_index = 0;
 Boolean NULL_FILENAMES=FALSE;

 DPRINTF (("Executing...SetBufferFileNames\n"));

 for (i = 0; i < num_of_buffers; i++)
 {
     if (buffers[i].name == NULL)
     {
       /* generate buffer name using Untitled as a base name */
       file_set[i]=BuildBufferFileName(DEFAULT_BUFFER_FILENAME,
                                       -1,
                                       buffers[i].bp,
                                       buffers[i].size);
     }
     else
     {
        /* file name is supplied by the drag initiator */
       file_set[i] = XtNewString(buffers[i].name);
     }

     DPRINTF(("file_set[%d]=%s\n", i, file_set[i]));

 } /* end for loop */


 /* Rename any collisions to unique names  */
 RenameCollisions(file_set, num_of_buffers);

 return;
}


/*
 * This is a generic function for generating a name for an untitled buffer.
 * A default name (Untitled) is used in conjunction with the name template
 * information from the types database.
 */

static char *
BuildBufferFileName (char   *file_name,
                     int     postfix_index,
                     void   *buffer,
                     int     buffer_size)
{
  const char delim = '_';
  char *new_file_name;


  DPRINTF (("Executing....BuildBufferFileName\n"));

  /* Malloc memory and contruct the new file name */
  new_file_name = (char *) XtMalloc (strlen(file_name) + 1 +
                                     MAX_POSTFIX_LENGTH  + 1);

  DPRINTF (("BuildBufferFileName: Old file name is %s\n", file_name));

  /* determine whether to append post fix name */
  if (postfix_index == -1)
     strcpy(new_file_name, file_name);
  else
     sprintf(new_file_name,"%s%c%d", file_name, delim, postfix_index);

  /* Retrieve the name template if it exists and use it in the filename */
  new_file_name = RetrieveAndUseNameTemplateInfo(buffer,
                                                 buffer_size,
                                                 new_file_name);

  DPRINTF(("BuildBufferFileName: Returning new_file_name=%s\n", new_file_name));

  /* return new file name */
  return (new_file_name);
}



/*
 * This is a function for building a buffer name using predfined input
 * and name template information from the types database.
 * WARNING: template_input will be freed. It must point to a char *.
 */

static char *
RetrieveAndUseNameTemplateInfo(
        void *buffer,
        int buffer_size,
        char *template_input)

{
   char *name_template;
   char *buffer_name = NULL;

   name_template = DtDtsBufferToAttributeValue(buffer,
                                               buffer_size,
                                               DtDTS_DA_NAME_TEMPLATE,
                                               NULL);
   if (name_template)
      buffer_name = (char *) XtMalloc(strlen(name_template) +
                                      strlen(template_input) +
                                       1);
   if (buffer_name)
   {
      sprintf(buffer_name, name_template, template_input);
      DtDtsFreeAttributeValue(name_template);
      XtFree(template_input);
      return(buffer_name);
   }
   else
   {
      return(template_input);
   }
}


/*
 * This is a function for resolving collisions in a list of buffer names.
 */

static void
RenameCollisions( char ** list, int count )
{
  register int i, j, k, l;
  char flg = 0, flg2 = 0;

  for( i = 0; i < count; ++i )
  {
    unsigned int addIndex = 1;

    if( *(list[i]) == 0x0 )
    {
      k = i;
      flg = 1;
    }

    for( j = i+1; j < count; ++j )
    {
      if( strcmp( list[i], list[j] ) == 0 )
      {
        RenameEntry( &(list[j]), ++addIndex );
        l = i;
        flg2 = 1;
      }
    }
    if( flg2 )
    {
      flg2 = 0;
      RenameEntry( &(list[l]), 1 );
    }
  }
  if( flg )
  {
    free( list[k] );
    list[k] = (char *)malloc( strlen( DEFAULT_BUFFER_FILENAME ) + 1 );
    sprintf( list[k], "%s", DEFAULT_BUFFER_FILENAME );
  }
}


/*
 * This is a function for adding an index to a buffer name which has
 * collided.
 */

static void
RenameEntry( char ** name, unsigned int addIndex )
{

#define MAX_INT_SIZE 15

char * tmpPtr, * newName;

  if( *name == 0x0 )
    return;
  else if( **name == 0x0 )
  {
    newName = (char *)XtCalloc(1,strlen(DEFAULT_BUFFER_FILENAME)+ MAX_INT_SIZE);
    sprintf( newName, "%s_%d", DEFAULT_BUFFER_FILENAME, addIndex );
  }
  else
  {
    tmpPtr = strrchr( *name, '.' );

    newName = (char *)XtCalloc( 1, strlen( *name ) + MAX_INT_SIZE );

    if( tmpPtr == NULL )
      sprintf( newName, "%s_%d", *name, addIndex );
    else if( tmpPtr == *name )
      sprintf( newName, "%d%s", addIndex, *name );
    else
    {
      *tmpPtr = 0x0;
      sprintf( newName, "%s_%d.%s", *name, addIndex, ++tmpPtr);
    }
  }
  free( *name );
  *name = newName;
}


/*
 * This is a generic function for freeing buffer information extracted from
 * drop input.
 */

void
_DtFreeDroppedBufferInfo(char **file_set,
              BufferInfo *buffer_set,
              char **host_set,
              int num_of_buffers)
{
  int i;

  DPRINTF (("Executing..._DtFreeDroppedBufferInfo\n"));

  /* Check for Null pointers */
  if (file_set && buffer_set && host_set )
  {
     for (i=0; i< num_of_buffers; i++ )
     {
       XtFree(file_set[i]);
       XtFree(host_set[i]);
     }

     XtFree((char *)file_set);
     XtFree((char *)host_set);
     XtFree((char *)buffer_set);
  }
}


/*
 * This is a generic function for determining if a buffer is executable.
 */

Boolean
_DtIsBufferExecutable(
        void *buffer,
        int buffer_size)

{
   char *exe_attribute;
   Boolean is_exe = False;

   exe_attribute = DtDtsBufferToAttributeValue(buffer,
                                               buffer_size,
                                               "IS_EXECUTABLE",
                                               NULL);
   if (exe_attribute)
   {
      if (DtDtsIsTrue(exe_attribute))
         is_exe = True;

      DtDtsFreeAttributeValue(exe_attribute);
   }

   return(is_exe);
}


/*
 * This is a generic function for extracting file information from drop input.
 */

void
_DtSetDroppedFileInfo(
        DtDndDropCallbackStruct *drop_parameters,
        char ***file_set,
        char ***host_set)
{
   int numFiles, i;

   numFiles = drop_parameters->dropData->numItems;

   *file_set = (char **)XtMalloc(sizeof(char *) * numFiles);
   *host_set = (char **)XtMalloc(sizeof(char *) * numFiles);
   for(i = 0; i < numFiles; i++)
   {
      (*file_set)[i] = XtNewString(drop_parameters->dropData->data.files[i]);
      (*host_set)[i] = home_host_name;

      DPRINTF(("ProcessMoveCopyLink:\n host_set[%d]=%s, file_set[%d]=%s\n",
               i, (*host_set)[i], i, (*file_set)[i]));
   }
}


/*
 * This is a generic function for copying file info extracted from drop input.
 */

void
_DtCopyDroppedFileInfo(
        int num_files,
        char **orig_file_set,
        char **orig_host_set,
        char ***new_file_set,
        char ***new_host_set)
{
   int i;

   *new_file_set = (char **)XtMalloc(sizeof(char *) * num_files);
   *new_host_set = (char **)XtMalloc(sizeof(char *) * num_files);
   for(i = 0; i < num_files; i++)
   {
      (*new_file_set)[i] = XtNewString(orig_file_set[i]);
      (*new_host_set)[i] = XtNewString(orig_host_set[i]);
   }
}


/*
 * This is a generic function for freeing file info extracted from drop input.
 */

void
_DtFreeDroppedFileInfo(
        int num_files,
        char **file_set,
        char **host_set)
{
   int i;

   for(i = 0; i < num_files; i++)
      XtFree(file_set[i]);

   XtFree((char *)file_set);
   XtFree((char *)host_set);
}


/*
 * This is a generic function for resolving a cannonical path from user input.
 */

void
_DtPathFromInput(
        char *input_string,
        char *current_dir,
        char **host,
        char **rel_path)

{
   char *path;
   char *tmp_path = NULL;
   char *true_path = NULL;
   Tt_status tt_status;
   int dir_len;

   /* find host */
   *host = XtNewString(home_host_name);

   /* find relative path */
   tmp_path = path = XtNewString(input_string);

   /* Strip any spaces from name -- input is overwritten */
   path = (char *) _DtStripSpaces(path);

   /* Resolve, if there're any, environment variables */
   {
      FILE *pfp;
      char command[MAXPATHLEN];

      sprintf(command,"echo %s",path);

      if((pfp=popen(command,"r")) != NULL)
      {
	  int read_ok = 1;

          if (NULL == (fgets(command,MAXPATHLEN,pfp))) 
	  {
	      /*
	       * Try a few more reads and if the read still fails, 
	       * just use the path as is.
	       */
	      int i;
	      for (i=0; i < 5; i++) 
	      {
	 	  sleep (1);
 		  if (NULL != (fgets(command,MAXPATHLEN,pfp))) 
		      break;
	     }
	     if (i >= 5)
		 read_ok = 0;
	 }

	 if (read_ok) 
	 {
             command[strlen(command)-1] = '\0';
             XtFree(path);
             path = XtNewString(command);
             pclose(pfp);
	 }
      }
   }

   /* Resolve '~' -- new memory is allocated, old memory is freed */
   if (*path == '~')
      path = _DtChangeTildeToHome(path);

   /* If current dir provided, check for relative path */
   if (path && current_dir)
   {
      if (*path != '/')
      {
         /* file is relative path i.e.      xyz/abc */
         if (strcmp(current_dir, "/") == 0)
         {
            tmp_path = (char *)XtMalloc(strlen(current_dir) + strlen(path) + 1);
            sprintf(tmp_path, "%s%s", current_dir, path);
         }
         else
         {
            tmp_path = (char *)XtMalloc(strlen(current_dir) + strlen(path) + 2);
            sprintf(tmp_path, "%s/%s", current_dir, path);
         }

         XtFree(path);
         path = tmp_path;
         tmp_path = NULL;
      }
   }
   else if (!path)
   {
     *rel_path = NULL;
     XtFree(tmp_path);
     return;
   }

   /* Resolve '.' or '..' -- input is overwritten, output may be NULL! */
   /* Save pointer to path to free if output is NULL.                  */
   tmp_path = path;
   path = (char *) DtEliminateDots(path);

   if (path)
   {
      /* Resolve to local pathname */
      true_path = ResolveLocalPathName(*host,
                                       path,
                                       NULL,
                                       home_host_name,
                                       &tt_status);
      XtFree(path);

      /* Strip off trailing '/' */
      dir_len = strlen(true_path);
      if (dir_len > 1 && *(true_path + dir_len - 1) == '/')
         *(true_path + dir_len - 1) = '\0';
   }
   else
   {
      true_path = NULL;
      XtFree(tmp_path);
   }

   *rel_path = true_path;
}


/*
 * This function takes a path name, and resolves any leading '~' to refer
 * to one of the following:
 *
 *   1) if ~ or ~/path, then it resolves to the user's home directory on
 *      their home host.
 *
 *   2) if ~user or ~user/path, then it resolves to the specified user's
 *      home directory on the home host.
 *
 * This function never resolves to any host but the home host, since we
 * have no way of determining a user's home directory on any system other
 * than the home system.
 */

char *
_DtChangeTildeToHome (
        char *input_string)
{
   char *path;
   char *full_path;
   struct passwd * pwInfo;

   if ((input_string[1] != '\0') && (input_string[1] != '/'))
   {
      char *path;

      /* ~user or ~user/path format */

      /* is there a path? */
      path = DtStrchr(input_string, '/');

      /* find user */
      if (path)
         *path = '\0';
      if ((pwInfo = getpwnam(input_string + 1)) == NULL)
      {
         /* user doesn't exist */
         if (path)
            *path = '/';
         return NULL;
      }

      if (path)
      {
         /* ~user/path format */

         *path = '/';

         if (strcmp(pwInfo->pw_dir, "/") == 0)
         {
            /* We don't want to end up with double '/' in the path */
            full_path = (char *) XtMalloc(strlen(path) + 1);
            strcpy(full_path, path);
         }
         else
         {
            full_path = (char *) XtMalloc(strlen(pwInfo->pw_dir) +
                                          strlen(path) + 1);
            sprintf(full_path, "%s%s", pwInfo->pw_dir, path);
         }
      }
      else
      {
         /* ~user format */

         full_path = XtMalloc(strlen(pwInfo->pw_dir) + 1);
         strcpy(full_path, pwInfo->pw_dir);
      }
   }
   else if (input_string[1])
   {
      /* ~/path format */

      /* NOTE: users_home_dir has trailing '/' */
      full_path = XtMalloc(strlen(users_home_dir) + strlen(input_string+2) + 1);
      sprintf(full_path, "%s%s", users_home_dir, (input_string + 2));
   }
   else
   {
      /* ~ format */

      full_path = XtMalloc(strlen(users_home_dir) + 1);
      strcpy(full_path, users_home_dir);
   }

   XtFree(input_string);
   return(full_path);
}


/*
 * This function checks for spaces in filenames.
 */
#ifdef _CHECK_FOR_SPACES

Boolean
_DtSpacesInFileNames(
         char **fileNames, 
         int  fileCount )
{
  int i;

  for (i = 0; i < fileCount; i++)
  {
     fileNames[i] = (char *) _DtStripSpaces(fileNames[i]);

     if (DtStrchr (fileNames[i], ' ') != NULL ||
         DtStrchr (fileNames[i], '\t') != NULL)
        return(TRUE);
  }

  return(FALSE);
}

#endif

/*
 * This resolves the path for an appmanager object to its true path.
 */

char *
_DtResolveAppManPath(
         char *path,
         char *restricted_dir )
{
   if (strlen(path) > strlen(restricted_dir))
   {
      char *linkDir, *ptr, *tmpPath;

      linkDir = path + strlen(restricted_dir) + 1;
      ptr = DtStrchr(linkDir, '/');
      if (ptr)
      {
         *ptr = '\0';
         linkDir = _DtFollowLink(path);
         *ptr = '/';
         tmpPath = XtMalloc(strlen(linkDir) + strlen(ptr) + 1);
         sprintf(tmpPath, "%s%s", linkDir, ptr);
         XtFree(path);
         path = tmpPath;
      }
      else
      {
         linkDir = _DtFollowLink(path);
         XtFree(path);
         path = XtNewString(linkDir);
      }
   }

   return(path);
}
