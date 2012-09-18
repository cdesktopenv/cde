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
/* selectx.c 1.12 - Fujitsu source for CDEnext    96/03/01 09:34:52      */
/* $TOG: selectx.c /main/5 1997/07/23 17:24:39 samborn $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h> /* dtex add */
#include <math.h> /* dtex add */
#include <nl_types.h>

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
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>


#include "xoakufont.h"
#include "selectxlfd.h"
#include "excutil.h" /* dtex add */

/*
 * There is no public header file for this function (only an
 * internal header XmStringI.h).
 */
extern XtPointer _XmStringUngenerate (XmString string,
                        XmStringTag tag,
                        XmTextType tag_type,
                        XmTextType output_type);

/*extern Resource	resource ; dtex del*/
/* dtex add */
extern Exc_data *ed;
extern ListData *ld;
FalGlyphRegion  *gr;
int num_gr;
FalFontID		fid;
extern char *maintitle;
extern char **errmsg_org;

/*
 * parameters
 */

FalFontData fullFontData;

void	PopupSelectXLFD() ;
static Widget	CreateSelectXLFD() ;

/*extern	void	xlfdPopupDialog() ; dtex del*/
extern	int	GetListsOfItems() ;

/* extern	void	ReadCB(); dtex del */

Widget	xlfdDialog;
static Widget xlfdWform;

#define CS0 "Codeset 0"
#define CS1 "Codeset 1"
#define CS2 "Codeset 2"
#define CS3 "Codeset 3"

#define ERROR_1 errmsg_org[fal_utyerrno & 0xff]
/*#define ERROR_1 fal_errmsg_org[fal_utyerrno & 0xff]*/
/*#define ERROR_2 fal_errmsg_func[fal_utyerrno >>8]*/

static Widget	pull1, pull2, pull3, pull4, scrolllist;
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

/*
 * util dtex
 */

void
xlfdPopupDialog(w)
Widget w;
{
    if (! XtIsManaged(w))
        XtManageChild(w);
    else
        XRaiseWindow(XtDisplayOfObject(w), XtWindow(XtParent(w)));
}

void
ForcePopdownDialog(w)
Widget w;
{
    if (XtIsManaged(w)){
	XtUnmanageChild(w);
    }
}

int fontcheck(char *mode)
/*
 * If fontfile can't be opend, return -1.
 * If fontfile is editted by other UDC client, return 1.
 * normary return 0.
 */
{
    int mask;
    FalFontDataList	*fulllist = NULL;
    FalFontID		fontid;

    mask =  FAL_FONT_MASK_XLFDNAME | FAL_FONT_MASK_GLYPH_INDEX |
	    FAL_FONT_MASK_DEFINED |
	    FAL_FONT_MASK_UNDEFINED | FAL_FONT_MASK_CODE_SET;

    if (strcmp(mode, "w") == 0)
	mask |= FAL_FONT_MASK_UPDATE;

    fontid = FalOpenSysFont(&fullFontData, mask, &fulllist);
    if (fontid == 0) {
	if ((fal_utyerrno & 0xff) == 0x10)/*_FAL_ERR_LCKD*/
	    return (1);
	return (-1);
    }
    
    FalCloseFont( fontid );
    return (0);
}

void setallcode(ListData *ld)
{
    int		i, code, codenum;
    int		*codep;
    int		code1, code2;

    codenum = 0;
    for (i = 0; i < num_gr; i++) {
	codenum += (abs(gr[i].end - gr[i].start) + 1);
    }

    ld->allcode = (int *) calloc(codenum, sizeof(int));
    ld->allcode_num = codenum;
    codep = ld->allcode;
    for (i = 0; i < num_gr; i++) {
	code1 = smaller(gr[i].start, gr[i].end);
	code2 = bigger(gr[i].start, gr[i].end);
	for (code = code1; code <= code2; code++) {
	    *codep = code;
	    codep++;
	}
    }
}

