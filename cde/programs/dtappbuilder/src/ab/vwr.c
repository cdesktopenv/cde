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
 *	$XConsortium: vwr.c /main/3 1995/11/06 17:57:10 rswiston $
 *
 * @(#)vwr.c	1.21 16 Aug 1994
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


/*
 * browser.c
 * This file contains code to implement the generic object browser.
 * It should be standalone - it should not contain dependencies on
 * the App Builder.
 *
 * Currently, this is a direct port of the Devguide browser.
 *
 * This file will be modified (soon) for the new browser for the CDE 
 * App Builder.
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <Xm/Xm.h>
#include <ab_private/vwr.h>

/*
 * Some internal functions.
static ViewerNode	*r_locate_node();
 */
static void		erase_node();
static int		child_of_selected_node();
static void		draw_icon();
static void		draw_arrow();

/*************************************
	ADT of the browser structure
 *************************************/

/*
 * Create the browser object.
 */
Vwr
vwr_create(
    VMethods	m
)
{
    Vwr	v;

    v = (Viewer *) calloc(1, sizeof(Viewer));

    v->obj_data = NULL;
    v->methods = m;
    v->ui_handle = NULL;
    v->properties = NULL;
    v->next = NULL;
    v->previous = NULL;

    /*
     * v->tree == root
     * v->current_tree == 'current' subtree
     */
    v->tree = v->current_tree = NULL;

    if (m)
    {
	(*m->init_prop)(v);
    }

    return v;
}


/*
 * Destroy the browser object.
 */
void
vwr_destroy(
    Vwr	v
)
{
    VMethods	m;

    m = v->methods;

    if (m && m->free_prop)
	(*m->free_prop)(v);

    free(v);
}


/*
 * Recursively destroy a tree and whatever below it.
 */
void
vwr_destroy_tree
(
    ViewerNode	*tree
)
{
    Viewer		*b;
    ViewerMethods	*m;
    ViewerNode	*child;
    ViewerNode	*next_child;
    int			i, num_child;

    if (!tree)
	return;

    b = tree->browser;
    m = b->methods;

    num_child = (*m->get_num_children)(tree);

    /*
     * Recursively destroy children subtrees
     * Destroy procs should check for (child == NULL) 
     */
    for (i=0, child = (*m->get_child)(tree, 0); 
            (i < num_child); 
            child = (*m->get_child)(tree, ++i))
        vwr_destroy_tree(child);

    if (tree == b->current_tree)
        b->current_tree = b->tree;

    vwr_destroy_node(tree);
}

VNode
vwr_create_node(void)
{
    VNode	bnode;

    bnode = (VNode) malloc(sizeof(ViewerNode));
    
    bnode->browser = NULL;
    bnode->obj_data = NULL;
    bnode->elm_methods = NULL;
    bnode->elements = NULL;
    bnode->num_elements = 0;
    bnode->boundbox_shown = TRUE;
    bnode->x = bnode->y = bnode->width = bnode->height = -1;
    bnode->state = 0;
    bnode->next = NULL;
    bnode->previous = NULL;

    return (bnode);
}


/*
 * Destroy one browser node.
 * To avoid mulitply destroying the same node, use this
 * function.
 */
void
vwr_destroy_node
(
    ViewerNode	*node
)
{
    free(node->elements);
    free(node);
}

void
vwr_init_elements(
    VNode	vnode
)
{
    Vwr		v;
    VMethods	m;
    
    if (!vnode)
	return;

    if (!(v = vnode->browser))
	return;

    if (!(m = v->methods))
	return;

    (*m->init_elements)(vnode);
}


/*
 * Reset the current tree to be the root of the tree.
 */
void
vwr_reset(Viewer *b)
{
	b->current_tree = b->tree;
}


/*
 * Delete a tree node from a given tree.
 */
void
vwr_delete_tree(Vwr v, VNode node)
{
}


/*
 * Traverse the given tree and apply the user-given function
 * to each of the nodes in the tree.
 * If the user-function (*fn) returns FALSE, the traversal
 * aborts.
 */
ViewerNode	*
vwr_traverse_tree
(
    ViewerNode		*tree,
    int			(*fn)(ViewerNode *)
)
{
    ViewerMethods	*m;
    ViewerNode	*child,
    			*rtn_node = NULL,
    			*tmp_node = NULL;
    int			i, num_child;

    if (!tree)
	return (NULL);

    m = BNODE_METHODS(tree);

    num_child = (*m->get_num_children)(tree);

    /*
     * Recursively traverse child subtrees
     * The traverse proc should check for (child == NULL) 
     */
    for (i=0, child = (*m->get_child)(tree, 0); 
            (i < num_child); 
            child = (*m->get_child)(tree, ++i))
    {
        if (tmp_node = vwr_traverse_tree(child, fn))
            rtn_node = tmp_node;
    }

    if ((*fn) (tree))
        return tree;
    else
        return rtn_node;
}

/**************************************************
	Drawing routines for the browser objects.
	PUBLIC FUNCTIONS.
 **************************************************/

#ifdef MATRIX
/*
 * Draw the tree in a matrix form.
 */
void
draw_matrix(b)
	Viewer	*b;
{
	ViewerNode	*node;

	if (!b->current_tree || !b->current_tree->first)
		return;

	recalc_matrix(b);
	for (node = b->current_tree->first; node; node = node->next) {
		render_matrix(b, node, XtWindow(b->canvas), b->select_fn);
	}
}
#endif


/**************************************************
	Drawing routines for the browser objects.
	PRIVATE FUNCTIONS.
 **************************************************/

#ifdef OLD
/*
 * Erase a node.
 */
static void
erase_node
(
    Viewer		*b,
    ViewerNode	*node,
    Pixmap		pixmap
)
{
    Widget	draw_area;

    draw_area = aob_ui_draw_area(b);

    XClearArea(XtDisplay(draw_area), XtWindow(draw_area), node->x, node->y, 
			node->width, node->height, FALSE);
}
#endif


#ifdef MATRIX
/*
 * Recalculate the coordinates of the nodes in a matrix.
 * b->select_fn is applied to each
 * of the nodes in the tree to determine if the node is to be
 * drawn.
 * After this function is called, some of the coordinates stored
 * in the tree node will be changed.  So to switch back to drawing
 * a tree, all the coordinates have to be recalculated. 
 * A temporary list is derived to temporarily link up all the
 * nodes that will be shown.
 */
static void
recalc_matrix(b)
	Viewer	*b;
{
	ViewerNode	*head = NULL;
	int	max_y, width;
	ViewerNode	*node;

	for (node = b->current_tree->last; node; node = node->previous)
		line_up_nodes(b, node, b->select_fn, &head);

	if (!head)
		return;

	XtVaGetValues(b->canvas, XtNwidth, &width, NULL);

	compute_matrix(b, head, X_ORIGIN, Y_ORIGIN, width, &max_y);
	reallocate_canvas(b->canvas, width, max_y);
}


/*
 * Line up the nodes in a linear list.  The next_in_line
 * pointer is used here.
 */
static void
line_up_nodes(b, tree, fn, next)
	Viewer	*b;
	ViewerNode	*tree;
	int	(*fn)();
	ViewerNode	**next;
{
	ViewerNode	*child;

	for (child = tree->last; child; child = child->previous)
		line_up_nodes(b, child, fn, next);

	if (!fn || (*fn) (b, tree)) {
		/*
		 * This is a node of concern.
		 */
		tree->next_in_line = *next;
		*next = tree;
	}
	else {
		tree->next_in_line = NULL;
		/*
		 * This is a node of no concern.  Put
		 * some irrelevant x, y coordinates in
	 	 * the node.
		 */
		tree->icon_x = -1 - tree->icon_width;
		tree->icon_y = -1 - tree->icon_height;
		tree->str_width = -1;
		tree->str_height = -1;
	}
}


/*
 * Compute the coordinates of the nodes if drawn in the
 * matrix form.
 * The tree data structure is good for drawing trees, but
 * bad for drawing matrices.  Well, what can I do?
 */
static void
compute_matrix(b, head, start_x, start_y, x_limit, end_y)
	Viewer	*b;
	ViewerNode	*head;
	int	start_x;
	int	start_y;
	int	x_limit;
	int	*end_y;
{	
	ViewerNode	*node;
	int	x = start_x;
	int	y = start_y;
	int	row_max_height = 0;
	
	for (node = head; node; node = node->next_in_line) 
	{
		browser_node_compute(node, 0, 0);
		if (x + node->width > x_limit) 
		{
			x = start_x;
			y = y + row_max_height + NODE_Y_GAP; 
			row_max_height = 0;
		}
		node->x = x;
		node->y = y; 
		node->icon_x = x + node->icon_x;
		node->icon_y = y + node->icon_y;
		node->str_x = x + node->str_x;
		node->str_y = y + node->str_y;
		x = x + node->width + NODE_X_GAP;
		if (node->height > row_max_height)
			row_max_height = node->height;
	}
	*end_y = y + row_max_height + NODE_Y_GAP;
}


/*
 * Actually draw the matrix of nodes on the screen.
 */
static void
render_matrix(b, tree, pixmap, fn)
	Viewer	*b;
	ViewerNode	*tree;
	Pixmap	pixmap;
	int	(*fn)();
{
	ViewerNode	*child;

	if (!fn || (*fn) (b, tree))
	{
		if (tree && tree->selected)
			browser_node_render(tree, pixmap, TRUE);
		else
			browser_node_render(tree, pixmap, FALSE);
	}

	for (child = tree->first; child; child = child->next)
		render_matrix(b, child, pixmap, fn);
}

#endif


/*
 * Given x and y, determine which node the point is in.
 */
VNode
vwr_locate_node
(
    Vwr		b,
    int		x,
    int		y
)
{
    VNode		selected_node = NULL;
    ViewerMethods	*m;

    if (!b)
	return(NULL);

    m = b->methods;

    /*
    selected_node = r_locate_node(b->current_tree, x, y);
    */
    selected_node = (*m->locate_node)(b->current_tree, x, y);

    return selected_node;
}

#ifdef OLD
/*
 * Draw the selection box for the selected icon.
 */
static void
draw_icon(pixmap, x, y, node)
	Pixmap	pixmap;
	int	x;
	int	y;
	ViewerNode	*node;
{

	int	x_delta = x - node->icon_x;
	int	y_delta = y - node->icon_y;
	XDrawRectangle(dpy, pixmap, xor_gc, 
			node->icon_x + x_delta + ICON_OFFSET,
			node->icon_y + y_delta, 
			node->icon_width - 2 * ICON_OFFSET, 
			node->icon_height);
}


/*
 * Draw an up-pointing arrow at the given point.
 */
static void
draw_arrow(pixmap, x, y)
	Pixmap	pixmap;
	int	x;
	int	y;
{
#define pcount 7

	int	i;
	XPoint plist[pcount];
	static XPoint list[] = {
		{-2, 10}, {-2, 6}, {-6, 6}, {0, 0}, {6, 6}, {2, 6}, {2, 10},
	};

	for (i = 0; i < pcount; i++)
	{
		plist[i] = list[i];
		plist[i].x += x;
		plist[i].y += y;
	}

	XFillPolygon(dpy, pixmap, xor_gc, plist, pcount, Convex, CoordModeOrigin);

#undef pcount
}

#endif

/*
 * vwr_num_cond()
 * Returns:
 *	- the nuber of nodes satisfying the passed 
 *	  condition/function
 */
void
vwr_num_cond(
    VNode	tree,
    int		*num,		/* RETURN */
    int		(*cond)(VNode)
)
{
    VMethods	m;
    VNode	child;
    int		i, 
		num_child;

    if (!tree || !num)
	return;

    m = BNODE_METHODS(tree);

    num_child = (*m->get_num_children)(tree);

    /*
     * Recursively count selected nodes of child subtree
     * This proc should check for (child == NULL) 
     */
    for (i=0, child = (*m->get_child)(tree, 0); 
            (i < num_child); 
            child = (*m->get_child)(tree, ++i))
    {
        vwr_num_cond(child, num, cond);
    }

    /*
     * Increment running total if:
     *	- there is no condition function
     *	- if the condition function returns a non zero
     *    result
     */
    if (!cond || (*cond)(tree))
    {
	++(*num);
    }
}

/*
 * vwr_get_cond()
 * Returns the nodes satisfying the passed condition/function
 * in the passed array of nodes.
 * Returns the number of nodes satisfying the condition in
 * 'num'.
 * Space for the array is allocated. This space must be free'd
 * by the caller of this function.
 */
void
vwr_get_cond(
    VNode	tree,
    VNode	**array,	/* RETURN */
    int		*num,		/* RETURN */
    int		(*cond)(VNode)
)
{
    VMethods	m;
    VNode	child;
    VNode	*cur_array = NULL,
    		*child_array;
    int		child_num,
    		cur_num = 0;
    int		i, 
		num_child;

    /*
     * Return immediately if a NULL root is passed or
     * the count pointer is NULL
     */
    if (!tree || !num)
	return;

    /*
     * Get the methods vector.
     * Return if it is NULL
     */
    if (!(m = BNODE_METHODS(tree)))
	return;

    /*
     * Get child count
     */
    num_child = (*m->get_num_children)(tree);

    /*
     * Search for child nodes satisfying condition first
     */
    for (i=0, child = (*m->get_child)(tree, 0); 
            (i < num_child); 
            child = (*m->get_child)(tree, ++i))
    {
	/*
	 * Reset child count, array
	 */
	child_num = 0;
	child_array = NULL;

	/*
	 * Get nodes from current child subtree satisfying 
	 * condition
	 */
        vwr_get_cond(child, &child_array, &child_num, cond);

	/*
	 * Any nodes satisfied condition ?
	 */
	if (child_num > 0)
	{
	    /*
	     * Have we found any nodes yet ?
	     * If we have,
	     *	cur_array = the array containing the nodes we
	     *      have found so far
	     *	cur_num = the current total
	     */
	    if (cur_num > 0)
	    {
	        VNode	*new_array;

		/*
		 * We need to allocate a bigger array
		 * and copy everything into it
		 */
	        new_array = (VNode *)calloc(sizeof(VNode), 
				cur_num + child_num);

		/*
		 * Copying the current nodes list
		 */
		memcpy(new_array, cur_array, 
			(cur_num * sizeof(VNode)));

		/*
		 * Copying the child nodes list
		 */
		memcpy(&new_array[cur_num], child_array, 
			(child_num * sizeof(VNode)));
		
		/*
		 * Free the old array AND
		 * the child array
		 */
		free((char *)cur_array);
		free((char *)child_array);

		/*
		 * Point current array to this new, 
		 * bigger array
		 */
		cur_array = new_array;

	    }
	    else
	    {
		/*
		 * If we have not found any nodes yet, make the 
		 * returned node the current node
		 */
		cur_array = child_array;

		/*
		 * Paranoia - make sure cur_num is not < 0
		 */
		cur_num = 0;
	    }

	    /*
	     * Increment current node count
	     */
	    cur_num += child_num;
	}

    }

    /*
     * Check if parent node satisfies condition
     */
    if (!cond || (*cond)(tree))
    {
        VNode	*new_array;

	/*
	 * Allocate new array to add one extra node
	 */
        new_array = (VNode *)calloc(sizeof(VNode), 
                                cur_num + 1);

	/*
	 * If we have nodes accumulated from before,
	 * copy those guys into this new array.
	 */
        if (cur_num > 0)
        {
            memcpy(new_array, cur_array, 
                    (cur_num * sizeof(VNode)));

	    /*
	     * Copy the parent node
	     */
            new_array[cur_num] = tree;

	    /*
	     * Free old array
	     */
            free((char *)cur_array);

	    /*
	     * Point current array to this new one
	     */
	    cur_array = new_array;

        }
        else
        {
	    /*
	     * No nodes accumulated so far
	     * Just copy parent node
	     */
            new_array[0] = tree;

	    cur_array = new_array;
        }

	/*
	 * Increment count
	 */
        ++cur_num;
    }

    /*
     * Return total number of nodes found and the array
     * of nodes
     */
    *num = cur_num;
    *array = cur_array;
}

/*
 * Repaint the viewer
 */
void
vwr_repaint
(
    Viewer	*v
)
{
    VMethods	m;

    if (!v || !v->current_tree)
        return;

    m = v->methods;

    (*m->render_tree)(v->current_tree);
}

