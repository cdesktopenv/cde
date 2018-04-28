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
 *	$XConsortium: InterruptibleCmd.h /main/3 1995/11/06 16:31:32 rswiston $
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


//////////////////////////////////////////////////////////////
// InterruptibleCmd.h: Abstract class that supports lengthy,
//                     user-interruptible activities
//////////////////////////////////////////////////////////////
#ifndef INTERRUPTIBLECMD_H
#define INTERRUPTIBLECMD_H

#ifndef I_HAVE_NO_IDENT
#endif

#include <Xm/Xm.h>
#include "NoUndoCmd.h"

// Define a type for the callback invoked when the task is finished

class InterruptibleCmd;

typedef void (*TaskDoneCallback) ( InterruptibleCmd *, Boolean, void * );

class InterruptibleCmd : public NoUndoCmd {
    
  private:
    
    XtWorkProcId     _wpId;         // The ID of the workproc
    TaskDoneCallback _callback;     // Application-defined callback
    void            *_clientData;
    Boolean workProc ();
    static Boolean  workProcCallback ( XtPointer );
    static void     interruptCallback ( void * );
    void interrupt(); 
    
  protected:
    
    Boolean      _done;         // TRUE if the task has been completed
    virtual void cleanup();     // Called when task ends
    virtual void updateMessage ( char * );
    
    // Derived classes implement doit(), declared by Cmd
    
  public:
    
    InterruptibleCmd ( char *, char *, int );
    virtual ~InterruptibleCmd();
    
    virtual void execute();  // Overrides base class member function
    virtual void execute ( TaskDoneCallback, void * );
};
#endif
