/* $XConsortium: CmdInterface.C /main/4 1996/04/05 16:48:53 mgreess $ */
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
// CmdInterface.C
/////////////////////////////////////////////////////////
#include "CmdInterface.h"
#include "Cmd.h"

CmdInterface::CmdInterface ( Cmd *cmd ) : UIComponent( cmd->name() )
{
    _active = TRUE;
    _cmd    = cmd;
    cmd->registerInterface ( this );
}

void CmdInterface::executeCmdCallback ( Widget, 
				       XtPointer clientData,
				       XtPointer )
{
    CmdInterface *obj = (CmdInterface *) clientData;
    
    obj->_cmd->execute();     
}

void CmdInterface::activate()
{
    if ( _w )
	XtSetSensitive ( _w, TRUE );
    _active = TRUE;
}

void CmdInterface::deactivate()
{
    if ( _w )
	XtSetSensitive ( _w, FALSE );
    _active = FALSE;
}
