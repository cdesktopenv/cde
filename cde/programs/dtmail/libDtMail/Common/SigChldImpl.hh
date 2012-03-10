/*
 *+SNOTICE
 *
 *
 *	$XConsortium: SigChldImpl.hh /main/4 1996/04/21 19:49:01 drk $
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

#ifndef _SIGCHLDIMPL_HH
#define _SIGCHLDIMPL_HH

#include <DtMail/DtVirtArray.hh>
#include <DtMail/Threads.hh>

struct SigChldInfo : public DtCPlusPlusAllocator {
    int		pid;
    Condition	cond;
};

extern DtVirtArray<SigChldInfo *>	*DtMailSigChldList;

#endif
