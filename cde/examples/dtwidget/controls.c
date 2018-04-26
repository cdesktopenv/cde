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
/* $XConsortium: controls.c /main/4 1995/10/27 10:41:00 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company	
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */


/*
 * controls.c
 *
 * Example code for libDtWidget controls
 * (ComboBox, SpinBox, MenuButton)
 */

#include <Xm/XmAll.h>

#include <Xm/SSpinB.h>
#include <Dt/ComboBox.h>
#include <Dt/MenuButton.h>

#define ApplicationClass "Controls"

static void CreateSpinBoxes(Widget);
static void CreateComboBoxes(Widget);
static void CreateMenuButtons(Widget);

main(int argc, char **argv)
{
    XtAppContext appContext;
    Arg args[20];
    int n;

    Widget toplevel, mainWindow, spinContainer, comboContainer, menuContainer;
    
    toplevel = XtAppInitialize(&appContext, ApplicationClass, NULL, 0,
						&argc, argv, NULL, NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNspacing, 40); n++;
    mainWindow = XmCreateWorkArea(toplevel, "mainWindow", args, n);
    XtManageChild(mainWindow);

    n = 0;
    XtSetArg(args[n], XmNspacing, 20); n++;
    spinContainer = XmCreateWorkArea(mainWindow, "spinContainer", args, n);
    XtManageChild(spinContainer);
    CreateSpinBoxes(spinContainer);

    n = 0;
    XtSetArg(args[n], XmNspacing, 20); n++;
    comboContainer = XmCreateWorkArea(mainWindow, "comboContainer", args, n);
    XtManageChild(comboContainer);
    CreateComboBoxes(comboContainer);

    n = 0;
    XtSetArg(args[n], XmNspacing, 20); n++;
    menuContainer = XmCreateWorkArea(mainWindow, "menuContainer", args, n);
    XtManageChild(menuContainer);
    CreateMenuButtons(menuContainer);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(appContext);
}


/*
 * Example code for XmSimpleSpinBox
 */

static char *spinValueStrings[] = {
	"alpha", "beta", "gamma", "delta",
	"epsilon", "zeta", "eta", "theta",
	"iota", "kappa", "lambda", "mu",
	"nu", "xi", "omicron", "pi",
	"rho", "sigma", "tau", "upsilon",
	"phi", "chi", "psi", "omega"
};

static void ModifyVerifyCb(Widget, XtPointer, XtPointer);

