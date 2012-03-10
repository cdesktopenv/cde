/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmIPlace.h /main/4 1995/11/01 11:41:34 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void InitIconPlacement (WmWorkspaceData *pWS);
extern int GetNextIconPlace (IconPlacementData *pIPD);
extern void CvtIconPlaceToPosition (IconPlacementData *pIPD, int place, int *pX, int *pY);
extern int FindIconPlace (ClientData *pCD, IconPlacementData *pIPD, int x, int y);
extern int CvtIconPositionToPlace (IconPlacementData *pIPD, int x, int y);
extern void PackRootIcons (void);
extern void MoveIconInfo (IconPlacementData *pIPD, int p1, int p2);
