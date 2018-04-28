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
 *	$XConsortium: vwr.h /main/3 1995/11/06 17:57:27 rswiston $
 *
 * @(#)vwr.h	1.27 16 Aug 1994
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
 * vwr.h
 *
 * Declarations for the generic object viewer.
 * This file should be standalone.
 */
#ifndef _VWR_H
#define _VWR_H

#ifndef BIT_FIELD 
#define BIT_FIELD(field)        unsigned field : 1 
#endif 

typedef struct _Viewer			*Vwr;  
typedef struct _ViewerNode		*VNode;  
typedef struct _ViewerMethods		*VMethods;  
typedef struct _ViewerNodeElm		*VNodeElm;  
typedef struct _ViewerElmMethods	*VElmMethods;  

typedef struct _ViewerMethods  
{
    /*
     * Initialize/free:
     *	viewer properties
     *	viewer nodes
     *	viewer node elements
     */
    void	(*init_prop)(Vwr);
    void	(*init_ui)(Vwr);
    VNode	(*init_node)(Vwr, void *);
    void	(*init_elements)(VNode);
    void	(*free_prop)(Vwr);
    void	(*free_ui)(Vwr);
    void	(*free_node)(VNode, void *);
    void	(*free_elements)(VNode);

    /*
     * Insert/delete of viewer data into/from client data
     */
    int		(*insert_tree)(Vwr, void *);
    VNode	(*insert_node)(Vwr, void *);
    void	(*remove_tree)(Vwr, void *);
    void	(*remove_node)(Vwr, void *);

    /*
     * Viewer <-> Application obj
     */
    void	(*get_obj_data)(VNode);
    VNode	(*get_viewer_data)(void *);
    void	(*set_viewer_data)(void *, VNode);

    /*
     * For traversals
     */
    VNode	(*get_sibling)(VNode);
    VNode	(*get_parent)(VNode);
    int		(*get_num_children)(VNode);
    VNode	(*get_child)(VNode, int);

    /*
     * Draw operations
     */
    unsigned long	(*get_drawarea)(Vwr);
    void	(*compute_tree)(Vwr, int *, int *);
    void	(*compute_node)(VNode, int, int);
    void	(*render_tree)(VNode);
    void	(*render_node)(VNode, int);
    void	(*render_line)(VNode, VNode);
    void	(*preview_node)(VNode);

    VNode	(*locate_node)(VNode, int, int);
    unsigned long	(*locate_elements)(VNode, int, int);
} ViewerMethods;

#define BNODE_METHODS(node) (node->browser->methods)

/*
 * This is the methods vector for the viewer node elements
 */
typedef struct _ViewerElmMethods	
{
    void	(*init)(VNode, VNodeElm, void *);
    void	(*compute)(VNode, VNodeElm, int, int);
    void	(*render)(VNode, VNodeElm, int);
    void	(*free)(VNode, VNodeElm);
} ViewerElmMethods;  


/*
 * This describes each element of a viewer node.
 * Element here refers to things like class name or instance name.
 * If the viewer node is to contain say:
 *	class name
 *	instance name
 *	object glyph, etc.
 * then each viewer node will contain an array of these structures 
 * to describe each of those elements.
 *
 * For now, there will not be a init/draw method for every 
 * ViewerNodeElm. Instead, the one single init/draw method will 
 * be responsible for intepreting all the ViewerNodeElm's and 
 * initializing/drawing them correctly.
 */
typedef struct _ViewerNodeElm 
{
    int			x;	/* x,y,width,height location of this element */
    int			y;
    unsigned int	width;
    unsigned int	height;
    void		*data;	/* e.g. class/instance name string or pixmap */
} ViewerNodeElm;

/*
 * This describes the viewer in general.
 * It also contains handles to the actual widgets for the viewer.
 */
typedef struct _Viewer 
{
    VMethods	methods;

    void	*obj_data;	/* Pointer back to project data */

    void	*ui_handle;

    /*
     * Viewer properties
     */
    void	*properties;

    VNode	tree;
    VNode	current_tree;

    Vwr		next;
    Vwr		previous;
} Viewer;


/*
 * Description of one viewer node
 */
typedef struct _ViewerNode 
{
    Vwr		browser;	/* points to viewer info */

    void	*obj_data;	/* Pointer back to application object */

    VElmMethods	*elm_methods;
    VNodeElm	elements;	/* Array of node elements */
    int		num_elements;	/* Number of node elements */
    short	boundbox_shown;	/* TRUE if the bounding box of the 
				 * icon is drawn 
				 */


    int		x;		/* x,y,width,height of this node */
    int		y;
    int		width;
    int		height;

    int		state;		/* Can be used for: 
				 * selected/previewed/expanded etc.. 
				 */

    VNode	next; 		/* Points to the next/previous viewer data node */
    VNode	previous;	/* for a particular object */
} ViewerNode;


/*
 * Create/destroy
 */
Vwr			vwr_create(
			    VMethods	m
			);

void			vwr_destroy(
			    Vwr v
			);

VNode			vwr_create_node();

void			vwr_destroy_node(
			    VNode bnode
			);

ViewerNodeElm		vwr_create_elm();

ViewerNodeElm		vwr_destroy_elm(
			    VNodeElm belm
			);

/*
 * Functions that manipulate the tree structure.
 */
void			vwr_destroy_tree(
			    ViewerNode *tree
			);

void			vwr_delete_tree(
			    Viewer *v, 
			    ViewerNode *node
			);

void			vwr_reset(
			    Viewer *v
			);

VNode			vwr_traverse_tree();

void			vwr_init_elements(
			    VNode	vnode
			);

/*
 * Viewer graphics routines.
 */
VNode			vwr_locate_node(
			    Viewer *v, 
			    int x, 
			    int y
			);

void			vwr_get_cond(
			    VNode	tree,
			    VNode	**array,/* RETURN */
			    int		*num,	/* RETURN */
			    int		(*cond)(VNode)
			);

void			vwr_num_cond(
    			    VNode	tree,
    			    int		*num,		/* RETURN */
    			    int		(*cond)(VNode)
			);

void			vwr_repaint(
			    Viewer	*v
			);

#endif /* _VWR_H */
