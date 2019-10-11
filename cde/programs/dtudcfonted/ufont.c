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
/* ufont.c 1.45 - Fujitsu source for CDEnext    96/12/03 18:34:11      */
/* $XConsortium: ufont.c /main/14 1996/12/17 19:30:16 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <X11/Intrinsic.h>

#include "xoakufont.h"
#include "util.h"

#define _CLIENT_CAT_NAME "dtudcfonted"
extern char *_DtGetMessage(char *filename, int set, int n, char *s);
#define GETMESSAGE(set, number, string) GetMessage(set, number, string)
static char *
GetMessage(int set, int number, char *string)
{
    char *tmp, *ret;
    tmp = _DtGetMessage(_CLIENT_CAT_NAME, set, number, string);
    ret = malloc(strlen(tmp) + 1);
    strcpy(ret, tmp);
    return (ret);
}

/****************************************************************
 *    Widgets							*
 ***************************************************************/
Widget	toplevel;
static Widget	dnPopW;
Widget	wgeScro, editPopW;

static int	select_x, select_y, select_w, select_h;
static int	r1_x, r1_y, r2_x, r2_y, cut_w, cut_h;

extern	Widget		xlfdDialog, cpyDialog;

extern FalFontID font_id;

Pixmap          arrow_pix=0;


static XtAppContext app; /* application context */
static int edpane_size=0;

static void OtherFontSelect(void);
void drawDelCode(int i);
void drawDelPtn(int i);
static void xeg_init(void);


static void dstrypaneEditPtn(void);
void chgEdCode(int code, char mode);
void chgEdList(int statloc, int slctloc, char mode);
static void chgEdPtn(int code);
static void DrawRectEdPn(int x1, int y1, int x2, int y2);
static void DrawBorderEdPn(int x1, int y1, int x2, int y2);
static void DrawPointEdPn(int x, int y, int mode);
static void DrawDpPn(void);
static void DrawPointDpPn(int x, int y, int mode);
static void musPoint(int evtype, int px, int py);
static void musLine(int evtype, int px, int py);
static void musCircle(int evtype, int px, int py);
static void musRect(int proc, int evtype, int px, int py);
static void musRegionProc(int proc, int evtype, int px, int py);
#ifdef __linux__
static void musPasteProc(Widget w, XtPointer client_data, XEvent *event, Boolean*);
#else
static void musPasteProc(Widget w, XtPointer client_data, XEvent *event);
#endif
static void rubLine(int x1, int y1, int x2, int y2);
static void rubBand(int x1, int y1, int x2, int y2);
static void rubCircle(int ox, int oy, int rx, int ry);
static void resetEditMode(unsigned int flag);
static void copyPatterns(FalFontData *fdata,
			 int s1_code,
			 int s2_code,
			 int d1_code,
			 int proc);
extern String MngCodeTfValue(void);
extern String CpySrcCodeTfValue(void);
extern String CpyDestCodeTfValue(void);
char	*get_cmd_path(char *path, char *cmd);
extern  FalFontData     fullFontData;
extern  FalFontData     copyFontData;

extern void PopupSelectXLFD(Widget top);
extern void UpdateMessage(String str);
extern void DispMngErrorMessage(String msg);
extern void DispCpyErrorMessage(String msg);
static int setRealDelArea(int *s_ncode,
			  int *e_ncode,
			  int *sq_start,
			  int *sq_end);

/****************************************************************
 * parameters   						*
 ***************************************************************/
static Arg	arg[30];
static int	n;

Resource resource;

/****************************************************************
 * callback routines 						*
 ***************************************************************/

static void CancelCB(void);

static void
ExitCB(void)
{
    exit(0);
}

int
efctPtnNum(void)
{
    int no;
    int sq;
    int cnt;

    for ( cnt = 0, sq = edlist.sqstart;
	     sq < (edlist.sqstart + edlist.nptn); sq++) {
	no = ptnSqToNo(sq);
	if (( no >= 0) && (codeCheck( noToCode( no)) == 0))
	    cnt++;
    }
    return( cnt);
}

void
Error_message(Widget widget, char *message)
{
    static NoticeButton is_lock[] = {
    NBTNARGS( ExitCB, NULL, 'E', True, False ),
    NBTNARGS( CancelCB, NULL, 'C', True, True )
    };
    static NButton LockBTN = NBUTTONS( is_lock );

    SetItemLabel(&LockBTN, 0, resource.l_exit);
    SetItemLabel(&LockBTN, 1, resource.l_cancel);
    PopupNotice( (widget), message,
		D_ERROR,
		&LockBTN,
		True,
		resource.l_question_title);
}

void
Error_message2(Widget widget, char *message)
{
    static NoticeButton is_lock[] = {
    NBTNARGS( CancelCB, NULL, 'C', True, True )
    };
    static NButton LockBTN = NBUTTONS( is_lock );

    if (widget == NULL)
	widget = toplevel;

    SetItemLabel(&LockBTN, 0, resource.l_ok);
    PopupNotice( (widget), message,
		D_ERROR,
		&LockBTN,
		True,
		resource.l_question_title);
}

/*
 * contents : read a character pattern from SNF file
 */
/*ARGSUSED*/
static void
CBmOblB_edit( Widget widget, caddr_t clientData, caddr_t callData )
{
    int		ptn_n;
    int		ptn_w;
    int		ptn_h;
    int		code;
    int		i;
    int		ret;
    char	err[128];
    extern int begin_code;

    extern int ptnGetInfo();
    extern void PopupEditPtn();

    /* open font file and get informations of character to be edited  */

    ret = readSNF( &(edg.fdata), &(edg.width), &(edg.height), err);

    if( ret == -1 ) {
	Error_message(widget, err);
	return;
    }
    dn.ptn_w = (Dimension) edg.width;
    dn.ptn_h = (Dimension) edg.height;


    if( xlfdDialog != NULL )
	PopdownDialog(xlfdDialog);

    ptnGetInfo( &ptn_n, &ptn_w, &ptn_h );
    for( i=0 ; i<ptn_n ; i++ ) {
	code = noToCode( ptnSqToNo(i) );
	if( begin_code > code){
	    edlist.sqstart = i+1;
	} else {
	    edlist.nptn++;
	}
    }
    if( efctPtnNum() > 0) {
	for ( i=edlist.sqstart; i<(edlist.sqstart+edlist.nptn); i++){
	    if ( codeCheck( noToCode( ptnSqToNo(i))) == 0)
		break;
	}
	edg.ncode = ptnSqToNo( i );
	edg.code = noToCode( edg.ncode );
	ptnGet( edg.code, edg.ptn );
    }
    PopupEditPtn(toplevel);
}



void CBeOblB_aAdd(void);

static Boolean do_read = False;
static Boolean do_end = False;

static void CancelCB(void) { }

static void ContReadCB(Widget w)
{
    FalCloseFont(font_id);
    editPtnW = NULL;
    OtherFontSelect();
    PopupSelectXLFD(toplevel);
}

static void SaveReadCB(void)
{
    CBeOblB_aAdd();
    do_read = True;
    PopupSelectXLFD(toplevel);
}


static void ContEndCB(void)
{
    FalCloseFont(font_id);
    exit(0);
}

static void SaveEndCB(void)
{
    CBeOblB_aAdd();
    do_end = True;
}


/*
 * contents : quit editting
 */


/*ARGSUSED*/
static int
QuitEditPtn( Widget widget, caddr_t clientData, caddr_t callData )
{
    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );

    if( edg.code != 0 ){
	ptnAdd( edg.code, edg.ptn );
    }
    if( edg.flag == ON ){
	return(0);
    }
    return(1);
}



void
OpenCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    static NoticeButton is_save_read_btn1[] = {
    NBTNARGS( SaveReadCB, NULL, 'S', True, False ),
    NBTNARGS( ContReadCB, NULL, 'O', True, False ),
    NBTNARGS( CancelCB, NULL, 'C', True, True ),
    };
    static NoticeButton is_save_read_btn2[] = {
    NBTNARGS( ContReadCB, NULL, 'O', True, False ),
    NBTNARGS( CancelCB, NULL, 'C', True, True ),
    };
    static NButton SaveReadBTN1 = NBUTTONS( is_save_read_btn1 );
    static NButton SaveReadBTN2 = NBUTTONS( is_save_read_btn2 );

	if (QuitEditPtn((Widget)NULL, (caddr_t)NULL, (caddr_t)NULL)){
	    FalCloseFont(font_id);
	    editPtnW = NULL;
	    OtherFontSelect();
	    PopupSelectXLFD(toplevel);
	}
	else{
	    if (! fullFontData.prm) {
	        SetItemLabel(&SaveReadBTN1, 0, resource.l_do_save_exit);
	        SetItemLabel(&SaveReadBTN1, 1, resource.l_dont_save_exit);
	        SetItemLabel(&SaveReadBTN1, 2, resource.l_cancel);
	        PopupNotice( (w), resource.mn_saved_open ,
		    D_QUESTION,
		    &SaveReadBTN1,
		    True,
		    resource.l_question_title);
	    } else {
	        SetItemLabel(&SaveReadBTN2, 0, resource.l_dont_save_exit);
	        SetItemLabel(&SaveReadBTN2, 1, resource.l_cancel);
	        PopupNotice( (w), resource.mn_saved_open ,
		    D_QUESTION,
		    &SaveReadBTN2,
		    True,
		    resource.l_question_title);
	    }
	}
}


void
ReadCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (fullFontData.xlfdname == NULL) {
	return;
    }
    if (fullFontData.xlfdname == (char *) -1) {
	Error_message(w, resource.mn_prop_font);
	return;
    }
    CBmOblB_edit( w, client_data, call_data );
}

/****************************************************************
 * callback routines and event handler 				*
 ***************************************************************/

/*
 * contents : write character patterns to SNF file
*/

