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
 * File:         spcd_event.h $XConsortium: spcd_event.h /main/3 1995/11/01 11:22:05 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _SPCD_event_h
#define _SPCD_event_h

#include <bms/SbEvent.h>

extern SbInputId SPCD_AddInput
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
    (int fd, SbInputCallbackProc proc, void* data);
#else
    ();
#endif
    
extern SbInputId SPCD_AddException
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
    (int fd, SbInputCallbackProc proc, void* data);
#else
   ();
#endif

extern void SPCD_RemoveInput
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (SbInputId id);
#else
   ();
#endif

extern void SPCD_RemoveException
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (SbInputId id);
#else
   ();
#endif

extern void SPCD_MainLoopUntil
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (Boolean *flag);
#else
   ();
#endif

extern void SPCD_BreakMainLoop
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void);
#else
   ();
#endif

#endif	/* _SPCD_event_h */
