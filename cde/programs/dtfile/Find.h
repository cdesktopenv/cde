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
/* $XConsortium: Find.h /main/3 1995/11/02 14:39:33 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Find.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the find file dialog.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Find_h
#define _Find_h


extern DialogClass * findClass;

#define MAX_DIR_PATH_LEN 1024

#define ON  0
#define OFF 1

typedef struct
{
   Boolean       displayed;
   Position      x;
   Position      y;
   Dimension     width;
   Dimension     height;

#if defined(__hpux) || defined(sun)
   Boolean       follow_links;
#endif
   String        directories;
   String        filter;
   String        content;
   String      * matches;
   int           num_matches;
   int           selected_item;
   FileMgrData * file_mgr_data;
} FindData, * FindDataPtr;


typedef struct
{
   XtCallbackProc callback;
   XtPointer      client_data;
   XtPointer      find_rec;
} FindApply;


typedef struct
{
   Widget shell;
   Widget form;
   Widget searchDirectory;
   Widget content;
   Widget followLink;
   Widget widgArry[2];
   Widget listLabel;
   Widget fileNameFilter;
   Widget matchList;
   Widget putOnDT;
   Widget newFM;

   Widget separator;
   Widget start;
   Widget stop;
   Widget close;
   Widget help;

   FindApply  * apply_data;
   int          selectedItem;
   FILE       * popenId;
   int          childpid;
   XtInputId    alternateInputId;
   Boolean      searchInProgress;
   FileMgrRec * fileMgrRec;
} FindRec;


#endif /* _Find_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
