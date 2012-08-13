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
/*
 *      $XConsortium: conn_obj.c /main/4 1996/07/03 16:57:53 mustafa $
 *
 * @(#)conn_obj.c	1.17 17 Feb 1994	cde_app_builder/src/ab
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


#include <stdio.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>

#include <ab_private/connP.h>
#include <ab_private/brws.h>
#include <ab_private/abobj.h>
#include "dtbuilder.h"

static char *when_labels[(int)AB_WHEN_NUM_VALUES+1];
static char *stdact_labels[(int)AB_BUILTIN_ACTION_NUM_VALUES+1];
static char	*unknown_str = NULL;
static char	*invalid_str = NULL;

static AB_WHEN
application_whens[] =
{
    AB_WHEN_SESSION_SAVE,
    AB_WHEN_SESSION_RESTORE,
    AB_WHEN_TOOLTALK_DO_COMMAND,
    AB_WHEN_TOOLTALK_GET_STATUS,
    AB_WHEN_TOOLTALK_PAUSE_RESUME,
    AB_WHEN_TOOLTALK_QUIT,
};

static AB_WHEN
basewin_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_HIDDEN,
    AB_WHEN_SHOWN,
    AB_WHEN_DRAGGED_FROM,
    AB_WHEN_DROPPED_ON
};

static AB_BUILTIN_ACTION
basewin_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
button_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_ACTIVATED,
};

static AB_BUILTIN_ACTION
button_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
choice_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
};

static AB_BUILTIN_ACTION
choice_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
combobox_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_ITEM_SELECTED,
};

static AB_BUILTIN_ACTION
combobox_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
container_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_DRAGGED_FROM,
    AB_WHEN_DROPPED_ON,
};

static AB_BUILTIN_ACTION
container_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
group_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
};

static AB_BUILTIN_ACTION
group_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
menubar_whens[] = 
{ 
    AB_WHEN_AFTER_CREATED, 
    AB_WHEN_DESTROYED, 
};
 
static AB_BUILTIN_ACTION 
menubar_acts[] = 
{ 
    AB_STDACT_DISABLE, 
    AB_STDACT_ENABLE, 
    AB_STDACT_HIDE, 
    AB_STDACT_SHOW, 
};

static AB_WHEN
panedwin_whens[] = 
{ 
    AB_WHEN_AFTER_CREATED, 
    AB_WHEN_DESTROYED, 
};
 
static AB_BUILTIN_ACTION 
panedwin_acts[] = 
{ 
    AB_STDACT_DISABLE, 
    AB_STDACT_ENABLE, 
    AB_STDACT_HIDE, 
    AB_STDACT_SHOW, 
};

static AB_WHEN
dialog_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_HIDDEN,
    AB_WHEN_SHOWN,
    AB_WHEN_DRAGGED_FROM,
    AB_WHEN_DROPPED_ON
};

static AB_BUILTIN_ACTION
dialog_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
drawarea_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_DRAGGED_FROM,
    AB_WHEN_DROPPED_ON,
    AB_WHEN_REPAINT_NEEDED,
    AB_WHEN_AFTER_RESIZED,
};

static AB_BUILTIN_ACTION
drawarea_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
label_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_DRAGGED_FROM,
    AB_WHEN_DROPPED_ON,
};

static AB_BUILTIN_ACTION
label_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
list_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_DOUBLE_CLICKED_ON,
    AB_WHEN_DRAGGED_FROM,
    AB_WHEN_DROPPED_ON,
    AB_WHEN_ITEM_SELECTED,
};

static AB_BUILTIN_ACTION
list_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
menu_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_POPPED_UP,
    AB_WHEN_POPPED_DOWN,
};

static AB_WHEN
separator_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
};

static AB_BUILTIN_ACTION
separator_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
scale_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_DRAGGED,
    AB_WHEN_VALUE_CHANGED,
};

static AB_BUILTIN_ACTION
scale_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
    AB_STDACT_SET_VALUE,
};

static AB_WHEN
spinbox_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_VALUE_CHANGED,
};

static AB_BUILTIN_ACTION
spinbox_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
    AB_STDACT_SET_VALUE,
};

static AB_WHEN
termpane_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
};

static AB_BUILTIN_ACTION
termpane_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};

static AB_WHEN
textfield_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_BEFORE_TEXT_CHANGED,
    AB_WHEN_TEXT_CHANGED,
};

static AB_BUILTIN_ACTION
textfield_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
    AB_STDACT_SET_TEXT,
};

static AB_WHEN
textpane_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_BEFORE_TEXT_CHANGED,
    AB_WHEN_TEXT_CHANGED,
};

static AB_BUILTIN_ACTION
textpane_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_TEXT,
};

static AB_WHEN
menuitem_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_ACTIVATED,
};

static AB_BUILTIN_ACTION
menuitem_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
menubaritem_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
};

static AB_BUILTIN_ACTION
menubaritem_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
choiceitem_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_ACTIVATED,
    AB_WHEN_TOGGLED,
};

static AB_BUILTIN_ACTION
choiceitem_acts[] =
{
    AB_STDACT_DISABLE,
    AB_STDACT_ENABLE,
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
    AB_STDACT_SET_LABEL,
};

static AB_WHEN
message_whens[] =
{
    AB_WHEN_ACTION1,
    AB_WHEN_ACTION2,
    AB_WHEN_ACTION3,
    AB_WHEN_CANCEL,
};

static AB_WHEN
fchooser_whens[] =
{
    AB_WHEN_AFTER_CREATED,
    AB_WHEN_DESTROYED,
    AB_WHEN_HIDDEN,
    AB_WHEN_SHOWN,
    AB_WHEN_OK,
    AB_WHEN_CANCEL,
};

static AB_BUILTIN_ACTION
fchooser_acts[] =
{
    AB_STDACT_HIDE,
    AB_STDACT_SHOW,
};   

ConnObj
ConnP_conn_objs[] =
{
    { AB_TYPE_PROJECT, 	-1, 		"Application",
	application_whens, XtNumber(application_whens),
	AB_WHEN_SESSION_SAVE,
        (AB_BUILTIN_ACTION *) NULL, (Cardinal) 0,
	(AB_BUILTIN_ACTION) NULL			},
    { AB_TYPE_BUTTON,	-1,		"Button",
	button_whens, XtNumber(button_whens),
	AB_WHEN_ACTIVATED,
	button_acts, XtNumber(button_acts),
	AB_STDACT_SET_LABEL				},
    { AB_TYPE_CHOICE,	-1,		"Choice",
	choice_whens, XtNumber(choice_whens),
	AB_WHEN_AFTER_CREATED,
	choice_acts, XtNumber(choice_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_ITEM,	(int)AB_ITEM_FOR_CHOICE, "Choice Item",
	choiceitem_whens, XtNumber(choiceitem_whens),
	AB_WHEN_ACTIVATED,
	choiceitem_acts, XtNumber(choiceitem_acts),
	AB_STDACT_SET_LABEL				},
    { AB_TYPE_COMBO_BOX, -1,		"Combo Box",
	combobox_whens, XtNumber(combobox_whens),
	AB_WHEN_ITEM_SELECTED,
	combobox_acts, XtNumber(combobox_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_CONTAINER, -1,    	"Control Pane",
        container_whens, XtNumber(container_whens),
	AB_WHEN_AFTER_CREATED,
        container_acts, XtNumber(container_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_DIALOG,	-1,		"Custom Dialog",
	dialog_whens, XtNumber(dialog_whens),
	AB_WHEN_SHOWN,
	dialog_acts, XtNumber(dialog_acts),
	AB_STDACT_SHOW					},
    { AB_TYPE_DRAWING_AREA,	-1,	"Draw Area Pane",
	drawarea_whens, XtNumber(drawarea_whens),
	AB_WHEN_AFTER_RESIZED,
	drawarea_acts, XtNumber(drawarea_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_FILE_CHOOSER,	-1,	"File Selection Dialog",
	fchooser_whens, XtNumber(fchooser_whens),
	AB_WHEN_SHOWN,
	fchooser_acts, XtNumber(fchooser_acts),
	AB_STDACT_SHOW					},
    { AB_TYPE_CONTAINER, AB_CONT_GROUP,	"Group",
        group_whens, XtNumber(group_whens),
	AB_WHEN_AFTER_CREATED,
        group_acts, XtNumber(group_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_LABEL,	-1,		"Label",
	label_whens, XtNumber(label_whens),
	AB_WHEN_AFTER_CREATED,
	label_acts, XtNumber(label_acts),
	AB_STDACT_SET_LABEL				},
    { AB_TYPE_LIST,	-1,		"List",
	list_whens, XtNumber(list_whens),
	AB_WHEN_ITEM_SELECTED,
	list_acts, XtNumber(list_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_BASE_WINDOW,	-1,	"Main Window",
	basewin_whens, XtNumber(basewin_whens),
	AB_WHEN_SHOWN,
	basewin_acts, XtNumber(basewin_acts),
	AB_STDACT_SHOW					},
    { AB_TYPE_MENU,	-1,		"Menu",
	menu_whens, XtNumber(menu_whens),
	AB_WHEN_POPPED_UP,
	(AB_BUILTIN_ACTION *) NULL, (Cardinal) 0,
	(AB_BUILTIN_ACTION) NULL			},
    { AB_TYPE_ITEM,	(int)AB_ITEM_FOR_MENU, "Menu Item",
	menuitem_whens, XtNumber(menuitem_whens),
	AB_WHEN_ACTIVATED,
	menuitem_acts, XtNumber(menuitem_acts),
	AB_STDACT_SET_LABEL				},
    { AB_TYPE_CONTAINER, AB_CONT_MENU_BAR,	"Menubar",
        menubar_whens, XtNumber(menubar_whens),
	AB_WHEN_AFTER_CREATED,
        menubar_acts, XtNumber(menubar_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_ITEM,	(int)AB_ITEM_FOR_MENUBAR, "Menubar Item",
	menubaritem_whens, XtNumber(menubaritem_whens),
	AB_WHEN_AFTER_CREATED,
	menubaritem_acts, XtNumber(menubaritem_acts),
	AB_STDACT_SET_LABEL				},
    { AB_TYPE_MESSAGE,	-1,	"Message",
	message_whens, XtNumber(message_whens), 
	(AB_WHEN) NULL,
	(AB_BUILTIN_ACTION *) NULL, (Cardinal) 0,
	(AB_BUILTIN_ACTION) NULL			},
    { AB_TYPE_CONTAINER, AB_CONT_PANED,	"Paned Window", 
        panedwin_whens, XtNumber(panedwin_whens), 
	AB_WHEN_AFTER_CREATED,
        panedwin_acts, XtNumber(panedwin_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_SCALE,	-1,		"Scale",
	scale_whens, XtNumber(scale_whens),
	AB_WHEN_VALUE_CHANGED,
	scale_acts, XtNumber(scale_acts),
	AB_STDACT_SET_VALUE				},
    { AB_TYPE_SEPARATOR,-1,		"Separator",
	separator_whens, XtNumber(separator_whens),
	AB_WHEN_AFTER_CREATED,
	separator_acts, XtNumber(separator_acts),
	AB_STDACT_HIDE					},
    { AB_TYPE_SPIN_BOX,	-1,		"Spin Box",
	spinbox_whens, XtNumber(spinbox_whens),
	AB_WHEN_VALUE_CHANGED,
	spinbox_acts, XtNumber(spinbox_acts),
	AB_STDACT_SET_VALUE				},
    { AB_TYPE_TERM_PANE,	-1,	"Term Pane",
	termpane_whens, XtNumber(termpane_whens),
	AB_WHEN_AFTER_CREATED,
	termpane_acts, XtNumber(termpane_acts),
	AB_STDACT_DISABLE				},
    { AB_TYPE_TEXT_FIELD,	-1,	"Text Field",
	textfield_whens, XtNumber(textfield_whens),
	AB_WHEN_TEXT_CHANGED,
	textfield_acts, XtNumber(textfield_acts),
	AB_STDACT_SET_TEXT				},
    { AB_TYPE_TEXT_PANE,	-1,	"Text Pane",
	textpane_whens, XtNumber(textpane_whens),
	AB_WHEN_TEXT_CHANGED,
	textpane_acts, XtNumber(textpane_acts),
	AB_STDACT_SET_TEXT				}
};

int		ConnP_num_conn_objs = XtNumber(ConnP_conn_objs);


ConnWhen	*ConnP_conn_whens = (ConnWhen *)NULL;
int		ConnP_num_conn_whens = 0;


ConnAct		*ConnP_conn_acts = (ConnAct *)NULL;
int		ConnP_num_conn_acts = 0;


static AB_OBJECT_TYPE	cur_source_type	= AB_TYPE_UNKNOWN;
static int		cur_source_subtype	= -1;

static AB_OBJECT_TYPE	cur_target_type	= AB_TYPE_UNKNOWN;
static int		cur_target_subtype	= -1;

static AB_FUNC_TYPE	cur_action_type	= AB_FUNC_UNDEF;

static ABObj	cur_source	= (ABObj)NULL;
static ABObj	cur_target	= (ABObj)NULL;
static ABObj	cur_action_obj	= (ABObj)NULL;


static BOOL	is_descendant(
		    ABObj	parent,
		    ABObj	child
		);
static void	assign_when_list(
		    AB_OBJECT_TYPE	source_type,
		    int			source_subtype
		);
static void	assign_action_list(
		    AB_OBJECT_TYPE	target_type,
		    int			target_subtype
		);
static STRING	get_when_label(
		    AB_WHEN	when_type
		);
static STRING	get_stdact_label(
		    AB_BUILTIN_ACTION	stdact_type
		);

static STRING
get_when_label(
    AB_WHEN	when_type
)
{
    STRING	ret_val = NULL;

    /*
     * Fetch right string to return from when_labels array
     * Make sure the index used is valid, otherwise, use
     * the "unknown" string.
     */
    if ((AB_WHEN_UNDEF <= when_type) && (when_type <= AB_WHEN_NUM_VALUES))
	ret_val = when_labels[when_type];
    else
	ret_val = when_labels[AB_WHEN_UNDEF];

    /*
     * This returns the "invalid" string
     */
    if (!ret_val)
	ret_val = when_labels[AB_WHEN_UNDEF];

    return(ret_val);
}

