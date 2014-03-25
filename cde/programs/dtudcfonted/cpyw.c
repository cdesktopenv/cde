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
/* cpyw.c 1.9 - Fujitsu source for CDEnext    96/10/30 13:31:40      */
/* $XConsortium: cpyw.c /main/7 1996/11/08 01:53:38 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#include <stdio.h>

#include <X11/Intrinsic.h>

#include "xoakufont.h"
#include "util.h"


extern Resource resource ;
extern Widget	cpyDialog;

/**
 **  create and display the "User defined character ecitor :Copy" window
 ** ==================================================================
 **/

void	PopdownCpyPtn();
void	DoCpyProc();
void	CpyPtnCB();
void	CpyCB();

/* static */ TextField SrcTf;
/* static */ TextField DestTf;


/**
 **  contents : initialize the "Copy" window 
 ** --------------------
 **
 **
 **/

void
InitCpyPtn()
{
	/* initialize font infomation */
}


/**
 **  contents : create and pop up the "Copy" window 
 ** ----------------------------------
 **
 **/

void
CpyPtnCB()
{
    extern void CpyPtnXLFD();
    CpyPtnXLFD();
}


/**
 **  contents : pop down the "Copy" window 
 ** ----------------------
 **/

void
PopdownCpyPtn()
{
    ForcePopdownDialog(cpyDialog);
}


/**  contents : 
 **  get the character strings of source code area from the input field 
 ** --------------------------------------
 **/

String
CpySrcCodeTfValue()
{
    return(GetTextFieldValue(&SrcTf));
}


/**  contents : 
 **  get the character strings of destination code area from input field 
 ** --------------------------------------
 **/

String
CpyDestCodeTfValue()
{
    return(GetTextFieldValue(&DestTf));
}


/**
 **  contents : pop up the copy confirmation window 
 ** --------------------------------
 **/

void
PopupCpyNotice( message )
String message;
{
    char bmsg[512] = "";

    static NoticeButton buttons[] = {
	NBTNARGS( DoCpyProc, NULL, NULL, True, False ),
	NBTNARGS( NULL, NULL, NULL, True, False ),
    };
    static NButton BTN = NBUTTONS( buttons );

    strcpy( bmsg, resource.mn_cpy_lost );
    strcat( bmsg, message );
    SetItemLabel(&BTN, 0, resource.l_ok);
    SetItemLabel(&BTN, 1, resource.l_cancel);
    PopupNotice( cpyPtnW, bmsg, D_WARNING, &BTN, False, resource.l_warning_title);
}


/**
 **  contents : "Copy" button callback 
 ** ------------------------
 **  execute the copy processing 
 **/

/*ARGSUSED*/
void
CpyCB(w, cdata)
Widget w;
XtPointer cdata;
{
    extern Boolean BeforeCpyCheck();
    if (BeforeCpyCheck(cdata)){
	PopdownCpyPtn();
	DoCpyProc();
    }
}



/**
 **  "Add/Delete" window 
 ** ===================================================================
 **/

/**
 **  contents : pop down the "Add/Delete" window 
 ** ------------------------------
 **/

static void
PopdownMngPtn()
{
    PopdownDialog(mngPtnW);
}

/**
 **  contents : "Add" button callback 
 ** ------------------------
 **/

static void
AddCB()
{
    int s_code, e_code;
    extern Boolean BeforeMngCheck();
    extern void DoAddProc();

    if (BeforeMngCheck(&s_code, &e_code)){
	DoAddProc(s_code, e_code);
	PopdownMngPtn();
    }
}

/**
 **  contents : "Delete" button callback 
 ** ------------------------
 **/

static void
DelCB()
{
    int s_code, e_code;
    extern void DoDelProc();
    extern Boolean BeforeMngCheck();

    if (BeforeMngCheck(&s_code, &e_code)){
	DoDelProc(s_code, e_code);
    }
}

static ButtonItems add_btn[] = {
    BTNARGS( (void(*)(struct _WidgetRec*,void*,void*)) AddCB, NULL, NULL, True, True ),
    BTNARGS( (void(*)(struct _WidgetRec*,void*,void*)) DelCB, NULL, NULL, True, False ),
    BTNARGS( (void(*)(struct _WidgetRec*,void*,void*)) PopdownMngPtn, NULL, NULL, True, False ),
};
static Button AddBTN = BUTTONS( add_btn );

static TextField MngCodeTf;

/**
 **  contents : create and pop up the "Add/Delete" window 
 ** ------------------------------------
 **/

void
MngPtnCB()
{
    if (! mngPtnW){
	Widget	rowcol;

	SetItemLabel(&AddBTN, 0, resource.l_add);
	SetItemLabel(&AddBTN, 1, resource.l_delete);
	SetItemLabel(&AddBTN, 2, resource.l_cancel);
	rowcol =
	CreateDialogAndButtons( toplevel, resource.l_add_title,

	    NULL, &AddBTN, XtNumber(add_btn), &mngPtnW);

	CreateTextField(rowcol, "wgkTxtF",
	    resource.l_manage_code, &MngCodeTf, 4);
    }
    PopupDialog(mngPtnW);
}




