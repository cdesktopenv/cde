/*
 *+SNOTICE
 *
 *	$XConsortium: QueryDialogManager.C /main/4 1996/04/21 19:43:02 drk $
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

#ifdef DEAD_WOOD

#include "QueryDialogManager.hh"
#include "RoamApp.h"
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

QueryDialogManager *theQueryDialogManager = 
    new QueryDialogManager ( "QueryDialog" );


QueryDialogManager::QueryDialogManager ( char   *name ) 
		   :DialogManager ( name )
                   
{
    // Empty
}

Widget QueryDialogManager::createDialog ( Widget parent )
{

    Widget dialog = XmCreateQuestionDialog ( parent, _name, NULL, 0);
    
    XtVaSetValues ( dialog,
		   XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		   NULL );
    
    return dialog;

}
#endif /* DEAD_WOOD */