FalFontID openfont()
{
    int mask;
    FalFontDataList	*fulllist = NULL;
    FalFontID		fontid;
    char			*fullpath=NULL;

    mask =  FAL_FONT_MASK_XLFDNAME | FAL_FONT_MASK_GLYPH_INDEX |
	    FAL_FONT_MASK_DEFINED |
	    FAL_FONT_MASK_UNDEFINED | FAL_FONT_MASK_CODE_SET;
    fontid = FalOpenSysFont(&fullFontData, mask, &fulllist);
    if (fontid == 0) {
	return ( 0 );
    }
    if (fullpath != NULL)
	FalFree(fullpath);
    if (FalFontIDToFileName(fontid, &fullpath) < 0){
	FalCloseFont(fontid);
	fontid = NULL;
    }
    if ((ed->fontfile = strdup(fullpath)) == NULL) {	    
	exit (-1);
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

    ld->existcode = (int *) calloc(ld->allcode_num, sizeof(int));
    existcodep = ld->existcode ;
    allcodep = ld->allcode;
    existcode_num = 0;
    for (i = 0; i < ld->allcode_num; i++) {
	code = *allcodep;
	pattern = FalReadFont(fid, code, 0, 0);
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
	    buf[i+1] = 0;
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
    *cp = 0;
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
 * callbacks							*
 ***************************************************************/

void quit(w, client_data, call_data) /* dtex */
Widget	w;
caddr_t	*client_data, *call_data;
{
    excterminate(ed);
}

static char *
char_set(char *str) /* dtex */
{
    int i, count;
    char *p;
    for (i=strlen(str),p=&(str[i]),count=0; i && count < 2; i--,p--) {
	if (*p == '-')
	    count ++;
    }
    if (count == 2)
	return(p + 2);
    else
	return(str);
}

static char *
spc(str, ch, count)
char *	str;
char	ch;
int	count;
{
    char *p;
    p = str + strlen(str);
    for(;count && (str < p);p--) {
	if (*p == ch)
	    count --;
    }
    if (! count)
	return(p+1);
    else
	return(NULL);
}

static void OpenWindowCB() /* dtex change */
{
    char	*str, *p;
    XmStringTable	st;
    /*dtex add */
    int r;
    int		ans;
    char	*msg;
    char	*msg2;
    char	*msg3;
    char	err[128];
    extern void selcharcd();
    char *locale;
    int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED
	| FAL_FONT_MASK_XLFDNAME;
    FalFontData key;
    FalFontDataList *fontlist;
    FalFontData *f;

    msg = GETMESSAGE(4, 2, "This font has no user defined characters.");
    msg2 = GETMESSAGE(4, 4, "Failed to open the selected font. You have no right to access for the font file, or the format of the file is not consistent.");
    msg3 = GETMESSAGE(4, 6, "The font file is used by other UDC client.");

    XtVaGetValues(scrolllist, XmNselectedItems, &st, NULL);
    if( st == NULL ){
	return ;
    }
    str = (char *) _XmStringUngenerate((XmString) st[0],
				 NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    if ( str == NULL ) {
	return;
    }

    p = spc(str, '-', 4);
    p++;

    if (*p == 'p' || *p == 'P') {
	str = NULL;
    }

    fullFontData.xlfdname = str;
    if ((ed->xlfdname = strdup(str)) == NULL) {
	exit (-1);
    }
    if(udc_flag == True)
	fullFontData.cd_set = udc_val;
    else { /* dtex change */
	memset(&key, 0x00, sizeof(FalFontData));
	key.xlfdname = fullFontData.xlfdname;
	if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR)
	    return;
	if (fontlist->num != 1)
	    return;
	f = fontlist->list;
	/*fullFontData.cd_set = -1;*/
	fullFontData.cd_set = f->cd_set;
	FalFreeFontList(fontlist);
    }

    if (ld != NULL) {
	freeld(ld);
    }

    if ((ld = (ListData *) malloc(sizeof(ListData))) == NULL) {
	excerror(ed, EXCERRMALLOC, "selcharcd", "exit");
    }
    memset(ld, 0x00, sizeof(ListData));
    ld->ed = ed;
    locale = getenv("LANG");
    if (locale == NULL)
	locale = "C";
    if (FalGetUDCGIArea(locale, fullFontData.cd_set,
			char_set(fullFontData.xlfdname), &gr, &num_gr)
	== FAL_ERROR) {
	    fprintf(stderr, "error in FalGetUDCGIArea\n");
	    sprintf(err, "%s", ERROR_1);
	    AskUser(ed->toplevel, ed, err, &ans, "error");
	    return;
    }
    if (ed->function == EXPORT) {
	r = fontcheck("r");
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
	selcharcd(ld->ed);
    } else if (ed->function == IMPORT) {
	r = fontcheck("w");
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

/**
 **  contents : "Cancel" button callback 
 ** ------------------------
 **  
 ** 
 **/

/*ARGSUSED*/
static void
OpenCancelCB( widget, clientData, callData ) /* dtex change */
Widget		widget;
caddr_t		clientData;
caddr_t		callData;
{
/*    extern void ForcePopdownDialog();
    if ( !editPtnW ){
	exit( 0 );
    } dtex del */
    ForcePopdownDialog(xlfdDialog);
}


/*
* create selection window view
*/
void
PopupSelectXLFD( top )
Widget	top ;
{

    if( xlfdDialog == NULL ){
	if( (xlfdDialog = CreateSelectXLFD( top )) == NULL ){
	    exit( -1 ) ;
	}
    }
    /* pop up select window */
    xlfdPopupDialog( xlfdDialog );
}


static void
create_xlfd()
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
	if (xlf) {
	    for (i=0; i < xlf_count; i++) {
		XmStringFree(xlf[i]);
	    }
	    XtFree((char *)xlf);
	}
	xlf = (XmString *)XtMalloc(sizeof(XmString) * fontlist->num);
	for (i=0, f=fontlist->list; i < fontlist->num; i++, f++) {
	    xlf[xlf_count++] = XmStringCreateLocalized(f->xlfdname);
	}
	FalFreeFontList(fontlist);
}

static void
udc_call(w)
Widget w;
{
	XmString label;
	char *moji;

	XtVaGetValues(w, XmNlabelString, &label, NULL);

	moji = (char *) _XmStringUngenerate((XmString) label, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (moji) {
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
	} else {
		      udc_flag = False;
	}
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
sty_call(w)
Widget w;
{
	XmString label;
	char *moji;
	if (sty_val) {
		XtFree(sty_val);
		sty_val = NULL;
	}
	XtVaGetValues(w, XmNlabelString, &label, NULL);

	moji = (char *) _XmStringUngenerate((XmString) label, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (moji) {
	      if(strncmp(moji, "*", 1) == 0) {
		      sty_flag = False;
	      }
	      else {
		      sty_val = XtMalloc(sizeof(char) * (strlen(moji) + 1));
		      strcpy(sty_val, moji);
		      sty_flag = True;
	      }
	      XtFree(moji);
	}

	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
wls_call(w)
Widget w;
{
	XmString label;
	char *moji;

	XtVaGetValues(w, XmNlabelString, &label, NULL);

	moji = (char *) _XmStringUngenerate((XmString) label, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (moji) {
	      if(strncmp(moji, "*", 1) == 0) {
		      wls_flag = False;
	      }
	      else {
		      wls_val = atoi(moji);
		      wls_flag = True;
	      }
	      XtFree(moji);
	}

	XmStringFree(label);
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
hls_call(w)
Widget w;
{
	XmString label;
	char *moji;
	XtVaGetValues(w, XmNlabelString, &label, NULL);

	moji = (char *) _XmStringUngenerate((XmString) label, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (moji) {
	      if(strncmp(moji, "*", 1) == 0) {
		      hls_flag = False;
	      }
	      else {
		      hls_val = atoi(moji);
		      hls_flag = True;
	      }
	      XtFree(moji);
	}
	XmStringFree(label);
	create_xlfd();
	XtVaSetValues(scrolllist, XmNitems, xlf, XmNitemCount, xlf_count, NULL);
}

static void
button_set1()
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
        for (j=0; j<udc_count; j++)
	    XtSetSensitive(button1[j], False);
        FalFreeFontList(fontlist);
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
button_set2()
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
button_set3()
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
button_set4()
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

void
data_sort(data, count)
int *data;
int count;
{
    int *p1, *p2, tmp, i;

    for (; count; count--) {
        for (i=1, p1=data, p2=data+1; i < count; i++, p1++, p2++) {
	    if( *p1 > *p2) {
		tmp = *p2;
		*p2 = *p1;
		*p1 = tmp;
	    }
	}
    }
}


static void
font_init() /* dtex change */
{
	FalFontDataList *fontlist;
	FalFontData	*f;
	Boolean		found;
	int		i, j;
	char		tmp[16];
	char		err[128];
	Widget		button;
	int             ans; /* dtex add */
/*	extern void Error_message(); dtex del */
	char *p;

	p = GETMESSAGE(4, 8, "There is no font");
	xlf_count = udc_count = sty_count = wls_count = hls_count = 0;
	if (FalGetFontList(NULL, FAL_FONT_MASK_DEFINED |
	    		FAL_FONT_MASK_UNDEFINED, &fontlist) == FAL_ERROR) {
		sprintf(err, "%s", ERROR_1);
/*		Error_message((Widget)NULL, err); dtex del */
		AskUser(ed->toplevel, ed, err, &ans, "error");
		return;
	}
	if(fontlist->num == 0) {
	        FalFreeFontList(fontlist);
/*		strcpy(err, resource.mn_no_font); dtex change */
		strcpy(err, p);
/*		Error_message((Widget)NULL, err); dtex del */
		AskUser(ed->toplevel, ed, err, &ans, "error");
		return;
	}
	udc = (int *)XtMalloc(sizeof(int) * fontlist->num);
	sty = (char **)XtMalloc(sizeof(char *) * fontlist->num);
	wls = (int *)XtMalloc(sizeof(int) * fontlist->num);
	hls = (int *)XtMalloc(sizeof(int) * fontlist->num);
	for (i=0, f=fontlist->list; i < fontlist->num; i++, f++) {
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

	if (udc_count > 0) {
	  button1 = (Widget *) XtMalloc(sizeof(Widget) * udc_count);
	}
	if (sty_count > 0) { 
	  button2 = (Widget *) XtMalloc(sizeof(Widget) * sty_count);
	}
	if (wls_count > 0) {
	  button3 = (Widget *) XtMalloc(sizeof(Widget) * wls_count);
	}
	if (hls_count > 0) {
	  button4 = (Widget *) XtMalloc(sizeof(Widget) * hls_count);
	}

	button = XmCreatePushButton(pull1, "*", NULL, 0);
	XtManageChild(button);
	XtAddCallback(button, XmNactivateCallback,
						(XtCallbackProc)udc_call, NULL);
	for (i=0; i < udc_count; i++) {
		if(udc[i] == FAL_FONT_CS0)
			sprintf(tmp, CS0);
		else if(udc[i] == FAL_FONT_CS1)
			sprintf(tmp, CS1);
		else if(udc[i] == FAL_FONT_CS2)
			sprintf(tmp, CS2);
		else if(udc[i] == FAL_FONT_CS3)
			sprintf(tmp, CS3);
		else
			sprintf(tmp, "Codeset %x?", udc[i]);
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
		sprintf(tmp, "%d", wls[i]);
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
		sprintf(tmp, "%d", hls[i]);
		button4[i] = XmCreatePushButton(pull4, tmp, NULL, 0);
		XtManageChild(button4[i]);
		XtAddCallback(button4[i], XmNactivateCallback,
						(XtCallbackProc)hls_call, NULL);
	}
}


static Widget
CreateSelectXLFD( top ) /* dtex change */
Widget	top ;
{

	int		n;
	Arg		args[16];
	XmString	xms, xms1 ;
	Widget		editW ;
	Widget		frame, row1, label1, row2, cas1, cas2, cas3, cas4;
	XmString	str;
	char		*p;
	udc_flag = sty_flag = wls_flag = hls_flag = False;

        /*
	*  create base window
	*/
        n = 0 ;
        XtSetArg( args[n], XmNautoUnmanage, False ) ;		n++ ;
        XtSetArg( args[n], XmNtitle, maintitle ) ;n++ ; /* dtex add */
        XtSetArg( args[n], XmNnoResize, True ) ;		n++ ;
        XtSetArg( args[n], XmNminimizeButtons, True ) ;		n++ ;
/*        xms = XmStringCreateLocalized( resource.exec_label ) ; dtex change*/
	p = GETMESSAGE(4, 10, "Open");
        xms = XmStringCreateLocalized( p ) ;
        XtSetArg( args[n], XmNokLabelString, xms ) ;		n++ ;
/*        xms1 = XmStringCreateLocalized( resource.quit_label) ; dtex change*/
	p = GETMESSAGE(4, 12, "Cancel");
        xms1 = XmStringCreateLocalized( p ) ;
        XtSetArg( args[n], XmNhelpLabelString, xms1 ) ;		n++ ;
/*        editW = XmCreateTemplateDialog( top, "dtfonteditor - Open", args, n ); dtex change*/
	p = GETMESSAGE(4, 14, "UDC data exchange utility");
        editW = XmCreateTemplateDialog( top, p, args, n );

	XmStringFree( xms ) ;
	XmStringFree( xms1 ) ;

	n = 0;
	pull1 = XmCreatePulldownMenu(toplevel, "pull", args, n);
	pull2 = XmCreatePulldownMenu(toplevel, "pull", args, n);
	pull3 = XmCreatePulldownMenu(toplevel, "pull", args, n);
	pull4 = XmCreatePulldownMenu(toplevel, "pull", args, n);

	n = 0 ;
   	xlfdWform  = XmCreateRowColumn( editW, "BaseForm", args, n );
    	XtManageChild( xlfdWform );

	n = 0;
	frame = XmCreateFrame( xlfdWform, "frame", args, n);
    	XtManageChild( frame );

	n = 0 ;
   	row1  = XmCreateRowColumn( frame, "row", args, n );
    	XtManageChild( row1 );

	p = GETMESSAGE(4, 16, "SelectItems");
	n = 0;
	label1 = XmCreateLabel( row1, p, args, n);
    	XtManageChild( label1 );

	n = 0 ;
   	row2  = XmCreateRowColumn( row1, "row", args, n );
    	XtManageChild( row2 );

	n = 0 ;
	/*str = XmStringCreateLocalized(resource.l_codeset) ; dtex change */
	p = GETMESSAGE(4, 18, "- Codeset:");
        str = XmStringCreateLocalized( p );
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull1);  n++;
   	cas1  = XmCreateOptionMenu( row2, "CodeSet", args, n );
    	XtManageChild( cas1 );
	XtAddCallback(XmOptionButtonGadget(cas1), XmNcascadingCallback,
					(XtCallbackProc)button_set1, NULL);
	XmStringFree(str);
 
	n = 0 ;
	/*str = XmStringCreateLocalized(resource.l_style) ; dtex change */
	p = GETMESSAGE(4, 20, "-   Style:");
        str = XmStringCreateLocalized( p );
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull2);  n++;
   	cas2  = XmCreateOptionMenu( row2, "Style", args, n );
	XtAddCallback(XmOptionButtonGadget(cas2), XmNcascadingCallback,
					(XtCallbackProc)button_set2, NULL);
    	XtManageChild( cas2 );
	XmStringFree(str);

	n = 0 ;
	/*str = XmStringCreateLocalized(resource.l_width) ; dtex change */
	p = GETMESSAGE(4, 22, "-   Width:");
        str = XmStringCreateLocalized( p );
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull3);  n++;
   	cas3  = XmCreateOptionMenu( row2, "Width", args, n );
    	XtManageChild( cas3 );
	XtAddCallback(XmOptionButtonGadget(cas3), XmNcascadingCallback,
					(XtCallbackProc)button_set3, NULL);
	XmStringFree(str);

	n = 0 ;
	/*str = XmStringCreateLocalized(resource.l_height) ; dtex change */
	p = GETMESSAGE(4, 24, "-  Height:");
        str = XmStringCreateLocalized( p );
	XtSetArg(args[n], XmNlabelString, str);  n++;
	XtSetArg(args[n], XmNsubMenuId, pull4);  n++;
   	cas4  = XmCreateOptionMenu( row2, "Height", args, n );
    	XtManageChild( cas4 );
	XtAddCallback(XmOptionButtonGadget(cas4), XmNcascadingCallback,
					(XtCallbackProc)button_set4, NULL);
	XmStringFree(str);

	font_init();
	create_xlfd();

	n = 0;
        XtSetArg(args[n], XmNvisibleItemCount, 10) ;	n++ ;
        XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT) ;	n++ ;
        XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC) ;	n++ ;
        XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT) ;	n++ ;
        XtSetArg(args[n], XmNitems, xlf) ;	n++ ;
        XtSetArg(args[n], XmNitemCount, xlf_count) ;	n++ ;
	scrolllist = XmCreateScrolledList(xlfdWform, "scrolllist", args, n);
    	XtManageChild(scrolllist);

        /*  
	 * Error_Messege
	 */

        XtAddCallback(editW, XmNokCallback, (XtCallbackProc)OpenWindowCB, NULL);
/*        XtAddCallback(editW, XmNokCallback, (XtCallbackProc)ReadCB, NULL); dtex del */
        XtAddCallback(editW, XmNcancelCallback, (XtCallbackProc)quit, NULL);
/*        XtAddCallback(editW, XmNhelpCallback,
					(XtCallbackProc)OpenCancelCB, NULL); dtex del */
        XtAddCallback(editW, XmNhelpCallback,
					(XtCallbackProc)quit, NULL);

	return( editW ) ;
}