/**
 **  contents : get the character strings of code for add or delete from the input field 
 ** --------------------------------------------
 **/

String
MngCodeTfValue()
{
    return(GetTextFieldValue(&MngCodeTf));
}



/**
 **  the delete confirmation window 
 ** ===================================================================
 **/

static Widget	CreateDelNotice();
static void	drawDelPtn();
static void	drawDelCode();
static void	CBdnOblB_del();
static void	EHdnBulB_disp();
void	EHStaT_disp();			/* add 1995.09.20 */

static	Widget	dnStaT_code[D_MAX], dnBulB_disp[D_MAX], dnPopW;

/**
 **  contents : pop up the delete confirmation window 
 ** --------------------------
 **/

void
PopupDelNotice( owner )
Widget owner;
{
    int i;
     Widget delNoticeW = NULL;

    delNoticeW = CreateDelNotice(owner);
    XtVaSetValues(XtParent(delNoticeW), XmNmappedWhenManaged, False, NULL);
    XtManageChild(delNoticeW);

    for ( i=0 ; i < dn.disp_num ; i++ ) {
	/* moved from paneDelNotice body */
	dl[i].disp_winID = XtWindow( dnBulB_disp[i] );

	drawDelCode(i);
	drawDelPtn(i);
    }
    XtVaSetValues(XtParent(delNoticeW), XmNmappedWhenManaged, True, NULL);

    XBell(XtDisplayOfObject(delNoticeW), 0);
}


/**
 **  contents : clear the image of deleted character 
 ** ----------------------------------------------
 **/

static void
dstrypaneDelNotice()
{
    int	i;

    for ( i=0; i<dn.disp_num; i++ ){
	XFree((char *) dl[i].dispImage);
    }
}


/**
 **  contents : "OK" button callback 
 ** ----------------------------
 **/

static void
DelOkCB()
{
    CBdnOblB_del();
    PopdownMngPtn();
}


/**
 **  contents : callback of the delete list 
 ** ------------------------------------
 **/

static void
delScProc( value )
int value;
{
    int		i;
    extern int RelToAbsSq();

    dn.sq_top = RelToAbsSq( dn.sq_start, value - dn.sq_start);
    for ( i=0 ; i < dn.disp_num ; i++ ) {
	drawDelCode(i);
	drawDelPtn(i);
    }
}


/**
 **  contents : create the delete confirmation window 
 ** --------------------------
 **/

static Widget
CreateDelNotice(owner)
Widget owner;
{
    Widget form, listBase, itemform[D_MAX];
    char buf[64];
    int	i, val, min, max;
    int c ;
    extern int AbsSqToRel();
    extern void AddDestroyProc();

    static NoticeButton buttons[] = {
	NBTNARGS( (void(*)(struct _WidgetRec*,void*,void*)) DelOkCB, NULL, NULL, True, False ),
	NBTNARGS( NULL, NULL, NULL, True, False ),
    };
    static NButton BTN = NBUTTONS( buttons );
    c = AbsSqToRel( dn.sq_start, dn.sq_end) + 1;
    if (c == 1) {
	sprintf(buf, "%s(%d %s)", resource.mg_start_del_s,
					c, resource.mg_start_del_cs );
    } else {
	sprintf(buf, "%s(%d %s)", resource.mg_start_del_m,
					c, resource.mg_start_del_cm );
    }
    SetItemLabel(&BTN, 0, resource.l_ok);
    SetItemLabel(&BTN, 1, resource.l_cancel);
    form = CreateTemplateDialog( owner, buf, D_QUESTION, &BTN,
		resource.l_question_title, &dnPopW);
    AddDestroyProc(dnPopW, dstrypaneDelNotice);

    if ( dn.disp_num < (AbsSqToRel( dn.sq_start, dn.sq_end) + 1)) {

	val = AbsSqToRel( dn.sq_start, dn.sq_top) + dn.sq_start;
	min = dn.sq_start;
	max = AbsSqToRel( dn.sq_start, dn.sq_end) + dn.sq_start +1;
	listBase = CreateScrollBase( form, "listBase",
				min, max, val, dn.disp_num, delScProc );
    }
    else{
	listBase =
	CreateRowColumn( form, "listBase", L_VERTICAL, 4,2,2 );
    }

    for(i=0; i<dn.disp_num; i++) {
	/* create form for the codes and fonts to be deleted */
	itemform[i] = CreateForm( listBase, "listBase" );

	/* create widgets for the codes to be deleted in the list */
	dnStaT_code[i] = CreateLabel( itemform[i], "label", "    " );
	AddTopAttachForm( dnStaT_code[i], 0 );
	AddLeftAttachForm( dnStaT_code[i], 0 );
	AddBottomAttachForm( dnStaT_code[i], 0 );

	/* create widgets for the image of the codes to be deleted in the list */
	dnBulB_disp[i] = CreateDrawingArea( itemform[i], "image",
			edg.width, edg.height, EHdnBulB_disp, i);
	AddLeftAttachWidget( dnBulB_disp[i], dnStaT_code[i], 0 );

	/* */
	dl[i].dispImage  = XCreateImage( xl.display,
		DefaultVisual( xl.display, DefaultScreen(xl.display) ),
		1, XYBitmap, 0, dl[i].ptn, dn.ptn_w, dn.ptn_h, 8, 0);
	dl[i].dispImage->bitmap_bit_order = MSBFirst;
	dl[i].dispImage->byte_order       = MSBFirst;

    }
    XtManageChildren( itemform, dn.disp_num );
    return(dnPopW);
}


