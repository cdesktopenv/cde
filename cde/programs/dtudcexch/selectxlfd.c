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
/* $XConsortium: selectxlfd.c /main/3 1996/10/14 14:45:17 barstow $ */
/*
 *  All Rights Reserved, Copyright (c) FUJITSU LIMITED 1995
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <nl_types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>

#include "FaLib.h"
#include "falxlfd.h"
#include "selectxlfd.h"
#include "excutil.h"

char *shell_n[] = { "pop0", "pop1", "pop2", "pop3" } ;
char *list_n[] 	= { "list0", "list1", "list2", "list3" } ;

#ifdef DEBUG
#undef Dprintf
#define Dprintf fprintf
#else
#define Dprintf
#endif /* DEBUG */

/*
 * There is no public header file for this function (only an
 * internal header XmStringI.h).
 */
extern XtPointer _XmStringUngenerate (XmString string,
                        XmStringTag tag,
                        XmTextType tag_type,
                        XmTextType output_type);

extern Rsrc resource;
extern Exc_data *ed;
extern ListData *ld;
extern fal_utyerror;
extern fal_utyderror;
extern fal_utyexists;
extern char *maintitle;

FalFontID	fid;


/*
 * parameters
 */

char	*fullPathName	= NULL ;

FalFontDataList		*font_L ;
FalxCodeList		*CodeList ;
FalxStyleList		*StyleList ;
FalxSizeList		*SizeList ;
FalxUdcArea		*UdcArea ;
int			SelectedOffset ;
int			UdcAreaCount ;
int			getmask ;

FalxFontDataBuff	KeyBuff ;


void	PopupSelectXLFD() ;

Widget	xlfdDialog, xlfdWform,
	slctBLabel[BUTTONITEMS],slctBText[BUTTONITEMS],	slctButton[BUTTONITEMS],
	listPop[BUTTONITEMS],	listW[BUTTONITEMS],
	separatorW, errorMD, msgBox ;
extern	Widget	cpyDialog ;

extern Widget toplevel;


/*
 * tmp
 */

int fontcheck(char *fontfile, char *mode)
/*
 * If fontfile can't be opend, return -1.
 * If fontfile is editted by other UDC client, return 1.
 * normary return 0.
 */
{
    int chk_fd;

    if (strcmp(mode, "r") == 0) {
	if ((chk_fd = open(fontfile, O_RDONLY)) < 0) {
	    return (-1);
	}
    } else if (strcmp(mode, "w") == 0) {
	if ((chk_fd = open(fontfile, O_RDWR)) < 0) {
	    return (-1);
	}
    }
    if (isLock(chk_fd) == 1) {
	close(chk_fd);
	return(1);
    }
    close(chk_fd);
    return (0);
}

void setallcode(ListData *ld)
{
    int		i, code, codenum;
    int		*codep;
    int		code1, code2;

    codenum = 0;
    for (i = 0; i < UdcAreaCount; i++) {
	codenum += (abs(UdcArea[i].udc_end - UdcArea[i].udc_start) + 1);
    }

    ld->allcode = (int *) calloc(codenum, sizeof(int));
    ld->allcode_num = codenum;
    codep = ld->allcode;
    for (i = 0; i < UdcAreaCount; i++) {
	code1 = smaller(UdcArea[i].udc_start, UdcArea[i].udc_end);
	code2 = bigger(UdcArea[i].udc_start, UdcArea[i].udc_end);
	for (code = code1; code <= code2; code++) {
	    *codep = code;
	    codep++;
	}
    }
}

FalFontID openfont(void)
{
    int		protect_key = FAL_FONT_DISPLAY;
    int		codeset;
    FalFontID	fontid;

    fontid = NULL;
    codeset = font_L->list[0].cd_set;
    if ((fontid = FalOpenFont(fullPathName, protect_key, codeset))
	== FAL_ERROR) {
	fprintf(stderr, "FalOpenFont error\n");
	return ((FalFontID) NULL);
    }
    return (fontid);
}

