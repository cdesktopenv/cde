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
 *	$XConsortium: RoamInterruptibleCmd.hh /main/3 1995/11/06 16:12:42 rswiston $
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


//////////////////////////////////////////////////////////////
// RoamInterruptibleCmd.h: Abstract class that supports lengthy,
//                     user-interruptible activities
//////////////////////////////////////////////////////////////
#ifndef ROAMINTERRUPTIBLECMD_H
#define ROAMINTERRUPTIBLECMD_H

#include <Xm/Xm.h>
#include "NoUndoCmd.h"

// Influenced by the InterruptibleCmd class from MotifApp.
// Different because it does not call the derived class's
// doit() repeatedly.  Instead, it repeatedly calls check_if_done().

// Define a type for the callback invoked when the task is finished

class RoamInterruptibleCmd;

typedef void (*RoamTaskDoneCallback) (
		RoamInterruptibleCmd *, 
		Boolean, 
		void * );

class RoamInterruptibleCmd : public NoUndoCmd {
    
  private:
    
    XtWorkProcId     _wpId;         // The ID of the workproc
    RoamTaskDoneCallback _callback;     // Application-defined callback
    void            *_clientData;
    
  protected:
    
    Boolean      _done;         // TRUE if the task has been completed
    Boolean _interrupted;	// TRUE if the task was interrupted

    virtual void cleanup();     // Called when task ends
    virtual void updateMessage ( char * );

    virtual void post_dialog() = 0;
    virtual void unpost_dialog() = 0;
    virtual void check_if_done() = 0;

    Boolean workProc ();
    static Boolean  workProcCallback ( XtPointer );
    static void     interruptCallback ( void * );
    void interrupt(); 

    
    // Derived classes implement doit(), declared by Cmd
    
  public:
    
    RoamInterruptibleCmd ( char * , char *, int );
    virtual ~RoamInterruptibleCmd();
    
    virtual void execute();  // Overrides base class member function
    virtual void execute ( RoamTaskDoneCallback, void * );

    // Enable others to check if the command was interrupted...
    virtual Boolean  interrupted();
    
    // Force update (and flushing of events in queue).
    virtual void update();

};
#endif




