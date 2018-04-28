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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: dtkcmds.h /main/6 1996/04/01 17:54:01 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        dtkcmds.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for dtkcmds.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_dtkcmds_h
#define _Dtksh_dtkcmds_h

#include <Dt/Wsm.h>
#include <Dt/Print.h>
#include "name.h"

#define CONVERT_SUCCEEDED 1
#define CONVERT_POSTPONED 0
#define CONVERT_FAILED   -1

extern Widget Toplevel;
extern char str_nill[];
extern wtab_t * DTKSHConversionWidget;
extern classtab_t * DTKSHConversionClass;
extern char * DTKSHConversionResource;



extern wtab_t * set_up_w( 
                        Widget wid,
                        wtab_t *parent,
                        char *var,
                        char *name,
                        classtab_t *class) ;
extern void parse_args( 
                        char *arg0,
                        int argc,
                        char **argv,
                        wtab_t *w,
                        wtab_t *parent,
                        classtab_t *class,
                        int *n,
                        Arg *args,
	                int * pargc,
	                char ** pargv,
	                Boolean postponePixmaps ) ;
extern void free_args( 
                        int n,
                        Arg *args) ;
extern int do_XtInitialize( 
                        int argc,
                        char *argv[]) ;
extern int do_XtCreateApplicationShell( 
                        int argc,
                        char *argv[]) ;
extern int do_XtCreatePopupShell( 
                        int argc,
                        char *argv[]) ;
extern int do_XtCreateManagedWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XtCreateWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XtPopup( 
                        int argc,
                        char *argv[]) ;
extern int do_XtDestroyWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_single_widget_test_func( 
                        int (*func)(),
                        int argc,
                        char **argv) ;
extern int do_XtIsSensitive( 
                        int argc,
                        char *argv[]) ;
extern int do_XtIsShell( 
                        int argc,
                        char *argv[]) ;
extern int do_XtIsManaged( 
                        int argc,
                        char *argv[]) ;
extern int do_XtIsRealized( 
                        int argc,
                        char *argv[]) ;
extern int do_XtRealizeWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XtUnrealizeWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XtMapWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XtUnmapWidget( 
                        int argc,
                        char **argv) ;
extern int do_XtPopdown( 
                        int argc,
                        char **argv) ;
extern int do_XtMainLoop( 
                        int argc,
                        char **argv) ;
extern int do_XtDisplay( 
                        int argc,
                        char **argv) ;
extern int do_XtDisplayOfObject( 
                        int argc,
                        char **argv) ;
extern int do_XtNameToWidget( 
                        int argc,
                        char **argv) ;
extern int do_XtScreen( 
                        int argc,
                        char **argv) ;
extern int do_XtWindow( 
                        int argc,
                        char **argv) ;
extern int do_XtCallCallbacks( 
                        int argc,
                        char **argv) ;
extern int do_XtHasCallbacks( 
                        int argc,
                        char **argv) ;
extern int do_XtAddCallback( 
                        int argc,
                        char **argv) ;
extern int AddOneCallback( 
                        char *cmd,
                        char *widget,
                        char *cbName,
                        char *kshcmd,
                        char *propAtomStr) ;
extern int do_XtRemoveCallback( 
                        int argc,
                        char **argv) ;
extern int RemoveOneCallback( 
                        char *cmd,
                        char *widget,
                        char *cbName,
                        char *kshcmd,
                        char *propAtomStr,
			char *handleStr) ;
extern int do_XtAddEventHandler( 
                        int argc,
                        char **argv) ;
extern int do_XtRemoveEventHandler( 
                        int argc,
                        char **argv) ;
extern int do_XtGetValues( 
                        int argc,
                        char **argv) ;
extern int do_XtSetValues( 
                        int argc,
                        char **argv) ;
extern int do_XtAddWorkProc( 
                        int argc,
                        char *argv[]) ;
extern int do_XtRemoveWorkProc( 
                        int argc,
                        char *argv[]) ;
extern int do_XtAddTimeOut( 
                        int argc,
                        char *argv[]) ;
extern int do_XtRemoveTimeOut( 
                        int argc,
                        char *argv[]) ;
extern int do_XtUnmanageChildren( 
                        int argc,
                        char *argv[]) ;
extern int do_XtManageChildren( 
                        int argc,
                        char *argv[]) ;
extern int do_XtIsSubclass( 
                        int argc,
                        char *argv[]) ;
extern int do_XtClass( 
                        int argc,
                        char *argv[]) ;
extern int do_managelist_func( 
                        int argc,
                        char *argv[],
                        int (*func)()) ;
extern int create_standard_gc( 
                        Display *display,
                        Window drawable) ;
extern int do_XBell( 
                        int argc,
                        char *argv[]) ;
extern int do_XRootWindowOfScreen( 
                        int argc,
                        char *argv[]) ;