static Boolean
WPwriteSNF( int restart )
{
    int		rc, err;
    char	str[MG_MAXSIZE];

    switch( rc = writeSNF( restart, &err ) ) {
    case -1:
	if (dnPopW)
	    XtSetSensitive( dnPopW,  TRUE );
	switch( err ) {
	case 001:
	    sprintf(str, "%s(permission denied)", resource.me_write_snf);
	    break;
        case 002:
            sprintf(str, "%s(disk full)", resource.me_write_snf);
            break;
	case 101:
	    sprintf(str, "%s(pipe error)", resource.me_write_snf);
	    break;
	case 102:
	    sprintf(str, "%s(fork error)", resource.me_write_snf);
	    break;
	case 103:
	    sprintf(str, "%s(execv error)", resource.me_write_snf);
	    break;
	case 104:
	    sprintf(str, "%s(data error)", resource.me_write_snf);
	    break;
	default:
	    sprintf(str, "%s", resource.me_write_snf);
	}
	UpdateMessage( str );
	return( TRUE );
    case 0:
	edg.flag = 0;
	UpdateMessage( resource.mg_register );
	sleep(1);
	UpdateMessage("");

	if (do_read){
	    do_read = False;
	    ContReadCB((Widget)NULL);
	}
	if (do_end){
	    do_read = False;
	    ContEndCB();
	}
	return(TRUE);
    default:
	if( rc == 1101 )
	    sprintf( str, "%s", resource.me_wait );
	else
	    sprintf( str, "%s(%3d%%)", resource.mg_write_snf , rc-1000 );

	UpdateMessage( str );
	XtAppAddWorkProc( app, (XtWorkProc)WPwriteSNF, (XtPointer)ON );
	if( rc == 1101 ){
	    XSync( xl.display,0 );
	}
	return( TRUE );
    }
}



/*
 * contents : write character patterns to SNF file
 */

void
CBeOblB_aAdd(void)
{
    char	str[MG_MAXSIZE];

    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
    UpdateMessage( resource.mg_write_snf );

    if( edg.code != 0 )
	ptnAdd( edg.code, edg.ptn );

    if( edg.flag == ON ) {
	sprintf( str, "%s", resource.mg_write_snf );
	if (dnPopW)
	    XtSetSensitive( dnPopW,  FALSE );

	XtAppAddWorkProc( app, (XtWorkProc)WPwriteSNF, (XtPointer)OFF );
	return;
    }
    else{
	UpdateMessage( "" );
    }
}



/*
 * contents : destroy the editor window
 */

static void
OtherFontSelect(void)
{
    dstrypaneEditPtn();
    xeg_init();
}



/*
 * contents : close dtudcfonted
 */

/*ARGSUSED*/
void
CBeOblB_aEnd( Widget widget, caddr_t clientData, caddr_t callData )
{
    static NoticeButton is_save_exit_btn[] = {
    NBTNARGS( SaveEndCB, NULL, 'S', True, False ),
    NBTNARGS( ContEndCB, NULL, 'E', True, False ),
    NBTNARGS( CancelCB,  NULL, 'C', True, True ),
    };
    static NButton SaveEndBTN = NBUTTONS( is_save_exit_btn );

    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );

    if( edg.code != 0 )
	ptnAdd( edg.code, edg.ptn );
    if( edg.flag == ON ) {

	SetItemLabel(&SaveEndBTN, 0, resource.l_do_save);
	SetItemLabel(&SaveEndBTN, 1, resource.l_dont_save);
	SetItemLabel(&SaveEndBTN, 2, resource.l_cancel);
	PopupNotice( widget, resource.mn_saved_exit, D_QUESTION,
			    &SaveEndBTN, True, resource.l_question_title);
	return;
    } else {
        FalCloseFont(font_id);
    }

    exit(0);
}



/*ARGSUSED*/
void
CBeOblB_rCmd( Widget widget, int proc, caddr_t callData )
{
    extern void SelectUnset();
    extern void UndoSet();
    if (!select_x && !select_y && !select_w && !select_h)
	return;
    em.rsv_f = ON;
    bitPtnCopy( em.rsv_ptn,  edg.ptn );
    rubBand( r1_x,  r1_y,  r2_x,  r2_y );
    switch( proc ) {
    case PROC_CLS:
	bitDrawCls (edg.ptn, select_x, select_y, select_w, select_h );
	break;
    case PROC_SET:
	bitDrawSet (edg.ptn, select_x, select_y, select_w, select_h );
	break;
    case PROC_REV:
	bitDrawRev (edg.ptn, select_x, select_y, select_w, select_h );
	break;
    case PROC_ROLL:
	bitDrawRoll(edg.ptn, select_x, select_y, select_w, select_h);
	break;
    case PROC_SYMV:
	bitDrawSymV(edg.ptn, select_x, select_y, select_w, select_h );
	break;
    case PROC_SYMH:
	bitDrawSymH(edg.ptn, select_x, select_y, select_w, select_h );
	break;
    }
    edg.flag = ON;
    DrawRectEdPn( 0, 0,  edg.width - 1,  edg.height - 1 );
    DrawDpPn();
    select_x = select_y = select_w = select_h = 0;
    SelectUnset();
    UndoSet();
}

/*ARGSUSED*/
void
CBeOblB_rCmdp( Widget widget, int proc, caddr_t callData )
{
    extern Widget wgeBulB_edit;
    extern void CopySet();
    extern void UndoSet();
    extern void SelectUnset();

    switch( proc ) {
    case PROC_CPY:
	if (!select_x && !select_y && !select_w && !select_h)
	    return;
	rubBand( r1_x,  r1_y,  r2_x,  r2_y );
	bitDrawCpy(edg.ptn, select_x, select_y, select_w, select_h, False);
	cut_w = select_w;
	cut_h = select_h;
	select_x = select_y = select_w = select_h = 0;
	CopySet();
	SelectUnset();
	break;
    case PROC_CUT:
	if (!select_x && !select_y && !select_w && !select_h)
	    return;
	em.rsv_f = ON;
	rubBand( r1_x,  r1_y,  r2_x,  r2_y );
	bitPtnCopy( em.rsv_ptn,  edg.ptn );
	bitDrawCpy(edg.ptn, select_x, select_y, select_w, select_h, True);
	cut_w = select_w;
	cut_h = select_h;
	edg.flag = ON;
	DrawRectEdPn( 0, 0,  edg.width - 1,  edg.height - 1 );
	DrawDpPn();
	select_x = select_y = select_w = select_h = 0;
	CopySet();
	UndoSet();
	SelectUnset();
	break;
    case PROC_PASTE:
	XtAddEventHandler(wgeBulB_edit,
			    ButtonReleaseMask|PointerMotionMask,
				    False, musPasteProc, NULL );
    }
}




/*
 * contents : cancel all editting
 */

/*ARGSUSED*/
void
CBeOblB_rCan( Widget widget, caddr_t clientData, caddr_t callData )
{
    extern void UndoUnset();

    resetEditMode( RES_MSG | RES_PROC | RES_SLCT );

    if( em.rsv_f == ON ) {
	bitPtnCopy( edg.ptn, em.rsv_ptn );
	em.rsv_f = OFF;

	DrawRectEdPn( 0, 0, edg.width - 1, edg.height - 1 );
	DrawDpPn();
        UndoUnset();
    }
}




/*
 *
 *   contents : get a sequential number of the editor
 */
int
RelToAbsSq( int from, int cnt)
{
    int i;
    int no;

    if ( cnt >= 0) {
        for ( i = -1; from < (edlist.sqstart + edlist.nptn) ; from++) {
            no = ptnSqToNo(from);
            if (( no >= 0) && ( noToCode( no) >= 0) ) {
                i++;
                if ( i >= cnt)
                    return( from);
            }
        }
    } else {
        cnt *= -1;
        for ( i = -1; from >= edlist.sqstart; from--) {
            no = ptnSqToNo(from);
            if (( no >= 0) && ( noToCode( no) >= 0) ) {
                i++;
                if ( i >= cnt)
                    return(from);
            }
        }
    }
    return( -1);
}

/*
 *   contents : get a relative number of the system area
 */
int
AbsSqToRel( int from, int to)
{
    int sq;
    int cnt;
    int sign = 1;
    int no;

    if ( from > to) {
	sq = from;
	from = to;
	to = sq;
	sign = -1;
    }

    for ( cnt = -1, sq = from; sq <= to; sq++) {
	no = ptnSqToNo(sq);
	if (( no >= 0) && (codeCheck( noToCode( no)) == 0))
	    cnt++;
    }

    if ( cnt < 0)
	cnt = 0;

    cnt *= sign;
    return( cnt);
}


/*
 * contents : be the character list selected
 */

/*ARGSUSED*/
void
EHeStaT_list( Widget widget, int select, XEvent *e )
{
    int		sq, no;
    int		code;

    resetEditMode( RES_MSG|RES_PROC|RES_SLCT|RES_RSV );

    if( (e->xbutton.button != 1) || (e->type != ButtonRelease) ||
	(e->xbutton.x < 0) || ((int)edlist.elem_w < e->xbutton.x) ||
	(e->xbutton.y < 0) || ((int)edlist.elem_h < e->xbutton.y)
       )
	return;

    if( edlist.slctloc == select )
	return;

    sq = RelToAbsSq( edlist.sqstart + edlist.statloc, select);

    if( (no = ptnSqToNo(sq)) == -1 )
	return;

    code = noToCode( no );
    if( codeCheck( code ) == -1 )
	return;

    if( ptnSense( code) == 0 )
	return ;

    chgEdList( edlist.statloc, select, OFF );
    if( ptnSense(edg.code) == 1 )
	ptnAdd(edg.code, edg.ptn );
    chgEdPtn( code );
}




/*
 * contents : update the character list
 */

/*ARGSUSED*/
void
CBeScro( Widget widget, caddr_t clientData, caddr_t callData )
{
    int		newl;
    int         new_statloc;
    int         new_slct;

    n = 0;
    XtSetArg( arg[n], XmNvalue,   (XtArgVal)&newl );	n++;
    XtGetValues( wgeScro , arg, n );

    if (( new_statloc = RelToAbsSq( edlist.sqstart, newl)) < 0) {
        new_statloc = 0;
    } else {
        new_statloc -= edlist.sqstart;
    }
    /*
     */
    new_slct = edlist.slctloc
	- AbsSqToRel( edlist.sqstart + edlist.statloc,
		      edlist.sqstart + new_statloc);
    chgEdList( new_statloc, new_slct, OFF);
}




