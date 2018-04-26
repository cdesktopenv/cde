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
/* $XConsortium: QuitCmd.C /main/4 1995/12/07 15:27:45 rswiston $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
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


///////////////////////////////////////////////////////////
// QuitCmd.C: Exit an application after checking with user.
//////////////////////////////////////////////////////////
#include "QuitCmd.h"
#include <stdlib.h>
#include "MainWindow.h"
#include "Application.h"

#include <nl_types.h>
extern nl_catd catd;

#include "NLS.hh"

QuitCmd::QuitCmd ( char *name, char *label, int active, MainWindow *mywindow) : 
                 WarnNoUndoCmd ( name, label, active ) 
{
    _mywindow = mywindow;
    _dialogParentWidget = _mywindow->baseWidget();
    setQuestion ( GETMSG(catd, 1, 9, "Close this folder?") );
}

void QuitCmd::doit()
{
    _mywindow->quit();
    
//    exit(status);
}       


// If there are no windows in the application (say, none have been created!)
// or if its the last window and the user wants to close it, enquire whether
// the user intends to quit the session since that is what closing the last
// window means!
//


// HI wants the Quit to "just happen" -- they don't want any dialog
// to come up when user chooses to Close a container.
// Just call doit().  
// Retain code in case they change their mind and later decide to 
// have a confirm dialog...
// void 
// QuitCmd::execute()
// {
//     if (theApplication->num_windows() <= 1) {
// 	char *qq = "Exit this session?";
// 	setQuestion(qq);
// 	this->AskFirstCmd::execute();
//     }
//     else {
// 	this->AskFirstCmd::execute();
//     }
// }

void
QuitCmd::execute()
{
    this->doit();
}


