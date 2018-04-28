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
/*
 *      $XConsortium: pal_glyph.c /main/3 1995/11/06 17:38:34 rswiston $
 *
 * @(#)pal_glyph.c	1.3 21 Feb 1994      cde_app_builder/src/ab
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * pal_glyph.c - Implements Palette Glyph object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>

const int    glyph_init_height = 175;
const int    glyph_init_width  = 400;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int	glyph_initialize(
                    ABObj   obj
                );
static Widget   glyph_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	glyph_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	glyph_prop_clear(
		    AB_PROP_TYPE type
		);
static int      glyph_prop_load(
                    ABObj   obj,
		    AB_PROP_TYPE type
                );
static int	glyph_prop_apply(
		    AB_PROP_TYPE type
		);

static BOOL	glyph_prop_pending(
		    AB_PROP_TYPE type
		);

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);


/*************************************************************************
**                                                                      **
**       Data                                                   	**
**                                                                      **
**************************************************************************/

PalItemInfo glyph_palitem_rec = {

    /* type             */  AB_TYPE_GLYPH,
    /* name             */  "Glyph",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  glyph_initialize,
    /* is_a_test        */  obj_is_glyph,
    /* prop_initialize  */  glyph_prop_init,
    /* prop_active	*/  glyph_prop_activate,
    /* prop_clear	*/  glyph_prop_clear,
    /* prop_load        */  glyph_prop_load,
    /* prop_apply	*/  glyph_prop_apply,
    /* prop_pending	*/  glyph_prop_pending

};
PalItemInfo *ab_glyph_palitem = &glyph_palitem_rec;

static ABObj current_obj[AB_PROP_TYPE_NUM_VALUES];

static Widget	prop_form[AB_PROP_TYPE_NUM_VALUES];

