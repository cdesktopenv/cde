/*
 *+SNOTICE
 *
 *
 *	$XConsortium: DlDynamicLib.C /main/4 1996/04/21 19:47:23 drk $
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

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

#include "DynamicLib.hh"
#include "DlDynamicLib.hh"

DlDynamicLib::DlDynamicLib(const char * path) 
: DynamicLib(path)
{
    _lib_handle = dlopen(path, RTLD_LAZY);
    return;
}

DlDynamicLib::~DlDynamicLib(void)
{
    if (_lib_handle) {
	dlclose(_lib_handle);
    }
}

void *
DlDynamicLib::getSym(const char * sym)
{
    return(dlsym(_lib_handle, sym));
}
