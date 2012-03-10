/* $XConsortium: InfoDialogManager.C /main/3 1996/04/21 19:32:17 drk $ */
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


///////////////////////////////////////////////////////////
// InfoDialogManager.C: 
//////////////////////////////////////////////////////////
#include "InfoDialogManager.h"
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

DialogManager *theInfoDialogManager = 
new InfoDialogManager ( "InformationDialog" );

InfoDialogManager::InfoDialogManager ( char   *name ) : 
                                   DialogManager ( name )
{
    // Empty
}

Widget InfoDialogManager::createDialog ( Widget parent )
{
    Widget dialog = XmCreateInformationDialog ( parent, _name, NULL, 0 );
    
    return dialog;
}
