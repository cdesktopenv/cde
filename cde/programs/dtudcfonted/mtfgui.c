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
/* mtfgui.c 1.21 - Fujitsu source for CDEnext    96/10/30 13:13:46      */
/* $XConsortium: mtfgui.c /main/11 1996/11/08 01:55:01 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#include <stdio.h>
#include <locale.h>
#include <X11/Intrinsic.h>
#include <Xm/MessageB.h>

#include "xoakufont.h"
#include "util.h"

void CBeOblB_aEnd();
void EHeStaT_list();
void CBeScro();
void EHeBulB_eMEv();
void EHeBulB_eExp();
void EHeBulB_dExp();
static void EHStaT_disp();

extern Resource resource;

extern int efctPtnNum();
extern char *char_set();

extern char *fullpath;
extern FalFontData fullFontData;

extern FalCodeRegion CodeArea[16];

/********************************************************************
 structure of widgets
 ********************************************************************/
extern	Widget	toplevel;
	Widget	editPopW,
		wgeScro,
		wgeBulB_edit;
static	Widget	wgeStaT_form[EDLIST_MAX],
		wgeStaT_disp[EDLIST_MAX],
		wgeStaT_list[EDLIST_MAX],
		wgeBulB_disp;

/**********************************************************************
 display windows
 **********************************************************************/

/*
 * contents : displays the "User Defined Charactrer editor" window
 */

static Widget CreateEditPtn();
void OpenCB();
void MngPtnCB();
void CpyPtnCB();
void CBeRecB_color();
void CBeRecB_obj();
void CBeOblB_aAdd();
void CBeOblB_rCmd();
void CBeOblB_rCmdp();
void CBeOblB_rCan();


/**
 **  contents : manage the codes list 
 ** ===================================================================
 **/

