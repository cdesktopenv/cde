/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmIDecor.h /main/4 1995/11/01 11:40:37 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern void CreateActiveIconTextWindow (WmScreenData *pSD);
extern void DrawIconTitle (ClientData *pcd);
extern void GetIconDimensions (WmScreenData *pSD, unsigned int *pWidth, 
			       unsigned int *pLabelHeight, 
			       unsigned int *pImageHeight);
extern void GetIconTitleBox (ClientData *pcd, XRectangle *pBox);
extern void HideActiveIconText (WmScreenData *pSD);
extern void IconExposureProc (ClientData *pcd, Boolean clearFirst);
extern void InitIconSize (WmScreenData *pSD);
extern Boolean MakeIcon (WmWorkspaceData *pWS, ClientData *pcd);
extern void MakeIconShadows (ClientData *pcd, int xOffset, int yOffset);
extern void MoveActiveIconText (ClientData *pcd);
extern void PaintActiveIconText (ClientData *pcd, Boolean erase);
extern void PutBoxInIconBox (ClientData *pCD, int *px, int *py, 
			     unsigned int *width, unsigned int *height);
extern void PutBoxOnScreen (int screen, int *px, int *py, unsigned int width, 
			    unsigned int height);
extern void RedisplayIconTitle (ClientData *pcd);
extern void ReparentIconWindow (ClientData *pcd, int xOffset, int yOffset);
extern void ShowActiveIcon (ClientData *pcd);
extern void ShowActiveIconText (ClientData *pcd);
extern void ShowInactiveIcon (ClientData *pcd, Boolean refresh);


