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
 *      $XConsortium: objxm_objects.c /main/6 1996/10/28 16:24:20 mustafa $
 *
 *      @(#)objxm_objects.c	1.117 23 May 1995
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
 * objxm_objects.c - Implements methods for configuring the Set
 *		     of UI Objects into their appropriate
 *		     Motif hierarchies
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MenuShell.h>
#include <Xm/PanedW.h>
#include <Xm/ScrolledW.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/FileSB.h>
#include <Xm/TextF.h>
#include <Xm/List.h>
#include <Dt/ComboBox.h>
#include <Dt/TermPrim.h>
#include <Dt/Term.h>
#include <Dt/SpinBox.h>
#include <ab_private/obj.h>
#include <ab_private/obj_notify.h>
#include "objxmP.h"


#define	SELECT_OBJ_KEY	"__selected__"


/*************************************************************************
**                                                                      **
**       Private Function Declartions					**
**                                                                      **
**************************************************************************/
/*
 * General methods
 */
#define check_obj_class_name(obj) \
	    ((obj_get_class_name(obj) != NULL)? \
	         OK \
  	     : \
	         obj_set_class_name(obj, objxm_obj_get_default_motif_class(obj)))

/*
 * XmConfigure/UnConfigure Methods
 */
static int	mainwin_xmconfig(
		    ABObj	obj
		);
static int      mainwin_unconfig(
                    ABObj       obj
                );
static int      choice_xmconfig(
                    ABObj       obj
                );
static int	choice_unconfig(
		    ABObj	obj
		);
static int      combo_box_xmconfig(
                    ABObj       obj
                );
static int	combo_box_unconfig(
		    ABObj	obj
		);
static ABObj    combo_box_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static int      cpanel_xmconfig(
                    ABObj       obj
                );
static int      cpanel_unconfig(
                    ABObj       obj
                );
static int	custdlg_xmconfig(
		    ABObj	obj
		);
static int      custdlg_unconfig(
                    ABObj       obj
                );
static int      layer_xmconfig(
                    ABObj       obj
                );
static int	layer_unconfig(
		    ABObj	obj
		);
static int      group_xmconfig(
                    ABObj       obj
                );
static int      group_unconfig(
                    ABObj       obj
                );
static int      item_xmconfig(
                    ABObj       obj
                );
static int      list_xmconfig(
                    ABObj       obj
		);
static int      list_unconfig(
                    ABObj       obj
                );
static int      menu_xmconfig(
                    ABObj       obj
                );
static int      menu_unconfig(
                    ABObj       obj
                );
static int      menubar_xmconfig(
                    ABObj       obj
                );
static int      pane_xmconfig(
                    ABObj       obj
                );
static int      pane_unconfig(
                    ABObj       obj
                );
static int	simple_xmconfig(
                    ABObj       obj
                );
static int      simple_unconfig(
                    ABObj       obj
                );
static int      scale_xmconfig(
                    ABObj       obj
                );
static int      scale_unconfig(
                    ABObj       obj
                );
static int      spin_box_xmconfig(
                    ABObj       obj
                );
static int      spin_box_unconfig(
                    ABObj       obj
                );
static int      textf_xmconfig(
                    ABObj       obj
                );
static int      textf_unconfig(
                    ABObj       obj
                );
static int      fchooser_xmconfig(
                    ABObj       obj
                );
static int      box_unconfig(
                    ABObj       obj
                );

/*
 * GetConfigObj Methods
 */
static ABObj    mainwin_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    choice_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    container_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    custdlg_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj	layer_get_config_obj(
		    ABObj	    obj,
		    AB_CFG_OBJ_TYPE type
		);
static ABObj    item_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    list_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    menu_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    pane_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    separator_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    spin_box_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    simple_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj    textf_get_config_obj(
                    ABObj           obj,
                    AB_CFG_OBJ_TYPE type
                );
static ABObj 	scale_get_config_obj(
		    ABObj       obj,
		    AB_CFG_OBJ_TYPE type
		);
static ABObj 	box_get_config_obj(
		    ABObj       obj,
		    AB_CFG_OBJ_TYPE type
		);
static ABObj 	panedwin_get_config_obj(
		    ABObj       obj,
		    AB_CFG_OBJ_TYPE type
		);

/*
 * Resource Setting methods
 */
static int	mainwin_set_args(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);
static int	simple_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	choice_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	combo_box_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	cpanel_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	custdlg_set_args(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);
static int      layer_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	group_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	item_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	drawp_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	separator_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	spin_box_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	termp_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	textp_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	list_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	menu_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	menubar_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	textf_set_args(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int 	scale_set_args(
		    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
		);
static int 	fchooser_set_args(
		    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
		);

/*
 * Post-Instantiation Methods
 */
static int	post_set_size(
		    ABObj	obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int      mainwin_post_inst(
                    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
                );
static int	choice_post_inst(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);
static int	cpanel_post_inst(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);
static int      dialog_post_inst(
                    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
                );
static int      drawp_post_inst(
                    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
                );
static int      item_post_inst(
                    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
                );
static int      list_post_inst(
                    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype
                );
static int      menubar_post_inst(
                    ABObj       obj,
                    OBJXM_CONFIG_TYPE ctype
                );
static int	termp_post_inst(
		    ABObj	obj,
                    OBJXM_CONFIG_TYPE ctype
		);
static int	fchooser_post_inst(
		    ABObj	obj,
                    OBJXM_CONFIG_TYPE ctype
		);
static void 	clear_fchooser_fields(
		    Widget      widget
		);

/*
 * General
 */

static ABObj	get_menu_ref(
		    ABObj	obj
		);


/*************************************************************************
**                                                                      **
**       Data                                                           **
**                                                                      **
**************************************************************************/

XmConfigInfoRec button_config_rec = {
    /* xmconfig         */  simple_xmconfig,
    /* xmunconfig	*/  simple_unconfig,
    /* get_config_obj   */  simple_get_config_obj,
    /* set_args         */  simple_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_button,
};

XmConfigInfoRec choice_config_rec = {
    /* xmconfig         */  choice_xmconfig,
    /* xmunconfig       */  choice_unconfig,
    /* get_config_obj   */  choice_get_config_obj,
    /* set_args         */  choice_set_args,
    /* post_instantiate */  choice_post_inst,
    /* is_a_test        */  obj_is_choice,
};

XmConfigInfoRec combo_box_config_rec = {
    /* xmconfig         */  combo_box_xmconfig,
    /* xmunconfig	*/  combo_box_unconfig,
    /* get_config_obj   */  combo_box_get_config_obj,
    /* set_args         */  combo_box_set_args,
    /* post_instantiate */  post_set_size,
    /* is_a_test        */  obj_is_combo_box,
};

XmConfigInfoRec cpanel_config_rec = {
    /* xmconfig         */  cpanel_xmconfig,
    /* xmunconfig       */  cpanel_unconfig,
    /* get_config_obj   */  container_get_config_obj,
    /* set_args         */  cpanel_set_args,
    /* post_instantiate */  cpanel_post_inst,
    /* is_a_test        */  obj_is_control_panel,
};

XmConfigInfoRec custdlg_config_rec = {
    /* xmconfig         */  custdlg_xmconfig,
    /* xmunconfig       */  custdlg_unconfig,
    /* get_config_obj   */  custdlg_get_config_obj,
    /* set_args         */  custdlg_set_args,
    /* post_instantiate */  dialog_post_inst,
    /* is_a_test        */  obj_is_popup_win,
};

XmConfigInfoRec layer_config_rec = {
    /* xmconfig         */  layer_xmconfig,
    /* xmunconfig       */  layer_unconfig,
    /* get_config_obj   */  layer_get_config_obj,
    /* set_args         */  layer_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_layers,
};

XmConfigInfoRec group_config_rec = {
    /* xmconfig         */  group_xmconfig,
    /* xmunconfig       */  group_unconfig,
    /* get_config_obj   */  container_get_config_obj,
    /* set_args         */  group_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_group,
};

XmConfigInfoRec drawp_config_rec = {
    /* xmconfig         */  pane_xmconfig,
    /* xmunconfig       */  pane_unconfig,
    /* get_config_obj   */  pane_get_config_obj,
    /* set_args         */  drawp_set_args,
    /* post_instantiate */  drawp_post_inst,
    /* is_a_test        */  obj_is_drawing_area,
};

XmConfigInfoRec item_config_rec = {
    /* xmconfig         */  item_xmconfig,
    /* xmunconfig	*/  simple_unconfig,
    /* get_config_obj   */  item_get_config_obj,
    /* set_args         */  item_set_args,
    /* post_instantiate */  item_post_inst,
    /* is_a_test        */  obj_is_item,
};

XmConfigInfoRec label_config_rec = {
    /* xmconfig         */  simple_xmconfig,
    /* xmunconfig	*/  simple_unconfig,
    /* get_config_obj   */  simple_get_config_obj,
    /* set_args         */  simple_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_label,
};

XmConfigInfoRec list_config_rec = {
    /* xmconfig         */  list_xmconfig,
    /* xmunconfig	*/  list_unconfig,
    /* get_config_obj   */  list_get_config_obj,
    /* set_args         */  list_set_args,
    /* post_instantiate */  list_post_inst,
    /* is_a_test        */  obj_is_list,
};

XmConfigInfoRec mainwin_config_rec = {
    /* xmconfig         */  mainwin_xmconfig,
    /* xmunconfig       */  mainwin_unconfig,
    /* get_config_obj   */  mainwin_get_config_obj,
    /* set_args         */  mainwin_set_args,
    /* post_instantiate */  mainwin_post_inst,
    /* is_a_test        */  obj_is_base_win,
};

XmConfigInfoRec menu_config_rec = {
    /* xmconfig         */  menu_xmconfig,
    /* xmunconfig	*/  menu_unconfig,
    /* get_config_obj   */  menu_get_config_obj,
    /* set_args         */  menu_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_menu,
};

XmConfigInfoRec menubar_config_rec = {
    /* xmconfig         */  menubar_xmconfig,
    /* xmunconfig	*/  simple_unconfig,
    /* get_config_obj   */  container_get_config_obj,
    /* set_args         */  menubar_set_args,
    /* post_instantiate */  menubar_post_inst,
    /* is_a_test        */  obj_is_menubar,
};

XmConfigInfoRec separator_config_rec = {
    /* xmconfig         */  simple_xmconfig,
    /* xmunconfig	*/  simple_unconfig,
    /* get_config_obj   */  separator_get_config_obj,
    /* set_args         */  separator_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_separator,
};

XmConfigInfoRec spin_box_config_rec = {
    /* xmconfig         */  spin_box_xmconfig,
    /* xmunconfig	*/  spin_box_unconfig,
    /* get_config_obj   */  spin_box_get_config_obj,
    /* set_args         */  spin_box_set_args,
    /* post_instantiate */  post_set_size,
    /* is_a_test        */  obj_is_spin_box,
};

XmConfigInfoRec termp_config_rec = {
    /* xmconfig         */  pane_xmconfig,
    /* xmunconfig	*/  pane_unconfig,
    /* get_config_obj   */  pane_get_config_obj,
    /* set_args         */  termp_set_args,
    /* post_instantiate */  termp_post_inst,
    /* is_a_test        */  obj_is_term_pane,
};

XmConfigInfoRec textf_config_rec = {
    /* xmconfig         */  textf_xmconfig,
    /* xmunconfig	*/  textf_unconfig,
    /* get_config_obj   */  textf_get_config_obj,
    /* set_args         */  textf_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_text_field,
};

XmConfigInfoRec textp_config_rec = {
    /* xmconfig         */  pane_xmconfig,
    /* xmunconfig	*/  pane_unconfig,
    /* get_config_obj   */  pane_get_config_obj,
    /* set_args         */  textp_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_text_pane,
};

XmConfigInfoRec scale_config_rec = {
    /* xmconfig         */  scale_xmconfig,
    /* xmunconfig	*/  scale_unconfig,
    /* get_config_obj   */  scale_get_config_obj,
    /* set_args         */  scale_set_args,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_scale,
};

XmConfigInfoRec file_chooser_config_rec = {
    /* xmconfig         */  fchooser_xmconfig,
    /* xmunconfig	*/  box_unconfig,
    /* get_config_obj   */  box_get_config_obj,
    /* set_args         */  fchooser_set_args,
    /* post_instantiate */  fchooser_post_inst,
    /* is_a_test        */  obj_is_file_chooser,
};

XmConfigInfoRec panedwin_config_rec = {
    /* xmconfig         */  simple_xmconfig,
    /* xmunconfig       */  simple_unconfig,
    /* get_config_obj   */  panedwin_get_config_obj,
    /* set_args         */  NULL,
    /* post_instantiate */  NULL,
    /* is_a_test        */  obj_is_paned_win,
};


static BOOL	init_done = FALSE;

Widget		ObjxmP_toplevel = NULL;
int		ObjxmP_visual_depth = 0;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Initialize Data & state information for Objxm library.
 * "toplevel" can be a valid widget ID (when called by AB Front-end)
 * or NULL (when called from code-generator)
 */
int
objxm_init(
    Widget	toplevel
)
{
   if (!init_done)
   {
	ObjxmP_toplevel = toplevel;
	if (toplevel != NULL)
	    XtVaGetValues(toplevel, XmNdepth, &ObjxmP_visual_depth, NULL);

	objxm_strs_init();

   	objxm_register_config_info(&mainwin_config_rec);
   	objxm_register_config_info(&button_config_rec);
   	objxm_register_config_info(&choice_config_rec);
   	objxm_register_config_info(&combo_box_config_rec);
   	objxm_register_config_info(&cpanel_config_rec);
   	objxm_register_config_info(&custdlg_config_rec);
   	objxm_register_config_info(&drawp_config_rec);
   	objxm_register_config_info(&group_config_rec);
   	objxm_register_config_info(&item_config_rec);
   	objxm_register_config_info(&label_config_rec);
   	objxm_register_config_info(&layer_config_rec);
   	objxm_register_config_info(&list_config_rec);
   	objxm_register_config_info(&separator_config_rec);
   	objxm_register_config_info(&spin_box_config_rec);
   	objxm_register_config_info(&menu_config_rec);
   	objxm_register_config_info(&menubar_config_rec);
   	objxm_register_config_info(&termp_config_rec);
   	objxm_register_config_info(&textf_config_rec);
   	objxm_register_config_info(&textp_config_rec);
   	objxm_register_config_info(&scale_config_rec);
   	objxm_register_config_info(&file_chooser_config_rec);
   	objxm_register_config_info(&panedwin_config_rec);

	obj_add_destroy_callback(objxmP_destroy, NULL);

   	init_done = TRUE;
   }

   return OK;
}

/*
 * Create the footer-select area for Window Objects:
 */
static int
xmconfig_window_select_area(
    ABObj	winobj,
    ABObj	parentobj
)
{
    ABObj	msgobj, modobj, selobj;

    /* Create MessageArea SubObj */
    msgobj  = obj_create(AB_TYPE_LABEL, NULL);
    msgobj->part_of = winobj;
    obj_append_child(parentobj, msgobj);
    obj_set_class_name(msgobj, _xmForm);
    obj_set_flag(msgobj, NoCodeGenFlag);
    obj_set_unique_name(msgobj,
	ab_ident_from_name_and_label(obj_get_name(winobj),"status_area"));

    /* Create StatusArea SubObj inside MessageArea */
    modobj = obj_create(AB_TYPE_LABEL, NULL);
    modobj->part_of = winobj;
    obj_append_child(msgobj, modobj);
    obj_set_unique_name(modobj,
        ab_ident_from_name_and_label(obj_get_name(winobj),"module_display"));
    obj_set_class_name(modobj, _xmLabel);
    obj_set_attachment(modobj, AB_CP_WEST, AB_ATTACH_GRIDLINE, (void*)0, 0);
    obj_set_attachment(modobj, AB_CP_EAST, AB_ATTACH_GRIDLINE, (void*)50, 0);
    obj_set_flag(modobj, NoCodeGenFlag);

    /* Create Select-Status SubObj inside MessageArea*/
    selobj = obj_create(AB_TYPE_LABEL, NULL);
    selobj->part_of = winobj;
    obj_append_child(msgobj, selobj);
    obj_set_label(selobj, SELECT_OBJ_KEY); /* Key for identifying */
    obj_set_unique_name(selobj,
        ab_ident_from_name_and_label(obj_get_name(winobj),"select_display"));
    obj_set_class_name(selobj, _xmLabel);
    obj_set_attachment(selobj, AB_CP_WEST, AB_ATTACH_GRIDLINE, (void*)50, 0);
    obj_set_attachment(selobj, AB_CP_EAST, AB_ATTACH_GRIDLINE, (void*)100, 0);
    obj_set_flag(selobj, NoCodeGenFlag);

    return 0;
}

static int
set_window_select_area_args(
    ABObj       obj
)
{
    ABObj       modobj, selobj;
    ABObj	msgobj;

    modobj = objxm_comp_get_subobj(obj, AB_CFG_STATUS_OBJ);
    selobj = objxm_comp_get_subobj(obj, AB_CFG_SELECT_OBJ);

    msgobj = obj_get_parent(modobj);

    /* Pane Resources */
    objxm_obj_set_ui_arg(msgobj, AB_ARG_INT, XmNpositionIndex, XmLAST_POSITION);

    /* REMIND: aim - memory leak */
    /* Module-area Resources */
    objxm_obj_set_ui_arg(modobj, AB_ARG_XMSTRING, XmNlabelString,
                XmStringCreateLocalized(obj_get_name(obj_get_module(obj))));
    objxm_obj_set_ui_arg(modobj, AB_ARG_BOOLEAN, XmNrecomputeSize, FALSE);
    objxm_obj_set_ui_arg(modobj, AB_ARG_LITERAL, XmNalignment, XmALIGNMENT_BEGINNING);

    /* Select-area Resources */
    objxm_obj_set_ui_arg(selobj, AB_ARG_XMSTRING, XmNlabelString,
                XmStringCreateLocalized("         "));
    objxm_obj_set_ui_arg(selobj, AB_ARG_BOOLEAN, XmNrecomputeSize, FALSE);
    objxm_obj_set_ui_arg(selobj, AB_ARG_LITERAL, XmNalignment, XmALIGNMENT_END);

    return 0;

}

static int
make_pane_fixed_height(
    ABObj       paneobj,
    ABObj       child,
    int		height,
    int         margin
)
{
    Dimension   hgt;
    Dimension   pane_hgt = 0;

    /* If a child is specified, it's height takes preference,
     * else the "height" parameter is used.
     */
    if (child != NULL)
    {
	if (child->ui_handle != NULL)
    	    XtVaGetValues((Widget)child->ui_handle,
                            XmNheight,      &hgt,
                            NULL);
	else
	    return -1;
    }
    else
	hgt = (Dimension)height;

    pane_hgt = (Dimension)(hgt + margin);

    /* If something goes wrong, at least force the pane to be visible! */
    objxm_obj_set_ui_arg(paneobj, AB_ARG_INT, XmNpaneMinimum,
		pane_hgt > 1? pane_hgt : 32);
    objxm_obj_set_ui_arg(paneobj, AB_ARG_INT, XmNpaneMaximum,
		pane_hgt > 1? pane_hgt : 32);

    obj_set_flag(paneobj, AttrChangedFlag);

    return 0;
}

static int
post_inst_window_select_area(
    ABObj	obj
)
{
    ABObj	selobj, msgobj;

    selobj = objxm_comp_get_subobj(obj, AB_CFG_SELECT_OBJ);
    msgobj = obj_get_parent(selobj);

    make_pane_fixed_height(msgobj, selobj, 0, 0);

    return 0;
}


/*
 ************************************************
 ************************************************
 */
ABObj
objxm_comp_mainwin_get_area(
    ABObj               obj,
    AB_CONTAINER_TYPE   area_type
)
{
    ABObj	pwobj, mwobj, child;
    int 	num_children;
    int 	i;

    if (!obj_is_base_win(obj))
        return NULL;

    if (obj_has_flag(obj, XmConfiguredFlag))
    {
    	pwobj = obj_get_child(obj, 0);
    	mwobj = obj_get_child(pwobj, 0);
    }
    else
	mwobj = obj;

    num_children = obj_get_num_children(mwobj);
    for (i = 0; i < num_children; i++)
    {
        child = obj_get_child(mwobj, i);
	if (child->type == AB_TYPE_CONTAINER &&
	    obj_get_subtype(child) == area_type)
	    return child;
    }
    return NULL;
}

static int
mainwin_xmconfig(
    ABObj    obj
)
{
    ABObj		project = obj_get_project(obj);
    ABObj               pwobj, mwobj, workobj;
    ABObj               menubar, toolbar, footer;

    /* ApplicationShell */
    obj->part_of = obj;
    if (obj_get_root_window(project) == obj)
    	obj_set_class_name(obj, _applicationShell);
    else
	obj_set_class_name(obj, _topLevelShell);

    /* Create PanedWindow SubObj (used for BUILD MODE ONLY) */
    pwobj = obj_create(AB_TYPE_CONTAINER, NULL);
    pwobj->part_of = obj;
    obj_set_class_name(pwobj, _xmPanedWindow);
    obj_set_flag(pwobj, NoCodeGenFlag);

    /* Create MainWindow SubObj */
    mwobj = obj_create(AB_TYPE_CONTAINER, NULL);
    mwobj->part_of = obj;
    obj_set_class_name(mwobj, _xmMainWindow);

    /* If Window has a Menubar child, reparent it */
    menubar = objxm_comp_mainwin_get_area(obj, AB_CONT_MENU_BAR);
    if (menubar != NULL)
    {
        obj_unparent(menubar);
        obj_insert_child(mwobj, menubar, 0);
    }

    /* If Window has a Toolbar child, reparent it */
    toolbar = objxm_comp_mainwin_get_area(obj, AB_CONT_TOOL_BAR);
    if (toolbar != NULL)
    {
        obj_unparent(toolbar);
        obj_append_child(mwobj, toolbar);
    }

    /* Create WorkArea SubObj */
    workobj = obj_create(AB_TYPE_CONTAINER, NULL);
    workobj->part_of = obj;
    obj_append_child(mwobj, workobj);
    obj_set_class_name(workobj, _xmForm);

    /* If Window has a Footer child, reparent it */
    footer = objxm_comp_mainwin_get_area(obj, AB_CONT_FOOTER);
    if (footer != NULL)
    {
        obj_unparent(footer);
        obj_append_child(mwobj, footer);
    }

    /* Move any remaining children to be parented off WorkObj
     * BEFORE the ShellObj is parented off obj
     */
    obj_move_children(workobj, obj);
    obj_append_child(pwobj, mwobj);
    obj_append_child(obj, pwobj);
    obj_set_unique_name(pwobj,
        ab_ident_from_name_and_label(obj_get_name(obj), "panedwin"));
    obj_set_unique_name(mwobj,
	ab_ident_from_name_and_label(obj_get_name(obj), "mainwin"));
    obj_set_unique_name(workobj,
        ab_ident_from_name_and_label(obj_get_name(obj), "form"));

    xmconfig_window_select_area(obj, pwobj);

    return OK;

}

static int
mainwin_unconfig(
    ABObj       obj
)
{
    ABObj       workobj, pwobj, mwobj;
    ABObj       menubar, toolbar, footer;

    workobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    pwobj = obj_get_child(obj, 0);
    mwobj = obj_get_child(pwobj, 0);

    menubar = objxm_comp_mainwin_get_area(obj, AB_CONT_MENU_BAR);
    toolbar = objxm_comp_mainwin_get_area(obj, AB_CONT_TOOL_BAR);
    footer  = objxm_comp_mainwin_get_area(obj, AB_CONT_FOOTER);

    if (menubar != NULL)
	obj_reparent(menubar, obj);

    if (toolbar != NULL)
	obj_reparent(toolbar, obj);

    if (footer != NULL)
	obj_reparent(footer, obj);

    /* Move all other Children to RootObj */
    obj_move_children(obj, workobj);

    obj_destroy(pwobj); /* destroys PanedWindow,MainWindow & Message SubObjs */

    return OK;

}
static ABObj
mainwin_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    ABObj workobj = NULL;
    ABObj pwobj, mwobj, msgobj, child;
    int   i, num_children;

    pwobj = obj_get_child(obj, 0);
    mwobj = obj_get_child(pwobj, 0);
    msgobj = obj_get_child(pwobj, 1);

    switch(type)
    {
        case AB_CFG_WINDOW_PW_OBJ:
            cfg_obj = pwobj;
            break;
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_LABEL_OBJ:
            cfg_obj = obj;
            break;
        case AB_CFG_STATUS_OBJ:
	    cfg_obj = obj_get_child(msgobj, 0);
	    break;
        case AB_CFG_SELECT_OBJ:
            cfg_obj = obj_get_child(msgobj, 1);
	    break;
        case AB_CFG_MENU_PARENT_OBJ:
        case AB_CFG_WINDOW_MW_OBJ:
	case AB_CFG_HELP_OBJ:
            cfg_obj = mwobj;
	    break;
	case AB_CFG_DND_OBJ:
	case AB_CFG_SIZE_OBJ:
        case AB_CFG_PARENT_OBJ:
        {
	    num_children = obj_get_num_children(mwobj);
            for (i = 0; i < num_children; i++)
            {
                child = obj_get_child(mwobj, i);
                if (obj_is_control_panel(child) && child->part_of == obj)
		{
                    workobj = child;
		    break;
		}
            }
	    cfg_obj = workobj;
            break;
        }

        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
            cfg_obj = NULL;
            break;

        default:
	    util_dprintf(1, "mainwin_get_config_obj: unknown type\n");
            cfg_obj = NULL;
    }

    return(cfg_obj);
}

static int
mainwin_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    STRING      label;
    ABObj       module = obj_get_module(obj);
    ABObj	pwobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_PW_OBJ);
    ABObj	mwobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_MW_OBJ);
    ABObj       shobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj       workobj  = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);
    BOOL        cgen_args;

    cgen_args = (ctype == OBJXM_CONFIG_CODEGEN? TRUE : FALSE);

    /* Shell Resources */
    objxm_comp_set_icon_args(obj, ctype);

    if ((label = obj_get_label(obj)) != NULL)
        objxm_obj_set_ui_arg(shobj, AB_ARG_STRING, XmNtitle, label);

    objxm_obj_set_ui_arg(shobj, AB_ARG_BOOLEAN, XmNallowShellResize, TRUE);

    /* PanedWindow Resources */
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNmarginWidth, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNmarginHeight, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNspacing, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNsashHeight, 1);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNsashWidth, 1);

    /* MainWindow Resources */
    if (!cgen_args)
    	objxm_obj_set_ui_arg(mwobj, AB_ARG_BOOLEAN, XmNallowResize,       True);

    /* Form WorkArea resources */
    objxm_obj_set_literal_ui_arg(workobj, ctype,  XmNresizePolicy,
	obj_get_width(obj) == -1? XmRESIZE_ANY : XmRESIZE_GROW);
    objxm_obj_set_ui_arg(workobj, AB_ARG_INT,     XmNmarginWidth,      0);
    objxm_obj_set_ui_arg(workobj, AB_ARG_INT,     XmNmarginHeight,     0);

    /* Set the interface name and select-state on the footer */
    if (ctype == OBJXM_CONFIG_BUILD)
	set_window_select_area_args(obj);

    return 0;
}
static int
mainwin_post_inst(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	workobj  = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);
    ABObj	mwobj    = obj_get_parent(workobj);

    if (ctype == OBJXM_CONFIG_BUILD)
    {
        post_inst_window_select_area(obj);

	XtManageChild(objxm_get_widget(workobj));
	util_dprintf(3,"MANAGING %s\n", obj_get_name(workobj));
	XtVaSetValues(objxm_get_widget(mwobj),
		XmNworkWindow,	objxm_get_widget(workobj),
		NULL);
    }
    else
    {
    	objxm_obj_set_widget_ui_arg(mwobj, ctype, XmNworkWindow, workobj);
        obj_set_flag(mwobj, AttrChangedFlag);
    }
    return 0;
}

