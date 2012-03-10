/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmWinInfo.h /main/5 1995/12/27 17:24:50 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern void FindClientPlacement (ClientData *pCD);
extern void FixWindowConfiguration (ClientData *pCD, unsigned int *pWidth, 
				    unsigned int *pHeight, 
				    unsigned int widthInc, 
				    unsigned int heightInc);
extern void FixWindowSize (ClientData *pCD, unsigned int *pWidth, 
			   unsigned int *pHeight, unsigned int widthInc, 
			   unsigned int heightInc);
extern ClientData *GetClientInfo (WmScreenData *pSD, Window clientWindow, 
				  long manageFlags);
extern ClientData *GetWmClientInfo (WmWorkspaceData *pWS, ClientData *pCD, 
				    long manageFlags);
extern void CalculateGravityOffset (ClientData *pCD, int *xoff, int *yoff);
extern Boolean InitClientPlacement (ClientData *pCD, long manageFlags);
extern void InitCColormapData (ClientData *pCD);
extern void MakeSystemMenu (ClientData *pCD);
extern void PlaceFrameOnScreen (ClientData *pCD, int *pX, int *pY, int w, 
				int h);
extern void PlaceIconOnScreen (ClientData *pCD, int *pX, int *pY);
extern void ProcessMwmHints (ClientData *pCD);
extern void ProcessWmClass (ClientData *pCD);
extern void ProcessWmHints (ClientData *pCD, Boolean firstTime);
extern void ProcessWmIconTitle (ClientData *pCD, Boolean firstTime);
extern void ProcessWmNormalHints (ClientData *pCD, Boolean firstTime, 
				  long manageFlags);
extern void ProcessWmTransientFor (ClientData *pCD);
extern void ProcessWmWindowTitle (ClientData *pCD, Boolean firstTime);
extern Boolean SetupClientIconWindow (ClientData *pCD, Window window);
extern Boolean WmGetWindowAttributes (Window window);
extern void ProcessSmClientID (ClientData *pCD);
extern void ProcessWmSaveHint (ClientData *pCD);
