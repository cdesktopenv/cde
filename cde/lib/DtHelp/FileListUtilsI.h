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
/* $XConsortium: FileListUtilsI.h /main/5 1995/10/26 12:19:25 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FileListUtilsI.h
 **
 **   Project:     DtHelp Project
 **
 **   Description: File locating and handling utilities
 ** 
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _FileListUtilsI_h
#define _FileListUtilsI_h

#include <Xm/XmP.h>

typedef struct _DtHelpFileRec {
  struct _DtHelpFileRec *  next;           /* next file in list */
  char *                fileName;          /* basename plus extensions */
  char *                fullFilePath;      /* full path of the file */
  int                   nameKey;           /* used for quick identity compare*/
  char *                fileTitle;         /* comparable title of file */
  XmString              fileTitleXmStr;    /* displayable title of file */
  char *                docId;             /* string identifying doc */
  char *                timeStamp;         /* string with time of doc */
  void *                clientData;        /* data assoc with the file */
  unsigned int          fileSelected:1;    /* file selected for activity */
  unsigned int          reservedFlag1:1; 
  unsigned int          reservedFlag2:1;
  unsigned int          reservedFlag3:1;
  unsigned int          clientFlag1:1;
  unsigned int          clientFlag2:1;
  unsigned int          clientFlag3:1;
  unsigned int          clientFlag4:1;
} _DtHelpFileRec, * _DtHelpFileEntry, * _DtHelpFileList;

typedef void (*_DtHelpFileScanProcCB)(
          int           count,
          XtPointer     clientData);

typedef Boolean (*_DtHelpFileInfoProc)(
    XtPointer  pDisplayArea, /* in: display area in use */
    char *     volumePath,    /* in: full path to file */
    char **    ret_title,     /* out: mallocd doc title string */
    XmString * ret_titleXmStr,/* out: mallocd doc title XmStr */
    char **    ret_docId,     /* out: mallocd doc Id string */
    char **    ret_timeStamp, /* out: mallocd doc time string */
    int *      ret_nameKey,   /* out: hash value for fast discimination */
    XmFontList * io_fontList, /* io: fontList for title */
    Boolean *  ret_mod);      /* out: has font list been changed */


/* Bitwise-ORable flags that affect the comparisons 
   done when comparing two files. */
typedef enum {
  _DtHELP_FILE_NAME = 0x01,
  _DtHELP_FILE_DIR = 0x02,
  _DtHELP_FILE_TITLE = 0x04,
  _DtHELP_FILE_TIME = 0x08,
  _DtHELP_FILE_IDSTR = 0x10,
  _DtHELP_FILE_LOCALE = 0x20
  } _DtHelpFileAttributes;

Boolean _DtHelpFileListAddFile (
        _DtHelpFileList *      in_out_list,
        XmFontList *           io_fontList,
        Boolean *              ret_mod,
        char *                 fullFilePath,
        char *                 fileName,
        _DtHelpFileInfoProc    infoProc,
        int                    compareFlags,
        int                    sortFlags,
        XtPointer              pDisplayArea);
_DtHelpFileList _DtHelpFileListGetMatch ( 
        _DtHelpFileList     fileListHead,
        char *              fullFilePath,
        _DtHelpFileInfoProc infoProc,
        int                 compareFlags,
        XtPointer           pDisplayArea);
Boolean _DtHelpFileIsSameP(
        char *              fileName1,
        char *              fileName2,
        _DtHelpFileInfoProc infoProc,
        int                 compareFlags,
        XtPointer           pDisplayArea);
_DtHelpFileList _DtHelpFileListGetNext (
        _DtHelpFileList fileListHead,
        _DtHelpFileList curFile);
int _DtHelpFileListScanPaths (
        _DtHelpFileList * in_out_list,
        XmFontList *      io_fontList,
        Boolean *         ret_mod,
        char *            type,
        const char *      suffixList[],
        Boolean           searchHomeDir,
        _DtHelpFileInfoProc infoProc,
        XtPointer           pDisplayArea,
        int               sysPathCompareFlags,
        int               otherPathCompareFlags,
        int               sortFlags,
        _DtHelpFileScanProcCB scanProc,
        XtPointer         clientData);
void _DtHelpFileFreeEntry (
        _DtHelpFileEntry entry);

#endif /* _FileListUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