static Widget	name_label[AB_PROP_TYPE_NUM_VALUES],
		name_field[AB_PROP_TYPE_NUM_VALUES],
		name_cb[AB_PROP_TYPE_NUM_VALUES],

		colorf_label[AB_PROP_TYPE_NUM_VALUES],
		colorf_field[AB_PROP_TYPE_NUM_VALUES],
		colorf_cb[AB_PROP_TYPE_NUM_VALUES],

                monof_label[AB_PROP_TYPE_NUM_VALUES],
                monof_field[AB_PROP_TYPE_NUM_VALUES],
                monof_cb[AB_PROP_TYPE_NUM_VALUES],

		fg_label[AB_PROP_TYPE_NUM_VALUES],
		fg_button[AB_PROP_TYPE_NUM_VALUES],
		fg_field[AB_PROP_TYPE_NUM_VALUES],
		fg_cb[AB_PROP_TYPE_NUM_VALUES],
		fg_glyph[AB_PROP_TYPE_NUM_VALUES],

		bg_label[AB_PROP_TYPE_NUM_VALUES],
		bg_button[AB_PROP_TYPE_NUM_VALUES],
		bg_field[AB_PROP_TYPE_NUM_VALUES],
		bg_cb[AB_PROP_TYPE_NUM_VALUES],
		bg_glyph[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
glyph_initialize(
    ABObj     obj
)
{
    STRING name;

    name = obj_alloc_unique_name(obj_get_module(obj), "glyph", -1);
    obj_set_name(obj, name);
    util_free(name);

    return OK;

}

static Widget
glyph_prop_init(
    Widget       parent,
    AB_PROP_TYPE type
)
{
    Widget  prop_pane,
	    setting,
	    label,
	    button;
    Arg     args[20];
    int	    i;
    Pixel   parent_bg;
    XmString xmlabel;

    prop_form[type] = prop_pane = XtVaCreateManagedWidget("glyph_prop_pane",
            		xmFormWidgetClass,
            		parent,
            		XmNresizePolicy, XmRESIZE_ANY,
            		NULL);

    /* Set record of changebars currently "ON" to 0 */
    prop_init_change_count(prop_pane);

    XtVaGetValues(prop_pane,
			XmNbackground,	&parent_bg,
			NULL);

    /********************************************************************
     *  Create Name Setting					   	*
     ********************************************************************/
    name_field[type] = XtVaCreateManagedWidget("namefield",
			xmTextFieldWidgetClass,
			prop_pane,
			XmNshadowThickness, 	1,
			XmNhighlightThickness, 	1,
			XmNmaxLength, 		80,
			XmNleftAttachment,	XmATTACH_POSITION,
			XmNleftPosition,	prop_setting_anchor,
			XmNleftOffset,		prop_offset_from_lbl,
                        XmNrightAttachment,     XmATTACH_POSITION, 
                        XmNrightPosition,       prop_setting_end, 
			XmNtopAttachment,	XmATTACH_FORM,
			XmNtopOffset,		prop_setting_offset,
			NULL);
    setting = name_field[type];

    xmlabel = XmStringCreateLocalized("Object Name:");
    name_label[type] = label = XtVaCreateManagedWidget("prop_label",
                        xmLabelWidgetClass, 
                        prop_pane, 
                        XmNlabelString,         xmlabel, 
                        XmNalignment,           XmALIGNMENT_END,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_anchor,
			XmNleftAttachment,	XmATTACH_POSITION,
			XmNleftPosition,	prop_label_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,         	setting,  
			XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
			XmNbottomWidget,	setting,
                        NULL);
    XmStringFree(xmlabel);

    name_cb[type] = XtVaCreateManagedWidget("changebar",
                        xmSeparatorWidgetClass,
                        prop_pane,
                        XmNorientation,         XmVERTICAL,
                        XmNseparatorType,       XmSINGLE_LINE,
                        XmNwidth,               prop_cb_width, 
                        XmNbackground,          parent_bg,
                        XmNforeground,          parent_bg,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_cb_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
			XmNrightAttachment,	XmATTACH_POSITION,
			XmNrightPosition,	prop_label_anchor,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        XmNuserData,            PROP_CB_OFF,
                        NULL);
    XtAddCallback(name_field[type], XmNvalueChangedCallback, prop_field_chgCB, name_cb[type]); 
    XtVaSetValues(name_field[type], XmNuserData, PROP_EDIT, NULL);


    /******************************************************************** 
     *  Create Color Filename Setting                                   *   
     ********************************************************************/
    colorf_field[type] = XtVaCreateManagedWidget("field",
                        xmTextFieldWidgetClass,
                        prop_pane,
                        XmNshadowThickness,     1,
                        XmNhighlightThickness,  1,
                        XmNmaxLength,           80,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_setting_anchor,
                        XmNleftOffset,          prop_offset_from_lbl, 
                        XmNtopAttachment,       XmATTACH_WIDGET,
			XmNtopWidget,		setting,
                        XmNtopOffset,           prop_setting_offset,
			XmNrightAttachment,	XmATTACH_POSITION,
			XmNrightPosition,	prop_setting_end,
                        NULL);

    setting = colorf_field[type];
    xmlabel = XmStringCreateLocalized("Color Filename:");
    colorf_label[type] = XtVaCreateManagedWidget("prop_label",
                        xmLabelWidgetClass,
                        prop_pane,
                        XmNlabelString,         xmlabel,
                        XmNalignment,           XmALIGNMENT_END,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_anchor,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_label_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        NULL);
    XmStringFree(xmlabel); 

    colorf_cb[type] = XtVaCreateManagedWidget("changebar",
                        xmSeparatorWidgetClass,
                        prop_pane,
                        XmNorientation,         XmVERTICAL,
                        XmNseparatorType,       XmSINGLE_LINE,
                        XmNwidth,               prop_cb_width, 
                        XmNbackground,          parent_bg,
                        XmNforeground,          parent_bg,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_cb_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_label_anchor,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        XmNuserData,            PROP_CB_OFF,
                        NULL);
    XtAddCallback(colorf_field[type], XmNvalueChangedCallback, prop_field_chgCB, colorf_cb[type]);  
    XtVaSetValues(colorf_field[type], XmNuserData,      PROP_EDIT, NULL); 


    /********************************************************************
     *  Create Mono Filename Setting                                    *
     ********************************************************************/
    monof_field[type] = XtVaCreateManagedWidget("field",
                        xmTextFieldWidgetClass,
                        prop_pane,
                        XmNshadowThickness,     1,
                        XmNhighlightThickness,  1,
                        XmNmaxLength,           80,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_setting_anchor,
                        XmNleftOffset,          prop_offset_from_lbl,
                        XmNtopAttachment,       XmATTACH_WIDGET,
                        XmNtopWidget,           setting,
                        XmNtopOffset,           prop_setting_offset,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_end,
                        NULL);

    setting = monof_field[type];
    xmlabel = XmStringCreateLocalized("Color Filename:");
    monof_label[type] = XtVaCreateManagedWidget("prop_label",
                        xmLabelWidgetClass,
                        prop_pane,
                        XmNlabelString,         xmlabel,
                        XmNalignment,           XmALIGNMENT_END,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_anchor,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_label_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        NULL);
    XmStringFree(xmlabel);

    monof_cb[type] = XtVaCreateManagedWidget("changebar",
                        xmSeparatorWidgetClass,
                        prop_pane,
                        XmNorientation,         XmVERTICAL,
                        XmNseparatorType,       XmSINGLE_LINE,
                        XmNwidth,               prop_cb_width, 
                        XmNbackground,          parent_bg,
                        XmNforeground,          parent_bg,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_cb_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_label_anchor,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        XmNuserData,            PROP_CB_OFF,
                        NULL);
    XtAddCallback(monof_field[type], XmNvalueChangedCallback, prop_field_chgCB,
			monof_cb[type]);
    XtVaSetValues(monof_field[type], XmNuserData,      PROP_EDIT, NULL);



    /********************************************************************
     *  Create Background Color Setting                                 *
     ********************************************************************/
 
/*
    xmlabel = XmStringCreateLocalized("ColorChooser...");
    bg_button[type] = button = XtVaCreateManagedWidget("color_button",
                        xmPushButtonWidgetClass,
                        prop_pane,
                        XmNlabelString,		xmlabel,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_setting_anchor,
			XmNleftOffset,		prop_offset_from_lbl,
                        XmNtopAttachment,       XmATTACH_WIDGET,
                        XmNtopWidget,           setting,
                        XmNtopOffset,           prop_setting_offset,
                        NULL);
    XmStringFree(xmlabel); 


    xmlabel = XmStringCreateLocalized("   ");
    bg_glyph[type] =
                XtVaCreateManagedWidget("color_glyph[type]",
                        xmLabelWidgetClass,
                        prop_pane,
                        XmNlabelString,		xmlabel,
                        XmNborderWidth,         1,
                        XmNleftAttachment,      XmATTACH_WIDGET,
                        XmNleftWidget,          button,
                        XmNleftOffset,          4,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           button,
			XmNtopOffset,		2,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        button,
			XmNbottomOffset,	2,
                        NULL);
    XmStringFree(xmlabel); 

    bg_field[type] =
                XtVaCreateManagedWidget("color_field",
                        xmTextFieldWidgetClass,
                        prop_pane,
                        XmNshadowThickness,     1,
                        XmNhighlightThickness,  1,
                        XmNcolumns,             14,
                        XmNleftAttachment,      XmATTACH_WIDGET,
                        XmNleftWidget,          bg_glyph[type],
                        XmNleftOffset,          4,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           button,
                        XmNtopOffset,           -1,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_end,
                        NULL);
    setting = button;

    xmlabel = XmStringCreateLocalized("Background:");
    bg_label[type] = label = XtVaCreateManagedWidget("prop_label",
                        xmLabelWidgetClass,
                        prop_pane,
                        XmNlabelString,         xmlabel,
                        XmNalignment,           XmALIGNMENT_END,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_anchor,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_label_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        NULL);
    XmStringFree(xmlabel); 

    bg_cb[type] = XtVaCreateManagedWidget("changebar",
                        xmSeparatorWidgetClass,
                        prop_pane,
                        XmNorientation,         XmVERTICAL,
                        XmNseparatorType,       XmSINGLE_LINE,
                        XmNwidth,               prop_cb_width, 
                        XmNbackground,          parent_bg,
                        XmNforeground,          parent_bg,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_cb_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_label_anchor,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        XmNuserData,            PROP_CB_OFF,
                        NULL);
    XtAddCallback(bg_field[type], XmNvalueChangedCallback, prop_field_chgCB, bg_cb[type]);
    XtVaSetValues(bg_field[type], XmNuserData,      PROP_EDIT, NULL);

    /* Install color chooser */
    prop_add_color_chooser(bg_button[type],bg_glyph[type],bg_field[type]);

*/

    /********************************************************************
     *  Create Foreground Color Setting                                 *
     ********************************************************************/

/*
    xmlabel = XmStringCreateLocalized("ColorChooser...");
    fg_button[type] = button = XtVaCreateManagedWidget("color_button",
			xmPushButtonWidgetClass,
			prop_pane,
			XmNlabelString,		xmlabel,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_setting_anchor,
			XmNleftOffset,		prop_offset_from_lbl,
                        XmNtopAttachment,       XmATTACH_WIDGET,
                        XmNtopWidget,           setting,
                        XmNtopOffset,           prop_setting_offset,
			NULL);
    XmStringFree(xmlabel); 


    xmlabel = XmStringCreateLocalized("   ");
    fg_glyph[type] =
		XtVaCreateManagedWidget("color_glyph",
			xmLabelWidgetClass,
			prop_pane,
			XmNlabelString, 	xmlabel,
			XmNborderWidth,		1,
			XmNleftAttachment,	XmATTACH_WIDGET,
			XmNleftWidget,		button,
			XmNleftOffset,		4,
			XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget,		button,
			XmNtopOffset,		2,
			XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
			XmNbottomWidget,	button,
			XmNbottomOffset,	2,
			NULL);
    XmStringFree(xmlabel); 

    fg_field[type] =
		XtVaCreateManagedWidget("color_field[type]",
                        xmTextFieldWidgetClass,
                        prop_pane,
                        XmNshadowThickness,     1,
                        XmNhighlightThickness,  1,
			XmNcolumns,		14,
                        XmNleftAttachment,      XmATTACH_WIDGET, 
                        XmNleftWidget,          fg_glyph[type], 
			XmNleftOffset,		4,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           button,
			XmNtopOffset,		-1,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_end,
                        NULL);
    setting = button;

    xmlabel = XmStringCreateLocalized("Foreground:");
    fg_label[type] = label = XtVaCreateManagedWidget("prop_label",
                        xmLabelWidgetClass,
                        prop_pane,
                        XmNlabelString,         xmlabel,
                        XmNalignment,           XmALIGNMENT_END,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_setting_anchor,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_label_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        NULL);
    XmStringFree(xmlabel); 

    fg_cb[type] = XtVaCreateManagedWidget("changebar",
                        xmSeparatorWidgetClass,
                        prop_pane,
                        XmNorientation,         XmVERTICAL,
                        XmNseparatorType,       XmSINGLE_LINE,
                        XmNwidth,               prop_cb_width, 
                        XmNbackground,          parent_bg,
                        XmNforeground,          parent_bg,
                        XmNleftAttachment,      XmATTACH_POSITION,
                        XmNleftPosition,        prop_cb_anchor,
                        XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET,
                        XmNtopWidget,           setting,
                        XmNrightAttachment,     XmATTACH_POSITION,
                        XmNrightPosition,       prop_label_anchor,
                        XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomWidget,        setting,
                        XmNuserData,            PROP_CB_OFF,
                        NULL);
    XtAddCallback(fg_field[type], XmNvalueChangedCallback, prop_field_chgCB, fg_cb[type]);
    XtVaSetValues(fg_field[type], XmNuserData,      PROP_EDIT, NULL);

    /* Install color chooser */
    prop_add_color_chooser(fg_button[type],fg_glyph[type],fg_field[type]);

*/

    /*
     * Prop Init done.
     */

    current_obj[type] = NULL;
    if (type == AB_PROP_REVOLVING)
        XtVaSetValues(parent, 
			XmNuserData, current_obj[type], 
			NULL);

    return(prop_pane);

}

static int
glyph_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    /* 
     * Set sensitivity of each Prop Sheet setting
     */
    ui_set_active(name_label[type], active);
    ui_set_active(name_field[type], active);

    ui_set_active(colorf_label[type], active);
    ui_set_active(colorf_field[type], active);

    ui_set_active(monof_label[type], active);
    ui_set_active(monof_field[type], active);

/*
    ui_set_active(fg_label[type], active);
    ui_set_active(fg_button[type], active);
    ui_set_active(fg_field[type], active);
*/

    return OK;
}

