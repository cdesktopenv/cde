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