void makelist(ListData *ld)
{
    char	*pattern;
    int		code;
    int		*existcodep, *allcodep;
    int		existcode_num;
    int		i;
/*    FalFontinfo fontinfo;*/

/*    FalQueryFont(fid, &fontinfo);*/

    ld->existcode = (int *) calloc(ld->allcode_num, sizeof(int));
    existcodep = ld->existcode ;
    allcodep = ld->allcode;
    existcode_num = 0;
    for (i = 0; i < ld->allcode_num; i++) {
	code = *allcodep;
	pattern = FalReadFont(fid, code, NULL, NULL);
	if (fal_utyexists == 0) {
	    *existcodep = *allcodep;
	    existcodep++;
	    existcode_num++;
	}
	allcodep++;
    }
    ld->existcode_num = existcode_num;
}

void setexistcode(ListData *ld)
{
    int *codep, *allcodep;
    int i;

/* open font */
    fid = openfont();

    makelist(ld);

/* close font */
    if (fid != NULL)
	FalCloseFont(fid);
}


char i2c(int num)
{
    char c;

    if ((0 <= num) && (num <= 9))
	c = '0' + num;
    else if ((10 <= num) && (num <= 15))
	c = 'a' + (num - 10);
    else
	fprintf(stderr,"error in i2c\n");
    return c;
}

char *i2s(int code)
{
    char	buf[8];
    char	*number;
    char	*cp;
    int		i;
    int		tmp;
    int		len;

    tmp = code;
    for (i = 0; ; i++) {
	if (tmp < 16) {
	    buf[i] = i2c(tmp);
	    buf[i+1] = NULL;
	    break;
	}
	buf[i] = i2c(tmp % 16);
	tmp = tmp / 16;
    }
    len = strlen(buf);
    number = (char *) calloc(len+3, sizeof(char));
    cp = number;
    memcpy(cp, "0x", 2);
    cp += 2;
    for (i = len - 1; i >= 0; i--) {
	*cp = buf[i];
	cp++;
    }
    *cp = NULL;
    return (number);
}

void setexistcode_c(ListData *ld)
{
    int		i;
    char	**code_cp;
    int		*codep;

    if (ld->existcode_num == 0) {
	return;
    }

    ld->existcode_c = (char **) calloc(ld->existcode_num, sizeof(char *));
    code_cp = ld->existcode_c;
    codep = ld->existcode;
    for (i = 0; i < ld->existcode_num; i++) {
	*code_cp = i2s(*codep);
	code_cp++;
	codep++;
    }
}

void getexistcode(ListData *ld)
{
    setallcode(ld);
    setexistcode(ld);
    setexistcode_c(ld);
}



/****************************************************************
 * common functions						*
 ***************************************************************/

/*
 * pop up and down dialog box
 */

void
xlfdPopupDialog(Widget w)
{
    if (! XtIsManaged(w))
        XtManageChild(w);
    else
        XRaiseWindow(XtDisplayOfObject(w), XtWindow(XtParent(w)));
}

void
xlfdPopdownDialog(Widget w)
{
    if (XtIsManaged(w)){
        XtUnmanageChild(w);
    }
}


/*
* get pixel width
*/
Dimension
GetPixelWidth(Widget w, int columns, Dimension *width)
{
	Arg		args[1] ;
	XtSetArg( args[0], XmNwidth, width ) ;
	XtGetValues( w , args, 1 ) ;
	return( *width / (Dimension)columns ) ;
}


/****************************************************************
 * callbacks							*
 ***************************************************************/

void quit(Widget w, caddr_t *client_data, caddr_t *call_data)
{
    char	*msg;
    int		ans;

    msg = GETMESSAGE(6, 2, "Do you want to terminate udcexchange?");

    AskUser(ed->toplevel, ed, msg, &ans, "question");
    if (ans == 1) {/* terminate ok */
	FalxFreeCodeList( CodeList ) ;
	FalxFreeStyleList( StyleList ) ;
	FalxFreeSizeList( SizeList ) ;
	FalxFreeFontList( font_L ) ;
	excterminate(ed);
    }
}


/*
 * Unmanage widgets
 */
