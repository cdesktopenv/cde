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
/* $XConsortium: MenuBar.h /main/4 1996/04/21 19:46:27 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//         Copyright 1991 by Prentice Hall
//         All Rights Reserved
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// MenuBar.h: A menu bar, whose panes support items
//            that execute Cmds

//////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// MODIFIED TO SUPPORT SUBMENUS - not described in Book
///////////////////////////////////////////////////////////


#ifndef MENUBAR_H
#define MENUBAR_H

#include "UIComponent.h"
#include <Xm/RowColumn.h>

class Cmd;
class CmdList;

class MenuBar : public UIComponent {
  protected:
#ifdef DEAD_WOOD
    virtual	Boolean isValidMenuPane(Widget);
#endif /* DEAD_WOOD */

    virtual Widget createPulldown ( Widget, CmdList *, 
			Boolean, unsigned char);

    virtual Widget createPulldown ( Widget, CmdList *, 
			Widget *, Boolean, unsigned char);
    
  public:
    
    MenuBar ( Widget, char *, unsigned char = XmMENU_BAR );
    
    // Create a named menu pane from a list of Cmd objects
    
    virtual Widget addCommands ( Widget *, CmdList *,  Boolean = FALSE, 
			unsigned char = XmMENU_BAR);

    virtual Widget addCommands ( CmdList *, Boolean = FALSE, 
			unsigned char = XmMENU_BAR);

    virtual Widget addCommands ( Widget, CmdList *);

    virtual void addCommand ( Widget, Cmd *);

    virtual  void   removeCommands( Widget, CmdList *);

    virtual  void   removeCommand( Widget, int at);

    virtual  void   removeOnlyCommands( Widget, CmdList *);


    virtual void changeLabel ( Widget, int, char *);

    virtual void changeLabel(Widget, const char * wid_name, const char * label);

    virtual void rotateLabels ( Widget, int, int );
    
    virtual const char *const className() { return "MenuBar"; }
};
#endif   
