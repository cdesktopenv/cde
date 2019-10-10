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
/* util.h 1.5 - Fujitsu source for CDEnext    96/09/18 13:59:22      */
/* $XConsortium: util.h /main/5 1996/09/19 19:42:11 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>

#define MarginWidth 4
#define RowMarginWidth 11
#define MarginHeight 11
#define HeightSpace 4
#define MinimumSpace 4

/*****  Exclusive Set  *****/
typedef struct _ToggleButt {
    Widget	w;
    char *	label;
    XtPointer	clientdata;
    String	name;
    XtArgVal	sensitive;
    XtCallbackProc	cb;
    XtArgVal	set;
} ToggleButt;

#define EXARGS( clientdata, name, sensitive, cb, set ){ \
		NULL, \
    (char *)	"", \
    (XtPointer)	clientdata, \
		name, \
    (XtArgVal)	sensitive, \
    (XtCallbackProc)	cb, \
    (XtArgVal)	set \
}

typedef struct _RadioButt {
    String	label;
    ToggleButt	*items;
    int		items_cnt;
    String	name;
    Widget	w;
    XtPointer	udata;
} RadioButt;


#define EXCLUSIVE(a) { 0, a, XtNumber(a), 0, 0, 0 }

#define ExclusiveItems		ToggleButt
#define Exclusive		RadioButt

#define SetItemLabel(b,n,l) (b)->items[(n)].label = (l)

/*****  PushButton Set  *****/

typedef struct _ButtonItems {
	char *	label;
	XtCallbackProc  cb;
	XtPointer clientdata;
	XtArgVal  mnemonic;
	XtArgVal  sensitive;
	XtArgVal  deflt;
} ButtonItems;

#define BTNARGS( cb, clientdata, mnumonic, sensitive, deflt ) { \
	(char *) "", \
	(XtCallbackProc) cb, \
	(XtPointer) clientdata, \
	(XtArgVal) mnumonic, \
	(XtArgVal) sensitive, \
	(XtArgVal) deflt \
}

typedef struct _Button {
	Widget		*w;
	ButtonItems	*items;
	int		itemcnt;
} Button;

#define BUTTONS( items ) { 0, items, XtNumber(items) }

/*****  MenuButton Set  *****/

typedef struct _menuButtonItems {
	char	*label;
	char	mnemonic;
	Widget	menu;
} menuButtonItems;

#define MENUBTNARGS( mnumonic ) { \
	"",    \
	mnumonic, \
	NULL, \
}

typedef struct _MButton {
	menuButtonItems *items;
	int		itemcnt;
} MButton;

#define MBUTTONS(items) {items,XtNumber((items))}


/*****  NoticeButton Set  *****/

typedef struct _NoticeButton {
	char	*label;
	void	(*cb)();
} NoticeButton;

#define NBTNARGS( cb, clientdata, mnumonic, sensitive, deflt) { \
	"",    \
	cb,    \
}

typedef struct _NButton {
	NoticeButton	*items;
	int		itemcnt;
} NButton;

#define NBUTTONS(items) {items,XtNumber((items))}


/*****  TextField Set  *****/

typedef struct _TextField {
	Widget	w1;
	Widget	w2;
	String	label;
} TextField;


/*****  Function Type  *****/

#define D_WARNING	XmDIALOG_WARNING
#define D_QUESTION	XmDIALOG_QUESTION
#define D_ERROR		XmDIALOG_ERROR
#define D_TEMPLATE	XmDIALOG_TEMPLATE
#define L_VERTICAL	XmVERTICAL
#define L_HORIZONTAL	XmHORIZONTAL


#ifdef	USE_MACRO
#define AddLeftAttachWidget( w, ref, offset ) XtVaSetValues( w, \
	XmNleftAttachment, XmATTACH_WIDGET, \
	XmNleftWidget, ref, \
	XmNleftOffset, offset, \
	0)

