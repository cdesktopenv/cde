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
/* $XConsortium: ModAttr.h /main/3 1995/11/02 14:42:29 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           ModAttr.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the modify file attributes dialog.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _ModAttr_h
#define _ModAttr_h

#include <stdio.h> /* for FILENAME_MAX */
#include "Common.h"

extern DialogClass * modAttrClass;


#define MAXLINELENGTH 1024   /* for lines in dtfs configuration file            */
                             /* used in fsDialog.c in fgets call & as dimension */
                             /* of some arrays ... made large enough to hold    */
                             /* MAX_PATH for most machines                      */


typedef struct {                                /* File system attributes */
   char    buttonLabel[MAXLINELENGTH];
   char    fsDialogProgram[MAXLINELENGTH];
   char    path[MAXLINELENGTH];
   char    warningMessage[MAXLINELENGTH];
   Boolean dismissStdPermissionDialog;
} dtFSData;

typedef struct
{
   Boolean       displayed;
   Position      x;
   Position      y;
   Dimension     width;
   Dimension     height;

   String        host;
   String        directory;
   String        name;
   String        link_name;
   String        filetype;
   String        owner;
   String        group;
   int           accessBits;
   int           setuidBits;
   int           size;
   time_t        modTime;
   Widget        main_widget;
   
   PixmapData  * pixmapData;
} ModAttrData, * ModAttrDataPtr;


typedef struct
{
   XtCallbackProc   callback;
   XtPointer        client_data;
   XtPointer        modAttr_rec;
} ModAttrApply;


typedef struct
{
   Widget shell;
   Widget bboard;
   Widget name;
   Widget link_name;
   Widget link_label;
   Widget fileType;
   Widget owner;
   Widget ownerL;
   Widget owner_label;
   Widget group;
   Widget groupL;
   Widget ownerRead;
   Widget ownerWrite;
   Widget ownerExecute;
   Widget groupRead;
   Widget groupWrite;
   Widget groupExecute;
   Widget otherRead;
   Widget otherWrite;
   Widget otherExecute;
   Widget otherPLabel;
   Widget fsWarning;
   Widget fsPushButton;
   Widget size;
   Widget modTime;
   Widget ok;
   Widget cancel;
   Widget help;
   dtFSData fileSystem;

   ModAttrApply * apply_data;
} ModAttrRec;

#endif /* _ModAttr_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

