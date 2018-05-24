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
 *	$XConsortium: brws_mthds.c /main/4 1996/07/08 10:43:05 mustafa $
 *
 *	@(#)brws_mthds.c	1.69 29 Apr 1995
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <sys/param.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/istr.h>
#include <ab/util_types.h>
#include <ab_private/vwr.h>
#include <ab_private/pal.h>
#include <ab_private/abobj.h>
#include <ab_private/proj.h>
#include <ab_private/projP.h>
#include <ab_private/brwsP.h>
#include <ab_private/ui_util.h>

#define BRWS_DASH_WIDTH		3
#define BRWS_NUM_DASHES		3

#if !defined(__linux__)
/*
 * Somehow the compiler is not picking up strdup()
 * from string.h properly...
 */
extern char		*strdup();
#endif

/*
 * Misc functions used only in this file
 */
static ABObj		first_module_of_project(
			    ABObj	project
			);

/*
 * Viewer methods
 */
static ViewerNode	*init_vnode(
                            Vwr		b,
                            void	*obj_data
                        );

static ViewerNode	*init_toplevel_node(
                            Vwr		b,
                            void	*obj_data
                        );

static ViewerNode	*init_proj_vnode(
                            Vwr		b,
                            void	*obj_data
                        );

static void		init_glyph_elm(
			    VNode	vnode,
			    VNodeElm	elm,
			    void	*client_data
			);

static void		init_name_elm(
			    VNode	vnode,
			    VNodeElm	elm,
			    void	*client_data
			);

static void		init_class_elm(
			    VNode	vnode,
			    VNodeElm	elm,
			    void	*client_data
			);

static void		init_wclass_elm(
			    VNode	vnode,
			    VNodeElm	elm,
			    void	*client_data
			);

static void		free_str_elm(
			    VNode	vnode,
			    VNodeElm	elm
			);

static void		init_elements(
                            VNode	bnode
                        );

static void		free_elements(
                            VNode	bnode
                        );

static void             free_prop(
			    Vwr	v
			);

static void		free_node(
			    VNode	vnode,
			    void	*obj_data
			);

static void		init_proj_props(
                            Vwr		viewer
                        );

static void		init_mod_props(
                            Vwr		viewer
                        );

static void		free_elements(
                            VNode	bnode
                        );

static int		insert_projview_tree(
    			    Vwr		viewer,
    			    void	*obj_data
			);
static int		insert_proj_tree(
    			    Vwr		viewer,
    			    void	*obj_data
			);

static int		insert_entire_tree(
    			    Vwr		viewer,
    			    void	*obj_data
			);

static VNode		insert_node(
    			    Vwr 	viewer,
    			    void 	*obj_data
			);

static void		remove_tree(
			    Vwr		viewer,
			    void	*obj_data
			);

static void		remove_node(
			    Vwr 	viewer,
			    void	*obj_data
			);

static VNode		get_browser_data(
			    void	*obj_data
			);

static void		set_browser_data(
			    void	*obj_data,
			    VNode	vnode
			);

static VNode		get_projwin_data(
			    void	*obj_data
			);

static void		set_projwin_data(
			    void	*obj_data,
			    VNode	vnode
			);

static ViewerNode	*get_parent(
    			    VNode	bnode
			);

static int		get_num_salient_children(
                            VNode bnode
		        );

static int		get_num_children_of_mod(
    			    VNode	bnode
			);

static int		get_num_children_of_proj(
    			    VNode	bnode
			);

static VNode		get_salient_child(
                            VNode	bnode,
                            int		which_child
                        );

static VNode		get_child_of_mod(
    			    VNode	bnode,
    			    int		which_child
			);

static VNode		get_child_of_proj(
    			    VNode	bnode,
    			    int		which_child
			);

static int		get_num_children_ui(
                            VNode bnode
		        );

static ViewerNode	*get_child_ui(
                            VNode	bnode,
                            int		which_child
                        );

static unsigned long		get_toplevel_drawarea(
				    Vwr		v
				);

static unsigned long		get_detailed_drawarea(
				    Vwr		v
				);

static unsigned long		get_proj_drawarea(
				    Vwr		v
				);

static void		compute_tree(
    			    Vwr		b,
    			    int		*end_x,
    			    int		*end_y
			);

static void		compute_matrix(
    			    Vwr		b,
    			    int		*end_x,
    			    int		*end_y
			);

static void		compute_subtree(
    			    VNode	tree,
    			    int		start_x,
    			    int		start_y,
    			    int		*max_x,
    			    int		*max_y
			);

static void		compute_submatrix(
    			    VNode	tree,
    			    int		start_x,
    			    int		start_y,
    			    int		*max_x,
    			    int		*max_y
			);

static void		compute_glyph_elm(
			    VNode	node,
			    VNodeElm	str_elm,
			    int		x,
			    int		y
			);

static void		compute_str_elm(
			    VNode	node,
			    VNodeElm	str_elm,
			    int		x,
			    int		y
			);

static void		compute_node(
    			    VNode	node,
    			    int		x,
    			    int		y
			);

static void		render_salient_tree(
    			    VNode	tree
			);

static void		render_mod_tree(
    			    VNode	tree
			);

static void		render_proj_tree(
    			    VNode	tree
			);

static void		render_glyph_elm(
    			    VNode	node,
    			    VNodeElm	glyph_elm,
    			    int		hilite
			);

static void		render_str_elm(
    			    VNode	node,
    			    VNodeElm	str_elm,
    			    int		hilite
			);

static void		render_node(
    			    VNode	node,
    			    int		hilite
			);

static void		render_line(
    			    VNode	parent,
    			    VNode	child
			);

static VNode		locate_node(
    			    VNode	tree,
    			    int		x,
    			    int		y
			);

static unsigned long	locate_elements(
    			    VNode	vnode,
    			    int		x,
    			    int		y
			);

static void		render_node_bbox(
                            ViewerNode	*node
                        );

static void		draw_zoom_in(
			    Vwr	 	b,
			    VNode	node
			);

static void		draw_zoom_out(
			    Vwr		b
			);

static void		draw_collapsed_feedback(
			    VNode	node
			);

static void		compute_collapsed_feedback(
			    VNode	node,
			    int		*max_x,
			    int		*max_y
			);


static ViewerMethods proj_view_methods = {
    init_proj_props,		/* init_prop */
    NULL,			/* init_ui */
    init_toplevel_node,		/* init_node */
    init_elements,		/* init_elements */
    free_prop,			/* free_prop */
    NULL,			/* free_ui */
    free_node,			/* free_node */
    free_elements,		/* free_elements */
    insert_projview_tree,	/* insert_tree */
    insert_node,		/* insert_node */
    remove_tree,		/* remove_tree */
    remove_node,		/* remove_node */
    NULL,			/* get_obj_data */
    get_browser_data,		/* get_viewer_data */
    set_browser_data,		/* set_viewer_data */
    NULL,			/* get_sibling */
    get_parent,			/* get_parent */
    get_num_children_of_mod,	/* get_num_children */
    get_child_of_mod,		/* get_child */
    get_toplevel_drawarea,	/* get_drawarea */
    compute_matrix,		/* compute_tree */
    compute_node,		/* compute_node */
    render_mod_tree,		/* render_tree */
    render_node,		/* render_node */
    render_line,		/* render_line */
    NULL,			/* preview_node */
    locate_node,		/* locate_node */
    locate_elements		/* locate_elements */
};

static ViewerMethods mod_view_methods = {
    init_mod_props,		/* init_prop */
    NULL,			/* init_ui */
    init_vnode,			/* init_node */
    init_elements,		/* init_elements */
    free_prop,			/* free_prop */
    NULL,			/* free_ui */
    free_node,			/* free_node */
    free_elements,		/* free_elements */
    insert_entire_tree,		/* insert_tree */
    insert_node,		/* insert_node */
    remove_tree,		/* remove_tree */
    remove_node,		/* remove_node */
    NULL,			/* get_obj_data */
    get_browser_data,		/* get_viewer_data */
    set_browser_data,		/* set_viewer_data */
    NULL,			/* get_sibling */
    get_parent,			/* get_parent */
    get_num_salient_children,	/* get_num_children */
    get_salient_child,		/* get_child */
    get_detailed_drawarea,	/* get_drawarea */
    compute_tree,		/* compute_tree */
    compute_node,		/* compute_node */
    render_salient_tree,	/* render_tree */
    render_node,		/* render_node */
    render_line,		/* render_line */
    NULL,			/* preview_node */
    locate_node,		/* locate_node */
    locate_elements		/* locate_elements */
};

static ViewerMethods proj_methods = {
    init_proj_props,		/* init_prop */
    NULL,			/* init_ui */
    init_proj_vnode,		/* init_node */
    init_elements,		/* init_elements */
    free_prop,			/* free_prop */
    NULL,			/* free_ui */
    free_node,			/* free_node */
    free_elements,		/* free_elements */
    insert_proj_tree,		/* insert_tree */
    insert_node,		/* insert_node */
    remove_tree,		/* remove_tree */
    remove_node,		/* remove_node */
    NULL,			/* get_obj_data */
    get_projwin_data,		/* get_viewer_data */
    set_projwin_data,		/* set_viewer_data */
    NULL,			/* get_sibling */
    get_parent,			/* get_parent */
    get_num_children_of_proj,	/* get_num_children */
    get_child_of_proj,		/* get_child */
    get_proj_drawarea,		/* get_drawarea */
    compute_matrix,		/* compute_tree */
    compute_node,		/* compute_node */
    render_proj_tree,		/* render_tree */
    render_node,		/* render_node */
    render_line,		/* render_line */
    NULL,			/* preview_node */
    locate_node,		/* locate_node */
    locate_elements		/* locate_elements */
};

static ViewerElmMethods glyphElmMethods = {
    init_glyph_elm,
    compute_glyph_elm,
    render_glyph_elm,
    NULL
};

static ViewerElmMethods nameElmMethods = {
    init_name_elm,
    compute_str_elm,
    render_str_elm,
    free_str_elm
};

static ViewerElmMethods classElmMethods = {
    init_class_elm,
    compute_str_elm,
    render_str_elm,
    free_str_elm
};

static ViewerElmMethods wclassElmMethods = {
    init_wclass_elm,
    compute_str_elm,
    render_str_elm,
    free_str_elm
};

static VElmMethods elmMethods[] = {
    &glyphElmMethods,
    &nameElmMethods,
    &classElmMethods,
    &wclassElmMethods
};

VMethods brwsP_proj_methods = &proj_view_methods;
VMethods brwsP_mod_methods = &mod_view_methods;
VMethods projP_methods = &proj_methods;

/*
 * Create, initialize and return a browser node
 */
static ViewerNode	*
init_vnode(
    Viewer	*b,
    void	*obj_data
)
{
    AB_OBJ		*obj = (AB_OBJ *)obj_data;
    ViewerNode	*node, *cur_node;
    BrowserProps	props;
    ViewerMethods	*m;

    if (!b)
	return (NULL);

    props = aob_browser_properties(b);

    node = vwr_create_node();

    node->browser = b;
    node->obj_data = (void *) obj;
    node->elm_methods = elmMethods;
    node->elements = NULL;
    node->num_elements = 0;
    node->boundbox_shown = TRUE;
    node->x = node->y = node->width = node->height = -1;

    /*
     * Use initial state as stored in browser object
     */
    node->state = props->initial_state;

    /*
     * Init node state to:
     *	SELECTED	- to match AB_OBJ's select state
     */
    if (obj_is_selected(obj))
        BRWS_NODE_SET_STATE(node, BRWS_NODE_SELECTED);
    else
        BRWS_NODE_UNSET_STATE(node, BRWS_NODE_SELECTED);

    m = b->methods;
    cur_node = (*m->get_viewer_data)(obj);
    node->next = cur_node;

    /*
     * If nodes for other browsers exist, link them to this new
     * one
     */
    if (cur_node)
	cur_node->previous = node;

    /*
     * Make this new node the first one on the AB_OBJ list
     */
    if (obj)
        (*m->set_viewer_data)(obj, node);

    return node;
}

/*
 * Create, initialize and return a viewer node for toplevel view
 */
static ViewerNode	*
init_toplevel_node(
    Viewer	*b,
    void	*obj_data
)
{
    AB_OBJ		*obj = (AB_OBJ *)obj_data;
    ViewerNode	*node, *cur_node;
    BrowserProps	props;
    ViewerMethods	*m;

    if (!b)
	return (NULL);

    props = aob_browser_properties(b);

    node = vwr_create_node();

    node->browser = b;
    node->obj_data = (void *) obj;
    node->elm_methods = elmMethods;
    node->elements = NULL;
    node->num_elements = 0;
    node->boundbox_shown = TRUE;
    node->x = node->y = node->width = node->height = -1;

    /*
     * Use initial state as stored in browser object
     */
    node->state = props->initial_state;

    /*
     * Do not initialize node select state
     * This is taken care of by initial state
     */

    m = b->methods;
    cur_node = (*m->get_viewer_data)(obj);
    node->next = cur_node;

    /*
     * If nodes for other browsers exist, link them to this new
     * one
     */
    if (cur_node)
	cur_node->previous = node;

    /*
     * Make this new node the first one on the AB_OBJ list
     */
    if (obj)
        (*m->set_viewer_data)(obj, node);

    return node;
}

/*
 * Create, initialize and return a vnode for the project window
 */
static ViewerNode	*
init_proj_vnode(
    Viewer	*b,
    void	*obj_data
)
{
    AB_OBJ		*obj = (AB_OBJ *)obj_data;
    ViewerNode	*node, *cur_node;
    BrowserProps	props;
    ViewerMethods	*m;

    if (!b)
	return (NULL);

    m = b->methods;

    /*
     * If a projwin vnode already exists, return it.
     */
    if (cur_node = (*m->get_viewer_data)(obj))
	return (cur_node);

    props = aob_browser_properties(b);

    node = vwr_create_node();

    node->browser = b;
    node->obj_data = (void *) obj;
    node->elm_methods = elmMethods;
    node->elements = NULL;
    node->num_elements = 0;
    node->boundbox_shown = TRUE;
    node->x = node->y = node->width = node->height = -1;
    node->next = NULL;

    /*
     * Use initial state as stored in browser object
     */
    node->state = props->initial_state;

    BRWS_NODE_UNSET_STATE(node, BRWS_NODE_SELECTED);

    /*
     * Make this new node the first one on the AB_OBJ list
     */
    if (obj)
        (*m->set_viewer_data)(obj, node);

    return node;
}

static void
init_glyph_elm(
    VNode	vnode,
    VNodeElm	elm,
    void	*client_data
)
{
    AB_OBJ		*obj = (AB_OBJ *)client_data;
    Pixmap		image = 0;

    /*
     * Object glyph
     */
    ui_get_obj_pixmap(obj, &image, &elm->width, &elm->height);
    elm->data = (void *)image;
    elm->x = elm->y = -1;
}

static void
init_name_elm(
    VNode	vnode,
    VNodeElm	elm,
    void	*client_data
)
{
    AB_OBJ		*obj = (AB_OBJ *)client_data;
    char		*tmp;

    /*
     * Object name
     */
    if (elm->data)
	free(elm->data);
#ifdef BRWS_NO_FILE_NAMES
    if (obj_is_module(obj) && (obj->info.module.file != NULL))
    {
	char	*fullpath, *filename;

	fullpath = obj_get_file(obj);

        /*
         * Check return value of strrchr before adding 1 to it
         */
	if (filename = strrchr(fullpath, '/'))
	    tmp = strdup(filename + 1);
	else
	    tmp = strdup(fullpath);
    }
    else
#else
        tmp = strdup(util_strsafe(obj_get_name(obj)));
#endif /* BRWS_NO_FILE_NAMES */
    elm->data = (void *)tmp;
    elm->x = elm->y = elm->width = elm->height = -1;
}

static void
init_class_elm(
    VNode	vnode,
    VNodeElm	elm,
    void	*client_data
)
{
    PalEditableObjInfo	*editable_obj_info = NULL;
    AB_OBJ		*obj = (AB_OBJ *)client_data;
    int			subtype;
    char		*tmp = NULL;

    if (!elm || !obj)
	return;

    /*
     * Free Object class/type string if it exists
     */
    if (elm->data)
	free(elm->data);

    /*
     * Get subtype
     */
    subtype = obj_get_subtype(obj);

    /*
     * However, if the object is a scale (i.e. either scale/gauge),
     * the subtype field is not used, instead, it's read-only state
     * is used.
     */
    if (obj_is_scale(obj))
    {
	BOOL	read_only = obj_get_read_only(obj);

	/*
	 * Scale: read_only == False
	 * Gauge: read_only == True
	 */
	subtype = (int)read_only;
    }

    /*
     * The type strings can be obtained from various places.
     * We should centralize this sometime...
     */

    /*
     * If the object in question is a module, just hardcode the string
     * since there is no palette info on modules.
     * Note: Object type strings are NOT internationalized, so no problem
     * here.
     */
    if (obj_is_module(obj))
    {
	tmp = "Module";
    }

    /*
     * Hardcode string for layers object.
     * Layers are not palette objects and they are not editable either.
     */
    if (!tmp && obj_is_layers(obj))
    {
	tmp = "Layers";
    }

    /*
     * If the object is an item/message object, get the string from
     * the palette editable object information.
     */
    if (!tmp && (obj_is_item(obj) || obj_is_message(obj)))
    {
        editable_obj_info = pal_get_editable_obj_info(obj);

        if (editable_obj_info && editable_obj_info->name)
	    tmp = istr_string(editable_obj_info->name);
    }

    /*
     * Still no string up to now.
     * Use the palette item info. pal_get_item_subname()
     * will return the subtype (e.g. Drawn Button) string
     * if it exists for the object type.
     */
    if (!tmp)
    {
        tmp = pal_get_item_subname(obj, subtype);
    }

    /*
     * No subtype string.
     * Return the type string.
     */
    if (!tmp)
    {
        PalItemInfo		*pal_info;
        pal_info = pal_get_item_info(obj);
	if (pal_info)
	    tmp = pal_info->name;
    }

    /*
     * This is the 'old' lookup table with all uppercase strings.
     */
    if (!tmp)
        tmp = util_object_type_to_browser_string(obj->type);

    tmp = strdup(tmp);

    elm->data = (void *)tmp;
    elm->x = elm->y = elm->width = elm->height = -1;
}

static void
init_wclass_elm(
    VNode	vnode,
    VNodeElm	elm,
    void	*client_data
)
{
    AB_OBJ		*obj = (AB_OBJ *)client_data;
    char		*tmp;

    /*
     * Widget class
     */
    if (elm->data)
	free(elm->data);
    tmp = strdup(util_strsafe(obj_get_class_name(obj)));
    elm->data = (void *)tmp;
    elm->x = elm->y = elm->width = elm->height = -1;
}

static void
free_str_elm(
    VNode	vnode,
    VNodeElm	elm
)
{
    /*
     * Free string element
     */
    if (elm->data)
    {
	free(elm->data);
	elm->data = NULL;
    }
    elm->x = elm->y = elm->width = elm->height = -1;
}


static void
init_elements(
    ViewerNode	*vnode
)
{
    AB_OBJ		*obj;
    ViewerNodeElm	*elm;
    VElmMethods		*elm_methods;
    int			i;


    if (!vnode)
	return;

    if (!vnode->elements)
    {
	elm = (ViewerNodeElm *)calloc(BRWS_NUM_ELM, sizeof(ViewerNodeElm));
        vnode->num_elements = BRWS_NUM_ELM;
	vnode->elements = elm;

	for (i=0; i < vnode->num_elements; ++i)
	{
	    elm[i].data = NULL;
            elm[i].x = elm[0].y =
		elm[i].width = elm[i].height = -1;
	}
    }
    else
	elm = vnode->elements;

    obj = (AB_OBJ *)vnode->obj_data;
    elm_methods = vnode->elm_methods;

    for (i=0; i < vnode->num_elements; ++i)
    {
        (*(elm_methods[i]->init))(vnode, &elm[i], (void *)obj);
    }
}

/*
 * Free structure that holds browser properties
 */
static void
free_prop(
    Vwr	v
)
{
    if (!v && !v->properties)
	return;

    free(v->properties);

    v->properties = NULL;
}

/*
 * The obj_data parameter is not really necessary since
 * it hangs off the vnode, but I did not want to assume that
 * is the case for all users of the viewer node
 */
static void
free_node(
    VNode	vnode,
    void	*obj_data
)
{
    ABObj	obj = (ABObj)obj_data;
    Vwr		viewer;
    VNode	prev,
		next;
    VMethods	m;
    int		i;

    if (!vnode || !obj_data)
	return;

    viewer = vnode->browser;
    m = viewer ? viewer->methods : NULL;

    if (!m)
	return;

    prev = vnode->previous;
    next = vnode->next;

    if (prev)
    {
        prev->next = vnode->next;
    }
    else
    {
        (*m->set_viewer_data)(obj, vnode->next);
    }

    if (next)
    {
        next->previous = vnode->previous;
    }

    free(vnode);
}

static void
free_elements(
    ViewerNode	*vnode
)
{
    AB_OBJ		*obj;
    ViewerNodeElm	*elm;
    VElmMethods		*elm_methods;
    int			i;


    if (!vnode || !vnode->elm_methods ||
		!vnode->elements || !vnode->elements)
	return;

    obj = (AB_OBJ *)vnode->obj_data;
    elm = vnode->elements;
    elm_methods = vnode->elm_methods;

    for (i=0; i < vnode->num_elements; ++i)
    {
	if (elm_methods[i]->free)
            (*(elm_methods[i]->free))(vnode, &elm[i]);
    }

    free(elm);

    vnode->elements = NULL;
    vnode->num_elements = 0;
}

static void
init_proj_props(
    Vwr		viewer
)
{
    BrowserProps	props;

    props = (BrowserProps)malloc(sizeof(BrowserProperties));

    /*
     * Possible values for bit vector:
     *	BRWS_SHOW_TYPE, BRWS_SHOW_NAME, BRWS_SHOW_WIDGET_CLASS, BRWS_SHOW_GLYPH
     */
    props->elements_shown = BRWS_SHOW_NAME | BRWS_SHOW_GLYPH;
    props->initial_state = BRWS_NODE_EXPANDED | BRWS_NODE_VISIBLE;
    props->orientation = BRWS_HORIZONTAL;
    props->show_mult_trees = FALSE;

    /*
     * This flag is currently used to determine if feedback
     * is needed for the browser when say someone selects an
     * object on an module or on another browser.
     */
    props->active = FALSE;

    viewer->properties = (void *)props;
}

static void
init_mod_props(
    Vwr		viewer
)
{
    BrowserProps	props;

    props = (BrowserProps)malloc(sizeof(BrowserProperties));

    /*
     * Possible values for bit vector:
     *	BRWS_SHOW_TYPE, BRWS_SHOW_NAME, BRWS_SHOW_WIDGET_CLASS, BRWS_SHOW_GLYPH
     */
    props->elements_shown = BRWS_SHOW_GLYPH | BRWS_SHOW_NAME;
    props->initial_state = BRWS_NODE_EXPANDED | BRWS_NODE_VISIBLE;
    props->orientation = BRWS_VERTICAL;
    props->show_mult_trees = FALSE;
    props->active = TRUE;

    viewer->properties = (void *)props;
}

static ABObj
first_module_of_project(
    ABObj	project
)
{
    AB_TRAVERSAL	trav;
    ABObj		first_mod = NULL;

    if (!project || !obj_is_project(project))
	return (NULL);

    trav_open(&trav, project, AB_TRAV_MODULES);
    first_mod = trav_next(&trav);
    trav_close(&trav);

    return first_mod;
}


/*
 * insert_projview_tree()
 * insert_tree() method for module view of browser.
 *
 * This insert_tree() method only inserts nodes into
 * modules, and immediate children of modules. The current
 * module the browser is currently browsing is stored in:
 *	viewer->obj_data
 *
 * The obj_data passed can be any type of AB_OBJ. If the obj_data
 * passed is not in the current module browsed, the browser nodes of
 * current module browsed is deleted and the module containing the
 * obj_data passed is inserted.
 *
 *
 *
 */
static int
insert_projview_tree
(
    Vwr		viewer,
    void	*obj_data
)
{
    ABObj		root = (ABObj)obj_data,
    			browsed_module = NULL,
        		insert_module = NULL,
    			cur_obj;
    Vwr			b_list;
    AB_TRAVERSAL	trav;
    VNode		bnode,
			module_node;
    ViewerMethods	*m;

    if ((!viewer) || (!root))
	return(0);

    /*
     * This function only insert nodes into modules, and immediate children
     * of modules.
     *
     * NOTE: If an object passed in is not a project/module/window, we
     * can get an infinite loop. This is avoided by trav'ing on the modules
     * with AB_TRAV_WINDOWS.
     */
    if (!obj_is_project(root) && !obj_is_module(root)
	&& !obj_is_window(root) && !obj_is_menu(root) && !obj_is_message(root))
    {
	root = obj_get_module(root);
    }

    /*
     * Get methods vector
     */
    m = viewer->methods;

    /*
     * Get module that this browser is currently browsing...
     */
    if (viewer->tree)
        browsed_module = (AB_OBJ *)viewer->tree->obj_data;

    /*
     * If a project is passed in, this function is called
     * recursively with the appropriate module.
     */
    if (obj_is_project(root))
    {

	/*
	 * The module to insert depends on whether this browser
	 * is already being used to browse an existing module or
	 * not.
	 */
	if (!browsed_module)
	{
	    /*
	     * Currently not browsing anything. This browser will
	     * browse the first module of the project
	     */
	    insert_module = first_module_of_project(root);
	}
	else
	{
	    /*
	     * This browser is currently being used. Check if the
	     * module passed is the same one. If yes, re-insert it.
	     * Otherwise, insert nothing.
	     * Re-inserting will only have an effect if there are new
	     * objects in the tree that the browser does not know
	     * about yet.
	     */
	    if (root == obj_get_project(browsed_module))
	    {
		insert_module = browsed_module;
	    }
	}

	/*
	 * Recursive call with proper module to insert
	 */
	insert_projview_tree(viewer, insert_module);

	return(0);
    }

    if (obj_is_module(root))
    {
	if (browsed_module != root)
	{
	    (*m->remove_tree)(viewer, browsed_module);
	    viewer->tree = viewer->current_tree = NULL;
            viewer->obj_data = (void *)NULL;
	}

        bnode = (*m->insert_node)(viewer, root);

	viewer->tree = viewer->current_tree = bnode;
        viewer->obj_data = (void *)obj_get_project(root);

	/*
	 * Insert all windows of this module, skip
	 * actions
	 */
        for (trav_open(&trav, root, AB_TRAV_WINDOWS);
	    (cur_obj = trav_next(&trav)) != NULL; )
	{
	    insert_projview_tree(viewer, cur_obj);
        }
        trav_close(&trav);

	/*
	 * Insert all menus of this module.
	 */
        for (trav_open(&trav, root, AB_TRAV_MENUS);
	    (cur_obj = trav_next(&trav)) != NULL; )  {
	    insert_projview_tree(viewer, cur_obj);
        }
        trav_close(&trav);

	/*
	 * Insert all message objects of this module.
	 */
        for (trav_open(&trav, root, AB_TRAV_CHILDREN);
	    (cur_obj = trav_next(&trav)) != NULL; )
	{
	    if (obj_is_message(cur_obj))
	        insert_projview_tree(viewer, cur_obj);
        }
        trav_close(&trav);

	brws_set_module_name(viewer);

	return (0);
    }

    /*
     * The object is something below a module
     * Check:
     *	1. Is this object in the same module as the currently
     *	   browsed module ('browsed_module') ?
     *	2. Has the viewer node been inserted into the module for
     *	   this object ?
     *
     * In either case, we want to insert the module.
     */
    insert_module = obj_get_module(root);

    module_node = aob_find_bnode(insert_module, viewer);

    if ((insert_module != browsed_module) || (!module_node))
    {
        insert_projview_tree(viewer, insert_module);

	return (0);
    }

    /*
     * Attach viewer node onto object
     */
    bnode = (*m->insert_node)(viewer, root);

    return (0);
}

static int
insert_proj_tree
(
    Vwr		viewer,
    void	*obj_data
)
{
    AB_OBJ		*root = (AB_OBJ *)obj_data;
    Vwr			b_list;
    AB_TRAVERSAL	trav;
    AB_OBJ		*cur_obj;
    VNode		bnode;
    ViewerMethods	*m;

    if (!viewer || !root)
	return(0);

    /*
     * This routine does not insert viewer nodes into anything
     * below a module. This checks for that.
     */
    if (!obj_is_project(root) && !obj_is_module(root))
    {
	root = obj_get_module(root);

	if (!root || !obj_is_defined(root))
	    return(0);
    }

    m = viewer->methods;
    bnode = (*m->insert_node)(viewer, root);

    if (obj_is_project(root))
    {
	viewer->tree = viewer->current_tree = bnode;
        viewer->obj_data = (void *)root;

        for (trav_open(&trav, root, AB_TRAV_MODULES);
	    (cur_obj = trav_next(&trav)) != NULL; )
	{
	    /* Don't show any undefined modules.
	     * Undefined modules can occur if a module
	     * is imported which references another
	     * module that does not exist in the project
	     * (i.e. :win-children).
	     */
	    if (obj_is_defined(cur_obj))
	    {
		insert_proj_tree(viewer, cur_obj);
	    }
        }
    }

    return (0);
}



static int
insert_entire_tree
(
    Viewer	*viewer,
    void	*obj_data
)
{
    ABObj		root = (ABObj)obj_data,
    			browsed_module = NULL,
    			insert_module = NULL,
    			cur_obj;
    Viewer		*b_list;
    VNode		bnode,
			module_node;
    ViewerMethods	*m;
    AB_TRAVERSAL	trav;

    if (!viewer || !root)
	return(0);

    /*
     * Get methods vector
     */
    m = viewer->methods;

    /*
     * Get module that this browser is currently browsing...
     */
    if (viewer->tree)
        browsed_module = (ABObj)viewer->tree->obj_data;

    /*
     * If a project is passed in, this function is called
     * recursively with the appropriate module.
     */
    if (obj_is_project(root))
    {

	/*
	 * The module to insert depends on whether this browser
	 * is already being used to browse an existing module or
	 * not.
	 */
	if (!browsed_module)
	{
	    /*
	     * Currently not browsing anything. This browser will
	     * browse the first module of the project
	     */
	    insert_module = first_module_of_project(root);
	}
	else
	{
	    /*
	     * This browser is currently being used. Check if the
	     * module passed is the same one. If yes, re-insert it.
	     * Otherwise, insert nothing.
	     * Re-inserting will only have an effect if there are new
	     * objects in the tree that the browser does not know
	     * about yet.
	     */
	    if (root == obj_get_project(browsed_module))
	    {
		insert_module = browsed_module;
	    }
	}

	/*
	 * Recursive call with proper module to insert
	 */
	insert_entire_tree(viewer, insert_module);

	return(0);
    }

    if (obj_is_module(root))
    {
	if (browsed_module != root)
	{
	    (*m->remove_tree)(viewer, browsed_module);
	    viewer->tree = viewer->current_tree = NULL;
	    viewer->obj_data = (void *)NULL;
	}

        bnode = (*m->insert_node)(viewer, root);

	viewer->tree = viewer->current_tree = bnode;
        viewer->obj_data = (void *)obj_get_project(root);
    }
    else
    {
        /*
         * The object is something below a module
         * Check:
         *	1. Is this object in the same module as the currently
         *	   browsed module ('browsed_module') ?
         *	2. Has the viewer node been inserted into the module for
         *	   this object ?
         *
         * In either case, we want to insert the module.
         */
        insert_module = obj_get_module(root);

        module_node = aob_find_bnode(insert_module, viewer);

        if ((insert_module != browsed_module) || (!module_node))
        {
            insert_entire_tree(viewer, insert_module);

	    return (0);
        }

        bnode = (*m->insert_node)(viewer, root);
    }

    /*
     * Insert viewer nodes into all AB_OBJ's, even though we only
     * need salient ones. We will need the non-salient ones later,
     * e.g. for showing widget classes etc...
     *
     * NOTE: AB_TRAV_UI will not work here since a trav of AB_TRAV_UI
     * on a module will return the module first. This will cause
     * an infinite loop.
     */
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
	(cur_obj = trav_next(&trav)) != NULL; )  {
	insert_entire_tree(viewer, cur_obj);
    }

    return (0);
}


