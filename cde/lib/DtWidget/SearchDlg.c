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
/* $XConsortium: SearchDlg.c /main/4 1996/08/28 16:51:52 drk $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        SearchDlg.c
**
**  Project:     Editor widget
**
**  Description:  Routines which manipulate the dialogs associated with
**                find/change functions.
**  -----------
**
*******************************************************************
* (c) Copyright 1993, 1994 Hewlett-Packard Company
* (c) Copyright 1993, 1994 International Business Machines Corp.
* (c) Copyright 1993, 1994 Sun Microsystems, Inc.
* (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#include "EditorP.h"
#include <Xm/DialogS.h>
#include <Xm/PushBG.h>
#include <Xm/Separator.h>
#include <Xm/Form.h>
#include <Xm/ScrollBar.h>
#include <Xm/List.h>
#include <Xm/LabelG.h>
#include <Xm/TextF.h>
#include <Xm/MwmUtil.h>
#include <Dt/DtMsgsP.h>

static void ManageFind(
	DtEditorWidget pPriv);
static void UnmanageFind(
	DtEditorWidget pPriv);
static void CreateSearchDialog(
	DtEditorWidget pPriv);

static void
ManageFind(
        DtEditorWidget pPriv)
{
    Arg al[10];
    int ac;

    ac = 0;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_findText(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 10); ac++;
    XtSetValues(M_search_replaceLbl(pPriv), al, ac);

    XtManageChild(M_search_findLbl(pPriv));
    XtManageChild(M_findText(pPriv));
}

static void
UnmanageFind(
        DtEditorWidget pPriv)
{
    Arg al[10];
    int ac;

    XtUnmanageChild(M_search_findLbl(pPriv));
    XtUnmanageChild(M_findText(pPriv));

    ac = 0;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_spellList(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 10); ac++;
    XtSetValues(M_search_replaceLbl(pPriv), al, ac);
    
}

/************************************************************************
 *
 *  _DtEditorSearch - Post a dialog asking for search string
 *
 ************************************************************************/

