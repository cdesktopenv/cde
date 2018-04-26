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
/*
 *+SNOTICE
 *
 *	$XConsortium: NoOpCmd.h /main/3 1995/11/06 16:10:29 rswiston $
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


/////////////////////////////////////////////////////////////
// NoOpCmd.h: Example, dummy command class
/////////////////////////////////////////////////////////////
#ifndef NOOPCMD_H
#define NOOPCMD_H

#include "Cmd.h"
#include "AskFirstCmd.h"
#include "InfoDialogManager.h"

class NoOpCmd : public Cmd {
    
  protected:
    
    virtual void doit();   
    virtual void undoit(); 
//    virtual void okCallback(void *);

  public:
    
    NoOpCmd ( char *, char *, int );
    virtual const char *const className () { return "NoOpCmd"; }
};

class LabelCmd: public Cmd {
protected:

#ifdef CAN_INLINE_VIRTUALS
    virtual void doit() {}
    virtual void undoit() {}
#else /* ! CAN_INLINE_VIRTUALS */
    virtual void doit();
    virtual void undoit();
#endif /* ! CAN_INLINE_VIRTUALS */

public:
    LabelCmd ( char *name, char *label, int active) :Cmd(name, label, active){}
#ifdef CAN_INLINE_VIRTUALS
    virtual ~LabelCmd(){}
    virtual const char *const className () { return "LabelCmd"; }
#else /* CAN_INLINE_VIRTUALS */
    ~LabelCmd();
    virtual const char *const className ();
#endif /* CAN_INLINE_VIRTUALS */
};

#endif