#define AddLeftAttachForm( w, offset ) XtVaSetValues( w, \
	XmNleftAttachment, XmATTACH_FORM, \
	XmNleftOffset, offset, \
	0)

#define AddTopAttachWidget( w, ref, offset ) XtVaSetValues( w, \
	XmNtopAttachment, XmATTACH_WIDGET, \
	XmNtopWidget, ref, \
	XmNtopOffset, offset, \
	0)

#define AddTopAttachForm( w, offset ) XtVaSetValues( w, \
	XmNtopAttachment, XmATTACH_FORM, \
	XmNtopOffset, offset, \
	0)

#define AddRightAttachWidget( w, ref, offset ) XtVaSetValues( w, \
	XmNrightAttachment, XmATTACH_WIDGET, \
	XmNrightWidget, ref, \
	XmNrightOffset, offset, \
	0)

#define AddRightAttachForm( w, offset ) XtVaSetValues( w, \
	XmNrightAttachment, XmATTACH_FORM, \
	XmNrightOffset, offset, \
	0)

#define AddBottomAttachForm( w, offset ) XtVaSetValues( w, \
	XmNbottomAttachment, XmATTACH_FORM, \
	XmNbottomOffset, offset, \
	0)
#endif	/* USE_MACRO */


extern Widget CreateDrawingArea(Widget owner,
				String name,
				int width,
				int height,
				void (*proc)(),
				int val);
extern Widget CreateDialogAndButtons(Widget owner,
				     String name,
				     void (*delcb)(),
				     Button *btns,
				     int btns_cnt,
				     Widget *pop);
extern String GetTextFieldValue(TextField *textf);
extern Widget CreateTemplateDialog(Widget w,
				   char *message,
				   unsigned char type,
				   NButton *button,
				   String title,
				   Widget *pop);
extern Widget CreateScrollBase(Widget owner,
			       String name,
			       int min,
			       int max,
			       int val,
			       int vcnt,
			       void (*sbproc)());

Widget	CreateCaptionFrame(Widget owner,
			   String name,
			   String labelstr,
			   int type,
			   int thickness);
void	CreateTextField(Widget owner,
			String name,
			String labelstr,
			TextField *data,
			int maxlength);
void	CreateMenuButtons(Widget owner, Button *buttons, int buttons_cnt);
Widget	GuiInitialize(XtAppContext *app,
		      String class_name,
		      int *ac,
		      String av[]);
void	AddLeftAttachWidget(Widget w, Widget ref, int offset);
void	AddLeftAttachForm(Widget w, int offset);
void	AddTopAttachWidget(Widget w, Widget ref, int offset);
void	AddTopAttachForm(Widget w, int offset);
void	AddRightAttachWidget(Widget w, Widget ref, int offset);
void	AddRightAttachForm(Widget w, int offset);
void	AddBottomAttachForm(Widget w, int offset);
void	PopupDialog(Widget w);
void	PopdownDialog(Widget w);
void	ForcePopdownDialog(Widget w);
void	SetLabelString(Widget w, String str);
void	SetFooterString(Widget w, String str);
void	PopupNotice(Widget owner,
		    char *message,
		    unsigned char type,
		    NButton *button,
		    Boolean do_format,
		    String title);
void	AddDeleteProc(Widget w, void (*delcb)());
Widget	CreateMenuBarAndFooterMessageForm(Widget owner,
					  String name,
					  MButton *buttons,
					  int bcnt,
					  Widget *pop,
					  Widget *footer);
Widget	GetMenuWidget(MButton *buttons, int buttons_num);
Widget	CreateForm(Widget owner, String name);
Widget	CreateLabel(Widget owner, String name, String str);
Widget	CreateFrame(Widget owner,
		    String name,
		    int type,
		    int thickness);
Widget	CreateRowColumn(Widget owner,
			String name,
			int layout,
			int space,
			int marginw,
			int marginh);
Widget	CreateScrollBar(Widget owner,
			String name,
			int height,
			int val,
			int min,
			int max,
			void (*proc)());