static int
glyph_prop_clear(
    AB_PROP_TYPE type
)
{
    if (current_obj[type] == NULL)
	return OK;

    /* Clear Name */
    prop_set_field(name_field[type], "");
 
    /* Clear Color Filename */
    prop_set_field(colorf_field[type], "");

    /* Clear Mono Filename */
    prop_set_field(monof_field[type], "");
 
    /* Clear Background Color */
/*
    prop_set_field(bg_field[type], "");
    prop_set_color_glyph(bg_glyph[type], NULL);
*/
 
    /* Clear Foreground Color */
/*
    prop_set_field(fg_field[type], "");
    prop_set_color_glyph(fg_glyph[type], NULL);
*/
    turnoff_changebars(type);

    current_obj[type] = NULL;
    if (type == AB_PROP_REVOLVING)
	XtVaSetValues(ab_glyph_palitem->rev_prop_frame, 
			XmNuserData, current_obj[type], 
			NULL);

    return OK;

}

static int
glyph_prop_load(
    ABObj        obj,
    AB_PROP_TYPE type
)
{
    BOOL	   reset = FALSE;

    if (obj == NULL) 
    {
	if (current_obj[type] != NULL) 
	{
	    /* Reset props for current object */
	    obj = current_obj[type];
	    reset = TRUE;
	}
	else
	    return ERROR;
    }
    else if (!obj_is_base_win(obj))
	return ERROR;
    else
    {
	/* Load props for new object */
    	current_obj[type] = obj;
	if (type == AB_PROP_REVOLVING)
	    XtVaSetValues(ab_glyph_palitem->rev_prop_frame, 
			XmNuserData, current_obj[type], 
			NULL);
    }

    /* Load Name */
    prop_set_field(name_field[type], obj_get_name(obj));

    /* Load Color Filename */
/*
    prop_set_field(colorf_field[type], obj_get_color_filename(obj));
*/

    /* Load Mono Filename */
/*
    prop_set_field(monof_field[type], obj_get_mono_filename(obj));
*/

    /* Load Background Color */ 
/*
    prop_set_field(bg_field[type], obj_get_bg_color(obj)); 
    prop_set_color_glyph(bg_glyph[type], obj_get_bg_color(obj));
*/
 
    /* Load Foreground Color */ 
/*
    prop_set_field(fg_field[type], obj_get_fg_color(obj)); 
    prop_set_color_glyph(fg_glyph[type], obj_get_fg_color(obj)); 
*/

    /* Turn off changebars */
    turnoff_changebars(type);

    return OK;
}

