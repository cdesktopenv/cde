/*
 * $XConsortium: TermViewMenu.h /main/1 1996/04/21 19:20:32 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */


#ifndef	_Dt_TermViewMenu_h
#define	_Dt_TermViewMenu_h

extern Widget
_DtTermViewCreatePulldownMenu
(
    Widget		  termView,
    Widget		  parent,
    Arg			  menuArglist[],
    int			  menuArgcount
);

extern Widget
_DtTermViewCreatePopupMenu
(
    Widget		  termView,
    Widget		  parent,
    Arg			  menuArglist[],
    int			  menuArgcount
);

extern Widget
_DtTermViewCreateCascadeButton
(
    Widget		  parent,
    Widget		  subMenuId,
    char		 *label,
    KeySym		  mnemonic,
    char		 *accelerator,
    char		 *acceleratorText,
    XtCallbackProc	  callback,
    XtPointer		  clientData
);

extern Widget
_DtTermViewCreatePushButton
(
    Widget		  parent,
    char		 *label,
    KeySym		  mnemonic,
    char		 *accelerator,
    char		 *acceleratorText,
    XtCallbackProc	  callback,
    XtPointer		  clientData
);

extern Widget
_DtTermViewCreateToggleButton
(
    Widget		  parent,
    char		 *label,
    KeySym		  mnemonic,
    char		 *accelerator,
    char		 *acceleratorText,
    XtCallbackProc	  callback,
    XtPointer		  clientData
);

extern Widget
_DtTermViewCreateSeparator
(
    Widget		  parent,
    char		 *label
);

extern void
_HptermViewMenuToggleMenuBar
(
    Widget		  w
);

extern int
_DtTermViewGetUserFontListIndex
(
    Widget		  w
);

extern void
_DtTermViewSetUserFontListIndex
(
    Widget		  w,
    int			  i
);

#endif	/* _Dt_TermViewMenu_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