extern int do_XWidthOfScreen( 
                        int argc,
                        char *argv[]) ;
extern int do_XHeightOfScreen( 
                        int argc,
                        char *argv[]) ;
extern int do_XDefineCursor( 
                        int argc,
                        char *argv[]) ;
extern int do_XUndefineCursor( 
                        int argc,
                        char *argv[]) ;
extern int do_XtRemoveAllCallbacks( 
                        int argc,
                        char *argv[]) ;
extern int do_XTextWidth( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawArc( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawImageString( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawLine( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawLines( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawPoint( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawPoints( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawRectangle( 
                        int argc,
                        char *argv[]) ;
extern int do_XCopyArea( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawSegments( 
                        int argc,
                        char *argv[]) ;
extern int do_XDrawString( 
                        int argc,
                        char *argv[]) ;
extern int do_XFillArc( 
                        int argc,
                        char *argv[]) ;
extern int do_XFillPolygon( 
                        int argc,
                        char *argv[]) ;
extern int do_XFillRectangle( 
                        int argc,
                        char *argv[]) ;
extern int do_XClearArea( 
                        int argc,
                        char *argv[]) ;
extern int do_XClearWindow( 
                        int argc,
                        char *argv[]) ;
extern int ConvertTypeToString( 
                        char *arg0,
                        classtab_t *class,
                        wtab_t *w,
                        wtab_t *parent,
                        char *resource,
                        XtArgVal val,
                        char **ret) ;
extern int ConvertStringToType( 
                        char *arg0,
                        wtab_t *w,
                        wtab_t *parent,
                        classtab_t *class,
                        char *resource,
                        char *val,
                        XtArgVal *ret,
                        int *freeit,
                        Boolean postponePixmaps) ;
extern int do_XtAddInput( 
                        int argc,
                        char *argv[]) ;
extern int do_XtRemoveInput( 
                        int argc,
                        char *argv[]) ;
extern void Translation_ksh_eval( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void RestorePriorEnvVarValues( 
                        Namval_t *np1,
                        char *value1,
                        Namval_t *np2,
                        char *value2) ;
extern void stdCB( 
                        void *widget,
                        caddr_t clientData,
                        caddr_t callData) ;
extern void stdWSCB( 
                        void *widget,
                        Atom atom,
                        caddr_t clientData) ;
extern void stdInputCB( 
                        inputrec_t *inp,
                        int *source,
                        XtInputId *id) ;
extern int stdWorkProcCB( 
                        char *clientData) ;
extern void stdTimerCB( 
                        char *clientData,
                        long *id) ;
extern void stdPrintSetupProc(
                        int proctype,
                        void  *widget,
                        DtPrintSetupData *callData);
extern void stdPrinterInfoProc(
                        void  *widget,
                        DtPrintSetupData *callData);
extern void stdSelectFileProc(
                        void  *widget,
                        DtPrintSetupData *callData);
extern void stdSelectPrinterProc(
                        void  *widget,
                        DtPrintSetupData *callData);
extern void stdSetupProc(
                        void  *widget,
                        DtPrintSetupData *callData);
extern void stdVerifyPrinterProc(
                        void  *widget,
                        DtPrintSetupData *callData);
extern int do_VerifyString( 
                        int argc,
                        char *argv[]) ;
extern int do_XFlush( 
                        int argc,
                        char *argv[]) ;
extern int do_XSync( 
                        int argc,
                        char *argv[]) ;
extern int do_XRaiseWindow( 
                        int argc,
                        char *argv[]) ;
extern int do_XtSetSensitive( 
                        int argc,
                        char *argv[]) ;
extern int do_XtOverrideTranslations( 
                        int argc,
                        char **argv) ;
extern int do_XtAugmentTranslations( 
                        int argc,
                        char **argv) ;
extern int do_XtUninstallTranslations( 
                        int argc,
                        char *argv[]) ;
extern int do_XtParent( 
                        int argc,
                        char **argv) ;
extern int do_XtLastTimestampProcessed( 
                        int argc,
                        char **argv) ;
extern dtksh_client_data_t * GetNewCBData( 
                        char *ksh_cmd,
                        wtab_t *w,
                        char *cbname,
                        Atom propAtom) ;
extern int LocateCBRecord( 
                        wtab_t *w,
                        char *cbname,
                        char *ksh_cmd,
                        Atom propAtom,
                        DtWsmCBContext handle) ;
extern void stdEH( 
                        void *widget,
                        caddr_t clientData,
                        XEvent *event,
                        Boolean *continueToDispatch) ;
extern dtksh_event_handler_data_t * GetNewEHData( 
                        char *ksh_cmd,
                        wtab_t *w,
                        EventMask eventMask,
                        Boolean nonMaskable) ;
extern int do_DtSessionRestorePath( 
                        int argc,
                        char *argv[]) ;
extern int do_DtSessionSavePath( 
                        int argc,
                        char *argv[]) ;
extern int do_DtShellIsIconified( 
                        int argc,
                        char *argv[]) ;
extern int do_DtSetStartupCommand( 
                        int argc,
                        char *argv[]) ;
extern int do_DtSetIconifyHint( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmAddWorkspaceFunctions( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmRemoveWorkspaceFunctions( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmGetCurrentWorkspace( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmSetCurrentWorkspace( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmGetWorkspaceList( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmGetWorkspacesOccupied( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmSetWorkspacesOccupied( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmGetCurrentBackdropWindow( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmOccupyAllWorkspaces( 
                        int argc,
                        char *argv[]) ;
extern int do__DtGetHourGlassCursor( 
                        int argc,
                        char *argv[]) ;
extern int do__DtTurnOnHourGlass( 
                        int argc,
                        char *argv[]) ;
extern int do__DtTurnOffHourGlass( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWsmAddCurrentWorkspaceCallback( 
                        int argc,
                        char **argv) ;
extern int do_DtWsmRemoveWorkspaceCallback( 
                        int argc,
                        char **argv) ;
extern int do_DtDbLoad(
                        int argc,
                        char *argv[]) ;
extern int do_DtDbReloadNotify(
                        int argc,
                        char *argv[]) ;
extern int do_DtActionExists(
                        int argc,
                        char *argv[]) ;
extern int do_DtActionLabel(
                        int argc,
                        char *argv[]) ;
extern int do_DtActionDescription(
                        int argc,
                        char *argv[]) ;
extern int do_DtActionInvoke(
                        int argc,
                        char *argv[]) ;
extern int do_DtDtsLoadDataTypes(
                        int argc,
                        char *argv[] );
extern int do_DtDtsFileToDataType(
                        int argc,
                        char *argv[] );
extern int do_DtDtsFileToAttributeValue(
                        int argc,
                        char *argv[] );
extern int do_DtDtsFileToAttributeList(
                        int argc,
                        char *argv[] );
extern int do_DtDtsDataTypeToAttributeValue(
                        int argc,
                        char *argv[] );
extern int do_DtDtsDataTypeToAttributeList(
                        int argc,
                        char *argv[] );
extern int do_DtDtsFindAttribute(
                        int argc,
                        char *argv[] );
extern int do_DtDtsDataTypeNames(
                        int argc,
                        char *argv[] );
extern int do_DtDtsSetDataType(
                        int argc,
                        char *argv[] );
extern int do_DtDtsDataTypeIsAction(
                        int argc,
                        char *argv[] );
extern int do_ttdt_open(
                        int argc,
                        char *argv[] );
extern int do_ttdt_close(
                        int argc,
                        char *argv[] );
extern int do_tttk_Xt_input_handler(
                        int argc,
                        char *argv[] );
extern int do_ttdt_session_join(
                        int argc,
                        char *argv[] );
extern int do_ttdt_session_quit(
                        int argc,
                        char *argv[] );
extern int do_ttdt_file_event(
                        int argc,
                        char *argv[] );
extern int do_ttdt_file_join(
                        int argc,
                        char *argv[] );
extern int do_ttdt_file_quit(
                        int argc,
                        char *argv[] );
extern int do_ttdt_Get_Modified(
                        int argc,
                        char *argv[] );
extern int do_ttdt_Save(
                        int argc,
                        char *argv[] );
extern int do_ttdt_Revert(
                        int argc,
                        char *argv[] );
extern int do_tt_error_pointer(
                        int argc,
                        char *argv[] );
extern int do_tttk_message_destroy(
                        int argc,
                        char *argv[] );
extern int do_tt_message_reply(
                        int argc,
                        char *argv[] );
extern int do_tttk_message_reject(
                        int argc,
                        char *argv[] );
extern int do_tttk_message_fail(
                        int argc,
                        char *argv[] );
extern int do_tt_file_netfile(
                        int argc,
                        char *argv[] );
extern int do_tt_netfile_file(
                        int argc,
                        char *argv[] );
extern int do_tt_host_file_netfile(
                        int argc,
                        char *argv[] );
extern int do_tt_host_netfile_file(
                        int argc,
                        char *argv[] );
extern Namval_t * GetNameValuePair( 
                        char *name) ;
extern void FreeNestedVariables( void ) ;
extern Namval_t * nopCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * dftCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * ehCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * transCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * scaleCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * arrowCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * comboCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * cmdCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * dAreaCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * dbtnCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * dtPrintSetupProcDisc(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
extern Namval_t * fselCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * listCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * pbtnCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * rcCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * sbarCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * swinCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * sboxCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * tbtnCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * textCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * textCreateDisc2( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern Namval_t * helpCreateDisc( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
extern int * LockKshFileDescriptors( void ) ;
extern void UnlockKshFileDescriptors( 
                        int * fdList) ;



#endif /* _Dtksh_dtkcmds_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
