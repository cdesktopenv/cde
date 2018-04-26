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
/* $TOG: editor.c /main/4 1998/04/02 18:18:47 rafi $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

/*
 * editor.c
 *
 * A simple editor based on the DtEditor widget.
 */

#include <stdio.h>
#include <locale.h>

#include <Xm/XmAll.h>

#include <Dt/Editor.h>

/*
 * Constants
 */

#define ApplicationClass "Editor"

/*
 * Types
 */

typedef struct _AppData {
	XtAppContext appContext;
	Display	*display;
	Widget top;
	Widget mainWindow;
	Widget menuBar;
	Widget editor;
	Widget messageTextF;
	Widget fileSelectionBox;
	Widget cutButton;
	Widget copyButton;
	Widget clearButton;
	Widget deleteButton;
	Widget deselectButton;
	Widget cutPopupButton;
	Widget copyPopupButton;
	Boolean wordWrapOn;
	Boolean statusLineOn;
	Boolean overstrikeModeOn;
} AppData;

/*
 * Used to specify whether data is being loaded, inserted, or saved
 * from the editor. Used used by the FSB and its callbacks.
 */

typedef enum _LoadSaveType {
        LOAD_DATA,
        INSERT_DATA,
        SAVE_DATA
} LoadSaveType;

/*
 * Data
 */

static AppData ad;


/*
 * Functions
 */

static void SetResizeHints(void);
static void DisplayMessage(char *);
static Widget CreateMenuBar(Widget);
static Widget CreatePopupMenu(Widget);
static Widget CreateEditor(Widget);
static Widget CreateButton(Widget, String, char, XtCallbackProc, XtPointer);
static Widget CreateToggle(Widget, String, char, XtCallbackProc, XtPointer, Boolean);
static Widget CreateCascade(Widget, String, char, Widget);
static void PopupHandler(Widget, XtPointer, XEvent*, Boolean*);
static void SetSelectionState(Boolean);


/*
 * main
 */

void main(int argc, char **argv)
{
    Arg	al[10];
    int ac;

    XtSetLanguageProc( (XtAppContext)NULL, (XtLanguageProc)NULL,
    		       (XtPointer)NULL );

    /* Initialize the application's data */
    ad.fileSelectionBox = (Widget) NULL;
    ad.wordWrapOn = False;
    ad.statusLineOn = False;
    ad.overstrikeModeOn = False;

    /* Initialize the toolkit and open the display */
    ad.top = XtAppInitialize(&ad.appContext, ApplicationClass, NULL, 0,
                             &argc, argv, NULL, NULL, 0);
    ad.display = XtDisplay(ad.top);

    /* Create MainWindow. */
    ac = 0;
    ad.mainWindow = (Widget) XmCreateMainWindow (ad.top, "main", al, ac);
    XtManageChild (ad.mainWindow);

    /* Create MenuBar in MainWindow. */
    ad.menuBar = CreateMenuBar(ad.mainWindow);
    XtManageChild(ad.menuBar);

    /* Create editor widget in MainWindow. */
    ad.editor = CreateEditor(ad.mainWindow);
    XtManageChild(ad.editor);
    ad.messageTextF = DtEditorGetMessageTextFieldID(ad.editor);

    /* Create the editor popup menu */
    CreatePopupMenu(ad.editor);

    /* Set the main window widgets. */
    XmMainWindowSetAreas(ad.mainWindow, ad.menuBar, NULL, NULL, NULL, ad.editor);

    /* Realize toplevel widget */
    XtRealizeWidget (ad.top);

    /* Set the resize increment and minimum window size properties. */
    SetResizeHints();

    /* Set up menu buttons dependent on selection */
    SetSelectionState(False);

    XtAppMainLoop(ad.appContext);
} 


/************************************************************************
 *
 * Callbacks
 *
 ************************************************************************/

/*
 * File menu callbacks
 */

static void ResetEditorCb(Widget w, XtPointer cd, XtPointer cb)
{
   DtEditorReset(ad.editor);
}