static STRING
get_stdact_label(
    AB_BUILTIN_ACTION	stdact_type
)
{
    STRING	ret_val = NULL;

    /*
     * Fetch right string to return from the stdact_labels array
     * Make sure the index used is valid, otherwise, use
     * the "unknown" string.
     */
    if ((AB_STDACT_UNDEF <= stdact_type) && 
	(stdact_type <= AB_BUILTIN_ACTION_NUM_VALUES))
	ret_val = stdact_labels[stdact_type];
    else
	ret_val = stdact_labels[stdact_type];

    /*
     * This returns the "invalid" string
     */
    if (!ret_val)
	ret_val = when_labels[AB_WHEN_UNDEF];

    return(ret_val);
}


extern char **
connP_get_when_labels(
    int	*num_labels
)
{
    static STRING	*when_label_list = (STRING *)NULL;

    if (when_label_list == NULL)
    {
	register int	i;

	when_label_list = (STRING *)XtMalloc(
			    sizeof(STRING) * ((int)AB_WHEN_NUM_VALUES - 1));

	for (i = 1; i < (int)AB_WHEN_NUM_VALUES; i++)
	    when_label_list[i-1] = get_when_label((AB_WHEN)i);
    }
    *num_labels = (int)AB_WHEN_NUM_VALUES - 1;
    return(when_label_list);
}

