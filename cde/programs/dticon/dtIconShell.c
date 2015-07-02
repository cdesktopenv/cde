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
/* $XConsortium: dtIconShell.c /main/5 1996/10/21 15:27:41 mgreess $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/*******************************************************************************
        dtIconShell.c

*******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MenuShell.h>
#include <Xm/CascadeB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Label.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/DrawnB.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <X11/Shell.h>
#include <Dt/TitleBox.h>

#define PIXMAP_HEIGHT   48
#define PIXMAP_WIDTH    48
#define COLOR_BUTTON_SIZE 30
#define VIEWPORT_INITIAL_HEIGHT 290

/*******************************************************************************
        Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

#include "externals.h"
#include "main.h"

#define    RES_CONVERT( res_name, res_value) \
    XtVaTypedArg, (res_name), XmRString, (res_value), strlen(res_value) + 1

static Widget instructionText;
extern Widget coordinateText;
static Widget cmdButtonForm;
static Widget cmdButtonMgr;
static Widget cmdForm;
static Widget rotateMenu_pb1;
static Widget rotateMenu_pb2;
static Widget flipMenu_pb1;
static Widget flipMenu_pb2;

Widget staticColorsBox;
Widget symbolicColorsBox;
Widget staticGreysBox;
Widget dtIconShell;
Widget mainWindow;
Widget mainForm;
Widget iconForm;
Widget iconImage;
Widget iconSize;
Widget monoImage;
Widget monoLabel;
Widget pointButton;
Widget lineButton;
Widget rectangleButton;
Widget circleButton;
Widget eraseButton;
Widget floodButton;
Widget polylineButton;
Widget polygonButton;
Widget ellipseButton;
Widget selectButton;
Widget fillToggle;
Widget stdColorPens;
Widget color_pb1;
Widget color_pb2;
Widget color_pb3;
Widget color_pb4;
Widget color_pb5;
Widget color_pb6;
Widget color_pb7;
Widget color_pb8;
Widget stdGreyPens;
Widget grey_pb1;
Widget grey_pb2;
Widget grey_pb3;
Widget grey_pb4;
Widget grey_pb5;
Widget grey_pb6;
Widget grey_pb7;
Widget grey_pb8;
Widget symbolicColorPens;
Widget fgColorToggle;
Widget bgColorToggle;
Widget tsColorToggle;
Widget bsColorToggle;
Widget selectColorToggle;
Widget transparentColorToggle;
Widget viewport;
Widget tabletBorder;
Widget tabletFrame;
Widget tablet;
Widget menu1;
Widget fileMenu;
Widget fileMenu_new_pb;
Widget fileMenu_open_pb;
Widget fileMenu_save_pb;
Widget fileMenu_saveAs_pb;
Widget fileMenu_quit_pb;
Widget fileMenu_top_pb;
Widget editMenu;
Widget editMenu_undo_pb;
Widget editMenu_cut_pb;
Widget editMenu_copy_pb;
Widget editMenu_paste_pb;
Widget rotateMenu;
Widget editMenu_rotate_pb;
Widget flipMenu;
Widget editMenu_flip_pb;
Widget editMenu_scale_pb;
Widget editMenu_resize_pb;
Widget editMenu_addHS_pb;
Widget editMenu_deleteHS_pb;
Widget editMenu_clear_pb;
Widget editMenu_grabImage_pb;
Widget editMenu_top_pb;
Widget optionsMenu;
Widget optionsMenu_grid;
Widget formatMenu;
Widget formatMenu_xbm_tb;
Widget formatMenu_xpm_tb;
Widget optionsMenu_format;
Widget magnificationMenu;
Widget magMenu_2x_tb;
Widget magMenu_3x_tb;
Widget magMenu_4x_tb;
Widget magMenu_5x_tb;
Widget magMenu_6x_tb;
Widget magMenu_8x_tb;
Widget magMenu_10x_tb;
Widget magMenu_12x_tb;
Widget optionsMenu_magnify;
Widget optionsMenu_top_pb;
Widget helpMenu;
Widget helpMenu_intro_pb;
Widget helpMenu_tasks_pb;
Widget helpMenu_ref_pb;
Widget helpMenu_item_pb;
Widget helpMenu_using_pb;
Widget helpMenu_version_pb;
Widget helpMenu_top_pb;

/*******************************************************************************
        The following are translation tables.
*******************************************************************************/

static char        *transTable1 = "#replace\n\
<Btn1Down>:tablet_event(\"DOWN\",\"1\")\n\
<Btn1Motion>:tablet_event(\"MOVE\",\"1\")\n\
<Btn1Up>:tablet_event(\"UP\",\"1\")\n\
<EnterWindow>:tablet_event(\"ENTER\",\"0\")\n\
<LeaveWindow>:tablet_event(\"LEAVE\",\"0\")\n\
<Expose>:tablet_event(\"EXPOSE\",\"0\")\n\
<FocusIn>:tablet_event(\"F_IN\",\"0\")\n\
<FocusOut>:tablet_event(\"F_OUT\",\"0\")\n\
<Motion>:tablet_event(\"MOVE\",\"0\")\n";

static char        *transTable2 = "#override\n\
<Expose>:tablet_event(\"EXPOSE\",\"0\")\n";

/*******************************************************************************
        Forward declarations of functions that are defined later in this file.
*******************************************************************************/

Widget create_dtIconShell();
static void NewGraphicsOp();

/*******************************************************************************
        The following are Action functions.
*******************************************************************************/

static void
action_tablet_event(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
    ProcessTabletEvent(w, event, params, *numParams);
}

/*******************************************************************************
        The following are callback functions.
*******************************************************************************/

static void
armCB_pointButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = POINT;
        NewGraphicsOp();
}

static void
armCB_lineButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = LINE;
        NewGraphicsOp();
}

static void
armCB_rectangleButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = RECTANGLE;
        NewGraphicsOp();
}

static void
armCB_circleButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = CIRCLE;
        NewGraphicsOp();
}

static void
armCB_eraseButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = ERASER;
        NewGraphicsOp();
}

static void
armCB_floodButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = FLOOD;
        NewGraphicsOp();
}

static void
armCB_polylineButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = POLYLINE;
        NewGraphicsOp();
}

static void
armCB_polygonButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = POLYGON;
        NewGraphicsOp();
}

static void
armCB_ellipseButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = ELLIPSE;
        NewGraphicsOp();
}