int
glyph_prop_apply(
    AB_PROP_TYPE   type
)
{
    STRING		value;

    if (current_obj[type] == NULL)
	return ERROR;

    if (!verify_props(type))
	return ERROR;

    if (prop_changed(name_cb[type]))
    {
	value = ui_get_field_string(name_field[type]);
	abobj_set_name(current_obj[type], value);
	XtFree(value);
    }
    if (prop_changed(colorf_cb[type]))
    {
/*
	value = ui_get_field_string(label_field[type]);
	abobj_set_label(current_obj[type], current_obj[type]->label_type, value);
	XtFree(value);
*/
    }
/*
    if (prop_changed(fg_cb[type]))
    {
	value = ui_get_field_string(fg_field[type]);
	abobj_set_foreground_color(current_obj[type], value);
	XtFree(value);
	prop_set_color_glyph(fg_glyph[type], obj_get_fg_color(current_obj[type]));
    }
    if (prop_changed(bg_cb[type])) 
    {
        value = ui_get_field_string(bg_field[type]);
        abobj_set_background_color(current_obj[type], value); 
        XtFree(value); 
        prop_set_color_glyph(bg_glyph[type], obj_get_bg_color(current_obj[type]));
    }
*/

    turnoff_changebars(type);

    return OK;

}

static BOOL
glyph_prop_pending(
    AB_PROP_TYPE type
)
{
    if (prop_get_change_count(prop_form[type]) > 0)
	return TRUE; /* Changes pending */
    else
	return FALSE;/* No Changes pending */
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{

    if (prop_changed(name_cb[type]) && !prop_name_ok(current_obj[type], name_field[type]))
	return FALSE;

/*
    if (prop_changed(fg_cb[type]) && !prop_color_ok(fg_field[type]))
	return FALSE;

    if (prop_changed(bg_cb[type]) && !prop_color_ok(bg_field[type]))
	return FALSE;
*/

    return TRUE;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    /* Ensure all Changebars are OFF and change_count is 0
     */
    prop_set_changebar(name_cb[type], 	     	PROP_CB_OFF);
    prop_set_changebar(colorf_cb[type], 	PROP_CB_OFF);
    prop_set_changebar(monof_cb[type], 		PROP_CB_OFF);
/*
    prop_set_changebar(bg_cb[type], 		PROP_CB_OFF);
    prop_set_changebar(fg_cb[type], 		PROP_CB_OFF);
*/

    prop_init_change_count(prop_form[type]);

}
