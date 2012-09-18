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
/* util.c 1.22 - Fujitsu source for CDEnext    96/10/30 13:31:41      */
/* $XConsortium: util.c /main/13 1996/11/08 01:56:34 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */




#include <stdlib.h>
#include <wchar.h>
#include <Xm/XmAll.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>

#include "util.h"
#include "ufontrsrc.h"
#include "FaLib.h"

extern Widget toplevel;
static void _destroy();
void _unmap();

extern Resource resource ;

#include "xpm.h"
#include "pixmaps/Pencil.pm"
#include "pixmaps/Line.pm"
#include "pixmaps/Rectangle.pm"
#include "pixmaps/Circle.pm"
#include "pixmaps/Eraser.pm"
#include "pixmaps/SelectArea.pm"

#ifdef XPM
#define ReadXpm XpmCreatePixmapFromData
#else
#define ReadXpm _DtXpmCreatePixmapFromData
#endif
#define NUMPIX 6

#include "pixmaps/arrow.pm"

/*ARGSUSED*/
Widget
CreateCaptionFrame(owner, name, labelstr, type, thickness)
Widget owner;
String name;
String labelstr;
int type;
int thickness;
{
    Widget top, label, frame;
    Arg args[20];
    int n;
    XmString xmstr;

    n = 0;
    top = XmCreateForm(owner, "form", args, n);
    if (labelstr && *labelstr){
        xmstr = XmStringCreateLocalized(labelstr);
        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        XtSetArg(args[n], XmNlabelString, xmstr);  n++;
        label = XmCreateLabelGadget(top, "label", args, n);
        XtManageChild(label);
        XmStringFree(xmstr);

        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
        XtSetArg(args[n], XmNtopWidget, label);  n++;
    }
    else{
        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    }
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNresizable, resource.capt_resize  );  n++;
    XtSetArg(args[n], XmNshadowType, type);  n++;
    XtSetArg(args[n], XmNshadowThickness , thickness);  n++;
    XtSetArg(args[n], XmNleftOffset, resource.capt_lftoff );  n++;
    XtSetArg(args[n], XmNtopOffset, resource.capt_topoff );  n++;
    XtSetArg(args[n], XmNrightOffset, resource.capt_rghoff );  n++;
    frame = XmCreateFrame(top, "frame", args, n);
    XtManageChild(frame);
    XtManageChild(top);
    return(frame);
}

/*
 *
 *	create pix button
 *
 */

Widget
CreatePixButton(owner, name, data)
Widget owner;
String name;
RadioButt *data;
{
    Arg args[20];
    int i, n;
    Pixmap mask;
    XpmAttributes attr;
    Pixmap pix[NUMPIX];
    Widget top;
    Display *disp;
    Window root;

    disp = XtDisplay(owner);
    root = DefaultRootWindow(disp);

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    XtSetArg(args[n], XmNradioAlwaysOne, TRUE); n++;
    XtSetArg(args[n], XmNradioBehavior, TRUE); n++;
    top = XmCreateRowColumn(owner, name, args, n);

    attr.valuemask = 0;

    ReadXpm(disp, root, Pencil, &pix[0], &mask, &attr);
    ReadXpm(disp, root, Line, &pix[1], &mask, &attr);
    ReadXpm(disp, root, Rectangle, &pix[2], &mask, &attr);
    ReadXpm(disp, root, Circle, &pix[3], &mask, &attr);
    ReadXpm(disp, root, Eraser, &pix[4], &mask, &attr);
    ReadXpm(disp, root, SelectArea, &pix[5], &mask, &attr);

    for (i=0; i < NUMPIX; i++) {
	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg(args[n], XmNlabelPixmap, pix[i]); n++;
	XtSetArg(args[n], XmNindicatorOn, False); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNfillOnSelect, False); n++;
	XtSetArg(args[n], XmNset, data->items[i].set); n++;
	XtSetArg(args[n], XmNwidth, 36); n++;
	XtSetArg(args[n], XmNheight, 36); n++;
	data->items[i].w = XmCreateToggleButton(top, data->items[i].name, args, n);
	XtManageChild(data->items[i].w);
	XtAddCallback(data->items[i].w, XmNvalueChangedCallback,
				 data->items[i].cb, data->items[i].clientdata);
    }
    XtManageChild(top);
    return(top);
}


/*
 *
 *	returns the value what the text field has 
 *
 */

