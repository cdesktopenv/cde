/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmCDecor.h /main/4 1995/11/01 11:33:13 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern Boolean FrameWindow (ClientData *pcd);
extern void FrameExposureProc (ClientData *pcd);
extern void BaseWinExposureProc (ClientData *pcd);
extern void SetFrameShape (ClientData *pcd);
extern Boolean ConstructFrame (ClientData *pcd);
extern void GenerateFrameDisplayLists (ClientData *pcd);
extern void AdoptClient (ClientData *pcd);
extern void GetTextBox (ClientData *pcd, XRectangle *pBox);
extern void DrawWindowTitle (ClientData *pcd, Boolean eraseFirst);
extern void CreateStretcherWindows (ClientData *pcd);
extern void CountFrameRectangles (WmScreenData *pSD);
extern Boolean AllocateFrameDisplayLists (ClientData *pcd);
extern void InitClientDecoration (WmScreenData *pSD);
extern Boolean AllocateGadgetRectangles (ClientData *pcd);
extern void ComputeGadgetRectangles (ClientData *pcd);
extern void GetSystemMenuPosition (ClientData *pcd, int *px, int *py, unsigned int height, Context context);
extern void ShowActiveClientFrame (ClientData *pcd);
extern void ShowInactiveClientFrame (ClientData *pcd);
extern void RegenerateClientFrame (ClientData *pcd);
extern void BevelSystemButton (RList *prTop, RList *prBot, int x, int y, unsigned int width, unsigned int height);
extern void BevelMinimizeButton (RList *prTop, RList *prBot, int x, int y, unsigned int height);
extern void BevelMaximizeButton (RList *prTop, RList *prBot, int x, int y, unsigned int height);
extern Boolean DepressGadget (ClientData *pcd, int gadget, Boolean depressed);
extern void PushGadgetIn (ClientData *pcd, int gadget);
extern void PopGadgetOut (ClientData *pcd, int gadget);
