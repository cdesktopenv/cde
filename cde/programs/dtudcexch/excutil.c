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
/* $XConsortium: excutil.c /main/3 1996/04/08 15:51:03 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "excutil.h"

extern char *maintitle;

Widget excCreatePushButton(Widget parent, String name, String label,
			   XtCallbackProc callback_func,
			   XtPointer client_data)
{
    Widget	widget;
    XmString	xms;
    Arg		args[20];
    Cardinal	n;

    n = 0;
    xms = XmStringCreateLocalized(label);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    widget = XmCreatePushButton(parent, name, args, n);
    XmStringFree(xms);
    XtAddCallback(widget, XmNactivateCallback,
		  (XtCallbackProc) callback_func,
		  (XtPointer) client_data);
    XtManageChild(widget);

    return (widget);
}

void excerror(Exc_data * ed, int excerrno, char * funcname, char * func)
{
    switch (excerrno)
    {
    case EXCERRMALLOC:
	fprintf(stderr, "dtudcexch:memory allocation error\n");
	freeExcdata(ed);
	exit (-1);
    case EXCERRNOUDC:
	fprintf(stderr, "dtudcexch:UDC is not defined in current locale\n");
	freeExcdata(ed);
	exit (-1);
    }
}

void dialogokCB(Widget widget, int * ans, XtPointer call_data)
{
    /* ok button pushed */
    *ans = 1;
}

void dialogcancelCB(Widget widget, int * ans, XtPointer call_data)
{
    /* cancel button pushed */
    *ans = 2;
}

void AskUser(Widget parent, Exc_data * ed, char *msg, int *r, char * type)
/* If ok button pushed, *r = 1. */
/* If cancel button pushed(if exists), *r = 2. */
{
    XmString				message;
    Widget				dialog;
    int					ans = 0;
    Cardinal				n;
    Arg					args[20];

    n = 0;
    message = XmStringCreateLocalized(msg);
    XtSetArg(args[n], XmNmessageString, message); n++;
    XtSetArg(args[n], XmNtitle, maintitle); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    if (strcmp(type, "warning") == 0) {
	dialog = XmCreateWarningDialog(parent, type, args, n);
    } else if (strcmp(type, "error") == 0) {
	dialog = XmCreateErrorDialog(parent, type, args, n);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    } else if (strcmp(type, "information") == 0) {
	dialog = XmCreateInformationDialog(parent, type, args, n);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    } else if (strcmp(type, "question") == 0) {
	dialog = XmCreateQuestionDialog(parent, type, args, n);
    } else {
	fprintf(stderr, "error in AskUser\n");
    }
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    XmStringFree(message);
    XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) dialogokCB,
		  (XtPointer) &ans);
    XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc) dialogcancelCB,
		  (XtPointer) &ans);
    XtManageChild(dialog);
    while (ans == 0) {
	XtAppProcessEvent(ed->app, XtIMAll);
	XSync(XtDisplay(dialog), 0);
    }
    *r = ans;
    XSync(XtDisplay(dialog), 0);
    XmUpdateDisplay(dialog);
}

int beki(int n, int num)
{
    int a,i;

    a = 1;
    for (i=0; i<num; i++)
	a = a * n;

    return a;
}

int c2h(char * num)
{
    int		i,n;
    char	*c;

    if ((strlen(num)==6)&&(strncmp(num,"0x",2)==0))
    {
	c = num;
	c=c+2;
	n=0;
	for (i=3;i>=0;i--)
	{
	    if (('0'<= *c)&&(*c <= '9'))
		n = n + ((*c - '0')*beki(16,i));
	    else if (('a' <= *c)&&(*c <= 'f'))
		n = n + ((*c - 'a'+10)*beki(16,i));
	    else
		Ecs("error in c2h.here");
	    c++;
	}
    } else
	Ecs("error in c2h");

    return n;
}
char * renge2num(char * renge)
{
    char * num;

    num = (char *) calloc(7, sizeof(char));

    strncpy(num,renge,6);

    return num;
}

void Ecs(char * string)
{
    fprintf(stderr,"%s\n",string);
}
void Ecd(int data)
{
    fprintf(stderr,"%d\n",data);
}

void freecsrec(UDCcsREC *rec)
{
    if (rec->charset != NULL)
	free(rec->charset);
    if (rec->udcrenge != NULL)
	free(rec->udcrenge);
}

void freecslist(int n, UDCcsREC *rec)
{
    int		i;
    UDCcsREC	*recp;

    recp = rec;
    for (i = 0; i < n; i++)
    {
	if (recp != NULL)
	{
	    freecsrec(recp);
	    recp++;
	} else {
	    break;
	}
    }
}

void freeExcdata(Exc_data *ed)
{
    if (ed == NULL)
	return;

/* Don't free ed->toplevel */

    if (ed->locale != NULL)
	free(ed->locale);

    if (ed->cslist != NULL)
	free(ed->cslist);

    if (ed->xlfdname != NULL)
	free(ed->xlfdname);

    if (ed->style != NULL)
	free(ed->style);

    if (ed->size != NULL)
	free(ed->size);

/* Don't free ed->fontfile */

    if (ed->bdffile != NULL)
	XtFree(ed->bdffile);

    if (ed->udcrenge != NULL)
	free(ed->udcrenge);

    if (ed->gpf_code_list != NULL)
	free(ed->gpf_code_list);

    if (ed->bdf_code_list != NULL)
	free(ed->bdf_code_list);

    if (ed->comment_list != NULL)
	free(ed->comment_list);

    free(ed);
}

void Ecss(char * s1, char * s2)
{
    fprintf(stderr,"%s is %s\n", s1, s2);
}
void Ecsd(char * s1, int i)
{
    fprintf(stderr,"%s = %d\n", s1, i);
}

void checkdata(Exc_data *ed)
{
    if (ed == NULL)
	return;

/* Don't free ed->toplevel */

    if (ed->locale != NULL)
	Ecss("ed->locale", ed->locale);

    Ecsd("ed->csnum", ed->csnum);

    if (ed->xlfdname != NULL)
	Ecss("ed->xlfdname",ed->xlfdname);

    if (ed->fontfile != NULL)
	Ecss("ed->fontfile",ed->fontfile);

    if (ed->bdffile != NULL)
	Ecss("ed->bdffile",ed->bdffile);

    if (ed->udcrenge != NULL)
	Ecss("ed->udcrenge",ed->udcrenge);

    Ecsd("ed->code_num", ed->code_num);
}

void excterminate(Exc_data * ed)
{
    freeExcdata(ed);
    exit(0);
}

void excexit(Exc_data * ed)
{
    excterminate(ed);
}

void freeld(ListData *ld)
{
    int		i;
    char	**cp;

    if (ld != NULL) {
	if (ld->allcode != NULL)
	    free(ld->allcode);
	if (ld->existcode != NULL)
	    free(ld->existcode);
	if (ld->existcode_c != NULL) {
	    cp = ld->existcode_c;
	    for (i = 0; i < ld->existcode_num; i++) {
		if (*cp != NULL)
		    free(*cp);
		cp++;
	    }
	    free(ld->existcode_c);
	}
	free(ld);
    }
}

int bigger(int num1, int num2)
{
    if (num1 >= num2)
	return (num1);
    else
	return (num2);
}

int smaller(int num1, int num2)
{
    if (num1 < num2)
	return (num1);
    else
	return (num2);
}
