/* $TOG: WmIPC.h /main/5 1999/09/20 15:17:55 mgreess $ */
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

extern void dtInitialize( 
                        char *program_name,
                        XtAppContext appContext) ;
extern void dtInitializeMessaging( Widget ) ;
extern void dtCloseIPC( void ) ;
extern void dtReadyNotification( void ) ;
extern void WmStopWaiting( void ) ;
extern void dtSendWorkspaceModifyNotification(
	WmScreenData 		*pSD, 
	Atom 			aWs, 
	int 			iType);

/********    End Public Function Declarations    ********/

/****************************   eof    ***************************/