static void
armCB_selectButton(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        GraphicsOp = SELECT;
        NewGraphicsOp();
}

static void
valueChangedCB_fillToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Switch_FillSolids();
        NewGraphicsOp();
}

static void
valueChangedCB_color_pb1(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR1);
}

static void
valueChangedCB_color_pb2(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR2);
}

static void
valueChangedCB_color_pb3(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR3);
}

static void
valueChangedCB_color_pb4(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR4);
}

static void
valueChangedCB_color_pb5(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR5);
}

static void
valueChangedCB_color_pb6(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR6);
}

static void
valueChangedCB_color_pb7(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR7);
}

static void
valueChangedCB_color_pb8(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(COLOR8);
}

static void
valueChangedCB_grey_pb1(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY1);
}

static void
valueChangedCB_grey_pb2(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY2);
}

static void
valueChangedCB_grey_pb3(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY3);
}

static void
valueChangedCB_grey_pb4(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY4);
}

static void
valueChangedCB_grey_pb5(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY5);
}

static void
valueChangedCB_grey_pb6(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY6);
}

static void
valueChangedCB_grey_pb7(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY7);
}

static void
valueChangedCB_grey_pb8(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(GREY8);
}

static void
valueChangedCB_fgColorToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(FG_COLOR);
}

static void
valueChangedCB_bgColorToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(BG_COLOR);
}

static void
valueChangedCB_tsColorToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(TS_COLOR);
}

static void
valueChangedCB_bsColorToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(BS_COLOR);
}

static void
valueChangedCB_selectColorToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(SELECT_COLOR);
}

static void
valueChangedCB_transparentColorToggle(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Select_New_Pen(TRANS_COLOR);
}

static void
activateCB_fileMenu_new_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_New();
}

static void
activateCB_fileMenu_open_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Open();
}

static void
activateCB_fileMenu_save_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Save();
}

static void
activateCB_fileMenu_saveAs_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_SaveAs();
}

static void
activateCB_fileMenu_quit_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Quit();
}

static void
activateCB_editMenu_undo_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Undo();
}

static void
activateCB_editMenu_cut_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Cut();
}

static void
activateCB_editMenu_copy_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Copy(&CutCopy, &CutCopy_mono);
}

static void
activateCB_editMenu_paste_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Paste();
}

static void
activateCB_rotateMenu_pb1(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_RotateLeft();
}

static void
activateCB_rotateMenu_pb2(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_RotateRight();
}

static void
activateCB_flipMenu_pb1(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_FlipV();
}

static void
activateCB_flipMenu_pb2(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_FlipH();
}

static void
activateCB_editMenu_scale_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Scale();
}

static void
activateCB_editMenu_resize_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Resize();
}

static void
activateCB_editMenu_addHS_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_AddHotspot();
        XtSetSensitive( editMenu_deleteHS_pb, True );
}

static void
activateCB_editMenu_deleteHS_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_DeleteHotspot();
        XtSetSensitive( editMenu_deleteHS_pb, False );
}

static void
activateCB_editMenu_grabImage_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

      Process_GrabImage();
}

static void
activateCB_editMenu_clear_pb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_Clear();
}

static void
valueChangedCB_optionsMenu_grid(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        Process_GridState();
}

static void
valueChangedCB_formatMenu_xbm_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_FileFormat(FORMAT_XBM);
}

static void
valueChangedCB_formatMenu_xpm_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_FileFormat(FORMAT_XPM);
}

static void
valueChangedCB_magMenu_2x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(2);
}

static void
valueChangedCB_magMenu_3x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(3);
}

static void
valueChangedCB_magMenu_4x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(4);
}

static void
valueChangedCB_magMenu_5x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(5);
}

static void
valueChangedCB_magMenu_6x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(6);
}

static void
valueChangedCB_magMenu_8x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(8);
}

static void
valueChangedCB_magMenu_10x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(10);
}

