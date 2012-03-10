/* $XConsortium: PopupMenu.h /main/4 1995/11/01 11:29:18 rswiston $ */
/*****************************************************************************
 *
 *   File:         PopupMenu.h
 *
 *   Project:      CDE
 *
 *   Description:  This file contains defines and declarations needed
 *                 by PopupMenu.c and UI.c
 *
 *
 ****************************************************************************/

#ifndef _popup_menu_h
#define _popup_menu_h

#include "DataBaseLoad.h"


extern void CreatePopupMenu ( Widget );
extern void PostPopupMenu ( Widget, XtPointer, XEvent * );
extern void CreateWorkspacePopupMenu ( Widget , SwitchData *);
extern void PostWorkspacePopupMenu ( Widget, XtPointer, XEvent * );


#endif /* _popup_menu_h */
/*  DON"T ADD ANYTHING AFTER THIS #endif  */
