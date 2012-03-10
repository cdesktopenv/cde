/* $XConsortium: syncx.h /main/2 1996/06/25 20:04:52 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include "_falutil.h"

extern XrmQuark falrmStringToQuark(
#if NeedFunctionPrototypes
    _Xconst char* 	/* string */
#endif
);
extern XrmString falrmQuarkToString(
#if NeedFunctionPrototypes
    XrmQuark 		/* quark */
#endif
);

extern char *falGetAtomName(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Atom		/* atom */
#endif
);

extern char **falListFonts(
#if NeedFunctionPrototypes
    Display*		/* display */,
    _Xconst char*	/* pattern */,
    int			/* maxnames */,
    int*		/* actual_count_return */
#endif
);

extern char **falListFontsWithInfo(
#if NeedFunctionPrototypes
    Display*		/* display */,
    _Xconst char*	/* pattern */,
    int			/* maxnames */,
    int*		/* count_return */,
    XFontStruct**	/* info_return */
#endif
);

extern XFontStruct *falLoadQueryFont(
#if NeedFunctionPrototypes
    Display*		/* display */,
    _Xconst char*	/* name */
#endif
);

