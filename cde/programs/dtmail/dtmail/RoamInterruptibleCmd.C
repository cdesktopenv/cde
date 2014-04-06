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
/* $XConsortium: RoamInterruptibleCmd.C /main/3 1995/11/06 16:12:29 rswiston $ */
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
///////////////////////////////////////////////////////////////
// RoamInterruptibleCmd.C: Abstract class that supports lengthy,
//                     user-interruptible activities
//////////////////////////////////////////////////////////////
#include "RoamInterruptibleCmd.hh"
#include "DtMailWDM.hh"
#include "Application.h"
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <assert.h>
extern void forceUpdate( Widget );


RoamInterruptibleCmd::RoamInterruptibleCmd ( char *name, 
					     char *label, 
					     int active ) :
                        NoUndoCmd ( name, label, active )
{
    _wpId        = 0;   // There is no work procedure yet
    _callback    = NULL;	   // Callbacks are specified in execute()
    _clientData  = NULL;
    _done        = FALSE; 
    _interrupted = FALSE;
}

RoamInterruptibleCmd::~RoamInterruptibleCmd()
{
    // Clean up by removing all callbacks
    
    if ( _wpId)    
	XtRemoveWorkProc ( _wpId );
}

void 
RoamInterruptibleCmd::execute ( 
    RoamTaskDoneCallback callback, 
    void *clientData
)
{
    _callback   = callback;
    _clientData = clientData;
    execute();
}

void 
RoamInterruptibleCmd::execute()
{
    char *name_str;
    
    name_str = (char *) name();

    _done  = FALSE;  // Initialize flag

    // Let the derived class post the dialog.
    // Updates happen when derived classes call update() or 
    // updateMessage().

    post_dialog();
    
    // Call the Cmd execute function to handle the Undo, and other
    // general mechanisms supported by Cmd. 
    // execute() calls doit() of derived class.
    
    Cmd::execute();  

    // If the task was completed in a single call,
    // don't bother to set up a work procedure. Just
    // give derived classes a chance to cleanup and
    // call the application's callback function

    // If it was interrupted, the interruptCallback would have been
    // called already and the dialog would have been unposted...

    // We need to focus on only two cases here:  what if the task was
    // was completed in one call without interruptions (unpost dialog 
    // and call callback indicating task completed), and what if the 
    // task was not completed in one call (install a workProc...)
    // 

    // if it done but not interrupted, it was genuinely done.
    if (_done && !_interrupted)
      {
	  unpost_dialog();  // derived classes implement this
	  cleanup();
	  
	  if ( _callback )  // the FALSE is to say it was not interrupted.
	      ( *_callback )( this, FALSE, _clientData );
      }

    // If the task is not done and it was not interrupted and there is
    // a callback to install, install a work procedure to continue the 
    // task as soon as possible.  Call the callback via the work proc
    // after completion.
    // 
    
    else if ((!_done && !_interrupted && _callback)) 
      {
	  
	  _wpId = XtAppAddWorkProc ( theApplication->appContext(), 
				  &RoamInterruptibleCmd::workProcCallback,
 				  (XtPointer) this );
      }
}

Boolean 
RoamInterruptibleCmd::workProcCallback ( 
    XtPointer clientData 
)
{
    RoamInterruptibleCmd *obj = (RoamInterruptibleCmd *) clientData;
    
    // The work procedure just returns the value returned by the
    // workProc member function.
    
    return ( obj->workProc() );
}

Boolean 
RoamInterruptibleCmd::workProc()
{
    // Call derived class's check_if_done() and see if they think the
    // work is already done.

     check_if_done();

     if (_interrupted) {

	 unpost_dialog();
	 cleanup();

        // the TRUE is to say task was interrupted

	if ( _callback )  
	    ( *_callback )( this, TRUE, _clientData );
     }	 
    
    // If the task has been completed, hide the dialog,
    // give the derived class a chance to clean up, and notify
    // the application that instantiated this object.
    
    else if (_done) {
	unpost_dialog();
	cleanup();
	
        // the FALSE is to say task completed without interruptions

	if ( _callback )  
	    ( *_callback )( this, FALSE, _clientData );
    }
    
    return _done;
}

void 
RoamInterruptibleCmd::cleanup()
{
    // Empty
}

// The task has been interrupted.  The user clicked on interrupt...

void 
RoamInterruptibleCmd::interruptCallback ( 
    void * clientData 
)
{
    RoamInterruptibleCmd *obj = ( RoamInterruptibleCmd * ) clientData;
    
    // Just set the _interrupt flag to TRUE. The workProc() 
    // function will notice the next time it is called
    
    obj->interrupt();
}

void 
RoamInterruptibleCmd::interrupt()
{
    _interrupted = TRUE;

    if (_wpId) {
	// Remove the work procedure

	XtRemoveWorkProc ( _wpId );
    }
    
    // Remove the working dialog and give derived 
    // classes a chance to clean up 
    
    unpost_dialog();
    cleanup();
    
    // Notify the application that the task was interrupted
    
    if ( _callback )
	( *_callback )( this, TRUE, _clientData);
}

void 
RoamInterruptibleCmd::updateMessage ( 
    char * msg 
)
{
    theDtMailWDM->updateDialog ( msg );
    forceUpdate(theDtMailWDM->baseWidget());
}

Boolean
RoamInterruptibleCmd::interrupted()
{
    return _interrupted;
}

void
RoamInterruptibleCmd::update()
{
    forceUpdate(theDtMailWDM->baseWidget());
}



