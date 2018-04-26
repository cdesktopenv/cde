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
/* $XConsortium: cmnrtns.h /main/5 1995/11/01 16:11:28 rswiston $ */
#include <Dt/IconFile.h>
/***************************************************************************/
/*                                                                         */
/*  cmnrtns.h                                                              */
/*                                                                         */
/***************************************************************************/

#ifndef _CMNRTNS_H_INCLUDED
#define _CMNRTNS_H_INCLUDED

#ifndef STORAGECLASS
#ifdef  NOEXTERN
#define STORAGECLASS
#else
#define STORAGECLASS extern
#endif
#endif

/***************************************************************************/
/*                                                                         */
/*  Macro Declarations                                                     */
/*                                                                         */
/***************************************************************************/
#define SET_ICONGADGET_ICON_AND_EXT(widIconGadget,pszIconFileName, pszBuffer) {\
          sprintf(pszBuffer, "%s%s", pszIconFileName, bShowPixmaps ? PIXMAP_EXT : BITMAP_EXT); \
          XtVaSetValues (widIconGadget,                                     \
                         XmNimageName, "",                                  \
                         NULL);                                             \
        /*printf("setting icon to = '%s'\n", pszBuffer);*/                  \
          XtVaSetValues (widIconGadget,                                     \
                         XmNimageName, pszBuffer,                           \
                         NULL);                                             \
}

#define SET_ICONGADGET_ICON(widIconGadget,pszIconFileName) {                \
          /*printf("setting icon to = '%s'\n", pszIconFileName);*/          \
          XtVaSetValues (widIconGadget,                                     \
                         XmNimageName, "",                                  \
                         NULL);                                             \
          XtVaSetValues (widIconGadget,                                     \
                         XmNimageName, pszIconFileName,                     \
                         NULL);                                             \
}

#define FIND_ICONGADGET_ICON(pszIcon,pszFile,size) {                        \
          pszFile = (char *)NULL;                                           \
          pszFile = _DtGetIconFileName(XtScreen(CreateActionAppShell),       \
                                      NULL,                                 \
                                      pszIcon,                              \
                                      NULL,                                 \
                                      size);                                \
}

#define CHANGE_ICONGADGET_ICON(widIconGadget,buffer,name,size,type) {       \
          sprintf(buffer, "%s.%s.%s", name, size, type);                    \
          if (check_file_exists(buffer)) {                                  \
            SET_ICONGADGET_ICON(widIconGadget, buffer);                     \
          } else {                                                          \
            SET_ICONGADGET_ICON(widIconGadget, "");                         \
          }                                                                 \
}

#define SET_TOGGLEBUTTON(widToggleButton,bState) {                          \
          XtVaSetValues (widToggleButton,                                   \
                         XmNset, bState,                                    \
                         NULL);                                             \
}

/***************************************************************************/
/*                                                                         */
/*  Prototypes for functions                                               */
/*                                                                         */
/***************************************************************************/

ushort WriteDefinitionFile(char *, ActionData *);
ushort WriteActionFile(ActionData *);
void   Change_IconGadget_IconType(Widget, char *);
void   load_icons (Widget wid, XtPointer client_data,
		   XmFileSelectionBoxCallbackStruct *cbs);
void   GetWidgetTextString(Widget, char **);
void   PutWidgetTextString (Widget wid, char *pszText);
char * GetCoreName(char *pszFullName);
char **GetIconSearchPathList(void);
void   FreeIconSearchPathList(char **);
void   TurnOnHourGlassAllWindows();
void   TurnOffHourGlassAllWindows();
char * ReplaceSpaces(char *pszName);
void   SetIconData(Widget, char *, enum icon_size_range);
char * GetCorrectIconType(char *);
char * CreateMaskName(char *);
IconData * GetIconDataFromWid(Widget);


#endif /* _CMNRTNS_H_INCLUDED */
