/*
 *+SNOTICE
 *
 *
 *	$XConsortium: DlDynamicLib.hh /main/4 1996/04/21 19:47:26 drk $
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

// This implementation is for platforms that use dlopen/dlsym/dlclose.
//

#if !defined(_DLDYNAMICLIB_HH) && defined(DL_DYNAMIC_LIBS)
#define _DLDYNAMICLIB_HH

#include "DynamicLib.hh"

#include <dlfcn.h>

class DlDynamicLib : public DynamicLib {
  public:
    DlDynamicLib(const char * path);
    virtual ~DlDynamicLib(void);
    void * getSym(const char * symbol);

  private:
    void	*_lib_handle;
};

#endif