/*
 * contents : select the edit items by mouse
 */

/*ARGSUSED*/
void
EHeBulB_eMEv( Widget widget, caddr_t clientData, XEvent *e )
{
    int		px, py;
    int         downbutton;

    if (edpane.pix_w * edpane.pix_h == 0 ) {
	return;
    }

    if( e->type == ButtonPress ){
	em.drag_f = ON;
    }
    if( (edg.code == 0) || (em.drag_f == OFF) )
	return;
    if( e->type == ButtonRelease ){
	em.drag_f = OFF;
    }

    px = e->xbutton.x / edpane.pix_w;
    py = e->xbutton.y / edpane.pix_h;

    if (( e->type == ButtonPress) || (e->type == ButtonRelease)){
	downbutton = e->xbutton.button;
    }else if ( e->type == MotionNotify ){
	if ( e->xmotion.state & Button1Mask ){
	    downbutton = 1; /* select button */
	}else if ( e->xmotion.state & Button2Mask ){
	    downbutton = 2; /* adjust button */
	} else {
	    downbutton = 0;
	}
    }

    switch( downbutton ) {
    case 1:
	switch( em.proc ) {
	case PROC_POINT:
	    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
	    musPoint( e->type, px, py );
	    break;
	case PROC_LINE:
	    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
	    musLine( e->type, px, py );
	    break;
	case PROC_CIRCLE:
	    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
	    musCircle( e->type, px, py );
	    break;
	case PROC_RECT:
	    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
	    musRect( em.proc, e->type, px, py );
	    break;
	case PROC_ERASE:
	    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
	    musPoint( e->type, px, py );
	    break;
	case PROC_SELECT:
	    musRegionProc( em.proc, e->type, px, py );
	    break;
	default:
	    break;
	}
	break;

    case 2:
	if( (0 <= px) && (px < edg.width) && (0 <= py) && (py < edg.height) ) {
	    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );

	    switch( e->type ) {
	    case MotionNotify:
		if( (em.adj_px == px) && (em.adj_py == py) )
		    return;
		break;
	    case ButtonPress:
		em.adj_px = px;
		em.adj_py = py;
		if( bitRead(edg.ptn, px, py) == 0 ) {
		    bitSet( edg.ptn, px, py );
		    DrawPointEdPn( px,py, 1 );
		    DrawPointDpPn( px,py, 1 );
		}
		else {
		    bitReset( edg.ptn, px, py );
		    DrawPointEdPn( px,py, 0 );
		    DrawPointDpPn( px,py, 0 );
		}
		edg.flag = ON;
		break;
	    default:
		return;
	    }
	}
    default:
	break;
    }
}




/*
 * contents : restore the editting pane
 */

/*ARGSUSED*/
void
EHeBulB_eExp( Widget widget, caddr_t clientData, XEvent *e )
{
    int		x1, y1;
    int		x2, y2;

    if (edpane.pix_w * edpane.pix_h == 0 ) {
	return;
    }

    x1 = e->xexpose.x / edpane.pix_w;
    y1 = e->xexpose.y / edpane.pix_h;
    x2 = (e->xexpose.x + e->xexpose.width - 1) / edpane.pix_w;
    y2 = (e->xexpose.y + e->xexpose.height - 1) / edpane.pix_h;

    DrawRectEdPn( x1, y1, x2, y2 );
    DrawBorderEdPn( x1, y1, x2, y2 );
}




/*
 * contents : specifies the drawing operation (Pont/Line/Rectangle/Circle)
 */

/*ARGSUSED*/
void
CBeRecB_obj( Widget widget, int obj, XmToggleButtonCallbackStruct *call)
{
    extern void SelectUnset();

    if (call->set == False)
	return;

    resetEditMode( RES_MSG | RES_RSV | RES_SLCT );

    if (obj == PROC_ERASE)
	edpane.color = OFF;
    else
	edpane.color = ON;
    edpane.obj = obj;
    em.proc = obj;
    if (obj != PROC_SELECT) {
	if (select_x || select_y || select_w || select_h) {
	    rubBand( r1_x,  r1_y,  r2_x,  r2_y );
	    select_x = select_y = select_w = select_h = 0;
	    SelectUnset();
	}
    }
}




/*
 * contents : restore the displaying pane
 */

/*ARGSUSED*/
void
EHeBulB_dExp( Widget widget, caddr_t clientData )
{
    if (xl.display == NULL ){
	return;
    }

    DrawDpPn();
}

/*
 * contents : set the range to be add or deleted
 */

static int
codeAreaSet(int *s_code, int *e_code)
{
    char	*str;
    char	delm;

    str = MngCodeTfValue();
    delm = '\0';
    *s_code = 0;
    *e_code = 0;
    if (!str || !*str){
	return(-1);
    }
    sscanf( str, "%x %c %x", s_code, &delm, e_code );
    if (str) free( str );
    switch( delm ) {
    case '\0':
	*e_code = *s_code;
	break;
    case '-':
	break;
    default:
	return(-1);
    }
    if( codeCheck(*s_code) || codeCheck(*e_code) ||
	(*s_code > *e_code) ) {
	return(-1);
    }
    return(0);
}


/****************************************************************
 * callback routines (character management window)		*
 ***************************************************************/

Boolean
BeforeMngCheck(int *s_code, int *e_code)
{
    if(codeAreaSet(s_code, e_code) == -1) {
	DispMngErrorMessage( resource.me_illegal_code );
	return(False);
    }
    return(True);
}

void
DoAddProc(int s_code, int e_code)
{
    int		code;

    int		s_ncode;
    int		e_ncode;
    char	ptn[MAXPTNBYTE];
    char	mode;
    int		i;

    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );

    s_ncode = codeToNo( s_code );
    e_ncode = codeToNo( e_code );
    mode = OFF;
    bitPtnClear( ptn );
    for( i=s_ncode ; i <= e_ncode ; i++ ) {
	code = noToCode(i);
	if ( codeCheck( code))
	    continue;
	if (ptnSense(code) == 0) {
	    if(ptnAdd(code, ptn) != 1) {
		UpdateMessage( resource.me_non_memory );
		return;
	    }
	    edg.flag = ON;
	    mode = ON;
	    edlist.nptn++;
	}
    }

    if( ptnSense( edg.code ) == 1 ) {
	ptnAdd( edg.code, edg.ptn );
    }

    if(mode == ON) {
	chgEdCode( s_code, mode );
    }

    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );
}



void PopupDelNotice();
Widget CreateDelNotice();


void
DoDelProc( int s_code, int e_code )
{
    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );

    if( ptnSense( edg.code) == 1 )
	ptnAdd( edg.code, edg.ptn );

    dn.s_ncode = codeToNo( s_code );
    dn.e_ncode = codeToNo( e_code );

    if (setRealDelArea(&dn.s_ncode, &dn.e_ncode, &dn.sq_start, &dn.sq_end) == -1) {
	DispMngErrorMessage( resource.me_nodelete_code );
	return;
    }
    dn.sq_top  = dn.sq_start;
    dn.disp_num = AbsSqToRel( dn.sq_start, dn.sq_end) + 1;
    dn.disp_num = (dn.disp_num <= D_MAX) ? dn.disp_num : D_MAX;

    PopupDelNotice( mngPtnW );

}




/*
 * contents : get the real range to be add or delete
 */


static int
setRealDelArea(int *s_ncode, int *e_ncode, int *sq_start, int *sq_end )
{
    int		ncode;
    int		flg;
    int		sq;
    int		i;

    /* first code */
    flg = 0;
    ncode = *e_ncode;
    for( i=*s_ncode ; i <= ncode ; i++ ) {
	if (((sq = ptnNoToSq(i)) != -1) && (codeCheck( noToCode(i)) == 0)) {
	    flg = 1;
	    break;
	}
    }
    if (flg == 1) {
	*s_ncode = ptnSqToNo( sq );
	*sq_start = sq;
    } else {
	return(-1);
    }

    /* last code */
    flg = 0;
    ncode = *s_ncode;
    for( i=*e_ncode ; i >= ncode ; i-- ) {
	if ((( sq = ptnNoToSq(i)) != -1) && (codeCheck( noToCode(i) ) == 0)) {
	    flg = 1;
	    break;
	}
    }
    *e_ncode = ptnSqToNo( sq );
    *sq_end = sq;
    return(0);
}






/****************************************************************
 * callback routines (Copy)					*
 ****************************************************************/

static int
CpySrcCodeCheck(void)
{
    char	*str;
    char	delm;

    str = CpySrcCodeTfValue();
    if (!str)
	return(-1);
    if (!*str) {
	free(str);
	return(-1);
    }
    delm = '\0';
    cpm.s1_code = 0;
    cpm.s2_code = 0;
    sscanf( str, "%x %c %x", &cpm.s1_code,&delm, &cpm.s2_code );
    free( str );
    switch( delm ) {
    case '\0':
	cpm.s2_code = cpm.s1_code;
	break;
    case '-':
	break;
    default:
	return(-1);
    }
    if (cpm.s1_code > cpm.s2_code)
	return(-1);
    return(0);
}

static int
CpyDestCodeCheck(void)
{
    char	*str;

    str = CpyDestCodeTfValue();
    if (!str)
	return(-1);
    if (!*str){
	free(str);
	return(-1);
    }
    cpm.d1_code = 0;
    sscanf( str, "%x", &cpm.d1_code );
    free( str );

    if( codeCheck(cpm.d1_code)  ) {
	return(-1);
    }
    return(0);
}

