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
/* cpyx.c 1.29 - Fujitsu source for CDEnext    96/10/30 13:13:45      */
/* $XConsortium: cpyx.c /main/8 1996/11/08 01:52:55 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <nl_types.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>

#include <Xm/XmAll.h>

/*
 * There is no public header file for this function (only an
 * internal header XmStringI.h).
 */
extern XtPointer _XmStringUngenerate (XmString string,
                        XmStringTag tag,
                        XmTextType tag_type,
                        XmTextType output_type);

#include "util.h"
#include "selectxlfd.h"
#include "xoakufont.h"

#include "xpm.h"
#ifdef XPM
#define ReadXpm XpmCreatePixmapFromData
#else
#define ReadXpm _DtXpmCreatePixmapFromData
#endif
#include "pixmaps/arrow.pm"

static Widget	CreateCopyXLFD(Widget Top);
static void	CpyXLFDCB(Widget w, caddr_t client_data, caddr_t *call_data);

extern	int	CreateOtherLabelAndText(int     num,
					Widget  owner,
					Widget  baseForm,
					Widget  *slctBLabel,
					Widget  topW,
					Widget  *slctBText,
					Widget  *slctButton);
extern	Widget	xlfdCreateScrolledList(Widget owner,
				       char *name,
				       Arg *args,
				       int n);
extern	int	GetItemsToDisplay(int num, int *itemcnt, XmString **xms_list);

extern	void	xlfdPopupDialog(Widget w);

extern	void	InitCpyPtn(void);
extern	void	_unmap(void);
extern	void	CpyCB(Widget w, XtPointer cdata);
extern	void	PopdownCpyPtn(void);
extern	void	AddPopupProc(Widget w, void(*popupcb)());

extern FalFontData fullFontData;

/*
 * parameters
 */

FalFontData     copyFontData;

extern  Widget	toplevel ;
Widget	cpyDialog=NULL;
static Widget codeLabel[COPYLABELS];
static Widget codeText[COPYLABELS+1];

extern	TextField SrcTf;
extern	TextField DestTf;

extern	char AreaStr[160];

/*
 *  resource database
 */
extern	Resource resource ;

#define CS0 "Codeset 0"
#define CS1 "Codeset 1"
#define CS2 "Codeset 2"
#define CS3 "Codeset 3"

#define FAL_ERROR_STR resource.falerrmsg[((fal_utyerrno & 0xff) > 25) ? 0 : (fal_utyerrno & 0xff)]

static Widget		pull1, pull2, pull3, pull4, scrolllist;
static int	xlf_count = 0;
static XmString *xlf=NULL;
static int	udc_count = 0;
static Boolean	udc_flag = False;
static int	*udc=NULL;
static int	udc_val;
static int	sty_count = 0;
static Boolean	sty_flag = False;
static char	**sty=NULL;
static char 	*sty_val=NULL;
static int	wls_count = 0;
static Boolean	wls_flag = False;
static int	*wls=NULL;
static int	wls_val;
static int	hls_count = 0;
static Boolean	hls_flag = False;
static int	*hls=NULL;
static int	hls_val;
static Widget       *button1=NULL;
static Widget       *button2=NULL;
static Widget       *button3=NULL;
static Widget       *button4=NULL;
static Widget       focus_widget=NULL;

/****************************************************************
 * callbacks							*
 ***************************************************************/


