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
/* $XConsortium: cmnrtns.c /main/4 1995/11/01 16:11:08 rswiston $ */
/*****************************************************************************/
/*                                                                           */
/*  cmnrnts.c                                                                */
/*                                                                           */
/*   Common routines                                                         */
/*                                                                           */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Dt/Icon.h>
#include <X11/cursorfont.h>
#include "UxXt.h"
#include <libgen.h>
#include <Dt/HourGlass.h>

#include "dtcreate.h"
#include "CreateActionAppShell.h"
#include "fileio.h"
#include "cmnrtns.h"

/*****************************************************************************/
/*                                                                           */
/*  ReplaceSpaces                                                            */
/*                                                                           */
/*****************************************************************************/
char * ReplaceSpaces(char *pszName)
{
  char *newName;
  char *ptr;

  newName = (char *)XtMalloc(strlen(pszName) + 1);
  memset(newName, 0, strlen(pszName) + 1);
  strcpy(newName, pszName);
  ptr = strrchr(newName, ' ');
  if (ptr) {
     ptr = newName;
     while ((*ptr) && (ptr = strchr(ptr, ' '))) {
       *ptr = '_';
       ptr++;
     }
  }
  return(newName);
}

#if 0
/*****************************************************************************/
/*                                                                           */
/*  GetExtName                                                               */
/*                                                                           */
/*****************************************************************************/
char * GetExtName(char *pszFileName)
{
  char *type;

  type = strrchr(pszFileName, '.');
  if (type) {
     type = strtok(type, ".");
     return(type);
  } else {
     return(NULL);
  }
}
#endif

#if 0
/*****************************************************************************/
/*                                                                           */
/*  GetPathName                                                              */
/*                                                                           */
/*****************************************************************************/
char * GetPathName(char *pszFileName)
{
  char *ptr;
  char *pszPath;

  pszPath = (char *)malloc(strlen(pszFileName) + 1);
  strcpy(pszPath, pszFileName);
  ptr = strrchr(pszPath, '/');
  if (ptr) {
     *ptr = '\0';
  } else {
     free(pszPath);
     pszPath = (char *)NULL;
  }
  return(pszPath);
}
#endif

#if 0
/*****************************************************************************/
/*                                                                           */
/*  Change_IconGadget_IconType                                               */
/*                                                                           */
/*****************************************************************************/
void Change_IconGadget_IconType(Widget widIconGadget, char *pszNewType)
{
  char   *pszOldName;
  char   pszNewName[MAXBUFSIZE];
  char   *tmpptr;
  ushort rc;

  XtVaGetValues(widIconGadget, XmNimageName, &pszOldName, NULL);
  if (pszOldName) {
     tmpptr = strrchr(pszOldName, '.');
     *tmpptr = '\0';
     sprintf(pszNewName, "%s.%s", pszOldName, pszNewType);
#ifdef DEBUG
     printf("monochrome name is '%s'\n", pszNewName); /* debug */
#endif
     if (check_file_exists(pszNewName)) {
        SET_ICONGADGET_ICON(widIconGadget, pszNewName);
     } else {
        SET_ICONGADGET_ICON(widIconGadget, "");
     }
  }
}
#endif

#if 0
/*****************************************************************************/
/*                                                                           */
/*  Change_IconName_IconType                                                 */
/*                                                                           */
/*****************************************************************************/
char * Change_IconName_IconType(char *pszOldName, char *pszNewType)
{
  char   *tmpptr;
  ushort rc;
  char   *pszNewName = (char *)NULL;

  if (pszOldName) {
     pszNewName = (char *)malloc(strlen(pszOldName) + 10);
     tmpptr = strrchr(pszOldName, '.');
     *tmpptr = '\0';
     sprintf(pszNewName, "%s.%s", pszOldName, pszNewType);
#ifdef DEBUG
     printf("new icon name is '%s'\n", pszNewName);
#endif
     if (!check_file_exists(pszNewName)) {
        free(pszNewName);
        pszNewName = (char *)NULL;
     }
  }
  return(pszNewName);
}
#endif