/*
 ************************************************
 ** Custom Dialog Methods			*
 ************************************************
 */

ABObj
objxm_comp_custdlg_get_area(
    ABObj               obj,
    AB_CONTAINER_TYPE   area_type
)
{
    ABObj       pwobj, bbobj, child;
    int         num_children;
    int         i;

    if (!obj_is_popup_win(obj))
        return NULL;

    if (obj_has_flag(obj, XmConfiguredFlag))
    {
	bbobj = obj_get_child(obj, 0);
        pwobj = obj_get_child(bbobj, 0);
    }
    else
        pwobj = obj;

    num_children = obj_get_num_children(pwobj);
    for (i = 0; i < num_children; i++)
    {
        child = obj_get_child(pwobj, i);
        if (child->type == AB_TYPE_CONTAINER &&
            obj_get_subtype(child) == area_type)
            return child;
    }
    return NULL;

}
static int
custdlg_xmconfig(
    ABObj    obj
)
{
    ABObj    		bbobj, pwobj, workobj;
    ABObj		button_panel, footer;
    ABObj		apobj = NULL;
    STRING		name;

    /* DialogShell */
    obj->part_of = obj;
    obj_set_class_name(obj, _xmDialogShell);

    /* Dialog-Form */
    bbobj = obj_create(AB_TYPE_CONTAINER, NULL);
    bbobj->part_of = obj;
    obj_set_class_name(bbobj, _xmForm);

    /* Create PanedWindow SubObj */
    pwobj = obj_create(AB_TYPE_CONTAINER, NULL);
    pwobj->part_of = obj;
    obj_set_class_name(pwobj, _xmPanedWindow);

    /* Create WorkArea SubObj */
    workobj = obj_create(AB_TYPE_CONTAINER, NULL);
    workobj->part_of = obj;
    obj_append_child(pwobj, workobj);
    obj_set_class_name(workobj, _xmForm);

    /* Look to see if it has an Button Panel child */
    button_panel = objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL);
    if (button_panel != NULL)
	obj_reparent(button_panel, pwobj);

    /* Look to see if it has a footer child */
    footer = objxm_comp_custdlg_get_area(obj, AB_CONT_FOOTER);
    if (footer != NULL)
	obj_reparent(footer, pwobj);

    /* Move any remaining children to be parented off WorkObj
     * BEFORE the ShellObj is parented off obj
     */
    obj_move_children(workobj, obj);
    obj_append_child(obj, bbobj);
    obj_append_child(bbobj, pwobj);

    name = obj_get_name(obj);

    obj_set_unique_name(bbobj,
	ab_ident_from_name_and_label(name, "shellform"));
    obj_set_unique_name(pwobj,
	ab_ident_from_name_and_label(name, "panedwin"));
    obj_set_unique_name(workobj,
        ab_ident_from_name_and_label(name, "form"));

    /* Make PanedWindow SubObj Fill entire Dialog-Form */
    obj_set_attachment(pwobj, AB_CP_WEST, AB_ATTACH_GRIDLINE, (void*)0, 0);
    obj_set_attachment(pwobj, AB_CP_EAST, AB_ATTACH_GRIDLINE, (void*)100, 0);
    obj_set_attachment(pwobj, AB_CP_NORTH, AB_ATTACH_GRIDLINE, (void*)0, 0);
    obj_set_attachment(pwobj, AB_CP_SOUTH, AB_ATTACH_GRIDLINE, (void*)100, 0);

    xmconfig_window_select_area(obj, pwobj);

    return OK;

}

