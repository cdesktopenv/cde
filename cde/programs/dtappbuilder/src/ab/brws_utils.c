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
 *	$XConsortium: brws_utils.c /main/4 1996/10/02 10:59:00 drk $
 *
 *	@(#)brws_utils.c	1.54 29 Mar 1995
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


#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <ab_private/util.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/brwsP.h>
#include <ab_private/proj.h>
#include <ab_private/x_util.h>
#include <ab_private/istr.h>

static ABBrowser	get_browser_for_win(
			    AB_OBJ *project, 
			    Window	w
			);

static void		select_node(
		    	    VNode	selected_node
			);

static void		deselect_node(
		    	    VNode	selected_node
			);

static void		toggle_select_node(
		    	    VNode	selected_node
			);

static int		select_fn(
		    	    VNode	node
			);

static int		collapsed_fn(
		    	    VNode	module
			);

static void		r_deselect_all_nodes(
		    	    VNode	tree,
		    	    int		flag
			);

static void		draw_area_snap (
			    Vwr		v
			);

static Boolean		clipWindowWorkProc(
			    XtPointer	client_data
			);

static void		clipwindowEventHandler(
			    Widget	widget,
			    XtPointer	client_data,
			    XEvent	*event,
			    Boolean	*cont_dispatch
			);

static void		clipwindowResizeCB(
			    Widget	w, 
			    XtPointer	client_data, 
			    XtPointer	call_data
			);

static char		*ab_browser_project_name = "*module_name";

static int
select_fn(
    VNode       vnode
)
{
    if (BRWS_NODE_STATE_IS_SET(vnode, BRWS_NODE_SELECTED))
        return (1);
		 
    return (0);
}

static int
collapsed_fn(
    VNode	module
)
{
    if (!BRWS_NODE_STATE_IS_SET(module, BRWS_NODE_EXPANDED))
	return (1);

    return (0);
}


static ABBrowser
get_browser_for_win(
    AB_OBJ *project, 
    Window	w
)
{
    ABBrowser	cur_browser;
    Widget	draw_area;
    
    if (!project || !w)
	return (NULL);
    
    cur_browser = (ABBrowser)project->info.project.browsers;

    while (cur_browser)
    {
	draw_area = brws_draw_area(cur_browser->module);

	if ( draw_area && (w == XtWindow(draw_area)) )
	    return(cur_browser);

	cur_browser = cur_browser->next;
    }

    return ((ABBrowser)NULL);
}

BOOL
aob_is_browser_win(
    AB_OBJ *project,
    Window	w
)
{
    return(get_browser_for_win(project, w) != NULL);
}

AB_OBJ	*
aob_get_object_from_xy(
    AB_OBJ *project, 
    Window	w,
    int x, 
    int y
)
{
    ABBrowser	cur_browser;
    VNode	found_node;
    
    if (!project || !w)
	return (NULL);
    
    cur_browser = get_browser_for_win(project, w);
    if (cur_browser != NULL)
    {
	found_node = vwr_locate_node(cur_browser->module, x, y);

	if (found_node)
	{
	    return((AB_OBJ *)found_node->obj_data);
	}
    }
    return (NULL);
}

/*
 * aob_find_bnode
 * Searches for the ViewerNode on the AB_OBJ corresponding to the
 * passed Viewer
 */
ViewerNode	*
aob_find_bnode
(
    AB_OBJ	*obj,
    Viewer	*v
)
{
    ViewerNode		*bnodes, 
			*found = NULL;
    ViewerMethods	*m;

    if (!v || !obj)  
	return (NULL);

    m = v->methods;

    if (!m)
	return (NULL);

    bnodes = (*m->get_viewer_data)(obj);

    for (found = bnodes; found && (found->browser != v); found = found->next);

    return (found);
}

/*
 * Returns project from browser object
 */
AB_OBJ	*
aob_project_from_browser(
    Viewer	*b
)
{
    return (b ? (AB_OBJ *)b->obj_data : NULL);
}

/*
 * Returns browser properties
 */
BrowserProperties *
aob_browser_properties(
    Viewer	*b
)
{
    return (b ? (BrowserProperties *)b->properties : NULL);
}

/*
 * Copies the properties of one browser to another
 */
void
aob_copy_props(
    Viewer     *from,
    Viewer     *to
)
{
    BrowserProperties	*from_prop,
				*to_prop;
    if (!from || !to)
        return;
							      
    from_prop = aob_browser_properties(from);
    to_prop = aob_browser_properties(to);
    /*
     * These are the only ones that matter now
     */
    to_prop->orientation = from_prop->orientation;
    to_prop->elements_shown = from_prop->elements_shown;
}

/*
 * Get the UI handle from a browser
 */
BrowserUiObjects *
aob_ui_from_browser
(
    Viewer	*b
)
{
    return (b ? (BrowserUiObjects *)b->ui_handle : NULL);
}

/*
 * Get the shell widget for a browser
 */
Widget
aob_ui_shell
(
    Viewer	*b
)
{
    BrowserUiObjects	*ui;
    
    if (!b)
	return (NULL);

    ui = aob_ui_from_browser(b);

    return(ui ? ui->shell : NULL);
}

/*
 * Get the draw area widget for a browser
 */
Widget
brws_draw_area
(
    Viewer	*b
)
{
    VMethods		m;
    
    if (!b)
	return (NULL);

    m = b->methods;

    return ((Widget)(*m->get_drawarea)(b));
}

/*
 * Get the elements of a browser node
 */
ViewerNodeElm *
aob_bnode_elements(
    ViewerNode	*bnode
)
{
    if (!bnode)
	return (NULL);

    return ((ViewerNodeElm *)bnode->elements);
}

/*
 * Get the bit vector which contains info on
 * what node elements of the bnodes are to be shown
 */
unsigned long
browser_get_elm_shown
(
    Viewer	*b
)
{
    BrowserProperties	*props;

    props = aob_browser_properties(b);

    return(props ? props->elements_shown : (unsigned long)NULL);
}

/*
 * Return the count of node elements that are
 * shown for the browser
 */
int
browser_num_elm_shown
(
    Viewer	*b
)
{
    int			i, count = 0;
    unsigned long	elm_shown;

    elm_shown = browser_get_elm_shown(b);

    for (i = 0; i < BRWS_NUM_ELM; ++i)
    {
	count += (elm_shown & 1);

	elm_shown = elm_shown >> 1;
    }

    return (count);
}


/*
 * Browser node selection
 */
void
brws_select
(
    AB_OBJ	*obj
)
{
    AB_OBJ	*project;
    ABBrowser	b_list;
    AB_OBJ	*root_obj = obj;
    VNode	selected_nodes;
    Widget	draw_area;
    ViewerMethods	*m;

    if (!obj)
	return;

    /*
     * Is this necessary ?
     */
    root_obj = obj_get_root(obj);

    if (!root_obj)
	return;

    selected_nodes = (VNode)root_obj->browser_data;

    if (!selected_nodes)
	return;

    /*
     * Get project/browser list
    project = obj_get_project(obj);
    b_list = (ABBrowser)project->info.project.browsers;
     */

    while (selected_nodes)
    {
        BrowserProps	prop = 
		aob_browser_properties(selected_nodes->browser);

	if (prop->active)
	    select_node(selected_nodes);

	selected_nodes = selected_nodes->next;
    }
}

/*
 * Browser node de-selection
 */
void
brws_deselect
(
    AB_OBJ	*obj
)
{
    AB_OBJ	*project;
    ABBrowser	b_list;
    AB_OBJ	*root_obj = obj;
    VNode	selected_nodes;
    Widget	draw_area;
    ViewerMethods	*m;

    if (!obj)
	return;

    /*
     * Is this necessary ?
     */
    root_obj = obj_get_root(obj);

    if (!root_obj)
	return;

    selected_nodes = (VNode)root_obj->browser_data;

    if (!selected_nodes)
	return;

    /*
     * Get project/browser list
    project = obj_get_project(obj);
    b_list = (ABBrowser)project->info.project.browsers;
     */

    while (selected_nodes)
    {
        BrowserProps	prop = 
		aob_browser_properties(selected_nodes->browser);

	if (prop->active)
	    deselect_node(selected_nodes);

	selected_nodes = selected_nodes->next;
    }
}

void
brws_toggle_select
(
    AB_OBJ *obj
)
{
    AB_OBJ	*root_obj = obj;
    VNode	selected_nodes;
    Widget	draw_area;

    if (!obj)
	return;

    root_obj = obj_get_root(obj);

    if (!root_obj)
	return;

    selected_nodes = (VNode)root_obj->browser_data;

    if (!selected_nodes)
	return;

    while(selected_nodes)
    {
        BrowserProps	prop = 
		aob_browser_properties(selected_nodes->browser);

	if (prop->active)
	    toggle_select_node(selected_nodes);

	selected_nodes = selected_nodes->next;
    }
}

void
aob_deselect_all_objects(
    AB_OBJ	*project
)
{
    ABBrowser	browsers;

    if (!project)
	return;

    browsers = (ABBrowser)project->info.project.browsers;

    while (browsers)
    {
        aob_deselect_all_nodes(browsers->module, TRUE);
	browsers = browsers->next;
    }
}

static void
select_node
(
    VNode selected_node
)
{
    ViewerMethods	*m;
    Viewer		*v;
    ABObj		obj;

    if (!selected_node)
	return;

    v = selected_node->browser;
    m = v->methods;

    BRWS_NODE_SET_STATE(selected_node, BRWS_NODE_SELECTED);

    obj = (AB_OBJ *)selected_node->obj_data;

    if (!brwsP_node_is_collapsed(selected_node))
        (*m->render_node)(selected_node, TRUE);
}

static void
deselect_node
(
    VNode selected_node
)
{
    ViewerMethods	*m;
    Viewer		*v;
    ABObj		obj;

    if (!selected_node)
	return;

    v = selected_node->browser;
    m = v->methods;

    BRWS_NODE_UNSET_STATE(selected_node, BRWS_NODE_SELECTED);

    obj = (AB_OBJ *)selected_node->obj_data;

    if (!brwsP_node_is_collapsed(selected_node))
        (*m->render_node)(selected_node, FALSE);
}


/*
 * Deselect all the nodes in the tree.
 * The flag argument is to determine whether immediate refreshing
 * in needed or not.
 */
void
aob_deselect_all_nodes
(
    Viewer	*b,
    int		flag
)
{
    r_deselect_all_nodes(b->current_tree, flag);
}

static void
toggle_select_node
(
    VNode selected_node
)
{
    AB_OBJ		*obj;
    Viewer		*b;
    ViewerMethods	*m;
    Widget		draw_area;

    if (!selected_node)  
	return;

    b = selected_node->browser;
    m = b->methods;
    obj = (AB_OBJ *)selected_node->obj_data;
    draw_area = brws_draw_area(b);

    if (BRWS_NODE_STATE_IS_SET(selected_node, BRWS_NODE_SELECTED))
    {
	BRWS_NODE_UNSET_STATE(selected_node, BRWS_NODE_SELECTED);
        (*m->render_node)(selected_node, FALSE);
    }
    else
    {
	BRWS_NODE_SET_STATE(selected_node, BRWS_NODE_SELECTED);
        (*m->render_node)(selected_node, TRUE);
    }
}

/*
 * Recusively deselect all the nodes in the tree.
 */
static void
r_deselect_all_nodes
(
    ViewerNode	*tree,
    int			flag
)
{
    VMethods	m;
    VNode	child;
    VNode	*selected_nodes = NULL;
    int		i, num_child, num_selected = 0;

    if (!tree)
	return;

    vwr_get_cond(tree, &selected_nodes, 
			&num_selected, select_fn);

    m = BNODE_METHODS(tree);

    for (i=0; i < num_selected; ++i)
    {
	/*
	 * Make node state unselected
	 */
	BRWS_NODE_UNSET_STATE(selected_nodes[i], BRWS_NODE_SELECTED);

	/*
	 * If flag is set, and node is expanded (== not collapsed),
	 * re-render node in unselected state
	 */
        if (flag && !brwsP_node_is_collapsed(selected_nodes[i]))
            (*m->render_node)(selected_nodes[i], FALSE);
    }

    if (selected_nodes)
	util_free(selected_nodes);
}

void
brws_set_module_name
(
    Vwr		b
)
{
    BrowserUiObjects *ui;
    DtbBrwsMainwindowInfo	instance;
    AB_OBJ	*module_obj = NULL;
    Widget	shell,
		project_label;
    XmString	xmlabel;
    char	*module_name = NULL;
    char	title[300];

    if (!b)
	return;
    
    shell = aob_ui_shell(b);

    if (b->tree)
        module_obj = (ABObj)b->tree->obj_data;

    if (module_obj)
    {
	if (obj_get_file(module_obj) != NULL)
	{
	    char	*fullpath, *filename;

	    fullpath = obj_get_file(module_obj);

            /*
             * Check return value of strrchr before adding 1 to it
             */
	    if (filename = strrchr(fullpath, '/'))
	        module_name = (STRING)strdup(filename + 1);
	    else
	        module_name = (STRING)strdup(fullpath);

	}
	else
	{
	    module_name = util_strsafe(obj_get_name(module_obj));
	}
    }

    if (module_name)
    {
        xmlabel = XmStringCreateLocalized(module_name);

        sprintf(title, "Module Browser - %s", module_name);
    }
    else
    {
        xmlabel = XmStringCreateLocalized(" ");
        sprintf(title, "Module Browser");
    }

    XtVaSetValues(shell, XtNtitle, title, NULL);

    ui = aob_ui_from_browser(b);
    instance = (DtbBrwsMainwindowInfo)ui->ip;
    project_label = instance->module_name;

    XtVaSetValues(project_label, XmNlabelString, xmlabel, NULL);

    XmStringFree(xmlabel);

}

/*
 * aob_preview
 * Parameters:
 *	obj		- which object to preview in the browser
 *	browser_window	- which browser window to do the priviewing in
 *	
 * Assumes:
 *	that x_conn_fullscreen_init() was called before this function 
 *	is called and x_conn_fullscreen_cleanup() will be called after
 *	this function is called.
 */
extern void
aob_preview(
    ABObj	obj,
    Window	browser_window
)
{
#define		AB_BROWSER_PREVIEW_MARGIN	2
    Vwr		b;
    VNode	bnode;
    Display	*dpy;
    Widget	draw_area;
    Window	root, child_win;
    int		root_x, root_y;

    if (!obj || !browser_window)
	return;

    bnode = (VNode)obj->browser_data;
    b = bnode->browser;
    draw_area = brws_draw_area(b);

    /*
     * Search for the relevant bnode
     * - there may be more than one browser visible
     */
    while(bnode && (browser_window != XtWindow(draw_area)))
    {
        bnode = bnode->next;
	if (bnode)
	{
            b = bnode->browser;
            draw_area = brws_draw_area(b);
	}
    }

    if (!bnode)
	return;

    dpy = XtDisplay(draw_area);
    root = RootWindow(dpy, DefaultScreen(dpy));

    XTranslateCoordinates(dpy, browser_window, root,
		    bnode->x, bnode->y, &root_x, &root_y, &child_win);
    
    root_x -= AB_BROWSER_PREVIEW_MARGIN;
    root_y -= AB_BROWSER_PREVIEW_MARGIN;

    x_fullscreen_preview_box(draw_area, root, 
		root_x, 
		root_y,
		root_x + bnode->width + 
		    AB_BROWSER_PREVIEW_MARGIN + 
		    AB_BROWSER_PREVIEW_MARGIN,
		root_y + bnode->height + 
		    AB_BROWSER_PREVIEW_MARGIN + 
		    AB_BROWSER_PREVIEW_MARGIN);
    
#undef AB_BROWSER_PREVIEW_MARGIN
}

void
brws_update_node(
    ABObj	obj
)
{
    Vwr		v;
    VNode	update_nodes = NULL;
    VMethods	*m;

    if (!obj)
	return;
    

    update_nodes = (VNode)obj->browser_data;

    while (update_nodes)
    {
        vwr_init_elements(update_nodes);

	v = update_nodes->browser;

	if (obj_is_module(obj))
	{
	    brws_set_module_name(v);
	}

	erase_viewer(v);
	draw_viewer(v);

	update_nodes = update_nodes->next;
    }
}

/*
 * brws_get_browser_for_obj()
 * Returns the browser for the passed obj.
 * The browser list is searched for the module that contains the passed
 * object. If none is found, a new browser is created, populated with
 * the module and it is returned.
 *
 * If obj is NULL, an empty browser is searched for.
 */
ABBrowser
brws_get_browser_for_obj(
    ABObj	obj
)
{
    ABObj	module = NULL,
		proj = NULL;
    ABBrowser	cur_b = NULL,
		b_list = NULL;
    ABObj	project = proj_get_project();

    if (obj)
    {
        module = obj_get_module(obj);
        proj = obj_get_project(obj);
        b_list = proj ? (ABBrowser)proj->info.project.browsers : NULL;
    }
    else
    {
        b_list = project ? (ABBrowser)project->info.project.browsers : NULL;
    }

    for (cur_b = b_list; cur_b; cur_b = cur_b->next)
    {
        if (cur_b->project->tree &&
                ((AB_OBJ *)cur_b->project->tree->obj_data == module))
            break;
    }

    if (!cur_b)
    {
        cur_b = brws_create();
        cur_b->next = b_list;
        if (b_list)
            b_list->previous = cur_b;

	if (project)
            project->info.project.browsers = cur_b;

	if (module)
            brws_add_objects_to_browser(cur_b, module);
    }

    return(cur_b);
}

/*
 * brws_get_browser_shell_for_obj()
 * Returns the browser shell for the passed obj.
 * The browser list is searched for the module that contains the passed
 * object. If found, its shell is returned.
 */
Widget
brws_get_browser_shell_for_obj(
    ABObj	obj
)
{
    ABObj	module = NULL,
		proj = NULL;
    ABBrowser	cur_b = NULL,
		b_list = NULL;
    ABObj	project = proj_get_project();

    if (!obj)
	return NULL;
    
    module = obj_get_module(obj);
    proj = obj_get_project(obj);
    b_list = proj ? (ABBrowser)proj->info.project.browsers : NULL;

    for (cur_b = b_list; cur_b; cur_b = cur_b->next)
    {
        if (cur_b->project->tree &&
                ((AB_OBJ *)cur_b->project->tree->obj_data == module))
            break;
    }

    if (!cur_b)
	return NULL;
    
    return(aob_ui_shell(cur_b->module));
}

int
brwsP_select_fn(
    VNode       vnode
)
{
    if (BRWS_NODE_STATE_IS_SET(vnode, BRWS_NODE_SELECTED))
        return (1);
		 
    return (0);
}

/*
 * Synch up the toplevel and detailed view of browser
 * The browser consists of the toplevel view and the
 * detailed view. The toplevel view is used to control
 * what is displayed in the detailed view. Selecting a node
 * in the toplevel view (a basewindow for example), will
 * show the node and it's children in the detailed view.
 *
 * If the 'select_at_least_one' flag is TRUE, and
 * there are no nodes selected in the toplevel view, 
 * the first node will be selected, making the corresponding
 * node in the detailed view visible.
 * If the 'select_at_least_one' is FALSE, no special action 
 * is taken.
 */
void
brwsP_sync_views(
    ABBrowser	ab,
    short	select_at_least_one
)
{
    Vwr		toplevel,
		detailed;
    VNode	toplevel_child,
		detailed_child;
    VMethods	m;
    int		num_child,
		num_selected = 0,
		i;

    if (!ab)
	return;

    /*
     * Get toplevel/detailed view
     */
    toplevel = ab->project;
    detailed = ab->module;

    if (!toplevel || !detailed)
	return;

    m = toplevel->methods;

    /*
     * Any nodes currently selected in toplevel view ?
     */
    vwr_num_cond(toplevel->current_tree, &num_selected, brwsP_select_fn);

    /*
     * Get child count of toplevel view
     */
    num_child = (*m->get_num_children)(toplevel->current_tree);

    /*
     * For every node in toplevel view
     */
    for (i=0, toplevel_child = (*m->get_child)(toplevel->current_tree, 0); 
            (i < num_child); 
            toplevel_child = (*m->get_child)(toplevel->current_tree, ++i))
    {
	ABObj	cur_obj;

	if (!toplevel_child)
	    continue;

	/*
	 * Get ABObj of current toplevel child node
	 */
	cur_obj = (ABObj)toplevel_child->obj_data;

	/*
	 * From the ABObj, get the corresponding node in detailed 
	 * view
	 */
        detailed_child = aob_find_bnode(cur_obj, detailed);

	if (!detailed_child)
	    continue;

        if (BRWS_NODE_STATE_IS_SET(toplevel_child, BRWS_NODE_SELECTED))
        {
            /*
            * Node in toplevel view is selected.
            * Make node in detailed view visible.
            */
            BRWS_NODE_SET_STATE(detailed_child, BRWS_NODE_VISIBLE);
        }
        else
        {
            /*
             * Node in toplevel view is not selected.
	     *
	     * If 'select_at_least_one' flag set...
             * Check first if there are no nodes selected.
             * If there are none, make the first non selected node
             * in the toplevel view selected and make the corresponding 
             * node in the detailed view visible.
             */
            if (select_at_least_one && !num_selected)
            {
                BRWS_NODE_SET_STATE(toplevel_child, BRWS_NODE_SELECTED);
                BRWS_NODE_SET_STATE(detailed_child, BRWS_NODE_VISIBLE);
                ++num_selected;
            }
            else
            {
		/*
		 * Make node in detailed view not visible
		 */
                BRWS_NODE_UNSET_STATE(detailed_child, BRWS_NODE_VISIBLE);
            }
        }
    }

}


/*
 * brwsP_node_is_collapsed()
 * Returns TRUE if node is collapsed, FALSE otherwise.
 * Note: on error conditions, TRUE is returned.
 */
int
brwsP_node_is_collapsed
(
    VNode	node
)
{
    VNode	parent;
    VMethods	m;
    Vwr		v;
	    ABObj	obj;

    if (!node)
	return (TRUE);
    
    obj = (AB_OBJ *)node->obj_data;
    /*
    fprintf(stderr, "brwsP_node_is_collapsed(%s), viewer = %p\n", 
	obj_get_name(obj),
	node->browser);
    */

    if (!(v = node->browser))
	return (TRUE);

    if (!(m = v->methods))
	return (TRUE);

    while (parent = (*m->get_parent)(node))
    {
	if (!BRWS_NODE_STATE_IS_SET(parent, BRWS_NODE_EXPANDED))
	{

	    obj = (AB_OBJ *)parent->obj_data;

	    /*
	    fprintf(stderr, "parent node %s is collapsed\n", obj_get_name(obj));
	    */
	    return (TRUE);
	}

	node = parent;
    }

    return (FALSE);
}

/*
 * brwsP_node_is_visible()
 * Returns TRUE if node is visible, FALSE otherwise.
 * Note: on error conditions, TRUE is returned.
 */
int
brwsP_node_is_visible
(
    VNode	node
)
{
    VNode	parent;
    VMethods	m;
    Vwr		v;
	    ABObj	obj;

    if (!node)
	return (FALSE);
    
    if (!BRWS_NODE_STATE_IS_SET(node, BRWS_NODE_VISIBLE))
	return (FALSE);

    obj = (AB_OBJ *)node->obj_data;
    /*
    fprintf(stderr, "brwsP_node_is_visible(%s), viewer = %p\n", 
	obj_get_name(obj),
	node->browser);
    */

    if (!(v = node->browser))
	return (TRUE);

    if (!(m = v->methods))
	return (TRUE);

    while (parent = (*m->get_parent)(node))
    {
	if (!BRWS_NODE_STATE_IS_SET(parent, BRWS_NODE_VISIBLE))
	{

	    obj = (AB_OBJ *)parent->obj_data;

	    /*
	    fprintf(stderr, "parent node %s is not visible\n", obj_get_name(obj));
	    */
	    return (FALSE);
	}

	node = parent;
    }

    return (TRUE);
}

/*
 * brwsP_make_drawarea_snap()
 * Make the passed draw area 'snap' to be the size of it's parent
 * whenever it's parent is resized.
 * This function tries to take advantage of the fact that the parent 
 * in most cases is the clipwindow of a scrolled window which is 
 * also a draw area widget. In this case, a resize callback is 
 * registered. In other cases, an event handler is registered.
 */
void
brwsP_make_drawarea_snap(
    Vwr		v,
    Widget	draw_area
)
{
    Widget	clipwindow;
    
    if (!v || !draw_area)
	return;
    
    clipwindow = XtParent(draw_area);

    if (XmIsDrawingArea(clipwindow))  {
	XtAddCallback(clipwindow, XmNresizeCallback, 
			clipwindowResizeCB, (XtPointer)v);
    }
    else
    {
        XtAddEventHandler(clipwindow,
                StructureNotifyMask, False,
                clipwindowEventHandler, (XtPointer)v);
    }
}

/*
 * Resize callback for parent of draw area
 */
static void
clipwindowResizeCB(
    Widget	w, 
    XtPointer	client_data, 
    XtPointer	call_data
)
{
    /* 
     * Defer munging until we're out of the ScrolledWindow Resize callback 
     */
    XtAppAddWorkProc(XtWidgetToApplicationContext(w), clipWindowWorkProc, client_data);
}

/*
 * Event handler for parent of draw area
 */
static void
clipwindowEventHandler(
    Widget	widget,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_dispatch
)
{
    if (event->type != ConfigureNotify)
        return;

    /* 
     * Defer munging until we're out of the ScrolledWindow Event Handler
     */
    XtAppAddWorkProc(XtWidgetToApplicationContext(widget), 
		clipWindowWorkProc, client_data);
}


/*
 * Work proc for clip window
 * - make draw area snap to clip window's size.
 */
static Boolean
clipWindowWorkProc(
    XtPointer client_data
)
{
    draw_area_snap((Vwr)client_data);
    return(True);
}


/*
 * draw_area_snap()
 * Make size of draw area snap to size of clipwindow.
 * The clipwindow is the direct parent of the draw area.
 * The preferred/minimum size of the draw area is stored in
 * the viewer properties. This is the size needed to render
 * whatever graphics is needed.
 *
 * Logic used:
 *	if clipwindow_width > min_width
 *	    make draw area width == clip window width
 *	else
 *	    if draw area width > minimum width (i.e. preferred width)
 *	        make draw area width == minimum width
 *
 * Same logic used for height
 */
static void
draw_area_snap (
    Vwr		v
)
{
    Widget		clipwindow = NULL,
    			draw_area = NULL;
    Dimension		cw_width = 0,
			cw_height = 0,
    			da_borderWidth = 0,
			da_width = 0,
			da_height = 0;
    BrowserProps	props = NULL;
    Arg			arg[4];
    int			num_args = 0;

    if (!v)
	return;

    /*
     * Get viewer properties
     */
    if (!(props = aob_browser_properties(v)))
	return;

    /*
     * Get draw area and clip window
     */
    if (!(draw_area = brws_draw_area(v)))
	return;

    if (!(clipwindow = XtParent(draw_area)))
	return;

    /*
     * Get clipwindow width/height, draw area
     * width/height/border width
     */
    XtVaGetValues(clipwindow, 
		XmNwidth, &cw_width, 
		XmNheight, &cw_height, 
		NULL);
    XtVaGetValues(draw_area, 
		XmNborderWidth, &da_borderWidth, 
		XmNwidth, &da_width, 
		XmNheight, &da_height, 
		NULL);

    if ((Dimension)cw_width > props->min_width)
    {
	/*
	 * Clip window width is more than minimum width
	 * -> expand draw area
	 * The dimension that we actually use must be offset by the
	 * border width to ensure a perfect fit in the clip window. 
	 * Otherwise, this will trigger the scrollbar in the clipwindow.
	 */
        if (cw_width > 2u * da_borderWidth) {
	    cw_width -= 2u * da_borderWidth;

	    if (cw_width > 2) 
	    {
	        XtSetArg(arg[num_args], XmNwidth, (cw_width - 2));
		num_args++;
	    }
        }
    }
    else
    {
	/*
	 * Clip window width is less than minimum width
	 * -> check if draw area width is more than it's minimum.
	 * If yes, shrink it back down to the minimum width.
	 */
	if ((Dimension)da_width > props->min_width)
	{
            XtSetArg(arg[num_args], XmNwidth, props->min_width);
            num_args++;
	}
    }


    if ((Dimension)cw_height > props->min_height)
    {
	/*
	 * Clip window height is more than minimum height
	 * -> expand draw area
	 * The dimension that we actually use must be offset by the
	 * border width to ensure a perfect fit in the clip window. 
	 * Otherwise, this will trigger the scrollbar in the clipwindow.
	 */
        if (cw_height > 2u * da_borderWidth) {
	    cw_height -= 2u * da_borderWidth;

	    if (cw_height > 2) 
	    {
	        XtSetArg(arg[num_args], XmNheight, (cw_height - 2));
		num_args++;
	    }
        }
    }
    else
    {
	/*
	 * Clip window height is less than minimum height
	 * -> check if draw area height is more than it's minimum.
	 * If yes, shrink it back down to the minimum height.
	 */
	if ((Dimension)da_height > props->min_height)
	{
            XtSetArg(arg[num_args], XmNheight, props->min_height);
            num_args++;
	}
    }
    
    if (num_args > 0)
	XtSetValues(draw_area, arg, num_args);
}

void 
brwsP_collapse_selected(
    ABBrowser	ab
)
{
    Vwr		v = NULL;
    VNode	*selected_nodes = NULL;
    VMethods	m;
    int		num_selected = 0,
		i;
    short	redraw = FALSE;


    if (!ab)
	return;
    
    if (!(v = ab->module))
	return;

    if (!(m = v->methods))
	return;
    
    /*
     * Get nodes that are selected
     */
    vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, select_fn);
    
    /*
     * Return if no selected nodes
     */
    if (num_selected == 0) {
	util_free(selected_nodes);
	return;
    }

    for (i=0; i < num_selected; ++i)
    {
	/*
	 * For each selected node, check if they are expanded.
	 * If they are, mark them as collapsed.
	 */
	if (BRWS_NODE_STATE_IS_SET(selected_nodes[i], BRWS_NODE_EXPANDED))
	{
	    BRWS_NODE_UNSET_STATE(selected_nodes[i], BRWS_NODE_EXPANDED);
	    redraw = TRUE;
	}
    }

    if (redraw)
    {
        erase_viewer(v);
        draw_viewer(v);
    }

    /*
     * Free up node list if it contained anything
     */
    if (selected_nodes)
	util_free(selected_nodes);
}


