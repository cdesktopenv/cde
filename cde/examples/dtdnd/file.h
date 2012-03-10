/* $XConsortium: file.h /main/3 1995/10/27 10:39:01 rswiston $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         file.h
 **
 **   Description:  Header for file transfer portion of CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * Public File Transfer Function Declarations
 */

void		fileCheckForDrag(Widget, XEvent*, int, int);
Widget          fileCreateDragSource(Widget);
void            fileDragSetup(Widget);
void            fileTransferCallback(Widget, XtPointer, XtPointer);
char *		fileStoreBuffer(char*, void*, int);