static void OpenFileCb( Widget w, XtPointer cd, XtPointer cb)
{
   DtEditorErrorCode error;
   XmFileSelectionBoxCallbackStruct *fcb = (XmFileSelectionBoxCallbackStruct *) cb;
   LoadSaveType LoadSaveFlag = (LoadSaveType)cd;
   char *name = (char *) XtMalloc( sizeof(char) * fcb->length + 1 );

   name[0] ='\0';

   /*
    *  Get the name of the file & pass it to the editor widget
    */

   name = XmStringUnparse(fcb->value, NULL, XmCHARSET_TEXT,
                          XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

   /*
    * Load or insert the file, as specified
    */
   if ( LoadSaveFlag == LOAD_DATA )
     error = DtEditorSetContentsFromFile(ad.editor, name); 
   else
     error = DtEditorInsertFromFile(ad.editor, name); 

   switch (error)
   {
     case DtEDITOR_NO_ERRORS:
     {
       DisplayMessage("File loaded");
       break;
     }
     case DtEDITOR_NULLS_REMOVED:
     {
	DisplayMessage( "All embedded null characters removed from the file" );
	break;
     }
     case DtEDITOR_READ_ONLY_FILE:
     {
       DisplayMessage( "The file is write protected" );
       break;
     }
     case DtEDITOR_NONEXISTENT_FILE:
     {
       DisplayMessage( "Could not find file" );
       break;
     }
     case DtEDITOR_DIRECTORY:
     {
       DisplayMessage( "The name given is a directory" );
       break;
     }
     case DtEDITOR_CHAR_SPECIAL_FILE:
     {
       DisplayMessage( "The name given is a character special device" );
       break;
     }
     case DtEDITOR_BLOCK_MODE_FILE:
     {
       DisplayMessage( "The name given is a block mode device");
       break;
     }
     case DtEDITOR_INSUFFICIENT_MEMORY:
     {
       DisplayMessage( "Not enough available memory to load file");
       break;
     }
     case DtEDITOR_UNREADABLE_FILE:
     default:
     {
       DisplayMessage( "Could not read the file" );
       break;
     }
   }

   /*
    * Remove the OK callback so it can be added again with a new value for 
    * the LoadSaveFlag flag.
    */
   XtRemoveCallback(w, XmNokCallback, OpenFileCb, cd);

   /*
    * Remove the FSB dialog & clean up
    */
   XtUnmanageChild(w);
   XtFree (name);
}


static void SaveAsFileCb(Widget w, XtPointer cd, XtPointer cb)
{
   DtEditorContentRec	cr;
   DtEditorErrorCode	error;
   Boolean		overWrite = False,
			hardCarriageReturns = True,
			markContentsAsSaved = True;
   XmFileSelectionBoxCallbackStruct *fcb = (XmFileSelectionBoxCallbackStruct *)cb;
   LoadSaveType LoadSaveFlag = (LoadSaveType) cd;

   char *name = XtMalloc(sizeof(char) * fcb->length + 1 );
   name[0] ='\0';
   XmStringGetLtoR(fcb->value, XmFONTLIST_DEFAULT_TAG, &name);

   /*
    * Ask the widget to save its contents to the named file.
    */

   error = DtEditorSaveContentsToFile(ad.editor, name, overWrite,
		hardCarriageReturns, markContentsAsSaved); 

   switch(error)
   {
     case DtEDITOR_NO_ERRORS:
     {
       DisplayMessage( "The file has been saved" );
       break;
     }
     case DtEDITOR_UNWRITABLE_FILE:
     {
       DisplayMessage( "The file is read only" ); 
       break;
     }
     case DtEDITOR_WRITABLE_FILE:
     {
       DisplayMessage( "File not saved, it already exists" ); 
       break;
     }
     case DtEDITOR_DIRECTORY:
     {
       DisplayMessage( "The name given is a directory" );
       break;
     }
     case DtEDITOR_CHAR_SPECIAL_FILE:
     {
       DisplayMessage( "The name given is a character special device" );
       break;
     }
     case DtEDITOR_BLOCK_MODE_FILE:
     {
       DisplayMessage( "The name given is a block mode device");
       break;
     }
     case DtEDITOR_SAVE_FAILED:
     default:
     {
       DisplayMessage( "Could not save the file.  Check disc space" );
       break;
     }
   }

   /*
    * Remove the OK callback so it can be added again with a new value for 
    * the LoadSaveFlag flag.
    */
   XtRemoveCallback(w, XmNokCallback, SaveAsFileCb, cd);

   /*
    * Remove the FSB dialog & clean up
    */
   XtUnmanageChild( w );
   XtFree (name);

}


static void CancelOpenCb(Widget w, XtPointer cd, XtPointer cb)
{
   /* Remove the OK callback so it can be added again with a new value for 
    * the LoadSaveFlag flag.
    */
   XtRemoveCallback(w, XmNokCallback, OpenFileCb, cd);

   /* Remove the FSB dialog & clean up */
   XtUnmanageChild(w);
}


static void DisplayFsbCb(Widget w, XtPointer cd, XtPointer cb)
{
  Arg al[10];
  int ac;
  XmString tmpStr1, tmpStr2;

  LoadSaveType LoadSaveFlag = (LoadSaveType) cd;

  /* Create the FSB, if we need to */

  if (ad.fileSelectionBox == (Widget) NULL)
  {
     ac = 0;
     ad.fileSelectionBox = XmCreateFileSelectionDialog(ad.mainWindow,
						    "file_sel_dlg",
						    al, ac);
     XtAddCallback(ad.fileSelectionBox, XmNcancelCallback, CancelOpenCb,	
						(XtPointer)LoadSaveFlag);

  }

  /*
   * Set FSB title & label depending up whether loading, inserting, or
   * saving a container.
   */
  switch ( LoadSaveFlag )
  {
    case LOAD_DATA:
    {
      tmpStr1 = XmStringCreateLocalized("Open a File");
      tmpStr2 = XmStringCreateLocalized("File to open");

      /*
       * Add the OK callback so the curent value of the LoadSaveFlag 
       * flag is passed in.
       */
      XtAddCallback(ad.fileSelectionBox, XmNokCallback, OpenFileCb,
						(XtPointer)LoadSaveFlag);
      break;
    }

    case INSERT_DATA:
    {
      tmpStr1 = XmStringCreateLocalized("Include a File");
      tmpStr2 = XmStringCreateLocalized("File to include");

      /*
       * Add the OK callback so the curent value of the LoadSaveFlag 
       * flag is passed in.
       */
      XtAddCallback(ad.fileSelectionBox, XmNokCallback, OpenFileCb,
						(XtPointer)LoadSaveFlag);
      break;
    }

    case SAVE_DATA:
    {
      tmpStr1 = XmStringCreateLocalized("Save to File");
      tmpStr2 = XmStringCreateLocalized("Save to file");

      /*
       * Add the OK callback so the save as callback is called.
       */
      XtAddCallback(ad.fileSelectionBox, XmNokCallback, SaveAsFileCb,
						(XtPointer)LoadSaveFlag);
      break;
    }

    default:
    {
	break;
    }
  }

  XtVaSetValues(ad.fileSelectionBox,
	XmNdialogTitle, tmpStr1,
  	XmNselectionLabelString, tmpStr1,
	NULL);

  XmStringFree(tmpStr1);
  XmStringFree(tmpStr2);

  /* Display the FSB */

  XtManageChild (ad.fileSelectionBox);
}


static void ExitCb(Widget w, XtPointer cd, XtPointer cb)
{
    exit(0);
}

/***************************************************** 
 *
 * Edit menu callbacks
 *
 */

static void UndoCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorUndoEdit(ad.editor); 
}

