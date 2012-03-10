/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmManage.h /main/4 1995/11/01 11:44:22 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void AdoptInitialClients (WmScreenData *pSD);
extern void DeleteClientContext (ClientData *pCD);
extern void ManageWindow (WmScreenData *pSD, Window clientWindow, long manageFlags);
extern void UnManageWindow (ClientData *pCD);
extern void WithdrawTransientChildren (ClientData *pCD);
extern void WithdrawWindow (ClientData *pCD);
extern void ResetWithdrawnFocii (ClientData *pCD);
extern void FreeClientFrame (ClientData *pCD);
extern void FreeIcon (ClientData *pCD);
extern void WithdrawDialog (Widget dialogboxW);
extern void ReManageDialog (WmScreenData *pSD, Widget dialogboxW);
#ifdef PANELIST
extern void RegisterEmbeddedClients (
	Widget wPanelist, 
	WmFpEmbeddedClientList pECD, 
	int count);
extern void RegisterPushRecallClients (
	Widget wPanelist, 
	WmFpPushRecallClientList pPRCD, 
	int count);
extern void UnParentControls(WmScreenData *pSD, Boolean unmap);
extern void RegisterIconBoxControl (Widget wPanelist);
extern Boolean ReparentEmbeddedClient (
	WmFpEmbeddedClientData *pECD,
	Widget newControl,
	Window newWin,
	int x, 
	int y,
	unsigned int width, 
	unsigned int height);
extern void ScanForEmbeddedClients (WmScreenData *pSD);
extern void ScanForPushRecallClients (WmScreenData *pSD);
#endif /* PANELIST */
