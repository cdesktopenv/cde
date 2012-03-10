/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmFeedback.h /main/4 1995/11/01 11:37:25 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern void ConfirmAction (WmScreenData *pSD, int nbr);
extern void HideFeedbackWindow (WmScreenData *pSD);
extern void InitCursorInfo (void);
extern void PaintFeedbackWindow (WmScreenData *pSD);
extern void ShowFeedbackWindow (WmScreenData *pSD, int x, int y, 
				unsigned int width, unsigned int height, 
				unsigned long style);
extern void ShowWaitState (Boolean flag);
extern void UpdateFeedbackInfo (WmScreenData *pSD, int x, int y, 
				unsigned int width, unsigned int height);
extern void UpdateFeedbackText (WmScreenData *pSD, int x, int y, 
				unsigned int width, unsigned int height);

