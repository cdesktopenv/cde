/* $XConsortium: WmPresence.h /main/4 1995/11/01 11:48:09 rswiston $ */
#ifdef WSM
/* 
 * (c) Copyright 1987, 1988, 1989, 1990, 1992 HEWLETT-PACKARD COMPANY 
 * ALL RIGHTS RESERVED 
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

/********    Public Function Declarations    ********/

extern Boolean MakePresenceBox( 
                        WmScreenData *pSD) ;
extern void ShowPresenceBox( 
                        ClientData *pClient,
                        Context wsContext) ;
extern void HidePresenceBox( 
                        WmScreenData *pSD,
			Boolean  userDismissed) ;
extern MenuItem * GetPresenceBoxMenuItems( 
                        WmScreenData *pSD) ;
extern void UpdatePresenceWorkspaces( WmScreenData *pSD );

/********    End Public Function Declarations    ********/
#endif /* WSM */