#ifdef _HPUX_SOURCE
String
#else
XtPointer
#endif
GetTextFieldValue(textf)
TextField *textf;
{
    char *s1, *s2, *s3;
    if (textf->w2 == NULL) {
	    XtVaGetValues(textf->w1, XmNvalue, &s1, NULL);
	    return(s1);
    } else {
	if (XtIsSensitive(textf->w2)) {
	    XtVaGetValues(textf->w1, XmNvalue, &s1, NULL);
	    XtVaGetValues(textf->w2, XmNvalue, &s2, NULL);
	    s3 = (char *) XtMalloc(strlen(s1) + strlen(s2) + 2);
	    strcpy(s3, s1);
	    strcat(s3, "-");
	    strcat(s3, s2);
	    XtFree(s1);
	    XtFree(s2);
	    return(s3);
	} else {
	    XtVaGetValues(textf->w1, XmNvalue, &s1, NULL);
	    return(s1);
	}
    }
}

/*ARGSUSED*/
static void
arrow_change(w, data)
TextField *data;
{
    if (XtIsSensitive(data->w2)) {
	XtSetSensitive(data->w2, False);
    } else {
	XtSetSensitive(data->w2, True);
    }
}

static Widget		focus_widget=NULL;
extern char		AreaStr[160];
extern FalFontData	fullFontData;

static void
focus(w)
Widget w;
{
    focus_widget = w;
}

static void
code_input()
{
    extern void CodeWindow();
    CodeWindow(focus_widget, fullFontData.xlfdname, False);
}

/*ARGSUSED*/
void
CreateTextField(owner, name, labelstr, data, maxlength)
Widget owner;
String name;
String labelstr;
TextField *data;
int maxlength;
{
    Widget		row, label, arrow, textfield, code;
    Arg			args[20];
    register int	n;
    Display		*disp;
    Window		root;
    Pixmap		mask;
    XpmAttributes	attr;
    XmString		xms;
    extern Pixmap	arrow_pix;

    n = 0;
    XtSetArg(args[n], XmNorientation, (XtArgVal)XmHORIZONTAL); n++;
    row = XmCreateRowColumn(owner, "row", args, n);
    XtManageChild(row);

    n = 0;
    xms = XmStringCreateLocalized(labelstr);
    XtSetArg(args[n], XmNlabelString, xms);  n++;
    label = XmCreateLabelGadget(row, "label", args, n);
    XtManageChild(label);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNcolumns, maxlength);  n++;
    XtSetArg(args[n], XmNmaxLength, maxlength);  n++;
    data->w1 = focus_widget = textfield =
				XmCreateText(row, "textField", args, n);
    XtManageChild(textfield);
    XtAddCallback(textfield, XmNfocusCallback, (XtCallbackProc)focus, NULL);

    if (! arrow_pix) {
	disp = XtDisplay(row);
	root = DefaultRootWindow(disp);
	attr.valuemask = 0;
	ReadXpm(disp, root, arrow_pm, &arrow_pix, &mask, &attr);
    }

    n = 0;
    XtSetArg(args[n], XmNlabelPixmap, arrow_pix);  n++;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
    XtSetArg(args[n], XmNindicatorOn, False); n++;
    XtSetArg(args[n], XmNshadowThickness, 2); n++;
    XtSetArg(args[n], XmNfillOnSelect, False); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    arrow = XmCreateToggleButton(row, "arrow", args, n);
    XtManageChild(arrow);
    XtAddCallback(arrow, XmNvalueChangedCallback,
    				(XtCallbackProc)arrow_change, (XtPointer)data);

    n = 0;
    XtSetArg(args[n], XmNcolumns, maxlength );  n++;
    XtSetArg(args[n], XmNmaxLength, maxlength);  n++;
    data->w2 = textfield = XmCreateText(row, "textField", args, n);
    XtManageChild(textfield);
    XtSetSensitive(textfield, False);
    XtAddCallback(textfield, XmNfocusCallback, (XtCallbackProc)focus, NULL);

    n = 0;
    xms = XmStringCreateLocalized(resource.code) ;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    code = XmCreatePushButton(row, "Code", args, n);
    XtManageChild(code);
    XmStringFree(xms);

    XtAddCallback(code, XmNactivateCallback,
			(XtCallbackProc)code_input, (XtPointer)textfield);
}


