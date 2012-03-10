/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.3
*/ 
/*   $XConsortium: WmProperty.h /main/4 1995/11/01 11:48:54 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern SizeHints * GetNormalHints (ClientData *pCD);
extern void ProcessWmProtocols (ClientData *pCD);
extern void ProcessMwmMessages (ClientData *pCD);
extern void SetMwmInfo (Window propWindow, long flags, Window wmWindow);
#ifdef WSM
void SetMwmSaveSessionInfo (Window wmWindow);
#endif /* WSM */
extern PropWMState * GetWMState (Window window);
extern void SetWMState (Window window, int state, Window icon);
extern PropMwmHints * GetMwmHints (ClientData *pCD);
extern PropMwmInfo * GetMwmInfo (Window rootWindowOfScreen);
extern void ProcessWmColormapWindows (ClientData *pCD);
extern Colormap FindColormap (ClientData *pCD, Window window);
extern MenuItem * GetMwmMenuItems (ClientData *pCD);
#ifdef WSM
extern void GetInitialPropertyList (ClientData *pCD);
extern Status GetWorkspaceHints (Display *display, Window window, Atom **ppWsAtoms, unsigned int *pCount, Boolean *pbAll);
#ifdef HP_VUE
extern void SetWorkspaceInfo (Window propWindow, WorkspaceInfo *pWsInfo, unsigned long cInfo);
#endif /* HP_VUE */
extern void SetWorkspacePresence (Window propWindow, Atom *pWsPresence, unsigned long cPresence);
extern Boolean HasProperty(ClientData *pCD, Atom property);
extern void DiscardInitialPropertyList (ClientData *pCD);
extern void GetInitialPropertyList (ClientData *pCD);
extern void SetWorkspaceListProperty (WmScreenData *pSD);
extern void SetCurrentWorkspaceProperty (WmScreenData *pSD);
extern void SetWorkspaceInfoProperty (WmWorkspaceData *pWS);
extern void DeleteWorkspaceInfoProperty (WmWorkspaceData *pWS);
extern char *WorkspacePropertyName (WmWorkspaceData *pWS);
#endif /* WSM */