/******************************************************************************/
/*                                                                            */
/* GetCoreName                                                                */
/*                                                                            */
/******************************************************************************/
char * GetCoreName(char *pszFullName)
{
  char *name;
  char *ptr;
  char *newName;

  name = strrchr(pszFullName, '/');
  if (name) {
     name = strtok(name, "/");
  } else {
     name = pszFullName;
  }
  newName = (char *)malloc(strlen(name) + 1);
  memset(newName, 0, strlen(name) + 1);
  strcpy(newName, name);
  ptr = strrchr(newName, '.');
  if (ptr) {
    *ptr = '\0';
  }
  return(newName);
}

/******************************************************************************/
/*                                                                            */
/* load_icons - puts selected icons into the appropriate icon gadget.         */
/*                                                                            */
/* INPUT: Widget wid - OK button on Open File dialog,                         */
/*        XtPointer client_data                                               */
/*        XmFileSelectionBoxCallbackStruct *cbs                               */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void load_icons (Widget wid, XtPointer client_data,
                 XmFileSelectionBoxCallbackStruct *cbs)
{
  char          *full_name, *path_and_base_name, *type_name, *size_name;
  char          *base_name;
  char          *ptr;
  XtArgVal /* int */     iSource;
  FiletypeData  *pFtD;

  /*****************************************/
  /* Get icon name and separate into parts */
  /*****************************************/
  full_name = (char *)client_data;
  path_and_base_name = (char *)malloc(strlen(full_name)+1);
  strcpy(path_and_base_name, full_name);

  /*****************************************/
  /* Strip off icon type extension.        */
  /*****************************************/
  ptr = strrchr(path_and_base_name, '.');
  if (ptr) {
     type_name = strtok(ptr, ".");
     *ptr = '\0';
  } else {
     type_name = (char *)NULL;
  }

  /*****************************************/
  /* Get size extention.                   */
  /*****************************************/
  ptr = strrchr(path_and_base_name, '.');
  if (ptr) {
     size_name = strtok(ptr, ".");
     *ptr = '\0';
  } else {
     size_name = (char *)NULL;
  }

#ifdef DEBUG
    printf("path&base = %s\n", path_and_base_name); /* debug */
    printf("type      = %s\n", type_name); /* debug */
    printf("size      = %s\n", size_name); /* debug */
#endif

  /* ***** cmvc 6715 *****
  if ((!path_and_base_name) || (!type_name) || (!size_name)) {
    printf ("'%s' is not a proper icon file name!\n", full_name);
  }
  else
  */
    {
    XtVaGetValues(IconSelector, XmNuserData, &iSource, NULL);
    base_name = basename(path_and_base_name);
    ptr = XtMalloc(strlen(base_name) + 1);
    strcpy(ptr, base_name);
    switch (iSource) {
      case CA_ACTION_ICONS:
           AD.pszIcon = ptr;
           SetIconData(CA_LRG_IconGadget, path_and_base_name, Large_Icon);
           SetIconData(CA_MED_IconGadget, path_and_base_name, Medium_Icon);
           SetIconData(CA_TINY_IconGadget, path_and_base_name, Tiny_Icon);
           break;
      case CA_FILETYPE_ICONS:
           XtVaGetValues(AddFiletype, XmNuserData, &pFtD, NULL);
           pFtD->pszIcon = ptr;
           SetIconData(AF_MED_IconGadget, path_and_base_name, Medium_Icon);
           SetIconData(AF_TINY_IconGadget, path_and_base_name, Tiny_Icon);
           break;
    }
  }
  free(path_and_base_name);
  return;
}

/******************************************************************************/
/*                                                                            */
/* GetWidgetTextString                                                        */
/*                                                                            */
/* INPUT: Widget wid - TextField widget to get string from.                   */
/*        Pointer to variable to store string                                 */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void GetWidgetTextString (Widget wid, char **ppszText)
{
  char *pszTmp;

  if (*ppszText) {
     XtFree(*ppszText);
     *ppszText = (char *)NULL;
  }
  if (XmIsTextField(wid)) {
     pszTmp = XmTextFieldGetString (wid);
  } else if (XmIsText(wid)) {
     pszTmp = XmTextGetString (wid);
  }

  if (pszTmp) {
     if (!strcmp(pszTmp, "")) {
        XtFree(pszTmp);
        pszTmp = (char *)NULL;
     }
     *ppszText = pszTmp;
  }
}

