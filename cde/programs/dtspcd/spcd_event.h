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