static int
custdlg_unconfig(
    ABObj	obj
)
{
    ABObj 	bbobj, workobj;
    ABObj	button_panel, footer;

    bbobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_BB_OBJ);
    workobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    button_panel = objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL);
    footer  = objxm_comp_custdlg_get_area(obj, AB_CONT_FOOTER);

    if (button_panel != NULL)
        obj_reparent(button_panel, obj);

    if (footer != NULL)
        obj_reparent(footer, obj);

    /* Move all other Children to RootObj */
    obj_move_children(obj, workobj);

    obj_destroy(bbobj);

    return OK;

}

static ABObj
custdlg_get_config_obj(
    ABObj	obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    ABObj workobj = NULL;
    ABObj modobj = NULL;
    ABObj selobj  = NULL;
    ABObj bbobj, pwobj, msgobj, child;
    int   i, num_children;

    bbobj = obj_get_child(obj, 0);
    pwobj = obj_get_child(bbobj, 0);

    switch(type)
    {
	case AB_CFG_WINDOW_BB_OBJ:
	case AB_CFG_HELP_OBJ:
	    cfg_obj = bbobj;
	    break;
	case AB_CFG_WINDOW_PW_OBJ:
	case AB_CFG_MENU_PARENT_OBJ:
	    cfg_obj = pwobj;
	    break;
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_LABEL_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_PARENT_OBJ:
	case AB_CFG_STATUS_OBJ:
	case AB_CFG_SELECT_OBJ:
    	    num_children = obj_get_num_children(pwobj);
    	    /* Search through immediate children of the Shell-container
     	     * and find the WorkArea & Status SubObjs if they exist
     	     */
    	    for (i = 0; i < num_children; i++)
    	    {
        	child = obj_get_child(pwobj, i);
        	if (obj_is_control_panel(child) && child->part_of == obj)
            	    workobj = child;
		else if (obj_is_label(child) && child->part_of == obj)
		{
		    msgobj = child;
		    modobj = obj_get_child(msgobj, 0);
		    selobj = obj_get_child(msgobj, 1);
		}
    	    }
	    if (type == AB_CFG_SELECT_OBJ)
		cfg_obj = selobj;
	    else if (type == AB_CFG_STATUS_OBJ)
		cfg_obj = modobj;
	    else  /* PARENT or SIZE */
		cfg_obj = (workobj? workobj : pwobj);

	    break;

        case AB_CFG_BORDER_OBJ:
	case AB_CFG_MENU_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
	    cfg_obj = NULL;
	    break;

	default:
	    util_dprintf(1, "custdlg_get_config_obj: unknown type\n");
	    cfg_obj = NULL;
    }
    return(cfg_obj);
}

static int
custdlg_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    STRING      label;
    ABObj       shobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj	bbobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_BB_OBJ);
    ABObj	pwobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_PW_OBJ);
    ABObj       workobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);
    BOOL        cgen_args;

    cgen_args = (ctype == OBJXM_CONFIG_CODEGEN? TRUE : FALSE);

    /* Shell Resources */
    if ((label = obj_get_label(obj)) != NULL)
        objxm_obj_set_ui_arg(shobj, AB_ARG_STRING, XmNtitle, label);
    objxm_obj_set_ui_arg(shobj, AB_ARG_BOOLEAN, XmNallowShellResize, TRUE);

    /* BulletinBoard-Dialog Resources */
    objxm_obj_set_ui_arg(bbobj, AB_ARG_INT, XmNmarginWidth, 0);
    objxm_obj_set_ui_arg(bbobj, AB_ARG_INT, XmNmarginHeight, 0);
    objxm_obj_set_ui_arg(bbobj, AB_ARG_BOOLEAN, XmNdefaultPosition, False);

    /* Since all windows should be resizable in Build mode, ONLY set
     * this resource (to remove resize decorations from the dialog) if
     * we are doing Code-generation.
     */
    if (cgen_args && !obj_get_resizable(obj))
	objxm_obj_set_ui_arg(bbobj, AB_ARG_BOOLEAN, XmNnoResize, True);

    /* PanedWindow Resources */
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNmarginWidth, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNmarginHeight, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNspacing, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNsashHeight, 1);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNsashWidth, 1);

    /* Form WorkArea resources */
    objxm_obj_set_literal_ui_arg(workobj, ctype,  XmNresizePolicy,
        obj_get_width(obj) == -1? XmRESIZE_ANY : XmRESIZE_GROW);
    objxm_obj_set_ui_arg(workobj, AB_ARG_INT,     XmNmarginWidth,      0);
    objxm_obj_set_ui_arg(workobj, AB_ARG_INT,     XmNmarginHeight,     0);
    objxm_obj_set_ui_arg(workobj, AB_ARG_BOOLEAN, XmNallowResize,	True);

    /* Set the interface name and select-state on the footer */
    if (ctype == OBJXM_CONFIG_BUILD)
	set_window_select_area_args(obj);

    return 0;
}

static int
dialog_post_inst(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{

    if (ctype == OBJXM_CONFIG_BUILD)
	post_inst_window_select_area(obj);

    if (obj_is_popup_win(obj))
	objxm_comp_set_default_button_args(obj, ctype);

    return 0;
}


/*
 **********************************************
 ** Simple Methods (Button, Label, Item, ...)**
 **********************************************
 */
int
simple_xmconfig(
    ABObj    obj
)
{
    /* Configure Menu-reference if a menu exists for obj */
    objxm_comp_config_menu_ref(obj);

    obj_set_class_name(obj, objxm_obj_get_default_motif_class(obj));

    return OK;
}

static int
simple_unconfig(
    ABObj	obj
)
{
    ABObj	mr_obj = NULL;

    /* Delete Menu-reference */
    objxm_comp_unconfig_menu_ref(obj);

    return OK;
}

static ABObj
simple_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;

    switch(type)
    {
        case AB_CFG_OBJECT_OBJ:
	case AB_CFG_DND_OBJ:
	case AB_CFG_HELP_OBJ:
        case AB_CFG_LABEL_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_SELECT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = obj;
            break;

	case AB_CFG_MENU_OBJ:
	    /* Menu-Refs are the only possible children */
	    cfg_obj = (obj_get_num_children(obj) > 0)? obj_get_child(obj, 0) : NULL;
	    break;
        case AB_CFG_PARENT_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_BORDER_OBJ:
            break;
        default:
	    util_dprintf(1, "simple_get_config_obj: unsupported config-type\n");
    }

    return(cfg_obj);
}

static int
simple_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	lbl_obj;

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_align_args(obj, ctype);

    lbl_obj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
    objxm_obj_set_ui_arg(lbl_obj, AB_ARG_BOOLEAN, XmNrecomputeSize,
	obj_get_width(obj) == -1? True : False);

    return 0;

}

/*
 *************************
 ** Choice Methods      **
 *************************
 */