/*ARGSUSED*/
static void
CpyXLFDCB(Widget w, caddr_t client_data, caddr_t *call_data)
{
    char	*str;
    XmStringTable	st;
    /* get font file name (str:  XLFD name)	*/
    /* get font file name (str:  XLFD name)	*/
    XtVaGetValues(scrolllist, XmNselectedItems, &st, NULL);
    if( st == NULL ){
        copyFontData.xlfdname = NULL;
	return ;
    }
    str = _XmStringUngenerate(st[0], NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    copyFontData.xlfdname = str;
    if(udc_flag == True)
	copyFontData.cd_set = udc_val;
    else
	copyFontData.cd_set = -1;
}


void
CpyPtnXLFD(void)
{
    if( cpyDialog == NULL )
    {
	CreateCopyXLFD( toplevel ) ;

	/*
	 * add callbacks
	 */
	XtAddCallback(cpyDialog, XmNokCallback, (XtCallbackProc)_unmap, NULL);
	XtAddCallback(cpyDialog, XmNokCallback,
					(XtCallbackProc)CpyXLFDCB, NULL);
	XtAddCallback(cpyDialog, XmNokCallback, (XtCallbackProc)CpyCB, NULL);

	XtAddCallback(cpyDialog, XmNcancelCallback,
				(XtCallbackProc)_unmap, (XtPointer)1);
	XtAddCallback(cpyDialog, XmNcancelCallback,
				(XtCallbackProc)CpyXLFDCB,  (XtPointer)1);
	XtAddCallback(cpyDialog, XmNcancelCallback,
				(XtCallbackProc)CpyCB, (XtPointer)1);
	XtAddCallback(cpyDialog, XmNhelpCallback, (XtCallbackProc)_unmap, NULL);
	XtAddCallback(cpyDialog, XmNhelpCallback,
				(XtCallbackProc)PopdownCpyPtn, NULL);

	AddPopupProc( cpyDialog, InitCpyPtn ) ;

	SrcTf.w1  = codeText[0] ;
	SrcTf.w2  = codeText[1] ;
	DestTf.w1 = codeText[2] ;
	DestTf.w2 = NULL;

    }
    xlfdPopupDialog( cpyDialog );
}


static void
create_xlfd(void)
{
	int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
	FalFontData key;
	FalFontDataList *fontlist;
	FalFontData *f;
	int i;

	if (udc_flag == True) {
		key.cd_set = udc_val;
		mask |= FAL_FONT_MASK_CODE_SET;
	}
	if (sty_flag == True) {
		key.style.name = sty_val;
		mask |= FAL_FONT_MASK_STYLE_NAME;
	}
	if (wls_flag == True) {
		key.size.w = wls_val;
		mask |= FAL_FONT_MASK_SIZE_W;
	}
	if (hls_flag == True) {
		key.size.h = hls_val;
		mask |= FAL_FONT_MASK_SIZE_H;
	}
	xlf_count = 0;
	if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR) {
		return;
	}
	if(fontlist->num == 0) {
	        FalFreeFontList(fontlist);
		return;
	}
	xlf = (XmString *)XtMalloc(sizeof(XmString) * fontlist->num);
	for (i=0, f=fontlist->list; i < fontlist->num; i++, f++) {
	    xlf[xlf_count++] = XmStringCreateLocalized(f->xlfdname);
	}
	FalFreeFontList(fontlist);
}