void
_DtEditorSearch(
        DtEditorWidget pPriv,
        Boolean spell,
        Boolean createonly )
{
    Arg al[10];             /* arg list */
    int ac;        /* arg count */

    /* create the dialog if it is the first time */
    if ( !M_search_dialog(pPriv) ) 
	CreateSearchDialog(pPriv);

    if (!createonly) 
    {
        /* Prepare to display the dialog */
        XtUnmanageChild(M_search_dialog(pPriv));
	/* 
	 * Turn off the Find, Change, and Change All buttons.
	 * They will be turned on by the appropriate callback.
	 */
	_DtEditorSetFindSensitivity(pPriv, False);
	_DtEditorSetReplaceSensitivity(pPriv, False);
	_DtEditorSetReplaceAllSensitivity(pPriv, False);

	/* 
	 * Clear the Change To field.  If in Find/Change mode it will
	 * be seeded with the last replace string, in Spell mode it will
	 * remain blank.
	 */
	XmTextFieldSetString( M_replaceText(pPriv), (char *)NULL );

        if (spell) 
	{
	    /* 
	     * Set it up as a Spell dialog
	     */

	    Position y;
	    Dimension height, formHeight;

            M_search_dialogMode(pPriv) = SPELL;

	    /*
	     * Set the dialog title.
	     * If DtNdialogTitle has been set then prepend it. 
	     */
	    if ( E_dialogTitle(pPriv) != (XmString)NULL) {

	      XmString titleStr = XmStringConcat( E_dialogTitle(pPriv), 
			 			  M_spellTitle(pPriv) );

              XtSetArg(al[0],XmNdialogTitle,titleStr);
              XtSetValues(M_search_dialog(pPriv), al, 1);

	      XmStringFree( titleStr );
	    }
	    else {
              XtSetArg(al[0],XmNdialogTitle,M_spellTitle(pPriv));
              XtSetValues(M_search_dialog(pPriv), al, 1);
	    }

	    ac = 0;
            XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	    XtSetValues(XtParent(M_search_spellList(pPriv)), al, ac);
            XtManageChild(M_search_listLbl(pPriv));
            XtManageChild(XtParent(M_search_spellList(pPriv)));

	    /*
	     * Select first item in list which will cause its selectCB
	     * to sensitize the Find button and clear the Change To field.
	     */
	    XmListSelectPos(M_search_spellList(pPriv), 1, True);

	    /* 
	     * Hide the Find field & label 
	     */
	    UnmanageFind(pPriv);

	    ac = 0;
            XtSetArg(al[ac], XmNy, &y); ac++;
            XtSetArg(al[ac], XmNheight, &height); ac++;
	    XtGetValues(XtParent(M_search_spellList(pPriv)), al, ac);
	    ac = 0;
            XtSetArg(al[ac], XmNheight, &formHeight); ac++;
	    XtGetValues(M_search_dialog(pPriv), al, ac);
	    ac = 0;
            XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
            XtSetArg(al[ac], XmNbottomOffset, formHeight - (y + height)); ac++;
	    XtSetValues(XtParent(M_search_spellList(pPriv)), al, ac);
        }
	else 
	{
	    /* 
	     * Set it up as a Find/Change dialog
	     */

	    XEvent event;

            M_search_dialogMode(pPriv) = REPLACE;

	    /*
	     * Set the dialog title.
	     * If DtNdialogTitle has been set then prepend it.
	     */
            ac = 0;
	    if ( E_dialogTitle(pPriv) != (XmString)NULL) {
	      XmString titleStr = XmStringConcat( E_dialogTitle(pPriv), 
			 			  M_fndChngTitle(pPriv) );

              XtSetArg(al[0], XmNdialogTitle, titleStr);
              XtSetValues(M_search_dialog(pPriv), al, 1);

	      XmStringFree( titleStr );
	    }
	    else {
              XtSetArg( al[0], XmNdialogTitle,M_fndChngTitle(pPriv) ); 
              XtSetValues(M_search_dialog(pPriv), al, 1);
	    }

	    /* 
	     * Show the Find field & label 
	     */
	    ManageFind(pPriv);

	    /* 
	     * Seed the Find field with the last string we searched for
	     * and select it.  This will cause _DtEditorFindTextChangedCB
	     * to enable the Find & Change All buttons.
	     */

	    if ( M_search_string(pPriv) ) {
	       XmTextFieldSetString(M_findText(pPriv),M_search_string(pPriv));
               event.xkey.time = XtLastTimestampProcessed(M_display(pPriv));
	       XtCallActionProc( (Widget) M_findText(pPriv), "select-all",
			         &event, (String *)NULL, 0 );
    	    }
	    else
	       XmTextFieldSetString( M_findText(pPriv), (char *)NULL );

	    /* 
	     * Seed the Change To field with the last replacement string.
	     */

	    if ( M_replace_string(pPriv) ) 
	      XmTextFieldSetString( M_replaceText(pPriv), 
				    M_replace_string(pPriv) );

	    /*
	     * Hide the list of misspelled words
	     */
            XtUnmanageChild(M_search_listLbl(pPriv));
            XtUnmanageChild(XtParent(M_search_spellList(pPriv)));
        } 

	/*
	 * Display the dialog
	 */
        XtManageChild(M_search_dialog(pPriv));
    }

    if (M_search_dialogMode(pPriv) == SPELL)
      XmProcessTraversal(M_replaceText(pPriv), XmTRAVERSE_CURRENT); 
    else
      XmProcessTraversal(M_findText(pPriv), XmTRAVERSE_CURRENT); 
}

static void
CreateSearchDialog(
        DtEditorWidget pPriv)
{
    Arg	     al[10];         /* arg list */
    int ac;         /* arg count */
    Pixel    textBackground, textForeground;
    XmString tmpStr1;

    /* 
     * The Find/Change & the Spell dialogs are the same dialog with
     * various components unmanaged depending upon the mode of the
     * dialog.  There is no reason why these could not be separate
     * dialogs if they need to be.  All of the callbacks and routines
     * which manipulate these dialogs are aware of the dialog's mode and
     * could be split into two separate callbacks/routines.
     */

    /*
     * Match the background & foreground colors of the edit window
     * Don't use DtNtextBackground/Foreground directly because they
     * will be DtUNSPECIFIED.
     */
    ac = 0;
    XtSetArg(al[ac], XmNforeground, &textForeground); ac++;
    XtSetArg(al[ac], XmNbackground, &textBackground); ac++;
    XtGetValues(M_text(pPriv), al, ac);

    /* 
     * Create the form dialog.
     */
    ac = 0;
    M_search_dialog(pPriv) = XmCreateFormDialog(M_topLevelShell(pPriv), 
						"findDlg", al, ac);
    ac = 0;
    XtSetArg(al[ac], XmNautoUnmanage, False); ac++;
    XtSetValues(M_search_dialog(pPriv), al, ac);

    /*
     * When creating the fields & buttons use the appropriate label
     * resource (e.g. DtNchangeButtonLabel), if it has been set, then
     * clear the resource to save space.  The field or button widget
     * will contain the actual value & it can be gotten from there,
     * if it is needed.
     *
     * If the appropriate resource has not been set, use its default
     * value from the message catalog.
     */

    /* 
     * Create the list of misspelled words
     */
    ac = 0;
    if (E_misspelledListLabel(pPriv) != (XmString) DtUNSPECIFIED) {
      /*
       * Use the resource value & clear it (to save space).
       */

      tmpStr1 = XmStringCopy(E_misspelledListLabel(pPriv));
      E_misspelledListLabel(pPriv) = (XmString) DtUNSPECIFIED;
    }
    else {
      /*
       * The resource has not been set so use its default value
       */
      tmpStr1 = XmStringCreateLocalized(MISSPELLED);
    }
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNfontList, E_labelFontList(pPriv)); ac++;
    M_search_listLbl(pPriv) = XmCreateLabelGadget(M_search_dialog(pPriv), 
						  "listLbl", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_listLbl(pPriv));

    ac = 0;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_listLbl(pPriv)); ac++;
    XtSetArg(al[ac], XmNscrollingPolicy, XmAUTOMATIC); ac++;
    XtSetArg(al[ac], XmNfontList, E_textFontList(pPriv)); ac++;
    M_search_spellList(pPriv) = XmCreateScrolledList(M_search_dialog(pPriv), 
						     "spellList", al, ac);
    XtManageChild(M_search_spellList(pPriv));
    XtManageChild(XtParent(M_search_spellList(pPriv)));

    XtAddCallback( M_search_spellList(pPriv), XmNhelpCallback,
		   (XtCallbackProc)_DtEditorHelpSearchSpellCB, 
		   (XtPointer)pPriv );

    /* 
     * Create the find field & its label
     */
    ac = 0;
    if (E_findFieldLabel(pPriv) != (XmString) DtUNSPECIFIED) {
      /*
       * Use the resource value & clear it (to save space).
       */

      tmpStr1 = XmStringCopy(E_findFieldLabel(pPriv));
      E_findFieldLabel(pPriv) = (XmString) DtUNSPECIFIED;
    }
    else {
      /*
       * The resource has not been set so use its default value
       */
      tmpStr1 = XmStringCreateLocalized(FIND_LABEL);
    }
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopOffset, 10); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNfontList, E_labelFontList(pPriv)); ac++;
    M_search_findLbl(pPriv) = XmCreateLabelGadget(M_search_dialog(pPriv), 
						  "findLbl", al, ac);
    XmStringFree(tmpStr1);

    ac = 0;
    XtSetArg (al[ac], XmNbackground, textBackground);   ac++;
    XtSetArg (al[ac], XmNforeground, textForeground);   ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_findLbl(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 5); ac++;
    XtSetArg(al[ac], XmNfontList, E_textFontList(pPriv)); ac++;
    M_findText(pPriv) = XmCreateTextField(M_search_dialog(pPriv), "findTxt", 
					   al, ac);

    XtAddCallback( M_findText(pPriv), XmNhelpCallback,
		   (XtCallbackProc)_DtEditorHelpSearchFindCB, 
		   (XtPointer)pPriv );

    /* 
     * Create the Change To field & its label
     */
    ac = 0;
    if (E_changeFieldLabel(pPriv) != (XmString) DtUNSPECIFIED) {
      /*
       * Use the resource value & clear it (to save space).
       */

      tmpStr1 = XmStringCopy(E_changeFieldLabel(pPriv));
      E_changeFieldLabel(pPriv) = (XmString) DtUNSPECIFIED;
    }
    else {
      /*
       * The resource has not been set so use its default value
       */
      tmpStr1 = XmStringCreateLocalized(CHANGE_LABEL);
    }
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_findText(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 10); ac++;
    XtSetArg(al[ac], XmNfontList, E_labelFontList(pPriv)); ac++;
    M_search_replaceLbl(pPriv) = XmCreateLabelGadget(M_search_dialog(pPriv), 
					"repLbl", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_replaceLbl(pPriv));

    ac = 0;
    XtSetArg (al[ac], XmNbackground, textBackground);   ac++;
    XtSetArg (al[ac], XmNforeground, textForeground);   ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_replaceLbl(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 5); ac++;
    XtSetArg(al[ac], XmNfontList, E_textFontList(pPriv)); ac++;
    M_replaceText(pPriv) = XmCreateTextField(M_search_dialog(pPriv), "repTxt", 
					   al, ac);
    XtManageChild(M_replaceText(pPriv));

    XtAddCallback( M_replaceText(pPriv), XmNhelpCallback,
		   (XtCallbackProc)_DtEditorHelpSearchChangeCB, 
		   (XtPointer)pPriv );
    /* 
     * Only enable Change To field if the widget is editable
     */
    XtSetSensitive(M_search_replaceLbl(pPriv), M_editable(pPriv));
    XtSetSensitive(M_replaceText(pPriv), M_editable(pPriv));
    
    ac = 0;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_replaceText(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 5); ac++;
    M_search_separator(pPriv) = 
		XmCreateSeparator(M_search_dialog(pPriv), "separator", 
					   al, ac);
    XtManageChild(M_search_separator(pPriv));

    /* 
     * Create the buttons & their labels
     */

    /* Find button */
    ac = 0;
    if (E_findButtonLabel(pPriv) != (XmString) DtUNSPECIFIED) {
      /*
       * Use the resource value & clear it (to save space).
       */

      tmpStr1 = XmStringCopy(E_findButtonLabel(pPriv));
      E_findButtonLabel(pPriv) = (XmString) DtUNSPECIFIED;
    }
    else {
      /*
       * The resource has not been set so use its default value
       */
      tmpStr1 = XmStringCreateLocalized(FIND_BUTTON);
    }
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNleftPosition, 2); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNrightPosition, 33); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_separator(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 7); ac++;
    XtSetArg(al[ac], XmNfontList, E_buttonFontList(pPriv)); ac++;
    M_search_findBtn(pPriv) = XmCreatePushButtonGadget(M_search_dialog(pPriv), 
						 "findBtn", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_findBtn(pPriv));

    /* Change button */
    ac = 0;
    if (E_changeButtonLabel(pPriv) != (XmString) DtUNSPECIFIED) {
      /*
       * Use the resource value & clear it (to save space).
       */

      tmpStr1 = XmStringCopy(E_changeButtonLabel(pPriv));
      E_changeButtonLabel(pPriv) = (XmString) DtUNSPECIFIED;
    }
    else {
      /*
       * The resource has not been set so use its default value
       */
      tmpStr1 = XmStringCreateLocalized(CHANGE_BUTTON);
    }
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNleftPosition, 35); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNrightPosition, 66); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_separator(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 7); ac++;
    XtSetArg(al[ac], XmNfontList, E_buttonFontList(pPriv)); ac++;
    M_search_replaceBtn(pPriv) = XmCreatePushButtonGadget(
						 M_search_dialog(pPriv), 
					   	 "replBtn", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_replaceBtn(pPriv));

    /* Change All button */
    ac = 0;
    if (E_changeAllButtonLabel(pPriv) != (XmString) DtUNSPECIFIED) {
      /*
       * Use the resource value & clear it (to save space).
       */

      tmpStr1 = XmStringCopy(E_changeAllButtonLabel(pPriv));
      E_changeAllButtonLabel(pPriv) = (XmString) DtUNSPECIFIED;
    }
    else {
      /*
       * The resource has not been set so use its default value
       */
      tmpStr1 = XmStringCreateLocalized(CHNG_ALL_BUTTON);
    }
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNleftPosition, 68); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNrightPosition, 98); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_separator(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 7); ac++;
    XtSetArg(al[ac], XmNfontList, E_buttonFontList(pPriv)); ac++;
    M_search_replaceAllBtn(pPriv) = XmCreatePushButtonGadget(
						M_search_dialog(pPriv), 
						"replAllBtn", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_replaceAllBtn(pPriv));

    /* Close button */
    ac = 0;
    tmpStr1 = XmStringCreateLocalized(CLOSE_BUTTON);
    XtSetArg(al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNleftPosition, 13); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNrightPosition, 43); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_findBtn(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 2); ac++;
    XtSetArg(al[ac], XmNfontList, E_buttonFontList(pPriv)); ac++;
    M_search_closeBtn(pPriv) = XmCreatePushButtonGadget(M_search_dialog(pPriv), 
						  "closeBtn", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_closeBtn(pPriv));

    ac = 0;
    tmpStr1 = XmStringCreateLocalized(HELP_BUTTON);
    XtSetArg (al[ac], XmNlabelString, tmpStr1); ac++;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNleftPosition, 56); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(al[ac], XmNrightPosition, 86); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_search_findBtn(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 2); ac++;
    XtSetArg(al[ac], XmNfontList, E_buttonFontList(pPriv)); ac++;
    M_search_helpBtn(pPriv) = XmCreatePushButtonGadget(M_search_dialog(pPriv), 
						 "helpBtn", al, ac);
    XmStringFree(tmpStr1);
    XtManageChild(M_search_helpBtn(pPriv));


    XtRemoveAllCallbacks(M_search_spellList(pPriv), XmNbrowseSelectionCallback);
    XtAddCallback(M_search_spellList(pPriv), XmNbrowseSelectionCallback,
                      (XtCallbackProc) _DtEditorMisspelledSelectCB, pPriv);
    XtAddCallback(M_search_spellList(pPriv), XmNdefaultActionCallback,
                      (XtCallbackProc) _DtEditorMisspelledDblClickCB, pPriv);

    ac = 0;
    XtSetArg(al[ac], XmNvisibleItemCount, 4); ac++;
    XtSetValues(M_search_spellList(pPriv), al, ac);

    ac = 0;
    XtSetArg(al[ac], XmNheight, &M_search_buttonHeight(pPriv)); ac++;
    XtGetValues(M_search_helpBtn(pPriv), al, ac);

    ac = 0;
    XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(al[ac], XmNtopWidget, M_replaceText(pPriv)); ac++;
    XtSetArg(al[ac], XmNtopOffset, 5); ac++;
    XtSetArg(al[ac], XmNbottomOffset, 2 * M_search_buttonHeight(pPriv) + 35);
    ac++;
    XtSetValues(M_search_separator(pPriv), al, ac);

    XtAddCallback(M_search_findBtn(pPriv), XmNactivateCallback,
                      (XtCallbackProc) _DtEditorDialogSearchCB, 
		      (XtPointer) pPriv);
    XtAddCallback(M_search_replaceBtn(pPriv), XmNactivateCallback,
                      (XtCallbackProc) _DtEditorDialogReplaceCB, 
		      (XtPointer) pPriv);
    XtAddCallback(M_search_replaceAllBtn(pPriv), XmNactivateCallback,
                      (XtCallbackProc) _DtEditorDialogReplaceAllCB, 
		      (XtPointer) pPriv);
    XtAddCallback(M_search_closeBtn(pPriv), XmNactivateCallback,
                      (XtCallbackProc) _DtEditorDialogFindCancelCB,
		      (XtPointer) pPriv);
    XtAddCallback(M_search_helpBtn(pPriv), XmNactivateCallback,
                      (XtCallbackProc) _DtEditorHelpSearchCB,
		      (XtPointer) pPriv);
    XtAddCallback(M_search_dialog(pPriv), XmNmapCallback,
                      (XtCallbackProc) _DtEditorSearchMapCB, pPriv);
    XtAddCallback(M_findText(pPriv), XmNvalueChangedCallback, 
		      (XtCallbackProc)_DtEditorFindTextChangedCB, pPriv);
    XtAddCallback(M_replaceText(pPriv), XmNvalueChangedCallback, 
		      (XtCallbackProc)_DtEditorReplaceTextChangedCB, pPriv);
    XtAddCallback(M_search_dialog(pPriv), XmNhelpCallback, 
		  (XtCallbackProc) _DtEditorHelpSearchCB, (XtPointer)pPriv);

    XtSetArg(al[0], XmNdefaultButton, M_search_closeBtn(pPriv));
    XtSetValues(M_search_dialog(pPriv), al, 1);

    XtRealizeWidget(M_search_dialog(pPriv));
} /* end CreateSearchDialog */ 