int
ClearText(int num, Widget *slctBText)
{
	int 	i ;
	/* toggle button unset */
	for( i=0; i<BUTTONITEMS; i++ ){
	    if( i>num ){
		XmTextSetString( slctBText[i], "" );
	    }
	}
	return ;
}

int
ClearButtons(int num, Widget *slctButton)
{
	int 	i, cnt ;
	Boolean	isSet ;
	Arg	args[5] ;
	/* toggle button unset */
	for( i=0; i<BUTTONITEMS; i++ ){
	    if( i != num ){
		cnt = 0;
		XtSetArg( args[cnt], XmNset, &isSet );	cnt++;
		XtGetValues( slctButton[i], args, cnt );
		if( isSet == True ){
		    cnt = 0;
		    XtSetArg( args[cnt], XmNset, False );	cnt++;
		    XtSetValues( slctButton[i], args, cnt );
		}
	    }
	}
	return ;
}



/*
 * get text field position
 */
int
GetPositionOfLists(
int	num,
Widget	xlfdDialog,
Widget	*listPop,
Widget	*slctBText,
Position	*x,
Position	*y)
{
	int		i ;
	Position	tbx, tby, tx, ty ;
	Dimension	theight ;
	Arg	args[10] ;

	i = 0;
	XtSetArg(args[i], XmNx, &tbx );	i++;
	XtSetArg(args[i], XmNy, &tby );	i++;
	XtGetValues( xlfdDialog, args, i);
	i = 0;
	XtSetArg(args[i], XmNx, &tx );	i++;
	XtSetArg(args[i], XmNy, &ty );	i++;
	XtSetArg(args[i], XmNheight, &theight );	i++;
	if( num==BUTTONITEMS-1 )
	    XtGetValues( slctBText[num], args, i ) ;
	else
	    XtGetValues( XtParent(slctBText[num]), args, i ) ;	/* ScrolledText */
	/*  determine the position of list */
	*x = tx + tbx + CX ;
	if( num==BUTTONITEMS-1 ){
	*y = ty + tby + (Position)theight + 14 + CY ;
	}else{
	*y = ty + tby + (Position)theight + CY ;
	}

	for( i=0; i<BUTTONITEMS; i++ ){
	    if( XtIsManaged( listPop[i] ) )	XtUnmanageChild( listPop[i] );
	}
	if( !XtIsManaged( listPop[num] ) )	XtManageChild( listPop[num] );

	return ;
}




