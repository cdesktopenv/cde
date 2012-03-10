/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmColormap.h /main/4 1995/11/01 11:35:11 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void InitWorkspaceColormap (WmScreenData *pSD);
extern void InitColormapFocus (WmScreenData *pSD);
#ifndef OLD_COLORMAP
extern void ForceColormapFocus (WmScreenData *pSD, ClientData *pCD);
#endif
extern void SetColormapFocus (WmScreenData *pSD, ClientData *pCD);
extern void WmInstallColormap (WmScreenData *pSD, Colormap colormap);
extern void ResetColormapData (ClientData *pCD, Window *pWindows, int count);
#ifndef IBM_169380
extern void AddColormapWindowReference (ClientData *pCD, Window window);
extern void RemoveColormapWindowReference (ClientData *pCD, Window window);
#endif
extern void ProcessColormapList (WmScreenData *pSD, ClientData *pCD);
