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
/* $XConsortium: vgsiaform.c /main/5 1996/05/07 12:22:34 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log$
 * Revision 1.1.2.2  1995/06/06  19:39:54  Chris_Beute
 * 	New file added for SIA password expiration forms handling
 * 	[1995/05/31  19:52:41  Chris_Beute]
 *
 * Revision 1.1.2.2  1995/05/22  18:13:32  Peter_Derr
 * 	Add a Cancel button.
 * 	[1995/05/15  18:06:21  Peter_Derr]
 * 
 * 	Support SIAMENUONE and SIAMENUANY
 * 	[1995/05/12  20:29:53  Peter_Derr]
 * 
 * 	SIA form UI
 * 	[1995/05/11  22:09:09  Peter_Derr]
 * 
 * $EndLog$
 */

#include "vg.h"
#include "vgmsg.h"
#include <alloca.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>

XmString multiline_xmstring(char *);
static void NoEchoCB( Widget, XtPointer, XtPointer);
static void RespondSiaFormCB( Widget, XtPointer, XtPointer);
static void CancelSiaFormCB( Widget, XtPointer, XtPointer);
static void SiaBackspace( Widget, XEvent *, char **, Cardinal *);


static char textEventBindings[] = {
"<Key>Return:           		next-tab-group()"
};

static char activateBindings[] = {
"<Key>osfActivate:      		activate()\n\
  Ctrl <Key>Return:             	activate()\n\
 <Key>Return:           		activate()"
};

static char activatePBBindings[] = {
"<Key>osfActivate:      		ArmAndActivate()\n\
  Ctrl <Key>Return:             	ArmAndActivate()\n\
 <Key>Return:           		ArmAndActivate()"
};

static char noEchoBindings[] = {
"Ctrl<Key>:     			my-insert()\n\
 <Key>osfBackSpace:     		sia-bksp()\n\
 <Key>osfDelete:                	sia-bksp()"
};

static XtActionsRec textActions[] = {
	{"sia-bksp", (XtActionProc)SiaBackspace},
    };



SiaFormInfo *globalSiaFormInfo;

/***************************************************************************
 *
 *  SiaForm
 *
 *  Widgets: sia_form, sia_title, sia_prompts
 ***************************************************************************/