void
#if __STDC__
CreateMenuButtons( Widget owner, Button *buttons, int buttons_cnt )
#else
CreateMenuButtons( owner, buttons, buttons_cnt )
Widget	owner;
Button	*buttons;
int		buttons_cnt;
#endif
{
    Arg args[4];
    char buf[64];
    XmString xms;
    int i, n;

#define LABEL(x)	(buttons->items[(x)].label)
#define NMNIC(x)	(buttons->items[(x)].mnemonic)
#define SENS(x) 	(buttons->items[(x)].sensitive)

    buttons->w = (Widget *) malloc(sizeof(Widget)*buttons_cnt);

    if (buttons->w) {
	for (i=0; i<buttons_cnt; i++) {
	    if (LABEL(i) && *((char *) LABEL(i))){
		n=0;
		if (strchr(LABEL(i), NMNIC(i))){
		    sprintf(buf, "%s", LABEL(i));
		}else{
		    sprintf(buf, "%s(%c)", LABEL(i), NMNIC(i));
		}
		xms = XmStringCreateLocalized(buf);
		XtSetArg(args[n],XmNlabelString, xms); n++;
		XtSetArg(args[n],XmNmnemonic, NMNIC(i)); n++;
		XtSetArg(args[n],XmNsensitive, SENS(i)); n++;
		buttons->w[i] =
		XmCreatePushButtonGadget(owner, "button", args, n);
		if (buttons->items[i].cb){
		    XtAddCallback(buttons->w[i], XmNactivateCallback,
			    buttons->items[i].cb, buttons->items[i].clientdata);
		}
		XmStringFree(xms);
	    } else{
		n = 0;
		XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
		XtSetArg(args[n], XmNmargin, resource.menu_margin );  n++;
		buttons->w[i] = XmCreateSeparatorGadget(owner,
							"separater", args, n);
	    }
	    XtManageChild(buttons->w[i]);
	}
    }
#undef LABEL
#undef NMNIC
#undef SENS
}

static Atom
DeleteWindowAtom()
{
    static Atom delatom = 0;
    if (! delatom){
	delatom = XInternAtom(XtDisplayOfObject(toplevel),
		"WM_DELETE_WINDOW", False);
    }
    return(delatom);
}

/*ARGSUSED*/
Widget
#if __STDC__
CreateDialogAndButtons( Widget owner, String name, 
	void (*delcb)(), Button *btns, int btns_cnt, Widget *pop )
#else
CreateDialogAndButtons( owner, name, delcb, btns, btns_cnt, pop )
Widget owner;
String name;
void   (*delcb)();
Button *btns;
int    btns_cnt;
Widget *pop;
#endif
{
    int		n;
    Arg		args[32];
    Arg arg[8];
    Widget	rowcol;
    XmString	cs1, cs2, cs3;

    n = 0;
    XtSetArg( args[n], XmNautoUnmanage, resource. dia_tm_automng );	n++;
    XtSetArg( args[n], XmNmarginWidth, resource.dia_tm_width  );	n++;
    XtSetArg( args[n], XmNmarginHeight, resource.dia_tm_height  );	n++;
    if (btns->itemcnt > 0){
	cs1 = XmStringCreateLocalized(btns->items[0].label);
	XtSetArg(args[n], XmNokLabelString, cs1);  n++;
    }
    if (btns->itemcnt > 1){
	cs2 = XmStringCreateLocalized(btns->items[1].label);
	XtSetArg(args[n], XmNcancelLabelString, cs2);  n++;
    }
    if (btns->itemcnt > 2){
	cs3 = XmStringCreateLocalized(btns->items[2].label);
	XtSetArg(args[n], XmNhelpLabelString, cs3);  n++;
    }
    XtSetArg(args[n], XmNnoResize, resource.dia_tm_resize  );  n++;
    XtSetArg(args[n], XmNminimizeButtons, resource.dia_tm_minimize  );  n++;
    XtSetArg(args[n], XmNtitle, name  );  n++;
    *pop = XmCreateTemplateDialog(toplevel, "dialog", args, n);
    if (delcb)
	XmAddWMProtocolCallback(XtParent(*pop),
			DeleteWindowAtom(), delcb, 0);
    n = 0;
    XtSetArg( arg[n], XmNmarginWidth, resource.dia_rw_width );   n++;
    XtSetArg( arg[n], XmNmarginHeight, resource.dia_rw_height );  n++;
    XtSetArg( arg[n], XmNspacing, resource.dia_rw_spacing  ); n++;
    XtSetArg( arg[n], XmNorientation, XmVERTICAL);  n++;
    rowcol = XmCreateRowColumn( *pop, "RowCol", arg, n);
    XtManageChild(rowcol);
    if (btns->itemcnt > 0){
	if (! btns->items[0].cb)
	    XtAddCallback(*pop, XmNokCallback, _unmap, 0);
	XtAddCallback(*pop, XmNokCallback, btns->items[0].cb,
	  (btns->items[0].clientdata != NULL) ? btns->items[0].clientdata : 0);
    }
    if (btns->itemcnt > 1){
	if (! btns->items[1].cb)
	    XtAddCallback(*pop, XmNcancelCallback, _unmap, 0);
	XtAddCallback(*pop, XmNcancelCallback, btns->items[1].cb,
	  (btns->items[1].clientdata != NULL) ? btns->items[1].clientdata : 0);
    }
    if (btns->itemcnt > 2){
	if (! btns->items[2].cb)
	    XtAddCallback(*pop, XmNhelpCallback, _unmap, 0);
	XtAddCallback(*pop, XmNhelpCallback, btns->items[2].cb,
	  (btns->items[2].clientdata != NULL) ? btns->items[2].clientdata : 0);
    }
    n = 0;
    XtSetValues(*pop, args, n);
    if (btns->itemcnt == 1){
	XmStringFree(cs1);
    }
    else if (btns->itemcnt == 2){
	XmStringFree(cs1);
	XmStringFree(cs2);
    }
    else if (btns->itemcnt == 3){
	XmStringFree(cs1);
	XmStringFree(cs2);
	XmStringFree(cs3);
    }
    return(rowcol);
}