extern char **
connP_get_act_labels(
    int	*num_labels
)
{
    static STRING	*stdact_label_list = (STRING *)NULL;

    if (stdact_label_list == NULL)
    {
	register int	i;

	stdact_label_list = (STRING *)XtMalloc(
			sizeof(STRING) * ((int)AB_BUILTIN_ACTION_NUM_VALUES - 1));

	for (i = 1; i < (int)AB_BUILTIN_ACTION_NUM_VALUES; i++)
	    stdact_label_list[i-1] = get_stdact_label((AB_BUILTIN_ACTION)i);
    }
    *num_labels = (int)AB_BUILTIN_ACTION_NUM_VALUES - 1;
    return(stdact_label_list);
}

extern AB_OBJECT_TYPE
connP_get_source_type(
    void
)
{
    if (cur_source_type == AB_TYPE_UNKNOWN)
	connP_set_source_type(AB_TYPE_BUTTON, -1);
    return cur_source_type;
}

extern int
connP_get_source_subtype(
    void
)
{
    return cur_source_subtype;
}

extern AB_OBJECT_TYPE
connP_get_target_type(
    void
)
{
    if (cur_target_type == AB_TYPE_UNKNOWN)
	connP_set_target_type(AB_TYPE_BUTTON, -1);
    return cur_target_type;
}