static void CutCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorCutToClipboard(ad.editor); 
}

static void CopyCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorCopyToClipboard(ad.editor); 
}

static void PasteCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorPasteFromClipboard(ad.editor); 
}

static void ClearCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorClearSelection(ad.editor);
}

static void DeleteCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorDeleteSelection(ad.editor);
}

static void SelectAllCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorSelectAll(ad.editor); 
}

static void DeselectCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorDeselect(ad.editor); 
}

static void FindCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorInvokeFindChangeDialog(ad.editor); 
}

static void SpellCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorInvokeSpellDialog(ad.editor); 
}

/***************************************************** 
 *
 * Format menu callbacks
 *
 */

static void FormatAllCb(Widget w, XtPointer cd, XtPointer cb)
{
   DtEditorFormat(ad.editor, (DtEditorFormatSettings *) NULL, 
		  DtEDITOR_FORMAT_ALL); 
}

static void FormatParaCb(Widget w, XtPointer cd, XtPointer cb)
{
   DtEditorFormat(ad.editor, (DtEditorFormatSettings *) NULL, 
		  DtEDITOR_FORMAT_PARAGRAPH); 
}

static void InvokeFormatDlgCb(Widget w, XtPointer cd, XtPointer cb)
{
    DtEditorInvokeFormatDialog(ad.editor); 
}