static void
udc_call(Widget w)
{
	XmString label;
	char *moji;
	XtVaGetValues(w, XmNlabelString, &label, NULL);
	moji = _XmStringUngenerate(label, NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if(strncmp(moji, "*", 1) == 0) {
		udc_flag = False;
	} else if(strcmp(moji, CS0) == 0) {
		udc_val = FAL_FONT_CS0;
		udc_flag = True;
	} else if(strcmp(moji, CS1) == 0) {
		udc_val = FAL_FONT_CS1;
		udc_flag = True;
	} else if(strcmp(moji, CS2) == 0) {
		udc_val = FAL_FONT_CS2;
		udc_flag = True;
	} else if(strcmp(moji, CS3) == 0) {
		udc_val = FAL_FONT_CS3;
		udc_flag = True;
	} else {
		udc_flag = False;
	}
	XtFree(moji);
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
sty_call(Widget w)
{
	XmString label;
	char *moji;
	if (sty_val) {
		XtFree(sty_val);
		sty_val = NULL;
	}
	XtVaGetValues(w, XmNlabelString, &label, NULL);
	moji = _XmStringUngenerate(label, NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if(strncmp(moji, "*", 1) == 0) {
		sty_flag = False;
	}
	else {
		sty_val = XtMalloc(sizeof(char) * (strlen(moji) + 1));
		strcpy(sty_val, moji);
		sty_flag = True;
	}
	XtFree(moji);
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
wls_call(Widget w)
{
	XmString label;
	char *moji;
	XtVaGetValues(w, XmNlabelString, &label, NULL);
	moji = _XmStringUngenerate(label, NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if(strncmp(moji, "*", 1) == 0) {
		wls_flag = False;
	}
	else {
		wls_val = atoi(moji);
		wls_flag = True;
	}
	XmStringFree(label);
	XtFree(moji);
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
hls_call(Widget w)
{
	XmString label;
	char *moji;
	XtVaGetValues(w, XmNlabelString, &label, NULL);
	moji = _XmStringUngenerate(label, NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if(strncmp(moji, "*", 1) == 0) {
		hls_flag = False;
	}
	else {
		hls_val = atoi(moji);
		hls_flag = True;
	}
	XmStringFree(label);
	XtFree(moji);
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
button_set1(void)
{
    int		i, j;
    int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
    FalFontData key;
    FalFontDataList *fontlist;
    FalFontData *f;
    Boolean found;

    if (sty_flag == True) {
        key.style.name = sty_val;
        mask |= FAL_FONT_MASK_STYLE_NAME;
    }
    if (wls_flag == True) {
        key.size.w = wls_val;
        mask |= FAL_FONT_MASK_SIZE_W;
    }
    if (hls_flag == True) {
        key.size.h = hls_val;
        mask |= FAL_FONT_MASK_SIZE_H;
    }
    if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR) {
        for (j=0; j<udc_count; j++)
	    XtSetSensitive(button1[j], False);
	return;
    }
    if(fontlist->num == 0) {
	FalFreeFontList(fontlist);
        for (j=0; j<udc_count; j++)
	    XtSetSensitive(button1[j], False);
        return;
    }

    for (j=0; j<udc_count; j++) {
        for (i=0, f=fontlist->list, found=False; i < fontlist->num; i++, f++) {
            if(udc[j] == f->cd_set) {
		found = True;
                break;
            }
        }
	if(found == False)
	    XtSetSensitive(button1[j], False);
	else
	    XtSetSensitive(button1[j], True);
    }
    FalFreeFontList(fontlist);
}

static void
button_set2(void)
{
    int		i, j;
    int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
    FalFontData key;
    FalFontDataList *fontlist;
    FalFontData *f;
    Boolean found;

    if (udc_flag == True) {
        key.cd_set = udc_val;
        mask |= FAL_FONT_MASK_CODE_SET;
    }
    if (wls_flag == True) {
        key.size.w = wls_val;
        mask |= FAL_FONT_MASK_SIZE_W;
    }
    if (hls_flag == True) {
        key.size.h = hls_val;
        mask |= FAL_FONT_MASK_SIZE_H;
    }
    if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR) {
        for (j=0; j<sty_count; j++)
	    XtSetSensitive(button2[j], False);
	return;
    }
    if(fontlist->num == 0) {
        for (j=0; j<sty_count; j++)
	    XtSetSensitive(button2[j], False);
	FalFreeFontList(fontlist);
        return;
    }

    for (j=0; j<sty_count; j++) {
        for (i=0, f=fontlist->list, found=False; i < fontlist->num; i++, f++) {
	    if(strcmp(sty[j], f->style.name) == 0) {
		found = True;
	        break;
	    }
        }
	if(found == False)
	    XtSetSensitive(button2[j], False);
	else
	    XtSetSensitive(button2[j], True);
    }
    FalFreeFontList(fontlist);
}

static void
button_set3(void)
{
    int		i, j;
    int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
    FalFontData key;
    FalFontDataList *fontlist;
    FalFontData *f;
    Boolean found;

    if (udc_flag == True) {
        key.cd_set = udc_val;
        mask |= FAL_FONT_MASK_CODE_SET;
    }
    if (sty_flag == True) {
        key.style.name = sty_val;
        mask |= FAL_FONT_MASK_STYLE_NAME;
    }
    if (hls_flag == True) {
        key.size.h = hls_val;
        mask |= FAL_FONT_MASK_SIZE_H;
    }
    if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR) {
       for (j=0; j<wls_count; j++)
	    XtSetSensitive(button3[j], False);
	return;
    }
    if(fontlist->num == 0) {
       for (j=0; j<wls_count; j++)
	    XtSetSensitive(button3[j], False);
	FalFreeFontList(fontlist);
        return;
    }

    for (j=0; j<wls_count; j++) {
        for (i=0, f=fontlist->list, found=False; i < fontlist->num; i++, f++) {
            if(wls[j] == f->size.w) {
		found = True;
                break;
            }
        }
	if(found == False)
	    XtSetSensitive(button3[j], False);
	else
	    XtSetSensitive(button3[j], True);
    }
    FalFreeFontList(fontlist);
}

static void
button_set4(void)
{
    int		i, j;
    int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
    FalFontData key;
    FalFontDataList *fontlist;
    FalFontData *f;
    Boolean found;

    if (udc_flag == True) {
        key.cd_set = udc_val;
        mask |= FAL_FONT_MASK_CODE_SET;
    }
    if (sty_flag == True) {
        key.style.name = sty_val;
        mask |= FAL_FONT_MASK_STYLE_NAME;
    }
    if (wls_flag == True) {
        key.size.w = wls_val;
        mask |= FAL_FONT_MASK_SIZE_W;
    }
    if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR) {
        for (j=0; j<hls_count; j++)
	    XtSetSensitive(button4[j], False);
	return;
    }
    if(fontlist->num == 0) {
        for (j=0; j<hls_count; j++)
	    XtSetSensitive(button4[j], False);
	FalFreeFontList(fontlist);
        return;
    }

    for (j=0; j<hls_count; j++) {
        for (i=0, f=fontlist->list, found=False; i < fontlist->num; i++, f++) {
            if(hls[j] == f->size.h) {
                found = True;
                break;
            }
        }
        if (found == False)
	    XtSetSensitive(button4[j], False);
	else
	    XtSetSensitive(button4[j], True);
    }
    FalFreeFontList(fontlist);
}


static void
font_init(void)
{
	FalFontDataList *fontlist;
	FalFontData	*f;
	Boolean		found;
	int		i, j;
	char		tmp[16];
	char		err[128];
	Widget		button;
	extern void	data_sort();
	extern void	Error_message();

	xlf_count = udc_count = sty_count = wls_count = hls_count = 0;
	if (FalGetFontList(NULL, FAL_FONT_MASK_DEFINED |
			FAL_FONT_MASK_UNDEFINED, &fontlist) == FAL_ERROR) {
		snprintf(err, sizeof(err), "%s", FAL_ERROR_STR);
		Error_message((Widget)NULL, err);
		return;
	}
	if(fontlist->num == 0) {
	        FalFreeFontList(fontlist);
		snprintf(err, sizeof(err), "%s", resource.mn_no_font);
		Error_message((Widget)NULL, err);
		return;
	}
	xlf = (XmString *)XtMalloc(sizeof(XmString) * fontlist->num);
	udc = (int *)XtMalloc(sizeof(int) * fontlist->num);
	sty = (char **)XtMalloc(sizeof(char *) * fontlist->num);
	wls = (int *)XtMalloc(sizeof(int) * fontlist->num);
	hls = (int *)XtMalloc(sizeof(int) * fontlist->num);
	for (i=0, f=fontlist->list; i < fontlist->num; i++, f++) {
	    xlf[xlf_count++] = XmStringCreateLocalized(f->xlfdname);
	    for (j=0,found=False; j<udc_count; j++) {
		if(udc[j] == f->cd_set) {
			found=True;
			break;
		}
	    }
	    if (found == False) {
		udc[udc_count++] = f->cd_set;
	    }
	    for (j=0,found=False; j<sty_count; j++) {
		if(strcmp(sty[j], f->style.name) == 0) {
			found=True;
			break;
		}
	    }
	    if (found == False) {
		sty[sty_count] = XtMalloc(sizeof(char) * (strlen(f->style.name) + 1));
		strcpy(sty[sty_count++], f->style.name);
	    }
	    if (f->size.w != -1) {
	    for (j=0,found=False; j<wls_count; j++) {
		if(wls[j] == f->size.w) {
			found=True;
			break;
		}
	    }
	    if (found == False) {
		wls[wls_count++] = f->size.w;
	    }
	    }
	    for (j=0,found=False; j<hls_count; j++) {
		if(hls[j] == f->size.h) {
			found=True;
			break;
		}
	    }
	    if (found == False) {
		hls[hls_count++] = f->size.h;
	    }
	}
	FalFreeFontList(fontlist);

	data_sort(udc, udc_count);
	data_sort(wls, wls_count);
	data_sort(hls, hls_count);

	button1 = (Widget *) XtMalloc(sizeof(Widget) * udc_count);
	button2 = (Widget *) XtMalloc(sizeof(Widget) * sty_count);
	button3 = (Widget *) XtMalloc(sizeof(Widget) * wls_count);
	button4 = (Widget *) XtMalloc(sizeof(Widget) * hls_count);

	button = XmCreatePushButton(pull1, "*", NULL, 0);
	XtManageChild(button);
	XtAddCallback(button, XmNactivateCallback,
						(XtCallbackProc)udc_call, NULL);
	for (i=0; i < udc_count; i++) {
		if(udc[i] == FAL_FONT_CS0)
			snprintf(tmp, sizeof(tmp), "%s", CS0);
		else if(udc[i] == FAL_FONT_CS1)
			snprintf(tmp, sizeof(tmp), "%s", CS1);
		else if(udc[i] == FAL_FONT_CS2)
			snprintf(tmp, sizeof(tmp), "%s", CS2);
		else if(udc[i] == FAL_FONT_CS3)
			snprintf(tmp, sizeof(tmp), "%s", CS3);
		else
			snprintf(tmp, sizeof(tmp), "Codeset %x?", udc[i]);
		button1[i] = XmCreatePushButton(pull1, tmp, NULL, 0);
		XtManageChild(button1[i]);
		XtAddCallback(button1[i], XmNactivateCallback,
						(XtCallbackProc)udc_call, NULL);
	}

	button = XmCreatePushButton(pull2, "*", NULL, 0);
	XtManageChild(button);
	XtAddCallback(button, XmNactivateCallback,
						(XtCallbackProc)sty_call, NULL);
	for (i=0; i < sty_count; i++) {
		button2[i] = XmCreatePushButton(pull2, sty[i], NULL, 0);
		XtManageChild(button2[i]);
		XtAddCallback(button2[i], XmNactivateCallback,
						(XtCallbackProc)sty_call, NULL);
	}

	button = XmCreatePushButton(pull3, "*", NULL, 0);
	XtManageChild(button);
	XtAddCallback(button, XmNactivateCallback,
						(XtCallbackProc)wls_call, NULL);
	for (i=0; i < wls_count; i++) {
		snprintf(tmp, sizeof(tmp), "%d", wls[i]);
		button3[i] = XmCreatePushButton(pull3, tmp, NULL, 0);
		XtManageChild(button3[i]);
		XtAddCallback(button3[i], XmNactivateCallback,
						(XtCallbackProc)wls_call, NULL);
	}

	button = XmCreatePushButton(pull4, "*", NULL, 0);
	XtManageChild(button);
	XtAddCallback(button, XmNactivateCallback,
						(XtCallbackProc)hls_call, NULL);
	for (i=0; i < hls_count; i++) {
		snprintf(tmp, sizeof(tmp), "%d", hls[i]);
		button4[i] = XmCreatePushButton(pull4, tmp, NULL, 0);
		XtManageChild(button4[i]);
		XtAddCallback(button4[i], XmNactivateCallback,
						(XtCallbackProc)hls_call, NULL);
	}
}

/*ARGSUSED*/
static void
arrow_change(Widget w, Widget wid)
{
    if (XtIsSensitive(wid)) {
	XtSetSensitive(wid, False);
    } else {
	XtSetSensitive(wid, True);
    }
}

static void
focus(Widget w)
{
    focus_widget = w;
}

static void
code_input(void)
{
    char	*str;
    XmStringTable	st;
    extern void	CodeWindow();
    XtVaGetValues(scrolllist, XmNselectedItems, &st, NULL);
    if( st == NULL ){
	return ;
    }
    str = _XmStringUngenerate(st[0], NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    CodeWindow(focus_widget, str, True);
}

static void
code_input2(void)
{
    extern void	CodeWindow();
    CodeWindow(codeText[2], fullFontData.xlfdname, False);
}

static Widget
CreateCopyXLFD( Widget top )
{

	int		n;
	Arg		args[16];
	XmString	xms, xms1, xms2 ;
        Widget          frame, row, label1, row1, row2, row3, row4, row5;
	Widget		sep, cas1, cas2, cas3, cas4;
	Widget		arrow, code;
	XmString        str;
	extern Pixmap	arrow_pix;
	Display             *disp;
	Window              root;
	Pixmap              mask;
	XpmAttributes       attr;

	udc_flag = sty_flag = wls_flag = hls_flag = False;

        /*
	*  create base window
	*/
	n = 0 ;
	XtSetArg( args[n], XmNautoUnmanage,	False ) ;        n++ ;
	xms = XmStringCreateLocalized( resource.l_copy ) ;
	XtSetArg( args[n], XmNokLabelString,	xms ) ;		n++ ;
	xms1 = XmStringCreateLocalized( resource.l_overlay ) ;
	XtSetArg( args[n], XmNcancelLabelString, xms1 ) ;	n++ ;
	xms2 = XmStringCreateLocalized( resource.quit_label) ;
	XtSetArg( args[n], XmNhelpLabelString,	xms2 ) ;        n++ ;
	XtSetArg( args[n], XmNnoResize,		True ) ;	n++ ;
	XtSetArg( args[n], XmNminimizeButtons,	True ) ;        n++ ;
	XtSetArg( args[n], XmNtitle,	resource.l_copy_title) ;        n++ ;
	cpyDialog = XmCreateTemplateDialog( top, "copy_dialog", args, n );

	XmStringFree( xms ) ;
	XmStringFree( xms1 ) ;
	XmStringFree( xms2 ) ;

	n = 0;
	pull1 = XmCreatePulldownMenu(top, "pull", args, n);
	pull2 = XmCreatePulldownMenu(top, "pull", args, n);
	pull3 = XmCreatePulldownMenu(top, "pull", args, n);
	pull4 = XmCreatePulldownMenu(top, "pull", args, n);

	n = 0 ;
	row  = XmCreateRowColumn( cpyDialog, "BaseForm", args, n );
	XtManageChild( row );

	n = 0;
	frame = XmCreateFrame( row, "frame", args, n);
	XtManageChild( frame );

	n = 0 ;
	row1  = XmCreateRowColumn( frame, "row", args, n );
	XtManageChild( row1 );

	n = 0;
	str = XmStringCreateLocalized(resource.l_selectitem) ;
	XtSetArg(args[n], XmNlabelString, str);  n++;
	label1 = XmCreateLabel( row1, "SelectItems", args, n);
	XtManageChild( label1 );
	XmStringFree(str);

	n = 0 ;
	row2  = XmCreateRowColumn( row1, "row", args, n );
	XtManageChild( row2 );

	n = 0 ;
	str = XmStringCreateLocalized(resource.l_codeset) ;
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull1);  n++;
	cas1  = XmCreateOptionMenu( row2, "CodeSet", args, n );
	XtManageChild( cas1 );
	XtAddCallback(XmOptionButtonGadget(cas1), XmNcascadingCallback,
					(XtCallbackProc)button_set1, NULL);
	XmStringFree(str);

	n = 0 ;
	str = XmStringCreateLocalized(resource.l_style) ;
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull2);  n++;
	cas2  = XmCreateOptionMenu( row2, "Style", args, n );
	XtAddCallback(XmOptionButtonGadget(cas2), XmNcascadingCallback,
					(XtCallbackProc)button_set2, NULL);
	XtManageChild( cas2 );
	XmStringFree(str);

	n = 0 ;
	str = XmStringCreateLocalized(resource.l_width) ;
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull3);  n++;
	cas3  = XmCreateOptionMenu( row2, "Width", args, n );
	XtManageChild( cas3 );
	XtAddCallback(XmOptionButtonGadget(cas3), XmNcascadingCallback,
					(XtCallbackProc)button_set3, NULL);
	XmStringFree(str);

	n = 0 ;
	str = XmStringCreateLocalized(resource.l_height) ;
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull4);  n++;
	cas4  = XmCreateOptionMenu( row2, "Height", args, n );
	XtManageChild( cas4 );
	XtAddCallback(XmOptionButtonGadget(cas4), XmNcascadingCallback,
					(XtCallbackProc)button_set4, NULL);
	XmStringFree(str);

	font_init();

	n = 0;
        XtSetArg(args[n], XmNvisibleItemCount, 10) ;	n++ ;
        XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT) ;	n++ ;
        XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC) ;	n++ ;
        XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT) ;	n++ ;
        XtSetArg(args[n], XmNitems, xlf) ;	n++ ;
        XtSetArg(args[n], XmNitemCount, xlf_count) ;	n++ ;
	scrolllist = XmCreateScrolledList(row, "scrolllist", args, n);
	XtManageChild(scrolllist);

	n = 0;
	sep = XmCreateSeparator(row, "sep", args, n);
	XtManageChild(sep);

	n = 0 ;
	row3  = XmCreateRowColumn( row, "row", args, n );
	XtManageChild( row3 );

	if (! arrow_pix) {
	    disp = XtDisplay(row);
	    root = DefaultRootWindow(disp);
	    attr.valuemask = 0;
	    ReadXpm(disp, root, arrow_pm, &arrow_pix, &mask, &attr);
	}

	n = 0 ;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL) ;	n++ ;
	row4  = XmCreateRowColumn( row3, "row", args, n );
	XtManageChild( row4 );

	xms = XmStringCreateLocalized(resource.l_copy_src_code);
	n = 0;
	XtSetArg(args[n], XmNlabelString, xms);	n++ ;
	codeLabel[0] = XmCreateLabel(row4 , "copylabel", args, n);
	XtManageChild(codeLabel[0]);
	XmStringFree( xms ) ;

	n = 0;
	XtSetArg(args[n], XmNcolumns,	4);			n++;
	XtSetArg(args[n], XmNmaxLength,	4);			n++;
	XtSetArg(args[n], XmNeditable,	True);			n++;
	XtSetArg(args[n], XmNcursorPositionVisible, True);	n++;
	focus_widget = codeText[0] = XmCreateText(row4 , "copytext",
								args, n);
	XtManageChild(codeText[0]);

	XtAddCallback(codeText[0], XmNfocusCallback, (XtCallbackProc)focus, NULL);

	n = 0;
	XtSetArg(args[n], XmNlabelPixmap, arrow_pix);  n++;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg(args[n], XmNindicatorOn, False); n++;
	XtSetArg(args[n], XmNshadowThickness, 2); n++;
	XtSetArg(args[n], XmNfillOnSelect, False); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	arrow = XmCreateToggleButton(row4, "arrow", args, n);
	XtManageChild(arrow);

	n = 0;
	XtSetArg(args[n], XmNcolumns,	4);			n++;
	XtSetArg(args[n], XmNmaxLength,	4);			n++;
	XtSetArg(args[n], XmNeditable,	True);			n++;
	XtSetArg(args[n], XmNcursorPositionVisible, True);	n++;
	codeText[1] = XmCreateText(row4 , "copytext", args, n);
	XtManageChild(codeText[1]);
	XtSetSensitive(codeText[1], False);

	XtAddCallback(codeText[1], XmNfocusCallback, (XtCallbackProc)focus, NULL);

	XtAddCallback(arrow, XmNvalueChangedCallback,
			(XtCallbackProc)arrow_change, (XtPointer)codeText[1]);

	n = 0;
	xms = XmStringCreateLocalized(resource.code) ;
	XtSetArg(args[n], XmNlabelString, xms); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	code = XmCreatePushButton(row4, "Code", args, n);
	XtManageChild(code);
	XmStringFree(xms);

	XtAddCallback(code, XmNactivateCallback,
			(XtCallbackProc)code_input, (XtPointer)NULL);

	n = 0 ;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL) ;	n++ ;
	row5  = XmCreateRowColumn( row3, "row", args, n );
	XtManageChild( row5 );

        xms = XmStringCreateLocalized(resource.l_copy_dest_code);

	n = 0;
	XtSetArg(args[n], XmNlabelString, xms);	n++ ;
	codeLabel[1] = XmCreateLabel(row5 , "copylabel", args, n);
	XtManageChild(codeLabel[1]);
	XmStringFree( xms ) ;

	n = 0;
	XtSetArg(args[n], XmNcolumns,	4);			n++;
	XtSetArg(args[n], XmNmaxLength,	4);			n++;
	XtSetArg(args[n], XmNeditable,	True);			n++;
	XtSetArg(args[n], XmNcursorPositionVisible, True);	n++;
	codeText[2] = XmCreateText(row5 , "copytext", args, n);
	XtManageChild(codeText[2]);

	n = 0;
	xms = XmStringCreateLocalized(resource.code) ;
	XtSetArg(args[n], XmNlabelString, xms); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	code = XmCreatePushButton(row5, "Code", args, n);
	XtManageChild(code);
	XmStringFree(xms);

	XtAddCallback(code, XmNactivateCallback,
			(XtCallbackProc)code_input2, (XtPointer)NULL);

	return( cpyDialog ) ;
}
