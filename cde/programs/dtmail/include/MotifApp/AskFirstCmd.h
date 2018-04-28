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
 *	$XConsortium: AskFirstCmd.h /main/3 1995/11/06 16:29:53 rswiston $
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


///////////////////////////////////////////////////////////////
// AskFirstCmd.h: Base class for Cmds that ask for confirmation
////////////////////////////////////////////////////////////////
#ifndef ASKFIRSTCMD_H
#define ASKFIRSTCMD_H

#ifndef I_HAVE_NO_IDENT
#endif

#include "Cmd.h"
#include "DialogManager.h"

class AskFirstCmd : public Cmd {
    
  private:
    
    // Callback for the yes choice on the dialog
    
    static void yesCallback ( void * );
    static void cancelCallback ( void *clientData );
    
    //  Derived classes should use setQuestion to change
    // the string displayed in the dialog
    
    char *_question;
    DialogManager *_dialog;

#ifndef CPLUSPLUS2_1
  protected:    

    Widget _dialogParentWidget;

    virtual void doit()   = 0;  // Specific actions must be defined    
    virtual void undoit()   = 0;  // Specific actions must be defined    
#endif

  public:
    
    AskFirstCmd ( char *, char *, int );
    
    void setQuestion ( char *str );
    
    virtual void execute(); // Overrides the Cmd member function

    virtual void doYesCallback();
    
    virtual const char *const className ()  { return "AskFirstCmd"; }
};
#endif
