/*
 *+SNOTICE
 *
 *
 *	$XConsortium: LanguagePortability.C /main/4 1996/04/21 19:48:27 drk $
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

#include <stdlib.h>
#include <DtMail/DtLanguages.hh>

void *
DtCPlusPlusAllocator::operator new(size_t size)
{
    return(malloc(size));
}

void
DtCPlusPlusAllocator::operator delete(void * ptr)
{
    free(ptr);
}
