/* $XConsortium: _fallcDynamic.c /main/3 1996/05/08 19:00:37 drk $ */
/* lcDynamic.c 1.1 - Fujitsu source for CDEnext    95/12/07 10:56:55 	*/ 
/*
 * Copyright 1995 by FUJITSU LIMITED
 * This is source code modified by FUJITSU LIMITED under the Joint
 * Development Agreement for the CDEnext PST.
 * This is unpublished proprietry source code of FUJITSU LIMITED
 *
 * Modifier: Takanori Tateno   FUJITSU LIMITED
 *
 */
#include <stdio.h>
#include <string.h>
#include "_fallibint.h"
#ifdef USE_DYNAMIC_LOADER

#ifndef XLOCALEDIR
#define XLOCALEDIR "/usr/lib/X11/locale"
#endif

#define LCLIBNAME    "xi18n.so"

extern void *dlopen();
extern void *dlsym();
extern int dlclose();
extern char *dlerror();

#define LAZY       1
#define NOW        2
#define GLOBAL     0x100

XLCd
_fallcDynamicLoader(name)
    char *name;
{
    char libpath[1024];
    XLCdMethods _fallcGenericMethods;
    XLCd lcd;
    void *nlshandler;

    sprintf(libpath,"%s/%s/%s",
		XLOCALEDIR,name,LCLIBNAME);
    nlshandler = dlopen(libpath,LAZY);
    _fallcGenericMethods = (XLCdMethods)dlsym(nlshandler,
				"genericMethods");
    lcd = _fallcCreateLC(name,_fallcGenericMethods);
    

    return lcd;
}
#endif /* USE_DYNAMIC_LOADER */