void listEH(Widget w, int num, XButtonEvent *event)
{
	int		i, j, ITEM_COUNT;
	Arg		args[10];
	XmString	*xms ;
	char		*str ;

	int		rtn ;
	Position	x, y ;

	/*
	 *	get list items
	 */
	ITEM_COUNT = 0;
	rtn = GetItemsToDisplay( num, &ITEM_COUNT, &xms ) ;
	switch( rtn ){
	    case FALX_TRY_NEXT :
		fprintf( stderr, "There are no fonts that was required.\n" ) ;
		XmTextSetString( slctBText[0], " " );
		ClearText( -1, slctBText ) ;
		FalxFreeDataBuff( FALX_ALL, &KeyBuff ) ;
		getmask = 0 ;
		return ;
	    case 0 :
		break ;
	    default :
		fprintf( stderr, "GetItemsToDisplay() error.\n" ) ;
		exit(-1) ;
	}

	if( num == 0 && ITEM_COUNT == 1 ){

	    str = (char *) _XmStringUngenerate((XmString) xms[0], NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	    if (str)
	       XmTextSetString(slctBText[num], str );
	    return ;
	}
	if( ITEM_COUNT < 0 ){
	    exit(-1) ;
	}


	/*
	 * get text field position
	 */
	GetPositionOfLists( num, xlfdDialog, listPop, slctBText, &x, &y ) ;

	i = 0;
	XtSetArg(args[i], XmNx, x  ) ;	i++;
	XtSetArg(args[i], XmNy, y  ) ;  i++;
	XtSetValues(listPop[num], args, i );

	/*
	 *	set list items
	 */
	i = 0;
	XtSetArg (args[i], XmNitems, xms ); i++;
	XtSetArg (args[i], XmNitemCount, ITEM_COUNT ); i++;
	XtSetValues(listW[num], args, i );

	/*
	 * pop up list
	 */
	XtPopup( listPop[num], XtGrabNone );

}




void listPrintCB(Widget w, int num, XmListCallbackStruct *call_data)
{
	char	*str ;

	/* clear text fields and unset buttons */
	ClearButtons( num, slctButton ) ;


	str = (char *) _XmStringUngenerate((XmString) call_data->item, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (str) {
	    XmTextSetString(slctBText[num], str);
	}

#if 0
	/* set key data 	*/
	if( SetKeyBuff( num, str ) ){
	    exit(-1) ;
	}
#endif	/* 0 */

	if( XtIsManaged( listPop[num] ) )	XtUnmanageChild( listPop[num] );

}





void OpenWindow(Widget w, caddr_t client_data, caddr_t *call_data)
{
    int		r ;
    int		i ;
    int		ans;
    char	*msg;
    char	*msg1;
    char	*msg2;
    char	*msg3;
    char	*str, buff[512] ;

    msg = GETMESSAGE(6, 4, "No UDCs exist in this font");
    msg1 = GETMESSAGE(6, 6, "XLFD name isn't selected");
    msg2 = GETMESSAGE(6, 8, "Can't open the font file");
    msg3 = GETMESSAGE(6, 10, "The font file is used by other UDC client.");

    /* get font file name (str:  XLFD name)	*/
    str = XmTextGetString( slctBText[0] );
    if (( str == NULL ) || (strcmp(str, "") == 0)) {
	AskUser(w, ed, msg1, &ans, "error");
	return;
    }
    if( fullPathName != NULL ){
	free( fullPathName ) ;
    }
    SelectedOffset = FalxSearchFontFileName( str, font_L, buff ) ;
    if( SelectedOffset < 0 ){
        exit(-1) ;
    }
    if( (fullPathName = strdup( buff )) == NULL ){
        exit(-1) ;
    }
    UdcAreaCount = FalxGetGlyphIndexInfo( str, font_L, &UdcArea ) ;
    if( UdcAreaCount < 0 ){
        exit(-1) ;
    }

    if( fullPathName != NULL ){
	Dprintf( stderr, "OpenWindow() : full path name = %s\n", fullPathName );
    }
    for( i=0; i<UdcAreaCount; i++ ){
	Dprintf( stderr, "OpenWindow() : start(%d) = 0x%x\n", i, UdcArea[i].udc_start );
	Dprintf( stderr, "OpenWindow() : end  (%d) = 0x%x\n", i, UdcArea[i].udc_end );
    }
    if ((ed->xlfdname = strdup(str)) == NULL) {
	exit (-1);
    }
    if ((ed->fontfile = strdup(fullPathName)) == NULL) {
	exit (-1);
    }
/*    getxlfd(ed);*/
/* initialize ld */
    if ((ld = (ListData *) malloc(sizeof(ListData))) == NULL) {
	excerror(ed, EXCERRMALLOC, "selcharcd", "exit");
    }
    memset(ld, 0x00, sizeof(ListData));
    ld->ed = ed;

    if (ed->function == EXPORT) {
	r = fontcheck(ed->fontfile, "r");
	if (r == -1) { /* file open error */
	    AskUser(ld->ed->toplevel, ld->ed, msg2, &ans, "error");
	    return;
	} else if (r == 1) { /* file is being editted by other UDC client */
	    AskUser(ld->ed->toplevel, ld->ed, msg3, &ans, "error");
	    return;
	}
        /* get existing UDC code */
	getexistcode(ld);
	if (ld->existcode_num == 0) {
	    AskUser(ld->ed->toplevel, ld->ed, msg, &ans, "error");
	    return;
	}
	XtUnmanageChild(xlfdDialog);
	selcharcd(ed);
    } else if (ed->function == IMPORT) {
	r = fontcheck(ed->fontfile, "w");
	if (r == -1) { /* file open error */
	    AskUser(ld->ed->toplevel, ld->ed, msg2, &ans, "error");
	    return;
	} else if (r == 1) { /* file is being editted by other UDC client */
	    AskUser(ld->ed->toplevel, ld->ed, msg3, &ans, "error");
	    return;
	}
        /* get existing UDC code */
	getexistcode(ld);

	XtUnmanageChild(xlfdDialog);
	getcharcd(ed);
    }
}



/****************************************************************
 * functions for create window view
****************************************************************/

#define SetLabel( cnt ){ \
	switch( cnt ){ \
	    case 0 : buf = resource.xlfd_label ; break ; \
	    case 1 : buf = resource.code_label ; break ; \
	    case 2 : buf = resource.style_label; break ; \
	    case 3 : buf = resource.size_label ; break ; \
	} \
        xms = XmStringCreateLocalized( buf ); \
}


int
CreateXLFDLabelAndText(
Widget	owner,
Widget	*slctBLabel,
Widget	*slctBText,
Widget	*slctButton)
{

	Widget	_slctBLabel ;
	Widget	_slctBText ;
	Widget	_slctButton ;
	int		i, j;
	Arg		args[16];
	char		*buf ;
	XmString	xms ;

        /*
	*  create XLFD titles
	*/
	i = 0;
	XtSetArg( args[i], XmNx,            20 ) ;	i++ ;
	XtSetArg( args[i], XmNheight,       20 ) ;	i++ ;
	XtSetArg( args[i], XmNtopAttachment, XmATTACH_FORM ) ; i++ ;
	XtSetArg( args[i], XmNtopOffset,    20 ) ;	i++ ;
/*
	_slctBLabel = XmCreateLabelGadget( owner , "xlfdlabel", args, i );
*/
	_slctBLabel = XmCreateLabel( owner , "xlfdlabel", args, i );

	XtManageChild( _slctBLabel );

        /*
	*  create text fields to display XLFD font name
	*/
	i = 0;
	XtSetArg( args[i], XmNleftAttachment,	XmATTACH_FORM ) ; i++ ;
	XtSetArg( args[i], XmNleftOffset,	20 ) ;		i++ ;
	XtSetArg( args[i], XmNtopAttachment,	XmATTACH_WIDGET ); i++ ;
	XtSetArg( args[i], XmNtopOffset,	5 ) ;		i++ ;
        XtSetArg (args[i], XmNtopWidget,	_slctBLabel );	i++;
	XtSetArg( args[i], XmNcolumns,		XLFD_COLUMNS ) ;       	i++ ;
	XtSetArg( args[i], XmNspacing,		0 ) ;  		i++ ;
	XtSetArg( args[i], XmNeditMode, XmSINGLE_LINE_EDIT ) ;	i++ ;
	XtSetArg( args[i], XmNscrollBarDisplayPolicy,   XmAS_NEEDED ) ; i++ ;
	XtSetArg( args[i], XmNscrollingPolicy,   XmAPPLICATION_DEFINED ) ;  i++ ;
/*
	XtSetArg( args[i], XmNscrollingPolicy,   XmAUTOMATIC ) ;  i++ ;
	XtSetArg( args[i], XmNlistSizePolicy,   XmCONSTANT ) ;	i++ ;

	XtSetArg( args[i], XmNwidth, 600 ) ;			i++ ;
	XtSetArg( args[i], XmNfontList,	fixed ) ;	i++ ;
	XtSetArg( args[i], XmNeditable,	True ) ;       i++ ;
	XtSetArg( args[i], XmNlistSizePolicy,   XmVARIABLE ) ;  i++ ;
*/
	_slctBText = XmCreateScrolledText( owner , "xlfdtext", args, i );

	XtManageChild( _slctBText );

        /*
	*  create XLFD select buttons
	*/
	i = 0;
	XtSetArg( args[i], XmNindicatorType,	XmONE_OF_MANY ) ;	i++ ;
        XtSetArg (args[i], XmNtopWidget,	_slctBText );		i++;
	XtSetArg( args[i], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ;	i++ ;
	XtSetArg( args[i], XmNtopOffset,        15  ) ;			i++ ;
	XtSetArg( args[i], XmNleftAttachment,	XmATTACH_WIDGET ) ;	i++ ;
	XtSetArg( args[i], XmNleftWidget,	_slctBText ) ;		i++ ;
	XtSetArg( args[i], XmNleftOffset,	5 ) ;		i++ ;	/* WIDTH */
	XtSetArg( args[i], XmNrightAttachment, XmATTACH_FORM ) ;      i++ ;
	XtSetArg( args[i], XmNrightOffset,	5 ) ;		i++ ;	/* WIDTH */
        xms = XmStringCreateLocalized( "" );
	XtSetArg( args[i], XmNlabelString,     xms ) ;      i++ ;
/*
	_slctButton = XmCreateToggleButtonGadget( owner , "xlfdbtn", args, i);
*/
	_slctButton = XmCreateToggleButton( owner , "xlfdbtn", args, i);

	XtManageChild( _slctButton );

	*slctBLabel = _slctBLabel ;
	*slctBText  = _slctBText ;
	*slctButton = _slctButton ;

}




int
CreateOtherLabelAndText(
int	num,
Widget	owner,
Widget	baseForm,
Widget	*slctBLabel,
Widget	topW,
Widget	*slctBText,
Widget	*slctButton)
{

	Widget	_slctBLabel ;
	Widget	_slctBText ;
	Widget	_slctButton ;
	int		i, j;
	Arg		args[16];
	char		*buf ;
	XmString	xms ;


	if( num < 1 )	return(0) ;

        /*
	*  create key titles
	*/
	i = 0;
        XtSetArg ( args[i], XmNtopWidget,
		(num==1) ? baseForm : topW ); i++;
	XtSetArg( args[i], XmNx,            20 ) ;	i++ ;
	XtSetArg( args[i], XmNheight,       20 ) ;	i++ ;
	XtSetArg( args[i], XmNtopAttachment, XmATTACH_WIDGET ) ; i++ ;
/* DEBUG
	XtSetArg( args[i], XmNtopOffset,    20 ) ;	i++ ;
*/
	XtSetArg( args[i], XmNtopOffset,    36 ) ;	i++ ;
	SetLabel(num) ;
	XtSetArg( args[i], XmNlabelString,	xms ) ;	i++ ;
/*
	_slctBLabel = XmCreateLabelGadget( owner , "label", args, i);
*/
	_slctBLabel = XmCreateLabel( owner , "label", args, i);

	XtManageChild( _slctBLabel );

        /*
	*  create text fields to display selected keys
	*/
	i = 0;
	XtSetArg( args[i], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET ) ;	i++ ;
	XtSetArg( args[i], XmNtopOffset,	-5 ) ;		i++ ;
        XtSetArg (args[i], XmNtopWidget, 	_slctBLabel );	i++;
	XtSetArg( args[i], XmNrightAttachment,	XmATTACH_OPPOSITE_WIDGET ) ; i++ ;
	XtSetArg( args[i], XmNrightOffset,	0 ) ;		i++ ;
        XtSetArg (args[i], XmNrightWidget,	baseForm );	i++;
	XtSetArg( args[i], XmNcolumns,		KEY_COLUMNS ) ;		i++ ;
	XtSetArg( args[i], XmNspacing,		0 ) ;		i++ ;
	XtSetArg( args[i], XmNeditMode,		XmSINGLE_LINE_EDIT ) ;	i++ ;
	XtSetArg( args[i], XmNscrollBarDisplayPolicy,   XmAS_NEEDED ) ;	i++ ;
	XtSetArg( args[i], XmNscrollingPolicy,   XmAPPLICATION_DEFINED ) ;  i++ ;
	/*
	XtSetArg( args[i], XmNscrollingPolicy,   XmAUTOMATIC ) ; 	i++ ;
	XtSetArg( args[i], XmNlistSizePolicy,	XmCONSTANT ) ;	i++ ;

	XtSetArg( args[i], XmNfontList,	fixed ) ;		i++ ;
	XtSetArg( args[i], XmNlistSizePolicy,   XmVARIABLE ) ;  i++ ;
	*/
	if( num==BUTTONITEMS-1 )
	    _slctBText = XmCreateText( owner , "text" , args, i );
	else
	    _slctBText = XmCreateScrolledText( owner , "text" , args, i );

	XtManageChild( _slctBText );

        /*
	*  create select buttons
	*/
	i = 0;
	XtSetArg( args[i], XmNindicatorType,	XmONE_OF_MANY ) ; i++ ;
	XtSetArg( args[i], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ;	i++ ;
	XtSetArg( args[i], XmNtopOffset,        5  ) ;		i++ ;
        XtSetArg (args[i], XmNtopWidget,	_slctBLabel );	i++;
	XtSetArg( args[i], XmNleftAttachment,	XmATTACH_WIDGET ) ;	i++ ;
	XtSetArg( args[i], XmNleftWidget,	_slctBText  ) ;		i++ ;
	XtSetArg( args[i], XmNleftOffset,	5 ) ;		i++ ;	/* WIDTH */
        xms = XmStringCreateLocalized( "" );
	XtSetArg( args[i], XmNlabelString,     xms ) ;		i++ ;
/*
	_slctButton = XmCreateToggleButtonGadget( owner , "button", args, i);
*/
	_slctButton = XmCreateToggleButton( owner , "button", args, i);

	XtManageChild( _slctButton );

	*slctBLabel = _slctBLabel ;
	*slctBText  = _slctBText ;
	*slctButton = _slctButton ;
}



/*
* create pop up lists
*/


Widget
xlfdCreateScrolledList (Widget owner, char *name, Arg *args, int n)
{
	Widget		listW ;

	XtSetArg( args[n], XmNselectionPolicy,		XmSINGLE_SELECT ) ;n++ ;
	XtSetArg( args[n], XmNlistSpacing,		4 ) ;		n++ ;
	XtSetArg( args[n], XmNlistSizePolicy,		XmCONSTANT ) ;	n++ ;
	XtSetArg( args[n], XmNscrollBarDisplayPolicy,	XmAS_NEEDED ) ;	n++ ;
        listW = XmCreateScrolledList( owner, name, args, n );

	XtManageChild( listW );


	return( listW );
}



/*
* create selection window view
*/
void
PopupSelectXLFD(Widget top)
{

    int		n, i, j;
    Arg		args[16];
    char		*buf ;
    XmString	xms ;
    Dimension	pix, xwidth, kwidth ;

    if( xlfdDialog == NULL ){
        /*
	*  create base window
	*/
        n = 0 ;
        XtSetArg( args[n], XmNautoUnmanage, False ) ;		n++ ;
        XtSetArg( args[n], XmNtitle, maintitle ) ;		n++ ;
        XtSetArg( args[n], XmNnoResize, True ) ;		n++ ;
        xms = XmStringCreateLocalized( resource.exec_label ) ;
        XtSetArg( args[n], XmNokLabelString, xms ) ;		n++ ;
        xms = XmStringCreateLocalized( resource.quit_label) ;
        XtSetArg( args[n], XmNhelpLabelString, xms ) ;		n++ ;
Dprintf( stdout, "(PopupSelectXLFD)XmCreateTemplateDialog: start\n" ) ;
        xlfdDialog = XmCreateTemplateDialog( top, "SelectDialog", args, n );
Dprintf( stdout, "(PopupSelectXLFD)XmCreateTemplateDialog: end\n" ) ;

	n = 0 ;
	xlfdWform  = XmCreateForm( xlfdDialog, "BaseForm", args, n );
	XtManageChild( xlfdWform );


        /*
	*  create XLFD columns
	*/
Dprintf( stdout, "(PopupSelectXLFD)CreateXLFDLabelAndText : start\n" ) ;
	CreateXLFDLabelAndText( xlfdWform ,
				 &slctBLabel[0], &slctBText[0], &slctButton[0] ) ;
	n = 0 ;
	SetLabel(0) ;
	XtSetArg( args[n], XmNlabelString,	xms ) ;	n++ ;
	XtSetValues( slctBLabel[0], args, n ) ;
Dprintf( stdout, "(PopupSelectXLFD)CreateXLFDLabelAndText : end\n" ) ;

        /*
	*  create other key columns
	*/
Dprintf( stdout, "(PopupSelectXLFD)CreateOtherLabelAndText :start\n" ) ;
	for( j=1; j<BUTTONITEMS; j++ ){
	    CreateOtherLabelAndText( j, xlfdWform, slctBText[0],
		 &slctBLabel[j], slctBLabel[j-1], &slctBText[j], &slctButton[j] ) ;
	}
Dprintf( stdout, "(PopupSelectXLFD)CreateOtherLabelAndText : end\n" ) ;

	/* set bottom offsets */
	n = 0 ;
	XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ) ; n++ ;
	XtSetArg( args[n], XmNbottomOffset,	36 ) ;		n++ ;
	XtSetValues( slctBLabel[BUTTONITEMS-1], args, n ) ;

	/* get pixel size */
	pix = GetPixelWidth( slctBText[0], XLFD_COLUMNS, &xwidth ) ;
	/* xwidth += pix / 2 ; */
	kwidth = pix * KEY_COLUMNS + pix / 2 ;

        /*
	*  create popup shells
	*/
	for( j=0; j<BUTTONITEMS; j++ ) {
	    n = 0;
	    listPop[j] = XtCreatePopupShell( shell_n[j], overrideShellWidgetClass,
					     xlfdWform , args, n );
	}

        /*
	*  create popup lists
	*/
	for( j=0; j<BUTTONITEMS; j++ ) {
	    n = 0 ;
	    XtSetArg( args[n], XmNvisibleItemCount, (j==0)? 10 : 5 ) ;	n++ ;
	    if( j==0 ){
		XtSetArg( args[n], XmNwidth,   xwidth ) ;		n++ ;
	    }else{
		XtSetArg( args[n], XmNwidth,   kwidth ) ;		n++ ;
	        XtSetArg( args[n], XmNlistMarginWidth,   0 ) ;		n++ ;
	    }
            listW[j] = xlfdCreateScrolledList( listPop[j], list_n[j], args, n );
	}

        /*
	 * Error_Messege
	 */
#if 0
	n = 0 ;
        XtSetArg( args[n], XmNokLabelString,	"OK" ) ;    n++ ;
        XtSetArg( args[n], XmNcancelLabelString,"Cancel" ) ;    n++ ;
        errorMD = XmCreateMessageDialog(toplevel, "ed0", NULL, 0);

        msgBox = XmMessageBoxGetChild(errorMD, XmDIALOG_HELP_BUTTON);
        XtUnmanageChild(msgBox);
#endif	/* 0 */

	/*
	 * add callbacks
	 */
	for( i=0; i<BUTTONITEMS; i++ )
	    XtAddEventHandler( slctButton[i], ButtonReleaseMask, False, (XtEventHandler) listEH, (XtPointer) i );

	for( i=0; i<BUTTONITEMS; i++ )
	    XtAddCallback(listW[i], XmNsingleSelectionCallback, (XtCallbackProc) listPrintCB, (XtPointer) i );

        XtAddCallback( xlfdDialog, XmNokCallback, (XtCallbackProc) OpenWindow, NULL );
        XtAddCallback( xlfdDialog, XmNhelpCallback,(XtCallbackProc) quit,NULL );

#if 0
        XtAddCallback( errorMD,	XmNcancelCallback, (XtCallbackProc) quit, NULL);
#endif	/* 0 */

    }
    /*
     * pop up select window
     */
Dprintf( stdout, "(PopupSelectXLFD)xlfdPopupDialog : start\n" ) ;
    xlfdPopupDialog( xlfdDialog );
Dprintf( stdout, "(PopupSelectXLFD)xlfdPopupDialog : end\n" ) ;
Dprintf( stdout, " : end\n" ) ;

}



#undef SetLabel
