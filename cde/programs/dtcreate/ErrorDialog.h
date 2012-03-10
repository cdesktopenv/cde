/* $XConsortium: ErrorDialog.h /main/4 1995/11/01 16:07:01 rswiston $ */

/*******************************************************************************
       ErrorDialog.h
       This header file is included by ErrorDialog.c

*******************************************************************************/

#ifndef _ERRORDIALOG_H_INCLUDED
#define _ERRORDIALOG_H_INCLUDED


#include <stdio.h>
#include <Xm/MessageB.h>

extern Widget   ErrorDialog;

/*******************************************************************************
       Declarations of global functions.
*******************************************************************************/


void    display_error_message (Widget parent, char *message);


#endif  /* _ERRORDIALOG_H_INCLUDED */