static void
XlfdCB()
{
  Widget	dialog;
  Arg		args[5];
  char		mess[1024];
  int		n;
  XmString	cs, cs1, cs2;

  sprintf(mess, "%s : %s", resource.file_name, fullpath);
  cs = XmStringCreateLocalized(mess);
  cs1 = XmStringSeparatorCreate();
  cs2 = XmStringConcat(cs, cs1);
  XmStringFree(cs);
  XmStringFree(cs1);
  sprintf(mess, "%s : %s", resource.xlfd_name, fullFontData.xlfdname);
  cs1 = XmStringCreateLocalized(mess);
  cs = XmStringConcat(cs2, cs1);
  XmStringFree(cs1);
  XmStringFree(cs2);
  n = 0;
  XtSetArg (args[n], XmNtitle, resource.l_xlfd_title); n++;
  XtSetArg (args[n], XmNmessageString, cs); n++;
  XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
  dialog = XmCreateInformationDialog (toplevel, "Xlfd_name", args, n);
  XtUnmanageChild (XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild (XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtManageChild (dialog);
  XmStringFree(cs);
}

static void
CodeAreaCB()
{
    char	mess[256];
    char	tmp[16];
    Widget	dialog;
    Arg		args[5];
    int		n;
    XmString	cs;

    sprintf(mess, "%s : ", resource.codearea);
    switch (fullFontData.cd_set) {
	case FAL_FONT_CS0:
	    strcat(mess, "CS0:");
	    break;
	case FAL_FONT_CS1:
	    strcat(mess, "CS1:");
	    break;
	case FAL_FONT_CS2:
	    strcat(mess, "CS2:");
	    break;
	case FAL_FONT_CS3:
	    strcat(mess, "CS3:");
	    break;
    }

    for (n=0; CodeArea[n].start != -1; n++) {
	sprintf(tmp, " %X - %X ", CodeArea[n].start, CodeArea[n].end);
	strcat(mess, tmp);
    }
    cs = XmStringCreateLocalized(mess);
    n = 0;
    XtSetArg (args[n], XmNtitle, resource.l_codearea_title); n++;
    XtSetArg (args[n], XmNmessageString, cs); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); n++;
    dialog = XmCreateInformationDialog (toplevel, "UDCarea", args, n);
    XtUnmanageChild (XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild (XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    XtManageChild (dialog);
    XmStringFree(cs);
}

void
ListSetLabelStr(i, str)
int i;
String str;
{
    SetLabelString( wgeStaT_list[i], str );
}

void
ListSelectItem(i)
int i;
{
    XtVaSetValues( wgeStaT_list[i],
	XmNbackground, (XtArgVal) resource.foreground,
	XmNforeground, (XtArgVal) resource.background,
	NULL);
    xl.list_winID = XtWindow( wgeStaT_disp[i] ) ;
}

void
ListUnselectItem(i)
int i;
{
    XtVaSetValues( wgeStaT_list[i],
	XmNbackground, (XtArgVal) resource.background,
	XmNforeground, (XtArgVal) resource.foreground,
	NULL);
    xl.list_winID = 0 ;
}

void
ListSetGlyphImage( i )
int	i ;
{
	int	code ;

	dl_glyph[i].disp_winID = XtWindow(wgeStaT_disp[i]);
	code = noToCode(ptnSqToNo(sq_disp[i]));
	if (codeCheck(code)) {
		XClearWindow(xl.display, dl_glyph[i].disp_winID);
		return;
	}
	if (code == edg.code && xl.dispImage != NULL) {
		XPutImage(xl.display, dl_glyph[i].disp_winID, xl.borderGC,
				xl.dispImage, 0,0,0,0, edg.width, edg.height );
	} else {
		if (dl_glyph[i].dispImage)
		    XFree((char *)dl_glyph[i].dispImage);
		if (ptnGet(code, dl_glyph[i].ptn))
		    return ;
		dl_glyph[i].dispImage  = XCreateImage( xl.display,
		    DefaultVisual(xl.display, DefaultScreen(xl.display)), 1,
		    XYBitmap, 0, dl_glyph[i].ptn, edg.width, edg.height, 8, 0);
		dl_glyph[i].dispImage->bitmap_bit_order = MSBFirst;
		dl_glyph[i].dispImage->byte_order       = MSBFirst;

		XPutImage(xl.display, dl_glyph[i].disp_winID, xl.borderGC,
			dl_glyph[i].dispImage, 0,0,0,0, edg.width, edg.height);
	}
}

/*ARGSUSED*/
static void
EHStaT_disp( widget, i )
Widget		widget;
int		i;			/* widget that have some ivent */
{
    ListSetGlyphImage( i );
}

/**
 **  contents : create the icon
 ** ===================================================================
 **/


/**
 **  contents : set callback functions for UDC editor window 
 ** ===================================================================
 **/

static void _create_editptn_after();

static Widget FooterMessage;
static Widget codeLabel;

static menuButtonItems menu_btn[] = {
    MENUBTNARGS( 'F' ),
    MENUBTNARGS( 'E' ),
    MENUBTNARGS( 'C' ),
    MENUBTNARGS( 'I' ),
};
static MButton MenuBTN = MBUTTONS( menu_btn );

static ButtonItems file_btn[] = {
    BTNARGS( OpenCB, NULL, 'O', True, False),
    BTNARGS( CBeOblB_aAdd,   NULL, 'S', True, False),
    BTNARGS( NULL,           NULL, NULL, NULL, NULL),
    BTNARGS( CBeOblB_aEnd,   NULL, 'E', True, False),
};
static Button FileBTN = BUTTONS( file_btn );

static ButtonItems edit_btn[] = {
    BTNARGS( CBeOblB_rCmd, PROC_CLS,  'C', True, False),
    BTNARGS( CBeOblB_rCmd, PROC_SET,  'S', True, False),
    BTNARGS( CBeOblB_rCmd, PROC_REV,  'R', True, False),
    BTNARGS( CBeOblB_rCmdp, PROC_CUT,  'U', True, False),
    BTNARGS( CBeOblB_rCmdp, PROC_CPY,  'O', True, False),
    BTNARGS( CBeOblB_rCmdp, PROC_PASTE,'P', True, False),
    BTNARGS( CBeOblB_rCmd, PROC_ROLL, 'A', True, False),
    BTNARGS( CBeOblB_rCmd, PROC_SYMV, 'V', True, False),
    BTNARGS( CBeOblB_rCmd, PROC_SYMH, 'H', True, False),
    BTNARGS( CBeOblB_rCan, NULL,      'N', True, False),
};
static Button EditBTN = BUTTONS( edit_btn );

static ButtonItems char_btn[] = {
    BTNARGS( MngPtnCB, NULL, 'A', True, False ),
    BTNARGS( CpyPtnCB, NULL, 'C', True, False ),
};
static Button CharBTN = BUTTONS( char_btn );

static ButtonItems info_btn[] = {
    BTNARGS( XlfdCB, NULL, 'X', True, False ),
    BTNARGS( CodeAreaCB, NULL, 'C', True, False ),
};
static Button InfoBTN = BUTTONS( info_btn );

static ExclusiveItems draw_ex[] = {
    EXARGS( PROC_POINT,  "pencil",    True, CBeRecB_obj, True  ),
    EXARGS( PROC_LINE,   "line",      True, CBeRecB_obj, False ),
    EXARGS( PROC_RECT,   "rectangle", True, CBeRecB_obj, False ),
    EXARGS( PROC_CIRCLE, "circle",    True, CBeRecB_obj, False ),
    EXARGS( PROC_ERASE,  "erase",     True, CBeRecB_obj, False ),
    EXARGS( PROC_SELECT, "select",    True, CBeRecB_obj, False ),
};
static Exclusive DrawEX = EXCLUSIVE( draw_ex );

static void
Unset()
{
    XtSetSensitive(EditBTN.w[0], False);
    XtSetSensitive(EditBTN.w[1], False);
    XtSetSensitive(EditBTN.w[2], False);
    XtSetSensitive(EditBTN.w[3], False);
    XtSetSensitive(EditBTN.w[4], False);
    XtSetSensitive(EditBTN.w[5], False);
    XtSetSensitive(EditBTN.w[6], False);
    XtSetSensitive(EditBTN.w[7], False);
    XtSetSensitive(EditBTN.w[8], False);
    XtSetSensitive(EditBTN.w[9], False);
}

void
SelectSet()
{
    XtSetSensitive(EditBTN.w[0], True);
    XtSetSensitive(EditBTN.w[1], True);
    XtSetSensitive(EditBTN.w[2], True);
    XtSetSensitive(EditBTN.w[3], True);
    XtSetSensitive(EditBTN.w[4], True);
    XtSetSensitive(EditBTN.w[6], True);
    XtSetSensitive(EditBTN.w[7], True);
    XtSetSensitive(EditBTN.w[8], True);
}

void
SelectUnset()
{
    XtSetSensitive(EditBTN.w[0], False);
    XtSetSensitive(EditBTN.w[1], False);
    XtSetSensitive(EditBTN.w[2], False);
    XtSetSensitive(EditBTN.w[3], False);
    XtSetSensitive(EditBTN.w[4], False);
    XtSetSensitive(EditBTN.w[6], False);
    XtSetSensitive(EditBTN.w[7], False);
    XtSetSensitive(EditBTN.w[8], False);
}

void
CopySet()
{
    XtSetSensitive(EditBTN.w[5], True);
}

void
UndoSet()
{
    XtSetSensitive(EditBTN.w[9], True);
}

void
UndoUnset()
{
    XtSetSensitive(EditBTN.w[9], False);
}

/**
 **  contents : create the UDC editor window 
 ** --------------------------------
 **/

void
PopupEditPtn(owner)
Widget owner;
{
    if (! editPtnW){
	editPtnW = CreateEditPtn(owner);
	_create_editptn_after();
    }
    XtPopup(editPtnW, XtGrabNone);
}

static Widget
CreateEditPtn(owner)
Widget owner;
{
    int		slimax;
    int		i;
    Widget	baseForm, pop, font_menu, edit_menu, char_menu, info_menu;
    Widget	listBase, listFrame, rc, editFrame, figure_w;
    Widget	imageFrame, imageForm, editPane, image;
    extern Widget CreateFrame();
    extern Widget CreatePixButton();

    Arg arg[1];


    SetItemLabel(&MenuBTN, 0, resource.l_font);
    SetItemLabel(&MenuBTN, 1, resource.l_edit);
    SetItemLabel(&MenuBTN, 2, resource.l_manage);
    SetItemLabel(&MenuBTN, 3, resource.l_info);
    baseForm = (Widget)
    CreateMenuBarAndFooterMessageForm( owner, "dtudcfonted",
	&MenuBTN, XtNumber(menu_btn), &pop, &FooterMessage );

    editPopW = pop;

    AddDeleteProc(pop, CBeOblB_aEnd);

    font_menu = GetMenuWidget( &MenuBTN, 0 );
    edit_menu = GetMenuWidget( &MenuBTN, 1 );
    char_menu = GetMenuWidget( &MenuBTN, 2 );
    info_menu = GetMenuWidget( &MenuBTN, 3 );

    SetItemLabel(&FileBTN, 0, resource.l_open_w);
    SetItemLabel(&FileBTN, 1, resource.l_save);
    SetItemLabel(&FileBTN, 3, resource.l_exit);

    SetItemLabel(&EditBTN, 0, resource.l_clear);
    SetItemLabel(&EditBTN, 1, resource.l_set);
    SetItemLabel(&EditBTN, 2, resource.l_reverse);
    SetItemLabel(&EditBTN, 3, resource.l_cut);
    SetItemLabel(&EditBTN, 4, resource.l_Copy);
    SetItemLabel(&EditBTN, 5, resource.l_paste);
    SetItemLabel(&EditBTN, 6, resource.l_roll);
    SetItemLabel(&EditBTN, 7, resource.l_updown_roll);
    SetItemLabel(&EditBTN, 8, resource.l_leftright_roll);
    SetItemLabel(&EditBTN, 9, resource.l_undo);

    SetItemLabel(&CharBTN, 0, resource.l_manage_w);
    SetItemLabel(&CharBTN, 1, resource.l_copy_w);

    SetItemLabel(&InfoBTN, 0, resource.l_xlfd);
    SetItemLabel(&InfoBTN, 1, resource.l_codearea);

    CreateMenuButtons( font_menu, &FileBTN, XtNumber(file_btn));
    CreateMenuButtons( edit_menu, &EditBTN, XtNumber(edit_btn));
    Unset();
    CreateMenuButtons( char_menu, &CharBTN, XtNumber(char_btn));
    CreateMenuButtons( info_menu, &InfoBTN, XtNumber(info_btn));

    /* create "Character list" */

    listFrame =
    CreateFrame( baseForm, "listFrame", XmSHADOW_IN, 2);
    AddTopAttachForm(listFrame, resource.ed_wge_topoff);
    AddLeftAttachForm(listFrame, resource.ed_wge_lftoff);
    AddBottomAttachForm(listFrame, resource.ed_wge_btmoff);

    listBase =
    CreateRowColumn( listFrame, "listFrame", L_VERTICAL, 4, 2, 2);

    for( i=0  ;  i < edlist.nlist  ;  i++ ) {
	wgeStaT_form[i] = CreateForm( listBase, "listform" );
	wgeStaT_list[i] =
	CreateLabel( wgeStaT_form[i], "label",	"0000" );
	wgeStaT_disp[i] = CreateDrawingArea( wgeStaT_form[i], "image",
			edg.width, edg.height, EHStaT_disp, i ) ;
	AddLeftAttachWidget( wgeStaT_disp[i], wgeStaT_list[i], 0 ) ;
	XtAddEventHandler( wgeStaT_list[i],
		ButtonPressMask|ButtonReleaseMask,
		False, EHeStaT_list, (XtPointer)i );
	XtAddEventHandler( wgeStaT_disp[i],
		ButtonPressMask|ButtonReleaseMask,
		False, EHeStaT_list, (XtPointer)i );

	if (i==0){
	    XtVaGetValues(wgeStaT_disp[i],
		XmNheight,	&(edlist.elem_h),
		XmNwidth,	&(edlist.elem_w), NULL);

	    edlist.back = resource.pane_background;
	    edlist.border = resource.pane_foreground;

	    dn.elem_h = edlist.elem_h;
	    dn.elem_w = edlist.elem_w;

            /*
	    */
	    edpane.pix_w  = edpane.width / edg.width;
	    edpane.pix_h  = edpane.height / edg.height;
	    if(edpane.pix_w > edpane.pix_h)
		edpane.pix_w = edpane.pix_h;
	    else
		edpane.pix_h = edpane.pix_w;
	    /* */
	    edpane.pix_w  = ( edpane.pix_w < 8 ) ? 8 : edpane.pix_w;
	    edpane.pix_h  = ( edpane.pix_h < 8 ) ? 8 : edpane.pix_h;
	    /* 
	    */
	    edpane.width  = edpane.pix_w * edg.width  - 1;
	    edpane.height = edpane.pix_h * edg.height - 1;

	    edlist.nlist = (edpane.height +2)/(int)(edlist.elem_h +8);
	    edlist.nlist =
		(edlist.nlist<EDLIST_MAX)?edlist.nlist:EDLIST_MAX;
	}
    }

    XtManageChildren( wgeStaT_form, edlist.nlist );
    for( i=0  ;  i < edlist.nlist  ;  i++ ) {
	XtManageChild( wgeStaT_list[i] );
	XtManageChild( wgeStaT_disp[i] );	
    }

    /* create the scroll bar for Charcter list */
    XtVaGetValues( listBase, XmNheight, &(edlist.list_h), NULL);

    if (( slimax = efctPtnNum()) < edlist.nlist)  
	slimax = edlist.nlist;
    wgeScro = CreateScrollBar( baseForm, "scrollBar", (edpane.height+2),
				edlist.nlist, 0, slimax, CBeScro);
    AddLeftAttachWidget( wgeScro, listFrame, 4 );
    AddTopAttachForm( wgeScro, resource.ed_wge_topoff );
    AddBottomAttachForm( wgeScro, resource.ed_wge_btmoff );

    rc =
    CreateForm( baseForm, "rc" );
    AddTopAttachForm( rc, resource.ed_wge_topoff );
    AddRightAttachForm( rc, resource.ed_wge_rghoff );
    AddBottomAttachForm( rc, resource.ed_wge_btmoff );

    editFrame =
    CreateFrame( baseForm, "editFrame", XmSHADOW_IN, 2);
    AddTopAttachForm(editFrame, resource.ed_wge_topoff);
    AddLeftAttachWidget(editFrame, wgeScro, 4);
    AddRightAttachWidget(editFrame, rc, 10);

    figure_w =
    CreatePixButton(rc, "Draw", &DrawEX);
    AddTopAttachForm(figure_w, 0);
    AddLeftAttachForm(figure_w, 0);
    AddRightAttachForm(figure_w, 0);

    imageFrame =
    CreateCaptionFrame( rc, "Frame", "  ", XmSHADOW_IN, 1);
    AddTopAttachWidget(XtParent(imageFrame), figure_w, 4);
    AddLeftAttachForm(XtParent(imageFrame), 0);
    AddRightAttachForm(XtParent(imageFrame), 0);
    AddBottomAttachForm(XtParent(imageFrame), 0);

    imageForm =
    CreateForm( imageFrame, "separator" );

    /* create the Editing pane */
    wgeBulB_edit = editPane =
    CreateDrawingArea( editFrame, "editPane",
			edpane.width, edpane.height, EHeBulB_eExp, 0);

    XtAddEventHandler( editPane,
	      ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
	      False, EHeBulB_eMEv, NULL );
    AddTopAttachForm( editPane, 2 );
    AddLeftAttachForm( editPane, 2 );

    /* create character image */
    codeLabel = CreateLabel( imageForm, "codeLabel", "0000" );
    AddTopAttachForm( codeLabel, 3 );
    AddLeftAttachForm( codeLabel, 3 );

    wgeBulB_disp = image = CreateDrawingArea( imageForm, "image", edg.width,
					edg.height, EHeBulB_dExp, 0);
    AddTopAttachWidget( image, codeLabel, 3 );
    AddLeftAttachForm( image, 6 );

    XtRealizeWidget( editPopW );

    return(editPopW);
}




void
SetCodeString(code)
int code;
{
    char str[8];

    if (! code){
	str[0] = NULL;
    }
    else{
	sprintf( str, "%4x:", code );
    }
    SetLabelString(codeLabel, str);
}



static void
_create_editptn_after()
{
    int		slctloc; 
    static char	dashPtn[] = {1,1};	/* Editing pane's border pattern */
    extern void chgEdList();


    /*
     *  set X-library interface 
     */
    xl.display    = XtDisplayOfObject( editPopW );
    xl.root_winID = RootWindow( xl.display, DefaultScreen(xl.display) );
    xl.edit_winID = XtWindow( wgeBulB_edit );
    xl.disp_winID = XtWindow( wgeBulB_disp );    
    xl.list_winID = XtWindow( wgeStaT_disp[0] );

    /*
     *  Graphic Context ( Pixel clear ) 
     */
    xl.backGC     = XCreateGC( xl.display, xl.root_winID, 0, 0 );
    XSetForeground( xl.display, xl.backGC, edpane.back );
    XSetBackground( xl.display, xl.backGC, edpane.border );

    /*
     *  Graphic Context ( Pixel set ) 
     */
    xl.borderGC   = XCreateGC( xl.display, xl.root_winID, 0, 0 );
    XSetForeground( xl.display, xl.borderGC, edpane.border );
    XSetBackground( xl.display, xl.borderGC, edpane.back );

    /*
     *  Graphic Context ( rubber band ) 
     */
    xl.rubGC      = XCreateGC( xl.display, xl.root_winID, 0, 0 );
    XSetForeground(xl.display, xl.rubGC, edpane.border ^ edpane.back);

    XSetFunction  ( xl.display, xl.rubGC, GXxor );

    /*
     *  Graphc Context ( lattice of Editing pane ) 
     */
    xl.dashGC     = XCreateGC( xl.display, xl.root_winID, 0, 0 );
    XSetForeground( xl.display, xl.dashGC, edpane.border );
    XSetBackground( xl.display, xl.dashGC, edpane.back );

    XSetLineAttributes( xl.display, xl.dashGC, 0,
			LineDoubleDash, CapButt, JoinMiter );
    XSetDashes    ( xl.display, xl.dashGC, 0, dashPtn, 2 );

    /*
     *  display Editing pane 
     */
    xl.dispImage  = XCreateImage( xl.display,
	DefaultVisual( xl.display, DefaultScreen(xl.display) ),
	1, XYBitmap, 0, edg.ptn, edg.width, edg.height, 8, 0	);
    xl.dispImage->bitmap_bit_order = MSBFirst;
    xl.dispImage->byte_order       = MSBFirst;
		    
    /*
     *  initialize 
     */
    edpane.color = ON;
    edpane.obj   = PROC_POINT;
    em.proc      = PROC_POINT;

    if(efctPtnNum() > 0){
	slctloc = 0;
	SetCodeString(edg.code);
    }
    else{
	slctloc = -1;    
    }
    chgEdList( 0, slctloc, ON );

}

void
UpdateMessage(str)
String str;
{
    static Boolean nomsg = False;

    if (! str || ! *str){
	if (nomsg){
	    return;
	}
	else{
	    nomsg = True;
	}
    }
    else{
	nomsg = False;
    }
    SetFooterString(FooterMessage, str);
}