Boolean
BeforeCpyCheck( int proc )
{
    int		s_ncode, e_ncode;
    int		r1_code, r2_code;
    int		no;
    int		i;
    char	rstr[30];
    extern void PopupCpyNotice();

    if (copyFontData.xlfdname == NULL) {
	DispCpyErrorMessage(resource.me_non_srcfile);
        return(False);
    }

    if (CpySrcCodeCheck() == -1){
	DispCpyErrorMessage( resource.me_illegal_srccode );
	return(False);
    }

    if (CpyDestCodeCheck() == -1){
	DispCpyErrorMessage( resource.me_illegal_destcode );
	return(False);
    }

    s_ncode = codeToNo( cpm.d1_code );
    e_ncode = codeToNo( cpm.d1_code + cpm.s2_code - cpm.s1_code);
    r1_code = r2_code = 0;
    for( i=s_ncode ; i <= e_ncode ; i++ ) {
        no = ptnSqToNo(i);
        if (( no >= 0) && (codeCheck( noToCode( no)) == 0)) {
	    if(r1_code == 0)
		r1_code = noToCode(i);
	    r2_code = noToCode(i);
	}
    }

    cpm.proc = proc;

    if( r1_code != 0 ) {
	if( r1_code == r2_code ) {
	    sprintf( rstr, "%s %x¡¡", resource.l_code, r1_code );
	} else if ( codeCheck(r1_code) && codeCheck(r2_code)){
	    sprintf( rstr, "	    %x - %x", r1_code, r2_code );
	} else {
	    sprintf( rstr, "%s %x - %x ", resource.l_code_range, r1_code, r2_code );
	}
	PopupCpyNotice(rstr);

	return(False);
    }
    return(True);
}

void
DoCpyProc(void)
{
    extern void PopdownCpyPtn();

    PopdownCpyPtn();

    resetEditMode( RES_MSG | RES_PROC | RES_SLCT | RES_RSV );

    copyPatterns( cpm.fdata, cpm.s1_code, cpm.s2_code, cpm.d1_code, cpm.proc );
}

/****************************************************************
 * main routine							*
 ****************************************************************/
#define R(name, class, member,def)      { \
	name, \
	class, \
	XtRString, \
	sizeof(char *), \
	XtOffsetOf(Resource, member), \
	XtRString, \
	(XtPointer)def }

#define RBoolean(name, class, member,def) \
	{ name, class, XtRBoolean, sizeof(XtRBoolean), \
	XtOffsetOf(Resource, member), XtRString, (XtPointer)def }

#define RDimension(name, class, member,def) \
	{ name, class, XtRDimension, sizeof(XtRDimension), \
	XtOffsetOf(Resource, member), XtRString, (XtPointer)def }

#define Rint(name, class, member,def) \
	{ name, class, XtRInt, sizeof(XtRInt), \
	XtOffsetOf(Resource, member), XtRString, (XtPointer)def }

#define RPixel(name, class, member,def) \
	{ name, class, XtRPixel, sizeof(XtRPixel), \
	XtOffsetOf(Resource, member), XtRString, (XtPointer)def }

static XtResource app_resources[] = {
RBoolean("Codepoint", "codepoint", codepoint, "False"),

R( "lptog", "Lptog", l_ptog, DFLTPATH ),
R( "lptogCmd", "LptogCmd", l_ptog_cmd, DFLTCMD ),

/* CreateCaptionFrame() */
RBoolean( "captResize",  "CaptResize",  capt_resize,  "True"  ) ,
Rint( "captLftoff",  "CaptLftoff",  capt_lftoff,  "2"  ) ,
Rint( "captTopoff",  "CaptTopoff",  capt_topoff,  "2"  ) ,
Rint( "captRghoff", "CaptRghoff", capt_rghoff, "2" ) ,

RPixel( "paneBackground", "paneBackground", pane_background, "white" ),
RPixel( "paneForeground", "PaneForeground", pane_foreground, "black" ),
RPixel( "background", "Background", background, "white" ),
RPixel( "foreground", "Foreground", foreground, "black" ),

/* CreateMenuButtons() */
RDimension( "menuMargin", "MenuMargin", menu_margin, "0" ) ,

/* CreateDialogAndButtons() */
RBoolean( "diaTmAutomng",  "DiaTmAutomng",  dia_tm_automng,  "False" ) ,
RDimension( "diaTmWidth",  "DiaTmWidth",  dia_tm_width,  "4" ) ,
RDimension( "diaTmHeight", "DiaTmHeight", dia_tm_height, "4" ) ,
RBoolean( "diaTmResize",   "DiaTmResize",   dia_tm_resize,   "True" ) ,
RBoolean( "diaTmMinimize", "DiaTmMinimize", dia_tm_minimize, "True" ) ,

RDimension( "diaRwWidth",   "DiaRwWidth",   dia_rw_width,   "6" ) ,
RDimension( "diaRwHeight",  "DiaRwHeight",  dia_rw_height,  "6" ) ,
RDimension( "diaRwSpacing", "DiaRwSpacing", dia_rw_spacing, "4" ) ,

/* CreateDrawingArea() */
RDimension( "drawBorder", "DrawBorder", draw_border, "1" ) ,
Rint( "drawTopoff", "DrawTopoff", draw_topoff, "2" ) ,
Rint( "drawLftoff", "DrawLftoff", draw_lftoff, "2" ) ,

/* PopupNotice() */
RBoolean( "popResize",   "PopResize",   pop_resize ,   "True" ) ,
RBoolean( "popMinimize", "PopMinimize", pop_minimize , "True" ) ,

/* CreateTemplateDialog() */
RBoolean( "tempResize",   "TempResize",   temp_resize,   "True" ) ,
RBoolean( "tempMinimize", "TempMinimize", temp_minimize, "True" ) ,

RDimension( "tempWidth",  "TempWidth",  temp_width,  "1" ) ,
RDimension( "tempHeight", "TempHeight", temp_height, "1" ) ,

/* CreateMenuBarAndFooterMessageForm() */
Rint( "edBarTopoff", "EdBarTopoff", ed_bar_topoff, "1" ) ,
Rint( "edBarLftoff", "EdBarLftoff", ed_bar_lftoff, "1" ) ,
Rint( "edBarRghoff", "EdBarRghoff", ed_bar_rghoff, "1" ) ,

Rint( "edFrmBtmoff", "EdFrmBtmoff", ed_frm_btmoff, "3" ) ,
Rint( "edFrmLftoff", "EdFrmLftoff", ed_frm_lftoff, "3" ) ,
Rint( "edFrmRghoff", "EdFrmRghoff", ed_frm_rghoff, "3" ) ,

RDimension( "edMsgMargintop", "EdMsgMargintop", ed_msg_margintop, "3" ) ,
RDimension( "edMsgMarginbtm", "EdMsgMarginbtm", ed_msg_marginbtm, "3" ) ,

RDimension( "edMesgMargintop", "EdMesgMargintop", ed_mesg_margintop, "3" ) ,
RDimension( "edMesgMarginbtm", "EdMesgMarginbtm", ed_mesg_marginbtm, "3" ) ,

Rint( "edSepBtmoff", "EdSepBtmoff", ed_sep_btmoff, "3" ) ,
Rint( "edSepLftoff", "EdSepLftoff", ed_sep_lftoff, "0" ) ,
Rint( "edSepRghoff", "EdSepRghoff", ed_sep_rghoff, "0" ) ,
RDimension( "edSepMargin", "EdSepMargin", ed_sep_margin, "0" ) ,

Rint( "edWgeTopoff", "EdWgeTopoff", ed_wge_topoff, "10" ) ,
Rint( "edWgeLftoff", "EdWgeLftoff", ed_wge_lftoff, "10" ) ,
Rint( "edWgeRghoff", "EdWgeRghoff", ed_wge_rghoff, "10" ) ,
Rint( "edWgeBtmoff", "EdWgeBtmoff", ed_wge_btmoff, "10" ) ,

/* CreateFrame() */
RBoolean( "frameResize", "FrameResize", frame_resize, "False" ) ,

/* CreateScrollBase() */
RDimension( "scllFrWidth", "ScllFrWidth", scll_fr_width, "500" ) ,

RDimension( "scllFrThick", "ScllFrThick", scll_fr_thick, "2" ) ,

Rint( "scllBarLftoff", "ScllBarLftoff", scll_bar_lftoff, "4" ) ,
};
#undef R
#undef RBoolean
#undef RDimension
#undef Rint
#undef RPixel

int CodePoint = False;

/*ARGSUSED*/
static void
xerror(Display *d, XErrorEvent *ev)
{
    fprintf (stderr, "dtudcfonted:  warning, error event receieved.\n");
    exit(-1);
}