static void CreateSpinBoxes(Widget parent)
{
    Widget titleLabel, spinBox;
    XmString *valueXmstrings;
    int numValueStrings;
    XmString labelString;
    Arg args[20];
    int i, n;
    
    /* Create value compound strings */

    numValueStrings = XtNumber(spinValueStrings);
    valueXmstrings = (XmString *)XtMalloc(numValueStrings * sizeof(XmString*));
    for (i = 0; i < numValueStrings; i++) {
	valueXmstrings[i] = XmStringCreateLocalized(spinValueStrings[i]);
    }

    /* Create title label */

    labelString = XmStringCreateLocalized("SpinBox Widget");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    titleLabel = XmCreateLabel(parent, "title", args, n);
    XtManageChild(titleLabel);
    XmStringFree(labelString);


    /*
     * Create a SimpleSpinBox containing string values.
     */

    n = 0;
    XtSetArg(args[n], XmNvalues, valueXmstrings); n++;
    XtSetArg(args[n], XmNnumValues, numValueStrings); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    spinBox = XmCreateSimpleSpinBox(parent, "spinBox1", args, n);
    XtManageChild(spinBox);


    /*
     * Create a SpinBox containing numeric values to 3 decimal places.
     * Position the arrows on either side of the displayed value.
     */

    n = 0; 
    XtSetArg(args[n], XmNspinBoxChildType, XmNUMERIC); n++;
    XtSetArg(args[n], XmNminimumValue, 1000); n++;
    XtSetArg(args[n], XmNmaximumValue, 100000); n++;
    XtSetArg(args[n], XmNincrementValue,1000); n++;
    XtSetArg(args[n], XmNdecimalPoints,3); n++;
    XtSetArg(args[n], XmNposition,1000); n++;
    XtSetArg(args[n], XmNarrowLayout, XmARROWS_SPLIT); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    spinBox = XmCreateSimpleSpinBox(parent, "spinBox2", args, n);
    XtManageChild(spinBox);


    /*
     * Create a SpinBox containing numeric values to 2 decimal places.
     * Position the arrows on the left of the displayed value.
     * Disallow alternate user changes by adding a modify/verify callback.
     */

    n = 0; 
    XtSetArg(args[n], XmNspinBoxChildType, XmNUMERIC); n++;
    XtSetArg(args[n], XmNminimumValue, 1500); n++;
    XtSetArg(args[n], XmNmaximumValue, 60500); n++;
    XtSetArg(args[n], XmNincrementValue,1500); n++;
    XtSetArg(args[n], XmNdecimalPoints,2); n++;
    XtSetArg(args[n], XmNposition,7500); n++;
    XtSetArg(args[n], XmNarrowLayout, XmARROWS_FLAT_BEGINNING); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    spinBox = XmCreateSimpleSpinBox(parent, "spinBox3", args, n);
    XtManageChild(spinBox);

    XtAddCallback(spinBox, XmNmodifyVerifyCallback, ModifyVerifyCb, NULL);


    /*
     * Create a SpinBox containing string values.
     * Position the arrows on the left of the display value
     */

    n = 0; 
    XtSetArg(args[n], XmNvalues, valueXmstrings); n++;
    XtSetArg(args[n], XmNnumValues, numValueStrings); n++;
    XtSetArg(args[n], XmNarrowLayout, XmARROWS_BEGINNING); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    spinBox = XmCreateSimpleSpinBox(parent, "spinBox4", args, n);
    XtManageChild(spinBox);


    /*
     * Create a SpinBox containing numeric values to 3 decimal places.
     * Position the arrows on the right of the displayed value.
     */

    n = 0;
    XtSetArg(args[n], XmNspinBoxChildType, XmNUMERIC); n++;
    XtSetArg(args[n], XmNminimumValue, 1000); n++;
    XtSetArg(args[n], XmNmaximumValue, 100000); n++;
    XtSetArg(args[n], XmNincrementValue,1000); n++;
    XtSetArg(args[n], XmNdecimalPoints,3); n++;
    XtSetArg(args[n], XmNposition,1000); n++;
    XtSetArg(args[n], XmNarrowLayout, XmARROWS_FLAT_END); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    spinBox = XmCreateSimpleSpinBox(parent, "spinBox5", args, n);
    XtManageChild(spinBox);


    /*
     * Free value strings, SpinBox has taken a copy.
     */

    for (i = 0; i < numValueStrings; i++) {
	XmStringFree(valueXmstrings[i]);
    }
    XtFree((char*)valueXmstrings);

}


/*
 * modify/verify callback.
 *
 * Allow/disallow alternate user changes
 */

static void ModifyVerifyCb(Widget w, XtPointer cd, XtPointer cb)
{
    XmSpinBoxCallbackStruct *scb= (XmSpinBoxCallbackStruct*)cb;
    static Boolean allowChange = True;

    scb->doit = allowChange;

    if (allowChange == False) {
	printf("XmSpinBox: XmNmodifyVerifyCallback. Change disallowed.\n");
	XBell(XtDisplay(w), 0);
    }

    allowChange = (allowChange == True) ? False : True;
}



/*
 * Example code for DtComboBox
 */


static char *comboValueStrings[] = {
	"alpha", "beta", "gamma", "delta",
	"epsilon", "zeta", "eta", "theta",
	"iota", "kappa", "lambda", "mu",
	"nu", "xi", "omicron", "pi",
	"rho", "sigma", "tau", "upsilon",
	"phi", "chi", "psi", "omega"
};

static char *colorStrings[] = { "Red", "Yellow", "Green", "Brown", "Blue" };