void 
brwsP_expand_selected(
    ABBrowser	ab
)
{
    Vwr		v = NULL;
    VNode	*selected_nodes = NULL;
    VMethods	m;
    int		num_selected = 0,
		i;
    short	redraw = FALSE;

    if (!ab)
	return;
    
    if (!(v = ab->module))
	return;

    if (!(m = v->methods))
	return;
    
    /*
     * Get nodes that are selected
     */
    vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, select_fn);
    
    /*
     * Return if no selected nodes
     */
    if (num_selected == 0) {
	util_free(selected_nodes);
	return;
    }

    for (i=0; i < num_selected; ++i)
    {
	/*
	 * For each selected node, check if they are collapsed.
	 * If they are, mark them as expanded.
	 */
	if (!BRWS_NODE_STATE_IS_SET(selected_nodes[i], BRWS_NODE_EXPANDED))
	{
	    BRWS_NODE_SET_STATE(selected_nodes[i], BRWS_NODE_EXPANDED);
	    redraw = TRUE;
	}
    }

    if (redraw)
    {
        erase_viewer(v);
        draw_viewer(v);
    }

    /*
     * Free up node list if it contained anything
     */
    if (selected_nodes)
	util_free(selected_nodes);
}


void 
brwsP_expand_collapsed(
    ABBrowser	ab
)
{
    Vwr		v = NULL;
    VNode	*collapsed_nodes = NULL;
    VMethods	m;
    int		num_collapsed = 0,
		i;
    short	redraw = FALSE;

    if (!ab)
	return;
    
    if (!(v = ab->module))
	return;

    if (!(m = v->methods))
	return;
    
    /*
     * Get nodes that are collapsed
     */
    vwr_get_cond(v->current_tree, &collapsed_nodes, 
			&num_collapsed, collapsed_fn);
    
    /*
     * Return if no collapsed nodes
     */
    if (num_collapsed == 0) {
	free(collapsed_nodes);
	return;
    }

    for (i=0; i < num_collapsed; ++i)
    {
	/*
	 * For each collapsed node, mark them as expanded only if
	 * they are visible.
	 */
	if (brwsP_node_is_visible(collapsed_nodes[i]))
	{
            BRWS_NODE_SET_STATE(collapsed_nodes[i], BRWS_NODE_EXPANDED);
	    redraw = TRUE;
	}
    }

    if (redraw)
    {
        erase_viewer(v);
        draw_viewer(v);
    }

    /*
     * Free up node list if it contained anything
     */
    if (collapsed_nodes)
	util_free(collapsed_nodes);
}