int
main(int argc, char *argv[])
{
    static char class_name[] = "Dtudcfonted";
    int i;

    /* initialize GUI */
    toplevel = GuiInitialize(&app, class_name, &argc, argv);

    /* get application's resources */
    XtGetApplicationResources( toplevel, &resource,
		app_resources, XtNumber(app_resources), NULL, 0);

    if (resource.codepoint)
	CodePoint = True;
    /* initialize the code range */
    for( i=1  ;  i < argc  ; i++  ) {
	if( !strcmp(argv[i],"-pane_size") ) {
	    i++;
	    edpane_size = atoi( argv[i] );
	} else if( !strcmp(argv[i],"-codepoint") ) {
	    CodePoint = True;
	}
    }

    resource.mg_write_snf = GETMESSAGE(2, 2, "Registration processing inside.");
    resource.me_write_snf = GETMESSAGE(2, 4, "Failed in the registration of the character.");
    resource.me_illegal_code = GETMESSAGE(2, 6, "Illegal code.");
    resource.me_non_memory = GETMESSAGE(2, 8, "Because the memory allocation cannot be done, it is not possible to add.");
    resource.mg_non_code = GETMESSAGE(2, 10, "There is no character of the specified code.");
    resource.me_illegal_srccode = GETMESSAGE(2, 12, "The mistake is found in the specified copy origin code.");
    resource.me_illegal_destcode = GETMESSAGE(2, 14, "The mistake is found in the specified copy target code.");
    resource.me_nodelete_code = GETMESSAGE(2, 16, "There is no character in the specified area.");
    resource.me_wait = GETMESSAGE(2, 18, "Please wait for a while.");
    resource.me_non_srcfile = GETMESSAGE(2, 20, "No copy origin code file is specified.");
    resource.mn_no_font = GETMESSAGE(2, 22, "Informations of user defined character cannot collect.\nThere is no font specified.");
    resource.mn_plural_font = GETMESSAGE(2, 24, "Informations of user defined character cannot collect.\nThere are plural fonts specified.");
    resource.mn_not_udc = GETMESSAGE(2, 26, "The specified font do not have any UDCs areas.");
    resource.mn_no_mem = GETMESSAGE(2, 28, "There are no memories any more.");
    resource.mn_illegal_area = GETMESSAGE(2, 30, "The information of UDCs area is illegal.");
    resource.mn_no_read = GETMESSAGE(2, 32, "The font file cannot read.\nPattern area was broken.");
    resource.mn_prop_font = GETMESSAGE(2, 34, "Dtudcfonted cannot edit proportional character font.");
    resource.mn_saved_open = GETMESSAGE(2, 36, "There are unsaved edits.\nDo you want to save them before opening?");
    resource.mn_saved_exit = GETMESSAGE(2, 38, "There are unsaved edits.\nDo you want to save them before exiting?");
    resource.mn_cpy_lost = GETMESSAGE(2, 40, "When the copy processing is done, the glyph\nimage within the following ranges is changed.\nDo you want to do the copy processing?\n\n");
    resource.mn_no_perm = GETMESSAGE(2, 42, "No write permission for this font. You should change the file access mode before saving.\n");

    resource.l_selectitem = GETMESSAGE(2, 100, "Select Items");	/* font open */
    resource.l_codeset = GETMESSAGE(2, 102, "         CodeSet:");
    resource.l_style = GETMESSAGE(2, 104, "           Style:");
    resource.l_width = GETMESSAGE(2, 106, "           Width:");
    resource.l_height = GETMESSAGE(2, 108, "          Height:");
    resource.exec_label = GETMESSAGE(2, 110, "Open");
    resource.quit_label = GETMESSAGE(2, 112, "Quit");

    resource.l_font = GETMESSAGE(2, 114, "Font");		/* menu bar */
    resource.l_edit = GETMESSAGE(2, 116, "Edit");
    resource.l_manage = GETMESSAGE(2, 118, "Character");
    resource.l_info = GETMESSAGE(2, 120, "Information");

    resource.l_open_w = GETMESSAGE(2, 122, "Open");		/* font menu */
    resource.l_save = GETMESSAGE(2, 124, "Save");
    resource.l_exit = GETMESSAGE(2, 126, "Exit");

    resource.l_clear = GETMESSAGE(2, 128, "Clear");		/* edit menu */
    resource.l_set = GETMESSAGE(2, 130, "Set");
    resource.l_reverse = GETMESSAGE(2, 132, "Reverse");
    resource.l_cut = GETMESSAGE(2, 134, "Cut");
    resource.l_Copy = GETMESSAGE(2, 135, "Copy");
    resource.l_paste = GETMESSAGE(2, 136, "Paste");
    resource.l_roll = GETMESSAGE(2, 138, "Rotate");
    resource.l_updown_roll = GETMESSAGE(2, 140, "Vertical Roll");
    resource.l_leftright_roll = GETMESSAGE(2, 142, "Horizontal Roll");
    resource.l_undo = GETMESSAGE(2, 144, "Undo");

    resource.l_manage_w = GETMESSAGE(2, 146, "Add/Delete...");	/* character menu */
    resource.l_copy_w = GETMESSAGE(2, 148, "Copy...");

    resource.l_xlfd = GETMESSAGE(2, 150, "XLFD name...");		/* information menu */
    resource.l_codearea = GETMESSAGE(2, 152, "Code Area...");

    resource.l_add = GETMESSAGE(2, 154, "Add");		/* add/delete dialog */
    resource.l_delete = GETMESSAGE(2, 156, "Delete");
    resource.l_cancel = GETMESSAGE(2, 158, "Cancel");

    resource.l_manage_code = GETMESSAGE(2, 160, "Code(hex):"); /* copy dialog */
    resource.l_copy_src_code = GETMESSAGE(2, 162, " Copy origin code(hex):");
    resource.l_copy_dest_code = GETMESSAGE(2, 164, "Copy target code(hex):");
    resource.l_copy = GETMESSAGE(2, 166, "Copy");
    resource.l_overlay = GETMESSAGE(2, 168, "Overlay");
    resource.l_ok = GETMESSAGE(2, 170, "OK");

    resource.l_do_save = GETMESSAGE(2, 172, "Save");	/* unsaved edits */
    resource.l_dont_save = GETMESSAGE(2, 174, "Do not Save");
    resource.l_do_save_exit = GETMESSAGE(2, 176, "Save and Exit");
    resource.l_dont_save_exit = GETMESSAGE(2, 178, "Do not Save and Exit");

    resource.mg_start_del_s = GETMESSAGE(2, 180, "The following character will be deleted.");	/* delete notice dialog */
    resource.mg_start_del_cs = GETMESSAGE(2, 182, "character");
    resource.mg_start_del_m = GETMESSAGE(2, 184, "The following characters will be deleted.");
    resource.mg_start_del_cm = GETMESSAGE(2, 186, "characters");
    resource.mg_register = GETMESSAGE(2, 188, "Register completed");
    resource.l_code = GETMESSAGE(2, 190, " Code : ");
    resource.l_code_range = GETMESSAGE(2, 192, " Code range : ");

    resource.message = GETMESSAGE(2, 194, "Message : ");

    resource.code = GETMESSAGE(2, 196, "code...");	/* code dialog */
    resource.previous = GETMESSAGE(2, 198, "Page up");
    resource.next = GETMESSAGE(2, 200, "Page down");
    resource.apply = GETMESSAGE(2, 202, "Apply");

    resource.l_question_title = GETMESSAGE(2, 204, "dtudcfonted - Question");
    resource.l_warning_title = GETMESSAGE(2, 206, "dtudcfonted - Warning");
    resource.l_error_title = GETMESSAGE(2, 208, "dtudcfonted -  Error");
    resource.l_copy_title = GETMESSAGE(2, 210, "dtudcfonted - Copy");
    resource.l_open_title = GETMESSAGE(2, 212, "dtudcfonted - Open");
    resource.l_add_title = GETMESSAGE(2, 214, "dtudcfonted - Add/Delete");
    resource.l_xlfd_title = GETMESSAGE(2, 216, "dtudcfonted - XLFD");
    resource.l_codearea_title = GETMESSAGE(2, 218, "dtudcfonted - CodeArea");

    resource.file_name = GETMESSAGE(2, 230, "File name");
    resource.xlfd_name = GETMESSAGE(2, 232, "XLFD name");
    resource.codearea = GETMESSAGE(2, 234, "Code area");

    resource.falerrmsg[0] = GETMESSAGE(2, 300, "Fatal error occurred.");
    resource.falerrmsg[1] = GETMESSAGE(2, 301, "Cannot open the font file.");
    resource.falerrmsg[2] = GETMESSAGE(2, 302, "Cannot read the font file.");
    resource.falerrmsg[3] = GETMESSAGE(2, 303, "There is no more memory.");
    resource.falerrmsg[4] = GETMESSAGE(2, 304, "Fatal error occurred.");
    resource.falerrmsg[5] = GETMESSAGE(2, 305, "The specified font file does not exist.");
    resource.falerrmsg[6] = GETMESSAGE(2, 306, "This font is not a pcf or snf font.");
    resource.falerrmsg[7] = GETMESSAGE(2, 307, "Cannot open fonts.list file.");
    resource.falerrmsg[8] = GETMESSAGE(2, 308, "The format of fonts.list file is illegal.");
    resource.falerrmsg[9] = GETMESSAGE(2, 309, "The descriptions of the fonts.list file are incorrect.");
    resource.falerrmsg[10] = GETMESSAGE(2, 310, "The format of fonts.list file is illegal.");
    resource.falerrmsg[11] = GETMESSAGE(2, 311, "Cannot open fonts.dir file.");
    resource.falerrmsg[12] = GETMESSAGE(2, 312, "Cannot read fonts.dir file.");
    resource.falerrmsg[13] = GETMESSAGE(2, 313, "Cannot read font properties.");
    resource.falerrmsg[14] = GETMESSAGE(2, 314, "Cannot get FONT property.");
    resource.falerrmsg[15] = GETMESSAGE(2, 315, "Cannot get FAMILY_NAME property.");
    resource.falerrmsg[16] = GETMESSAGE(2, 316, "This font file is already opened by other application.");
    resource.falerrmsg[17] = GETMESSAGE(2, 317, "Cannot lock font file.");
    resource.falerrmsg[18] = GETMESSAGE(2, 318, "Cannot unlock font file.");
    resource.falerrmsg[19] = GETMESSAGE(2, 319, "Cannot get lock information from the font file.");
    resource.falerrmsg[20] = GETMESSAGE(2, 320, "Cannot find the specified font file.");
    resource.falerrmsg[21] = GETMESSAGE(2, 321, "Cannot read NLS database.");
    resource.falerrmsg[22] = GETMESSAGE(2, 322, "Cannot get charset names from NLS database.");
    resource.falerrmsg[23] = GETMESSAGE(2, 323, "Charset name not defined in NLS database.");
    resource.falerrmsg[24] = GETMESSAGE(2, 324, "The specified font has not been opened.");
    resource.falerrmsg[25] = GETMESSAGE(2, 325, "Fatal error occurred.");

    xlfdDialog   = NULL ;
    cpyDialog    = NULL ;

    /* initialize the global values and allocate the memories */
    xeg_init();


    /* displays the selection window */
    PopupSelectXLFD(toplevel);

    XSetErrorHandler((XErrorHandler)xerror);
    XSetIOErrorHandler((XIOErrorHandler)xerror);

    /* main loop */
    XtAppMainLoop(app);

    return(-1);
}