/*
 * Insert an object to the object browser.
 */
static VNode
insert_node(
    Vwr 	viewer,
    void 	*obj_data
)
{
    AB_OBJ 		*obj = (AB_OBJ *)obj_data;
    ViewerMethods	*m;
    ViewerNode		*node;

    if (!viewer || !obj)
	return (NULL);

    m = viewer->methods;

    node = aob_find_bnode(obj, viewer);

    if (!node)
    {
	node = (*m->init_node)(viewer, obj);
    }
    else
        return (node);

    (*m->init_elements)(node);

    return (node);

}


static void
remove_tree
(
    Vwr		viewer,
    void	*obj_data
)
{
    AB_OBJ		*root = (AB_OBJ *)obj_data;
    Vwr			b_list;
    AB_TRAVERSAL	trav;
    AB_OBJ		*cur_obj;
    ViewerMethods	*m;

    if (!viewer || !root)
	return;

    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
	(cur_obj = trav_next(&trav)) != NULL; )  {
	remove_tree(viewer, cur_obj);
    }

    m = viewer->methods;
    (*m->remove_node)(viewer, root);
}

/*
 * Delete an object from the object browser.
 */
static void
remove_node(
    Vwr 	viewer,
    void	*obj_data
)
{
    AB_OBJ 		*obj = (AB_OBJ *)obj_data;
    ViewerMethods	*m;
    VNode		node;

    if (!viewer || !obj)
	return;

    m = viewer->methods;

    node = aob_find_bnode(obj, viewer);

    if (node)
    {
	/*
	 * Reset current_tree if the node it is pointing to is destroyed
	 * NOTE: This does not catch the case where a node *between*
	 * viewer->tree and viewer->current_tree is removed.
	 */
	if (viewer->current_tree == node)
	    viewer->current_tree = viewer->tree;

	if (viewer->tree == node)
	    viewer->tree = viewer->current_tree = NULL;

	/*
	 * Free elements of node
	 */
	(*m->free_elements)(node);

	/*
	 * Free node
	 */
	(*m->free_node)(node, obj);
    }
}


