/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmCEvent.h /main/4 1995/11/01 11:34:01 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern void CheckButtonPressBuiltin (XButtonEvent *buttonEvent, 
				     Context context, Context subContext, 
				     int partContext, ClientData *pCD);
extern void CheckButtonReleaseBuiltin (XButtonEvent *buttonEvent, 
				       Context context, Context subContext, 
				       ClientData *pCD);
extern Window GetParentWindow (Window window);
extern WmScreenData *GetScreenForWindow (Window win);
extern Boolean HandleCButtonPress (ClientData *pCD, XButtonEvent *buttonEvent);
extern void HandleCButtonRelease (ClientData *pCD, XButtonEvent *buttonEvent);
extern void HandleCColormapNotify (ClientData *pCD, 
				   XColormapEvent *colorEvent);
extern void HandleCConfigureRequest (ClientData *pCD, 
				     XConfigureRequestEvent *configureRequest);
extern void HandleCEnterNotify (ClientData *pCD, 
				XEnterWindowEvent *enterEvent);
extern void HandleCLeaveNotify (ClientData *pCD, 
				XLeaveWindowEvent *leaveEvent);
extern Boolean HandleCFocusIn (ClientData *pCD, 
			       XFocusChangeEvent *focusChangeEvent);
extern Boolean HandleCFocusOut (ClientData *pCD, 
				XFocusChangeEvent *focusChangeEvent);
extern Boolean HandleCKeyPress (ClientData *pCD, XKeyEvent *keyEvent);
extern void HandleClientMessage (ClientData *pCD, 
				 XClientMessageEvent *clientEvent);
extern void HandleCMotionNotify (ClientData *pCD, XMotionEvent *motionEvent);
extern void HandleCPropertyNotify (ClientData *pCD, 
				   XPropertyEvent *propertyEvent);
#ifndef NO_SHAPE
extern void HandleCShapeNotify (ClientData *pCD,  XShapeEvent *shapeEvent);
#endif /* NO_SHAPE */
extern Boolean HandleEventsOnClientWindow (ClientData *pCD, XEvent *pEvent);
extern Boolean HandleEventsOnSpecialWindows (XEvent *pEvent);


extern void SetupCButtonBindings (Window window, ButtonSpec *buttonSpecs);
extern Boolean WmDispatchClientEvent (XEvent *event);
extern void HandleIconBoxButtonPress (ClientData *pCD, 
				      XButtonEvent *buttonEvent, 
				      Context subContext);
extern void HandleIconButtonPress (ClientData *pCD, XButtonEvent *buttonEvent);


extern void ProcessButtonGrabOnClient (ClientData *pCD, 
				       XButtonEvent *buttonEvent, 
				       Boolean replayEvent);
extern void DetermineActiveScreen (XEvent *pEvent);
extern Boolean WmDispatchClientEvent (XEvent *event);

#define SetActiveScreen(psd) (ACTIVE_PSD=(psd), wmGD.queryScreen=False)
