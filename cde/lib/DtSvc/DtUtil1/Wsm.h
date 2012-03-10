/* $XConsortium: Wsm.h /main/5 1996/05/20 16:08:50 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _Dt_Wsm_h
#define _Dt_Wsm_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

#define DtWSM_REASON_ADD	0
#define DtWSM_REASON_DELETE	1
#define DtWSM_REASON_BACKDROP	2
#define DtWSM_REASON_TITLE	3
#define DtWSM_REASON_CURRENT	4


/*
 * Types
 */

/* Workspace property information */

typedef struct _DtWsmWorkspaceInfo {
	Atom		workspace;
	unsigned long	bg;
	unsigned long	fg;
	Atom		backdropName;
	int		colorSetId;
	char		*pchTitle;
	Window		*backdropWindows;
	int		numBackdropWindows;
} DtWsmWorkspaceInfo;


/* Workspace modified callback reasons */

typedef int DtWsmWsReason;

/* Workspace callback context (opaque) */

typedef struct _DtWsmCBContext * DtWsmCBContext;

/* Workspace callback prototype */

typedef void (*DtWsmWsChangeProc)(
		Widget		widget,
		Atom 		aWs,
		XtPointer	client_data);

/* Workspace modified callback prototype */

typedef void (*DtWsmWsModifiedProc)(
		Widget		widget,
		Atom 		aWs,
		DtWsmWsReason	reason,
		XtPointer	client_data);


/*
 * Functions
 */

extern void DtWsmAddWorkspaceFunctions(
		Display		*display,
		 Window		client);

extern void DtWsmRemoveWorkspaceFunctions(
		Display		*display,
		Window		client);

extern int DtWsmGetWorkspaceInfo(
		Display		*display,
		Window		root,
		Atom		aWS,
		DtWsmWorkspaceInfo **ppWsInfo);

extern void DtWsmFreeWorkspaceInfo(
		DtWsmWorkspaceInfo *pWsInfo);

extern int DtWsmGetWorkspaceList(
		Display		*display,
		Window		root,
		Atom		**ppWorkspaceList,
		int		*pNumWorkspaces);

extern int DtWsmGetCurrentWorkspace(
		Display		*display,
		Window		root,
		Atom		*paWorkspace);

extern DtWsmCBContext DtWsmAddCurrentWorkspaceCallback(
		Widget		widget,
		DtWsmWsChangeProc ws_change,
		XtPointer	client_data);

extern void DtWsmRemoveWorkspaceCallback(
		DtWsmCBContext	pCbCtx);

extern int DtWsmSetCurrentWorkspace(
		Widget		widget,
		Atom		aWs);

extern int DtWsmGetWorkspacesOccupied(
		Display		*display,
		Window		window,
		Atom		**ppaWs,
		unsigned long	*pNumWs);

extern void DtWsmSetWorkspacesOccupied(
		Display		*display,
		Window		window,
		Atom		*pWsHints,
		unsigned long	numHints);

extern void DtWsmOccupyAllWorkspaces(
		Display		*display,
		Window		window);

extern DtWsmCBContext DtWsmAddWorkspaceModifiedCallback(
		Widget		widget,
		DtWsmWsModifiedProc ws_modified,
		XtPointer	client_data);

extern Window DtWsmGetCurrentBackdropWindow(
		Display		*display,
		Window		root);

extern int DtWsmSetWorkspaceTitle(
		Widget		widget,
		Atom		aWs,
		char            *title);

extern int DtWsmChangeBackdrop(
		Display		*display,
		Window		root,
                char            *path,
                Pixmap          pixmap);

extern int DtWsmAddWorkspace(
		Widget		widget,
		char 		*title);

extern int DtWsmDeleteWorkspace(
		Widget		widget,
		Atom		aWs);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_Wsm_h */