/*
 * contents : initialize the global values
 */

static void
xeg_init(void)
{
    int i;

    edg.code = edg.ncode = 0;
    bitPtnClear( edg.ptn );
    edg.flag = (char)0;

    edlist.nlist = EDLIST_MAX;
    edlist.statloc = edlist.slctloc = 0;
    edlist.nptn = 0;
    edlist.sqstart = 0;
    edlist.list_h = edlist.elem_w = edlist.elem_h = (Dimension)0;
    edlist.back = resource.pane_background;
    edlist.border = resource.pane_foreground;

    if ( edpane_size ){
	edpane.width = edpane_size;
	edpane.height = edpane_size;
    } else {
	edpane.width  = EDPANE_SIZE;
	edpane.height = EDPANE_SIZE;
    }
    edpane.pix_w  = edpane.pix_h = 0;
    edpane.color  = 0;
    edpane.obj    = 0;
    edpane.back   = resource.pane_background;
    edpane.border = resource.pane_foreground;

    xl.display    = NULL;
    xl.root_winID = xl.edit_winID = xl.disp_winID = xl.list_winID =
			(Window)NULL;
    xl.borderGC   = xl.backGC = xl.rubGC = xl.dashGC = (GC)NULL;
    xl.dispImage  = NULL;

    dn.elem_w = dn.elem_h = (Dimension)0;
    dn.ptn_w  = dn.ptn_h  = (Dimension)0;
    dn.list_h = (Dimension)0;
    dn.s_ncode = dn.e_ncode = 0;
    dn.sq_top = dn.sq_start = dn.sq_end = 0;
    dn.disp_num = 0;

    for ( i=0 ; i<D_MAX ; i++ ) {
	dl[i].disp_winID = (Window)NULL;
	dl[i].dispImage = NULL;
	bitPtnClear( dl[i].ptn );
    }
    for ( i=0 ; i<EDLIST_MAX ; i++ ) {
	dl_glyph[i].disp_winID = (Window)NULL;
	dl_glyph[i].dispImage = NULL;
	bitPtnClear( dl_glyph[i].ptn );
	sq_disp[i] = 0 ;
    }
    em.drag_f = OFF;
    em.adj_px = em.adj_py = 0;
    em.proc = em.slct_f = 0;
    em.src1_px = em.src1_py = 0;
    em.src2_px = em.src2_py = 0;
    em.dest_px = em.dest_py = 0;
    em.rsv_f = (char)0;
    bitPtnClear( em.rsv_ptn );

    cpm.s1_code = cpm.s2_code = cpm.d1_code = 0;
    cpm.proc = 0;

    return;
}



/****************************************************************
 * distroy the "User defined character editor" window 		*
 ****************************************************************/

/*
 * contents : destroy the editting window
 */

static void
dstrypaneEditPtn(void)
{
    int i ;

    ptnClose();

    XFree( (char *)xl.dispImage );
    XFreeGC( xl.display, xl.borderGC );
    XFreeGC( xl.display, xl.backGC );
    XFreeGC( xl.display, xl.rubGC );
    XFreeGC( xl.display, xl.dashGC );

    for( i=0; i<EDLIST_MAX; i++ ){
	if( dl_glyph[i].dispImage ){
	    XFree( (char *)dl_glyph[i].dispImage );
	    dl_glyph[i].dispImage = NULL ;
	}
    }

    XtPopdown( editPopW );
    XtDestroyWidget( editPopW );

    return;
}


/****************************************************************
 * update character list 					*
 ***************************************************************/

/*
 * contents : 	add the specified code to the character list
 */

void
chgEdCode( int code, char mode )
{
    int	ncode;
    int	esq;
    int	statloc;
    int	slctloc;
    int	slimax;

    if( code == 0 ) {
	esq = -1;
    } else {
	ncode = codeToNo( code );
	esq = ptnNoToSq( ncode ) - edlist.sqstart;
    }
    if( esq < 0 ) {
	slctloc = -1;
	statloc = 0;
    } else if (efctPtnNum() <= edlist.nlist) {
	statloc = 0;
	slctloc = AbsSqToRel(edlist.sqstart, ptnNoToSq( ncode));
    } else {
	if ((esq >= edlist.statloc)
	    && (esq <= (RelToAbsSq( edlist.sqstart + edlist.statloc,
			    edlist.nlist - 1) - edlist.sqstart))){
	    statloc = edlist.statloc;
            slctloc = AbsSqToRel(edlist.sqstart+statloc,
					edlist.sqstart + esq);
	} else {
	    statloc = esq;
	    slctloc = 0;
	}
        while( RelToAbsSq(edlist.sqstart+statloc, edlist.nlist - 1) < 0)
	    statloc = RelToAbsSq( edlist.sqstart + statloc, -1)
		       - edlist.sqstart;
	slctloc = AbsSqToRel( edlist.sqstart + statloc, edlist.sqstart + esq);
    }

    n = 0;
    if (( slimax = efctPtnNum()) < edlist.nlist)
	slimax = edlist.nlist;
    XtSetArg( arg[n], XmNmaximum,   (XtArgVal)slimax );	n++;
    XtSetArg( arg[n], XmNvalue, (XtArgVal)AbsSqToRel( edlist.sqstart, edlist.sqstart + statloc) );	n++;
    XtSetValues( wgeScro , arg, n );

    chgEdList( statloc, slctloc, mode );
    chgEdPtn( code );
}


/*
 * contents : rewrite the character list
 */

void
chgEdList( int statloc, int slctloc, char mode  )
{
    int		sq;
    int		i;
    char	str[6];
    int         no;
    int		code;
    extern void ListSetLabelStr();
    extern void ListSetGlyphImage();
    extern void ListUnselectItem();
    extern void ListSelectItem();

    if((mode == ON) || (edlist.statloc != statloc)){
	for (i=0, sq=edlist.sqstart+statloc; i < edlist.nlist; sq++){
	    no = ptnSqToNo(sq);
	    sq_disp[i] = sq ;

	    if ((no >= 0) && (code = noToCode(no)) >= 0 ){
		sprintf(str, "%4x", code);
		ListSetLabelStr(i, str);
		ListSetGlyphImage(i) ;
		if (i == 0)
		    edlist.statloc = sq - edlist.sqstart;
		i++;
	    } else if (sq > ( edlist.sqstart + edlist.nptn - 1)){
		strcpy(str, "    ");
		ListSetLabelStr(i, str);
		ListSetGlyphImage(i) ;
		i++;
	    }
	}
    }

    if( (mode == ON) || (edlist.slctloc != slctloc) ) {
	if((0 <= edlist.slctloc) && (edlist.slctloc < edlist.nlist)){
	    ListUnselectItem(edlist.slctloc);
	}
	if((0 <= slctloc) && (slctloc < edlist.nlist)){
	    ListSelectItem(slctloc);
	}
	edlist.slctloc = slctloc;
    }
}


/*
 * contents : rewrite the editting pane
 */

static void
chgEdPtn( int code )
{
    extern void SetCodeString();

	if (xl.display == NULL ){
		return;
	}

    if( code == 0 ) {
	XClearWindow( xl.display, xl.edit_winID );
	DrawBorderEdPn( 0, 0, edg.width - 1, edg.height - 1 );
	XClearWindow( xl.display, xl.disp_winID );
	XClearWindow( xl.display, xl.list_winID );
	bitPtnClear( edg.ptn );
	edg.code = 0;
	edg.ncode = 0;
	SetCodeString(0);
    }

    if( ptnGet( code, edg.ptn) == 0 ) {
	edg.code = code;
	edg.ncode = codeToNo( code );
	DrawRectEdPn( 0, 0, edg.width - 1, edg.height - 1 );

	SetCodeString(code);
	DrawDpPn();
    }
}





/****************************************************************
 * draw patterns to the editting pane				*
 ***************************************************************/

/*
 * contents : draw a rectangle
 *
 */

static void
DrawRectEdPn( int x1, int y1, int x2, int y2 )
{
    int	i, j, wk;
    short    cx1, cy1, cx2, cy2;
    XRectangle    recOn[MAXPTNSIZE*MAXPTNSIZE];
    XRectangle    recOff[MAXPTNSIZE*MAXPTNSIZE];
    int	nron, nroff;

    if (xl.display == NULL ){
	    return;
    }

    if( x2 < x1 ) {
	wk = x2;
	x2 = x1;
	x1 = wk;
    }
    if( y2 < y1 ) {
	wk = y2;
	y2 = y1;
	y1 = wk;
    }

    nron = nroff = 0;
    for( j=y1   ;   j <= y2   ;  j++ ) {
	for( i=x1   ;   i <= x2   ;   i++ ) {
	    cx1 = (short)(edpane.pix_w * i);
	    cy1 = (short)(edpane.pix_h * j);
	    cx2 = (short)(cx1 + edpane.pix_w - 1);
	    cy2 = (short)(cy1 + edpane.pix_h - 1);

	    if( bitRead( edg.ptn, i, j ) != 0 ) {
		recOn[nron].x = cx1;
		recOn[nron].y = cy1;
		recOn[nron].width  = (unsigned short)(cx2 - cx1);
		recOn[nron].height = (unsigned short)(cy2 - cy1);
		nron++;
	    }
	    else {
		recOff[nroff].x = cx1;
		recOff[nroff].y = cy1;
		recOff[nroff].width  = (unsigned short)(cx2 - cx1);
		recOff[nroff].height = (unsigned short)(cy2 - cy1);
		nroff++;
	    }
	}
    }
    if( nron )
	XFillRectangles( xl.display, xl.edit_winID, xl.borderGC, recOn, nron );
    if( nroff )
	XFillRectangles( xl.display, xl.edit_winID, xl.backGC,   recOff, nroff );
}


/*
 * contents : draw a lattice inside of the editting pane
 */

