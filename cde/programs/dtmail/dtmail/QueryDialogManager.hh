/*
 *+SNOTICE
 *
 *	$XConsortium: QueryDialogManager.hh /main/4 1996/04/21 19:43:05 drk $
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

///////////////////////////////////////////////////////////
// QueryDialogManager.hh
//////////////////////////////////////////////////////////

#ifdef DEAD_WOOD

#ifndef QUERYDIALOGMANAGER_HH
#define QUERYDIALOGMANAGER_HH

#include "DialogManager.h"

class QueryDialogManager : public DialogManager {

protected:
    Widget createDialog ( Widget );

public:
  
    QueryDialogManager ( char * );
};

extern QueryDialogManager *theQueryDialogManager;

#endif

#endif /* DEAD_WOOD */
