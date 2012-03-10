/*
 * $XConsortium: TermViewGlobalDialog.h /main/1 1996/04/21 19:20:23 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermViewGlobalDialog_h
#define	_Dt_TermViewGlobalDialog_h

extern Widget _DtTermViewCreateGlobalOptionsDialog(Widget parent);

typedef struct _lineupList {
    Widget		  left;
    Dimension		  widthLeft;
    Dimension		  marginLeft;
    Dimension		  marginRight;
    Dimension		  marginWidth;
    Dimension		  correction;
    Widget		  right;
    struct _lineupList	 *next;
} _DtTermViewLineupList;


#define	CORRECTION_LABEL	7
#define	CORRECTION_OPTION_MENU	0
#define	CORRECTION_TEXT_OFFSET	-1

/* private function declarations... */
extern _DtTermViewLineupList * _DtTermViewLineupListCreate();
extern void _DtTermViewLineupListAddItem(
    _DtTermViewLineupList *list,
    Widget		  left,
    Widget		  right,
    Dimension		  correction);
extern void _DtTermViewLineupListLineup(
    _DtTermViewLineupList *list);

extern void _DtTermViewLineupListFree(
    _DtTermViewLineupList *list);
extern Widget _DtTermViewCreatePulldown(
    Widget		  parent,
    char		 *name);
extern Widget _DtTermViewCreateOptionMenu(
    Widget		  parent,
    Widget		  submenu,
    char		 *label,
    KeySym		  mnemonic,
    Arg			  al[],
    int			  ac);
extern void _DtTermViewEqualizeHeights(
    Widget		  label,
    Widget		  text);

#endif	/* _Dt_TermViewGlobalDialog_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