/* Initialize GUI */

Widget
GuiInitialize(app, class_name, ac, av)
XtAppContext *app;
String class_name;
int *ac;
String av[];
{
	Widget top;

	XtSetLanguageProc(NULL, NULL, NULL);
	_DtEnvControl(0);

	top = XtAppInitialize(app, class_name, NULL,0, ac,av, NULL,NULL,0);

	return(top);
}

Widget
CreateDrawingArea( owner, name, width, height, proc, val )
Widget owner;
String name;
int width;
int height;
void (*proc)();
int val;
{
	int n;
	Arg arg[16];
	Widget drawarea;

	n = 0;
	XtSetArg(arg[n], XmNwidth, width); n++;
	XtSetArg(arg[n], XmNheight, height); n++;
	XtSetArg(arg[n], XmNresizePolicy, XmRESIZE_NONE); n++;
	XtSetArg(arg[n], XmNborderWidth, resource.draw_border); n++;
	XtSetArg(arg[n], XmNbackground, resource.pane_background); n++;
	XtSetArg(arg[n], XmNtopAttachment, (XtArgVal)XmATTACH_FORM); n++;
	XtSetArg(arg[n], XmNleftAttachment, (XtArgVal)XmATTACH_FORM); n++;
	XtSetArg(arg[n], XmNtopOffset, (XtArgVal)resource.draw_topoff); n++;
	XtSetArg(arg[n], XmNleftOffset, (XtArgVal)resource.draw_lftoff); n++;
	drawarea = XmCreateDrawingArea(owner, name, arg, n);
	XtManageChild( drawarea );
	XtAddEventHandler(drawarea, ExposureMask, FALSE, proc, (XtPointer)val);
	return(drawarea);
}


#ifndef	USE_MACRO

void
AddLeftAttachWidget( w, ref, offset )
Widget w;
Widget ref;
int offset;
{
    XtVaSetValues( w,
    XmNleftAttachment, XmATTACH_WIDGET,
    XmNleftWidget, ref,
    XmNleftOffset, offset,
    NULL);
}

void
AddLeftAttachForm( w, offset )
Widget w;
int offset;
{
    XtVaSetValues( w,
    XmNleftAttachment, XmATTACH_FORM,
    XmNleftOffset, offset,
    NULL);
}

void
AddTopAttachWidget( w, ref, offset )
Widget w;
Widget ref;
int offset;
{
    XtVaSetValues( w,
    XmNtopAttachment, XmATTACH_WIDGET,
    XmNtopWidget, ref,
    XmNtopOffset, offset,
    NULL);
}

void
AddTopAttachForm( w, offset )
Widget w;
int offset;
{
    XtVaSetValues( w,
    XmNtopAttachment, XmATTACH_FORM,
    XmNtopOffset, offset,
    NULL);
}

void
AddRightAttachWidget( w, ref, offset )
Widget w;
Widget ref;
int offset;
{
    XtVaSetValues( w,
    XmNrightAttachment, XmATTACH_WIDGET,
    XmNrightWidget, ref,
    XmNrightOffset, offset,
    NULL);
}

void
AddRightAttachForm( w, offset )
Widget w;
int offset;
{
    XtVaSetValues( w,
    XmNrightAttachment, XmATTACH_FORM,
    XmNrightOffset, offset,
    NULL);
}

void
AddBottomAttachForm( w, offset )
Widget w;
int offset;
{
    XtVaSetValues( w,
    XmNbottomAttachment, XmATTACH_FORM,
    XmNbottomOffset, offset,
    NULL);
}
#endif	/* not USE_MACRO */

void
PopupDialog(w)
Widget w;
{
    if (! XtIsManaged(w))
	XtManageChild(w);
    else
	XRaiseWindow(XtDisplayOfObject(w), XtWindow(XtParent(w)));
}