/******************************************************************************/
/*                                                                            */
/* PutWidgetTextString                                                        */
/*                                                                            */
/* INPUT: Widget wid - Widget whose text string is being set                  */
/*        Pointer to text string.                                             */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void PutWidgetTextString (Widget wid, char *pszText)
{
  if (pszText) {
     if (XmIsTextField(wid)) {
        XmTextFieldSetString (wid, pszText);
     } else if (XmIsText(wid)) {
        XmTextSetString (wid, pszText);
     }
  }
}

/******************************************************************************/
/*                                                                            */
/* GetIconSearchPathList                                                      */
/*                                                                            */
/* INPUT:  none                                                               */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
char **GetIconSearchPathList(void)
{
  char    *iconpath = (char *)NULL;
  char    *ptr;
  char    *tmpptr;
  char    *strip;
  char    *path;
  int     i;
  int     count;
  int     size;
  char    **pplist;
  char    *lang;
  int     langsize;
  static  char *default_list1[] = {"~/.dt/icons", "/etc/dt/appconfig/icons/C", "/usr/dt/appconfig/icons/C"};
  static  char *default_list2[] = {"/etc/dt/appconfig/icons/C", "/usr/dt/appconfig/icons/C"};
  char    **default_list;
  Boolean bFound;
  char    *pszEnvVar;

  /**************************************************************************/
  /* Get contents of icon search path environment variable.                 */
  /**************************************************************************/
  pszEnvVar = getenv("XMICONSEARCHPATH");
  if ( pszEnvVar && strlen(pszEnvVar) ) {
     iconpath = (char *)malloc(strlen(pszEnvVar) + 1);
     strcpy(iconpath, pszEnvVar);
  }

  /**************************************************************************/
  /* If no iconpath then return an appropriate default.                     */
  /**************************************************************************/
  if (!iconpath) {
     pszEnvVar = getenv("HOME");
     if (pszEnvVar && strlen(pszEnvVar)) {
        default_list = default_list1;
        count = sizeof(default_list1)/sizeof(void *);
     } else {
        default_list = default_list2;
        count = sizeof(default_list2)/sizeof(void *);
     }
     pplist = (char **)calloc((count + 1), sizeof(void *));
     for (i=0; i < count; i++) {
        if (strchr(default_list[i], '~')) {
           if (pszEnvVar && strlen(pszEnvVar)) {
              pplist[i] = calloc(strlen(default_list[i]) + strlen(pszEnvVar) + 1, sizeof(char));
              strcpy(pplist[i], pszEnvVar);
           } else {
              pplist[i] = calloc(strlen(default_list[i]) + 1, sizeof(char));
           }
           strcat(pplist[i], &(default_list[i][1]));
        } else {
           pplist[i] = calloc(strlen(default_list[i]) + 1,sizeof(char));
           strcpy(pplist[i], default_list[i]);
        }
     }
     return(pplist);
  }

#ifdef DEBUG
  printf("Path = %s\n", iconpath);
#endif

  /**************************************************************************/
  /* Iterate through the search path once to get total count of individual  */
  /* paths within the search path.                                          */
  /**************************************************************************/
  count = 0;
  ptr = iconpath;
  while (ptr) {
    tmpptr = ptr;
    ptr = strchr(ptr, ':');
    /*printf("ptr = %s\n", ptr);*/
    if (tmpptr != ptr) {
       count++;
       if ((ptr) && (ptr[1] != '\0')) {
          for (; ptr[0] == ':'; ptr++);
       } else {
          ptr = (char *)NULL;
       }
    }
  }

  /**************************************************************************/
  /* Debug information.                                                     */
  /**************************************************************************/
#ifdef DEBUG
  printf("IconSearchPath = %s\n", iconpath);
  printf("# of paths = %d\n", count);
#endif

  /**************************************************************************/
  /* Get contents of lang environment variable.                             */
  /**************************************************************************/
  lang = getenv("LANG");
  if (lang) {
     langsize = strlen(lang);
  } else {
     langsize = 0;
  }

  /**************************************************************************/
  /* Allocate the array of pointers to store the individual path strings.   */
  /**************************************************************************/
  pplist = (char **)calloc((count+1), sizeof(void *));

  /**************************************************************************/
  /* Iterate through again to allocate space for each individual path and   */
  /* store that path.                                                       */
  /**************************************************************************/
  count = 0;
  ptr = iconpath;
  while (ptr) {
    tmpptr = ptr;
    ptr = strchr(ptr, ':');
    if (tmpptr != ptr) {
       /*********************************************************************/
       /* Make tmpptr the path.  Also move to the next path in the search   */
       /* path.                                                             */
       /*********************************************************************/
       strip = ptr;
       if ((ptr) && (ptr[1] != '\0')) {
          for (; ptr[0] == ':'; ptr++);
          *strip = '\0';
       } else {
          if (ptr) {
             *strip = '\0';
          }
          ptr = (char *)NULL;
       }

       /*********************************************************************/
       /* If %L in path, then add size of lang variable to it when          */
       /* allocating array for path.                                        */
       /*********************************************************************/
       if (strip = strstr(tmpptr, "%L")) {
         path = malloc(strlen(tmpptr) + langsize + 1);
       } else {
         path = malloc(strlen(tmpptr) + 1);
       }
       strcpy(path, tmpptr);

       /*********************************************************************/
       /* Strip off the /%B... stuff off of the path if there is some.      */
       /*********************************************************************/
       if (strip = strstr(path, "%B")) {
          *strip = '\0';
       }

       /*********************************************************************/
       /* Now replace %L with lang variable.                                */
       /*********************************************************************/
       if (strip = strstr(path, "%L")) {
          *strip = '\0';
          if (langsize) {
             strcat(path, lang);
          }
       }

       /*********************************************************************/
       /* Remove slash from end of path if there is one.                    */
       /*********************************************************************/
       size = strlen(path);
       if (size > 0) {
          if (path[size - 1] == '/') {
             path[size - 1] = '\0';
          }

#ifdef DEBUG
          printf("new path = %s\n", path);
#endif

          /***************************************************************/
          /* See if path is already in our list.                         */
          /***************************************************************/
          bFound = FALSE;
          for (i=0; (i < count) && (!bFound); i++) {
             if (!(strcmp(pplist[i], path))) {
                bFound = TRUE;
             }
          }
          /***************************************************************/
          /* If not in list, then add to list.                           */
          /***************************************************************/
          if (!bFound) {
             pplist[count] = path;
             count++;
          /***************************************************************/
          /* Else, just free resources.                                  */
          /***************************************************************/
          } else {
             free(path);
          }
       }

#ifdef DEBUG
       printf("Path%d = %s\n", count, pplist[count]);
#endif

#if 0
       **** moving this up a bit in the code ****

       /*********************************************************************/
       /* Get the next path in the icon search path.                        */
       /*********************************************************************/
       if ((ptr) && (ptr[1] != '\0')) {
          for (; ptr[0] == ':'; ptr++);
       } else {
          ptr = (char *)NULL;
       }
#endif
    }
  }
  free(iconpath);
  return(pplist);
}