static VNode
get_browser_data(
    void	*obj_data
)
{
    AB_OBJ	*obj = (AB_OBJ *)obj_data;

    return (VNode)(obj->browser_data);
}

static void
set_browser_data(
    void	*obj_data,
    VNode	vnode
)
{
    AB_OBJ	*obj = (AB_OBJ *)obj_data;

    obj->browser_data = (void *)vnode;
}

static VNode
get_projwin_data(
    void	*obj_data
)
{
    AB_OBJ	*obj = (AB_OBJ *)obj_data;

    return (VNode)(obj->projwin_data);
}

static void
set_projwin_data(
    void	*obj_data,
    VNode	vnode
)
{
    AB_OBJ	*obj = (AB_OBJ *)obj_data;

    obj->projwin_data = (void *)vnode;
}


static ViewerNode *
get_parent
(
    ViewerNode	*bnode
)
{
    AB_OBJ	*obj, *parent_obj, *tmp_parent;
    Viewer	*browser;

    if (!bnode)
	return(NULL);

    obj = (AB_OBJ *)bnode->obj_data;

    if (!obj)
	return(NULL);

    tmp_parent = obj_get_parent(obj);
    parent_obj = tmp_parent ? obj_get_root(tmp_parent) : NULL;

    browser = bnode->browser;

    return(aob_find_bnode(parent_obj, browser));
}