static void CreateComboBoxes(Widget parent)
{
    Widget titleLabel, comboBox, list;
    XmString *valueXmstrings, *colorXmstrings;
    int numValueStrings, numColorStrings;
    XmString labelString, xmString;
    Arg args[20];
    int i, n;
    
    /* Create value compound strings */

    numValueStrings = XtNumber(comboValueStrings);
    valueXmstrings = (XmString *)XtMalloc(numValueStrings * sizeof(XmString*));
    for (i = 0; i < numValueStrings; i++) {
	valueXmstrings[i] = XmStringCreateLocalized(comboValueStrings[i]);
    }

    /* Create color compound strings */

    numColorStrings = XtNumber(colorStrings);
    colorXmstrings = (XmString *)XtMalloc(numColorStrings * sizeof(XmString*));
    for (i = 0; i < numColorStrings; i++) {
	colorXmstrings[i] = XmStringCreateLocalized(colorStrings[i]);
    }

    /* Create title label */

    labelString = XmStringCreateLocalized("ComboBox Widget");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    titleLabel = XmCreateLabel(parent, "title", args, n);
    XtManageChild(titleLabel);
    XmStringFree(labelString);


    /*
     * Create an editable ComboBox containing the color strings.
     * Get the widget id of the drop down list, add some greek
     * letter names to it, and make more items visible.
     */

    n = 0;
    XtSetArg(args[n], DtNcomboBoxType, DtDROP_DOWN_COMBO_BOX); n++;
    XtSetArg(args[n], DtNitems, colorXmstrings); n++;
    XtSetArg(args[n], DtNitemCount, numColorStrings); n++;
    XtSetArg(args[n], DtNvisibleItemCount, 5); n++;
    XtSetArg(args[n], DtNcolumns, 10); n++;
    comboBox = DtCreateComboBox(parent, "comboBox1", args, n);
    XtManageChild(comboBox);

    list = XtNameToWidget(comboBox, "*List");
    XmListAddItems(list, valueXmstrings, 10, 0);
    XtVaSetValues(list, XmNvisibleItemCount, 10, NULL);


    /*
     * Create an editable ComboBox with no entries.
     * Get the widget id of the drop down list, add some greek
     * letter names to it and select the third item in the list.
     */

    n = 0;
    XtSetArg(args[n], DtNcomboBoxType, DtDROP_DOWN_COMBO_BOX); n++;
    XtSetArg(args[n], DtNorientation, DtLEFT); n++;
    XtSetArg(args[n], DtNcolumns, 10); n++;
    comboBox = DtCreateComboBox(parent, "comboBox2", args, n);
    XtManageChild(comboBox);

    list = XtNameToWidget(comboBox, "*List");        
    XmListAddItems(list, valueXmstrings, 7, 0);
    XtVaSetValues(list, XmNvisibleItemCount, 7, NULL);
    XtVaSetValues(comboBox, DtNselectedPosition, 3, NULL);


    /*
     * Create a non-editable ComboBox containing some greek letter names.
     * Position the arrow on the left.
     * Select the 'gamma' item in the list.
     */

    n = 0;
    XtSetArg(args[n], DtNorientation, DtLEFT); n++;
    XtSetArg(args[n], DtNitems, valueXmstrings); n++;
    XtSetArg(args[n], DtNitemCount, numValueStrings); n++;
    XtSetArg(args[n], DtNvisibleItemCount, 8); n++;
    comboBox = DtCreateComboBox(parent, "comboBox3", args, n);
    XtManageChild(comboBox);

    xmString = XmStringCreateLocalized("gamma");
    XtVaSetValues(comboBox, DtNselectedItem, xmString, NULL);
    XmStringFree(xmString);


    /*
     * Create a non-editable ComboBox with no entries.
     * Position the arrow on the right.
     * Add the greek letter names to the list and select the fourth item. 
     */

    n = 0;
    XtSetArg(args[n], DtNorientation, DtRIGHT); n++;
    XtSetArg(args[n], DtNvisibleItemCount, 8); n++;
    comboBox = DtCreateComboBox(parent, "comboBox4", args, n);
    XtManageChild(comboBox);

    for (i = 0; i < numValueStrings; i++) {
	DtComboBoxAddItem(comboBox, valueXmstrings[i], 0, True);
    }
    XtVaSetValues(comboBox, DtNselectedPosition, 4, NULL);


    /*
     * Free value and color strings, ComboBox has taken a copy.
     */

    for (i = 0; i < numValueStrings; i++) {
	XmStringFree(valueXmstrings[i]);
    }
    XtFree((char*)valueXmstrings);

    for (i = 0; i < numColorStrings; i++) {
	XmStringFree(colorXmstrings[i]);
    }
    XtFree((char*)colorXmstrings);
}



