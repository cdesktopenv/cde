/* $XConsortium: HelpDialog.h /main/3 1995/10/26 12:24:05 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_HelpDialog_h
#define _Dt_HelpDialog_h

#include <Dt/Help.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* General Help Dialog-specific Resources */

#ifndef DtNmarginWidth
#define DtNmarginWidth			XmNmarginWidth
#endif
#ifndef DtNmarginHeight
#define DtNmarginHeight			XmNmarginHeight
#endif
#ifndef DtNshowNewWindowButton
#define DtNshowNewWindowButton		"showNewWindowButton"
#endif
#ifndef DtNtopicTitle
#define DtNtopicTitle			"topicTitle"
#endif
#ifndef DtNvisiblePathCount
#define DtNvisiblePathCount		"visiblePathCount"
#endif

#ifndef DtCCallback
#define DtCCallback                     XmCCallback
#endif

#ifndef DtCMarginWidth
#define DtCMarginWidth			XmCMarginWidth
#endif
#ifndef DtCMarginHeight
#define DtCMarginHeight			XmCMarginHeight
#endif
#ifndef DtCShowNewWindowButton
#define DtCShowNewWindowButton		"ShowNewWindowButton"
#endif
#ifndef DtCTopicTitle
#define DtCTopicTitle			"TopicTitle"
#endif
#ifndef DtCVisiblePathCount
#define DtCVisiblePathCount		"VisiblePathCount"
#endif


/*
 * Types
 */

/* Widget class and instance */

typedef struct _DtHelpDialogWidgetClassRec * DtHelpDialogWidgetClass;
typedef struct _DtHelpDialogWidgetRec      * DtHelpDialogWidget;


/*
 * Data
 */

/* Widget class record */

externalref WidgetClass dtHelpDialogWidgetClass;


/*
 * Functions
 */

extern Widget DtCreateHelpDialog(
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_HelpDialog_h */