static int
choice_xmconfig(
    ABObj    obj
)
{
    AB_CHOICE_TYPE type;
    ABObj       ch_obj, p_obj;
    ABObj	lbl_obj = NULL;
    int         num_items;

    type = obj->info.choice.type;
    num_items = obj_get_num_children(obj);

    obj->part_of = obj;
    obj_set_class_name(obj, _xmRowColumn);

    if (obj_has_label(obj))
    {
    	/* Create Label SubObj */
    	lbl_obj = obj_create(AB_TYPE_LABEL, NULL);
    	lbl_obj->part_of = obj;
    	obj_set_class_name(lbl_obj, _xmLabel);

    	/* Create Choice SubObj */
    	ch_obj = obj_create(AB_TYPE_CHOICE, NULL);
    	ch_obj->part_of = obj;
    	obj_set_class_name(ch_obj, _xmRowColumn);
    }
    else /* No Label */
	ch_obj = obj;

    /* If OptionMenu, must build menu hierarchy for items */
    if (type == AB_CHOICE_OPTION_MENU)
    {
        /* Create Option PulldownMenu SubObj */
	/* NOTE: Menu must be FIRST child */
        p_obj = obj_create(AB_TYPE_CHOICE, NULL);
        p_obj->part_of = obj;
        obj_set_class_name(p_obj, _xmPulldownMenu);
    }
    else /* AB_CHOICE_EXCLUSIVE || AB_CHOICE_NONEXCLUSIVE */
        p_obj = ch_obj;

    if (p_obj != obj)
    	obj_move_children(p_obj, obj);

    if (type == AB_CHOICE_OPTION_MENU)
    {
        obj_insert_child(obj, p_obj, 0);
	obj_set_unique_name(p_obj,
	    ab_ident_from_name_and_label(obj_get_name(obj), "menu"));
    }

    if (lbl_obj) /* Has Label */
    {
    	obj_append_child(obj, lbl_obj);
	obj_set_unique_name(lbl_obj,
            ab_ident_from_name_and_label(obj_get_name(obj), "label"));

    	obj_append_child(obj, ch_obj);
        obj_set_unique_name(ch_obj,
            ab_ident_from_name_and_label(obj_get_name(obj), "choice"));
    }
    return OK;

}
static int
choice_unconfig(
    ABObj	obj
)
{
    ABObj	lbl_obj, ch_obj;
    ABObj	m_obj = NULL;

    if (obj_get_subtype(obj) == AB_CHOICE_OPTION_MENU)
	m_obj = obj_get_child(obj, 0);

    if (obj_has_label(obj)) /* Has Label */
    {
	lbl_obj = obj_get_child(obj, m_obj? 1 : 0);
	ch_obj  = obj_get_child(obj, m_obj? 2 : 1);

	obj_move_children(obj, m_obj? m_obj : ch_obj);

	obj_destroy(lbl_obj);
	obj_destroy(ch_obj);
    }
    else if (m_obj) /* No Label */
	obj_move_children(obj, m_obj);

    if (m_obj)
	obj_destroy(m_obj);

    return OK;

}

static ABObj
choice_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    AB_CHOICE_TYPE choice_type;
    BOOL	   has_label = TRUE;

    choice_type = obj->info.choice.type;
    if (!obj_has_label(obj))
	has_label = FALSE;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
	    cfg_obj = obj;
	break;

        case AB_CFG_OBJECT_OBJ:
	case AB_CFG_SIZE_OBJ:
	    if (has_label)
            	cfg_obj = obj_get_child(obj, choice_type == AB_CHOICE_OPTION_MENU? 2: 1);
	    else
		cfg_obj = obj;
            break;
        case AB_CFG_PARENT_OBJ:
            if (choice_type == AB_CHOICE_OPTION_MENU)
                cfg_obj = obj_get_child(obj, 0);
            else
                cfg_obj = has_label? obj_get_child(obj, 1) : obj;
            break;
        case AB_CFG_LABEL_OBJ:
	    if (has_label)
            	cfg_obj = obj_get_child(obj, choice_type == AB_CHOICE_OPTION_MENU? 1: 0);
	    else
		cfg_obj = NULL;
            break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SELECT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
            cfg_obj = NULL;
            break;
        default:
	    util_dprintf(1, "choice_get_config_obj: unsupported config-type\n");
    }

    return(cfg_obj);

}

static int
choice_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_CHOICE_TYPE type;
    AB_ORIENTATION orient;
    ABObj	   ch_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj	   xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
    ABObj	   lbl_obj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);
    if (lbl_obj) /* Clear margins on RowColumn */
    {
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNmarginHeight, 0);
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNmarginWidth, 0);
    }

    objxm_obj_set_ui_arg(xy_obj, AB_ARG_BOOLEAN, XmNisHomogeneous, FALSE);
    objxm_obj_set_literal_ui_arg(xy_obj, ctype, XmNpacking, XmPACK_TIGHT);

    if ((type = (AB_CHOICE_TYPE)obj_get_subtype(obj)) != AB_CHOICE_OPTION_MENU)
    {
        orient = obj_get_orientation(obj);

        switch (orient)
        {
            case AB_ORIENT_HORIZONTAL:
    		objxm_obj_set_literal_ui_arg(ch_obj, ctype, XmNorientation, XmHORIZONTAL);
        	if (obj_get_num_columns(obj) > 1)
        	{
            	    objxm_obj_set_literal_ui_arg(xy_obj, ctype, XmNentryVerticalAlignment,
                        XmALIGNMENT_BASELINE_BOTTOM);
		    if (lbl_obj)
            	    	objxm_obj_set_ui_arg(lbl_obj, AB_ARG_INT, XmNmarginHeight, 8);
        	}
        	break;
            case AB_ORIENT_VERTICAL:
        	objxm_obj_set_literal_ui_arg(ch_obj, ctype, XmNorientation,XmVERTICAL);
    		objxm_obj_set_literal_ui_arg(xy_obj, ctype, XmNentryVerticalAlignment,
            		XmALIGNMENT_BASELINE_BOTTOM);
		if (lbl_obj)
    		    objxm_obj_set_ui_arg(lbl_obj, AB_ARG_INT, XmNmarginHeight, 8);
        	break;
        }
	if (obj_get_num_columns(obj) > 1)
	    objxm_obj_set_literal_ui_arg(ch_obj, ctype, XmNpacking, XmPACK_COLUMN);
	else
	    objxm_obj_set_literal_ui_arg(ch_obj, ctype, XmNpacking, XmPACK_TIGHT);
    	objxm_obj_set_literal_ui_arg(ch_obj, ctype, XmNrowColumnType, XmWORK_AREA);
	objxm_obj_set_ui_arg(ch_obj, AB_ARG_INT,     XmNspacing,	0);
        objxm_obj_set_ui_arg(ch_obj, AB_ARG_BOOLEAN, XmNisHomogeneous, TRUE);
    	objxm_obj_set_ui_arg(ch_obj, AB_ARG_INT,     XmNnumColumns, obj_get_num_columns(obj));
    	objxm_obj_set_ui_arg(ch_obj, AB_ARG_INT,     XmNentryClass, xmToggleButtonWidgetClass);
    	objxm_obj_set_ui_arg(ch_obj, AB_ARG_BOOLEAN, XmNradioBehavior,
		type == AB_CHOICE_EXCLUSIVE? TRUE : FALSE);
    }
    else /* AB_CHOICE_OPTION_MENU */
    {
        ABObj	p_obj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

	objxm_obj_set_ui_arg(ch_obj, AB_ARG_INT, XmNspacing,	0);
    	objxm_obj_set_literal_ui_arg(ch_obj, ctype, XmNrowColumnType, XmMENU_OPTION);
	objxm_obj_set_literal_ui_arg(p_obj, ctype, XmNrowColumnType, XmMENU_PULLDOWN);
    }
    return 0;

}

static int
choice_post_inst(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    /* REMIND: aim - does this do any good?? */
    if (ctype == OBJXM_CONFIG_BUILD &&
        obj_get_subtype(obj) == AB_CHOICE_OPTION_MENU)
    {
        Widget widget;
	ABObj  chobj;
	Position x;

	chobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	widget = objxm_get_widget(chobj);

	XtVaGetValues(widget, XmNx, &x, NULL);
	XtVaSetValues(widget, XmNx, x-6, NULL);
    }
    return 0;

}

/*
 *******************************
 ** ControlPanel Methods      **
 *******************************
 */

static int
cpanel_xmconfig(
    ABObj    obj
)
{
    ABObj       panel_obj;

    /* Configure for border-frame */
    if ((panel_obj = objxm_comp_config_border_frame(obj)) == NULL)
	panel_obj = obj;

    /* Configure for Menu */
    objxm_comp_config_menu_ref(obj);

    obj_set_class_name(panel_obj, _xmForm);

    return OK;

}

static int
cpanel_unconfig(
    ABObj       obj
)
{
    ABObj       mr_obj = NULL;

    /* Delete Menu-reference first */
    objxm_comp_unconfig_menu_ref(obj);

    /* Remove Border Rootobj (collapse) */
    objxm_comp_unconfig_border_frame(obj);

    return OK;
}

static int
cpanel_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_CONTAINER_TYPE	cont_type;
    ABObj		parent;
    ABObj		cp_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj		xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
    int			margin = 0;
    unsigned char	resize_policy = XmRESIZE_GROW;

    objxm_comp_set_border_args(obj, ctype);

    cont_type = obj_get_container_type(obj);

    switch(cont_type)
    {
	case AB_CONT_BUTTON_PANEL:
	    margin = 7;
/*
	    objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpositionIndex, 1);
*/
	    break;
	case AB_CONT_FOOTER:
	    parent = obj_get_root(obj_get_parent(obj));
	    if (obj_is_popup_win(parent))
	    {
		if (objxm_comp_custdlg_get_area(parent, AB_CONT_BUTTON_PANEL) != NULL)
		    objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpositionIndex, 2);
		else
		    objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpositionIndex, 1);
	    }
	    break;
	default:
	    break;
    }

    objxm_obj_set_ui_arg(cp_obj, AB_ARG_INT, XmNmarginWidth,      margin);
    objxm_obj_set_ui_arg(cp_obj, AB_ARG_INT, XmNmarginHeight,     margin);
/*
    objxm_obj_set_literal_ui_arg(cp_obj, ctype, XmNresizePolicy, XmRESIZE_GROW);
*/
    /* For standard ControlPane's with Resize Policy of "Fit Contents", we want
     * to use XmRESIZE_ANY, all other cases we want to use XmRESIZE_GROW to ensure
     * they don't snap to a zero size if they have no children (because there is
     * no way to get it back!).  Only standard ControlPanes can have a height ==
     * -1.
     */
    objxm_obj_set_literal_ui_arg(cp_obj, ctype,  XmNresizePolicy,
        obj_get_height(obj) == -1? XmRESIZE_ANY : XmRESIZE_GROW);

    /* Need to set additional resources if child of panedWindow */
    parent = obj_get_parent(obj);
    if ((obj_get_type(parent) == AB_TYPE_CONTAINER) &&
	(obj_get_container_type(parent) == AB_CONT_PANED))
    {
	/* Set PanedWindow Contraint Resources */
	objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMinimum,
				obj_get_pane_min(obj));
	objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMaximum,
				obj_get_pane_max(obj));
    }
    return 0;
}

static int
cpanel_post_inst(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_CONTAINER_TYPE	cont_type;
    ABObj		mwobj;
    int			height;

    cont_type = obj_get_container_type(obj);

    switch(cont_type)
    {
        case AB_CONT_FOOTER:
        case AB_CONT_BUTTON_PANEL:
	    if (obj_is_base_win(obj_get_root(obj_get_parent(obj))))
	    {
		mwobj = obj_get_parent(obj);

		if (ctype == OBJXM_CONFIG_BUILD)
		{
                    /* Area must be Managed before MainWindow resource gets set */
		    XtManageChild(objxm_get_widget(obj));
		    util_dprintf(3, "MANAGING %s\n", obj_get_name(obj));
		    XtVaSetValues(objxm_get_widget(mwobj),
			XmNmessageWindow,	objxm_get_widget(obj),
			NULL);
		}
		else
		{
                    objxm_obj_set_widget_ui_arg(mwobj, ctype, XmNmessageWindow, obj);
                    obj_set_flag(mwobj, AttrChangedFlag);
		}
	    }
	    else
	    {
		height = obj_get_height(obj);

		/* If a height is specified for the pane, use that value, else
		 * pass in the object responsible for the size of the pane and
		 * let the routine query its widget's size.
		 */
		make_pane_fixed_height(obj, height == -1?
			objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ) : NULL,
			height, 0);
	    }
 	    break;
	case AB_CONT_TOOL_BAR:
	    mwobj = obj_get_parent(obj);

    	    /* Attach Toolbar to MainWindow SubObj */

            if (ctype == OBJXM_CONFIG_BUILD)
	    {
	        /* Area must be Managed before MainWindow resource gets set */
                XtManageChild(objxm_get_widget(obj));
                util_dprintf(3, "MANAGING %s\n", obj_get_name(obj));
		XtVaSetValues(objxm_get_widget(mwobj),
			XmNcommandWindow,	objxm_get_widget(obj),
			XmNcommandWindowLocation, XmCOMMAND_ABOVE_WORKSPACE,
			NULL);
	    }
	    else
	    {
                objxm_obj_set_widget_ui_arg(mwobj, ctype, XmNcommandWindow, obj);
                objxm_obj_set_literal_ui_arg(mwobj, ctype, XmNcommandWindowLocation,
                        XmCOMMAND_ABOVE_WORKSPACE);
                obj_set_flag(mwobj, AttrChangedFlag);
	    }
	    break;
	default:
	    break;
    }
    return 0;
}

/*
 ****************************************************
 ** Container Methods (ControlPanel & MenuBar)     **
 ****************************************************
 */