/******************************************************************************/
/*                                                                            */
/* FreeIconSearchPathList                                                     */
/*                                                                            */
/* INPUT:  none                                                               */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void FreeIconSearchPathList(char **pplist)
{
  char *ptr;
  int  i;

  /**************************************************************************/
  /* Iterate through the search path once to get total count of individual  */
  /* paths within the search path.                                          */
  /**************************************************************************/
  if (pplist) {
     for (i = 0; pplist[i]; free(pplist[i]), i++);
     /*
     i = 0;
     while (pplist[i]) {
       free(pplist[i]);
     }
     */
     free(pplist);
  }
}
/******************************************************************************/
/*                                                                            */
/* TurnOnHourGlassAllWindows                                                  */
/*                                                                            */
/* INPUT:  none                                                               */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void TurnOnHourGlassAllWindows()
{
  _DtTurnOnHourGlass(CreateActionAppShell);
  if ( (AddFiletype) &&
       (XtIsRealized(AddFiletype)) &&
       (XtIsManaged(AddFiletype)) ) {
     _DtTurnOnHourGlass(AddFiletype);
  }
  if ( (FileCharacteristics) &&
       (XtIsRealized(FileCharacteristics)) &&
       (XtIsManaged(FileCharacteristics)) ) {
     _DtTurnOnHourGlass(FileCharacteristics);
  }
  if ( (IconSelector) &&
       (XtIsRealized(IconSelector)) &&
       (XtIsManaged(IconSelector)) ) {
     _DtTurnOnHourGlass(IconSelector);
  }
  if ( (OpenFile) &&
       (XtIsRealized(OpenFile)) &&
       (XtIsManaged(OpenFile)) ) {
     _DtTurnOnHourGlass(OpenFile);
  }
}
/******************************************************************************/
/*                                                                            */
/* TurnOffHourGlassAllWindows                                                 */
/*                                                                            */
/* INPUT:  none                                                               */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void TurnOffHourGlassAllWindows()
{
  _DtTurnOffHourGlass(CreateActionAppShell);
  if ( (AddFiletype) &&
       (XtIsRealized(AddFiletype)) &&
       (XtIsManaged(AddFiletype)) ) {
     _DtTurnOffHourGlass(AddFiletype);
  }
  if ( (FileCharacteristics) &&
       (XtIsRealized(FileCharacteristics)) &&
       (XtIsManaged(FileCharacteristics)) ) {
     _DtTurnOffHourGlass(FileCharacteristics);
  }
  if ( (IconSelector) &&
       (XtIsRealized(IconSelector)) &&
       (XtIsManaged(IconSelector)) ) {
     _DtTurnOffHourGlass(IconSelector);
  }
  if ( (OpenFile) &&
       (XtIsRealized(OpenFile)) &&
       (XtIsManaged(OpenFile)) ) {
     _DtTurnOffHourGlass(OpenFile);
  }
}
/******************************************************************************/
/*                                                                            */
/* SetIconData                                                                */
/*                                                                            */
/* INPUT:  icon gadget widget id                                              */
/*         icon file base name                                                */
/*                                                                            */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void SetIconData(Widget wid, char *pszIconFile, enum icon_size_range enumIconSize)
{
  char      pmFileName[MAXBUFSIZE];
  char      bmFileName[MAXBUFSIZE];
  char      pszSize[MAX_EXT_SIZE];
  IconData  *pIconData;
  char      *pszName;

#if 0
  switch (enumIconSize) {
    case Large_Icon :
               strcpy(pszSize, LARGE_EXT);
               break;
    case Medium_Icon :
               strcpy(pszSize, MEDIUM_EXT);
               break;
    case Tiny_Icon :
               strcpy(pszSize, TINY_EXT);
               break;
  }

  sprintf(pmFileName, "%s%s%s", pszIconFile, pszSize, PIXMAP_EXT );
  sprintf(bmFileName, "%s%s%s", pszIconFile, pszSize, BITMAP_EXT );
#endif

  pszName = CreateIconName((char *)NULL, pszIconFile, enumIconSize, PIXMAP_EXT, FALSE);
  strcpy(pmFileName, pszName);
  if (pszName) XtFree(pszName);

  pszName = CreateIconName((char *)NULL, pszIconFile, enumIconSize, BITMAP_EXT, FALSE);
  strcpy(bmFileName, pszName);
  if (pszName) XtFree(pszName);

  pIconData = GetIconDataFromWid(wid);
  if (pIconData) {
     if ( (pIconData->pmDirtyBit) &&
          (pIconData->pmFileName) &&
          (strlen(pIconData->pmFileName)) ) {
#ifdef DEBUG
        printf("SetIconData: unlink '%s'\n", pIconData->pmFileName);  /* debug */
#endif
        unlink(pIconData->pmFileName);
        pIconData->pmDirtyBit = False;
     }
     strcpy(pIconData->pmFileName, pmFileName);

     if ( (pIconData->bmDirtyBit) &&
          (pIconData->bmFileName) &&
          (strlen(pIconData->bmFileName)) ) {
#ifdef DEBUG
        printf("SetIconData: unlink '%s'\n", pIconData->bmFileName);  /* debug */
#endif
        unlink(pIconData->bmFileName);
        pIconData->bmDirtyBit = False;
     }
     strcpy(pIconData->bmFileName, bmFileName);

     if (bShowPixmaps) {
        SET_ICONGADGET_ICON(wid, pmFileName);
     } else {
        SET_ICONGADGET_ICON(wid, bmFileName);
     }
  }
}
/******************************************************************************/
/*                                                                            */
/* GetCorrectIconType                                                         */
/*                                                                            */
/* INPUT:  icon file name                                                     */
/*                                                                            */
/* OUTPUT: correct icon type of icon file name passed                         */
/*                                                                            */
/******************************************************************************/
char * GetCorrectIconType(char *pszIconFile)
{
  char  *pszTmp;
  char  *ptr;
  char  pszNewType[MAX_EXT_SIZE];

  if (bShowPixmaps) {
     strcpy(pszNewType, PIXMAP_EXT);
  } else {
     strcpy(pszNewType, BITMAP_EXT);
  }

  if (pszIconFile) {
     pszTmp = XtMalloc(strlen(pszIconFile) + strlen(pszNewType) + 1);
     if (!pszTmp) return((char *)NULL);
     strcpy(pszTmp, pszIconFile);
     ptr = strrchr(pszTmp, '.');
     if (ptr) {
        strcpy(ptr, pszNewType);
     }
  } else {
     pszTmp = (char *)NULL;
  }
  return (pszTmp);
}
/******************************************************************************/
/*                                                                            */
/* CreateMaskName                                                             */
/*                                                                            */
/* INPUT:  icon file name                                                     */
/*                                                                            */
/* OUTPUT: mask file name for icon name passed in                             */
/*                                                                            */
/******************************************************************************/
char * CreateMaskName(char *pszIconName)
{
  char *pszTmpName;
  char *ptr;
  char *pszNewName;
  char *type_name;
  char *size_name;
  char type_ext[MAX_EXT_SIZE + 2];
  char size_ext[MAX_EXT_SIZE + 2];
  int  bytesneeded = 0;

  /***************************************************************/
  /* initialize temp arrays                                      */
  /***************************************************************/
  type_ext[0] = '\0';
  size_ext[0] = '\0';

  /***************************************************************/
  /* alloc memory for temporary name                             */
  /***************************************************************/
  pszTmpName = (char *)XtMalloc(strlen(pszIconName) + 1);
  if (pszTmpName) {
     strcpy(pszTmpName, pszIconName);
  } else {
     return((char *)NULL);
  }

  /*****************************************/
  /* Strip off icon type extension.        */
  /*****************************************/
  ptr = strrchr(pszTmpName, '.');
  if (ptr) {
     type_name = strtok(ptr, ".");
     *ptr = '\0';
  } else {
     type_name = (char *)NULL;
  }

  /*****************************************/
  /* Get size extention.                   */
  /*****************************************/
  ptr = strrchr(pszTmpName, '.');
  if (ptr) {
     size_name = strtok(ptr, ".");
     *ptr = '\0';
  } else {
     size_name = (char *)NULL;
  }

  /*****************************************/
  /* Alloc the storage for the new name    */
  /*****************************************/
  bytesneeded += ((pszTmpName) ? strlen(pszTmpName) : 0);
  bytesneeded += strlen("_m..");
  bytesneeded += ((size_name) ? strlen(size_name) : 0);
  bytesneeded += ((type_name) ? strlen(type_name) : 0);
  pszNewName = (char *)XtMalloc(bytesneeded + 1);

  /*****************************************/
  /* Create extension names                */
  /*****************************************/
  if (size_name) {
     sprintf(size_ext, ".%s", size_name);
  }
  if (type_name) {
     sprintf(type_ext, ".%s", type_name);
  }
  /*****************************************/
  /* And construct the new name from pieces*/
  /*****************************************/
  if (pszNewName) {
     if (size_name) {
        sprintf(pszNewName, "%s%s_m%s", pszTmpName, size_ext, type_ext);
     } else {
        sprintf(pszNewName, "%s_m%s%s", pszTmpName, size_ext, type_ext);
     }
  }
  if (pszTmpName) XtFree(pszTmpName);

#ifdef DEBUG
  printf("Mask file name = '%s'\n", pszNewName); /* debug */
#endif

  return(pszNewName);
}

