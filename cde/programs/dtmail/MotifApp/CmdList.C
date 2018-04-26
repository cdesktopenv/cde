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
/* $XConsortium: CmdList.C /main/3 1995/11/06 15:59:50 rswiston $ */
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


////////////////////////////////////////////////////////////
// CmdList.C: Maintain a list of Cmd objects
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// MODIFIED TO INHERIT FROM CMD - not described in Book
///////////////////////////////////////////////////////////


#include "CmdList.h"

CmdList::CmdList() : Cmd("CmdList", "CmdList", 1)
{
   _contents = 0;
   _numElements = 0;
   _pane = NULL;
}

CmdList::CmdList(char *name, char *label ) : Cmd(name, label, 1)
{
    // The list is initially empty
    
    _contents    = 0;
    _numElements = 0;
    _pane        = NULL;
}

CmdList::~CmdList()
{
    // free the list
    
    delete []_contents;
}

void CmdList::add ( Cmd *cmd )
{
    int i;
    Cmd **newList;

    // CmdList can only be undone if all Cmds it contains can be undone
    
    if(!cmd->hasUndo())
	_hasUndo = 0;
    
    // Allocate a list large enough for one more element
    
    newList = new Cmd*[_numElements + 1];
    
    // Copy the contents of the previous list to
    // the new list
    
    for( i = 0; i < _numElements; i++)
	newList[i] = _contents[i];
    
    // Free the old list
    
    if (_contents)
	delete []_contents;
    
    // Make the new list the current list
    
    _contents =  newList;
    
    // Add the command to the list and update the list size.
    
    _contents[_numElements] = cmd;
    
    _numElements++;
}

Cmd *CmdList::operator[] ( int index )
{
    // Return the indexed element
    
    return _contents[index];
}


void CmdList::doit()
{
    for( int i = 0; i < _numElements; i++)
	_contents[i]->execute();
}

void CmdList::undoit()
{
    if(_hasUndo)
	for( int i = _numElements - 1; i >=0; i--)
	    _contents[i]->undo();
}
