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
/* $TOG: MotifCmds.h /main/5 1998/09/21 18:52:49 mgreess $
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


/////////////////////////////////////////////////////////
// MotifCmds.h: Menu cmds
/////////////////////////////////////////////////////////
#ifndef MOTIFCMDS_H
#define MOTIFCMDS_H

#include "Cmd.h"
#include "CmdInterface.h"
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>


// Elsewhere in MotifApp, we refer to instances of these classes
// via their className() method. We compare them to SeparatorCmd
// or ToggleButtonCmd.
// We want derived classes to also answer to the same className().
// So, we make className() a non-virtual method.

class SeparatorCmd : public Cmd {
  public:
    virtual void doit();   
    virtual void undoit();   
    SeparatorCmd( char *, char *, int );
    const char *const className () { return "SeparatorCmd"; }
};

// Its critical that children of ToggleButtonCmd not have a 
// const className() method.
// Make className() a non-virtual method then.

class ToggleButtonCmd : public Cmd {
  public:
    virtual void doit();   
    virtual void undoit();   
    ToggleButtonCmd(
		char *name, char *label, int active,
		Boolean visible_when_off=TRUE,
		unsigned char indicator_type=XmONE_OF_MANY_ROUND);

    unsigned char	indicatorType();
    Boolean		visibleWhenOff();
    Boolean		getButtonState();
    void		setButtonState(Boolean, Boolean);
    
#ifdef CAN_INLINE_VIRTUALS
    const char *const	className () { return "ToggleButtonCmd"; }
#else /* ! CAN_INLINE_VIRTUALS */
    const char *const	className ();
#endif /* ! CAN_INLINE_VIRTUALS */

    unsigned char	_indicator_type;
    Boolean		_visible_when_off;

};
    
#endif
