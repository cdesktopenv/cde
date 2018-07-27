/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
_fallcDynamicLoader(char *name)
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
