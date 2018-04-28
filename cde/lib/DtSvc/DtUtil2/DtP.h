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
/*
 * File:         DtP.h $TOG: DtP.h /main/7 1998/07/30 12:12:49 mgreess $
 * Language:     C
 */

#ifndef _DtP_h
#define _DtP_h

#include <X11/Xmd.h>    /* for protocol typedefs */
#include <X11/Intrinsic.h>

#include <Dt/DtPStrings.h>
#include <Dt/DtGetMessageP.h>

#include <bms/sbport.h>
#include <bms/bms.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************************
 *
 * Miscellaneous Data Types
 *
 *********************************/

#define DtChar		XeChar
#define DtString	XeString

/*********************************
 *
 * Initalization
 *
 *********************************/

#define DtToolClass			XeToolClass

/*********************************
 *
 * Global variables (defined in DtUtil.c)
 *
 *********************************/
extern Display 		* _DtDisplay;
extern char 		* _DtApplicationName;
extern char 		* _DtApplicationClass;
extern char 		* _DtToolClass;

extern XtAppContext 	_DtAppContext;
extern XrmDatabase 	_DtResourceDatabase;
extern Widget   	_DtInitTtContextWidget;
extern XtAppContext 	* _DtInitAppContextp;

extern void   _DtAddToResource( Display *, const char * );
extern void   _DtAddResString( Display *, const char *, unsigned int);
extern char * _DtGetResString( Display *dpy, unsigned int);


#define _DT_ATR_RESMGR   (1 << 0)
#define _DT_ATR_PREFS    (1 << 1)

/*
    DESCRIPTION:

	Add strings to XA_RESOURCE_MANAGER property on the default root
	window.  Correctly merges resource specifications with the same
	name and different values.  The new value overwrites the old.

        _DtAddToResource() may be used where you would have used xrdb to 
        add a resource.

    SYNOPSIS:

        void _DtAddToResource(dpy,data)

        Display *dpy;       The application's display structure.

        char *data;          The string to be added to the
                             XA_RESOURCE_MANAGER property.

*/

extern char *_DtCreateDtDirs( Display * );
/*
    DESCRIPTION:

	Creates the directories needed for dt to operate in.  When an
        application saves its state inside a file, it should call this
        routine to set up the directories before saving any files.  The
        routine constructs the path to which all save files should be saved
        to when responding the the WM_SAVE_YOURSELF message issued by the
        session manager.  The routine  returns the path to save to.  It also
        allocates the memory for the path so when you are done with it you
        should free() it.

        WARNING:  If it can't create the directory it returns NULL

    SYNOPSIS:

	dirName = _DtCreateDtDirs (display);

	char *dirName;		The path to save to.

	Display *display;	The application's display structure.
*/

/*
 * The following string globals are available for use by any DT
 * component.  They represent the button labels in most dialogs,
 * and will be automatically localized by DtInitialize().
 */
extern const char * _DtOkString;
extern const char * _DtCancelString;
extern const char * _DtHelpString;
extern const char * _DtApplyString;
extern const char * _DtCloseString;

#ifdef __cplusplus
}
#endif

/* Do not add anything after this endif. */
#endif /* _DtP_h */