extern int
connP_get_target_subtype(
    void
)
{
    return cur_target_subtype;
}

extern void
connP_set_source_type(
    AB_OBJECT_TYPE	obj_type,
    int			obj_subtype
)
{
    BOOL	ChangedType = (obj_type != cur_source_type);

    if (ChangedType || (obj_subtype != cur_source_subtype))
    {
	cur_source_type = obj_type;
	if (connP_objtype_needs_subtype(obj_type, obj_subtype))
	    cur_source_subtype = obj_subtype;
	else
	    cur_source_subtype = -1;
	cur_action_obj = NULL;

	assign_when_list(cur_source_type, cur_source_subtype);
	connP_ui_source_type_update(cur_source_type, cur_source_subtype);
    }
}

extern void
connP_set_target_type(
    AB_OBJECT_TYPE	obj_type,
    int			obj_subtype
)
{
    BOOL        ChangedType = (obj_type != cur_target_type);

    if (ChangedType || (obj_subtype != cur_target_subtype))
    {
	cur_target_type = obj_type;
	if (connP_objtype_needs_subtype(obj_type, obj_subtype))
	    cur_target_subtype = obj_subtype;
	else
	    cur_target_subtype = -1;
	cur_action_obj = NULL;

 	if (ChangedType)
	{
	    assign_action_list(cur_target_type, cur_target_subtype);
	    connP_ui_target_type_update(cur_target_type, cur_target_subtype);
	}
    }
}

extern void
conn_set_source(
    ABObj	obj
)
{
    if (cur_source == obj)
	return;

    if ((cur_source = obj) != NULL)
    {
	connP_set_source_type(obj_get_type(obj), obj_get_subtype(obj));
    }
    else
    {
	if (connP_get_source_type() == AB_TYPE_UNDEF)
	    connP_set_source_type(AB_TYPE_BUTTON, -1);
    }
}

extern ABObj
connP_get_source(
)
{
    return cur_source;
}

extern void
conn_set_target(
    ABObj	obj
)
{
    if (cur_target == obj)
	return;

    if ((cur_target = obj) != NULL)
    {
	connP_set_target_type(obj_get_type(obj), obj_get_subtype(obj));
    }
    else
    {
	if (connP_get_target_type() == AB_TYPE_UNDEF)
	    connP_set_target_type(AB_TYPE_BUTTON, -1);
    }
}

extern ABObj
connP_get_target(
)
{
    return cur_target;
}

extern ABObj
connP_get_connection(
    void
)
{
    return cur_action_obj;
}

extern void
connP_set_connection(
    ABObj	action
)
{
    cur_action_obj = action;
}

extern void
connP_set_action_type(
    AB_FUNC_TYPE	action_type
)
{
    cur_action_type = action_type;
}

extern AB_FUNC_TYPE
connP_get_action_type(
    void
)
{
    return cur_action_type;
}