/***************************************************** 
 *
 * Options menu callbacks
 *
 */

static void OverstrikeCb(Widget w, XtPointer cd, XtPointer cb)
{
    ad.overstrikeModeOn = ad.overstrikeModeOn ? False : True;
    XtVaSetValues(ad.editor, DtNoverstrike, ad.overstrikeModeOn, NULL); 
}

static void StatusLineCb(Widget w, XtPointer cd, XtPointer cb)
{
    ad.statusLineOn = ad.statusLineOn ? False : True;
    XtVaSetValues(ad.editor, DtNshowStatusLine, ad.statusLineOn, NULL); 

    /* Reset the resize increment and minimum window size properties. */
   SetResizeHints();

}

static void WordWrapCb(Widget w, XtPointer cd, XtPointer cb)
{
    ad.wordWrapOn = ad.wordWrapOn ? False : True;
    XtVaSetValues(ad.editor, DtNwordWrap, ad.wordWrapOn, NULL); 
}

/***************************************************** 
 *
 * Editor callbacks
 *
 */

static void HelpCb(Widget w, XtPointer cd, XtPointer cb)
{
  DtEditorHelpCallbackStruct *ecb = (DtEditorHelpCallbackStruct *)cb;

  DisplayMessage( "Received a request for help");
}

static void TextSelectedCb(Widget w, XtPointer cd, XtPointer cb)
{
    SetSelectionState(True);
}

static void TextUnselectedCb(Widget w, XtPointer cd, XtPointer cb)
{
    SetSelectionState(False);
}

static void SetSelectionState(Boolean state)
{
    XtSetSensitive(ad.cutButton, state);
    XtSetSensitive(ad.copyButton, state);
    XtSetSensitive(ad.deleteButton, state);
    XtSetSensitive(ad.clearButton, state);
    XtSetSensitive(ad.deselectButton, state);
    XtSetSensitive(ad.cutPopupButton, state);
    XtSetSensitive(ad.copyPopupButton, state);
}

/************************************************************************
 *
 * PROCEDURES 
 *
 ************************************************************************/

/************************************************************************
 *
 * SetResizeHints - Set the resize increment properties
 *
 */

static void SetResizeHints(void)
{
   XSizeHints  sh;
   Dimension MBheight;

   DtEditorGetSizeHints(ad.editor, &sh);

   /*
    * Add Menu Bar height to the height of the Editor widget
    */
   XtVaGetValues( ad.menuBar, XmNheight, &MBheight, NULL);
   sh.min_height += MBheight;
   sh.base_height += MBheight;

   XSetWMSizeHints(ad.display, XtWindow(ad.top), &sh, XA_WM_NORMAL_HINTS);
}

/************************************************************************
 *
 * DisplayMessage - Display message in DtEditor's message area
 *
 */

static void DisplayMessage(char *message)
{
    XmTextFieldSetString(ad.messageTextF, message);
}

/************************************************************************
 *
 * CreateFileMenu - Create the File pulldown menu
 *
 */

static Widget CreateFileMenu(Widget menuBar)
{
    Widget fileMenu; 

    fileMenu = (Widget) XmCreatePulldownMenu (menuBar, "fileMenu", NULL, 0);
    CreateCascade(menuBar, "File", 'F', fileMenu);
    CreateButton(fileMenu, "New", 'N', ResetEditorCb, NULL);
    CreateButton(fileMenu, "Open...", 'O', DisplayFsbCb, (XtPointer)LOAD_DATA);
    CreateButton(fileMenu, "Include...", 'I', DisplayFsbCb,
							(XtPointer)INSERT_DATA);
    XtManageChild(XmCreateSeparatorGadget(fileMenu, "sep1", NULL, 0));
    CreateButton(fileMenu, "Save As...", 'A', DisplayFsbCb, (XtPointer)SAVE_DATA);
    XtManageChild(XmCreateSeparatorGadget(fileMenu, "sep2", NULL, 0));
    CreateButton(fileMenu, "Exit", 'x', ExitCb, NULL);

    return (fileMenu);
}

