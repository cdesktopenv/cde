/*
 * File:         SbEvent.h $XConsortium: SbEvent.h /main/3 1995/10/26 15:45:29 rswiston $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _SbEvent_h
#define _SbEvent_h

typedef unsigned long SbInputId;

typedef void (*SbInputCallbackProc)
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void* data, int *fd, SbInputId *id);
#else
   ();
#endif
    
extern SbInputId (*SbAddInput_hookfn)
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
    (int fd, SbInputCallbackProc proc, void* data);
#else
    ();
#endif
    
extern SbInputId (*SbAddException_hookfn)
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
    (int fd, SbInputCallbackProc proc, void* data);
#else
   ();
#endif

extern void  (*SbRemoveInput_hookfn)    
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (SbInputId id);
#else
   ();
#endif

extern void  (*SbRemoveException_hookfn)
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (SbInputId id);
#else
   ();
#endif

extern void  (*SbMainLoopUntil_hookfn)  
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (Boolean *flag);
#else
   ();
#endif

extern void XeCall_SbMainLoopUntil
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (Boolean *flag);
#else
   ();
#endif

extern void  (*SbBreakMainLoop_hookfn)  
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (void);
#else
   ();
#endif


#endif /* _SbEvent_h */