static int
get_num_children_of_proj
(
    ViewerNode	*bnode
)
{
    AB_OBJ	*obj;

    if (!bnode)
	return(-1);

    obj = (AB_OBJ *)bnode->obj_data;

    if (obj_is_project(obj))
        return(obj_get_num_children_cond(obj, obj_is_defined_module));

    return (0);
}

static int
get_num_children_of_mod
(
    ViewerNode	*bnode
)
{
    AB_OBJ	*obj;

    if (!bnode)
	return(-1);

    obj = (AB_OBJ *)bnode->obj_data;

    if (obj_is_module(obj))
        return(obj_get_num_children(obj));

    return (0);
}

static int
get_num_salient_children
(
    ViewerNode	*bnode
)
{
    AB_OBJ	*obj;

    if (!bnode)
	return(-1);

    obj = (AB_OBJ *)bnode->obj_data;

    /*
    return(obj_get_num_children(obj));
    */
    return(obj_get_num_salient_children(obj));
}

static ViewerNode *
get_child_of_mod
(
    ViewerNode	*bnode,
    int			which_child
)
{
    AB_OBJ		*obj,
			*child;
    Viewer		*browser;
    ViewerNode	*child_bnode;

    if (!bnode)
	return(NULL);

    obj = (AB_OBJ *)bnode->obj_data;
    browser = bnode->browser;

    if (!obj || !browser || !obj_is_module(obj))
	return (NULL);

    child = obj_get_child(obj, which_child);

    child_bnode = aob_find_bnode(child, browser);

    return(child_bnode);
}

static ViewerNode *
get_child_of_proj
(
    ViewerNode	*bnode,
    int			which_child
)
{
    AB_OBJ		*obj,
			*child;
    Viewer		*browser;
    ViewerNode	*child_bnode;

    if (!bnode)
	return(NULL);

    obj = (AB_OBJ *)bnode->obj_data;
    browser = bnode->browser;

    if (!obj || !browser || !obj_is_project(obj))
	return (NULL);

    child = obj_get_child_cond(obj, which_child, obj_is_defined_module);

    child_bnode = aob_find_bnode(child, browser);

    return(child_bnode);
}

static ViewerNode *
get_salient_child(
    ViewerNode	*bnode,
    int			which_child
)
{
    AB_OBJ		*obj,
			*child;
    Viewer		*browser;
    ViewerNode	*child_bnode;

    if (!bnode)
	return(NULL);

    obj = (AB_OBJ *)bnode->obj_data;
    browser = bnode->browser;

    if (!obj || !browser)
	return (NULL);

    /*
    child = obj_get_child(obj, which_child);
    */
    child = obj_get_salient_child(obj, which_child);

    child_bnode = aob_find_bnode(child, browser);

    return(child_bnode);
}

static BOOL
has_ui
(
    AB_OBJ	*obj
)
{
    if (obj_is_project(obj) || obj_is_module(obj))
        return(True);

    if (obj->ui_handle)
        return(True);

    return(False);
}

static int
get_num_children_ui
(
    ViewerNode	*bnode
)
{
    AB_OBJ	*obj;

    if (!bnode)
	return(-1);

    obj = (AB_OBJ *)bnode->obj_data;

    return(obj_get_num_children_cond(obj, has_ui));
}

static ViewerNode *
get_child_ui(
    ViewerNode	*bnode,
    int			which_child
)
{
    AB_OBJ		*obj,
			*child;
    Viewer		*browser;
    ViewerNode	*child_bnode = NULL;

    if (!bnode)
	return(NULL);

    obj = (AB_OBJ *)bnode->obj_data;
    browser = bnode->browser;

    if (!obj || !browser)
	return (NULL);

    child = obj_get_child_cond(obj, which_child, has_ui);

    child_bnode = aob_find_bnode(child, browser);

    return(child_bnode);
}

/*
 * Apply the (x, y) offset to all the elements of the passed
 * node
 */
static void
node_elm_offset
(
    ViewerNode	*node,
    int			x,
    int			y
)
{
    Viewer		*b;
    ViewerNodeElm	*elm;
    int			elm_shown;
    int			i;

    b = node->browser;
    elm_shown = browser_get_elm_shown(b);
    elm = node->elements;

    for (i=0; i < node->num_elements; ++i)
    {
        if (elm_shown | (1L << i))
	{
            elm[i].x += x;
            elm[i].y += y;
	}
    }
}