static ABObj
container_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    ABObj mr_obj  = NULL;
    ABObj b_obj = NULL;

    mr_obj = get_menu_ref(obj);

    if (obj_has_border_frame(obj))
	b_obj = obj;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
	    cfg_obj = obj;
	break;

        case AB_CFG_POSITION_OBJ:
	    if (obj_is_layers(obj_get_root(obj_get_parent(obj))))
		cfg_obj = obj_get_root(obj_get_parent(obj));
	    else
	        cfg_obj = obj;
	    break;
	case AB_CFG_DND_OBJ:
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_LABEL_OBJ:
        case AB_CFG_SELECT_OBJ:
	case AB_CFG_SIZE_OBJ:
        case AB_CFG_PARENT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = (b_obj? obj_get_child(obj, mr_obj? 1 : 0) : obj);
            break;
	case AB_CFG_BORDER_OBJ:
	    cfg_obj = b_obj;
	    break;
	case AB_CFG_MENU_OBJ:
	    cfg_obj = mr_obj;
	    break;
	case AB_CFG_SCROLLBAR_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
            cfg_obj = NULL;
            break;
        default:
	    util_dprintf(1, "container_get_config_obj: unknown type\n");
            cfg_obj = NULL;
    }

    return(cfg_obj);

}
/*
 *******************************
 * Glyph Methods	       *
 *******************************
 */
static int
layer_xmconfig(
    ABObj	obj
)
{
    ABObj	pane;
    int		i;

    obj_set_class_name(obj, _xmForm);

    /* Make the first layered-pane the one initially
     * visible for editing
     */
    for(i=0; i < obj_get_num_salient_children(obj); i++)
    {
	pane = obj_get_salient_child(obj, i);
	if (i == 0)
	    obj_set_flag(pane, VisibleFlag);
	else
	    obj_clear_flag(pane, VisibleFlag);
    }
    return OK;
}

static int
layer_unconfig(
    ABObj	obj
)
{
    return OK;
}

static ABObj
layer_get_config_obj(
    ABObj	obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj	cfg_obj;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_PARENT_OBJ:
            cfg_obj = obj;
            break;

	case AB_CFG_DND_OBJ:
        case AB_CFG_LABEL_OBJ:
        case AB_CFG_SELECT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
        case AB_CFG_BORDER_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_SCROLLBAR_OBJ:
        case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
            cfg_obj = NULL;
            break;

        default:
            util_dprintf(1, "layer_get_config_obj: unknown type\n");
            cfg_obj = NULL;
    }
    return cfg_obj;
}

static int
layer_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{

    objxm_obj_set_literal_ui_arg(obj, ctype, XmNresizePolicy, XmRESIZE_ANY);
    return OK;

}

/*
 *******************************
 ** Group Methods      **
 *******************************
 */

static int
group_xmconfig(
    ABObj    obj
)
{
    ABObj       panel_obj;

    /* Configure for border-frame */
    if ((panel_obj = objxm_comp_config_border_frame(obj)) == NULL)
	panel_obj = obj;

    obj_set_class_name(panel_obj, _xmForm);

    return OK;

}

/* Group Unconfig */

static int
group_unconfig(
    ABObj       obj
)
{
    ABObj       mr_obj = NULL;

    /* Delete Menu-reference first */
    objxm_comp_unconfig_menu_ref(obj);

    /* Remove Border Rootobj (collapse) */
    objxm_comp_unconfig_border_frame(obj);

    return OK;
}


static int
group_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	g_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    objxm_comp_set_border_args(obj, ctype);

    objxm_obj_set_ui_arg(g_obj, AB_ARG_INT, 	XmNmarginWidth,	     0);
    objxm_obj_set_ui_arg(g_obj, AB_ARG_INT, 	XmNmarginHeight,     0);
    objxm_obj_set_literal_ui_arg(g_obj, ctype, 	XmNresizePolicy,     XmRESIZE_ANY);

    /* REMIND: aim,11/16/93 -do we need to support this row-column stuff? */
    if (util_streq(obj_get_class_name(g_obj), _xmRowColumn))
    {
        switch (obj->info.container.packing)
        {
            case AB_PACK_NONE:
        	objxm_obj_set_literal_ui_arg(g_obj, ctype, XmNpacking, XmPACK_NONE);
            	break;
            case AB_PACK_TIGHT:
            	objxm_obj_set_literal_ui_arg(g_obj, ctype, XmNpacking, XmPACK_TIGHT);
            	break;
            case AB_PACK_EQUAL:
            	objxm_obj_set_literal_ui_arg(g_obj, ctype, XmNpacking, XmPACK_COLUMN);
            	break;
        }
        switch (obj->info.container.group_type)
        {
            case AB_GROUP_COLUMNS:
            	objxm_obj_set_literal_ui_arg(g_obj, ctype, XmNorientation, XmVERTICAL);
            	break;
            case AB_GROUP_ROWS:
            	objxm_obj_set_literal_ui_arg(g_obj, ctype, XmNorientation, XmHORIZONTAL);
            	break;
        }
    }
    return 0;
}

/*
 *******************************
 ** Item  Methods             **
 *******************************
 */

static int
item_xmconfig(
    ABObj    obj
)
{
    ABObj       parentobj = obj_get_root(obj_get_parent(obj));

    switch(obj_get_subtype(obj))
    {
	case AB_ITEM_FOR_CHOICE:
            if (obj_get_subtype(parentobj) == AB_CHOICE_OPTION_MENU)
            	obj_set_class_name(obj, _xmPushButton);
            else
            	obj_set_class_name(obj, _xmToggleButton);
	    break;
	case AB_ITEM_FOR_MENU:
	    obj_set_class_name(obj,
		obj_has_menu(obj)? _xmCascadeButton : _xmPushButton);
	    break;
	case AB_ITEM_FOR_MENUBAR:
            obj_set_class_name(obj, _xmCascadeButton);
	    break;
        case AB_ITEM_FOR_LIST:
            break;
     }

    objxm_comp_config_menu_ref(obj);

    return OK;

}

/* Item Unconfig: simple_unconfig */

static ABObj
item_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_LABEL_OBJ:
	    cfg_obj = obj;
	    break;
        case AB_CFG_MENU_PARENT_OBJ:
	    cfg_obj = obj_get_parent(obj);
	    break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = objxm_comp_get_subobj(obj_get_parent(obj), type);
            break;
	case AB_CFG_MENU_OBJ:
	    /* Menu-refs are the only possible children */
	    cfg_obj = (obj_get_num_children(obj) > 0)? obj_get_child(obj, 0) : NULL;
	    break;
        case AB_CFG_BORDER_OBJ:
	case AB_CFG_DND_OBJ:
        case AB_CFG_PARENT_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
            cfg_obj = NULL;
            break;
        default:
	    util_dprintf(1, "item_get_config_obj: unknown config-type\n");
            cfg_obj = NULL;
    }
    return(cfg_obj);
}

static int
item_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	i_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    STRING	mnemonic, accel, acceltext;

    objxm_comp_set_label_args(obj, ctype);

    if (obj_is_choice_item(obj))
    {
        ABObj parentobj = obj_get_parent(obj);
        ABObj vobj = obj_get_root(parentobj);

        if (vobj->info.choice.type == AB_CHOICE_EXCLUSIVE)
            objxm_obj_set_literal_ui_arg(i_obj, ctype, XmNindicatorType, XmONE_OF_MANY);
        else if (vobj->info.choice.type == AB_CHOICE_NONEXCLUSIVE)
            objxm_obj_set_literal_ui_arg(i_obj, ctype, XmNindicatorType, XmN_OF_MANY);

        if (vobj->info.choice.type != AB_CHOICE_OPTION_MENU)
            objxm_obj_set_ui_arg(i_obj, AB_ARG_BOOLEAN, XmNset,
		    obj_is_initially_selected(i_obj));
    }

    if ((mnemonic = obj_get_mnemonic(obj)) != NULL)
    {
	objxm_obj_set_ui_arg(i_obj, AB_ARG_MNEMONIC, XmNmnemonic,
		ctype == OBJXM_CONFIG_BUILD? (XtArgVal)XStringToKeysym(mnemonic) :
		(XtArgVal)istr_create(mnemonic));
    }
    if ((accel = obj_get_accelerator(obj)) != NULL)
    {
	acceltext = objxm_accel_to_acceltext(accel);

	objxm_obj_set_ui_arg(i_obj, AB_ARG_STRING, XmNaccelerator, accel);
	objxm_obj_set_ui_arg(i_obj, AB_ARG_XMSTRING, XmNacceleratorText,
		ctype == OBJXM_CONFIG_BUILD? (XtArgVal)XmStringCreateLocalized(acceltext) :
		(XtArgVal)istr_create(acceltext));
    }

    return 0;
}

static int
item_post_inst(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    /* Ensure the item's select-state gets set correctly */
    if (ctype == OBJXM_CONFIG_BUILD)
    	objxm_comp_set_item_selection(obj);

    return 0;
}

/*
 **********************************************
 ** Pane  Methods (TextPane & GraphicPane)   **
 **********************************************
 */
static int
pane_xmconfig(
    ABObj    obj
)
{
    ABObj	root_obj;
    ABObj	pane_obj;

    /* Configure for border-frame and set root_obj
     * to Obj directly under Frame
     */
    if ((root_obj = objxm_comp_config_border_frame(obj)) == NULL)
        root_obj = obj;

    if ( obj_has_hscrollbar(obj) || obj_has_vscrollbar(obj) )
    {
        /* Configure ScrolledWindow */
        root_obj->part_of = obj;
        obj_set_class_name(root_obj, _xmScrolledWindow);

	/* Pane SubObj is child of ScrolledWindow */
	pane_obj = objxmP_create_direct_subobj(root_obj, obj->type, "pane");
	pane_obj->part_of = obj;

        /* If TermPane, we must create the Vertical scrollbar as a
         * child of the ScrolledWindow and hook it up later to the
         * Term widget
         */
        if (obj_is_term_pane(obj))
        {
	    ABObj	sbobj;

            sbobj = obj_create(AB_TYPE_TERM_PANE, NULL);
            sbobj->part_of = obj;
            obj_set_class_name(sbobj, _xmScrollBar);
	    obj_append_child(root_obj, sbobj);
	    obj_set_unique_name(sbobj,
		ab_ident_from_name_and_label(obj_get_name(obj), "vscrollbar"));
        }
    }
    else /* No Scrolling */
        pane_obj = root_obj;

    if (obj_is_text_pane(obj))
	obj_set_class_name(pane_obj, _xmText);
    else if (obj_is_term_pane(obj))
	obj_set_class_name(pane_obj, _dtTerm);
    else
        obj_set_class_name(pane_obj, _xmDrawingArea);

    /* Configure Menu-ref if a menu exists */
    objxm_comp_config_menu_ref(obj);

    return OK;

}

static int
pane_unconfig(
    ABObj	obj
)
{
    ABObj	pane_obj;

    /* Destroy Menu-reference if it exists */
    objxm_comp_unconfig_menu_ref(obj);

    /* Collapse Frame if it exists */
    objxm_comp_unconfig_border_frame(obj);

    if (obj_has_vscrollbar(obj) || obj_has_hscrollbar(obj) )
    {
        pane_obj = obj_get_child(obj, 0);

	if (obj_get_num_children(pane_obj) > 0)
	    obj_move_children(obj, pane_obj);

	if (obj_is_term_pane(obj)) /* Destroy Scrollbar SubObj */
	{
	    ABObj	doomedPane = obj_get_child(obj, 1);
	    obj_destroy(doomedPane);
	}

	obj_destroy(pane_obj);
    }

    return OK;

}

static ABObj
pane_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj   = NULL;
    ABObj pane_obj  = obj;  /* Pane SubObj */
    ABObj mr_obj    = NULL; /* Menu-ref SubObj */
    ABObj sb_obj    = NULL; /* Scrollbar SubObj */
    ABObj b_obj	    = NULL; /* Border SubObj */

    mr_obj = get_menu_ref(obj);

    if (obj_has_border_frame(obj))
        b_obj = obj;

    if (obj_has_scrollbar(obj))
    {
	if (b_obj) /* has border frame */
	    sb_obj = obj_get_child(b_obj, (mr_obj? 1 : 0));
	else
	    sb_obj = obj;
    }

    if (b_obj) /* has border frame */
    {
	if (sb_obj) /* has scrolling */
	    pane_obj = obj_get_child(sb_obj, 0);
	else
	    pane_obj = obj_get_child(b_obj, (mr_obj? 1 : 0));
    }
    else if (sb_obj) /* has no border frame, but has scrolling */
    {
	pane_obj = obj_get_child(sb_obj, (mr_obj? 1 : 0));
    }
    else /* has no border frame and no scrolling */
    {
	pane_obj = obj;
    }

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
	    cfg_obj = obj;
	break;

        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = pane_obj;
            break;
        case AB_CFG_POSITION_OBJ:
            if (obj_is_layers(obj_get_root(obj_get_parent(obj))))
                cfg_obj = obj_get_root(obj_get_parent(obj));
            else
                cfg_obj = obj;
            break;
	case AB_CFG_BORDER_OBJ:
            cfg_obj = b_obj;
            break;
	case AB_CFG_SIZE_OBJ:
	    cfg_obj = sb_obj? sb_obj : obj;
	    break;
	case AB_CFG_DND_OBJ:
	case AB_CFG_PARENT_OBJ:
	    if (obj_is_text_pane(obj) ||
		obj_is_term_pane(obj))
		cfg_obj = NULL;
	    else
		cfg_obj = pane_obj;
	    break;
	case AB_CFG_SCROLLBAR_OBJ:
	    if (obj_is_term_pane(obj) && sb_obj)
		cfg_obj = obj_get_child(sb_obj, b_obj? 1:(mr_obj? 2:1));
	    break;
	case AB_CFG_MENU_OBJ:
	    cfg_obj = mr_obj;
	    break;
	case AB_CFG_SCROLLING_OBJ:
	    cfg_obj = sb_obj;
	    break;
        case AB_CFG_LABEL_OBJ:
        case AB_CFG_STATUS_OBJ:
            break;
        default:
	    util_dprintf(1, "pane_get_config_obj: unknown type\n");
    }

    return(cfg_obj);
}