void
PopdownDialog(w)
Widget w;
{
    if (XtIsManaged(w)){
	XtUnmanageChild(w);
    }
}

void
ForcePopdownDialog(w)
Widget w;
{
    if (XtIsManaged(w)){
	XtUnmanageChild(w);
    }
}

void
SetLabelString(w, str)
Widget w;
String str;
{
    XmString cs;
    cs = XmStringCreateLocalized(str);
    XtVaSetValues( w, XmNlabelString, cs, NULL);
    XmStringFree( cs );
}

void
SetFooterString(w, str)
Widget w;
String str;
{
    XmString cs;
    cs = XmStringCreateLocalized(str);
    XtVaSetValues( w, XmNlabelString, cs, NULL);
    XmStringFree( cs );
	XmUpdateDisplay(w);
}


/***********************************************************************
 create and pop up the confirmaiton window 
 **********************************************************************/

static Widget notice=NULL;

static void
format_str(st, charcnt , str)
Widget st;
int	charcnt ;
char *str;
{
	int i ;
	char *s, *p;
	static char buf[512];
	int lw;
	int len ;
	wchar_t wc ;
        XmString cs;

	if (! str || ! *str) 	return;
		
	for( p=str, s=buf, lw=0; *p != 0;  ){

		if ( (*p == '\n') || (charcnt <= lw) ){
			*s = '\n';	/* new line */
			lw = 0;
			s++;
		}
                if( (len = mbtowc( &wc, p, MB_CUR_MAX )) <= 0 ) break;
                for( i=0; i<len ; i++, s++, p++ )  *s = *p ;

                lw += wcwidth( wc );
	}
	*s = 0;

	cs = XmStringCreateLocalized(buf);
	XtVaSetValues(st, XmNlabelString, (XtArgVal)cs, (String)0 );
	XmStringFree(cs);
}

void _unmap()
{
	if (notice && XtIsManaged(notice)){
		XtUnmanageChild(notice);
	}
}

static void _destroy(w)
Widget w;
{
	if (w){
		XtDestroyWidget(w);
	}
	if (w == notice){
		notice = NULL;
	}
}


/*ARGSUSED*/
void
PopupNotice( owner, message, type, button, do_format, title )
Widget owner;
char *message;
unsigned char type;
NButton *button;
Boolean do_format;
String title;
{
    Widget	label, help, cancel;
    int		n;
    Arg		args[32];
    XmString cs1, cs2, cs3;

    n = 0;
    XtSetArg(args[n], XmNtitle, title ); n++;
    XtSetArg(args[n], XmNnoResize, resource.pop_resize  ); n++;
    XtSetArg(args[n], XmNminimizeButtons, resource.pop_minimize ); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdialogType, type);  n++;
    notice = XmCreateMessageDialog( toplevel , "PopupNotice", args, n);
    n = 0;
    if (button->itemcnt > 0){
	cs1 = XmStringCreateLocalized(button->items[0].label);
	XtSetArg(args[n], XmNokLabelString, cs1);  n++;
	if (button->items[0].cb)
	    XtAddCallback(notice, XmNokCallback, button->items[0].cb, NULL);
	cancel = XmMessageBoxGetChild(notice, XmDIALOG_CANCEL_BUTTON);
	help = XmMessageBoxGetChild(notice, XmDIALOG_HELP_BUTTON);
    }
    if (button->itemcnt > 1){
	cs2 = XmStringCreateLocalized(button->items[1].label);
	XtSetArg(args[n], XmNcancelLabelString, cs2);  n++;
	if (button->items[1].cb)
	    XtAddCallback(notice, XmNcancelCallback, button->items[1].cb, NULL);
    }
    if (button->itemcnt > 2){
	cs3 = XmStringCreateLocalized(button->items[2].label);
	XtSetArg(args[n], XmNhelpLabelString, cs3);  n++;
	if (button->items[2].cb)
	    XtAddCallback(notice, XmNhelpCallback, button->items[2].cb, NULL);
	XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)_unmap, NULL);
    }
    XtSetValues(notice, args, n);
    XtAddCallback(notice, XmNunmapCallback, (XtCallbackProc)_destroy, NULL);
    if (button->itemcnt == 1){
	XmStringFree(cs1);
	XtUnmanageChild(cancel);
	XtUnmanageChild(help);
    }
    else if (button->itemcnt == 2){
	XmStringFree(cs1);
	XmStringFree(cs2);
	XtUnmanageChild(help);
    }
    else if (button->itemcnt == 3){
	XmStringFree(cs1);
	XmStringFree(cs2);
	XmStringFree(cs3);
    }

    label = XmMessageBoxGetChild(notice, XmDIALOG_MESSAGE_LABEL);

    if (do_format)
	format_str(label, 52, message);
    else{
	cs1 = XmStringCreateLocalized(message);
	XtVaSetValues(notice,
	    XmNmessageString, (XtArgVal) cs1, (String) 0);
	XtVaSetValues(notice,
	    XmNmessageAlignment, XmALIGNMENT_CENTER, (String) 0);
	XmStringFree(cs1);
    }
    XtManageChild(notice);
    XBell(XtDisplayOfObject( toplevel ), 0);
}


