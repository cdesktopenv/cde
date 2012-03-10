/* $XConsortium: PrintI.h /main/1 1996/10/31 11:19:07 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _DtPrintI_h
#define _DtPrintI_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Conditionally defined macros for thread safe DtPrint ******/
#ifdef XTHREADS

#define _DtPrintWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)

#define _DtPrintDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)

#define _DtPrintAppLock(app)		XtAppLock(app)
#define _DtPrintAppUnlock(app)		XtAppUnlock(app)
#define _DtPrintProcessLock()		XtProcessLock()
#define _DtPrintProcessUnlock()		XtProcessUnlock()

#else /* XTHREADS */

#define _DtPrintWidgetToAppContext(w)
#define _DtPrintDisplayToAppContext(d)
#define _DtPrintAppLock(app)
#define _DtPrintAppUnlock(app)
#define _DtPrintProcessLock()
#define _DtPrintProcessUnlock()

#endif /* XTHREADS */

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtPrintI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