static unsigned long
get_toplevel_drawarea(
    Vwr		v
)
{
    DtbBrwsMainwindowInfo	instance;
    BrowserUiObj	ui;

    if (!v)
	return 0;

    ui = (BrowserUiObj)v->ui_handle;

    if (!ui)
	return 0;

    instance = (DtbBrwsMainwindowInfo)ui->ip;

    return (instance ? (unsigned long)instance->toplevel_drawarea :
		(unsigned long)NULL);
}

static unsigned long
get_detailed_drawarea(
    Vwr		v
)
{
    DtbBrwsMainwindowInfo	instance;
    BrowserUiObj	ui;

    if (!v)
	return 0;

    ui = (BrowserUiObj)v->ui_handle;

    if (!ui)
	return 0;

    instance = (DtbBrwsMainwindowInfo)(ui->ip);

    return (instance ? (unsigned long)instance->detailed_drawarea :
		(unsigned long)NULL);
}

static unsigned long
get_proj_drawarea(
    Vwr		v
)
{
    DtbProjProjMainInfo	proj_d;
    BrowserUiObj	ui;

    if (!v)
	return 0;

    ui = (BrowserUiObj)v->ui_handle;

    if (!ui)
	return 0;

    proj_d = (DtbProjProjMainInfo)ui->ip;

    return (proj_d ?
	(unsigned long)proj_d->module_drawarea
	: (unsigned long)NULL);
}

/*
 * Compute the coordinates of each node in the tree.
 * It does not draw the tree.
 */
static void
compute_tree
(
    Vwr			b,
    int			*end_x,
    int			*end_y
)
{
    VNode		top;
    AB_OBJ		*obj;
    int			i, num_child,
    			max_x = BRWS_X_ORIGIN,
			max_y = BRWS_Y_ORIGIN;

    if (!b || !b->current_tree)
        return;

    top = b->current_tree;

    if (!top)
	return;

    if (!BRWS_NODE_STATE_IS_SET(top, BRWS_NODE_VISIBLE))
	return;

    obj = (AB_OBJ *)top->obj_data;

    if (!obj)
	return;

    if (BRWS_NODE_STATE_IS_SET(top, BRWS_NODE_VISIBLE))
        compute_subtree(top, BRWS_X_ORIGIN, BRWS_Y_ORIGIN, end_x, end_y);
}


/*
 * Compute the coordinates of each node in the tree.
 * It does not draw the tree.
 */
static void
compute_matrix
(
    Vwr			b,
    int			*end_x,
    int			*end_y
)
{
    VNode		top;
    AB_OBJ		*obj;
    int			i, num_child,
    			max_x = BRWS_X_ORIGIN,
			max_y = BRWS_Y_ORIGIN;
    Widget		draw_area;
    XRectangle		w_rect;/* widget width,height,x,y */

    if (!b || !b->current_tree)
        return;

    top = b->current_tree;

    if (!top)
	return;

    obj = (AB_OBJ *)top->obj_data;

    if (!obj)
	return;

    draw_area = brws_draw_area(b);
    XtVaGetValues(draw_area,
            	XtNwidth,       &(w_rect.width),
            	XtNheight,      &(w_rect.height),
		NULL);

    *end_x = w_rect.width;
    *end_y = w_rect.height;

    compute_submatrix(top, BRWS_X_ORIGIN, BRWS_Y_ORIGIN,
			end_x, end_y);
}


/*
 * Compute the coordinates of each node in the subtree.
 * It does not draw the tree.
 */
static void
compute_subtree
(
    ViewerNode	*tree,
    int			start_x,
    int			start_y,
    int			*max_x,
    int			*max_y
)
{
    ViewerMethods	*m;
    AB_OBJ		*obj;
    Viewer		*b;
    int			i,
			tmp,
			num_child;

    if (!tree)
        return;

    obj = (AB_OBJ *)tree->obj_data;
    b = tree->browser;
    m = b->methods;

    /*
     * Get number of children
     */
    num_child = (*m->get_num_children)(tree);

    /*
     * If tree is childless.
     */
    if ((num_child == 0) || !(BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_EXPANDED)))
    {
        /*
         * Store start_x and start_y into the node.
         */
        (*m->compute_node)(tree, start_x, start_y);

        /*
         * Compute max_x and max_y which will be
         * used in subsequent calls to draw other
         * parts of the tree.
         */
        tmp = start_x + tree->width + BRWS_NODE_X_GAP;
        if (tmp > *max_x)
            *max_x = tmp;

        /*
         * Takes into consideration the icon height and
         * the height needed to display the name of the obj.
         */
        tmp = start_y + tree->height + BRWS_NODE_Y_GAP;
        if (tmp > *max_y)
            *max_y = tmp;

	if (!(BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_EXPANDED)))
	{
	    compute_collapsed_feedback(tree, max_x, max_y);
	}
    }
    else
    {
        ViewerNode	*child;
	BrowserProperties	*props;

	props = aob_browser_properties(b);

        /*
         * Place the parent mid-way in between its
         * children.
         */
        if (props->orientation == BRWS_VERTICAL)
        {
            (*m->compute_node)(tree, 0, start_y);
            tmp = start_y + tree->height + BRWS_NODE_Y_GAP;

            /*
             * Recursively compute the children of this node.
	     * Compute procs should check for (child == NULL)
             */
            for (i=0, child = (*m->get_child)(tree, 0);
                        (i < num_child);
                        child = (*m->get_child)(tree, ++i))
                if (child && BRWS_NODE_STATE_IS_SET(child, BRWS_NODE_VISIBLE))
                    compute_subtree(child, *max_x, tmp, max_x, max_y);

            tmp = (*max_x - start_x - BRWS_NODE_X_GAP - tree->width)/2 + start_x;
            if (tmp < start_x)
            {
                tree->x = start_x;
                tmp = start_x + tree->width + BRWS_NODE_X_GAP;
            }
            else
            {
                tree->x = tmp;
                tmp = tmp + tree->width + BRWS_NODE_X_GAP;
            }

            node_elm_offset(tree, tree->x, 0);

            if (tmp > *max_x)
                *max_x = tmp;

	    /*
	     * If for some reason the the max_y (i.e. height) of the entire
	     * subtree was not incremented (error in compute_node of child),
	     * make it big enough for this node.
	     */
            tmp = start_y + tree->height + BRWS_NODE_Y_GAP;
            if (*max_y < tmp)
                *max_y = tmp;
        }
        else
        {
            (*m->compute_node)(tree, start_x, 0);
            tmp = start_x + tree->width + BRWS_NODE_X_GAP;

            /*
             * Recursively draw the children of this node.
	     * Compute procs should check for (child == NULL)
             */
            for (i=0, child = (*m->get_child)(tree, 0);
                        (i < num_child);
                        child = (*m->get_child)(tree, ++i))
                if (child && BRWS_NODE_STATE_IS_SET(child, BRWS_NODE_VISIBLE))
                    compute_subtree(child, tmp, *max_y, max_x, max_y);

            tmp = (*max_y - start_y - BRWS_NODE_Y_GAP - tree->height)/2 + start_y;
            if (tmp < start_y)
            {
                tree->y = start_y;
                tmp = start_y + tree->height + BRWS_NODE_Y_GAP;
            }
            else
            {
                tree->y = tmp;
                tmp = tmp + tree->height + BRWS_NODE_Y_GAP;
            }

            node_elm_offset(tree, 0, tree->y);

            if (tmp > *max_y)
                *max_y = tmp;

	    /*
	     * If for some reason the the max_x (i.e. width) of the entire
	     * subtree was not incremented (error in compute_node of child),
	     * make it big enough for this node.
	     */
            tmp = start_x + tree->width + BRWS_NODE_X_GAP;
	    if (*max_x < tmp)
		*max_x = tmp;
        }
    }
}


/*
 * Compute the coordinates of each node in the subtree.
 * It does not draw the tree.
 */
static void
compute_submatrix
(
    ViewerNode	*tree,
    int			start_x,
    int			start_y,
    int			*max_x,
    int			*max_y
)
{
    ViewerMethods	*m;
    AB_OBJ		*obj;
    Vwr			b;
    VNode		child;
    BrowserProps	props;
    int			i,
			x = start_x,
			y = start_y,
			num_child,
			col_max_width = 0,
			row_max_height = 0;

    if (!tree)
        return;

    obj = (AB_OBJ *)tree->obj_data;
    b = tree->browser;
    m = b->methods;
    props = aob_browser_properties(b);

    /*
     * Skip the root node
     */
    if (tree != b->tree)
    {
        (*m->compute_node)(tree, x, y);

        if (props->orientation == BRWS_HORIZONTAL)
	    x += tree->width + BRWS_NODE_X_GAP;
        else
	    y += tree->height + BRWS_NODE_Y_GAP;
    }

    /*
     * Get number of children
     */
    num_child = (*m->get_num_children)(tree);

    for (i=0, child = (*m->get_child)(tree, 0);
        (i < num_child);
        child = (*m->get_child)(tree, ++i))
    {
	if (!child)
	    continue;
	compute_submatrix(child, x, y, max_x, max_y);

        if (props->orientation == BRWS_HORIZONTAL)
	{
	    if (x + child->width > *max_x)
	    {
		x = start_x;
		y += row_max_height + BRWS_NODE_Y_GAP;
		row_max_height = 0;
                (*m->compute_node)(child, x, y);
	    }

	    x += child->width + BRWS_NODE_X_GAP;

	    if (child->height > row_max_height)
		row_max_height = child->height;
	}
        else
	{
	    if (y + child->height > *max_y)
	    {
		y = start_y;
		x += col_max_width + BRWS_NODE_X_GAP;
		col_max_width = 0;
                (*m->compute_node)(child, x, y);
	    }

	    y += child->height + BRWS_NODE_Y_GAP;

	    if (child->width > col_max_width)
		col_max_width = child->width;
	}
    }

    *max_y = y + row_max_height + BRWS_NODE_Y_GAP;
    /*
    *max_x = x + col_max_width + BRWS_NODE_X_GAP;
    */
}


/*
 * compute the coordinates of the glyph element in
 * a node
 */
