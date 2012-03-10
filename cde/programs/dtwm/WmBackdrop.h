/* $XConsortium: WmBackdrop.h /main/4 1995/11/01 11:31:43 rswiston $ */
#ifdef WSM
/* 
 * (c) Copyright 1987, 1988, 1989, 1990, 1992 HEWLETT-PACKARD COMPANY 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

/********    Public Function Declarations    ********/

extern void ChangeBackdrop( 
                        WmWorkspaceData *pWS) ;
extern void ProcessBackdropResources( 
                        WmWorkspaceData *pWS,
                        unsigned long callFlags) ;
extern String FullBitmapFilePath( 
                        String pch) ;
extern void SetNewBackdrop( 
                        WmWorkspaceData *pWS,
                        Pixmap pixmap,
                        String bitmapFile) ;
extern Boolean IsBackdropWindow( 
                        WmScreenData *pSD,
                        Window win) ;

/********    End Public Function Declarations    ********/

#endif /* WSM */
/****************************   eof    ***************************/