/**
 **  contents : delete characters from editor 
 ** -----------------------------------------
 **  
 **/

static void
CBdnOblB_del()
{
    int		code;	/* code number for check */
    int		ncode;	/* sequential number in the codeset system area */
    int		sq_now;	/* sequential number in the editor */
    char	mode;	/* notify the modification of the edit list */
    extern void chgEdCode();

    XtSetSensitive( dnPopW, FALSE );
    mode = OFF;
    for( sq_now=dn.sq_end ; sq_now >= dn.sq_start ; sq_now-- ) {
	ncode = ptnSqToNo(sq_now);
	if ( codeCheck( noToCode(ncode)))
	    continue;
	if (ptnSense( noToCode(ncode) ) == 1) {
	    ptnDel( noToCode(ncode) );
	    edlist.nptn--;
	    edg.flag = ON;
	    mode = ON;		
	}
    }

    /* update the current character */

    sq_now++;
    for ( ; sq_now < (edlist.sqstart + edlist.nptn); sq_now++) {
	ncode = ptnSqToNo(sq_now);
        if (( ncode >= 0) && ( codeCheck( noToCode( ncode)) == 0))
            break;
    }
    if(sq_now > (edlist.sqstart + edlist.nptn -1)) {
        sq_now--;
        for ( ; sq_now >= edlist.sqstart; sq_now--) {
	    ncode = ptnSqToNo(sq_now);
            if (( ncode >= 0) && ( codeCheck( noToCode( ncode)) == 0))
	        break;
        }
    } 

    /* check the existence of the current character */
    if( (sq_now != -1) && ((ncode = ptnSqToNo(sq_now)) != -1) ){
	code = noToCode( ncode );
    } else {
	/* There is no characters on the editor, dtudcfonted does not 
	* display any character on the editing pane and forbid users
	* to input.
	* It can realize to put 0 to the first parameter of chgEdCode().
	*/
	code = 0;
    }
    if(codeCheck( code )) {
	code = 0;
    }

    /* */
    if( ptnSense( edg.code ) == 1 ) {
	ptnAdd( edg.code, edg.ptn );
    }

    /* renew the current character */
    if (mode == ON) {
	chgEdCode( code, mode );
    }
    XtSetSensitive( dnPopW,  TRUE );
}


/**
 **  contents : display the image of the character to be deleted 
 ** ------------------------
 **
 **/

static void
drawDelPtn( i )
int	i;	/* window number */
{
    if ( RelToAbsSq( dn.sq_top, i) <= dn.sq_end) {
	ptnGet( noToCode(ptnSqToNo(RelToAbsSq( dn.sq_top, i))),
								dl[i].ptn );
	XPutImage(xl.display, dl[i].disp_winID, xl.borderGC,
			dl[i].dispImage, 0,0,0,0, dn.ptn_w, dn.ptn_h );
    } else {
	return;
    }
}


/**
 **  contents : display the code of the character to be deleted 
 ** ----------------------
 **
 **/

static void
drawDelCode( i )
int	i;	/* window number */
{
    char	str[20];

    if ( RelToAbsSq( dn.sq_top, i) <= dn.sq_end) {
	sprintf(str, "%4x ", noToCode(ptnSqToNo(RelToAbsSq( dn.sq_top, i))));
	SetLabelString(dnStaT_code[i], str);
    } else {
	return;
    }
}


/**
 **  contents : re-display the code of the character to be deleted 
 ** --------------------------------------------------------------
 **/

/*ARGSUSED*/
static void
EHdnBulB_disp( widget, i )
Widget		widget;
int		i;			/* widget that have some ivent */
{
    drawDelPtn( i );
}



/**
 **  notify error message
 ** ===================================================================
 **
 **/

/**
 **  contents : inner functions of DispMngErrorMessage()
 ** ----------------------------------
 **/

static void
DispErrorMessage( owner, title, msg )
Widget owner;
String title;
String msg;
{
    static NoticeButton buttons[] = {
	NBTNARGS( NULL, NULL, NULL, True, False ),
    };
    static NButton BTN = NBUTTONS( buttons );

    SetItemLabel(&BTN, 0, resource.l_ok);
    PopupNotice( owner, msg, D_ERROR, &BTN, False, title );
}


/**
 **  contents : display "Add" or "Delete" error 
 ** --------------------------------
 **/

void
DispMngErrorMessage( msg )
String msg;
{
    DispErrorMessage( mngPtnW, resource.l_error_title, msg );
}


/**
 **  contents : display "Copy" error 
 ** --------------------------
 **/

void
DispCpyErrorMessage( msg )
String msg;
{
    DispErrorMessage( cpyPtnW, resource.l_error_title, msg );
}