static int
drawp_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	da_obj = NULL,  /* DrawArea SubObj */
		sw_obj = NULL,  /* ScrolledWin SubObj */
		xy_obj = NULL,	/* Position SubObj */
		parent = NULL;  /* ABObj parent NOT config parent */

    objxm_comp_set_border_args(obj, ctype);

    da_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);

    if (obj_has_vscrollbar(obj) || obj_has_hscrollbar(obj) )
    {
    	sw_obj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

	objxm_obj_set_literal_ui_arg(sw_obj, ctype, XmNscrollingPolicy, XmAUTOMATIC);
	objxm_obj_set_ui_arg(sw_obj, AB_ARG_INT, XmNspacing, 0);

        if (obj_get_vscrollbar_policy(obj) == AB_SCROLLBAR_ALWAYS ||
	    obj_get_hscrollbar_policy(obj) == AB_SCROLLBAR_ALWAYS )
            objxm_obj_set_literal_ui_arg(sw_obj, ctype, XmNscrollBarDisplayPolicy, XmSTATIC);
        else
            objxm_obj_set_literal_ui_arg(sw_obj, ctype, XmNscrollBarDisplayPolicy, XmAS_NEEDED);

        /* This sets the TOTAL width/height of the DrawArea (not necessarily
         * the 'visible' portion)
	 */
    	objxm_obj_set_ui_arg(da_obj, AB_ARG_INT, XmNwidth, obj_get_drawarea_width(obj));
    	objxm_obj_set_ui_arg(da_obj, AB_ARG_INT, XmNheight, obj_get_drawarea_height(obj));
    }

    /* Need to set additional resources if child of panedWindow */
    parent = obj_get_parent(obj);
    if ((obj_get_type(parent) == AB_TYPE_CONTAINER) &&
        (obj_get_container_type(parent) == AB_CONT_PANED))
    {
        /* Set PanedWindow Contraint Resources */
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMinimum,
                                obj_get_pane_min(obj));
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMaximum,
                                obj_get_pane_max(obj));
    }
    return 0;
}

static int
drawp_post_inst(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	gp_obj, sw_obj;

    if (ctype == OBJXM_CONFIG_BUILD &&
	(obj_has_hscrollbar(obj) || obj_has_vscrollbar(obj)))
    {
	gp_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	sw_obj = objxm_comp_get_subobj(obj, AB_CFG_SCROLLING_OBJ);

	XmScrolledWindowSetAreas(objxm_get_widget(sw_obj), NULL, NULL,
		objxm_get_widget(gp_obj));
    }
    return 0;
}

static int
termp_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	tp_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj       xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
    ABObj       scrolling_obj = objxm_comp_get_subobj(obj, AB_CFG_SCROLLING_OBJ);
    ABObj	parent;	/* ABObj parent NOT config parent */
    String      pstr;
    int		value;

    objxm_comp_set_border_args(obj, ctype);

    if ((value = obj_get_num_rows(obj)) > 0)
        objxm_obj_set_ui_arg(tp_obj, AB_ARG_INT, XmNrows, value);

    if ((value = obj_get_num_columns(obj)) > 0)
        objxm_obj_set_ui_arg(tp_obj, AB_ARG_INT, XmNcolumns, value);

    if ((pstr = obj_get_process_string(obj)) != NULL)
    	objxm_obj_set_ui_arg(tp_obj, AB_ARG_STRING, DtNsubprocessCmd, pstr);

    if (obj_has_vscrollbar(obj))
    {
  	ABObj   sb_obj = objxm_comp_get_subobj(obj, AB_CFG_SCROLLBAR_OBJ);

        objxm_obj_set_literal_ui_arg(scrolling_obj, ctype, XmNscrollingPolicy, XmAPPLICATION_DEFINED);
        objxm_obj_set_literal_ui_arg(scrolling_obj, ctype, XmNvisualPolicy,    XmVARIABLE);

	if (obj_get_vscrollbar_policy(obj) == AB_SCROLLBAR_ALWAYS)
            objxm_obj_set_literal_ui_arg(scrolling_obj, ctype,
			XmNscrollBarDisplayPolicy, XmSTATIC);
	else
	    objxm_obj_set_literal_ui_arg(scrolling_obj, ctype,
			XmNscrollBarDisplayPolicy, XmAS_NEEDED);

	objxm_obj_set_ui_arg(scrolling_obj, AB_ARG_INT, XmNspacing, 0);

	objxm_obj_set_literal_ui_arg(sb_obj, ctype, XmNorientation, XmVERTICAL);
    }

    /* Need to set additional resources if child of panedWindow */
    parent = obj_get_parent(obj);
    if ((obj_get_type(parent) == AB_TYPE_CONTAINER) &&
        (obj_get_container_type(parent) == AB_CONT_PANED))
    {
        /* Set PanedWindow Contraint Resources */
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMinimum,
                                obj_get_pane_min(obj));
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMaximum,
                                obj_get_pane_max(obj));
    }

    /* CMVC 3704 - Set the blink rate to 0 to eliminate distractions */
    objxm_obj_set_ui_arg(obj, AB_ARG_INT, XmNblinkRate, 0);
    return 0;
}

static int
termp_post_inst(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	paneObj = (ABObj) NULL,
		sbObj = (ABObj) NULL;

    /* If Scrollable TermPane, hook up vertical scrollbar to Term widget */
    if (obj_has_vscrollbar(obj))
    {
        sbObj = objxm_comp_get_subobj(obj_get_root(obj), AB_CFG_SCROLLBAR_OBJ);
        paneObj = objxm_comp_get_subobj(obj_get_root(obj), AB_CFG_OBJECT_OBJ);

        objxm_obj_set_widget_ui_arg(paneObj, ctype, DtNverticalScrollBar, sbObj);
	obj_set_flag(obj, AttrChangedFlag);
    }

    return 0;
}

static int
textp_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_SCROLLBAR_POLICY	vsb_policy, hsb_policy;
    ABObj       	tx_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj		sz_obj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
    ABObj		xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
    ABObj		sw_obj;
    ABObj		parent;	/* ABObj parent NOT config parent */
    STRING		strval;
    int			value;

    objxm_comp_set_read_only_args(obj, ctype);
    objxm_comp_set_border_args(obj, ctype);

    if ((value = obj_get_num_rows(obj)) != -1)
	objxm_obj_set_ui_arg(tx_obj, AB_ARG_INT, XmNrows, value);

    if ((value = obj_get_num_columns(obj)) != -1)
        objxm_obj_set_ui_arg(tx_obj, AB_ARG_INT, XmNcolumns, value);

    vsb_policy = obj_get_vscrollbar_policy(obj);
    hsb_policy = obj_get_hscrollbar_policy(obj);

    /* Check vertical scrollbar to determine if scrolling is enabled.
     * The presence of a Horizontal scrollbar is then determined based
     * on whether word-wrap is set or not.
     */
    if (obj_has_vscrollbar(obj))
    {
        objxm_obj_set_ui_arg(tx_obj, AB_ARG_BOOLEAN, XmNscrollVertical,
		vsb_policy != AB_SCROLLBAR_NEVER);
        objxm_obj_set_ui_arg(tx_obj, AB_ARG_BOOLEAN, XmNscrollHorizontal,
		hsb_policy != AB_SCROLLBAR_NEVER);

	sw_obj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

        objxm_obj_set_literal_ui_arg(sw_obj, ctype,
			XmNscrollingPolicy, XmAPPLICATION_DEFINED);
        objxm_obj_set_literal_ui_arg(sw_obj, ctype,
			XmNvisualPolicy,    XmVARIABLE);
	objxm_obj_set_literal_ui_arg(sw_obj, ctype,
			XmNscrollBarDisplayPolicy, XmSTATIC);

	objxm_obj_set_literal_ui_arg(sw_obj, ctype,
                                     XmNwidth, obj_get_textpane_height(sw_obj));
	objxm_obj_set_literal_ui_arg(sw_obj, ctype,
                                     XmNheight, obj_get_textpane_width(sw_obj));
    }

    objxm_obj_set_literal_ui_arg(tx_obj, ctype, XmNeditMode, XmMULTI_LINE_EDIT);
    objxm_obj_set_ui_arg(tx_obj, AB_ARG_BOOLEAN, XmNwordWrap, obj->info.text.word_wrap);

    if ((strval = obj_get_initial_value_string(obj)) != NULL)
	objxm_obj_set_ui_arg(tx_obj, AB_ARG_STRING, XmNvalue, strval);
    else /* strval == NULL, make sure the text pane is clean */
	if (ctype == OBJXM_CONFIG_BUILD)
	    objxm_obj_set_ui_arg(tx_obj, AB_ARG_STRING, XmNvalue, Util_empty_string);

    /* Need to set additional resources if child of panedWindow */
    parent = obj_get_parent(obj);
    if ((obj_get_type(parent) == AB_TYPE_CONTAINER) &&
        (obj_get_container_type(parent) == AB_CONT_PANED))
    {
        /* Set PanedWindow Contraint Resources */
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMinimum,
                                obj_get_pane_min(obj));
        objxm_obj_set_ui_arg(xy_obj, AB_ARG_INT, XmNpaneMaximum,
                                obj_get_pane_max(obj));
    }
    return 0;
}


/*
 ***********************
 ** List Methods      **
 ***********************
 */
static int
list_xmconfig(
    ABObj    obj
)
{
    ABObj       sw_obj, l_obj;
    ABObj	lbl_obj = NULL;

    obj->part_of = obj;

    if (obj_has_label(obj)) /* Has Label */
    {
    	/* Set Container RootObj class */
    	obj_set_class_name(obj, _xmRowColumn);

    	/* Create Label SubObj */
    	lbl_obj = obj_create(AB_TYPE_LABEL, NULL);
    	lbl_obj->part_of = obj;
    	obj_set_class_name(lbl_obj, _xmLabel);

	/* Create ScrolledWindow SubObj */
	sw_obj = obj_create(AB_TYPE_LIST, NULL);
	sw_obj->part_of = obj;
    }
    else /* No Label: Set Container RootObj class */
	sw_obj = obj;

    obj_set_class_name(sw_obj, _xmScrolledWindow);

    /* Create List SubObj */
    l_obj = obj_create(AB_TYPE_LIST, NULL);
    l_obj->part_of = obj;
    obj_set_class_name(l_obj, _xmList);

    /* Move Item children to the list SubObj */
    obj_move_children(l_obj, obj);

    /* Now that Items are moved, parent SubObjs from Root */
    if (lbl_obj != NULL)
    {
	obj_append_child(obj, lbl_obj);
	obj_set_unique_name(lbl_obj,
	    ab_ident_from_name_and_label(obj_get_name(obj), "label"));

	obj_append_child(obj, sw_obj);
	obj_set_unique_name(sw_obj,
	    ab_ident_from_name_and_label(obj_get_name(obj), "scrolledwin"));

    }
    obj_append_child(sw_obj, l_obj);
    obj_set_unique_name(l_obj,
	ab_ident_from_name_and_label(obj_get_name(obj), "list"));

    objxm_comp_config_menu_ref(obj);

    return OK;

}

static int
list_unconfig(
    ABObj	obj
)
{
    ABObj	lbl_obj = NULL;
    ABObj	sw_obj  = NULL;
    ABObj	l_obj;

    objxm_comp_unconfig_menu_ref(obj);

    if (obj_has_label(obj))
    {
	lbl_obj = obj_get_child(obj, 0);
	sw_obj  = obj_get_child(obj, 1);
	l_obj   = obj_get_child(sw_obj, 0);
    }
    else /* No Label */
	l_obj = obj_get_child(obj, 0);

    /* Move children from ListPane SubObj back to Root */
    obj_move_children(obj, l_obj);

    if (lbl_obj)
    {
	obj_destroy(lbl_obj);
	obj_destroy(sw_obj); /* destroys sw_obj & l_obj */
    }
    else
	obj_destroy(l_obj);

    return OK;

}

