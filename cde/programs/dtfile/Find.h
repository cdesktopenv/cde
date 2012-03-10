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