extern char *
connP_make_conn_string(
    ABObj	action_obj
)
{
    register int	i;
    long		si;
    static char		conn_string[BUFSIZ];
    char		buf[BUFSIZ];
    AB_ACTION_INFO	*action_info;
    ABObj		src_obj;
    STRING		modname;

    conn_string[0] = '\0';

    if (!obj_is_action(action_obj))
	return(conn_string);

    action_info = &(action_obj->info.action);

    src_obj = action_info->from;

    if (src_obj == NULL)
	return(conn_string);

    si = connP_get_obj_type_index(obj_get_type(src_obj),
				  obj_get_subtype(src_obj));

    if (si < 0)
	return(conn_string);

    for (i = 0; i < ConnP_conn_objs[si].num_whens; i++)
	if (ConnP_conn_objs[si].when_list[i] == action_info->when)
	{
	    strcat(conn_string, get_when_label(action_info->when));
	    break;
	}
    if (i == ConnP_conn_objs[si].num_whens)
    {
	strcat(conn_string, unknown_str);
    }
    
    strcat(conn_string, catgets(Dtb_project_catd, 100, 196, " on "));
    modname = abobj_get_moduled_name(src_obj);
    strcat(conn_string, (char*)modname);
    XtFree(modname);
    strcat(conn_string, " -> ");
    
    switch (action_info->func_type)
    {
      case AB_FUNC_BUILTIN:
	  {
	    int			ti;
	    ABObj		target_obj;

	    target_obj = action_info->to;
	    if (target_obj == NULL)
		return("");
	    
	    ti = connP_get_obj_type_index(obj_get_type(target_obj),
					  obj_get_subtype(target_obj));
	    if (ti < 0)
		return("");

	    for (i = 0; i < ConnP_conn_objs[ti].num_actions; i++)
		if (ConnP_conn_objs[ti].action_list[i] ==
			    action_info->func_value.builtin)
	    {
		strcat(conn_string,
			get_stdact_label(action_info->func_value.builtin));
		break;
	    }
	    if (i == ConnP_conn_objs[ti].num_actions)
	    {
		strcat(conn_string, unknown_str);
	    }
	    
	    strcat(conn_string, catgets(Dtb_project_catd, 100, 196, " on "));
	    modname = abobj_get_moduled_name(target_obj);
	    strcat(conn_string, (char*)modname);
	    XtFree(modname);
	}
	break;

      case AB_FUNC_USER_DEF:
	strcat(conn_string, catgets(Dtb_project_catd, 100, 197, "Call function"));
	strcat(conn_string, " \"");
	{
	    ISTRING	istr_val = action_info->func_value.func_name;

	    if (istr_val != NULL)
		strcat(conn_string, istr_string(istr_val));
	}
	strcat(conn_string, "\"");
	break;

      case AB_FUNC_CODE_FRAG:
	strcat(conn_string, catgets(Dtb_project_catd, 100, 198, "Execute Code"));
	break;

      case AB_FUNC_ON_ITEM_HELP:
	strcat(conn_string, catgets(Dtb_project_catd, 100, 199, "Activate On-Item Help"));
	break;

      case AB_FUNC_HELP_VOLUME:
	strcat(conn_string, catgets(Dtb_project_catd, 100, 200, "Access Help Volume"));
	strcat(conn_string, " \"");
	strcat(conn_string, obj_get_func_help_volume(action_obj)?
		obj_get_func_help_volume(action_obj):"");
	strcat(conn_string, "\" ");
	strcat(conn_string, catgets(Dtb_project_catd, 100, 201, "at location"));
	strcat(conn_string, " \"");
	strcat(conn_string, obj_get_func_help_location(action_obj)?
		obj_get_func_help_location(action_obj):"");
	strcat(conn_string, "\"");
	break;
    }

    return(conn_string);
}

extern long
connP_get_obj_type_index(
    AB_OBJECT_TYPE	ab_type,
    int			ab_subtype
)
{
    register long	i;

    if (connP_objtype_needs_subtype(ab_type, ab_subtype))
    {
	for (i = 0; i < ConnP_num_conn_objs; i++)
	    if ((ConnP_conn_objs[i].obj_type == ab_type) &&
		(ConnP_conn_objs[i].obj_subtype == ab_subtype))
		return(i);
    }
    else
    {
	for (i = 0; i < ConnP_num_conn_objs; i++)
	    if ((ConnP_conn_objs[i].obj_type == ab_type) &&
	 	(ConnP_conn_objs[i].obj_subtype == -1))
		return(i);
    }
    return(-1);
}

extern AB_ARG_TYPE
connP_action_needs_arg(
    AB_BUILTIN_ACTION	act_type
)
{
    switch (act_type)
    {
	case AB_STDACT_SET_LABEL:
	case AB_STDACT_SET_TEXT:
	    return(AB_ARG_STRING);
	case AB_STDACT_SET_VALUE:
	    return(AB_ARG_INT);
    }
    return(AB_ARG_UNDEF);
}

extern void
connP_guess_when_action(
    AB_OBJECT_TYPE	source_type,
    int			source_subtype,
    AB_OBJECT_TYPE	target_type,
    int			target_subtype,
    AB_WHEN		*when_ret,
    AB_FUNC_TYPE	*func_type,
    AB_BUILTIN_ACTION	*act_ret
)
{
    long     i = connP_get_obj_type_index(source_type, source_subtype);
    long     j = connP_get_obj_type_index(target_type, target_subtype);

    if (ConnP_conn_objs[i].when_list != NULL)
	*when_ret = ConnP_conn_objs[i].default_when;

    if (ConnP_conn_objs[j].action_list != NULL)
	*act_ret = ConnP_conn_objs[j].default_act;

    *func_type = AB_FUNC_BUILTIN;

    switch (source_type)
    {
      case AB_TYPE_PROJECT:
      case AB_TYPE_CONTAINER:
      case AB_TYPE_DRAWING_AREA:
      case AB_TYPE_MENU:
      case AB_TYPE_CHOICE:
      case AB_TYPE_LABEL:
      case AB_TYPE_SEPARATOR:
      case AB_TYPE_TERM_PANE:
	*func_type = AB_FUNC_USER_DEF;
	break;

      case AB_TYPE_ITEM:
	if (source_subtype == AB_ITEM_FOR_MENUBAR)
	    *func_type = AB_FUNC_USER_DEF;
	
	switch (target_type)
	{
	  case AB_TYPE_BASE_WINDOW:
	  case AB_TYPE_DIALOG:
	    if (is_descendant(connP_get_target(), connP_get_source()))
		*act_ret = AB_STDACT_HIDE;
	}
	break;

      case AB_TYPE_BUTTON:
	switch (target_type)
	{
	  case AB_TYPE_BASE_WINDOW:
	  case AB_TYPE_DIALOG:
	    if (is_descendant(connP_get_target(), connP_get_source()))
		*act_ret = AB_STDACT_HIDE;
	}
	break;

	case AB_TYPE_MESSAGE:
	    if (connP_get_source() != NULL)
	    {
		ABObj	src_obj = connP_get_source();

	    	*func_type = AB_FUNC_USER_DEF;
	    	*act_ret = (AB_BUILTIN_ACTION) NULL;

	    	if (obj_has_action1_button(src_obj))
		    *when_ret = ConnP_conn_objs[i].when_list[0];	
	    	else if (obj_has_action2_button(src_obj))
		    *when_ret = ConnP_conn_objs[i].when_list[1];
	    	else if (obj_has_action3_button(src_obj))
                    *when_ret = ConnP_conn_objs[i].when_list[2];
	    	else if (obj_has_cancel_button(src_obj))
                    *when_ret = ConnP_conn_objs[i].when_list[3];
	    }
	    break;
    }
}