/************************************************************************
 *
 * CreateEditMenu - Create the Edit pulldown menu
 *
 */

static Widget CreateEditMenu(Widget menuBar)
{
    Widget editMenu;

    editMenu = XmCreatePulldownMenu (menuBar, "editMenu", NULL, 0);

    CreateCascade(menuBar, "Edit", 'E', editMenu);

    CreateButton(editMenu, "Undo", 'U', UndoCb, NULL);
    XtManageChild(XmCreateSeparatorGadget(editMenu, "sep1", NULL, 0));
    ad.cutButton = CreateButton(editMenu, "Cut", 't', CutCb, NULL);
    ad.copyButton = CreateButton(editMenu, "Copy", 'C', CopyCb, NULL);
    CreateButton(editMenu, "Paste", 'P', PasteCb, NULL);
    ad.clearButton = CreateButton(editMenu, "Clear", 'e', ClearCb, NULL);
    ad.deleteButton = CreateButton(editMenu, "Delete", 't', DeleteCb, NULL);
    CreateButton(editMenu, "Select All", 'S', SelectAllCb, NULL);
    ad.deselectButton = CreateButton(editMenu, "Deselect", 'D', DeselectCb, NULL);
    XtManageChild(XmCreateSeparatorGadget(editMenu, "sep2", NULL, 0));
    CreateButton(editMenu, "Find/Change...", 'F', FindCb, (XtPointer)ad.editor);
    CreateButton(editMenu, "Check Spelling...", 'p', SpellCb, (XtPointer)ad.editor);

    return (editMenu);
}


/************************************************************************
 *
 * CreateFormatMenu - Create the FormatEdit pulldown menu
 *
 */

static Widget CreateFormatMenu(Widget menuBar)
{
    Widget formatMenu;

    formatMenu = XmCreatePulldownMenu (menuBar, "formatMenu", NULL, 0);

    CreateCascade(menuBar, "Format", 'r', formatMenu);

    CreateButton(formatMenu, "Settings...", 'S', InvokeFormatDlgCb, NULL);
    CreateButton(formatMenu, "All", 'A', FormatAllCb, NULL);
    CreateButton(formatMenu, "Paragraph", 'P', FormatParaCb, NULL);

    return (formatMenu);
}

/************************************************************************
 *
 * CreateOptionsMenu - Create the Options pulldown menu
 *
 */

static Widget CreateOptionsMenu(Widget menuBar)
{
    Widget optionsMenu;

    optionsMenu = XmCreatePulldownMenu (menuBar, "optionsMenu", NULL, 0);

    CreateCascade(menuBar, "Options", 'O', optionsMenu);

    CreateToggle(optionsMenu, "Overstrike", 'O', OverstrikeCb, NULL, ad.overstrikeModeOn);
    CreateToggle(optionsMenu, "Word Wrap", 'W', WordWrapCb, NULL,ad.wordWrapOn);
    XtManageChild(XmCreateSeparatorGadget(optionsMenu, "sep1", NULL, 0));
    CreateToggle(optionsMenu, "Status Line", 'S', StatusLineCb, NULL, ad.statusLineOn);

    return (optionsMenu);
}

/*
 * CreatePopupMenu - Create popup menu in editor window
 */

static Widget CreatePopupMenu(Widget parent)
{ 
    Widget popupMenu;
    Widget fileMenu;
    Widget editMenu;

    popupMenu = XmCreatePopupMenu(parent, "popupMenu", NULL, 0);

    XtAddEventHandler(parent, ButtonPressMask, False, PopupHandler,
							(XtPointer)popupMenu);

    fileMenu = XmCreatePulldownMenu (popupMenu, "fileMenu", NULL, 0);
    CreateCascade(popupMenu, "File", 'F', fileMenu);
    CreateButton(fileMenu, "Open...", 'O', DisplayFsbCb, (XtPointer)LOAD_DATA);
    CreateButton(fileMenu, "Include...", 'I', DisplayFsbCb,
							(XtPointer)INSERT_DATA);
    CreateButton(fileMenu, "Save As...", 'A', DisplayFsbCb, (XtPointer)SAVE_DATA);
    CreateButton(fileMenu, "New", 'N', ResetEditorCb, NULL);

    editMenu = XmCreatePulldownMenu (popupMenu, "editMenu", NULL, 0);
    CreateCascade(popupMenu, "Edit", 'E', editMenu);
    CreateButton(editMenu, "Undo", 'U', UndoCb, (XtPointer)LOAD_DATA);
    ad.cutPopupButton = CreateButton(editMenu, "Cut", 't', CutCb, NULL);
    ad.copyPopupButton = CreateButton(editMenu, "Copy", 'C', CopyCb, NULL);
    CreateButton(editMenu, "Paste", 'P', PasteCb, NULL);

    CreateButton(popupMenu, "Find/Change...", 'F', FindCb, (XtPointer)ad.editor);

    return (popupMenu);
}

