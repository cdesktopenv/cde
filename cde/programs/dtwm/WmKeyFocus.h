/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmKeyFocus.h /main/4 1995/11/01 11:43:42 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern void AutoResetKeyFocus (ClientData *pcdNoFocus, Time focusTime);
extern void ClearFocusIndication (ClientData *pCD, Boolean refresh);
extern Boolean CheckForKeyFocus (ClientListEntry *pNextEntry, 
				 unsigned long type, Boolean focusNext, 
				 Time focusTime);
extern void DoExplicitSelectGrab (Window window);
extern ClientData *FindLastTransientTreeFocus (ClientData *pCD, 
					       ClientData *pcdNoFocus);
extern ClientData *FindNextTFocusInSeq (ClientData *pCD, 
					unsigned long startAt);
extern ClientData *FindPrevTFocusInSeq (ClientData *pCD, 
					unsigned long startAt);
extern Boolean FocusNextTransient (ClientData *pCD, unsigned long type, 
				   Boolean initiate, Time focusTime);
extern Boolean FocusNextWindow (unsigned long type, Time focusTime);
extern Boolean FocusPrevTransient (ClientData *pCD, unsigned long type, 
				   Boolean initiate, Time focusTime);
extern Boolean FocusPrevWindow (unsigned long type, Time focusTime);
extern ClientData *GetClientUnderPointer (Boolean *pSameScreen);
extern void InitKeyboardFocus (void);
extern void RepairFocus (void);
extern void ResetExplicitSelectHandling (ClientData *pCD);
extern void SetFocusIndication (ClientData *pCD);
extern void SetKeyboardFocus (ClientData *pCD, long focusFlags);



