/* $XConsortium: MenuButton.h /main/3 1995/10/26 09:33:10 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_MenuButton_h
#define _Dt_MenuButton_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Constants
 */

/* Resources */

#ifndef DtNcascadingCallback
#define DtNcascadingCallback	XmNcascadingCallback
#endif
#ifndef DtNsubMenuId
#define DtNsubMenuId		XmNsubMenuId
#endif
#ifndef DtNcascadePixmap
#define DtNcascadePixmap	XmNcascadePixmap
#endif

#ifndef DtCCallback
#define DtCCallback		XmCCallback
#endif
#ifndef DtCPixmap
#define DtCPixmap		XmCPixmap
#endif
#ifndef DtCMenuWidget
#define DtCMenuWidget		XmCMenuWidget
#endif

#ifndef DtCR_CASCADING
#define DtCR_CASCADING		XmCR_CASCADING
#endif


/*
 * Types
 */

/* Widget class and instance */

typedef struct _DtMenuButtonRec      * DtMenuButtonWidget;
typedef struct _DtMenuButtonClassRec * DtMenuButtonWidgetClass;


/*
 * Data
 */

/* Widget class record */

externalref WidgetClass dtMenuButtonWidgetClass;


/*
 * Functions
 */

extern Widget DtCreateMenuButton( 
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_MenuButton_h */