/*
 * Example code for DtMenuButton
 */

/* MenuButton custom glyph */

#define menu_glyph_width 16
#define menu_glyph_height 16
static unsigned char menu_glyph_bits[] = {
   0xe0, 0x03, 0x98, 0x0f, 0x84, 0x1f, 0x82, 0x3f, 0x82, 0x3f, 0x81, 0x7f,
   0x81, 0x7f, 0xff, 0x7f, 0xff, 0x40, 0xff, 0x40, 0xfe, 0x20, 0xfe, 0x20,
   0xfc, 0x10, 0xf8, 0x0c, 0xe0, 0x03, 0x00, 0x00};

static void CreateMenuButtons(Widget parent)
{
    Widget menuButton, submenu, titleLabel, button;
    Pixmap cascadePixmap;
    Pixel fg, bg;
    Cardinal depth;
    XmString labelString;
    Arg args[20];
    int i, n;

    /* Create title label */

    labelString = XmStringCreateLocalized("MenuButton Widget");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    titleLabel = XmCreateLabel(parent, "title", args, n);
    XtManageChild(titleLabel);
    XmStringFree(labelString);


    /*
     * Create a MenuButton.
     * Add push buttons to the built-in popup menu.
     */

    labelString = XmStringCreateLocalized("Action");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    menuButton = DtCreateMenuButton(parent, "menuButton1", args, n);
    XtManageChild(menuButton);
    XmStringFree(labelString);

    XtVaGetValues(menuButton, DtNsubMenuId, &submenu, NULL);
    button = XmCreatePushButton(submenu, "Push", NULL, 0);
    XtManageChild(button);
    button = XmCreatePushButton(submenu, "Pull", NULL, 0);
    XtManageChild(button);
    button = XmCreatePushButton(submenu, "Turn", NULL, 0);
    XtManageChild(button);


    /*
     * Create a MenuButton.
     * Replace the built-in popup menu with a tear-off menu.
     * Add a custom pixmap in the colors of the MenuButton.
     */

    labelString = XmStringCreateLocalized("Movement");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    menuButton = DtCreateMenuButton(parent, "menuButton1", args, n);
    XtManageChild(menuButton);
    XmStringFree(labelString);

    /* Create a tear-off menu */

    n = 0;
    XtSetArg(args[0], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
    submenu = XmCreatePopupMenu(menuButton, "submenu", args, n);
    button = XmCreatePushButton(submenu, "Run", NULL, 0);
    XtManageChild(button);
    button = XmCreatePushButton(submenu, "Jump", NULL, 0);
    XtManageChild(button);
    button = XmCreatePushButton(submenu, "Stop", NULL, 0);
    XtManageChild(button);

    XtVaSetValues(menuButton, DtNsubMenuId, submenu, NULL);

    /* Create a pixmap using the menu button's colors and depth */

    XtVaGetValues(menuButton,
			XmNforeground, &fg,
			XmNbackground, &bg,
			XmNdepth, &depth,
			NULL);

    cascadePixmap = XCreatePixmapFromBitmapData(XtDisplay(menuButton),
				DefaultRootWindow(XtDisplay(menuButton)),
				(char*)menu_glyph_bits,
				menu_glyph_width, menu_glyph_height,
				fg, bg, depth);
    XtVaSetValues(menuButton, DtNcascadePixmap, cascadePixmap, NULL);
}