static void
compute_glyph_elm
(
    VNode	node,
    VNodeElm	glyph_elm,
    int		x,
    int		y
)
{
    if (!node || !glyph_elm)
	return;

    /*
     * The width/height of the glyph element
     * was initialized at init time
     */

    glyph_elm->x = x;
    glyph_elm->y = y;
}

/*
 * compute the coordinates of the string element in
 * a node
 */
static void
compute_str_elm
(
    VNode	node,
    VNodeElm	str_elm,
    int		x,
    int		y
)
{
    Viewer			*b;
    BrowserUiObjects	*ui;
    XFontStruct			*sm_font;
    int				font_ascent,
				font_descent;
    char			*str;

    if (!node || !str_elm)
	return;

    b = node->browser;
    ui = (BrowserUiObjects *)b->ui_handle;
    sm_font = ui->sm_font;
    font_ascent = sm_font->max_bounds.ascent;
    font_descent = sm_font->max_bounds.descent;

    str = (char *)str_elm->data;

    str_elm->width = XTextWidth(sm_font, str, strlen(str));
    str_elm->height = font_ascent + font_descent;

    str_elm->x = x;
    str_elm->y = y;
}

/*
 * Compute the coordinates of one node.
 * A browser node consists of node->num_elements
 * elements. Each element has a margin of width
 * BRWS_ELM_BBOX_MARGIN. And surrounding each element
 * and it's margin, on all 4 sides, is a border of
 * width BRWS_ELM_BORDER_WIDTH.
 *
 * The size (width/height) of each element is exactly
 * that. It DOES NOT include the margins/borders. The
 * position (x,y) of each element can be used directly
 * by the rendering routines - they have already been
 * padded by the size of margins/borders.
 *
 * The size of the node DOES include margins/borders.
 * The position of the node is the top left corner
 * of the border.
 */
static void
compute_node(
    ViewerNode	*node,
    int			x,
    int			y
)
{
    Vwr				b;
    ViewerNodeElm		*elm;
    VElmMethods			*elm_methods;
    int				elm_shown;
    int				cur_height = 0,
				cur_width = 0,
				cur_y_offset,
				i;
    BOOL			first_elm = TRUE;

    if (!node)
	return;

    b = node->browser;
    elm_shown = browser_get_elm_shown(b);
    elm = node->elements;
    elm_methods = node->elm_methods;

    /*
     * cur_height is the current height of the node as a whole
     * cur_y_offset is the current y offset for the current element
     * calculated
     *
     * We start both with the width of the top border and margin
     */
    cur_height = BRWS_ELM_BORDER_WIDTH + BRWS_ELM_BBOX_MARGIN;
    cur_y_offset = y + BRWS_ELM_BORDER_WIDTH + BRWS_ELM_BBOX_MARGIN;

    /*
     * Set x,y for node
     */
    node->x = x;
    node->y = y;

    for (i=0; i < node->num_elements; ++i)
    {
        if (elm_shown & (1L << i))
	{
	    /*
	     * For each visible element...
	     */

	    if (!first_elm)
	    {
                cur_height += BRWS_INTER_ELM_DISTANCE;
                cur_y_offset += BRWS_INTER_ELM_DISTANCE;
	    }

	    first_elm = FALSE;

	    /*
	     * increment node height, y offset by top margin
            cur_height += BRWS_ELM_BBOX_MARGIN;
            cur_y_offset += BRWS_ELM_BBOX_MARGIN;
	     */

	    /*
	     * Compute size, position of element
	     */
	    (*(elm_methods[i]->compute))(node, &elm[i], x, cur_y_offset);

	    /*
	     * Increment node height, y offset by new element height,
	     * bottom margin and bottom border
	     */
            cur_height += elm[i].height;
            cur_y_offset += elm[i].height;

	    /*
	     * Keep track of max width
	     */
            cur_width = (cur_width > elm[i].width) ?
				cur_width : elm[i].width;
	}
    }

    /*
     * We end with the width of the bottom border and margin
     */
    cur_height += BRWS_ELM_BORDER_WIDTH + BRWS_ELM_BBOX_MARGIN;
    cur_y_offset += BRWS_ELM_BORDER_WIDTH + BRWS_ELM_BBOX_MARGIN;


    /*
     * The width of the node is the width of the widest element plus
     * space for the margins/borders on both sides
     */
    node->width = cur_width +
		(2 * (BRWS_ELM_BBOX_MARGIN + BRWS_ELM_BORDER_WIDTH));

    /*
     * Set height obtained so far
     */
    node->height = cur_height;

    /*
     * Center all elements according to max width
     */
    for (i=0; i < BRWS_NUM_ELM; ++i)
    {
        if (elm_shown & (1L << i))
	{
            elm[i].x = x + (node->width - elm[i].width)/2;

        }
    }

}


/*
 * compute the coordinates of one collapsed node.
 */
static void
compute_collapsed_node(
    ViewerNode	*node,
    int			x,
    int			y
)
{
    Viewer			*b;
    BrowserUiObjects	*ui;
    ViewerNodeElm		*elm;
    XFontStruct			*sm_font;
    int				elm_shown;
    int				font_ascent,
				font_descent,
				cur_height = 0,
				cur_width = 0,
				cur_x_offset,
				cur_y_offset,
				i;
    char			*collapsed_str = ". . .";
    XCharStruct			overall = {0};
    int				ascent, descent, direction;

    b = node->browser;
    ui = (BrowserUiObjects *)b->ui_handle;
    elm_shown = browser_get_elm_shown(b);
    sm_font = ui->sm_font;
    font_ascent = sm_font->max_bounds.ascent;
    font_descent = sm_font->max_bounds.descent;
    elm = node->elements;

    /*
    cur_x_offset = 0;
    cur_height = cur_y_offset =
        BRWS_ELM_BBOX_MARGIN + BRWS_ELM_BORDER_WIDTH;
    */
    cur_x_offset = cur_y_offset = 0;

    node->x = x;
    node->y = y;

    XTextExtents(sm_font, collapsed_str, strlen(collapsed_str),
		&direction, &ascent, &descent, &overall);

    node->height = ascent + descent + BRWS_ELM_BBOX_MARGIN;
    node->width = overall.width + BRWS_ELM_BBOX_MARGIN;
}

/*
 * Render module tree
 */
static void
render_mod_tree
(
    VNode	tree
)
{
    ViewerMethods	*m;
    AB_OBJ		*obj;

    if (!tree)
	return;

    m = BNODE_METHODS(tree);

    obj = (AB_OBJ *)tree->obj_data;

    if (obj_is_module(obj))
    {
        VNode	child;
        int	i, num_child;

        num_child = (*m->get_num_children)(tree);
        /*
         * Render child nodes
         *
         * Render procs should check for (child == NULL)
         */
        for (i=0, child = (*m->get_child)(tree, 0);
                (i < num_child);
                child = (*m->get_child)(tree, ++i))
        {
            (*m->render_tree)(child);
        }
    }
    else
    {
        if (obj_is_window(obj) || obj_is_menu(obj) || obj_is_message(obj))
	{
            if (BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_SELECTED))
                (*m->render_node)(tree, TRUE);
            else
                (*m->render_node)(tree, FALSE);
	}
    }

}

/*
 * Actually render the tree on the given canvas.
 */
static void
render_proj_tree
(
    VNode	tree
)
{
    ViewerMethods	*m;
    AB_OBJ		*obj;

    if (!tree)
	return;

    m = BNODE_METHODS(tree);

    obj = (AB_OBJ *)tree->obj_data;

    if (obj_is_project(obj))
    {
        VNode	child;
        int	i, num_child;

        num_child = (*m->get_num_children)(tree);
        /*
         * Render child nodes
         *
         * Render procs should check for (child == NULL)
         */
        for (i=0, child = (*m->get_child)(tree, 0);
                (i < num_child);
                child = (*m->get_child)(tree, ++i))
        {
            (*m->render_tree)(child);
        }
    }
    else
    {
        if (obj_is_module(obj))
	{
            if (BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_SELECTED))
                (*m->render_node)(tree, TRUE);
            else
                (*m->render_node)(tree, FALSE);
	}
    }

}

/*
 * Render all 'salient' children of the tree (excluding the
 * project node).
 */
static void
render_salient_tree
(
    VNode	tree
)
{
    ViewerMethods	*m;
    AB_OBJ		*obj;
    VNode		child;
    int			i, num_child;

    if (!tree)
	return;

    m = BNODE_METHODS(tree);

    obj = (AB_OBJ *)tree->obj_data;

    num_child = (*m->get_num_children)(tree);

    /*
     * If this node is not supposed to be seen,
     * don't render it
     */
    if (!BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_VISIBLE))
	return;

    if (obj_is_project(obj))
    {
        /*
         * Don't render project nodes - render it's child nodes
         *
         * Render procs should check for (child == NULL)
         */
        for (i=0, child = (*m->get_child)(tree, 0);
                (i < num_child);
                child = (*m->get_child)(tree, ++i))
        {
            /*
             * If this child node is not supposed to be seen,
             * don't render it.
             */
            if (child && !BRWS_NODE_STATE_IS_SET(child, BRWS_NODE_VISIBLE))
                continue;

            (*m->render_tree)(child);
        }
    }
    else
    {
        if (BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_SELECTED))
            (*m->render_node)(tree, TRUE);
        else
            (*m->render_node)(tree, FALSE);

        if (BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_EXPANDED))
	{
            /*
             * Render child nodes
             *
             * Render procs should check for (child == NULL)
             */
            for (i=0, child = (*m->get_child)(tree, 0);
                    (i < num_child);
                    child = (*m->get_child)(tree, ++i))
            {
                /*
                 * If this child node is not supposed to be seen,
                 * don't render it.
	         * The render_tree method (this routine we're in at
	         * this moment) actually checks for this, but ...
                 */
                if (child && !BRWS_NODE_STATE_IS_SET(child, BRWS_NODE_VISIBLE))
	            continue;

                (*m->render_line)(tree, child);
                (*m->render_tree)(child);
            }
        }
	else
	{
	    /*
	     * Draw collapsed subtree feedback
	     */
	    if (num_child > 0)
	    {
		draw_collapsed_feedback(tree);
	    }
	}
    }

}

