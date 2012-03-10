/* $XConsortium: WarnNoUndoCmd.C /main/4 1995/12/07 15:28:22 rswiston $ */
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


//////////////////////////////////////////////////////////
// WarnNoUndoCmd.C: Warns user before executing a command
//////////////////////////////////////////////////////////
#include "WarnNoUndoCmd.h"

#include <nl_types.h>
extern nl_catd catd;

#include "NLS.hh"

WarnNoUndoCmd::WarnNoUndoCmd ( char *name, char *label, int active) : 
                     AskFirstCmd ( name, label, active )
{
    _hasUndo = 0;     // Specify that there is no undo
    
    setQuestion ( GETMSG(catd, 1, 10,
	   "This command cannot be undone. Proceed anyway?") );
}

void WarnNoUndoCmd::undoit()
{
    // Empty
} 
