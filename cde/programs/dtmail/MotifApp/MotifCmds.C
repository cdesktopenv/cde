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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: MotifCmds.C /main/5 1998/09/21 18:49:41 mgreess $ */
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
#include "MotifCmds.h"

SeparatorCmd::SeparatorCmd ( 
    char *name, 
    char *,
    int active 
) : Cmd ( name, NULL, active )
{
}


void
SeparatorCmd::doit()
{
}


void
SeparatorCmd::undoit()
{

}


ToggleButtonCmd::ToggleButtonCmd ( 
    char *name, 
    char *label,
    int active,
    Boolean visible_when_off,
    unsigned char indicator_type
) : Cmd ( name, label, active )
{
    _indicator_type = indicator_type;
    _visible_when_off = visible_when_off;
}


void
ToggleButtonCmd::doit()
{
}


void
ToggleButtonCmd::undoit()
{

}


// Returns TRUE if button is ON, FALSE if OFF.

Boolean
ToggleButtonCmd::getButtonState()
{
   CmdInterface *ci;
   Widget toggleButton;

   if (!_ci) return(False);

   ci = _ci[0];

   toggleButton = ci->baseWidget();

   return (XmToggleButtonGetState(toggleButton));

}

void
ToggleButtonCmd::setButtonState(
    Boolean state,
    Boolean notify
)
{
    
    CmdInterface *ci;
    Widget toggleButton;

    if (!_ci) return;

    ci = _ci[0];

    if (ci) {
	toggleButton = ci->baseWidget();

	XmToggleButtonSetState(toggleButton, state, notify);
    }
}

#ifndef CAN_INLINE_VIRTUALS
const char *const
ToggleButtonCmd::className( void )
{
    return "ToggleButtonCmd";
}
#endif /* ! CAN_INLINE_VIRTUALS */

unsigned char
ToggleButtonCmd::indicatorType( void )
{
    return _indicator_type;
}

Boolean
ToggleButtonCmd::visibleWhenOff( void )
{
    return _visible_when_off;
}