static BOOL
is_descendant(
    ABObj	parent,
    ABObj	child
)
{
    AB_TRAVERSAL	trav;
    ABObj		obj;
    BOOL		ret_val = FALSE;

    if (parent == NULL || child == NULL)
	return(FALSE);

    if (parent == child)
	return(TRUE);

    for (trav_open(&trav, parent, AB_TRAV_CHILDREN);
	 (obj = trav_next(&trav)) != NULL; )
	if (is_descendant(obj, child))
	{
	    ret_val = TRUE;
	    break;
	}
    trav_close(&trav);

    return(ret_val);
}

static void
assign_when_list(
    AB_OBJECT_TYPE	source_type,
    int			source_subtype
)
{
    long	i;
    int		n;
    ConnObj	*src_conn_obj;
    
    if (ConnP_conn_whens != (ConnWhen *)NULL)
	XtFree((char *)ConnP_conn_whens);
    ConnP_conn_whens = (ConnWhen *)NULL;
    ConnP_num_conn_whens = 0;
    
    i = connP_get_obj_type_index(source_type, source_subtype);
    if (i < 0)
	return;

    src_conn_obj = &(ConnP_conn_objs[i]);
    n = src_conn_obj->num_whens;

    if (n <= 0)
	return;
    
    ConnP_conn_whens = (ConnWhen *)XtMalloc(sizeof(ConnWhen) * n);

    for (i = 0; i < n; i++)
    {
	ConnP_conn_whens[i].when_type = src_conn_obj->when_list[i];
	ConnP_conn_whens[i].label = get_when_label(src_conn_obj->when_list[i]);
    }
    ConnP_num_conn_whens = n;
}

static void
assign_action_list(
    AB_OBJECT_TYPE	target_type,
    int			target_subtype
)
{
    long	i;
    int		n;
    ConnObj	*trg_conn_obj;
    
    if (ConnP_conn_acts != (ConnAct *)NULL)
	XtFree((char *)ConnP_conn_acts);
    ConnP_conn_acts = (ConnAct *)NULL;
    ConnP_num_conn_acts = 0;
    
    i = connP_get_obj_type_index(target_type, target_subtype);
    if (i < 0)
	return;

    trg_conn_obj = &(ConnP_conn_objs[i]);
    n = trg_conn_obj->num_actions;

    if (n <= 0)
	return;
    
    ConnP_conn_acts = (ConnAct *)XtMalloc(sizeof(ConnAct) * n);

    for (i = 0; i < n; i++)
    {
	ConnP_conn_acts[i].act_type = trg_conn_obj->action_list[i];
	ConnP_conn_acts[i].label = get_stdact_label(trg_conn_obj->action_list[i]);
    }
    ConnP_num_conn_acts = n;
}

/* This routine tries to match on type and subtype to
 * get the "When" and "Action", Object Type string info.
 */
extern int
connP_objtype_needs_subtype(
    AB_OBJECT_TYPE	obj_type,
    int			obj_subtype
)
{
    register	int	i;

    for (i = 0; i < ConnP_num_conn_objs; i++)
    {
	if ((ConnP_conn_objs[i].obj_type == obj_type) &&
	    (ConnP_conn_objs[i].obj_subtype == obj_subtype))
	{
	    if (ConnP_conn_objs[i].obj_subtype == -1)
		return (0);
	    else
		return(1);
	}
    }
    return(0);
}

extern long
connP_obj_enabled(
    AB_OBJECT_TYPE	obj_type,
    int			obj_subtype
)
{
    return(connP_get_obj_type_index(obj_type, obj_subtype) >= 0);
}

extern void	
connP_set_conn_arg(
    ABObj		ab_action,
    AB_ARG_TYPE		arg_type,
    char		*str_value
)
{
    /*
     * Populate arg entries
     */
    ab_action->info.action.arg_type = arg_type;
    switch (arg_type)
    {
      case AB_ARG_STRING:
	obj_set_arg_string(ab_action, str_value);
	break;
      case AB_ARG_INT:
	obj_set_arg_int(ab_action, atoi(str_value));
	break;
      default:
	break;
    }
}

extern void	
connP_make_builtin_conn(
    ABObj		ab_action,
    ABObj		c_source,
    ABObj		c_target,
    AB_WHEN		c_when,
    AB_BUILTIN_ACTION	c_act
)
{
    ab_action->info.action.from = c_source;
    ab_action->info.action.to = c_target;
    ab_action->info.action.when = c_when;
    ab_action->info.action.func_type = AB_FUNC_BUILTIN;
    ab_action->info.action.func_value.builtin = c_act;
    ab_action->info.action.auto_named = TRUE;
}