/*ARGSUSED*/
static void
_layout_centerEH(w, clientdata)
Widget w;
XtPointer clientdata;
{
    Widget *child;
    int num;
    Dimension bbw, mw, sw, cw;


    XtVaGetValues(w, XmNchildren, &child, XmNnumChildren, &num, NULL);
    XtVaGetValues(XtParent(w),
	XmNwidth, &bbw,
	XmNmarginWidth, &mw,
	XmNshadowThickness, &sw,
	NULL);
    XtVaGetValues(child[0], XmNwidth, &cw, NULL);
    XtVaSetValues(w, XmNwidth, ((int)bbw-2*((int)mw+(int)sw)), NULL);
    XtVaSetValues(child[0], XmNx,
    			((((int)bbw-2*((int)mw+(int)sw))-(int)cw)/2), NULL);
}

/*ARGSUSED*/
Widget
CreateTemplateDialog( w, message, type, button, title, pop )
Widget w;
char *message;
unsigned char type;
NButton *button;
String title;
Widget *pop;
{
    int		n;
    Arg		args[32];
    XmString	cs, cs1=NULL, cs2=NULL, cs3=NULL;
    Widget	brtnb;

    n = 0;
    cs = XmStringCreateLocalized(message);
    XtSetArg(args[n], XmNnoResize, resource.temp_resize );  n++;
    XtSetArg(args[n], XmNminimizeButtons, resource.temp_minimize );  n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNmessageString, cs);  n++;
    XtSetArg(args[n], XmNmessageAlignment, XmALIGNMENT_CENTER);  n++;
    XtSetArg(args[n], XmNtitle, title ); n++;

    if (button->itemcnt > 0){
	cs1 = XmStringCreateLocalized(button->items[0].label);
	XtSetArg(args[n], XmNokLabelString, cs1);  n++;
    }
    if (button->itemcnt > 1){
	cs2 = XmStringCreateLocalized(button->items[1].label);
	XtSetArg(args[n], XmNcancelLabelString, cs2);  n++;
    }
    if (button->itemcnt > 2){
	cs3 = XmStringCreateLocalized(button->items[2].label);
	XtSetArg(args[n], XmNhelpLabelString, cs3);  n++;
    }
    *pop = XmCreateTemplateDialog(w, "TemplateDialog", args, n);
    if (button->itemcnt > 0 && button->items[0].cb)
	XtAddCallback(*pop, XmNokCallback, button->items[0].cb, NULL);
    if (button->itemcnt > 1 && button->items[1].cb)
	XtAddCallback(*pop, XmNcancelCallback, button->items[1].cb, NULL);
    if (button->itemcnt > 2 && button->items[2].cb)
	XtAddCallback(*pop, XmNhelpCallback, button->items[2].cb, NULL);

    XmStringFree(cs);
    if (cs1) XmStringFree(cs1);
    if (cs2) XmStringFree(cs2);
    if (cs3) XmStringFree(cs3);

    XtAddCallback(*pop, XmNunmapCallback, (XtCallbackProc)_destroy, NULL);
    n = 0;
    XtSetArg(args[n], XmNwidth, resource.temp_width );  n++;
    XtSetArg(args[n], XmNheight, resource.temp_height );  n++;
    brtnb = XmCreateBulletinBoard(*pop, "BulletinBo", args, n);
    XtManageChild(brtnb);
    XtAddEventHandler(brtnb,
	StructureNotifyMask, True, (XtEventHandler)_layout_centerEH, NULL);
    return(brtnb);
}




void
AddDeleteProc(w, delcb)
Widget w;
void (*delcb)();
{
	Atom del = DeleteWindowAtom();
	XmAddWMProtocols( w, &del, 1);
	XmAddWMProtocolCallback( w, del, delcb, NULL );
}

void
AddPopupProc(w, popupcb)
Widget w;
void (*popupcb)();
{
    XtAddCallback(XtParent(w), XmNpopupCallback, popupcb, 0);
}