static ABObj
list_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    ABObj l_obj   = NULL;
    ABObj lbl_obj = NULL;
    ABObj sw_obj  = NULL;
    ABObj mr_obj  = NULL;
    BOOL  has_label = FALSE;

    if (obj_has_label(obj))
	has_label = TRUE;

    /* Check if 1st child is Menu-ref */
    mr_obj = get_menu_ref(obj);

    if (has_label)
    {
	lbl_obj = obj_get_child(obj, mr_obj? 1 : 0);
	sw_obj  = obj_get_child(obj, mr_obj? 2 : 1);
	l_obj   = obj_get_child(sw_obj, 0);
    }
    else
    {
	sw_obj = obj;
    	l_obj = obj_get_child(sw_obj, mr_obj? 1 : 0);
    }

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
	    cfg_obj = obj;
	break;

        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_PARENT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = l_obj;
            break;
        case AB_CFG_LABEL_OBJ:
	    cfg_obj = lbl_obj;
	    break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_MENU_OBJ:
	    cfg_obj = mr_obj;
	    break;
	case AB_CFG_SCROLLING_OBJ:
	case AB_CFG_SIZE_OBJ:
	    cfg_obj = sw_obj;
	    break;
	case AB_CFG_DND_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_BORDER_OBJ:
	    cfg_obj = NULL;
	    break;
        default:
	    util_dprintf(1, "list_get_config_obj: unknown type\n");
    }

    return(cfg_obj);
}
static int
list_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    int rows;
    ABObj	l_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj	sz_obj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);

    if ((rows = obj_get_num_rows(obj)) != -1)
    {
	objxm_obj_set_ui_arg(l_obj, AB_ARG_INT, XmNvisibleItemCount, rows);
        objxm_obj_remove_ui_arg(sz_obj, XmNheight);
    }

    objxm_obj_set_selection_arg(l_obj, ctype);

    objxm_obj_set_literal_ui_arg(l_obj, ctype, XmNlistSizePolicy,
		obj_get_width(obj) == -1? XmVARIABLE : XmCONSTANT);
    objxm_obj_set_literal_ui_arg(l_obj, ctype, XmNscrollBarDisplayPolicy,
		XmSTATIC);
/*
    if (obj->width >= 0 && l_obj != NULL)
        objxm_obj_set_ui_arg(l_obj, AB_ARG_INT, XmNwidth, (Dimension)obj->width);
*/
    return 0;
}
static int
list_post_inst(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	szObj;
    ABObj	subObj;

    /* If list width is fixed size, we must set it after
     * the encompassing ScrolledWindow has been instantiated/managed
     */
    if (obj_has_label(obj) && obj_get_width(obj) != -1)
    {
        szObj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

	if (ctype == OBJXM_CONFIG_BUILD)
	{
	    XtManageChild(objxm_get_widget(subObj));
	    XtManageChild(objxm_get_widget(szObj));
	}

	objxm_obj_set_ui_arg(szObj, AB_ARG_INT, XmNwidth,
		(Dimension)obj_get_width(obj));
	obj_set_flag(szObj, AttrChangedFlag);
    }
    return 0;
}


/*
 ***********************
 ** Menu Methods      **
 ***********************
 */
static int
menu_xmconfig(
    ABObj    obj
)
{
    return OK;
}

static int
menu_unconfig(
    ABObj	obj
)
{
    return OK;
}

static ABObj
menu_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_SELECT_OBJ:
	case AB_CFG_PARENT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
	case AB_CFG_LABEL_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
            cfg_obj = NULL;
	    break;
        default:
	    util_dprintf(1, "menu_get_config_obj: unknown type\n");
            cfg_obj = NULL;
    }
    return(cfg_obj);
}

static int
menu_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	pobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    objxm_obj_set_literal_ui_arg(pobj, ctype, XmNtearOffModel,
	obj_get_tearoff(obj)? XmTEAR_OFF_ENABLED : XmTEAR_OFF_DISABLED);

    return 0;
}


/*
 ***********************
 ** MenuBar Methods   **
 ***********************
 */
static int
menubar_xmconfig(
    ABObj    obj
)
{

    /* Set MenuShell RootObj class */
    obj_set_class_name(obj, _xmRowColumn);

    return OK;

}

/* Menubar Unconfig: simple_unconfig */

static int
menubar_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	mb_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    objxm_obj_set_literal_ui_arg(mb_obj, ctype, XmNrowColumnType,   XmMENU_BAR);

    return 0;
}

static int
menubar_post_inst(
    ABObj	obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj help_item;
    ABObj mwobj = obj_get_parent(obj);

    /* Attach Menubar to MainWindow SubObj */
    if (ctype == OBJXM_CONFIG_BUILD)
    {
	/* Area must be Managed before MainWindow resource is set */
	XtManageChild(objxm_get_widget(obj));
        util_dprintf(3,"MANAGING %s\n", obj_get_name(obj));
	XtVaSetValues(objxm_get_widget(mwobj),
		XmNmenuBar,	objxm_get_widget(obj),
		NULL);
    }
    else
    {
    	objxm_obj_set_widget_ui_arg(mwobj, ctype, XmNmenuBar, obj);
    	obj_set_flag(mwobj, AttrChangedFlag);
    }

    /* Find "Help" cascade button and set the resource on the menubar */
    if ((help_item = obj_get_menubar_help_item(obj)) != NULL)
    {
	objxm_obj_set_widget_ui_arg(obj, ctype, XmNmenuHelpWidget, help_item);
	obj_set_flag(obj, AttrChangedFlag);
    }
    return 0;
}


/*
 ***********************
 ** TextField Methods **
 ***********************
 */
static int
textf_xmconfig(
    ABObj    obj
)
{
    objxm_comp_config_labeled_obj(obj, AB_TYPE_TEXT_FIELD, "field", _xmTextField);

    return OK;
}

static int
textf_unconfig(
    ABObj	obj
)
{
    objxm_comp_unconfig_menu_ref(obj);
    objxm_comp_unconfig_labeled_obj(obj);

    return OK;
}

static ABObj
textf_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj;
    BOOL  has_label = TRUE;

    if (!obj_has_label(obj))
	has_label = FALSE;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
	    cfg_obj = obj;
	break;

        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
	case AB_CFG_SIZE_OBJ:
	    if (has_label)
                cfg_obj = obj_get_child(obj, 1);
	    else
		cfg_obj = obj;
            break;
        case AB_CFG_LABEL_OBJ:
	    if (has_label)
            	cfg_obj = obj_get_child(obj, 0);
	    else
		cfg_obj = NULL;
            break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
        case AB_CFG_PARENT_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
	case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
            cfg_obj = NULL;
            break;
        default:
	    util_dprintf(1, "textf_get_config_obj: unknown config-type\n");
            cfg_obj = NULL;
    }

    return(cfg_obj);
}

static int
textf_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj          tf_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj          sz_obj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
    int		   value;
    STRING	   valstr;

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);
    objxm_comp_set_read_only_args(obj, ctype);

    if ((value = obj_get_num_columns(obj)) > 0)
    {
	objxm_obj_set_ui_arg(tf_obj, AB_ARG_INT, XmNcolumns, value);
        objxm_obj_remove_ui_arg(sz_obj, XmNwidth);
    }
    if ((value = obj_get_max_length(obj)) > 0)
	objxm_obj_set_ui_arg(tf_obj, AB_ARG_INT, XmNmaxLength, value);

    if ((valstr = obj_get_initial_value_string(obj)) != NULL)
	objxm_obj_set_ui_arg(tf_obj, AB_ARG_STRING, XmNvalue, valstr);
    else /* valstr == NULL, make sure the text field is clean */
	if (ctype == OBJXM_CONFIG_BUILD)
	    objxm_obj_set_ui_arg(tf_obj, AB_ARG_STRING, XmNvalue, Util_empty_string);

    return 0;
}

static ABObj
get_menu_ref(
    ABObj	obj
)
{
    ABObj first_child;

    first_child = obj_get_child(obj, 0);
    if (first_child && obj_is_menu_ref(first_child))
	return first_child;

    return NULL;

}


/*
 *******************
 * Separator methods
 *******************
 */
static ABObj
separator_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;

    switch(type)
    {
	case AB_CFG_HELP_OBJ:
	    cfg_obj = obj;
	break;

        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SIZE_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
        case AB_CFG_SELECT_OBJ:
        case AB_CFG_PARENT_OBJ:
        case AB_CFG_BORDER_OBJ:
            cfg_obj = obj;
	    break;

	case AB_CFG_DND_OBJ:
        case AB_CFG_LABEL_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
	case AB_CFG_MENU_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
	    break;

        default:
            util_dprintf(1, "simple_get_config_obj: unsupported config-type\n");
    }

    return(cfg_obj);
}


static int
separator_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	   o_obj;

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);

    o_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    objxm_obj_set_orientation_arg(obj, ctype);
    objxm_obj_set_line_style_arg(obj, ctype);

    return 0;
}

/*
 *******************
 * SpinBox methods
 *******************
 */
static int
spin_box_xmconfig(
    ABObj    obj
)
{
    objxm_comp_config_labeled_obj(obj, AB_TYPE_SPIN_BOX, "field", _dtSpinBox);

    return OK;
}

static int
spin_box_unconfig(
    ABObj       obj
)
{
    objxm_comp_unconfig_labeled_obj(obj);

    return OK;
}

static ABObj
spin_box_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    BOOL  has_label = FALSE;

    if (obj_has_label(obj))
        has_label = TRUE;

    switch(type)
    {
        case AB_CFG_HELP_OBJ:
            cfg_obj = obj;
            break;
        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_SIZE_OBJ:
	case AB_CFG_PARENT_OBJ:
            cfg_obj = has_label? obj_get_child(obj, 1) : obj;
            break;
        case AB_CFG_LABEL_OBJ:
            cfg_obj = has_label? obj_get_child(obj, 0) : NULL;
            break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
        case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = NULL;
            break;
        default:
            util_dprintf(1, "spin_box_get_config_obj: unsupported config-type\n");
    }
    return(cfg_obj);
}


static int
spin_box_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	sb_obj;
    int		num_items;
    int		i;

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);

    sb_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    /* SpinBox Type */
    objxm_obj_set_literal_ui_arg(sb_obj, ctype, DtNspinBoxChildType,
          obj_get_text_type(obj) == AB_TEXT_NUMERIC? DtNUMERIC : XmSTRING);

    /* Arrow Style */
    objxm_obj_set_arrow_style_arg(sb_obj, ctype);

    if (obj_get_text_type(obj) == AB_TEXT_NUMERIC)
    {
	/* Value Range */
	objxm_obj_set_ui_arg(sb_obj, AB_ARG_INT, DtNminimumValue,
		obj_get_min_value(obj));
	objxm_obj_set_ui_arg(sb_obj, AB_ARG_INT, DtNmaximumValue,
		obj_get_max_value(obj));

	/* Initial Value */
	objxm_obj_set_ui_arg(sb_obj, AB_ARG_INT, DtNposition,
		obj_get_initial_value_int(obj));

	/* Increment/Decimal Points */
	objxm_obj_set_ui_arg(sb_obj, AB_ARG_INT, DtNincrementValue,
		obj_get_increment(obj));
	objxm_obj_set_ui_arg(sb_obj, AB_ARG_INT, XmNdecimalPoints,
		obj_get_decimal_points(obj));
    }
    else if (ctype == OBJXM_CONFIG_CODEGEN) /* String List */
    {
	/* Set Initially Selected Item - can only do this for generated code
	 * because in Build mode, the items might not be created yet!
	 */
	num_items = obj_get_num_items(obj);
	for (i = 0; i < num_items; i++)
	    if (obj_is_initially_selected(obj_get_item(obj, i)))
	    {
		objxm_obj_set_ui_arg(sb_obj, AB_ARG_INT, DtNposition, i);
		break;
	    }
    }
    return 0;
}

/*
 *******************
 * ComboBox methods
 *******************
 */
static int
combo_box_xmconfig(
    ABObj    obj
)
{
    objxm_comp_config_labeled_obj(obj, AB_TYPE_COMBO_BOX, "field", _dtComboBox);

    return OK;
}

static int
combo_box_unconfig(
    ABObj       obj
)
{
    objxm_comp_unconfig_labeled_obj(obj);

    return OK;
}

static ABObj
combo_box_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    BOOL  has_label = FALSE;

    if (obj_has_label(obj))
        has_label = TRUE;

    switch(type)
    {
        case AB_CFG_HELP_OBJ:
            cfg_obj = obj;
        break;

        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_PARENT_OBJ:
	case AB_CFG_SIZE_OBJ:
            cfg_obj = has_label? obj_get_child(obj, 1) : obj;
            break;
        case AB_CFG_LABEL_OBJ:
            cfg_obj = has_label? obj_get_child(obj, 0) : NULL;
            break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
        case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = NULL;
            break;
        default:
            util_dprintf(1, "combo_box_get_config_obj: unsupported config-type\n");
    }

    return(cfg_obj);
}


static int
combo_box_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);
    objxm_comp_set_read_only_args(obj, ctype);

    return 0;
}