extern void	
connP_make_user_def_conn(
    ABObj		ab_action,
    ABObj		c_source,
    AB_WHEN		c_when,
    char		*function_name
)
{
    ab_action->info.action.from = c_source;
    ab_action->info.action.to = NULL;
    ab_action->info.action.when = c_when;
    ab_action->info.action.func_type = AB_FUNC_USER_DEF;
    ab_action->info.action.func_value.func_name = istr_create(function_name);
}

extern void	
connP_make_code_frag_conn(
    ABObj		ab_action,
    ABObj		c_source,
    AB_WHEN		c_when,
    char		*code_frag
)
{
    ab_action->info.action.from = c_source;
    ab_action->info.action.to = NULL;
    ab_action->info.action.when = c_when;
    ab_action->info.action.func_type = AB_FUNC_CODE_FRAG;
    ab_action->info.action.func_value.code_frag = istr_create(code_frag);
}

#define ACT(field) ab_action->info.action.field

extern void
connP_builtin_remove_arg(
    ABObj		ab_action
)
{
    AB_ARG_TYPE	arg_type = ACT(arg_type);

    switch (arg_type)
    {
      case AB_ARG_STRING:
	if (ACT(arg_value.sval) != (ISTRING)NULL)
	{
	    istr_destroy(ACT(arg_value.sval));
	    ACT(arg_value.sval) = (ISTRING)NULL;
	}
    }
}

extern BOOL
connP_change_in_builtin(
    ABObj	ab_action,
    ABObj	c_source,
    ABObj	c_target,
    AB_WHEN	c_when,
    AB_BUILTIN_ACTION	c_act,
    char	*arg_str
)
{
    AB_ARG_TYPE		old_arg_type;
    AB_ARG_TYPE		cur_arg_type;
    BOOL		is_changed = FALSE;

#define CSETQ(lval, rval) \
    if ((lval) != (rval)) \
    { \
        (lval) = (rval); \
        is_changed = TRUE; \
    }

    CSETQ(ACT(from), c_source);
    CSETQ(ACT(to), c_target);
    CSETQ(ACT(when), c_when);
    CSETQ(ab_action->info.action.func_value.builtin, c_act);

    old_arg_type = connP_action_needs_arg(ACT(func_value.builtin));
    cur_arg_type = connP_action_needs_arg(c_act);

    if (old_arg_type != cur_arg_type)
    {
	is_changed = TRUE;
	connP_builtin_remove_arg(ab_action);
	connP_set_conn_arg(ab_action, cur_arg_type, arg_str);
    }
    else
    {
	ISTRING	istr_value;

	switch (cur_arg_type)
	{
	    case AB_ARG_STRING:
		if (arg_str != (char *)NULL)
		    istr_value = istr_create(arg_str);
	    	else
		    istr_value = (ISTRING)NULL;
	    	if (ACT(arg_value.sval) != istr_value)
	    	{
		    if (istr_value != (ISTRING)NULL)
			istr_destroy(istr_value);
		    is_changed = TRUE;
		    connP_builtin_remove_arg(ab_action);
		    connP_set_conn_arg(ab_action, cur_arg_type, arg_str);
	    	}
	    break;

	    case AB_ARG_INT:
		if (!util_strempty(arg_str))
		{
		    CSETQ(ACT(arg_value.ival), atoi(arg_str));
		    connP_set_conn_arg(ab_action, cur_arg_type, arg_str);
		}
	    break;
	}
    }

#undef CSETQ

    return(is_changed);
}

#undef ACT


extern void
connP_destroy_connections_for(
    ABObj	obj
)
{
    ABObj		project;
    AB_TRAVERSAL	trav;
    ABObj		cobj;

    if (obj == NULL ||
	(project = obj_get_project(obj)) == NULL)
	return;


    for (trav_open(&trav, project, AB_TRAV_ACTIONS);
        (cobj = trav_next(&trav)) != NULL; )
    {
	if (cobj->info.action.from == obj	||
	    (cobj->info.action.func_type == AB_FUNC_BUILTIN &&
	     cobj->info.action.to == obj))
	    obj_destroy(cobj);
    }
    trav_close(&trav);
}

extern void
connP_make_on_item_help_conn(
    ABObj               ab_action,
    ABObj               c_source,
    AB_WHEN             c_when
)
{
    ab_action->info.action.from = c_source;
    ab_action->info.action.to = NULL;
    ab_action->info.action.when = c_when;
    ab_action->info.action.func_type = AB_FUNC_ON_ITEM_HELP;
    ab_action->info.action.auto_named = TRUE;
}

extern void
connP_make_help_vol_conn(
    ABObj               ab_action,
    ABObj               c_source,
    AB_WHEN             c_when,
    STRING		volume,
    STRING		location
)
{
    ab_action->info.action.from = c_source;
    ab_action->info.action.to = NULL;
    ab_action->info.action.when = c_when;
    ab_action->info.action.func_type = AB_FUNC_HELP_VOLUME;
    ab_action->info.action.volume_id = istr_create(volume);
    ab_action->info.action.location= istr_create(location);
    ab_action->info.action.auto_named = TRUE;
}

/*
 * Initializes strings used in connections editor
 */
