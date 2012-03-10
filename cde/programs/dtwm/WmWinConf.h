/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmWinConf.h /main/5 1996/06/11 16:01:31 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern void AdjustPos (int *pX, int *pY, unsigned int oWidth, 
		       unsigned int oHeight, unsigned int nWidth, 
		       unsigned int nHeight);
extern unsigned int ButtonStateBit (unsigned int button);
extern void CancelFrameConfig (ClientData *pcd);
extern void	CheckEatButtonRelease (ClientData *pcd, XEvent *pev);
extern Boolean CheckVisualPlace (ClientData *pCD, int tmpX, int tmpY);
extern void CompleteFrameConfig (ClientData *pcd, XEvent *pev);
extern Cursor ConfigCursor (int frame_part);
extern void DoFeedback (ClientData *pcd, int x, int y, unsigned int width, 
			unsigned int height, unsigned long newStyle, 
			Boolean resizing);
extern Boolean DoGrabs (Window grab_win, Cursor cursor, unsigned int pmask, 
			Time grabTime, ClientData *pCD, Boolean alwaysGrab);
extern void DrawOutline (int x, int y, unsigned int width, 
			 unsigned int height);
extern void	EatButtonRelease (unsigned int releaseButtons);
extern void FixFrameValues (ClientData *pcd, int *pfX, int *pfY, 
			    unsigned int *pfWidth, unsigned int *pfHeight, 
			    Boolean resizing);
extern void FlashOutline (int x, int y, unsigned int width, 
			  unsigned int height);
extern void ForceOnScreen (int screen, int *pX, int *pY);
extern void GetClipDimensions (ClientData *pCD, Boolean fromRoot);
extern void GetConfigEvent (Display *display, Window window, 
			    unsigned long mask, int curX, int curY, 
			    int oX, int oY, unsigned oWidth, 
			    unsigned oHeight, XEvent *pev);
extern Window GrabWin (ClientData *pcd, XEvent *pev);
extern void HandleClientFrameMove (ClientData *pcd, XEvent *pev);
extern void HandleClientFrameResize (ClientData *pcd, XEvent *pev);
extern Boolean HandleResizeKeyPress (ClientData *pcd, XEvent *pev);
extern void    MoveOpaque (ClientData *pcd, int x, int y,
		 unsigned int width, unsigned int height);
extern void MoveOutline (int x, int y, unsigned int width, 
			 unsigned int height);
extern void ProcessNewConfiguration (ClientData *pCD, int x, int y, 
				     unsigned int width, unsigned int height, 
				     Boolean clientRequest);
extern void ReGrabPointer (Window grab_win, Time grabTime);
#if ((!defined(WSM)) || defined(MWM_QATS_PROTOCOL))
extern void SetGrabServer (void);
#endif /* !defined(WSM) || defined(MWM_QATS_PROTOCOL) */
extern void SetOutline (XSegment *pOutline, int x, int y, unsigned int width, 
			unsigned int height, int fatness);
extern void SetPointerPosition (int newX, int newY, int *actualX, 
				int *actualY);
extern Boolean SetPointerResizePart (ClientData *pcd, XEvent *pev);
extern Boolean StartClientMove (ClientData *pcd, XEvent *pev);
extern void StartClientResize (ClientData *pcd, XEvent *pev);
extern Boolean StartResizeConfig (ClientData *pcd, XEvent *pev);
extern int ResizeType (ClientData *pcd, XEvent *pev);
extern void UndoGrabs (void);
#ifdef WSM
extern void HandleMarqueeSelect (WmScreenData *pSD, XEvent *pev);
extern void StartMarqueeSelect(WmScreenData *pSD, XEvent *pev);
extern void UpdateMarqueeSelectData (WmScreenData *pSD);
extern Boolean HandleMarqueeKeyPress (WmScreenData *pSD, XEvent *pev);
extern void WindowOutline (int x, int y, unsigned int width, 
			 unsigned int height);
#endif /* WSM */

