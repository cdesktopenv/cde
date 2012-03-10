/*
 *+SNOTICE
 *
 *	$XConsortium: DtMailWDM.hh /main/4 1996/04/21 19:41:48 drk $
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

//////////////////////////////////////////////////////////
// DtMailWDM.hh
//////////////////////////////////////////////////////////
#ifndef DTMAILWDM_HH
#define DTMAILWDM_HH

#include "WorkingDialogManager.h"

class DtMailWDM : public WorkingDialogManager {
    
  protected:
    
    char *_text;
    
  public:
    
    DtMailWDM ( char * );
    
    virtual Widget post (char *,
			 char *, 
			 Widget ,
			 void *clientData      = NULL,
			 DialogCallback ok     = NULL,
			 DialogCallback cancel = NULL,
			 DialogCallback help   = NULL );

    virtual Widget post (char *,
			 char *, 
			 void *clientData      = NULL,
			 DialogCallback ok     = NULL,
			 DialogCallback cancel = NULL,
			 DialogCallback help   = NULL );
    
    void updateAnimation();
    void updateDialog(char *msg);
};

extern DtMailWDM *theDtMailWDM;

#endif
