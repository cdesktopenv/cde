/*
 * $XConsortium: TermViewHelp.h /main/1 1996/04/21 19:20:26 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermViewHelp_h
#define	_Dt_TermViewHelp_h

#ifdef	HPVUE
#include <Xvh/Xvh.h>
#include <Xvh/QuickHelpD.h>
#include <Xvh/HelpDialog.h>

#define	DtNcloseCallback		XmNcloseCallback
#define	DtNcolumns			XmNcolumns
#define	DtNhelpType			XmNhelpType
#define	DtNmanPage			XmNmanPage
#define	DtNhyperLinkCallback		XmNhyperLinkCallback
#define	DtNlocationId			XmNlocationId
#define	DtNhelpVolume			XmNhelpVolume
#define	DtNrows				XmNrows

#define	DtHelpDialogCallbackStruct	XvhHelpDialogCallbackStruct

#define	DtHELP_LINK_TOPIC		XvhLINK_JUMP_NEW
#define	DtHELP_LINK_MAN_PAGE		XvhLINK_MAN_PAGE
#define	DtHELP_LINK_APP_DEFINE		XvhLINK_APP_DEFINE

#define	DtHELP_TYPE_MAN_PAGE		XvhHELP_TYPE_MAN_PAGE
#define	DtHELP_TYPE_TOPIC		XvhHELP_TYPE_TOPIC

#define	DtCreateHelpDialog		XvhCreateHelpDialog
#define	DtCreateHelpQuickDialog		XvhCreateQuickHelpDialog

#else	/* HPVUE */
#include <Dt/Help.h>
#include <Dt/HelpQuickD.h>
#include <Dt/HelpDialog.h>
#endif	/* HPVUE */

#endif	/* _Dt_TermViewHelp_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
