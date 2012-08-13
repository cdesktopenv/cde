/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: WmWrkspace.h /main/4 1995/11/01 11:56:28 rswiston $ */
#ifdef WSM
/* 
 * (c) Copyright 1987,1988,1989,1990,1992,1993,1994 HEWLETT-PACKARD COMPANY 
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 * ALL RIGHTS RESERVED 
 */ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

/********    Public Function Declarations    ********/

extern void ChangeToWorkspace( 
                        WmWorkspaceData *pNewWS) ;
extern void ChangeWorkspaceTitle( WmWorkspaceData *pWS, char * pchTitle);
extern Boolean DuplicateWorkspaceName (
			WmScreenData *pSD, 
			unsigned char *name, 
			int num);
extern void UpdateWorkspacePresenceProperty( 
                        ClientData *pCD) ;
#ifdef HP_VUE
extern void UpdateWorkspaceInfoProperty( 
                        WmScreenData *pSD) ;
#endif /* HP_VUE */
extern WmWorkspaceData * CreateWorkspace( 
                        WmScreenData *pSD,
                        unsigned char *name) ;
extern void DeleteWorkspace( 
                        WmWorkspaceData *pWS) ;
extern Boolean GetClientWorkspaceInfo( 
                        ClientData *pCD,
                        long manageFlags) ;
extern Boolean WorkspaceIsInCommand( 
                        Display *dpy,
                        ClientData *pCD,
                        WorkspaceID **ppIDs,
                        unsigned int *pNumIDs) ;
extern Boolean ConvertNamesToIDs( 
                        WmScreenData *pSD,
                        unsigned char *pchIn,
                        WorkspaceID **ppAtoms,
                        unsigned int *pNumAtoms) ;
extern void CheckForPutInAllRequest( 
                        ClientData *pCD,
                        Atom *pIDs,
                        unsigned int numIDs) ;
extern Boolean FindWsNameInCommand( 
                        int argc,
                        char *argv[],
                        unsigned char **ppch) ;
extern void PutClientIntoWorkspace( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern void TakeClientOutOfWorkspace( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern WmWorkspaceData * GetWorkspaceData( 
                        WmScreenData *pSD,
                        WorkspaceID wsID) ;
extern unsigned char * GenerateWorkspaceName( 
                        WmScreenData *pSD,
                        int wsnum) ;
extern Boolean InWindowList( 
                        Window w,
                        Window wl[],
                        int num) ;
extern Boolean ClientInWorkspace( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern WsClientData * GetWsClientData( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern void SetClientWsIndex( 
                        ClientData *pCD) ;
extern Boolean ProcessWorkspaceHints( 
                        ClientData *pCD) ;
extern void ProcessWorkspaceHintList( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
extern void RemoveSingleClientFromWorkspaces( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
extern void RemoveSubtreeFromWorkspaces( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
#ifdef PANELIST
extern WorkspaceID * GetListOfOccupiedWorkspaces( 
			ClientData *pCD,
                        int *numIDs) ;
#endif /* PANELIST */
extern void HonorAbsentMapBehavior(
			ClientData *pCD) ;
extern void RemoveClientFromWorkspaces( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
extern void AddSingleClientToWorkspaces( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
extern void AddSubtreeToWorkspaces( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
extern void AddClientToWorkspaces( 
                        ClientData *pCD,
                        WorkspaceID *pIDs,
                        unsigned int numIDs) ;
extern void AddClientToWsList( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern void RemoveClientFromWsList( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern Boolean F_CreateWorkspace( 
                        String args,
                        ClientData *pCD,
                        XEvent *event) ;
extern Boolean F_DeleteWorkspace( 
                        String args,
                        ClientData *pCD,
                        XEvent *event) ;
extern Boolean F_GotoWorkspace( 
                        String args,
                        ClientData *pCD,
                        XEvent *event) ;
extern Boolean F_AddToAllWorkspaces( 
                        String args,
                        ClientData *pCD,
                        XEvent *event) ;
extern Boolean F_Remove( 
                        String args,
                        ClientData *pCD,
                        XEvent *event) ;
extern int GetCurrentWorkspaceIndex( 
                        WmScreenData *pSD) ;
extern void InsureIconForWorkspace( 
                        WmWorkspaceData *pWS,
                        ClientData *pCD) ;
extern Boolean GetLeaderPresence( 
                        ClientData *pCD,
                        WorkspaceID **ppIDs,
                        unsigned int *pnumIDs) ;
extern Boolean GetMyOwnPresence( 
                        ClientData *pCD,
                        WorkspaceID **ppIDs,
                        unsigned int *pnumIDs) ;
extern void ReserveIdListSpace( 
                        int numIDs) ;
extern void SaveWorkspaceResources( 
                        WmWorkspaceData *pWS,
                        unsigned long flags) ;
extern void SaveResources( 
                        WmScreenData *pSD) ;
extern void AddStringToResourceData( 
                        char *string,
                        char **pdata,
                        int *plen) ;
#ifdef PANELIST
extern void SetCurrentWorkspaceButton(
			WmScreenData *pSD);
extern void SetFrontPanelTitle( 
			WmScreenData *pSD);
#endif /* PANELIST */

/********    End Public Function Declarations    ********/
#endif /* WSM */


