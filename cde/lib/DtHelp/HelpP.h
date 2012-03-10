/* $XConsortium: HelpP.h /main/3 1995/10/26 12:24:50 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpP.h
 **
 **   Project:     CacheCreeek (Rivers) Project.
 **
 **   
 **   Description: Private header file for Dts.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtHelpP_h
#define	_DtHelpP_h

/* Privite Argument Resource Definitions for Help Widget */

#define DtNshowTopLevelButton   "showTopLevelButton"
#ifndef XmNshowTopLevelButton
#define XmNshowTopLevelButton	DtNshowTopLevelButton
#endif
#define DtCShowTopLevelButton   "ShowTopLevelButton"
#ifndef XmCShowTopLevelButton
#define XmCShowTopLevelButton	DtCShowTopLevelButton
#endif

#define DtNsrchHitPrefixFont    "srchHitPrefixFont"
#ifndef XmNsrchHitPrefixFont
#define XmNsrchHitPrefixFont	DtNsrchHitPrefixFont
#endif
#define DtCSrchHitPrefixFont   "SrchHitPrefixFont"
#ifndef XmCSrchHitPrefixFont
#define XmCSrchHitPrefixFont  	DtCSrchHitPrefixFont
#endif


#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP "/etc/dt"
#endif

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif


#define DtDEFAULT_SYSTEM_PATH \
                 CDE_CONFIGURATION_TOP "/appconfig/help/%L/%H:" \
                 CDE_CONFIGURATION_TOP "/appconfig/help/%L/%H.sdl:" \
                 CDE_CONFIGURATION_TOP "/appconfig/help/%L/%H.hv:" \
                 CDE_CONFIGURATION_TOP "/appconfig/help/C/%H:" \
                 CDE_CONFIGURATION_TOP "/appconfig/help/C/%H.sdl:" \
                 CDE_CONFIGURATION_TOP "/appconfig/help/C/%H.hv:" \
                 CDE_INSTALLATION_TOP "/appconfig/help/%L/%H:" \
                 CDE_INSTALLATION_TOP "/appconfig/help/%L/%H.sdl:" \
                 CDE_INSTALLATION_TOP "/appconfig/help/%L/%H.hv" \
                 CDE_INSTALLATION_TOP "/appconfig/help/C/%H:" \
                 CDE_INSTALLATION_TOP "/appconfig/help/C/%H.sdl:" \
                 CDE_INSTALLATION_TOP "/appconfig/help/C/%H.hv:"


#define DtDEFAULT_USER_PATH_FORMAT \
		"%s/.dt/help/%s/%%H:" \
		"%s/.dt/help/%s/%%H.sdl:" \
		"%s/.dt/help/%s/%%H.hv:" \
		"%s/.dt/help/%%H:" \
		"%s/.dt/help/%%H.sdl:" \
		"%s/.dt/help/%%H.hv"

/* this path expects a sprintf usage as follows:
    sprintf(buf,DtDEFAULT_USER_PATH_FORMAT,
   			homedir, dtusersessionname,
			homedir, dtusersessionname,
			homedir, dtusersessionname,
			homedir, homedir, homedir);
*/

/* #define DtVOLUMES_TYPE "volumes" */

#endif /* _DtHelpP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */





