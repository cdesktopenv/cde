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
/* $XConsortium: xmwidgets.c /main/6 1996/04/01 17:54:38 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

#include	"name.h" 
#include	"shell.h" 
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/ComboBox.h>
#include <Xm/Command.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawnB.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SashP.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>
#include <Xm/SSpinB.h>
#include <Xm/TearOffBP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>
#include <Dt/Print.h>

#define NO_AST
#include "dtksh.h"
#undef NO_AST

#include "xmksh.h"
#include "dtkcmds.h"
#include "widget.h"
#include "xmwidgets.h"
#include "msgs.h"




static discInfo * setArrowDisciplines( void ) ;
static discInfo * setComboDisciplines( void ) ;
static discInfo * setCmdDisciplines( void ) ;
static discInfo * setDAreaDisciplines( void ) ;
static discInfo * setDBtnDisciplines( void ) ;
static discInfo * setDtPrintSetupProcDisciplines( void ) ;
static discInfo * setFSelDisciplines( void ) ;
static discInfo * setListDisciplines( void ) ;
static discInfo * setPBtnDisciplines( void ) ;
static discInfo * setRCDisciplines( void ) ;
static discInfo * setScaleDisciplines( void ) ;
static discInfo * setSBarDisciplines( void ) ;
static discInfo * setSWinDisciplines( void ) ;
static discInfo * setSBoxDisciplines( void ) ;
static discInfo * setTextDisciplines( void ) ;
static discInfo * setTBtnDisciplines( void ) ;
static discInfo * setHelpDisciplines( void ) ;




static Namdisc_t arrowDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())arrowCreateDisc, NULL, NULL};
static Namdisc_t comboDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())comboCreateDisc, NULL, NULL};
static Namdisc_t cmdDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())cmdCreateDisc, NULL, NULL};
static Namdisc_t dAreaDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())dAreaCreateDisc, NULL, NULL};
static Namdisc_t dbtnDisc ={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())dbtnCreateDisc, NULL, NULL};
static Namdisc_t dtPrintDisc ={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())dtPrintSetupProcDisc, NULL, NULL};
static Namdisc_t fselDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())fselCreateDisc, NULL, NULL};
static Namdisc_t listDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())listCreateDisc, NULL, NULL};
static Namdisc_t pbtnDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())pbtnCreateDisc, NULL, NULL};
static Namdisc_t rcDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())rcCreateDisc, NULL, NULL};
static Namdisc_t scaleDisc={0, NULL, NULL, NULL, NULL,
                           (Namval_t *(*)()) scaleCreateDisc, NULL, NULL};
static Namdisc_t sbarDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())sbarCreateDisc, NULL, NULL};
static Namdisc_t swinDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())swinCreateDisc, NULL, NULL};
static Namdisc_t sboxDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())sboxCreateDisc, NULL, NULL};
static Namdisc_t textDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())textCreateDisc, NULL, NULL};
static Namdisc_t textDisc2={0, NULL, NULL, NULL, NULL,
                           (Namval_t *(*)())textCreateDisc2, NULL, NULL};
static Namdisc_t tbtnDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())tbtnCreateDisc, NULL, NULL};
static Namdisc_t dftDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())dftCreateDisc, NULL, NULL};
static Namdisc_t helpDisc={0, NULL, NULL, NULL, NULL, 
                           (Namval_t *(*)())helpCreateDisc, NULL, NULL};


/*
 * The main window fixups are required because of a Motif bug; the
 * resources were typed as XmRWindows, instead of XmRWidgets.
 */
resfixup_t mainwindow_fixups[] = {
        { "menuBar", XmCMenuBar, XtRWidget, sizeof(Widget) },
        { "commandWindow", XmCCommandWindow, XtRWidget, sizeof(Widget) },
        { "messageWindow", XmCMessageWindow, XtRWidget, sizeof(Widget) },
        { NULL }
};


/*
 * The help fixups are required to overcome name overlaps between other
 * widgets.
 */
resfixup_t helpDialogs_fixups[] = {
        { "helpType", DtCHelpType, "HelpType", sizeof(unsigned char) },
        { "scrollBarDisplayPolicy", XmCScrollBarDisplayPolicy,
               "HelpScrollBarDisplayPolicy", 
               sizeof(char) },
        { NULL }
};


/*
 * The following fixups are required to force the resource type to be one
 * we have defined, so that our special converter will work.  Typically,
 * there is no default StringTable to String converter.
 */
resfixup_t list_fixups[] = {
        { "items", XmCItems, "ListItems", sizeof(XmStringTable) },
        { "selectedItems", XmCSelectedItems, "SelectedListItems", 
               sizeof(XmStringTable) },
        { NULL }
};

resfixup_t command_fixups[] = {
        { "historyItems", XmCItems, "SelBoxItems", sizeof(XmStringTable) },
        { NULL }
};

resfixup_t selbox_fixups[] = {
        { "listItems", XmCItems, "SelBoxItems", sizeof(XmStringTable) },
        { NULL }
};

resfixup_t fileselbox_fixups[] = {
        { "fileListItems", XmCItems, "FSFileItems", sizeof(XmStringTable) },
        { "dirListItems", XmCItems, "FSDirItems", sizeof(XmStringTable) },
        { NULL }
};


/*
 * The following shell resources are 'fixed' to allow them to be referred
 * to in a shell script by their symbolic names, as opposed to their
 * integer values.  These also need to be force onto any subclasses.
 */
resfixup_t wmshell_fixups[] = {
        { "winGravity", XtCWinGravity, "XWinGravity", sizeof(int) },
        { NULL }
};

resfixup_t vendor_fixups[] = {
        { "winGravity", XtCWinGravity, "XWinGravity", sizeof(int) },
        { "mwmDecorations", XmCMwmDecorations, "MWMDecoration", sizeof(long) },
        { "mwmFunctions", XmCMwmFunctions, "MWMFunctions", sizeof(long) },
        { "mwmInputMode", XmCMwmInputMode, "MWMInputMode", sizeof(long) },
        { NULL }
};

/*
 * The following resource needs to be 'fixed' because it not only accepts
 * an integer value, but it also supports one specially defined value.
 */
resfixup_t panedwin_con_fixups[] = {
        {"positionIndex", XmCPositionIndex, "PanedWinPosIndex", sizeof(short)},
        { NULL }
};



classtab_t C[] = {
	/*
	 * NOTE: Keep these in alphabetical order
	 * because the initialize code below depends on
	 * the order.
	 */
	{ "XmArrowButton",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmArrowButtonGadget",  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmBulletinBoard",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmCascadeButton",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmCascadeButtonGadget", NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmComboBox",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmCommand",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmDialogShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmDrawingArea",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmDrawnButton",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmFileSelectionBox",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmForm",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmFrame",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmLabel",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmLabelGadget",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmList",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmMainWindow",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmMenuShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmMessageBox",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmPanedWindow",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmPushButton",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmPushButtonGadget",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmRowColumn",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmScale",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmScrollBar",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmScrolledWindow",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmSelectionBox",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmSeparator",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmSeparatorGadget",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmSimpleSpinBox",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmTearOffButton",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmText",	          NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmTextField",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmToggleButton",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "XmToggleButtonGadget", NULL, NULL, NULL, NULL, NULL, NULL},
	{ "ApplicationShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "OverrideShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "Shell",	   	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "TopLevelShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "TransientShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "VendorShell",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "WMShell",		  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "DtHelpDialog",         NULL, NULL, NULL, NULL, NULL, NULL},
	{ "DtHelpQuickDialog",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ "DtPrintSetupBox",	  NULL, NULL, NULL, NULL, NULL, NULL},
	{ NULL }
};

void
toolkit_init_widgets( void )
{
	int i, n = 0;
	struct namnod *nam;

	if (C[0].class != NULL)
		return;
	/*
	 * NOTE: keep these in alphabetical order because
	 * the widget table above is in the same order.
	 */
	C[n].disciplines = setArrowDisciplines();
	C[n++].class = xmArrowButtonWidgetClass;;
	C[n].disciplines = setArrowDisciplines();
	C[n++].class = xmArrowButtonGadgetClass;;
	C[n++].class = xmBulletinBoardWidgetClass;
	C[n++].class = xmCascadeButtonWidgetClass;
	C[n++].class = xmCascadeButtonGadgetClass;
	C[n].disciplines = setComboDisciplines();
	C[n++].class = xmComboBoxWidgetClass;
	C[n].disciplines = setCmdDisciplines();
	C[n].resfix = &command_fixups[0];
	C[n++].class = xmCommandWidgetClass;
	C[n].resfix = &vendor_fixups[0];
	C[n++].class = xmDialogShellWidgetClass;
	C[n].disciplines = setDAreaDisciplines();
	C[n++].class = xmDrawingAreaWidgetClass;
	C[n].disciplines = setDBtnDisciplines();
	C[n++].class = xmDrawnButtonWidgetClass;
	C[n].resfix = &fileselbox_fixups[0];
	C[n].disciplines = setFSelDisciplines();
	C[n++].class = xmFileSelectionBoxWidgetClass;
	C[n++].class = xmFormWidgetClass;
	C[n++].class = xmFrameWidgetClass;
	C[n++].class = xmLabelWidgetClass;
	C[n++].class = xmLabelGadgetClass;
	C[n].resfix = &list_fixups[0];
	C[n].disciplines = setListDisciplines();
	C[n++].class = xmListWidgetClass;
	C[n].resfix = &mainwindow_fixups[0];
	C[n++].class = xmMainWindowWidgetClass;
	C[n++].class = xmMenuShellWidgetClass;
	C[n++].class = xmMessageBoxWidgetClass;
	C[n].confix = &panedwin_con_fixups[0];
	C[n++].class = xmPanedWindowWidgetClass;
	C[n].disciplines = setPBtnDisciplines();
	C[n++].class = xmPushButtonWidgetClass;
	C[n].disciplines = setPBtnDisciplines();
	C[n++].class = xmPushButtonGadgetClass;
	C[n].disciplines = setRCDisciplines();
	C[n++].class = xmRowColumnWidgetClass;
	C[n].disciplines = setScaleDisciplines();
	C[n++].class = xmScaleWidgetClass;
	C[n].disciplines = setSBarDisciplines();
	C[n++].class = xmScrollBarWidgetClass;
	C[n].disciplines = setSWinDisciplines();
	C[n++].class = xmScrolledWindowWidgetClass;
	C[n].disciplines = setSBoxDisciplines();
	C[n].resfix = &selbox_fixups[0];
	C[n++].class = xmSelectionBoxWidgetClass;
	C[n++].class = xmSeparatorWidgetClass;
	C[n++].class = xmSeparatorGadgetClass;
	C[n++].class = xmSimpleSpinBoxWidgetClass;
	C[n++].class = xmTearOffButtonWidgetClass;
	C[n].disciplines = setTextDisciplines();
	C[n++].class = xmTextWidgetClass;
	C[n].disciplines = setTextDisciplines();
	C[n++].class = xmTextFieldWidgetClass;
	C[n].disciplines = setTBtnDisciplines();
	C[n++].class = xmToggleButtonWidgetClass;
	C[n].disciplines = setTBtnDisciplines();
	C[n++].class = xmToggleButtonGadgetClass;

	C[n].resfix = &vendor_fixups[0];
	C[n++].class = applicationShellWidgetClass;
	C[n++].class = overrideShellWidgetClass;
	C[n++].class = shellWidgetClass;
	C[n].resfix = &vendor_fixups[0];
	C[n++].class = topLevelShellWidgetClass;
	C[n].resfix = &vendor_fixups[0];
	C[n++].class = transientShellWidgetClass;
	C[n].resfix = &vendor_fixups[0];
	C[n++].class = vendorShellWidgetClass;
	C[n].resfix = &wmshell_fixups[0];
	C[n++].class = wmShellWidgetClass;

	C[n].resfix = &helpDialogs_fixups[0];
	C[n].disciplines = setHelpDisciplines();
	C[n++].class = dtHelpDialogWidgetClass;
	C[n].resfix = &helpDialogs_fixups[0];
	C[n].disciplines = setHelpDisciplines();
	C[n++].class = dtHelpQuickDialogWidgetClass;
	C[n].disciplines = setDtPrintSetupProcDisciplines();
	C[n++].class = dtPrintSetupBoxWidgetClass;
}


/*****************************************************************************
 *****************************************************************************
 *
 * At initialization time, these functions create the structures which
 * map a callback name to its associated discipline table.
 *
 *****************************************************************************
 *****************************************************************************/

static discInfo *
setArrowDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 4);

   info[0].callbackName = strdup(XmNactivateCallback);
   info[0].discipline = (void *)&arrowDisc;
   info[1].callbackName = strdup(XmNarmCallback);
   info[1].discipline = (void *)&arrowDisc;
   info[2].callbackName = strdup(XmNdisarmCallback);
   info[2].discipline = (void *)&arrowDisc;
   info[3].callbackName = NULL;
   info[3].discipline = NULL;
   return(info);
}

static discInfo *
setComboDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 2);

   info[0].callbackName = strdup(XmNselectionCallback);
   info[0].discipline = (void *)&comboDisc;
   info[1].callbackName = NULL;
   info[1].discipline = NULL;
   return(info);
}

static discInfo *
setCmdDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 3);

   info[0].callbackName = strdup(XmNcommandChangedCallback);
   info[0].discipline = (void *)&cmdDisc;
   info[1].callbackName = strdup(XmNcommandEnteredCallback);
   info[1].discipline = (void *)&cmdDisc;
   info[2].callbackName = NULL;
   info[2].discipline = NULL;
   return(info);
}

static discInfo *
setDAreaDisciplines( void )

{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 4);

   info[0].callbackName = strdup(XmNexposeCallback);
   info[0].discipline = (void *)&dAreaDisc;
   info[1].callbackName = strdup(XmNinputCallback);
   info[1].discipline = (void *)&dAreaDisc;
   info[2].callbackName = strdup(XmNresizeCallback);
   info[2].discipline = (void *)&dAreaDisc;
   info[3].callbackName = NULL;
   info[3].discipline = NULL;
   return(info);
}

static discInfo *
setDBtnDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 6);

   info[0].callbackName = strdup(XmNactivateCallback);
   info[0].discipline = (void *)&dbtnDisc;
   info[1].callbackName = strdup(XmNarmCallback);
   info[1].discipline = (void *)&dbtnDisc;
   info[2].callbackName = strdup(XmNdisarmCallback);
   info[2].discipline = (void *)&dbtnDisc;
   info[3].callbackName = strdup(XmNexposeCallback);
   info[3].discipline = (void *)&dbtnDisc;
   info[4].callbackName = strdup(XmNresizeCallback);
   info[4].discipline = (void *)&dbtnDisc;
   info[5].callbackName = NULL;
   info[5].discipline = NULL;
   return(info);
}

static discInfo *
setDtPrintSetupProcDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 2);

	/* Since the same discipline is used for all of DtPrintSetupBox's 
	 * DtPrintSetupProcs, we can use DtRPrintSetupProc for all of them
	 * to avoid defining unnecessary info entries.
	 */
   info[0].callbackName = strdup(DtRPrintSetupProc);
   info[0].discipline = (void *)&dtPrintDisc;
   info[1].callbackName = NULL;
   info[1].discipline = NULL;
   return(info);
}

static discInfo *
setFSelDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 5);

   info[0].callbackName = strdup(XmNapplyCallback);
   info[0].discipline = (void *)&fselDisc;
   info[1].callbackName = strdup(XmNcancelCallback);
   info[1].discipline = (void *)&fselDisc;
   info[2].callbackName = strdup(XmNnoMatchCallback);
   info[2].discipline = (void *)&fselDisc;
   info[3].callbackName = strdup(XmNokCallback);
   info[3].discipline = (void *)&fselDisc;
   info[4].callbackName = NULL;
   info[4].discipline = NULL;
   return(info);
}

static discInfo *
setListDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 6);

   info[0].callbackName = strdup(XmNbrowseSelectionCallback);
   info[0].discipline = (void *)&listDisc;
   info[1].callbackName = strdup(XmNdefaultActionCallback);
   info[1].discipline = (void *)&listDisc;
   info[2].callbackName = strdup(XmNextendedSelectionCallback);
   info[2].discipline = (void *)&listDisc;
   info[3].callbackName = strdup(XmNmultipleSelectionCallback);
   info[3].discipline = (void *)&listDisc;
   info[4].callbackName = strdup(XmNsingleSelectionCallback);
   info[4].discipline = (void *)&listDisc;
   info[5].callbackName = NULL;
   info[5].discipline = NULL;
   return(info);
}

static discInfo *
setPBtnDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 4);

   info[0].callbackName = strdup(XmNactivateCallback);
   info[0].discipline = (void *)&pbtnDisc;
   info[1].callbackName = strdup(XmNarmCallback);
   info[1].discipline = (void *)&pbtnDisc;
   info[2].callbackName = strdup(XmNdisarmCallback);
   info[2].discipline = (void *)&pbtnDisc;
   info[3].callbackName = NULL;
   info[3].discipline = NULL;
   return(info);
}

static discInfo *
setRCDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 6);

   info[0].callbackName = strdup(XmNentryCallback);
   info[0].discipline = (void *)&rcDisc;
   info[1].callbackName = strdup(XmNmapCallback);
   info[1].discipline = (void *)&rcDisc;
   info[2].callbackName = strdup(XmNtearOffMenuActivateCallback);
   info[2].discipline = (void *)&rcDisc;
   info[3].callbackName = strdup(XmNtearOffMenuDeactivateCallback);
   info[3].discipline = (void *)&rcDisc;
   info[4].callbackName = strdup(XmNunmapCallback);
   info[4].discipline = (void *)&rcDisc;
   info[5].callbackName = NULL;
   info[5].discipline = NULL;
   return(info);
}

static discInfo *
setScaleDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 3);

   info[0].callbackName = strdup(XmNdragCallback);
   info[0].discipline = (void *)&scaleDisc;
   info[1].callbackName = strdup(XmNvalueChangedCallback);
   info[1].discipline = (void *)&scaleDisc;
   info[2].callbackName = NULL;
   info[2].discipline = NULL;
   return(info);
}

static discInfo *
setSBarDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 9);

   info[0].callbackName = strdup(XmNdecrementCallback);
   info[0].discipline = (void *)&sbarDisc;
   info[1].callbackName = strdup(XmNdragCallback);
   info[1].discipline = (void *)&sbarDisc;
   info[2].callbackName = strdup(XmNincrementCallback);
   info[2].discipline = (void *)&sbarDisc;
   info[3].callbackName = strdup(XmNpageDecrementCallback);
   info[3].discipline = (void *)&sbarDisc;
   info[4].callbackName = strdup(XmNpageIncrementCallback);
   info[4].discipline = (void *)&sbarDisc;
   info[5].callbackName = strdup(XmNtoBottomCallback);
   info[5].discipline = (void *)&sbarDisc;
   info[6].callbackName = strdup(XmNtoTopCallback);
   info[6].discipline = (void *)&sbarDisc;
   info[7].callbackName = strdup(XmNvalueChangedCallback);
   info[7].discipline = (void *)&sbarDisc;
   info[8].callbackName = NULL;
   info[8].discipline = NULL;
   return(info);
}

static discInfo *
setSWinDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 2);

   info[0].callbackName = strdup(XmNtraverseObscuredCallback);
   info[0].discipline = (void *)&swinDisc;
   info[1].callbackName = NULL;
   info[1].discipline = NULL;
   return(info);
}

static discInfo *
setSBoxDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 5);

   info[0].callbackName = strdup(XmNapplyCallback);
   info[0].discipline = (void *)&sboxDisc;
   info[1].callbackName = strdup(XmNcancelCallback);
   info[1].discipline = (void *)&sboxDisc;
   info[2].callbackName = strdup(XmNnoMatchCallback);
   info[2].discipline = (void *)&sboxDisc;
   info[3].callbackName = strdup(XmNokCallback);
   info[3].discipline = (void *)&sboxDisc;
   info[4].callbackName = NULL;
   info[4].discipline = NULL;
   return(info);
}

static discInfo *
setTextDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 10);

   info[0].callbackName = strdup(XmNactivateCallback);
   info[0].discipline = (void *)&dftDisc;
   info[1].callbackName = strdup(XmNfocusCallback);
   info[1].discipline = (void *)&dftDisc;
   info[2].callbackName = strdup(XmNgainPrimaryCallback);
   info[2].discipline = (void *)&dftDisc;
   info[3].callbackName = strdup(XmNlosePrimaryCallback);
   info[3].discipline = (void *)&dftDisc;
   info[4].callbackName = strdup(XmNlosingFocusCallback);
   info[4].discipline = (void *)&textDisc;
   info[5].callbackName = strdup(XmNmodifyVerifyCallback);
   info[5].discipline = (void *)&textDisc;
   info[6].callbackName = strdup(XmNmodifyVerifyCallbackWcs);
   info[6].discipline = (void *)&textDisc2;
   info[7].callbackName = strdup(XmNmotionVerifyCallback);
   info[7].discipline = (void *)&textDisc;
   info[8].callbackName = strdup(XmNvalueChangedCallback);
   info[8].discipline = (void *)&dftDisc;
   info[9].callbackName = NULL;
   info[9].discipline = NULL;
   return(info);
}

static discInfo *
setTBtnDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 4);

   info[0].callbackName = strdup(XmNarmCallback);
   info[0].discipline = (void *)&tbtnDisc;
   info[1].callbackName = strdup(XmNdisarmCallback);
   info[1].discipline = (void *)&tbtnDisc;
   info[2].callbackName = strdup(XmNvalueChangedCallback);
   info[2].discipline = (void *)&tbtnDisc;
   info[3].callbackName = NULL;
   info[3].discipline = NULL;
   return(info);
}

static discInfo *
setHelpDisciplines( void )
{
   discInfo * info = (discInfo *)XtMalloc(sizeof(discInfo) * 3);

   info[0].callbackName = strdup(DtNhyperLinkCallback);
   info[0].discipline = (void *)&helpDisc;
   info[1].callbackName = strdup(DtNcloseCallback);
   info[1].discipline = (void *)&helpDisc;
   info[2].callbackName = NULL;
   info[2].discipline = NULL;
   return(info);
}