static void
DrawBorderEdPn( int x1, int y1, int x2, int y2 )
{
    short	cx1, cy1;
    short	cx2, cy2;
    int		ndseg;
    int		wk, i;
    XSegment	dseg[MAXPTNSIZE*2];

    if (xl.display == NULL ){
	    return;
    }

    if( x2 < x1 ) {
	wk = x2;
	x2 = x1;
	x1 = wk;
    }
    if( y2 < y1 ) {
	wk = y2;
	y2 = y1;
	y1 = wk;
    }

    ndseg = 0;

    cx1 = (short)(edpane.pix_w * x1);
    cx2 = (short)(edpane.pix_w * x2 + edpane.pix_w - 1);
    for( i=y1  ;  i <= y2  ;  i++ ) {
	cy1 = (short)(edpane.pix_h * i + edpane.pix_h - 1);
	dseg[ndseg].x1 = cx1;
	dseg[ndseg].y1 = cy1;
	dseg[ndseg].x2 = cx2;
	dseg[ndseg].y2 = cy1;
	ndseg++;
    }
    cy1 = (short)(edpane.pix_h * y1);
    cy2 = (short)(edpane.pix_h * y2 + edpane.pix_h - 1);
    for( i=x1  ;  i <= x2  ;   i++ ) {
	cx1 = (short)(edpane.pix_w * i + edpane.pix_w - 1);
	dseg[ndseg].x1 = cx1;
	dseg[ndseg].y1 = cy1;
	dseg[ndseg].x2 = cx1;
	dseg[ndseg].y2 = cy2;
	ndseg++;
    }

    if( ndseg ) {
	XDrawSegments( xl.display,  xl.edit_winID,  xl.dashGC,  dseg, ndseg);
    }
}


/*
 * contents : draw a dot
 */

static void
DrawPointEdPn( int x, int y, int mode )
{
    int		x1, y1;
    int		x2, y2;

    if (xl.display == NULL ){
	    return;
    }

    x1 = edpane.pix_w * x;
    y1 = edpane.pix_h * y;
    x2 = x1 + edpane.pix_w - 1;
    y2 = y1 + edpane.pix_h - 1;

    if( mode != 0 ) {
	XFillRectangle( xl.display, xl.edit_winID, xl.borderGC,
			x1, y1, x2-x1, y2-y1 );
    } else {
	XFillRectangle( xl.display, xl.edit_winID, xl.backGC,
			x1, y1, x2-x1, y2-y1 );
    }
}



/*
 * contents : draw a current character pattern to the display pane
 */

static void
DrawDpPn(void)
{
	if (xl.display == NULL ){
		return;
	}

	XPutImage( xl.display,  xl.disp_winID,  xl.borderGC,  xl.dispImage,
	      0, 0, 0, 0, edg.width, edg.height );
	if( xl.list_winID ) {
		XPutImage( xl.display,  xl.list_winID,  xl.borderGC,
			xl.dispImage, 0, 0, 0, 0, edg.width, edg.height );
	}
}



/*
 * contents : draw a dot
 */

static void
DrawPointDpPn( int x, int y, int mode )
{

    if (xl.display == NULL ){
	    return;
    }
    if( mode != 0 ){
	XDrawPoint( xl.display,  xl.disp_winID,  xl.borderGC, x,  y );
	if (xl.list_winID)
	    XDrawPoint( xl.display,  xl.list_winID,  xl.borderGC, x,  y );
    } else {
	XDrawPoint( xl.display,  xl.disp_winID,  xl.backGC,   x,  y );
	if (xl.list_winID)
	    XDrawPoint( xl.display,  xl.list_winID,  xl.backGC,   x,  y );
    }
}



/****************************************************************
 * draw patterns by mouse 					*
 ****************************************************************/


/*
 * contents : draw a dot
 */

static void
musPoint( int evtype, int px, int py )
{
    switch( evtype ) {
    case MotionNotify:
	if( (em.src1_px == px) && (em.src1_py == py) )
	    return;
	break;
    case ButtonPress:
	em.src1_px = px;
	em.src1_py = py;
	break;
    default:
	return;
    }

    if( edpane.color == ON )
	bitSet( edg.ptn, px, py );
    else
	bitReset( edg.ptn, px, py );
    edg.flag = ON;
    DrawPointEdPn( px, py, edpane.color );
    DrawPointDpPn( px, py, edpane.color );
}


/*
 * contents : draw a line
 */

static void
musLine( int evtype, int px, int py )
{
    int		r1_x, r1_y;
    int		r2_x, r2_y;
    int		rx,   ry;
    int		harf_pix_w;
    int		harf_pix_h;

    harf_pix_w = edpane.pix_w / 2;
    harf_pix_h = edpane.pix_h / 2;
    r1_x = em.src1_px * edpane.pix_w + harf_pix_w;
    r1_y = em.src1_py * edpane.pix_h + harf_pix_h;
    r2_x = em.src2_px * edpane.pix_w + harf_pix_w;
    r2_y = em.src2_py * edpane.pix_h + harf_pix_h;
    rx = px * edpane.pix_w + harf_pix_w;
    ry = py * edpane.pix_h + harf_pix_h;

    switch( evtype ) {
    case MotionNotify:
	if( (em.src2_px == px) && (em.src2_py == py) )
	    return;
	rubLine( r1_x, r1_y, r2_x, r2_y );
	rubLine( r1_x, r1_y, rx, ry );
	em.src2_px = px;
	em.src2_py = py;
	return;
    case ButtonPress:
	em.src1_px = em.src2_px = px;
	em.src1_py = em.src2_py = py;
	return;
    default:
	if( (r1_x == r2_x) && (r1_y == r2_y) )
	    return;
	rubLine( r1_x, r1_y, r2_x, r2_y );

	if ((em.src2_px < 0) || (em.src2_py < 0) ||
	    (edg.width <= em.src2_px) || (edg.height <= em.src2_py))
	    return;

	bitDrawLine( edg.ptn, em.src1_px, em.src1_py,
			      em.src2_px, em.src2_py, edpane.color );
	edg.flag = ON;
	DrawRectEdPn( em.src1_px, em.src1_py, em.src2_px, em.src2_py );
	DrawDpPn();
    }
}



/*
 * contents : draw a circle
 */

static void
musCircle( int evtype, int px, int py )
{
    int		r1_x, r1_y;
    int		r2_x, r2_y;
    int		rx,   ry;
    int		harf_pix_w;
    int		harf_pix_h;
    extern int	bitDrawCircle();

    harf_pix_w = edpane.pix_w / 2;
    harf_pix_h = edpane.pix_h / 2;

    r1_x = em.src1_px * edpane.pix_w + harf_pix_w;
    r1_y = em.src1_py * edpane.pix_h + harf_pix_h;
    r2_x = em.src2_px * edpane.pix_w + harf_pix_w;
    r2_y = em.src2_py * edpane.pix_h + harf_pix_h;
    rx = px * edpane.pix_w + harf_pix_w;
    ry = py * edpane.pix_h + harf_pix_h;

    switch( evtype ) {
    case MotionNotify:
	if( (em.src2_px == px) && (em.src2_py == py) ) {
	    return;
	}
	rubCircle( r1_x, r1_y, r2_x, r2_y );
	rubCircle( r1_x, r1_y, rx, ry );
	em.src2_px = px;
	em.src2_py = py;
	return;
    case ButtonPress:
	em.src1_px = em.src2_px = px;
	em.src1_py = em.src2_py = py;
	return;
    default:
	if( (em.src1_px == px) && (em.src1_py == py) ) {
	    return;
	}
	rubCircle( r1_x, r1_y, r2_x, r2_y );

	if( (px < 0) || (py < 0) || (edg.width <= px) || (edg.height <= py) ) {
	    return;
	}
	bitDrawCircle( edg.ptn, em.src1_px, em.src1_py, px, py, edpane.color);

	edg.flag = ON;
	DrawRectEdPn( 0, 0, edg.width -1, edg.height -1 );
	DrawDpPn();
    }
}


/*
 * contents : draw a rectangle
 */

static void
musRect( int proc, int evtype, int px, int py )
{
    int		r1_x, r1_y;
    int		r2_x, r2_y;
    int		rx,   ry;
    int		lux, luy;
    int		width, height;
    int		dpx, dpy, dp;

    if( (proc == PROC_ROLL) && (evtype != ButtonPress) ) {
	dpx = px - em.src1_px;
	dpy = py - em.src1_py;
	dp = (abs(dpx) > abs(dpy) ) ? abs(dpx) : abs(dpy);

	if( dpx != 0 )
	    px = em.src1_px + dpx/abs(dpx) * dp;
	if( dpy != 0 )
	    py = em.src1_py + dpy/abs(dpy) * dp;
    }

    r1_x = em.src1_px * edpane.pix_w + edpane.pix_w / 2;
    r1_y = em.src1_py * edpane.pix_h + edpane.pix_h / 2;
    r2_x = em.src2_px * edpane.pix_w + edpane.pix_w / 2;
    r2_y = em.src2_py * edpane.pix_h + edpane.pix_h / 2;
    rx = px * edpane.pix_w + edpane.pix_w / 2;
    ry = py * edpane.pix_h + edpane.pix_h / 2;

    switch( evtype ) {
    case MotionNotify:
	if( (em.src2_px == px) && (em.src2_py == py) )
	    return;
	rubBand( r1_x, r1_y, r2_x, r2_y );
	rubBand( r1_x, r1_y, rx, ry );
	em.src2_px = px;
	em.src2_py = py;
	return;
    case ButtonPress:
	resetEditMode( RES_SLCT );
	em.src1_px = em.src2_px = px;
	em.src1_py = em.src2_py = py;
	return;
    default:
	resetEditMode( RES_MSG | RES_PROC );
	rubBand( r1_x,  r1_y,  r2_x,  r2_y );
	if( (r1_x == r2_x) || (r1_y == r2_y) )
	    return;
	if( (em.src2_px < 0) || (em.src2_py < 0) ||
	    (edg.width <= em.src2_px) || (edg.height <= em.src2_py)	)
	    return;

	lux = (em.src1_px < em.src2_px ) ?  em.src1_px   :   em.src2_px;
	luy = (em.src1_py < em.src2_py ) ?  em.src1_py   :   em.src2_py;
	width  = abs( em.src1_px - em.src2_px ) + 1;
	height = abs( em.src1_py - em.src2_py ) + 1;

	bitDrawRect(edg.ptn, lux, luy, width, height, edpane.color);

	edg.flag = ON;
	DrawRectEdPn( em.src1_px, em.src1_py, px, py );
	DrawDpPn();
    }
}


