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
/* $XConsortium: udcexp.c /main/5 1996/10/14 14:45:34 barstow $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "excutil.h"
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Form.h>
#include <Xm/Label.h>

void	setselectedcode();

extern char *maintitle;

void udcexp(Exc_data * ed)
{
    ed->function = EXPORT;
    strcpy(ed->bdfmode,"w");

    PopupSelectXLFD(ed->toplevel);
}

void createbdf(Exc_data * ed)
{
    int i = 0;
    char *comment_list[] = {""};
    int comment_num = 0;
    char *msg;
    int	ans;

    msg = GETMESSAGE(10, 2, "Failed to make the BDF file");

    i =  ExpGpftoBDF(ed->fontfile, ed->bdffile,
		     ed->code_num, ed->gpf_code_list,
		     comment_num, comment_list, 0);
    if (i != 0) {
	AskUser(ed->toplevel, ed, msg, &ans, "error");
    }
    excterminate(ed);
}

void selcharokCB(Widget widget, ListData * ld, XtPointer call_data)
{
    int		num;
    Exc_data	*ed;
    int		ans;
    char	*msg;

    msg = GETMESSAGE(10, 4, "No indexes are selected");

    XtVaGetValues(ld->list, XmNselectedItemCount, &num, NULL);
    if (num == 0) {
/*	AskUser(widget, ld->ed, msg, &ans, "error");*/
	return;
    } else {
	setselectedcode(ld);
	XtUnmanageChild(XtParent(widget));
	ed = ld->ed;
	freeld(ld);
	getbdffn(ed);
    }
}
void selcharcancelCB(Widget widget, ListData * ld, XtPointer call_data)
{
    Exc_data	*ed;

    ed = ld->ed;
    freeld(ld);
    excterminate(ed);
}

XmString * setxmslist(ListData * ld)
{
    char	**cp;
    XmString	*xmslist, *xmsp;
    int		i;

    if ((xmslist = (XmString *) calloc(ld->existcode_num, sizeof(XmString *)))
	== NULL) {
	excerror(ld->ed, EXCERRMALLOC, "setxmslist", "exit");
    }
    cp = ld->existcode_c;
    xmsp = xmslist;
    for (i = 0; i < ld->existcode_num; i++) {
	*xmsp = XmStringCreateLocalized(*cp);
	xmsp++;
	cp++;
    }
    return (xmslist);
}

void freexmslist(ListData * ld, XmString * xmslist)
{
    XmString	*xmsp;
    int		i;

    if (xmslist != NULL) {
	xmsp = xmslist;
	for (i = 0; i < ld->existcode_num; i++) {
	    XmStringFree(*xmsp);
	    xmsp++;
	}
	free(xmslist);
    }
}

void selcharcd(Exc_data * ed)
{
    Widget		mainw, selcd, ok, cancel;
    Widget		slctLabel, form;
    Arg			args[20];
    Cardinal		n;
    char		*oklabel;
    char		*cancellabel;
    XmString		*xmslist;
    extern ListData	*ld;
    char		*p;

    oklabel = GETMESSAGE(10, 6, "OK");
    cancellabel = GETMESSAGE(10, 8, "Cancel");

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNtitle, maintitle); n++;
    mainw = XmCreateTemplateDialog(ed->toplevel, "mainw", args, n);

    n = 0;
    form = XmCreateForm( mainw, "form", args, n);
    XtManageChild(form);

    p = GETMESSAGE(10, 10, "glyph indexes");
    n = 0;
    XtSetArg( args[n], XmNx,		20 ) ;  n++;
    XtSetArg( args[n], XmNheight,       20 ) ;	n++ ;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    10 ) ;	n++ ;
    slctLabel = XmCreateLabel( form, p, args, n);
    XtManageChild(slctLabel);

    n = 0;
    xmslist = setxmslist(ld);
    XtSetArg( args[n], XmNleftAttachment,	XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,	20 ) ;		n++ ;
    XtSetArg( args[n], XmNtopAttachment,	XmATTACH_WIDGET ); n++ ;
    XtSetArg( args[n], XmNtopOffset,	5 ) ;		n++ ;
    XtSetArg( args[n], XmNwidth,	200 ) ;		n++ ;
    XtSetArg (args[n], XmNtopWidget,	slctLabel );	n++;
    XtSetArg(args[n], XmNitems, xmslist); n++;
    XtSetArg(args[n], XmNitemCount, ld->existcode_num); n++;
    XtSetArg(args[n], XmNvisibleItemCount, 10); n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT); n++;
    selcd = XmCreateScrolledList(form, "Select codes", args, n);
    freexmslist(ld, xmslist);
    XtManageChild(selcd);

    ld->list = selcd;

    ok = excCreatePushButton(mainw, "ok", oklabel,
			     (XtCallbackProc) selcharokCB, (XtPointer) ld);

    cancel = excCreatePushButton(mainw, "cancel", cancellabel,
				 (XtCallbackProc) selcharcancelCB,
				 (XtPointer) ld);

    XtManageChild(mainw);
}

void setselectedcode(ListData *ld)
{
    int		*position_list;
    int		position_count;
    int		i;
    int		*codep;

    XmListGetSelectedPos(ld->list, &position_list, &position_count);

    ld->ed->code_num = position_count;
    ld->ed->gpf_code_list = (int *) calloc(position_count, sizeof(int));
    codep = ld->ed->gpf_code_list;

    for (i = 0; i < position_count; i++) {
	*codep = *((ld->existcode)+(position_list[i]-1));
	codep++;
    }
}