void 
SiaForm(SiaFormInfo *siaFormInfo)
{
    int i;
    int	j;
    LoginTextPtr textdata; 
    XtTranslations      textTable;
    Widget sia_form, title_label, ok_button, cancel_button, prompts_row_col;
    Widget *prompt_labels;
    Widget *prompt_answers;
    XmString xmstr;
    XtTranslations noecho_trans, text_trans, activate_trans, activatePBtrans;

    globalSiaFormInfo = siaFormInfo;
    siaFormInfo->collect_status = 1;
    /*
     * XmCreateFormDialog()
     * XmCreateLabel()
     * XmCreateRowColumn()
     *   for each prompt
     *	   XmCreateLabel()
     *	   XmCreateTextField()
     *	   if not visible
     *		add ModifyVerify callback
     * XmCreatePushButton
     *		PB callback will collect answers, send them down the pipe
     *		and destroy the sia_form.
     */


    /*
     *  create the SIA form
     */

    i = 0;
    i = InitArg(Form); 
    XtSetArg(argt[i], XmNmarginHeight,		MBOX_MARGIN_HEIGHT	); i++;
    XtSetArg(argt[i], XmNmarginWidth,		MBOX_MARGIN_WIDTH	); i++;
    XtSetArg(argt[i], XmNshadowThickness,	SHADOW_THICKNESS	); i++;
    XtSetArg(argt[i], XmNnoResize,		False			); i++;
    XtSetArg(argt[i], XmNresizePolicy,		XmRESIZE_ANY		); i++;

    sia_form = XmCreateFormDialog(login_shell, "sia_form", argt, i);
    siaFormInfo->sia_form_widget = sia_form;

    /*
     *  create the title label
     */
    /* i = 0; */
    i = InitArg(Label);
    XtSetArg(argt[i], XmNlabelString, 		siaFormInfo->title	); i++;
    XtSetArg(argt[i], XmNtraversalOn,		False			); i++;
    XtSetArg(argt[i], XmNtopAttachment,		XmATTACH_FORM           ); i++;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_FORM           ); i++;
    XtSetArg(argt[i], XmNrightAttachment,	XmATTACH_FORM           ); i++;
    XtSetArg(argt[i], XmNalignment,		XmALIGNMENT_CENTER      ); i++;
    xmstr = multiline_xmstring(siaFormInfo->title);
    XtSetArg(argt[i], XmNlabelString,		xmstr			); i++;
    
    title_label = XmCreateLabel(sia_form, "title_label", argt, i);
    XtManageChild(title_label);

    XmStringFree(xmstr);


    /* ok button */

    activate_trans = XtParseTranslationTable(activateBindings);
    activatePBtrans = XtParseTranslationTable(activatePBBindings);
    
    i = InitArg(PushB);
    XtSetArg(argt[i], XmNbottomAttachment, 	XmATTACH_FORM		); i++;
    XtSetArg(argt[i], XmNtraversalOn, 		True			); i++;
    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_OK_LABEL, MC_DEF_OK_LABEL );
    XtSetArg(argt[i], XmNlabelString,		xmstr			); i++;

    ok_button = XmCreatePushButton(sia_form, "ok_button", argt, i);

    XtOverrideTranslations(ok_button, activatePBtrans);

    XmStringFree(xmstr);
    XtManageChild(ok_button);

    XtAddCallback(ok_button, XmNactivateCallback, RespondSiaFormCB, 
			siaFormInfo);    


    /* Cancel button */

    i = InitArg(PushB);
    XtSetArg(argt[i], XmNbottomAttachment, 	XmATTACH_FORM		); i++;
    XtSetArg(argt[i], XmNtraversalOn, 		True			); i++;
    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_CANCEL_LABEL, MC_DEF_CANCEL_LABEL );
    XtSetArg(argt[i], XmNlabelString,		xmstr			); i++;

    cancel_button = XmCreatePushButton(sia_form, "cancel_button", argt, i);

    XmStringFree(xmstr);
    XtManageChild(cancel_button);

    XtAddCallback(cancel_button, XmNactivateCallback, CancelSiaFormCB, 
			siaFormInfo);    

    /*
     * create the RowColumn widget for prompts
     */

    i =0;
    XtSetArg(argt[i], XmNorientation,		XmHORIZONTAL		); i++;
    XtSetArg(argt[i], XmNentryVerticalAlignment, XmALIGNMENT_CENTER	); i++;
    XtSetArg(argt[i], XmNrowColumnType,		XmWORK_AREA		); i++;
    XtSetArg(argt[i], XmNnumColumns,		siaFormInfo->num_prompts); i++;
    XtSetArg(argt[i], XmNpacking,		XmPACK_COLUMN		); i++;
    XtSetArg(argt[i], XmNtopAttachment,		XmATTACH_WIDGET		); i++;
    XtSetArg(argt[i], XmNtopWidget,		title_label		); i++;
    XtSetArg(argt[i], XmNbottomAttachment,	XmATTACH_WIDGET		); i++;
    XtSetArg(argt[i], XmNbottomWidget,		ok_button		); i++;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_FORM		); i++;
    XtSetArg(argt[i], XmNrightAttachment,	XmATTACH_FORM		); i++;

    switch (siaFormInfo->rendition)
	{
	case SIAMENUONE:
	    XtSetArg(argt[i], XmNradioBehavior,		True	); i++;
	    /* no break */
	case SIAMENUANY:
	    XtSetArg(argt[i], XmNisHomogeneous,		True		); i++;
	    XtSetArg(argt[i], XmNentryClass,   xmToggleButtonGadgetClass); i++;
	    XtSetArg(argt[i], XmNentryAlignment,  XmALIGNMENT_BEGINNING ); i++;
	    break;

	case SIAONELINER:
	    /* no break */
	case SIAFORM:
	    XtSetArg(argt[i], XmNentryAlignment,	XmALIGNMENT_END	); i++;
	}

    prompts_row_col = XmCreateRowColumn(sia_form, "prompts_row_col", argt, i);
    XtManageChild(prompts_row_col);

    prompt_labels = (Widget *) alloca(sizeof(Widget *) * siaFormInfo->num_prompts);
    prompt_answers = siaFormInfo->answer_widgets;

    noecho_trans = XtParseTranslationTable(noEchoBindings);
    text_trans = XtParseTranslationTable(textEventBindings);

    for (j=0; j<siaFormInfo->num_prompts; j++)
	{
	/*
	 *  create the prompt label
	 */
	char label_name[81];
	char answer_name[81];

	sprintf(label_name,"Prompt %d",j);
	sprintf(answer_name,"Answer %d",j);
	xmstr = multiline_xmstring(siaFormInfo->prompts[j]);

	switch (siaFormInfo->rendition)
	    {
	    case SIAMENUANY:
	    case SIAMENUONE:
		{
		/* i = InitArg(ToggleBG); */
		i = 0;
		XtSetArg(argt[i], XmNtraversalOn,	True		); i++;
		XtSetArg(argt[i], XmNlabelString,	xmstr		); i++;
		
		prompt_answers[j]  = XmCreateToggleButtonGadget(prompts_row_col,
						answer_name, argt, i);
		}
		break;

	    case SIAONELINER:
		/* no break */
	    case SIAFORM:
		{
		i = 0;
		XtSetArg(argt[i], XmNtraversalOn,	True		); i++;
		XtSetArg(argt[i], XmNlabelString,	xmstr		); i++;
		
		prompt_labels[j]  = XmCreateLabel(prompts_row_col, label_name, 
							argt, i);
		XtManageChild(prompt_labels[j]);

		/*
		 *  create the answer text field
		 */

		i = InitArg(Text);
		XtSetArg(argt[i], XmNselectionArrayCount,	1	); i++;
		XtSetArg(argt[i], XmNmaxLength,		80		); i++;
		XtSetArg(argt[i], XmNmappedWhenManaged,	True		); i++;

		XtAddActions(textActions, 1);
		prompt_answers[j] = XmCreateTextField(prompts_row_col, 
						    answer_name, argt, i);
		/* Make Return advance to next field. */
		XtOverrideTranslations(prompt_answers[j], text_trans);

		if (!siaFormInfo->visible[j])
		    {
		    /*
		     * allocate the answer buffer
		     */
		    siaFormInfo->answers[j] = XtMalloc(SIAMXPASSWORD+1);
		    (siaFormInfo->answers[j])[0] = '\0';

		    XtAddCallback(prompt_answers[j], XmNmodifyVerifyCallback, 
					NoEchoCB, siaFormInfo);
		    XtAddCallback(prompt_answers[j], XmNmotionVerifyCallback, 
					NoEchoCB, siaFormInfo);

		    /* handle Delete/Backspace in the no-echo case */
		    XtOverrideTranslations(prompt_answers[j], noecho_trans);
		    }

		}
		break;
	    } /* end  switch (siaFormInfo->rendition) */

	XtManageChild(prompt_answers[j]);
	}


    XtManageChild(sia_form);
    /*
     * Center the OK and Cancel buttons
     */
    {
	Dimension width;
	i = 0;
	XtSetArg(argt[i], XmNwidth,  &width);  i++;
	XtGetValues(ok_button, argt, i);
	i = 0;
	XtSetArg(argt[i], XmNleftAttachment,  XmATTACH_POSITION);  i++;
	XtSetArg(argt[i], XmNleftPosition,  33);  i++;
	XtSetArg(argt[i], XmNleftOffset,  -(width/2));  i++;
	XtSetValues(ok_button, argt, i);

	i = 0;
	XtSetArg(argt[i], XmNwidth,  &width);  i++;
	XtGetValues(cancel_button, argt, i);
	i = 0;
	XtSetArg(argt[i], XmNleftAttachment,  XmATTACH_POSITION);  i++;
	XtSetArg(argt[i], XmNleftPosition,  66);  i++;
	XtSetArg(argt[i], XmNleftOffset,  -(width/2));  i++;
	XtSetValues(cancel_button, argt, i);
    }

    if (siaFormInfo->rendition == SIAONELINER
	    || siaFormInfo->rendition == SIAFORM)
	{
	/*
	 * Make the Return key Activate on the last text field.
	 */
	XtOverrideTranslations(prompt_answers[siaFormInfo->num_prompts - 1],
			      activate_trans);
	XtAddCallback(prompt_answers[siaFormInfo->num_prompts - 1], 
		XmNactivateCallback, RespondSiaFormCB, siaFormInfo);    
	XSetInputFocus(XtDisplay(prompt_answers[0]), 
		XtWindow(prompt_answers[0]), RevertToPointerRoot, CurrentTime);

	i = 0;
	XtSetArg(argt[i], XmNinitialFocus,  prompt_answers[0]);  i++;
	XtSetValues(sia_form, argt, i);
	XmProcessTraversal(prompt_answers[0],XmTRAVERSE_CURRENT);
	}
}