/*
 * contents : edit inside of the specified area (clear, reverse, rotate,etc.)
 */

static void
musRegionProc( int proc, int evtype, int px, int py )
{
    int		rx,   ry;
    int		dpx, dpy, dp;
    extern void	SelectSet();

    if( (proc == PROC_ROLL) && (evtype != ButtonPress) ) {
	dpx = px - em.src1_px;
	dpy = py - em.src1_py;
	dp = (abs(dpx) > abs(dpy) ) ? abs(dpx) : abs(dpy);

	if( dpx != 0 )
	    px = em.src1_px + dpx/abs(dpx) * dp;
	if( dpy != 0 )
	    py = em.src1_py + dpy/abs(dpy) * dp;
    }

    r1_x = em.src1_px * edpane.pix_w + edpane.pix_w / 2;
    r1_y = em.src1_py * edpane.pix_h + edpane.pix_h / 2;
    r2_x = em.src2_px * edpane.pix_w + edpane.pix_w / 2;
    r2_y = em.src2_py * edpane.pix_h + edpane.pix_h / 2;
    rx = px * edpane.pix_w + edpane.pix_w / 2;
    ry = py * edpane.pix_h + edpane.pix_h / 2;

    switch( evtype ) {
    case MotionNotify:
	if( (em.src2_px == px) && (em.src2_py == py) )
	    return;
	rubBand( r1_x, r1_y, r2_x, r2_y );
	rubBand( r1_x, r1_y, rx, ry );
	em.src2_px = px;
	em.src2_py = py;
	return;
    case ButtonPress:
	if (select_x || select_y || select_w || select_h) {
	    rubBand( r1_x, r1_y, r2_x, r2_y );
	}
	resetEditMode( RES_SLCT );
	em.src1_px = em.src2_px = px;
	em.src1_py = em.src2_py = py;
	return;
    default:
	resetEditMode( RES_MSG | RES_PROC );
	select_x = select_y = select_w  = select_h = 0;
	if( (r1_x == r2_x) || (r1_y == r2_y) ) {
	    rubBand( r1_x,  r1_y,  r2_x,  r2_y );
	    return;
	}
	if( (em.src2_px < 0) || (em.src2_py < 0) ||
	    (edg.width <= em.src2_px) || (edg.height <= em.src2_py)	) {
	    rubBand( r1_x,  r1_y,  r2_x,  r2_y );
	    return;
	}

	select_x = (em.src1_px < em.src2_px ) ?  em.src1_px   :   em.src2_px;
	select_y = (em.src1_py < em.src2_py ) ?  em.src1_py   :   em.src2_py;
	select_w  = abs( em.src1_px - em.src2_px ) + 1;
	select_h = abs( em.src1_py - em.src2_py ) + 1;
	SelectSet();
    }
}


/*ARGSUSED*/
static void
#ifdef __linux__
musPasteProc(Widget w, XtPointer client_data, XEvent *event, Boolean* continue_to_dispatch)
#else
musPasteProc(Widget w, XtPointer client_data, XEvent *event)
#endif 
{
    static int ox=0, oy=0;
    int rc, tx, ty;
    extern void UndoSet();
    extern Widget wgeBulB_edit;
    extern int	bitDrawPaste();

    switch(event->type) {
    case MotionNotify:
	tx = (event->xmotion.x / edpane.pix_w) * edpane.pix_w + edpane.pix_w / 2;
	ty = (event->xmotion.y / edpane.pix_h) * edpane.pix_h + edpane.pix_h / 2;
	if (tx == ox && ty == oy)
	    return;
	if(ox) {
	    rubBand(ox, oy, ox + cut_w * edpane.pix_w, oy + cut_h * edpane.pix_h);
	}
	ox = tx;
	oy = ty;
	rubBand(ox, oy, ox + cut_w * edpane.pix_w, oy + cut_h * edpane.pix_h);
	break;
    case ButtonRelease:
	XtRemoveEventHandler(wgeBulB_edit,
				ButtonReleaseMask|PointerMotionMask,
				    False, (XtEventHandler)musPasteProc, NULL );
	rubBand(ox, oy, ox + cut_w * edpane.pix_w, oy + cut_h * edpane.pix_h);
	ox = 0;
	oy = 0;
        bitPtnCopy( em.rsv_ptn,  edg.ptn );

	tx = event->xbutton.x / edpane.pix_w;
	ty = event->xbutton.y / edpane.pix_h;
	rc = bitDrawPaste(edg.ptn, tx, ty);
	if( rc == -1 )
	     return;
	edg.flag = ON;
	em.rsv_f = ON;
	DrawRectEdPn( 0, 0,  edg.width - 1,  edg.height - 1 );
	DrawDpPn();
	UndoSet();
	break;
    }
}



static void
rubLine( int x1, int y1, int x2, int y2  )
{
    if( x1==x2 && y1==y2 ) return;

    XDrawLine( xl.display, xl.edit_winID, xl.rubGC,    x1,y1, x2,y2 );
}



static void
rubBand( int x1, int y1, int x2, int y2  )
{
    if( x1==x2 && y1==y2 )
	return;

    XDrawLine( xl.display, xl.edit_winID, xl.rubGC, x1, y1, x2, y1 );
    XDrawLine( xl.display, xl.edit_winID, xl.rubGC, x1, y1, x1, y2 );
    XDrawLine( xl.display, xl.edit_winID, xl.rubGC, x2, y1, x2, y2 );
    XDrawLine( xl.display, xl.edit_winID, xl.rubGC, x1, y2, x2, y2 );
}



static void
rubCircle( int ox, int oy, int rx, int ry )
{
    unsigned int	r;
    int			x, y;

    if( ox==rx && oy==ry ) return;

    x = rx - ox;
    y = ry - oy;
    r = (unsigned int)sqrt( (double)(x*x + y*y) );
    if ( r == 0 ) return;

    x = ox - (int)r;
    y = oy - (int)r;
    XDrawArc( xl.display, xl.edit_winID, xl.rubGC,
	     x, y, 2*r-1, 2*r-1,
	     0, 360*64
	     );
}



static void
resetEditMode( unsigned int flag )
{
    int		r1_x, r1_y;
    int		r2_x, r2_y;

    if( flag & RES_MSG )
	UpdateMessage( "" );

    if( flag & RES_PROC )
	em.proc = edpane.obj;

    if( (flag & RES_SLCT) && (em.slct_f) ) {
	r1_x = em.src1_px * edpane.pix_w + edpane.pix_w / 2;
	r1_y = em.src1_py * edpane.pix_h + edpane.pix_h / 2;
	r2_x = em.src2_px * edpane.pix_w + edpane.pix_w / 2;
	r2_y = em.src2_py * edpane.pix_h + edpane.pix_h / 2;
	rubBand( r1_x, r1_y, r2_x, r2_y );
	em.slct_f = OFF;
    }

    if( flag & RES_RSV )
	em.rsv_f = OFF;
}


/****************************************************************
 * copy character pattern					*
 ***************************************************************/

/*
 * contents : copy or overlay the new character pattern to the current pattern
 */

/*ARGSUSED*/
static void
copyPatterns(
    FalFontData *fdata,
    int s1_code,
    int s2_code,
    int d1_code,
    int proc)
{
    int		ret;
    int		d1_ncode, d2_ncode;
    int		code_d;
    int		code_disp;
    char	err[128];
    int		i_s, i_d;
    int		i;
    char	grc_d;
    char	**ptn;
    char	ptn2[MAXPTNBYTE];
    int		num;
    extern int	last_code;

    ret = copySNF(s1_code, s2_code, &ptn, &num, err);
    if( ret == -1 ) {
	Error_message2((Widget)NULL, err);
	return;
    }

    if (last_code < (d1_code + num))
	last_code = d1_code + num;

    if( ptnSense(edg.code) == 1 )
	ptnAdd( edg.code, edg.ptn );

    d1_ncode = codeToNo( d1_code );

    for (i_d=d1_ncode, i_s=0; i_s < num; i_d++) {
	if ( codeCheck( noToCode( i_d)))
	    continue;
	bitPtnClear( ptn2 );
	code_d = noToCode( i_d );
	grc_d = ptnGet( code_d,  ptn2 );

	if (grc_d == 0) {
	    if( proc == CPY_OVERLAY ) {
		for( i=0  ;  i < edg.height*((edg.width+7)/8)  ;  i++ ) {
		    ptn[i_s][i] |= ptn2[i];
		}
	    }
	    ptnAdd( code_d, ptn[i_s] );
	    edg.flag = ON;
	} else {
	    if( ptnAdd( code_d, ptn[i_s] ) != 1 ) {
		UpdateMessage( resource.me_non_memory );
		break;
	    }
	    edlist.nptn++;
	    edg.flag = ON;
	}
	i_s ++;
    }
    d2_ncode = i_d - 1;

    code_disp = 0;
    for( i=d1_ncode   ; i <= d2_ncode; i++ ) {
	if ( codeCheck( noToCode(i) ) )
	    continue;
	if( ptnSense( noToCode( i ) ) == 1 ) {
	    code_disp = noToCode( i );
	    break;
	}
    }
    freeSNF(ptn, num);

    if( (code_disp == 0) && (efctPtnNum() > 0))
	code_disp = noToCode( ptnSqToNo(edlist.sqstart) );

    chgEdCode( code_disp, ON );

    resetEditMode( (unsigned int) (RES_MSG | RES_PROC | RES_SLCT | RES_RSV) );
}
