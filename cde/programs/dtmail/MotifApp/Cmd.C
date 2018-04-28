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
/* $TOG: Cmd.C /main/4 1998/07/24 16:04:37 mgreess $ */
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


///////////////////////////////////////////////////////
// Cmd.C
///////////////////////////////////////////////////////
#include <stdlib.h>
#include "Cmd.h"
#include "CmdList.h"
#include "CmdInterface.h"

extern Cmd *theUndoCmd;  // External object that reverses the 
// most recent Cmd when executed

Cmd *Cmd::_lastCmd = NULL;  // Pointer to most recent Cmd

Cmd::Cmd ( char *name, char *label, int active )
{
    // Initialize all data members
    
    _name              = name;  
    _active            = active;  
    _numInterfaces     = 0;    
    _ci                = NULL;
    _activationList    = NULL;
    _deactivationList  = NULL;
    _hasUndo           = TRUE;
    _previouslyActive  = 0;

    if (label) {
	_label         = strdup(label);
    } else {
	_label         = strdup(name);
    }
}

Cmd::~Cmd()
{
    delete _activationList;	
    delete _deactivationList;	
    free (_label);
    if (_ci)
	delete [] _ci;
}

void Cmd::registerInterface ( CmdInterface *ci )
{
    // Make a new list, large enough for the new object
    
    CmdInterface **newList = new CmdInterface*[_numInterfaces + 1];
    
    // Copy the contents of the previous list to
    // the new list
    
    for( int i = 0; i < _numInterfaces; i++)
	newList[i] = _ci[i];
    
    // Free the old list
    
    if (_ci)
	delete []_ci;
    
    // Install the new list
    
    _ci =  newList;
    
    // Add the object to the list and update the list size.
    
    _ci[_numInterfaces] = ci;
    
    _numInterfaces++;
    
    if ( ci )
	if ( _active )
	    ci->activate();
	else
	    ci->deactivate();      
}

void Cmd::activate()
{
    // Activate the associated interfaces
    
    for ( int i = 0; i < _numInterfaces; i++ )
	_ci[i]->activate ();
    
    // Save the current value of active before setting the new state
    
    _previouslyActive = _active;
    _active = TRUE;
}

void Cmd::deactivate()
{
    // Deactivate the associated interfaces
    
    for ( int i = 0; i < _numInterfaces; i++ )
	_ci[i]->deactivate ();
    
    // Save the current value of active before setting the new state
    
    _previouslyActive = _active;
    _active = FALSE;
}

void Cmd::revert()
{
    // Activate or deactivate, as necessary, 
    // to return to the previous state
    
    if ( _previouslyActive )
	activate();
    else
	deactivate();
}

#ifdef DEAD_WOOD
void Cmd::addToActivationList ( Cmd *cmd )
{
    if ( !_activationList )
	_activationList = new CmdList();
    
    _activationList->add ( cmd );
}

void Cmd::addToDeactivationList ( Cmd *cmd )
{
    if ( !_deactivationList )
	_deactivationList = new CmdList();
    
    _deactivationList->add ( cmd );
}
#endif /* DEAD_WOOD */

void Cmd::execute()
{
    int i;      
    
    // If a command is inactive, it cannot be executed
    
    if ( !_active )
	return;
    

    // Activate or deactivate the global theUndoCmd, 
    // and remember the last command, as needed
    
    if ( _hasUndo )
    {
	Cmd::_lastCmd = this;
	theUndoCmd->activate();
    }
    else  
    {      
	Cmd::_lastCmd = NULL;
	theUndoCmd->deactivate();
    }
    
    // Process the commands that depend on this one
    
    if ( _activationList )    
	for ( i = 0; i < _activationList->size(); i++ )
	    (*_activationList)[i]->activate();
    
    if ( _deactivationList )    
	for ( i = 0; i < _deactivationList->size(); i++ )
	    (*_deactivationList)[i]->deactivate();


    // Call the derived class's doit member function to 
    // perform the action represented by this object
    
    doit();
    
}

void Cmd::undo()
{
    int i;
    
    // Call the derived class's undoit() member function.
    
    undoit();
    
    // The system only supports one level of undo, and this is it,
    // so deactivate the undo facility.
    
    theUndoCmd->deactivate();
    
    // Reverse the effects of the execute() member function by 
    // reverting all dependent objects to their previous state
    
    if ( _activationList )        
	for ( i = 0; i < _activationList->size(); i++ )
	    (*_activationList)[i]->revert();
    
    if ( _deactivationList )    
	for ( i = 0; i < _deactivationList->size(); i++ )
	    (*_deactivationList)[i]->revert();
}

#ifndef CAN_INLINE_VIRTUALS
const char *const
Cmd::className(void)
{
    return "Cmd";
}
#endif /* ! CAN_INLINE_VIRTUALS */
