/*******************************************************************************
**
**  agent.x
**
**  static char sccsid[] = "@(#)agent.x 1.5 94/07/16 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: agent.x /main/1 1996/04/21 19:21:23 drk $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/
%#include "rtable4.h"

enum Update_Status {
	update_succeeded=0,
	update_failed=1
};

/*	The AGENTPROG actually isn't used for callback.
	A transient number will be generated instead.  It's
	just declared here as a "syntax" holder for rpcgen
*/
program AGENTPROG {
        version AGENTVERS {
                Update_Status update_callback(Table_Res_4) = 1;
        }=1;
}=00;
