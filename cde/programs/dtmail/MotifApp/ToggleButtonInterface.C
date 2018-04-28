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
/* $TOG: ToggleButtonInterface.C /main/6 1998/09/21 18:50:08 mgreess $ */
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
//////////////////////////////////////////////////////////////
// ToggleButtonInterface.C: A toggle button interface to a Cmd object
///////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <ctype.h>
#include "Cmd.h"
#include "MotifCmds.h"
#include "ToggleButtonInterface.h"
#include <Xm/ToggleB.h>
#include "Help.hh"

ToggleButtonInterface::ToggleButtonInterface ( 
    Widget parent, 
    Cmd *cmd 
) : CmdInterface ( cmd )
{

    // We need to generate a button name that doesn't have illegal characters.
    //
    char *w_name = new char[200];
    strcpy(w_name, _name);
    for (char * cur = w_name; *cur; cur++) {
	if (isspace(*cur) || *cur == ',') {
	    *cur = '_';
	    continue;
	}

	if (*cur == '.') {
	    *cur = 0;
	    break;
	}
    }

    XmString label = XmStringCreateLocalized(cmd->getLabel());

    if (0 ==  strcmp(cmd->className(),"ToggleButtonCmd"))
    {
	ToggleButtonCmd	*tbc = (ToggleButtonCmd*) cmd;
	Boolean		visible_when_off = tbc->visibleWhenOff();
	unsigned char	indicator_type = tbc->indicatorType();

        _w = XtVaCreateWidget (w_name, 
		xmToggleButtonWidgetClass,
		parent,
		XmNlabelString, label,
		XmNvisibleWhenOff, visible_when_off,
		XmNindicatorType, indicator_type,
		NULL);
    }
    else
        _w = XtVaCreateWidget (w_name, 
		xmToggleButtonWidgetClass,
		parent,
		XmNlabelString, label,
		XmNvisibleWhenOff, TRUE,
		NULL);

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
		   XmNvalueChangedCallback, 
		   executeCmdCallback,
		   (XtPointer) this );  
#else

    XtAddCallback ( _w,  
		   XmNvalueChangedCallback,
		   &CmdInterface::executeCmdCallback,
		   (XtPointer) this );  
#endif
    delete [] w_name;
}

#ifndef CAN_INLINE_VIRTUALS
ToggleButtonInterface::~ToggleButtonInterface (void)
{
}
#endif /* ! CAN_INLINE_VIRTUALS */