static int
post_set_size(
    ABObj	obj,
    OBJXM_CONFIG_TYPE	ctype
)
{
    ABObj	szObj;

    szObj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

    /* For objects which don't honor size requests at create-time
     * (SpinBox, ComboBox), we need to set the size after instantiation...
     */
    if (obj_get_width(obj) != -1)
    {
	objxm_obj_set_ui_arg(szObj, AB_ARG_INT, XmNwidth, obj_get_width(obj));
	obj_set_flag(szObj, AttrChangedFlag);
    }
    if (obj_get_height(obj) != -1)
    {
	objxm_obj_set_ui_arg(szObj, AB_ARG_INT, XmNheight, obj_get_height(obj));
        obj_set_flag(szObj, AttrChangedFlag);
    }
    return 0;

}

/*
 *******************
 * Slider methods
 *******************
 */
static int
scale_xmconfig(
    ABObj    obj
)
{
    objxm_comp_config_labeled_obj(obj, AB_TYPE_SCALE, "scale", _xmScale);

    return OK;
}

static int
scale_unconfig(
    ABObj       obj
)
{
    objxm_comp_unconfig_labeled_obj(obj);

    return OK;
}

static ABObj
scale_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    BOOL  has_label = FALSE;

    if (obj_has_label(obj))
        has_label = TRUE;

    switch(type)
    {
        case AB_CFG_HELP_OBJ:
            cfg_obj = obj;
        break;

        case AB_CFG_OBJECT_OBJ:
        case AB_CFG_SIZE_OBJ:
            cfg_obj = has_label? obj_get_child(obj, 1) : obj;
            break;
        case AB_CFG_LABEL_OBJ:
            cfg_obj = has_label? obj_get_child(obj, 0) : NULL;
            break;
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SELECT_OBJ:
            cfg_obj = obj;
            break;

	case AB_CFG_DND_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
        case AB_CFG_PARENT_OBJ:
        case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
            cfg_obj = NULL;
            break;

        default:
            util_dprintf(1, "scale_get_config_obj: unknown config-type\n");
            cfg_obj = NULL;
    }

    return(cfg_obj);
}

static int
scale_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    ABObj	   o_obj;

    objxm_comp_set_label_args(obj, ctype);
    objxm_comp_set_lbl_pos_args(obj, ctype);
    objxm_comp_set_read_only_args(obj, ctype);

    o_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    /* Orientation */
    objxm_obj_set_orientation_arg(o_obj, ctype);

    /* Direction */
    objxm_obj_set_direction_arg(o_obj, ctype);

    /* Value Range */
    objxm_obj_set_ui_arg(o_obj, AB_ARG_INT, XmNminimum, obj_get_min_value(obj));
    objxm_obj_set_ui_arg(o_obj, AB_ARG_INT, XmNmaximum, obj_get_max_value(obj));

    /* Increment/Decimal Points */
    objxm_obj_set_ui_arg(o_obj, AB_ARG_INT, XmNscaleMultiple,
		obj_get_increment(obj));
    objxm_obj_set_ui_arg(o_obj, AB_ARG_INT, XmNdecimalPoints,
		obj_get_decimal_points(obj));

    /* Initial Value/Show Value */
    objxm_obj_set_ui_arg(o_obj, AB_ARG_INT, XmNvalue,
		obj_get_initial_value_int(obj));
    objxm_obj_set_ui_arg(o_obj, AB_ARG_BOOLEAN, XmNshowValue,
		obj_get_show_value(obj));

    return 0;
}

/*
 **********************
 * File Chooser methods
 **********************
 */
static int
fchooser_xmconfig(
    ABObj    obj
)
{
    ABObj               pwobj, workobj;
    ABObj               apobj = NULL;

    /* DialogShell */
    obj->part_of = obj;
    obj_set_class_name(obj, _xmDialogShell);

    /* Create PanedWindow SubObj */
    pwobj = obj_create(AB_TYPE_CONTAINER, NULL);
    pwobj->part_of = obj;
    obj_set_class_name(pwobj, _xmPanedWindow);
    obj_set_flag(pwobj, NoCodeGenFlag);

    /* Create WorkArea (FileSelectionBox) SubObj. Can't make it
     * type AB_TYPE_FILE_CHOOSER because it will be parented off
     * of AB_toplevel in xm_instantiate_obj(), which we don't
     * want.
     */
    workobj = obj_create(AB_TYPE_CONTAINER, NULL);
    workobj->part_of = obj;
    obj_append_child(pwobj, workobj);
    obj_set_class_name(workobj, _xmFileSelectionBox);

    obj_append_child(obj, pwobj);
    obj_set_unique_name(pwobj,
        ab_ident_from_name_and_label(obj_get_name(obj), "panedwin"));
    obj_set_unique_name(workobj,
        ab_ident_from_name_and_label(obj_get_name(obj), "fsb"));

    xmconfig_window_select_area(obj, pwobj);

    return OK;
}

static int
box_unconfig(
    ABObj       obj
)
{
    ABObj       pwobj;

    pwobj = obj_get_child(obj, 0);
    obj_destroy(pwobj);

    return OK;
}

static ABObj
box_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;
    ABObj modobj = NULL;
    ABObj selobj  = NULL;
    ABObj pwobj, msgobj, childobj;

    pwobj = obj_get_child(obj, 0);

    switch(type)
    {
	case AB_CFG_WINDOW_PW_OBJ:
	    cfg_obj = pwobj;
	    break;
        case AB_CFG_OBJECT_OBJ:
	{
	    /* Return the FileSelectionBox child */
	    cfg_obj = obj_get_child(pwobj, 0);
	    break;
	}
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_LABEL_OBJ:
	    /* Return the shell object (label = title) */
            cfg_obj = obj;
            break;
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_SELECT_OBJ:
	{
	    int		num_children = 0, i;

            if (type == AB_CFG_SIZE_OBJ)
                cfg_obj = pwobj;
	    else
	    {
		num_children = obj_get_num_children(pwobj);
		/* Search through immediate children of the Shell-container
		 * and find the WorkArea & Status SubObjs if they exist
		 */
		for (i = 0; i < num_children; i++)
		{
           	    childobj = obj_get_child(pwobj, i);
                    if (obj_is_label(childobj) && childobj->part_of == obj)
		    {
                        msgobj = childobj;
                        modobj = obj_get_child(msgobj, 0);
                        selobj = obj_get_child(msgobj, 1);
                    }
                }
        	if (type == AB_CFG_SELECT_OBJ)
                    cfg_obj = selobj;
		else if (type == AB_CFG_STATUS_OBJ)
                    cfg_obj = modobj;

	    }
	    break;
	}

	case AB_CFG_DND_OBJ:
        case AB_CFG_HELP_OBJ:
        case AB_CFG_MENU_OBJ:
        case AB_CFG_BORDER_OBJ:
        case AB_CFG_PARENT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
            cfg_obj = NULL;
            break;

        default:
            if (util_get_verbosity() > 0)
                fprintf(stderr,"box_get_config_obj: unsupported config-type\n");
	    cfg_obj = NULL;
    }

    return(cfg_obj);
}


static int
fchooser_set_args(
    ABObj       obj,
    OBJXM_CONFIG_TYPE ctype
)
{
    AB_FILE_TYPE_MASK	ftm = AB_FILE_TYPE_MASK_UNDEF;
    STRING		label = NULL, str = NULL;
    ABObj	   	fsb_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    ABObj	   	lbl_obj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
    ABObj       	pwobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_PW_OBJ);
    BOOL	   	cgen_args;
    XtArgVal		valstr;

    /* Shell Resources */
    if ((label = obj_get_label(obj)) != NULL)
        objxm_obj_set_ui_arg(lbl_obj, AB_ARG_STRING, XmNtitle, label);
    objxm_obj_set_ui_arg(lbl_obj, AB_ARG_BOOLEAN, XmNallowShellResize, TRUE);

    /* PanedWindow Resources */
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNmarginWidth, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNmarginHeight, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNspacing, 0);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNsashHeight, 1);
    objxm_obj_set_ui_arg(pwobj, AB_ARG_INT, XmNsashWidth, 1);

    cgen_args = (ctype == OBJXM_CONFIG_CODEGEN? TRUE : FALSE);

    /* FileSelectionBox Resources */
    str = obj_get_filter_pattern(obj);
    if (!util_strempty(str))
    {
	if (cgen_args)
	    valstr = (XtArgVal) istr_create(str);
	else
	    valstr = (XtArgVal) objxm_str_to_xmstr(ObjxmP_toplevel, str);
	objxm_obj_set_ui_arg(fsb_obj, AB_ARG_XMSTRING, XmNpattern, valstr);
    }

    str = obj_get_directory(obj);
    if (!util_strempty(str))
    {
	if (cgen_args)
	    valstr = (XtArgVal) istr_create(str);
	else
            valstr = (XtArgVal) objxm_str_to_xmstr(ObjxmP_toplevel, str);
	objxm_obj_set_ui_arg(fsb_obj, AB_ARG_XMSTRING, XmNdirectory, valstr);
    }

    str = obj_get_ok_label(obj);
    if (!util_strempty(str))
    {
	if (cgen_args)
            valstr = (XtArgVal) istr_create(str);
	else
            valstr = (XtArgVal) objxm_str_to_xmstr(ObjxmP_toplevel, str);
	objxm_obj_set_ui_arg(fsb_obj, AB_ARG_XMSTRING, XmNokLabelString, valstr);
    }

    objxm_obj_set_ui_arg(fsb_obj, AB_ARG_BOOLEAN,
	XmNautoUnmanage, obj_get_auto_dismiss(obj)? TRUE : FALSE);

    ftm = obj_get_file_type_mask(obj);
    switch (ftm)
    {
        case AB_FILE_REGULAR:
            objxm_obj_set_literal_ui_arg(fsb_obj, ctype, XmNfileTypeMask,
                                        XmFILE_REGULAR);
            break;
        case AB_FILE_DIRECTORY:
            objxm_obj_set_literal_ui_arg(fsb_obj, ctype, XmNfileTypeMask,
                                        XmFILE_DIRECTORY);
            break;
        case AB_FILE_ANY:
            objxm_obj_set_literal_ui_arg(fsb_obj, ctype, XmNfileTypeMask,
                                        XmFILE_ANY_TYPE);
            break;
    }

    /* Set the interface name and select-state on the footer */
    if (ctype == OBJXM_CONFIG_BUILD)
        set_window_select_area_args(obj);

    return 0;
}

static int
fchooser_post_inst(
    ABObj               obj,
    OBJXM_CONFIG_TYPE   ctype
)
{
    Widget      dir_list = NULL,
                files_list = NULL,
		fselnbox = NULL;
    ABObj       fsb_obj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (ctype == OBJXM_CONFIG_BUILD)
    {
        post_inst_window_select_area(obj);

	fselnbox = objxm_get_widget(fsb_obj);
	clear_fchooser_fields(fselnbox);

	dir_list = XmFileSelectionBoxGetChild(fselnbox, XmDIALOG_DIR_LIST);
	if (dir_list)
	    XmListDeleteAllItems(dir_list);

	files_list = XmFileSelectionBoxGetChild(fselnbox, XmDIALOG_LIST);
	if (files_list)
	    XmListDeleteAllItems(files_list);
    }
    return 0;
}

static void
clear_fchooser_fields(
    Widget      widget
)
{
    WidgetList  children = NULL;
    Cardinal    numChild = (Cardinal) 0;
    int         i;

    if (widget == NULL) return;

    XtVaGetValues(widget,
                        XmNnumChildren, &numChild,
                        XmNchildren, &children,
                NULL);

    for (i = 0; i < (int)numChild; i++)
    {
        if (XtClass(children[i]) == xmTextFieldWidgetClass)
        {
            XmTextFieldSetString(children[i], "");
        }
        else if (XtIsComposite(children[i]))
        {
            clear_fchooser_fields(children[i]);
        }
    }
}

/*
 **********************
 * Paned Window methods
 **********************
 */
static ABObj
panedwin_get_config_obj(
    ABObj       obj,
    AB_CFG_OBJ_TYPE type
)
{
    ABObj cfg_obj = NULL;

    switch(type)
    {
        case AB_CFG_OBJECT_OBJ:
	case AB_CFG_HELP_OBJ:
        case AB_CFG_POSITION_OBJ:
        case AB_CFG_SIZE_OBJ:
        case AB_CFG_SELECT_OBJ:
        case AB_CFG_MENU_PARENT_OBJ:
        case AB_CFG_PARENT_OBJ:
            cfg_obj = obj;
            break;
	case AB_CFG_DND_OBJ:
        case AB_CFG_LABEL_OBJ:
	case AB_CFG_MENU_OBJ:
	case AB_CFG_SCROLLBAR_OBJ:
	case AB_CFG_SCROLLING_OBJ:
        case AB_CFG_STATUS_OBJ:
        case AB_CFG_BORDER_OBJ:
            break;
        default:
	    util_dprintf(1, "panedwin_get_config_obj: unsupported config-type\n");
    }

    return(cfg_obj);
}