void
AddDestroyProc(w, destroycb)
Widget w;
void (*destroycb)();
{
    XtAddCallback(XtParent(w), XmNdestroyCallback, destroycb, 0);
}

Widget
CreateMenuBarAndFooterMessageForm(owner, name, buttons, bcnt, pop, footer)
Widget owner;
String name;
MButton *buttons;
int bcnt;
Widget *pop;
Widget *footer;
{
    Widget menuBar, form;
    Widget footerFrame, footerForm, footerLabel, sep, casBtn, baseForm;
    XmString cs;
    char buf[64];
    Arg arg[20];
    int n, i;

    n = 0;
    XtSetArg( arg[n], XmNiconName, name ); n++;
    XtSetArg( arg[n], XmNdeleteResponse, XmUNMAP ); n++;
    XtSetArg( arg[n], XmNmwmFunctions,
    		(MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_CLOSE)); n++;
    *pop = XtCreatePopupShell(name, applicationShellWidgetClass, owner, arg, n);

    n = 0;
    XtSetArg( arg[n], XmNshowSeparator, True ); n++;
    form = XmCreateMainWindow( *pop, "MainW", arg, n);
    XtManageChild(form);

    n = 0;
    menuBar = XmCreateMenuBar( form, "menuBar", arg, n);
    XtManageChild(menuBar);

    n = 0;
    baseForm = XmCreateForm( form, "wgeForm", arg, n);
    XtManageChild(baseForm);

    for (i=0; i<bcnt; i++){
        n = 0;
	XtSetArg(arg[n], XmNtearOffModel, XmTEAR_OFF_ENABLED);  n++;
    	buttons->items[i].menu =
	XmCreatePulldownMenu(menuBar, "menu", arg, n);
	n = 0;
	if (strchr(buttons->items[i].label, buttons->items[i].mnemonic)){
	    sprintf(buf, "%s", buttons->items[i].label);
	}else{
	    strcpy(buf, buttons->items[i].label);
	    strcat(buf, "(");
	    strcat(buf, &(buttons->items[i].mnemonic));
	    strcat(buf, ")");
	}
	cs = XmStringCreateLocalized(buf);
	XtSetArg(arg[n],XmNmnemonic, buttons->items[i].mnemonic ); n++;
	XtSetArg(arg[n],XmNlabelString, cs ); n++;
	XtSetArg(arg[n],XmNsubMenuId,	buttons->items[i].menu ); n++;
	casBtn = XmCreateCascadeButton( menuBar, "casBtnA", arg, n);
	XtManageChild(casBtn);
	XmStringFree(cs);
    }

    n = 0;
    XtSetArg( arg[n], XmNshadowType,       XmSHADOW_IN ); n++;
    footerFrame = XmCreateFrame( form, "footerFrame", arg, n);
    XtManageChild(footerFrame);
    XtVaSetValues(form, XmNmessageWindow, footerFrame, NULL);

    n = 0;
    XtSetArg( arg[n], XmNtopAttachment,    XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    footerForm = XmCreateForm( footerFrame, "footerForm", arg, n);
    XtManageChild(footerForm);

    n = 0;
    cs = XmStringCreateLocalized(resource.message);
    XtSetArg( arg[n], XmNlabelString,      cs); n++;
    XtSetArg( arg[n], XmNtopAttachment,    XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNmarginTop,    resource.ed_msg_margintop );  n++;
    XtSetArg( arg[n], XmNmarginBottom, resource.ed_msg_marginbtm );  n++;
    footerLabel = XmCreateLabel(footerForm, "Message", arg,n);
    XtManageChild(footerLabel);
    XmStringFree(cs);

    n = 0;
    cs = XmStringCreateLocalized("");
    XtSetArg( arg[n], XmNtopAttachment,    XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg( arg[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
    XtSetArg( arg[n], XmNleftWidget,   footerLabel); n++;
    XtSetArg( arg[n], XmNalignment,    XmALIGNMENT_BEGINNING);  n++;
    XtSetArg( arg[n], XmNmarginTop,    resource.ed_mesg_margintop );  n++;
    XtSetArg( arg[n], XmNmarginBottom, resource.ed_mesg_marginbtm );  n++;
    XtSetArg( arg[n], XmNlabelString, cs);  n++;
    *footer = XmCreateLabelGadget(footerForm, "MessageG", arg, n);
    XtManageChild(*footer);
    XmStringFree(cs);

    return(baseForm);
}

Widget
GetMenuWidget( buttons, buttons_num )
MButton *buttons;
int buttons_num;
{
    return(buttons->items[buttons_num].menu);
}

Widget
CreateForm( owner, name )
Widget owner;
String name;
{
	Widget form;
	int n;
	Arg arg[8];

	n=0;
	form = XmCreateForm( owner, name, arg, n );
	XtManageChild(form);
	return(form);
}

/*ARGSUSED*/
Widget
CreateLabel( owner, name, str )
Widget owner;
String name;
String str;
{
	Widget label;
	Arg arg[2];
	int n=0;
	XmString cs = XmStringCreateLocalized(str);
	XtSetArg( arg[n], XmNlabelString, cs); n++;
	label = XmCreateLabel( owner, "label", arg, n);
	XtManageChild(label);
	return(label);
}


/*ARGSUSED*/
Widget
CreateFrame(owner, name, type, thickness)
Widget owner;
String name;
XtPointer type;
XtPointer thickness;
{
	Widget frame;
	Arg args[20];
	int n;

	n = 0;
	XtSetArg(args[n], XmNresizable, resource.frame_resize );  n++;
	XtSetArg(args[n], XmNshadowType, type);  n++;
	XtSetArg(args[n], XmNshadowThickness , thickness);  n++;
	frame = XmCreateFrame(owner, "frame", args, n);
	XtManageChild(frame);
	return(frame);
}

/*ARGSUSED*/
Widget
CreateRowColumn(owner, name, layout, space, marginw, marginh)
Widget owner;
String name;
int layout;
int space;
int marginw;
int marginh;
{
	Widget rc;
	Arg args[20];
	int n;

	n = 0;
	XtSetArg(args[n], XmNorientation, layout);  n++;
	XtSetArg(args[n], XmNspacing, space);  n++;
	XtSetArg(args[n], XmNmarginWidth, marginw);  n++;
	XtSetArg(args[n], XmNmarginHeight, marginh);  n++;
	rc = XmCreateRowColumn(owner, "rowColumn", args, n);
	XtManageChild(rc);
	return(rc);
}

/*ARGSUSED*/
Widget
CreateScrollBar( owner, name, height, val, min, max, proc )
Widget owner;
String name;
int height;
int val;
int min;
int max;
void (*proc)();
{
    Widget sc;
    int n;
    Arg arg[16];

    n = 0;
    XtSetArg( arg[n], XmNsliderSize, (XtArgVal)val );  n++;
    XtSetArg( arg[n], XmNpageIncrement, (XtArgVal)val );   n++;
    XtSetArg( arg[n], XmNmaximum,        (XtArgVal)max );        n++;

    sc = XmCreateScrollBar(owner, "wgeScro", arg, n);
    XtManageChild( sc );
    XtAddCallback( sc, XmNvalueChangedCallback, proc, NULL );
    return(sc);
}


/*ARGSUSED*/
static void
_scbase_cb( w, proc, calldata )
Widget w;
void (*proc)();
XmScrollBarCallbackStruct *calldata;
{
    (*proc)(calldata->value);
}

Widget
CreateScrollBase( owner, name, min, max, val, vcnt, sbproc )
Widget owner;
String name;
int min;
int max;
int val;
int vcnt;
void (*sbproc)();
{
    int n;
    Arg arg[16];
    Widget base, frame, rwclm, sc;

    n=0;
    XtSetArg( arg[n], XmNwidth, resource.scll_fr_width ); n++;
    base = XmCreateForm( owner, name, arg, n );
    XtManageChild( base );
    n=0;
    XtSetArg( arg[n], XmNshadowType, XmSHADOW_IN ); n++;
    XtSetArg( arg[n], XmNshadowThickness, resource.scll_fr_thick ); n++;
    XtSetArg( arg[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
    frame = XmCreateFrame( base, name, arg, n );
    XtManageChild( frame );
    n=0;
    rwclm = XmCreateRowColumn( frame, name, arg, n );
    XtManageChild( rwclm );
    if (vcnt < (max-min)){
	n=0;
	XtSetArg( arg[n], XmNsliderSize, vcnt ); n++;
	XtSetArg( arg[n], XmNminimum, min ); n++;
	XtSetArg( arg[n], XmNmaximum, max ); n++;
	XtSetArg( arg[n], XmNvalue, val ); n++;
	XtSetArg( arg[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
	XtSetArg( arg[n], XmNleftWidget, rwclm ); n++;
	XtSetArg( arg[n], XmNleftOffset, resource.scll_bar_lftoff ); n++;
	XtSetArg( arg[n], XmNtopAttachment, XmATTACH_FORM ); n++;
	XtSetArg( arg[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
	sc = XmCreateScrollBar( base, name, arg, n );
	XtManageChild( sc );
	XtAddCallback( sc, XmNvalueChangedCallback,
				(XtCallbackProc)_scbase_cb, (XtPointer)sbproc);
    }
    return( rwclm );
}