/*
 *****************
 * NoEchoCB
 *
 * Save the text entered but do not display it
 *****************
 */

static void NoEchoCB( Widget w, XtPointer client_data, XtPointer call_data )
{
    int i;
    SiaFormInfo *siaFormInfo;
    XmTextVerifyPtr call;

    siaFormInfo = (SiaFormInfo *)client_data;
    call = (XmTextVerifyPtr) call_data;
    if(call->reason == XmCR_MOVING_INSERT_CURSOR)
	{
	call->doit=False;
	return;
	}
    for (i=0; i<siaFormInfo->num_prompts; i++)
	{
	if (w == siaFormInfo->answer_widgets[i])
	    {
	    if (call->text->ptr)
		strcat(siaFormInfo->answers[i], call->text->ptr);

	    call->text->ptr[0] = '\0';
	    return;
	    }
	}
}

/*
 ************************
 * RespondSiaFormCB
 *
 *      PB callback collects answers, sends them down the pipe
 *      and destroys the sia_form.
 ************************
 */

static void RespondSiaFormCB( Widget w, XtPointer client_data, 
				XtPointer call_data )
{
    char buf[REQUEST_LIM_MAXLEN];
    ResponseForm *r;
    int i;
    SiaFormInfo *siaFormInfo;
    char *answer_ptr;

    r = (ResponseForm *)buf;
    siaFormInfo = (SiaFormInfo *)client_data;

    r->hdr.opcode = REQUEST_OP_FORM;
    r->hdr.reserved = 0;
    r->hdr.length = sizeof(ResponseForm);

    r->collect_status = siaFormInfo->collect_status;
    r->num_answers = siaFormInfo->num_prompts;
    r->offAnswers = sizeof(ResponseForm);

    answer_ptr = buf + sizeof(ResponseForm);

    for (i=0; i<siaFormInfo->num_prompts; i++)
	{
	if (siaFormInfo->rendition == SIAMENUONE
		|| siaFormInfo->rendition == SIAMENUANY)
	    {
	    if (XmToggleButtonGadgetGetState(siaFormInfo->answer_widgets[i]))
		siaFormInfo->answers[i] = "X";
	    else
		siaFormInfo->answers[i] = NULL;
	    }
	else if (siaFormInfo->visible[i])
	    siaFormInfo->answers[i] =
			XmTextFieldGetString(siaFormInfo->answer_widgets[i]);
	if (!siaFormInfo->answers[i] || (siaFormInfo->answers[i])[0] == '\0')
	    {
	    *answer_ptr = '\0';
	    *answer_ptr++;
	    r->hdr.length++;
	    }
	else
	    {
	    int tmp;

	    r->hdr.length += strlen(siaFormInfo->answers[i]) + 1;
	    strcpy(answer_ptr, siaFormInfo->answers[i]);
	    tmp = strlen(answer_ptr) + 1;
	    answer_ptr+= tmp;
	    r->hdr.length += tmp;
	    }
	}
#ifdef DEBUG
    for (i=0; i<siaFormInfo->num_prompts; i++)
	printf("answer %d : %s\n", i, siaFormInfo->answers[i]);
#else
    write(1, buf, r->hdr.length);
#endif

    for (i=0; i<siaFormInfo->num_prompts; i++)
	{
	XtFree(siaFormInfo->answers[i]);
	XtFree(siaFormInfo->prompts[i]);
	XtFree(siaFormInfo->title);
	}
    XtDestroyWidget(siaFormInfo->sia_form_widget);

}

/***************************************************************************
 *
 *  SiaBackspace
 *
 *  Local backspace action for the no-echo case text widget. 
 *  Deletes the last character of the string in the 
 *  widget for each backspace key press.
 ***************************************************************************/

static void
SiaBackspace( Widget w, XEvent *event, char **params, Cardinal *num_params )
{
    int i, len;

    for (i=0; i<globalSiaFormInfo->num_prompts; i++)
	{
	if (globalSiaFormInfo->answer_widgets[i] == w)
	    {
	    len = strlen(globalSiaFormInfo->answers[i]);
	    if (len > 0)
		globalSiaFormInfo->answers[i][len-1] = '\0';
	    return;
	    }
	}
}

/*
 ************************
 * CancelSiaFormCB
 *
 *      Set collect_status to failure, call RespondSiaFormCB.
 *
 ************************
 */

static void CancelSiaFormCB( Widget w, XtPointer client_data, 
				XtPointer call_data )
{
    SiaFormInfo *siaFormInfo;

    siaFormInfo = (SiaFormInfo *)client_data;
    siaFormInfo->collect_status = 0;

    RespondSiaFormCB(w, client_data, call_data);
}