static void PopupHandler(Widget w, XtPointer cd, XEvent *event, Boolean *ctd)
{
    if (((XButtonEvent *)event)->button != Button3) return;

    XmMenuPosition((Widget)cd, (XButtonEvent *)event);
    XtManageChild ((Widget)cd);
}


/*
 * CreateMenuBar - Create MenuBar in MainWindow
 */

static Widget CreateMenuBar(Widget parent)
{ 
    Widget menuBar;

    menuBar = XmCreateMenuBar(parent, "menuBar", NULL, 0);

    CreateFileMenu(menuBar);
    CreateEditMenu(menuBar);
    CreateFormatMenu(menuBar);
    CreateOptionsMenu(menuBar);

    return (menuBar);
}


/*
 * CreateEditor - Create the editor widget
 */

static Widget CreateEditor(Widget parent)
{
    Widget w;
    Arg al[10];
    int ac;

    /* create editor widget */

    ac = 0;
    XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], DtNrows, 40); ac++;
    XtSetArg(al[ac], DtNcolumns, 80); ac++;
    w = DtCreateEditor(parent, "editor", al, ac);

    /* Add callbacks */

    XtAddCallback(w, DtNtextSelectCallback, TextSelectedCb, (XtPointer) w);
    XtAddCallback(w, DtNtextDeselectCallback, TextUnselectedCb, (XtPointer) w);
    XtAddCallback(w, XmNhelpCallback, HelpCb, NULL);

    return(w);
}

static Widget CreateButton(Widget parent, String label, char mnemonic, XtCallbackProc callback, XtPointer callData)
{
    Widget button;
    XmString labelString;
    Arg al[10];
    int ac;

    labelString = XmStringCreateLocalized(label);

    ac = 0;
    XtSetArg(al[ac], XmNlabelString, labelString); ac++;
    XtSetArg(al[ac], XmNmnemonic, mnemonic); ac++;
    button = XmCreatePushButtonGadget(parent, label, al, ac);
    XtAddCallback(button, XmNactivateCallback, callback, callData);
    XtManageChild(button);

    XmStringFree(labelString);

    return(button);
} 

static Widget CreateToggle(Widget parent, String label, char mnemonic, XtCallbackProc callback, XtPointer callData, Boolean value)
{
    Widget button;
    XmString labelString;
    Arg al[10];
    int ac;

    labelString = XmStringCreateLocalized(label);

    ac = 0;
    XtSetArg(al[ac], XmNlabelString, labelString); ac++;
    XtSetArg(al[ac], XmNmnemonic, mnemonic); ac++;
    XtSetArg(al[ac], XmNvisibleWhenOff, True); ac++;
    XtSetArg(al[ac], XmNset, value); ac++;
    button = XmCreateToggleButtonGadget(parent, label, al, ac);
    XtAddCallback(button, XmNvalueChangedCallback, callback, callData);
    XtManageChild(button);

    XmStringFree(labelString);

    return(button);
} 

static Widget CreateCascade(Widget parent, String label, char mnemonic, Widget subMenu)
{
    Widget button;
    XmString labelString;
    Arg al[10];
    int ac;

    labelString = XmStringCreateLocalized(label);

    ac = 0;
    XtSetArg(al[ac], XmNlabelString, labelString); ac++;
    XtSetArg(al[ac], XmNsubMenuId, subMenu); ac++;
    XtSetArg(al[ac], XmNmnemonic, mnemonic); ac++;
    button = XmCreateCascadeButtonGadget(parent, label, al, ac);
    XtManageChild(button);

    XmStringFree(labelString);

    return(button);
} 
