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
 *	$XConsortium: Cmd.h /main/5 1996/08/30 17:03:17 drk $
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


/////////////////////////////////////////////////////////
// Cmd.h: A base class for all command objects
/////////////////////////////////////////////////////////
#ifndef CMD_H
#define CMD_H

#ifndef I_HAVE_NO_IDENT
#endif

class CmdList;
class CmdInterface;

class Cmd {
    
    friend class CmdInterface;
    
  private:
    
    // Lists of other commands to be activated or deactivated
    // when this command is executed or "undone"
    
    CmdList       *_activationList;
    CmdList       *_deactivationList;
    void            revert();   // Reverts object to previous state
    int            _active;     // Is this command currently active?
    int            _previouslyActive; // Previous value of _active
    char          *_name;             // Name of this Cmd
    char          *_label;      // Label for the widget associated with Cmd.
    
  protected:
    
    int           _hasUndo;    // True if this object supports undo
    static Cmd   *_lastCmd;    // Pointer to last Cmd executed
    CmdInterface **_ci;            
    int            _numInterfaces;
    
    virtual void doit()   = 0;  // Specific actions must be defined
    virtual void undoit() = 0;  // by derived classes
    

  public:
    
    Cmd ( char *, char *,  int ); // Protected constructor
    
    virtual ~Cmd ();                 // Destructor
    
    // public interface for executing and undoing commands
    
    virtual void execute();  
    void    undo();
    
    void    activate();   // Activate this object
    void    deactivate(); // Deactivate this object
    char    *getLabel() { return _label; }  
    
#ifdef DEAD_WOOD
    // Functions to register dependent commands
    
    void    addToActivationList ( Cmd * );
    void    addToDeactivationList ( Cmd * );
#endif /* DEAD_WOOD */
    
    // Register an UIComponent used to execute this command
    
    void    registerInterface ( CmdInterface * );
    
    // Access functions 
    
    int active () { return _active; }
    int hasUndo() { return _hasUndo; }
    const char *const name () { return _name; }

#ifdef CAN_INLINE_VIRTUALS
    virtual const char *const className () { return "Cmd"; }
#else /* ! CAN_INLINE_VIRTUALS */
    virtual const char *const className ();
#endif /* ! CAN_INLINE_VIRTUALS */
};
#endif
