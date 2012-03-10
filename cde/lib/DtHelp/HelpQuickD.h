/* $XConsortium: HelpQuickD.h /main/3 1995/10/26 12:25:25 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_HelpQuickD_h
#define _Dt_HelpQuickD_h

#include <Dt/Help.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* Quick help dialog-specific Resources */

#ifndef DtNbackLabelString
#define DtNbackLabelString		"backLabelString"
#endif
#ifndef DtNhelpLabelString
#define DtNhelpLabelString		XmNhelpLabelString
#endif
#ifndef DtNmoreLabelString
#define DtNmoreLabelString		"moreLabelString"
#endif
#ifndef DtNcloseLabelString
#define DtNcloseLabelString		"closeLabelString"
#endif
#ifndef DtNprintLabelString
#define DtNprintLabelString		"printLabelString"
#endif

#ifndef DtCCallback
#define DtCCallback                     XmCCallback
#endif

#ifndef DtCBackLabelString
#define DtCBackLabelString		"BackLabelString"
#endif
#ifndef DtCHelpLabelString
#define DtCHelpLabelString		XmCHelpLabelString
#endif
#ifndef DtCMoreLabelString
#define DtCMoreLabelString		"MoreLabelString"
#endif
#ifndef DtCCloseLabelString
#define DtCCloseLabelString		"CloseLabelString"
#endif
#ifndef DtCPrintLabelString
#define DtCPrintLabelString		"PrintLabelString"
#endif


/*
 * Types
 */

/* Widget class and instance */

typedef struct _DtHelpQuickDialogWidgetClassRec * DtHelpQuickDialogWidgetClass;
typedef struct _DtHelpQuickDialogWidgetRec   * DtHelpQuickDialogWidget;


/*
 * Data
 */

/* Widget class record */

externalref WidgetClass dtHelpQuickDialogWidgetClass;


/*
 * Functions
 */

/* tmp backwards compat */
#define DtCreateQuickHelpDialog  DtCreateHelpQuickDialog

extern Widget DtCreateHelpQuickDialog(
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

extern Widget DtHelpQuickDialogGetChild(
		Widget		widget,
		unsigned char	child);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_HelpQuickD_h */
