/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.2
*/ 
/*   $XConsortium: WmInitWs.h /main/5 1996/09/14 14:50:22 drk $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void InitWmGlobal (int argc, char *argv [], char *environ []);
extern void InitWmScreen (WmScreenData *pSD, int sNum);
extern void InitWmWorkspace (WmWorkspaceData *pWS, WmScreenData *pSD);
extern void ProcessMotifWmInfo (Window rootWindowOfScreen);
extern void SetupWmWorkspaceWindows (void);
extern void MakeWorkspaceCursors (void);
extern void MakeWmFunctionResources (WmScreenData *pSD);
extern void MakeXorGC (WmScreenData *pSD);
extern void CopyArgv (int argc, char *argv []);
extern void InitScreenNames (void);
#ifndef NO_MESSAGE_CATALOG
extern void InitNlsStrings( void ) ;
#endif
#ifdef WSM
extern void InitWmDisplayEnv( void ) ;
#endif /* WSM */