static void
valueChangedCB_magMenu_12x_tb(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{

        New_MagFactor(12);
}

/*******************************************************************************
        The 'build_' function creates all the widgets
        using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget
build_dtIconShell( void )
{
        Widget        fileMenu_shell;
        Widget        editMenu_shell;
        Widget        rotateMenu_shell;
        Widget        flipMenu_shell;
        Widget        optionsMenu_shell;
        Widget        formatMenu_shell;
        Widget        magnificationMenu_shell;
        Widget        helpMenu_shell;
        XmString      tmpXmStr, tmpXmStr2, tmpXmStr3, tmpXmStr4;
        char         *tmpStr, *tmpStr2;
        static char   geometry[40];

        /* GETSTR uses a static buffer which may be overwritten */
        /* on subsequent calls, so save character in mnemonic   */
        char *        mnemonicStr;
        KeySym        mnemonic;

        /* use tmpStr as tmp string holder so GETSTR won't trash string */
        tmpStr2 = GETSTR(12,3, "Icon Editor - (UNTITLED)");
        tmpStr = XtNewString (tmpStr2);

        if (!session.useSession)
        {
            XtVaSetValues( TopLevel,
                           XmNminWidth, 10,
                           XmNminHeight, 10,
                           XmNtitle, tmpStr,
                           XmNiconName, GETSTR(10,66, "(UNTITLED)"),
                           XmNdeleteResponse, XmDO_NOTHING,
                           NULL);
            dtIconShell = TopLevel;
        }
        else
        {
            sprintf(geometry, "=%dx%d+%d+%d", session.width, session.height,
                    session.x, session.y);

            XtVaSetValues( TopLevel,
                           XmNminWidth, 10,
                           XmNminHeight, 10,
                           XmNtitle, tmpStr,
                           XmNiconName, GETSTR(10,66, "(UNTITLED)"),
                           XmNdeleteResponse, XmDO_NOTHING,
                           XmNgeometry, geometry,
                           XmNinitialState, session.iconicState,
                           NULL);
            dtIconShell = TopLevel;

        }

        mainWindow = XtVaCreateManagedWidget( "mainWindow",
                        xmMainWindowWidgetClass, dtIconShell,
                        NULL);

        mainForm = XtVaCreateManagedWidget( "mainForm",
                        xmFormWidgetClass, mainWindow,
                        NULL);


        tmpXmStr = GETXMSTR(12,204, "Pencil");
        instructionText = XtVaCreateManagedWidget( "instructionText",
                        xmLabelGadgetClass, mainForm,
                        XmNlabelString, tmpXmStr,
                        XmNtopOffset, 5,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNrightOffset, -230,
                        XmNrightAttachment, XmATTACH_OPPOSITE_FORM,
                        XmNleftOffset, 5,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = XmStringCreateLocalized ("       ");
        coordinateText = XtVaCreateManagedWidget( "coordinateText",
                        xmLabelGadgetClass, mainForm,
                        XmNlabelString, tmpXmStr,
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNtopOffset, 5,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNbottomOffset, 1,
                        XmNbottomWidget,   viewport,
                        XmNrightOffset, 70,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftOffset, 230,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        NULL);
        XmStringFree(tmpXmStr);


        iconForm = XtVaCreateManagedWidget( "iconForm",
                        xmFormWidgetClass, mainForm,
                        XmNtopWidget, instructionText,
                        XmNtopOffset, 5,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 5,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL);

        iconImage = XtVaCreateManagedWidget( "iconImage",
                        xmDrawnButtonWidgetClass, iconForm,
                        RES_CONVERT( XmNtranslations, transTable2 ),
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNshadowType, XmSHADOW_ETCHED_OUT,
                        XmNtraversalOn, False,
                        NULL);

        tmpXmStr = XmStringCreateLocalized ("32 x 32");
        iconSize = XtVaCreateManagedWidget( "iconSize",
                        xmLabelGadgetClass, iconForm,
                        XmNtopWidget, iconImage,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        monoImage = XtVaCreateManagedWidget( "monoImage",
                        xmDrawnButtonWidgetClass, iconForm,
                        RES_CONVERT( XmNtranslations, transTable2 ),
                        XmNshadowType, XmSHADOW_ETCHED_OUT,
                        XmNtopWidget, iconSize,
                        XmNtopOffset, 20,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtraversalOn, False,
                        NULL);

        tmpXmStr = GETXMSTR(12,2, "2 Color");
        monoLabel = XtVaCreateManagedWidget( "monoLabel",
                        xmLabelGadgetClass, iconForm,
                        XmNtopWidget, monoImage,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNlabelString, (tmpXmStr ),
                        NULL);
        XmStringFree(tmpXmStr);


        cmdButtonForm = XtVaCreateManagedWidget( "cmdButtonForm",
                        xmFormWidgetClass, mainForm,
                        XmNtopWidget, instructionText,
                        XmNtopOffset, 5,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 5,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL);

        cmdButtonMgr = XtVaCreateManagedWidget( "cmdButtonMgr",
                        xmRowColumnWidgetClass, cmdButtonForm,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNresizable, FALSE,
                        XmNnumColumns, 2,
                        XmNborderWidth, 1,
                        XmNmarginHeight, 0,
                        XmNmarginWidth, 0,
                        XmNspacing, 1,
                        XmNradioAlwaysOne, TRUE,
                        XmNradioBehavior, TRUE,
                        NULL);

        pointButton = XtVaCreateManagedWidget( "pointButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        lineButton = XtVaCreateManagedWidget( "lineButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        rectangleButton = XtVaCreateManagedWidget( "rectangleButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        circleButton = XtVaCreateManagedWidget( "circleButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        eraseButton = XtVaCreateManagedWidget( "eraseButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        floodButton = XtVaCreateManagedWidget( "floodButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        polylineButton = XtVaCreateManagedWidget( "polylineButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        polygonButton = XtVaCreateManagedWidget( "polygonButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        ellipseButton = XtVaCreateManagedWidget( "ellipseButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        selectButton = XtVaCreateManagedWidget( "selectButton",
                        xmToggleButtonWidgetClass, cmdButtonMgr,
                        XmNshadowThickness, 2,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNrecomputeSize, FALSE,
                        XmNwidth, PIXMAP_WIDTH,
                        XmNheight, PIXMAP_HEIGHT,
                        NULL);

        tmpXmStr = GETXMSTR(12,6, "Fill Solids");
        fillToggle = XtVaCreateManagedWidget( "fillToggle",
                        xmToggleButtonGadgetClass, cmdButtonForm,
                        XmNmarginHeight, 0,
                        XmNmarginTop, 0,
                        XmNmarginBottom, 0,
                        XmNtopWidget, cmdButtonMgr,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNshadowThickness, 0,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        cmdForm = XtVaCreateManagedWidget( "cmdForm",
                        xmFormWidgetClass, mainForm,
                        XmNrightOffset, 5,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 5,
                        XmNbottomAttachment, XmATTACH_FORM,
                        XmNleftOffset, 5,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNshadowThickness, 0,
                        NULL);

        tmpXmStr = GETXMSTR(12,8, "Static Colors");
        staticColorsBox = XtVaCreateManagedWidget( "staticColorsBox",
                        dtTitleBoxWidgetClass, cmdForm,
                        XmNmarginWidth, 5,
                        XmNmarginHeight, 5,
                        XmNtitleString, tmpXmStr,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNshadowType, XmSHADOW_ETCHED_OUT,
                        NULL);
        XmStringFree(tmpXmStr);

        stdColorPens = XtVaCreateManagedWidget( "stdColorPens",
                        xmFormWidgetClass, staticColorsBox,
                        NULL);

        color_pb1 = XtVaCreateManagedWidget( "color_pb1",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL);

        color_pb2 = XtVaCreateManagedWidget( "color_pb2",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);

        color_pb3 = XtVaCreateManagedWidget( "color_pb3",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, color_pb1,
                        NULL);

        color_pb4 = XtVaCreateManagedWidget( "color_pb4",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, color_pb2,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);

        color_pb5 = XtVaCreateManagedWidget( "color_pb5",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, color_pb3,
                        NULL);

        color_pb6 = XtVaCreateManagedWidget( "color_pb6",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, color_pb4,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);

        color_pb7 = XtVaCreateManagedWidget( "color_pb7",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, color_pb5,
                        NULL);

        color_pb8 = XtVaCreateManagedWidget( "color_pb8",
                        xmToggleButtonWidgetClass, stdColorPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, color_pb6,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);


        tmpXmStr = GETXMSTR(12,12, "Static Grays");
        staticGreysBox = XtVaCreateManagedWidget( "staticGreysBox",
                        dtTitleBoxWidgetClass, cmdForm,
                        XmNmarginWidth, 5,
                        XmNmarginHeight, 5,
                        XmNtitleString, tmpXmStr,
                        XmNleftWidget, staticColorsBox,
                        XmNleftOffset, 5,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNshadowType, XmSHADOW_ETCHED_OUT,
                        NULL);
        XmStringFree(tmpXmStr);

        stdGreyPens = XtVaCreateManagedWidget( "stdGreyPens",
                        xmFormWidgetClass, staticGreysBox,
                        NULL);

        grey_pb1 = XtVaCreateManagedWidget( "grey_pb1",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL);

        grey_pb2 = XtVaCreateManagedWidget( "grey_pb2",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);

        grey_pb3 = XtVaCreateManagedWidget( "grey_pb3",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, grey_pb1,
                        NULL);

        grey_pb4 = XtVaCreateManagedWidget( "grey_pb4",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, grey_pb2,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);

        grey_pb5 = XtVaCreateManagedWidget( "grey_pb5",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, grey_pb3,
                        NULL);

        grey_pb6 = XtVaCreateManagedWidget( "grey_pb6",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, grey_pb4,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);

        grey_pb7 = XtVaCreateManagedWidget( "grey_pb7",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, grey_pb5,
                        NULL);

        grey_pb8 = XtVaCreateManagedWidget( "grey_pb8",
                        xmToggleButtonWidgetClass, stdGreyPens,
                        XmNshadowThickness, 2,
                        XmNrecomputeSize, FALSE,
                        XmNlabelType, XmPIXMAP,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        XmNwidth, COLOR_BUTTON_SIZE,
                        XmNheight, COLOR_BUTTON_SIZE,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, grey_pb6,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNtopOffset, COLOR_BUTTON_SIZE,
                        NULL);


        tmpXmStr = GETXMSTR(12,10, "Dynamic Colors");
        symbolicColorsBox = XtVaCreateManagedWidget( "symbolicColorsBox",
                        dtTitleBoxWidgetClass, cmdForm,
                        XmNmarginWidth, 5,
                        XmNmarginHeight, 5,
                        XmNtitleString, tmpXmStr,
                        XmNleftWidget, staticGreysBox,
                        XmNleftOffset, 5,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNshadowType, XmSHADOW_ETCHED_OUT,
                        NULL);
        XmStringFree(tmpXmStr);

        symbolicColorPens = XtVaCreateManagedWidget( "symbolicColorPens",
                        xmRowColumnWidgetClass, symbolicColorsBox,
                        XmNpacking, XmPACK_COLUMN,
                        XmNradioAlwaysOne, FALSE,
                        XmNentryClass, xmToggleButtonWidgetClass,
                        XmNspacing, 0,
                        XmNmarginWidth, 0,
                        XmNmarginHeight, 0,
                        XmNradioBehavior, FALSE,
                        XmNorientation, XmHORIZONTAL,
                        XmNnumColumns, 3,
                        NULL);

        tmpXmStr = GETXMSTR(12,14, "Foreground");
        fgColorToggle = XtVaCreateManagedWidget( "fgColorToggle",
                        xmToggleButtonWidgetClass, symbolicColorPens,
                        XmNshadowThickness, 2,
                        XmNlabelString, tmpXmStr,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        NULL);
        XmStringFree(tmpXmStr);

        tmpXmStr = GETXMSTR(12,16, "Background");
        bgColorToggle = XtVaCreateManagedWidget( "bgColorToggle",
                        xmToggleButtonWidgetClass, symbolicColorPens,
                        XmNshadowThickness, 2,
                        XmNlabelString, tmpXmStr,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        NULL);
        XmStringFree(tmpXmStr);

        tmpXmStr = GETXMSTR(12,18, "TopShadow");
        tsColorToggle = XtVaCreateManagedWidget( "tsColorToggle",
                        xmToggleButtonWidgetClass, symbolicColorPens,
                        XmNshadowThickness, 2,
                        XmNlabelString, tmpXmStr,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        NULL);
        XmStringFree(tmpXmStr);

        tmpXmStr = GETXMSTR(12,20, "BottomShadow");
        bsColorToggle = XtVaCreateManagedWidget( "bsColorToggle",
                        xmToggleButtonWidgetClass, symbolicColorPens,
                        XmNshadowThickness, 2,
                        XmNlabelString, tmpXmStr,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        NULL);
        XmStringFree(tmpXmStr);

        tmpXmStr = GETXMSTR(12,22, "Select");
        selectColorToggle = XtVaCreateManagedWidget( "selectColorToggle",
                        xmToggleButtonWidgetClass, symbolicColorPens,
                        XmNshadowThickness, 2,
                        XmNlabelString, tmpXmStr,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        NULL);
        XmStringFree(tmpXmStr);

        tmpXmStr = GETXMSTR(12,24, "Transparent");
        transparentColorToggle=XtVaCreateManagedWidget("transparentColorToggle",
                        xmToggleButtonWidgetClass, symbolicColorPens,
                        XmNshadowThickness, 2,
                        XmNlabelString, tmpXmStr,
                        XmNindicatorOn, FALSE,
                        XmNfillOnSelect, FALSE,
                        NULL);
        XmStringFree(tmpXmStr);


        viewport = XtVaCreateManagedWidget( "viewport",
                        xmScrolledWindowWidgetClass, mainForm,
                        XmNheight, VIEWPORT_INITIAL_HEIGHT,
                        XmNtopWidget, instructionText,
                        XmNtopOffset, 5,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightWidget, iconForm,
                        XmNrightOffset, 5,
                        XmNrightAttachment, XmATTACH_WIDGET,
                        XmNbottomWidget, cmdForm,
                        XmNbottomOffset, 20,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftWidget, cmdButtonForm,
                        XmNleftOffset, 5,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNscrollingPolicy, XmAUTOMATIC,
                        NULL);


        tabletBorder = XtVaCreateManagedWidget( "tabletBorder",
                        xmDrawingAreaWidgetClass, viewport,
                        XmNmarginWidth, 10,
                        XmNmarginHeight, 10,
                        XmNtraversalOn, False,
                        NULL);


        tabletFrame = XtVaCreateManagedWidget( "tabletFrame",
                        xmFrameWidgetClass, tabletBorder,
                        XmNshadowType, XmSHADOW_ETCHED_OUT,
                        XmNmarginWidth, 0,
                        XmNmarginHeight, 0,
                        NULL);


        tablet = XtVaCreateManagedWidget( "tablet",
                        xmDrawnButtonWidgetClass, tabletFrame,
                        RES_CONVERT( XmNtranslations, transTable1 ),
                        XmNshadowThickness, 0,
                        XmNmarginWidth, 0,
                        XmNmarginHeight, 0,
                        XmNhighlightThickness, 0,
                        NULL);


        menu1 = XtVaCreateManagedWidget( "menu1",
                        xmRowColumnWidgetClass, mainWindow,
                        XmNmenuAccelerator, "<KeyUp>F10",
                        XmNrowColumnType, XmMENU_BAR,
                        NULL);


        fileMenu_shell = XtVaCreatePopupShell ("fileMenu_shell",
                        xmMenuShellWidgetClass, menu1,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        fileMenu = XtVaCreateWidget( "fileMenu",
                        xmRowColumnWidgetClass, fileMenu_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);


        mnemonicStr = GETSTR(12,26, "N");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,28, "New");
        fileMenu_new_pb = XtVaCreateManagedWidget( "new",
                        xmPushButtonGadgetClass, fileMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,30, "O");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,32, "Open...");
        fileMenu_open_pb = XtVaCreateManagedWidget( "open",
                        xmPushButtonGadgetClass, fileMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        XtVaCreateManagedWidget( "fileMenu_separator1",
                        xmSeparatorGadgetClass, fileMenu, NULL);

        mnemonicStr = GETSTR(12,34, "S");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,36, "Save");
        fileMenu_save_pb = XtVaCreateManagedWidget( "save",
                        xmPushButtonGadgetClass, fileMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,38, "A");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,40, "Save As...");
        fileMenu_saveAs_pb = XtVaCreateManagedWidget( "saveas",
                        xmPushButtonGadgetClass, fileMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        XtVaCreateManagedWidget( "fileMenu_separator1",
                        xmSeparatorGadgetClass, fileMenu, NULL);

        mnemonicStr = GETSTR(12,42, "x");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,44, "Exit");
        fileMenu_quit_pb = XtVaCreateManagedWidget( "exit",
                        xmPushButtonGadgetClass, fileMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        tmpXmStr = GETXMSTR(12,48, "File");
        mnemonicStr = GETSTR(12,46, "F");
        mnemonic = mnemonicStr[0];
        fileMenu_top_pb = XtVaCreateManagedWidget( "fileMenu_top_pb",
                        xmCascadeButtonWidgetClass, menu1,
                        XmNsubMenuId, fileMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        editMenu_shell = XtVaCreatePopupShell ("editMenu_shell",
                        xmMenuShellWidgetClass, menu1,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        editMenu = XtVaCreateWidget( "editMenu",
                        xmRowColumnWidgetClass, editMenu_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);


        mnemonicStr = GETSTR(12,50, "U");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,52, "Undo");
        editMenu_undo_pb = XtVaCreateManagedWidget( "undo",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive( editMenu_undo_pb, False);


        XtVaCreateManagedWidget( "editMenu_separator",
                        xmSeparatorGadgetClass, editMenu, NULL);

        mnemonicStr = GETSTR(12,54, "t");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,56, "Cut Area");
        editMenu_cut_pb = XtVaCreateManagedWidget( "cut",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive( editMenu_cut_pb, False);


        mnemonicStr = GETSTR(12,58, "C");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,60, "Copy Area");
        editMenu_copy_pb = XtVaCreateManagedWidget( "copy",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive( editMenu_copy_pb, False);


        mnemonicStr = GETSTR(12,62, "P");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,64, "Paste Area");
        editMenu_paste_pb = XtVaCreateManagedWidget( "paste",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive( editMenu_paste_pb, False);


        XtVaCreateManagedWidget( "editMenu_separator",
                        xmSeparatorGadgetClass, editMenu, NULL);


        rotateMenu_shell = XtVaCreatePopupShell ("rotateMenu_shell",
                        xmMenuShellWidgetClass, editMenu,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        rotateMenu = XtVaCreateWidget( "rotateMenu",
                        xmRowColumnWidgetClass, rotateMenu_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);

        mnemonicStr = GETSTR(12,68, "L");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,70, "Left");
        rotateMenu_pb1 = XtVaCreateManagedWidget( "rotateMenu_pb1",
                        xmPushButtonGadgetClass, rotateMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,72, "R");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,74, "Right");
        rotateMenu_pb2 = XtVaCreateManagedWidget( "rotateMenu_pb2",
                        xmPushButtonGadgetClass, rotateMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,76, "R");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,78, "Rotate Area");
        editMenu_rotate_pb = XtVaCreateManagedWidget( "editMenu_rotate_pb",
                        xmCascadeButtonGadgetClass, editMenu,
                        XmNsubMenuId, rotateMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive(editMenu_rotate_pb, False);


        flipMenu_shell = XtVaCreatePopupShell ("flipMenu_shell",
                        xmMenuShellWidgetClass, editMenu,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        flipMenu = XtVaCreateWidget( "flipMenu",
                        xmRowColumnWidgetClass, flipMenu_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);

        mnemonicStr = GETSTR(12,82, "V");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,84, "Vertical");
        flipMenu_pb1 = XtVaCreateManagedWidget( "flipMenu_pb1",
                        xmPushButtonGadgetClass, flipMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,86, "H");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,88, "Horizontal");
        flipMenu_pb2 = XtVaCreateManagedWidget( "flipMenu_pb2",
                        xmPushButtonGadgetClass, flipMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,90, "F");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,92, "Flip Area");
        editMenu_flip_pb = XtVaCreateManagedWidget( "editMenu_flip_pb",
                        xmCascadeButtonGadgetClass, editMenu,
                        XmNsubMenuId, flipMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive(editMenu_flip_pb, False);


        mnemonicStr = GETSTR(12,94, "S");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,96, "Scale Area");
        editMenu_scale_pb = XtVaCreateManagedWidget( "editMenu_scale_pb",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);
        XtSetSensitive(editMenu_scale_pb, False);


        mnemonicStr = GETSTR(12,98, "z");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,100, "Resize Icon...");
        editMenu_resize_pb = XtVaCreateManagedWidget( "editMenu_resize_pb",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        XtVaCreateManagedWidget( "editMenu_separator",
                        xmSeparatorGadgetClass, editMenu, NULL);


        mnemonicStr = GETSTR(12,102, "A");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,104, "Add Hot Spot");
        editMenu_addHS_pb = XtVaCreateManagedWidget( "editMenu_addHS_pb",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,106, "D");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,108, "Delete Hot Spot");
        editMenu_deleteHS_pb = XtVaCreateManagedWidget( "editMenu_deleteHS_pb",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        XtNsensitive, False,
                        NULL);
        XmStringFree(tmpXmStr);


        XtVaCreateManagedWidget( "editMenu_separator",
                        xmSeparatorGadgetClass, editMenu, NULL);


        mnemonicStr = GETSTR(12,110, "G");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,112, "Grab Screen Image");
        editMenu_grabImage_pb = XtVaCreateManagedWidget("editMenu_grabImage_pb",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
         XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,114, "e");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,116, "Clear Icon");
        editMenu_clear_pb = XtVaCreateManagedWidget( "editMenu_clear_pb",
                        xmPushButtonGadgetClass, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,118, "E");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,120, "Edit");
        editMenu_top_pb = XtVaCreateManagedWidget( "editMenu_top_pb",
                        xmCascadeButtonWidgetClass, menu1,
                        XmNsubMenuId, editMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        optionsMenu_shell = XtVaCreatePopupShell ("optionsMenu_shell",
                        xmMenuShellWidgetClass, menu1,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        optionsMenu = XtVaCreateWidget( "optionsMenu",
                        xmRowColumnWidgetClass, optionsMenu_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);


        mnemonicStr = GETSTR(12,122, "V");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,124, "Visible Grid");
        optionsMenu_grid = XtVaCreateManagedWidget( "optionsMenu_grid",
                        xmToggleButtonGadgetClass, optionsMenu,
                        XmNmnemonic, mnemonic,
                        XmNvisibleWhenOff, TRUE,
                        XmNindicatorType, XmN_OF_MANY,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        formatMenu_shell = XtVaCreatePopupShell ("formatMenu_shell",
                        xmMenuShellWidgetClass, optionsMenu,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        formatMenu = XtVaCreateWidget( "formatMenu",
                        xmRowColumnWidgetClass, formatMenu_shell,
                        XmNradioBehavior, TRUE,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);


        mnemonicStr = GETSTR(12,128, "B");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,130, "XBM");
        formatMenu_xbm_tb = XtVaCreateManagedWidget( "formatMenu_xbm_tb",
                        xmToggleButtonGadgetClass, formatMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,134, "P");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,136, "XPM");
        formatMenu_xpm_tb = XtVaCreateManagedWidget("formatMenu_xpm_tb",
                        xmToggleButtonGadgetClass, formatMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,138, "O");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,140, "Output Format");
        optionsMenu_format = XtVaCreateManagedWidget( "optionsMenu_format",
                        xmCascadeButtonGadgetClass, optionsMenu,
                        XmNsubMenuId, formatMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        magnificationMenu_shell =XtVaCreatePopupShell("magnificationMenu_shell",
                        xmMenuShellWidgetClass, optionsMenu,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        magnificationMenu = XtVaCreateWidget( "magnificationMenu",
                        xmRowColumnWidgetClass, magnificationMenu_shell,
                        XmNradioBehavior, TRUE,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);


        tmpXmStr = GETXMSTR(12,144, "2x");
        magMenu_2x_tb = XtVaCreateManagedWidget( "magMenu_2x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,146, "3x");
        magMenu_3x_tb = XtVaCreateManagedWidget( "magMenu_3x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,148, "4x");
        magMenu_4x_tb = XtVaCreateManagedWidget( "magMenu_4x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,150, "5x");
        magMenu_5x_tb = XtVaCreateManagedWidget( "magMenu_5x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,152, "6x");
        magMenu_6x_tb = XtVaCreateManagedWidget( "magMenu_6x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,154, "8x");
        magMenu_8x_tb = XtVaCreateManagedWidget( "magMenu_8x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,156, "10x");
        magMenu_10x_tb = XtVaCreateManagedWidget( "magMenu_10x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        tmpXmStr = GETXMSTR(12,158, "12x");
        magMenu_12x_tb = XtVaCreateManagedWidget( "magMenu_12x_tb",
                        xmToggleButtonGadgetClass, magnificationMenu,
                        XmNvisibleWhenOff, FALSE,
                        XmNlabelString,tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,160, "M");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,162, "Magnification");
        optionsMenu_magnify = XtVaCreateManagedWidget( "optionsMenu_magnify",
                        xmCascadeButtonGadgetClass, optionsMenu,
                        XmNmnemonic, mnemonic,
                        XmNsubMenuId, magnificationMenu,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        mnemonicStr = GETSTR(12,164, "O");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,166, "Options");
        optionsMenu_top_pb = XtVaCreateManagedWidget( "optionsMenu_top_pb",
                        xmCascadeButtonWidgetClass, menu1,
                        XmNmnemonic, mnemonic,
                        XmNsubMenuId, optionsMenu,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        helpMenu_shell = XtVaCreatePopupShell ("helpMenu_shell",
                        xmMenuShellWidgetClass, menu1,
                        XmNwidth, 10,
                        XmNheight, 10,
                        NULL);

        helpMenu = XtVaCreateWidget( "helpMenu",
                        xmRowColumnWidgetClass, helpMenu_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL);


        mnemonicStr = GETSTR(12,168, "v");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,170, "Overview...");
        helpMenu_intro_pb = XtVaCreateManagedWidget( "helpMenu_intro_pb",
                        xmPushButtonGadgetClass, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        XtVaCreateManagedWidget( "helpMenu_separator1",
                        xmSeparatorGadgetClass, helpMenu, NULL);

        mnemonicStr = GETSTR(12,172, "T");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,174, "Tasks...");
        helpMenu_tasks_pb = XtVaCreateManagedWidget( "helpMenu_tasks_pb",
                        xmPushButtonGadgetClass, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,176, "R");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,178, "Reference...");
        helpMenu_ref_pb = XtVaCreateManagedWidget( "helpMenu_ref_pb",
                        xmPushButtonGadgetClass, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,180, "O");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,182, "On Item");
        helpMenu_item_pb = XtVaCreateManagedWidget( "helpMenu_item_pb",
                        xmPushButtonGadgetClass, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        XtVaCreateManagedWidget( "helpMenu_separator2",
                        xmSeparatorGadgetClass, helpMenu, NULL);

        mnemonicStr = GETSTR(12,188, "U");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,190, "Using Help...");
        helpMenu_using_pb = XtVaCreateManagedWidget("helpMenu_using_pb",
                        xmPushButtonGadgetClass, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        XtVaCreateManagedWidget( "helpMenu_separator3",
                        xmSeparatorGadgetClass, helpMenu, NULL);

        mnemonicStr = GETSTR(12,192, "A");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,194, "About the Icon Editor...");
        helpMenu_version_pb = XtVaCreateManagedWidget( "helpMenu_version_pb",
                        xmPushButtonGadgetClass, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);

        mnemonicStr = GETSTR(12,196, "H");
        mnemonic = mnemonicStr[0];
        tmpXmStr = GETXMSTR(12,198, "Help");
        helpMenu_top_pb = XtVaCreateManagedWidget( "helpMenu_top_pb",
                        xmCascadeButtonWidgetClass, menu1,
                        XmNsubMenuId, helpMenu,
                        XmNmnemonic, mnemonic,
                        XmNlabelString, tmpXmStr,
                        NULL);
        XmStringFree(tmpXmStr);


        /* set up meaningful tab groups */
        XmAddTabGroup(cmdButtonMgr);            /* drawing tools   */
        XmAddTabGroup(fillToggle);              /* fill toggle     */
        XmAddTabGroup(viewport);                /* drawing tablet  */
        XmAddTabGroup(stdColorPens);            /* static colors   */
        XmAddTabGroup(stdGreyPens);             /* static greys    */
        XmAddTabGroup(symbolicColorPens);       /* symbolic colors */

        /* set up initial focus  */
        XmAddTabGroup(mainWindow);              /* main window     */
        XtVaSetValues(mainWindow, XmNinitialFocus, viewport, NULL);


        XtAddCallback(pointButton, XmNvalueChangedCallback,
                        armCB_pointButton, NULL);
        XtAddCallback(lineButton, XmNvalueChangedCallback,
                        armCB_lineButton, NULL);
        XtAddCallback(rectangleButton, XmNvalueChangedCallback,
                        armCB_rectangleButton, NULL);
        XtAddCallback(circleButton, XmNvalueChangedCallback,
                        armCB_circleButton, NULL);
        XtAddCallback(eraseButton, XmNvalueChangedCallback,
                        armCB_eraseButton, NULL);
        XtAddCallback(floodButton, XmNvalueChangedCallback,
                        armCB_floodButton, NULL);
        XtAddCallback(polylineButton, XmNvalueChangedCallback,
                        armCB_polylineButton, NULL);
        XtAddCallback(polygonButton, XmNvalueChangedCallback,
                        armCB_polygonButton, NULL);
        XtAddCallback(ellipseButton, XmNvalueChangedCallback,
                        armCB_ellipseButton, NULL);
        XtAddCallback(selectButton, XmNvalueChangedCallback,
                        armCB_selectButton, NULL);

        XtAddCallback(fillToggle, XmNvalueChangedCallback,
                        valueChangedCB_fillToggle, NULL);

        XtAddCallback(color_pb1, XmNvalueChangedCallback,
                        valueChangedCB_color_pb1, NULL);
        XtAddCallback(color_pb2, XmNvalueChangedCallback,
                        valueChangedCB_color_pb2, NULL);
        XtAddCallback(color_pb3, XmNvalueChangedCallback,
                        valueChangedCB_color_pb3, NULL);
        XtAddCallback(color_pb4, XmNvalueChangedCallback,
                        valueChangedCB_color_pb4, NULL);
        XtAddCallback(color_pb5, XmNvalueChangedCallback,
                        valueChangedCB_color_pb5, NULL);
        XtAddCallback(color_pb6, XmNvalueChangedCallback,
                        valueChangedCB_color_pb6, NULL);
        XtAddCallback(color_pb7, XmNvalueChangedCallback,
                        valueChangedCB_color_pb7, NULL);
        XtAddCallback(color_pb8, XmNvalueChangedCallback,
                        valueChangedCB_color_pb8, NULL);

        XtAddCallback(grey_pb1, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb1, NULL);
        XtAddCallback(grey_pb2, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb2, NULL);
        XtAddCallback(grey_pb3, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb3, NULL);
        XtAddCallback(grey_pb4, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb4, NULL);
        XtAddCallback(grey_pb5, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb5, NULL);
        XtAddCallback(grey_pb6, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb6, NULL);
        XtAddCallback(grey_pb7, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb7, NULL);
        XtAddCallback(grey_pb8, XmNvalueChangedCallback,
                        valueChangedCB_grey_pb8, NULL);

        XtAddCallback(fgColorToggle, XmNvalueChangedCallback,
                        valueChangedCB_fgColorToggle, NULL);
        XtAddCallback(bgColorToggle, XmNvalueChangedCallback,
                        valueChangedCB_bgColorToggle, NULL);
        XtAddCallback(tsColorToggle, XmNvalueChangedCallback,
                        valueChangedCB_tsColorToggle, NULL);
        XtAddCallback(bsColorToggle, XmNvalueChangedCallback,
                        valueChangedCB_bsColorToggle, NULL);
        XtAddCallback(selectColorToggle, XmNvalueChangedCallback,
                        valueChangedCB_selectColorToggle, NULL);
        XtAddCallback(transparentColorToggle, XmNvalueChangedCallback,
                        valueChangedCB_transparentColorToggle, NULL);


        XtVaSetValues(menu1, XmNmenuHelpWidget, helpMenu_top_pb, NULL);


        XtAddCallback(fileMenu_new_pb, XmNactivateCallback,
                        activateCB_fileMenu_new_pb, NULL);
        XtAddCallback(fileMenu_open_pb, XmNactivateCallback,
                        activateCB_fileMenu_open_pb, NULL);
        XtAddCallback(fileMenu_save_pb, XmNactivateCallback,
                        activateCB_fileMenu_save_pb, NULL);
        XtAddCallback(fileMenu_saveAs_pb, XmNactivateCallback,
                        activateCB_fileMenu_saveAs_pb, NULL);
        XtAddCallback(fileMenu_quit_pb, XmNactivateCallback,
                        activateCB_fileMenu_quit_pb, NULL);


        XtAddCallback(editMenu_undo_pb, XmNactivateCallback,
                        activateCB_editMenu_undo_pb, NULL);

        XtAddCallback(editMenu_cut_pb, XmNactivateCallback,
                        activateCB_editMenu_cut_pb, NULL);

        XtAddCallback(editMenu_copy_pb, XmNactivateCallback,
                        activateCB_editMenu_copy_pb, NULL);

        XtAddCallback(editMenu_paste_pb, XmNactivateCallback,
                        activateCB_editMenu_paste_pb, NULL);

        XtAddCallback(rotateMenu_pb1, XmNactivateCallback,
                        activateCB_rotateMenu_pb1, NULL);
        XtAddCallback(rotateMenu_pb2, XmNactivateCallback,
                        activateCB_rotateMenu_pb2, NULL);

        XtAddCallback(flipMenu_pb1, XmNactivateCallback,
                        activateCB_flipMenu_pb1, NULL);
        XtAddCallback(flipMenu_pb2, XmNactivateCallback,
                        activateCB_flipMenu_pb2, NULL);

        XtAddCallback(editMenu_scale_pb, XmNactivateCallback,
                        activateCB_editMenu_scale_pb, NULL);

        XtAddCallback(editMenu_resize_pb, XmNactivateCallback,
                        activateCB_editMenu_resize_pb, NULL);

        XtAddCallback(editMenu_addHS_pb, XmNactivateCallback,
                        activateCB_editMenu_addHS_pb, NULL);

        XtAddCallback(editMenu_deleteHS_pb, XmNactivateCallback,
                        activateCB_editMenu_deleteHS_pb, NULL);

        XtAddCallback(editMenu_grabImage_pb, XmNactivateCallback,
                        activateCB_editMenu_grabImage_pb, NULL);

        XtAddCallback(editMenu_clear_pb, XmNactivateCallback,
                        activateCB_editMenu_clear_pb, NULL);


        XtAddCallback(optionsMenu_grid, XmNvalueChangedCallback,
                        valueChangedCB_optionsMenu_grid, NULL);

        XtAddCallback(formatMenu_xbm_tb, XmNvalueChangedCallback,
                        valueChangedCB_formatMenu_xbm_tb, NULL);
        XtAddCallback(formatMenu_xpm_tb, XmNvalueChangedCallback,
                        valueChangedCB_formatMenu_xpm_tb, NULL);

        XtAddCallback(magMenu_2x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_2x_tb, NULL);
        XtAddCallback(magMenu_3x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_3x_tb, NULL);
        XtAddCallback(magMenu_4x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_4x_tb, NULL);
        XtAddCallback(magMenu_5x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_5x_tb, NULL);
        XtAddCallback(magMenu_6x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_6x_tb, NULL);
        XtAddCallback(magMenu_8x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_8x_tb, NULL);
        XtAddCallback(magMenu_10x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_10x_tb, NULL);
        XtAddCallback(magMenu_12x_tb, XmNvalueChangedCallback,
                        valueChangedCB_magMenu_12x_tb, NULL);


        XmMainWindowSetAreas( mainWindow, menu1, (Widget) NULL,
                        (Widget) NULL, (Widget) NULL, mainForm );

        return ( dtIconShell );
}


/*******************************************************************************
    NewGraphicsOp - sets the text in the instruction line
*******************************************************************************/
static void NewGraphicsOp()
{
    static char *fillOnStr = NULL;
    int fillOp = False;
    char *graphicsOpStr = NULL;
    char *tmpStr = NULL;
    XmString tmpXmStr = NULL;
    Arg args[10];
    int n;


    XtSetSensitive( editMenu_cut_pb, False);
    XtSetSensitive( editMenu_copy_pb, False);
    XtSetSensitive(editMenu_rotate_pb, False);
    XtSetSensitive(editMenu_flip_pb,  False);
    XtSetSensitive(editMenu_scale_pb,  False);
    XSync(dpy, 0);
    /* set the instruction text line to current drawing mode */
    switch (GraphicsOp) {

        case POINT:
               tmpStr = GETSTR(12, 204, "Pencil");
             break;
        case FLOOD:
               tmpStr = GETSTR(12, 206, "Flood");
             break;
        case LINE:
               tmpStr = GETSTR(12, 208, "Line");
             break;
        case POLYLINE:
               tmpStr = GETSTR(12, 210, "Polyline");
             break;
        case RECTANGLE:
               tmpStr = GETSTR(12, 212, "Rectangle");
               fillOp = True;
             break;
        case POLYGON:
               tmpStr = GETSTR(12, 214, "Polygon");
               fillOp = True;
             break;
        case CIRCLE:
               tmpStr = GETSTR(12, 216, "Circle");
               fillOp = True;
             break;
        case ELLIPSE:
               tmpStr = GETSTR(12, 218, "Ellipse");
               fillOp = True;
             break;
        case ERASER:
               tmpStr = GETSTR(12, 220, "Erase");
             break;
        case SELECT:
               tmpStr = GETSTR(12, 222, "Select");
             break;
        default :
               tmpStr = GETSTR(12, 224, "Invalid");
             break;

    } /* switch GraphicsOp */

    graphicsOpStr = XtNewString (tmpStr);

    /* create full help string */
    if ( fillOp && FillSolids)
    {
        /* if fill mode is on append fillOnStr to end of string */
        if (!fillOnStr)
        {
            tmpStr = GETSTR(12, 202, "(Filled)");
            fillOnStr = XtNewString (tmpStr);
        }

        tmpStr = (char *)XtMalloc(strlen(graphicsOpStr) + strlen(fillOnStr) +2);
        tmpStr = strcpy (tmpStr, graphicsOpStr);
        tmpStr = strcat (tmpStr, " ");
        tmpStr = strcat (tmpStr, fillOnStr);

        tmpXmStr = XMSTR(tmpStr);
        XtFree (tmpStr);
        XtFree (graphicsOpStr);
    }
    else
    {
        tmpXmStr = XMSTR(graphicsOpStr);
        XtFree (graphicsOpStr);
    }

    n = 0;
    XtSetArg (args[n], XmNlabelString, tmpXmStr);                          n++;
    XtSetValues (instructionText, args, n);

    XmStringFree(tmpXmStr);
}

/*******************************************************************************
        The following is the 'Interface function' which is the
        external entry point for creating this interface.
        This function should be called from your application or from
        a callback function.
*******************************************************************************/

Widget
create_dtIconShell( void )
{
        Widget                w;
        static int        init = 0;

        if ( !init )
        {
                static XtActionsRec        actions[] = {
                                { "tablet_event", action_tablet_event }
                                };

                XtAppAddActions( AppContext,
                                actions,
                                XtNumber(actions) );

                init = 1;
        }

        w = build_dtIconShell();

        Init_Editor(w);

        return (w);
}

/*******************************************************************************
        END OF FILE
*******************************************************************************/