extern void
conn_strings_init(
)
{
      unknown_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 158, "??unknown??"));
      invalid_str = 
	XtNewString(catgets(Dtb_project_catd, 100, 159, "??invalid??"));

      when_labels[AB_WHEN_UNDEF] = unknown_str;
      when_labels[AB_WHEN_ACTION1] =
	XtNewString(catgets(Dtb_project_catd, 100, 160, "Action1 Activated"));
      when_labels[AB_WHEN_ACTION2] =
	XtNewString(catgets(Dtb_project_catd, 100, 161, "Action2 Activated"));
      when_labels[AB_WHEN_ACTION3] =
	XtNewString(catgets(Dtb_project_catd, 100, 162, "Action3 Activated"));
      when_labels[AB_WHEN_ACTIVATED] =
	XtNewString(catgets(Dtb_project_catd, 100, 163, "Activated"));
      when_labels[AB_WHEN_AFTER_CREATED] =
	XtNewString(catgets(Dtb_project_catd, 100, 164, "Created"));
      when_labels[AB_WHEN_AFTER_RESIZED] =
	XtNewString(catgets(Dtb_project_catd, 100, 165, "Resized"));
      when_labels[AB_WHEN_BEFORE_TEXT_CHANGED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 166, "Before Text Changed"));
      when_labels[AB_WHEN_CANCEL] = 
	XtNewString(catgets(Dtb_project_catd, 100, 167, "Cancel Activated"));
      when_labels[AB_WHEN_DESTROYED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 168, "Destroyed"));
      when_labels[AB_WHEN_DOUBLE_CLICKED_ON] =  
	XtNewString(catgets(Dtb_project_catd, 100, 169, "Double Clicked On"));
      when_labels[AB_WHEN_DRAGGED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 170, "Dragged"));
      when_labels[AB_WHEN_DRAGGED_FROM] =  
	XtNewString(catgets(Dtb_project_catd, 100, 171, "Dragged From"));
      when_labels[AB_WHEN_DROPPED_ON] =  
	XtNewString(catgets(Dtb_project_catd, 100, 172, "Dropped On"));
      when_labels[AB_WHEN_HIDDEN] =  
	XtNewString(catgets(Dtb_project_catd, 100, 173, "Hidden"));
      when_labels[AB_WHEN_ITEM_SELECTED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 174, "Item Selected"));
      when_labels[AB_WHEN_OK] =  
	XtNewString(catgets(Dtb_project_catd, 100, 175, "Ok Activated"));
      when_labels[AB_WHEN_POPPED_DOWN] =  
	XtNewString(catgets(Dtb_project_catd, 100, 176, "Popped Down"));
      when_labels[AB_WHEN_POPPED_UP] =  
	XtNewString(catgets(Dtb_project_catd, 100, 177, "Popped Up"));
      when_labels[AB_WHEN_REPAINT_NEEDED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 178, "Repaint Needed"));
      when_labels[AB_WHEN_SESSION_SAVE] =  
	XtNewString(catgets(Dtb_project_catd, 100, 179, "Session Save"));
      when_labels[AB_WHEN_SESSION_RESTORE] =  
	XtNewString(catgets(Dtb_project_catd, 100, 180, "Session Restore"));
      when_labels[AB_WHEN_SHOWN] =  
	XtNewString(catgets(Dtb_project_catd, 100, 181, "Shown"));
      when_labels[AB_WHEN_TEXT_CHANGED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 182, "Text Changed"));
      when_labels[AB_WHEN_TOGGLED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 183, "Toggled"));
      when_labels[AB_WHEN_TOOLTALK_DO_COMMAND] =  
	XtNewString(catgets(Dtb_project_catd, 100, 184, "ToolTalk Do Command"));
      when_labels[AB_WHEN_TOOLTALK_GET_STATUS] =  
	XtNewString(catgets(Dtb_project_catd, 100, 185, "ToolTalk Get Status"));
      when_labels[AB_WHEN_TOOLTALK_PAUSE_RESUME] =  
	XtNewString(catgets(Dtb_project_catd, 100, 186, "ToolTalk Pause/Resume"));
      when_labels[AB_WHEN_TOOLTALK_QUIT] = 
	XtNewString(catgets(Dtb_project_catd, 100, 187, "ToolTalk Quit"));
      when_labels[AB_WHEN_VALUE_CHANGED] =  
	XtNewString(catgets(Dtb_project_catd, 100, 188, "Value Changed"));
      when_labels[AB_WHEN_NUM_VALUES] = invalid_str;

      stdact_labels[AB_STDACT_UNDEF] = unknown_str;
      stdact_labels[AB_STDACT_DISABLE] = 
	XtNewString(catgets(Dtb_project_catd, 100, 189, "Disable"));
      stdact_labels[AB_STDACT_ENABLE] = 
	XtNewString(catgets(Dtb_project_catd, 100, 190, "Enable"));
      stdact_labels[AB_STDACT_HIDE] = 
	XtNewString(catgets(Dtb_project_catd, 100, 191, "Hide"));
      stdact_labels[AB_STDACT_SET_LABEL] = 
	XtNewString(catgets(Dtb_project_catd, 100, 192, "Set Label"));
      stdact_labels[AB_STDACT_SET_TEXT] = 
	XtNewString(catgets(Dtb_project_catd, 100, 193, "Set Text"));
      stdact_labels[AB_STDACT_SET_VALUE] = 
	XtNewString(catgets(Dtb_project_catd, 100, 194, "Set Value"));
      stdact_labels[AB_STDACT_SHOW] = 
	XtNewString(catgets(Dtb_project_catd, 100, 195, "Show"));
      stdact_labels[AB_BUILTIN_ACTION_NUM_VALUES] = invalid_str;
}
