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
 * File:         Lock.h $XConsortium: Lock.h /main/4 1995/10/26 15:24:02 rswiston $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_lock_h
#define _Dt_lock_h

/*
    GENERAL DESCRIPTION:

        The DT lock facility provides simple exclusive locking.  It
        (as of 6/19/90) is based on the X11 selection-ownership
        mechanism, though users of Dt locking do not need to be aware of
        this.

	X11 server grabs are judiciously used to guarantee atomicity of
        operations.  If a process which holds a lock dies (or closes its
        X11 server connection for some other reason), the lock will be
        automatically released.

	Locks are identified by a string.  There is no mechanism to
        allocate unique lock strings to clients; users must take care to
        choose a string that will not be easily duplicated by some other
        client.

    SAMPLE CODE:

        #define MY_LOCK "MYAPP_MY_LOCK"

	...

	if (_DtGetLock (display, MY_LOCK)) {
	   <do whatever it is I want to do>
	   _DtReleaseLock (display, MY_LOCK);
        }
        else {
	   <do the alternative>
        }
*/

extern int _DtGetLock (
   Display *display,
   char  *lock_name);
/*
    DESCRIPTION:
    
    	_DtGetLock attempts to get the specified lock.  If nobody holds
        the lock, _DtGetLock will obtain the lock and return 1.  If
        somebody else already holds the lock, the lock will not be
        disturbed and _DtGetLock will return 0.

	If the process which owns a lock dies (or closes its X11 server
        connection), the lock will be automatically released.  To
        explicitly release a lock, use _DtReleaseLock.

	
    SYNOPSIS:

        success = _DtGetLock (display, lock);

	int success;		Returns 1 if the lock is obtained, 
				0 if not.

	Display *display;	The X11 server connection which will 
				hold the lock.

        char *lock;		The string which names the lock.
*/

extern void _DtReleaseLock (
   Display *display,
   char *lock_name);
/*
    DESCRIPTION:
    
        _DtReleaseLock releases a lock obtained by _DtGetLock.

	WARNING!!  It is perfectly legal for one process to release
	a lock held by another process.  By convention you should only
	release locks previously obtained by your process from _DtGetLock
        unless you are playing God and know what you are doing.

    SYNOPSIS:

        (void) _DtReleaseLock (display, lock);

	Display *display;	The X11 server connection which holds
				the lock.

        char *lock;		The string which names the lock.
*/

extern int _DtTestLock (
   Display *display,
   char *lock_name);
/*
    DESCRIPTION:
    
    	_DtTestLock returns a status indicating whether anybody holds the
        specified lock.
	
    SYNOPSIS:

        status = _DtTestLock (display, lock);

	int success;		Returns 1 if anybody holds the lock,
				0 otherwise.

	Display *display;	The X11 server connection.

        char *lock;		The string which names the lock.
*/
	
#endif /* _Dt_lock_h */
/* Do not add anything after this endif. */