void 
brwsP_tear_off_selected(
    ABBrowser		b
)
{
    ABBrowser		new_browser, b_list;
    BrowserProps	props, new_props;
    AB_OBJ		*project, *sel_obj;
    VNode		selected;
    ViewerMethods	*m;

    if (!b || !b->module)
	return;

    project = (AB_OBJ *)b->module->obj_data;

    if (!project)
	return;

    new_browser = brws_create();


    /*
     * Link this new browser with the rest
     */
    b_list = (ABBrowser)project->info.project.browsers;
    new_browser->next = b_list;
    if (b_list)
	b_list->previous = new_browser;

    /*
     * Make this new browser the first one on the list
     */
    project->info.project.browsers = new_browser;

    /*
     * Add VNodes to the viewers of this browser
     */
    m = new_browser->project->methods;
    (*m->insert_tree)(new_browser->project, b->project->tree->obj_data);

    m = new_browser->module->methods;
    (*m->insert_tree)(new_browser->module, b->module->tree->obj_data);

    brwsP_sync_views(new_browser, TRUE);

    /*
     * Code to make the root of the new browser the
     * current selected node in the current browser
    selected = node_selected(b->current_tree);
    sel_obj = (AB_OBJ *)selected->obj_data;

    new_browser->current_tree = aob_find_bnode(sel_obj, new_browser);
    */

    aob_copy_props(b->project, new_browser->project);
    aob_copy_props(b->module, new_browser->module);

    recompute_viewer(new_browser->project);
    recompute_viewer(new_browser->module);

    brws_popup(new_browser);
}