/*
 * Render a glyph element of a node
 */
static void
render_glyph_elm
(
    VNode	node,
    VNodeElm	glyph_elm,
    int		hilite
)
{
    Display		*dpy;
    Widget		draw_area;
    Vwr			b;
    BrowserUiObj	ui;
    GC			gc;
    Pixmap		image;

    if (!node || !glyph_elm)
        return;

    /*
     * Get handle to browser, draw area widget, ui info,
     * display
     */
    b = node->browser;
    ui = aob_ui_from_browser(b);
    draw_area = brws_draw_area(b);
    dpy = XtDisplay(draw_area);

    gc = ui->normal_gc;

    /*
     * Get handle to pixmap
     */
    image = (Pixmap)glyph_elm->data;

    /*
     * Render element by copying the image from pixmap to
     * draw area
     */
    XCopyArea(dpy, image, XtWindow(draw_area), gc,
			0, 0,
                        glyph_elm->width, glyph_elm->height,
                        glyph_elm->x, glyph_elm->y);
}

/*
 * Render a string element of a node
 */
static void
render_str_elm
(
    VNode	node,
    VNodeElm	str_elm,
    int		hilite
)
{
    Display		*dpy;
    Widget		draw_area;
    XFontStruct		*sm_font;
    Vwr			b;
    BrowserUiObj	ui;
    GC			gc;
    int			font_ascent;
    char        	*str;

    if (!node || !str_elm)
        return;

    /*
     * Get handle to browser, ui handle, draw area
     * display, font ascent
     */
    b = node->browser;
    ui = aob_ui_from_browser(b);
    draw_area = brws_draw_area(b);
    dpy = XtDisplay(draw_area);
    sm_font = ui->sm_font;
    font_ascent = sm_font->max_bounds.ascent;

    /*
     * Select GC depending on hilite flag
     */
    if (hilite)
    {
	gc = ui->select_gc;
    }
    else {
        gc = ui->normal_gc;
    }

    /*
     * String to render
     */
    str = (char *)str_elm->data;

    /*
     * Render string element on draw area
     */
    XDrawImageString(dpy, XtWindow(draw_area), gc,
		str_elm->x, str_elm->y + font_ascent,
		str, strlen(str));
}

/*
 * Draw one node on the offscreen pixmap.
 * The hilite boolean flag indicates whether the node should be drawn
 * hilited or not.
 */
static void
render_node
(
    ViewerNode	*node,
    int			hilite
)
{
    Display			*dpy;
    Widget			draw_area;
    Viewer			*b;
    AB_OBJ			*obj;
    VMethods			m;
    VNode			parent;
    ViewerNodeElm		*elm;
    VElmMethods			*elm_methods;
    int				elm_shown;
    int				cur_height = 0,
				cur_width = 0,
				cur_x_offset = 0,
				cur_y_offset = 0,
				i;

    if (!node)
        return;

    b = node->browser;
    m = b->methods;

    obj = (AB_OBJ *)node->obj_data;

    elm_shown = browser_get_elm_shown(b);

    draw_area = brws_draw_area(b);

    if (!draw_area)
	return;

    dpy = XtDisplay(draw_area);

    elm = node->elements;
    elm_methods = node->elm_methods;

    if (!BRWS_NODE_STATE_IS_SET(node, BRWS_NODE_VISIBLE))
	return;

    parent = (*m->get_parent)(node);
    while (parent)
    {
        if (!BRWS_NODE_STATE_IS_SET(parent, BRWS_NODE_VISIBLE))
	    return;

        parent = (*m->get_parent)(parent);
    }

    /*
     * Don't render this node if it is outside the window
     */
    if (node->x > 32766 || node->y > 32766)
        return;

    if (!hilite)
    {
        XClearArea(dpy, XtWindow(draw_area),
                node->x, node->y,
                node->width, node->height, FALSE);
    }

    for (i=0; i < node->num_elements; ++i)
    {
        if (elm_shown & (1L << i))
	{
	    (*(elm_methods[i]->render))(node, &elm[i], hilite);
	}
    }

    /*
     * Draw a bounding rectangle for the icon.
     */
    if (hilite)
	render_node_bbox(node);

}

/*
 * Draw the line between the node's icon and its parent's icon.
 */
static void
render_line(
    ViewerNode	*parent,
    ViewerNode	*child
)
{
    BrowserUiObjects	*ui;
    BrowserProperties	*props;
    Viewer			*b;
    ViewerMethods		*m;
    ViewerNode		*real_parent;
    ViewerNode		*cur_child;
    Widget			draw_area;
    GC				line_gc;
    XGCValues		gcvalues;
    XPoint			xp[3];
    int				num_child,
    				num_visible_siblings = 0,
				i;

    if (!parent || !child)
	return;

    /*
     * If the child node is not supposed to be seen, don't
     * draw a line to it.
     */
    if (!BRWS_NODE_STATE_IS_SET(child, BRWS_NODE_VISIBLE))
	return;

    b = child->browser;
    m = b->methods;
    props = aob_browser_properties(b);
    draw_area = brws_draw_area(b);

    if (!draw_area)
	return;

    ui = aob_ui_from_browser(b);
    line_gc = ui->line_gc;

    gcvalues.line_style = LineSolid;
    XChangeGC(XtDisplay(draw_area), line_gc, GCLineStyle, &gcvalues);

    /*
     * Don't render this link if it is outside the window
     */
    if (child->x > 32766 || child->y > 32766 ||
            parent->x > 32766 || parent->y > 32766)
        return;

    real_parent = (*m->get_parent)(child);

    num_child = (*m->get_num_children)(real_parent);

    for (i=0, cur_child = (*m->get_child)(real_parent, 0);
            (i < num_child);
            cur_child = (*m->get_child)(real_parent, ++i))
    {
	if (cur_child == child)
	    continue;

        if (cur_child &&
		BRWS_NODE_STATE_IS_SET(cur_child, BRWS_NODE_VISIBLE))
	    ++num_visible_siblings;
    }

    if (props->orientation == BRWS_VERTICAL)
    {
	/*
	 * Tree orientation is vertical
	 */
        if (num_visible_siblings == 0)
        {
	    /*
	     * This child is the only child of the parent.
	     * Draw just one line, centered, from child to parent
	     * It is up to the compute procs to make sure that the
	     * child and parent are already centered on top of one
	     * another.
	     */
            xp[0].x = child->x + child->width/2;
            xp[0].y = child->y - BRWS_NODE_LINK_GAP;

            xp[1].x = child->x + child->width/2;
            xp[1].y = parent->y + parent->height + BRWS_NODE_LINK_GAP;

            XDrawLines(XtDisplay(draw_area), XtWindow(draw_area), line_gc, xp, 2, CoordModeOrigin);
        }
        else
        {
	    /*
	     * Multiple siblings exist for the parent.
	     * 2 lines (3 points) are drawn per child:
	     * The one extra XDrawLine is unfortunately repeated
	     * for every child unnecessarily. It is for drawing the
	     * line from the parent to the subtree.
	     */
            xp[0].x = child->x + child->width/2;
            xp[0].y = child->y - BRWS_NODE_LINK_GAP;

            xp[1].x = child->x + child->width/2;
            xp[1].y = parent->y + parent->height +
			(BRWS_NODE_LINK_GAP + BRWS_NODE_SUBTREE_GAP);

            xp[2].x = parent->x + parent->width/2;
            xp[2].y = parent->y + parent->height +
			(BRWS_NODE_LINK_GAP + BRWS_NODE_SUBTREE_GAP);

            XDrawLines(XtDisplay(draw_area), XtWindow(draw_area), line_gc, xp, 3, CoordModeOrigin);

            xp[0].x = xp[1].x = parent->x + parent->width/2;
            xp[0].y = parent->y + parent->height + BRWS_NODE_LINK_GAP;
            xp[1].y = xp[0].y + BRWS_NODE_SUBTREE_GAP;

            XDrawLines(XtDisplay(draw_area), XtWindow(draw_area), line_gc, xp, 2, CoordModeOrigin);
        }
    }
    else
    {
	/*
	 * Tree orientation is vertical
	 */
        if (num_visible_siblings == 0)
        {
	    /*
	     * This child is the only child of the parent.
	     * Draw just one line, centered, from child to parent
	     * It is up to the compute procs to make sure that the
	     * child and parent are already centered on top of one
	     * another.
	     */
            xp[0].x = child->x - BRWS_NODE_LINK_GAP;
            xp[0].y = child->y + child->height/2;

            xp[1].x = parent->x + parent->width + BRWS_NODE_LINK_GAP;
            xp[1].y = child->y + child->height/2;

            XDrawLines(XtDisplay(draw_area), XtWindow(draw_area), line_gc, xp, 2, CoordModeOrigin);
        }
        else
	{
	    /*
	     * Multiple siblings exist for the parent.
	     * 2 lines (3 points) are drawn per child:
	     * The one extra XDrawLine is unfortunately repeated
	     * for every child unnecessarily. It is for drawing the
	     * line from the parent to the subtree.
	     */
            xp[0].x = child->x - BRWS_NODE_LINK_GAP;
            xp[0].y = child->y + child->height/2;

            xp[1].x = parent->x + parent->width +
				(BRWS_NODE_LINK_GAP + BRWS_NODE_SUBTREE_GAP);
            xp[1].y = child->y + child->height/2;

            xp[2].x = parent->x + parent->width +
				(BRWS_NODE_LINK_GAP + BRWS_NODE_SUBTREE_GAP);
            xp[2].y = parent->y + parent->height/2;

            XDrawLines(XtDisplay(draw_area), XtWindow(draw_area), line_gc, xp, 3, CoordModeOrigin);

            xp[0].x = parent->x + parent->width + BRWS_NODE_LINK_GAP;
            xp[1].x = xp[0].x + BRWS_NODE_SUBTREE_GAP;
            xp[0].y = xp[1].y = parent->y + parent->height/2;

            XDrawLines(XtDisplay(draw_area), XtWindow(draw_area), line_gc, xp, 2, CoordModeOrigin);
        }
    }
}


/*
 * Recursively calls itself to determine which node the point
 * is in.
 */
