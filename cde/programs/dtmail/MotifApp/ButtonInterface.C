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
/* $XConsortium: ButtonInterface.C /main/4 1996/04/05 16:48:43 mgreess $ */
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


//////////////////////////////////////////////////////////////
// ButtonInterface.C: A push button interface to a Cmd object
///////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <ctype.h>
#include "ButtonInterface.h"
#include <Xm/PushB.h>
#include "Help.hh"
#include "Cmd.h"

ButtonInterface::ButtonInterface ( Widget parent, 
				  Cmd *cmd ) : CmdInterface ( cmd )
{

    // We need to generate a button name that doesn't have illegal characters.
    //
    char w_name[200];
    mapName(_name, w_name);

    XmString label = XmStringCreateLocalized(_cmd->getLabel());
    _w = XtVaCreateWidget (w_name, 
			 xmPushButtonWidgetClass,
			 parent,
			 XmNlabelString, label, NULL);
    XmStringFree(label);

    printHelpId("_w", _w);
    // XtAddCallback(_w, XmNhelpCallback, HelpCB, helpId);
    // free(helpId);

    installDestroyHandler();
    
    // The _active member is set when each instance is registered
    // with an associated Cmd object. Now that a widget exists,
    // set the widget's sensitivity according to its active state.
    
    if ( _active )
	activate();     
    else
	deactivate();   

#ifdef GNU_CC  // No idea what the right ifdef is for automatically recognizing g++
    
    // G++ reportedly doesn't like the form expected by cfront. I'm
    // told this will work, but I haven't tested it myself.
    
    XtAddCallback ( _w,  
		   XmNactivateCallback, 
		   executeCmdCallback,
		   (XtPointer) this );  
#else

    XtAddCallback ( _w,  
		   XmNactivateCallback, 
		   &CmdInterface::executeCmdCallback,
		   (XtPointer) this );  
#endif
    
}

#ifndef CAN_INLINE_VIRTUALS
ButtonInterface::~ButtonInterface(void)
{
}
#endif /* ! CAN_INLINE_VIRTUALS */

void
ButtonInterface::mapName(const char * input, char * output)
{
    strcpy(output, input);
    for (char * cur = output; *cur; cur++) {
	if (isspace(*cur) || *cur == ',') {
	    *cur = '_';
	    continue;
	}

	if (*cur == '.' || *cur == ':') {
	    *cur = 0;
	    break;
	}
    }
}
