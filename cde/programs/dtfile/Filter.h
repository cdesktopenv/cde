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
/* $XConsortium: Filter.h /main/3 1995/11/02 14:39:05 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Filter.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the file filter dialog.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Filter_h
#define _Filter_h

#define SHOWN  0
#define HIDDEN 1

#define DEFAULT_FILTER "DOT_FILE,DOT_FOLDER,CURRENT_FOLDER"

extern DialogClass * filterClass;


typedef struct {
   char *filetype;
   int index;
   Boolean selected;
} FTData;

typedef struct
{
   Boolean       displayed;
   Position      x;
   Position      y;
   Dimension     width;
   Dimension     height;
 
   Boolean       match_flag;
   String        filter;
   Boolean       show_hidden;

   FTData        **user_data;
   int           count;
   int           filetypesFilteredCount;
   char          *filetypesFiltered;
} FilterData, * FilterDataPtr;


typedef struct
{
   XtCallbackProc callback;
   XtPointer      client_data;
   XtPointer      filter_rec;
} FilterApply;

typedef struct
{
   Widget shell;
   Widget form;
   Widget fileNameFilter;
   Widget hidArray[2];
   Widget hidden;
   Widget scrolled_window;
   Widget file_window;
   Widget select_all;
   Widget unselect_all;
   Widget label;

   Widget ok;
   Widget apply;
   Widget reset;
   Widget close;
   Widget help;

   FilterApply * apply_data;

   char cancel;
   FilterData * previous_filter_data;
} FilterRec;

#endif /* _Filter_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