static VNode
locate_node
(
    VNode	tree,
    int		x,
    int		y
)
{
    ViewerMethods	*m;
    VNode		child;
    int			i, num_child;

    if (!tree)
	return (NULL);

    if (!BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_VISIBLE))
	return (NULL);

    /*
     * Return the root if (x,y) is within the root
     * node
     */
    if (x >= tree->x &&
	    x <= tree->x + tree->width &&
	    y >= tree->y &&
	    y <= tree->y + tree->height)
        return tree;

    /*
     * If this node is collapsed, don't bother
     * traversing it's children
     */
    if (!BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_EXPANDED))
	return (NULL);

    m = BNODE_METHODS(tree);

    num_child = (*m->get_num_children)(tree);

    /*
     * Recursively look for a 'match' in each child
     * The locate proc should check for (child == NULL)
     */
    for (i=0, child = (*m->get_child)(tree, 0);
                (i < num_child);
                child = (*m->get_child)(tree, ++i))
    {
        VNode	tmp;

        if (tmp = (*m->locate_node)(child, x, y))
            return tmp;
    }

    return (NULL);
}

static unsigned long
locate_elements(
    VNode	vnode,
    int		x,
    int		y
)
{
    Vwr		v;
    VNodeElm	elm;
    int		i,
		num_elm,
		elm_shown;
    unsigned long	ret_mask = 0;

    if (!vnode || (x < 0) || (y < 0))
    {
	return (ret_mask);
    }

    if ((x < vnode->x) || (x > vnode->x + vnode->width) ||
	(y < vnode->y) || (y > vnode->y + vnode->height))
    {
	return (ret_mask);
    }

    v = vnode->browser;
    elm_shown = browser_get_elm_shown(v);
    elm = vnode->elements;
    num_elm = vnode->num_elements;

    for (i=0; i < num_elm; ++i)
    {
        if (!(elm_shown & (1L << i)) ||
	    (y < elm[i].y -
		BRWS_ELM_BBOX_MARGIN - BRWS_ELM_BORDER_WIDTH) ||
	    (y > elm[i].y + elm[i].height +
		BRWS_ELM_BBOX_MARGIN + BRWS_ELM_BORDER_WIDTH))
	{
	    continue;
	}
	ret_mask |= (1L << i);
    }

    return (ret_mask);
}



/*
 * Draw the zoom out feed back on the canvas.
 */
static void
draw_zoom_out
(
    Viewer	*b
)
{
    BrowserUiObjects	*ui_handle;
    int		x, y, width, height;
    Widget	draw_area;

    draw_area = brws_draw_area(b);

    XtVaGetValues(draw_area,
                    XtNx, &x,
                    XtNy, &y,
                    XtNwidth, &width,
                    XtNheight, &height,
                    NULL);

    ui_handle = aob_ui_from_browser(b);

    while (width > 0 || height > 0)
    {
        /*
         * Draw the rectangle.
         */
        XDrawRectangle(XtDisplay(draw_area), XtWindow(draw_area),
			ui_handle->normal_gc, x, y, width, height);
        XClearArea(XtDisplay(draw_area), XtWindow(draw_area),
                    x-4, y-4, width+8, height+8, FALSE);
                    x+=4; y+=4; width-=8; height-=8;
        if (width < 0)
            width = 0;
        if (height < 0)
            height = 0;
    }
}


/*
 * Draw the zoom in feedback on the canvas.
 */
static void
draw_zoom_in
(
    Viewer		*b,
    ViewerNode	*node
)
{
    BrowserUiObjects	*ui_handle;
    Widget			draw_area;
    int				x, y, width, height,
				canvas_x, canvas_y,
				canvas_width, canvas_height;

    if (!node)
        return;

    draw_area = brws_draw_area(b);

    XtVaGetValues(draw_area,
		XtNx, &canvas_x,
		XtNy, &canvas_y,
		XtNwidth, &canvas_width,
		XtNheight, &canvas_height,
		NULL);

    x = node->x + node->width/2;
    y = node->y + node->height/2;
    width = 2;
    height = 2;

    ui_handle = aob_ui_from_browser(b);

    while (width < canvas_width || height < canvas_height)
    {
        /*
        * Draw the rectangle.
        */
        XDrawRectangle(XtDisplay(draw_area), XtWindow(draw_area),
			ui_handle->normal_gc, x, y, width, height);

        x-=4; y-=4; width+=8; height+=8;
        if (width > canvas_width)
            width = canvas_width;
        if (height > canvas_height)
            height = canvas_height;
        if (x < canvas_x)
            x = canvas_x;
        if (y < canvas_y)
            y = canvas_y;

        /*
         * Erase the rectangle.
         */
        XClearArea(XtDisplay(draw_area), XtWindow(draw_area),
            x, y, width, height, FALSE);
    }
}

/*
 * Draw  bounding box of one node
 */
static void
render_node_bbox
(
    ViewerNode	*node
)
{
    XFontStruct			*sm_font;
    Display			*dpy;
    Widget			draw_area;
    Viewer			*b;
    BrowserUiObjects	*ui;
    ViewerNodeElm		*elm;
    unsigned long		elm_shown;
    Pixel		hc, bg;
    Pixmap		hp;
    GC			highlightGC;
    XGCValues			gcv;
    XtGCMask		gcm;
    int				num_elm_shown, i, font_ascent;

    if (!node)
        return;

    b = node->browser;
    ui = aob_ui_from_browser(b);
    num_elm_shown = browser_num_elm_shown(b);
    draw_area = brws_draw_area(b);
    dpy = XtDisplay(draw_area);
    elm = node->elements;
    sm_font = ui->sm_font;
    font_ascent = sm_font->max_bounds.ascent;
    elm_shown = browser_get_elm_shown(b);

    XtVaGetValues(draw_area,
            XmNbackground, &bg,
            XmNhighlightColor, &hc,
            XmNhighlightPixmap, &hp,
            NULL);

    gcm = GCForeground | GCBackground | GCLineWidth;
    gcv.line_width = BRWS_ELM_BORDER_WIDTH;
    gcv.foreground = hc;
    gcv.background = bg;

    gcv.line_style = LineSolid;
    gcm |= GCLineStyle;

    highlightGC = XtGetGC(draw_area, gcm, &gcv);

    /*
     * Subtract 1 from width and height because XDrawRectangle draws
     * the outline of the rectangle.
     */
    XDrawRectangle(dpy, XtWindow(draw_area), highlightGC,
                    node->x + (BRWS_ELM_BORDER_WIDTH - 1),
		    node->y + (BRWS_ELM_BORDER_WIDTH - 1),
		    node->width - BRWS_ELM_BORDER_WIDTH,
		    node->height - BRWS_ELM_BORDER_WIDTH);
}

/*
 * Erase the canvas of a browser.
 */
void
erase_viewer(Viewer *v)
{
    XRectangle    	w_rect;		/* widget width,height,x,y */
    Widget		draw_area;

    draw_area = brws_draw_area(v);

    XtVaGetValues(draw_area,
            		XtNwidth,       &(w_rect.width),
            		XtNheight,      &(w_rect.height),
		        NULL);

    XClearArea(XtDisplay(draw_area), XtWindow(draw_area),
		0, 0,
		w_rect.width,
		w_rect.height,
		FALSE);
}

/*
 * Erase a node.
 */
static void
erase_node
(
    ViewerNode	*node
)
{
    Viewer	*b;
    Widget	draw_area;

    if (!node)
	return;

    b = node->browser;

    draw_area = brws_draw_area(b);

    XClearArea(XtDisplay(draw_area), XtWindow(draw_area), node->x, node->y,
			node->width, node->height, FALSE);
}

static void
draw_collapsed_feedback
(
    VNode	node
)
{
    Vwr			v;
    BrowserUiObj	ui;
    BrowserProps	props;
    Widget		draw_area;
    Display		*dpy;
    GC			gc;
    char		dash_list[2] = {BRWS_DASH_WIDTH, BRWS_DASH_WIDTH};
    XGCValues		gcvalues;

    if (!node)
	return;

    v = node->browser;
    ui = aob_ui_from_browser(v);
    props = aob_browser_properties(v);
    draw_area = brws_draw_area(v);
    dpy = XtDisplay(draw_area);
    gc = ui->normal_gc;

    gcvalues.line_style = LineOnOffDash;
    XChangeGC(dpy, gc, GCLineStyle, &gcvalues);

    XSetDashes(dpy, gc, 0, dash_list, 2);
    if (props->orientation == BRWS_HORIZONTAL)
    {
	/*
	 * Horizontal
	 */

	XDrawLine(dpy, XtWindow(draw_area), gc,
		node->x + node->width + BRWS_NODE_LINK_GAP,
		node->y + (node->height/2),
		node->x + node->width
		    + BRWS_NODE_LINK_GAP
		    + (((2 * BRWS_NUM_DASHES) - 1) * BRWS_DASH_WIDTH),
		node->y + (node->height/2));
    }
    else
    {
	/*
	 * Vertical
	 */
	XDrawLine(dpy, XtWindow(draw_area), gc,
		node->x + (node->width/2),
		node->y + node->height + BRWS_NODE_LINK_GAP,
		node->x + (node->width/2),
		node->y + node->height
		    + BRWS_NODE_LINK_GAP
		    + (((2 * BRWS_NUM_DASHES) - 1) * BRWS_DASH_WIDTH));
    }
}

static void
compute_collapsed_feedback
(
    VNode	node,
    int		*max_x,
    int		*max_y
)
{
    Vwr			v;
    BrowserProps	props;

    if (!node)
	return;

    v = node->browser;
    props = aob_browser_properties(v);

    if (props->orientation == BRWS_HORIZONTAL)
    {
	/*
	 * Horizontal
	 */
        *max_x += BRWS_NODE_LINK_GAP
		+ (((2 * BRWS_NUM_DASHES) - 1) * BRWS_DASH_WIDTH);
    }
    else
    {
	/*
	 * Vertical
	 */
        *max_y += BRWS_NODE_LINK_GAP
		+ (((2 * BRWS_NUM_DASHES) - 1) * BRWS_DASH_WIDTH);
    }
}