void
brws_center_on_obj(
    ABBrowser	ab,
    ABObj	obj
)
{
    Vwr			top_level_view = NULL;
    Vwr			detailed_view = NULL;

    if (!ab || !obj)
	return;

    /*
     * Get top level and detailed tree view
     */
    top_level_view = ab->project;
    detailed_view = ab->module;

    if (top_level_view && detailed_view)
    {
        BrowserUiObj		ui;
        DtbBrwsMainwindowInfo	instance;
	VNode		detailed_node;
	Widget		vert_sb = (Widget)NULL;
	Widget		horiz_sb = (Widget)NULL;
	int		x, y, value, size, increment, page, 
			vert_max, horiz_max,
			vert_min, horiz_min;
	Dimension	scroll_width, scroll_height;
	BOOL		redraw_needed = FALSE;


	/*
	 * Get node in detailed view to center on
	 */
	detailed_node = aob_find_bnode(obj, detailed_view);

	if (!detailed_node)
	    return;

	/*
	 * If the node in the detailed view is not visible,
	 * make it visible first before proceeding.
	 * This is done by selecting the relevant node in the 
	 * top level view and calling brwsP_sync_views().
	 */
	if (!brwsP_node_is_visible(detailed_node))
	{
	    AB_TRAVERSAL	trav;
	    VNode		top_level_node;
	    ABObj		module,
				top_level_obj;
	    ViewerMethods	*m;

	    module = obj_get_module(obj);

	    /*
	     * Look for direct child of module which is the
	     * ancestor of the centered object/node
	     */
	    for (trav_open(&trav, module, AB_TRAV_CHILDREN); 
		(top_level_obj = trav_next(&trav)) != NULL; )  {
		
		if (obj_is_descendant_of(obj, top_level_obj) ||
		   (obj == top_level_obj))
		    break;
	    }
	    trav_close(&trav);

	    if (!top_level_obj)
		return;

	    /*
	     * Found top level ABObj - direct child of module which is
	     * the ancestor of 'obj'.
	     *
	     * Get the corresponding node in the top level view
	     */
	    top_level_node = aob_find_bnode(top_level_obj, top_level_view);

	    if (!top_level_node)
		return;

	    /*
	     * Select the top level node 
	     */
	    BRWS_NODE_SET_STATE(top_level_node, BRWS_NODE_SELECTED);

	    m = top_level_view->methods;

	    if (!m)
		return;

	    /*
	     * Render it selected
	     */
	    (*m->render_node)(top_level_node, TRUE);

	    /*
	     * Sync up the top level and detailed tree views
	     * After this the centered node will be 'visible'
	     */
	    brwsP_sync_views(ab, FALSE);

	    /*
	     * Erase and recompute views
	     */
	    erase_viewer(detailed_view);
            recompute_viewer(detailed_view);
	    redraw_needed = TRUE;
	}

        ui = aob_ui_from_browser(detailed_view);
        instance = (DtbBrwsMainwindowInfo)ui->ip;

        if (!instance->detailed_drawarea)
	    return;

	/*
	 * Get vertical/horizontal scrollbars of scrolled window
	 */
	XtVaGetValues(instance->detailed_drawarea_scrolledwin,
			XmNverticalScrollBar, &vert_sb,
			XmNhorizontalScrollBar, &horiz_sb,
			XmNwidth, &scroll_width,
			XmNheight, &scroll_height,
			NULL);

	/*
	 * If the scrolled area is smaller than the node, use the
	 * node's size as the scroll area - to prevent getting
	 * negative numbers in our calculations later on.
	 * BUG - am I casting properly here?
	 */
	if (scroll_width < (Dimension)detailed_node->width)
	    scroll_width = (Dimension)detailed_node->width;
	if (scroll_height < (Dimension)detailed_node->height)
	    scroll_height = (Dimension)detailed_node->height;

	/*
	 **********************************************************
	 * Center browser node to the middle of the scrolled window
	 **********************************************************
	 */

	/*
	 * Adjust vertical scrollbar
	 */


	if (vert_sb)
	    {
	    /*
	     * Get current scrollbar values
	     */
	    XtVaGetValues(vert_sb, 
		XmNminimum, &vert_min, 
		XmNmaximum, &vert_max, 
		NULL);
	    XmScrollBarGetValues(vert_sb, &value, &size, &increment, &page);

	    /*
	     * Don't adjust if for some reason if the node is off the browser
	     */
	    if (detailed_node->y < vert_max)
	    {
	        /*
	         * This adjusts the y position of the scrollbar so that
	         * the node is centered
	         */
	        y = detailed_node->y - ((int)scroll_height - detailed_node->height)/2;

	        /*
	         * Check for illegal y value:
	         * Must make sure the y position does not exceed 
	         * (XmNmaximum - XmNsliderSize)
	         */
	        if (vert_max - y < size)
		    y = vert_max - size;

	        if (y < vert_min)
		    y = vert_min;

	        /*
	         * Set the scrollbar with new 'centered' y position
	         */
	        XmScrollBarSetValues(vert_sb, (int)y, size, increment, page, True);
	    }
	}

	/*
	 * Adjust horizontal scrollbar
	 */

	if (horiz_sb)
	{
	    /*
	     * Get current scrollbar values
	     */
	    XtVaGetValues(horiz_sb, 
		XmNminimum, &horiz_min, 
		XmNmaximum, &horiz_max, 
		NULL);
	    XmScrollBarGetValues(horiz_sb, &value, &size, &increment, &page);

	    /*
	     * Don't adjust if for some reason if the node is off the browser
	     */
	    if (detailed_node->x < horiz_max)
	    {
	        /*
	         * This adjusts the y position of the scrollbar so that
	         * the node is centered
	         */
	        x = detailed_node->x - ((int)scroll_width - detailed_node->width)/2;

	        /*
	         * Check for illegal y value:
	         * Must make sure the y position does not exceed 
	         * (XmNmaximum - XmNsliderSize)
	         */
	        if (horiz_max - x < size)
		    x = horiz_max - size;

	        if (x < horiz_min)
		    x = horiz_min;

	        /*
	         * Set the scrollbar with new 'centered' y position
	         */
	        XmScrollBarSetValues(horiz_sb, (int)x, size, increment, page, True);
	    }
	}

	/*
	 * Redraw if needed
	 */
	if (redraw_needed)
	    draw_viewer(detailed_view);
    }
}
