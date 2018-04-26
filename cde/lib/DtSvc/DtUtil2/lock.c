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
 * File:         lock.c $XConsortium: lock.c /main/4 1995/10/26 15:33:05 rswiston $
 * Language:     C
 *
 * (C) Copyright 1989, Hewlett-Packard, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <X11/Xlib.h>


/********    Public Function Declarations    ********/

extern int _DtGetLock( Display *display, char *lock_name) ;
extern void _DtReleaseLock( Display *display, char *lock_name) ;
extern int _DtTestLock( Display *display, char *lock_name) ;

/********    End Public Function Declarations    ********/


int 
_DtGetLock(
        Display *display,
        char *lock_name )
{
   Atom lock_atom;
   Window selection_owner;
   Window root_window;
  
   /* Do as much processing as possible before grabbing the server. */
   lock_atom = XInternAtom (display, lock_name, False);
   root_window = DefaultRootWindow (display);

   /* Do an atomic test-and-set of the lock.  To make this atomic we
      must grab the server. */
   XGrabServer (display);
   if ((selection_owner = XGetSelectionOwner (display, lock_atom)) == None)
      XSetSelectionOwner (display, lock_atom, root_window, CurrentTime);
   XUngrabServer (display);
   XFlush (display);
  
   /* If the lock was clear then it was successfully acquired. */
   if (selection_owner == None)
      return (1);
   else
      return (0);
}

void 
_DtReleaseLock(
        Display *display,
        char *lock_name )
{
   Atom lock_atom;
  
   lock_atom = XInternAtom (display, lock_name, False);
   XSetSelectionOwner (display, lock_atom, None, CurrentTime);
   XFlush (display);  /* added to release lock NOW */
}

int 
_DtTestLock(
        Display *display,
        char *lock_name )
{
   Atom lock_atom;
  
   lock_atom = XInternAtom (display, lock_name, False);
   
   if (XGetSelectionOwner (display, lock_atom) == None)
      return (0);
   else 
      return (1);
}
      
