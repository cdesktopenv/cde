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
/* $XConsortium: tree.c /main/4 1995/11/09 12:53:11 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*	2-3-4 tree, a.k.a. red-black tree, implementation  */

#include <EUSCompat.h>
#include <sys/param.h>
#include <unistd.h>
#include <stdlib.h>
#include "tree.h"

extern int debug;

typedef struct {
	int size;		/* used in insert and size */
	int count;		/* used in checktree only */
	Rb_Status status;	/* used in checktree and insert */
	caddr_t data;		/* used in lookups only */
	Tree_node *i;		/* used in insert only */
	caddr_t key;		/* used in insert only */
	Tree_node *d;		/* used in delete only */
	Tree_node *y;		/* dummy that is at both links of z */
	Tree_node *z;		/* dummy used as child of leaf nodes */
	Rb_tree *tree;		/* back link to parent tree */
	_DtCmsGetKeyProc get;
	_DtCmsEnumerateProc enumerate;
	_DtCmsCompareProc compare;
	} Private;

typedef void (*Action_proc)
	(/* Private *private; Tree_node *y, *z, *root */);


static Tree_node *
balance(Tree_node *gg, Tree_node *g, Tree_node *f, Tree_node *x)
{
	Tree_node *t;
	Color tc;
	if (gg == NULL || g == NULL) exit (-1);
	if (f == g->llink) {
	  	if (x == f->rlink) {
	    		f->rlink = x->llink;
	    		x->llink = f;
	    		t = f;
	    		f = x;
	    		x = t;
	    	}
	}
	else {
	  	if (x == f->llink) {
	    		f->llink = x->rlink;
	    		x->rlink = f;
	    		t = f;
	    		f = x;
	    		x = t;
	    	}
	}
	if (x == f->llink) {
	  	g->llink = f->rlink;
	  	f->rlink = g;
	}
	else {
	  	g->rlink = f->llink;
	  	f->llink = g;
	}
	if (g == gg->rlink) gg->rlink = f;
	else gg->llink = f;
	tc = g->color;
	g->color = f->color;
	f->color = tc;
	return(f);
}

static void 
doit(Rb_tree *tree, Action_proc proc)
{
	Private *private;
	Tree_node *root;

	if (tree==NULL) return;
	private = (Private *) tree->private;
	root = tree->root;
	if (root == NULL || root->llink != NULL) {
		private->status = rb_badtable;
		return;
	}
	proc(private, private->y, private->z, root);
}

extern Rb_tree *
rb_create (_DtCmsGetKeyProc get, _DtCmsCompareProc compare)
{
	Private *p;
	Tree_node *root, *y, *z;
	Rb_tree *tree;

	p = (Private *) calloc (1, sizeof(Private));
	p->size = 0;
	p->count = 0;
	p->status = rb_ok;
	p->data = NULL;
	p->i = NULL;
	p->key = 0;
	p->d = NULL;
	p->y = (Tree_node *) calloc (1, sizeof(Tree_node));
	p->z = (Tree_node *) calloc (1, sizeof(Tree_node));
	p->get = get;
	p->enumerate = NULL;
	p->compare = compare;

	root = (Tree_node *) calloc (1, sizeof(Tree_node));
	y = p->y;
	z = p->z;
	tree = (Rb_tree *) calloc (1, sizeof(Rb_tree));
	tree->root = root;
	tree->private = (caddr_t) p;
	p->tree = tree;   /* link back so callbacks can access */
	root->color = black;
	root->llink = NULL;
	root->rlink = z;
	y->color = red;
	y->llink = y->rlink = NULL;
	z->color = black;
	z->llink = z->rlink = y;

	return(tree);
}


extern void
rb_destroy(Rb_tree *tree, _DtCmsEnumerateProc destroy)
{
	Private *p = NULL;
	caddr_t data = NULL;
	Tree_node *node = NULL;
	caddr_t key;

	/* NOTE:there is a client data field
		associated with the tree struct.
		It is up to the client to destroy
		these.				*/
	if (tree==NULL) return;
	p = (Private *) tree->private;
	data = rb_lookup_smallest(tree);

	/* enumerate tree, destroying data */
	while(data != NULL) {
		key = p->get(data);
		node = rb_delete(tree, key);
		if (destroy)
			destroy(data);
		free(node);
		data = rb_lookup_next_larger(tree, key);
	}

	/* destroy the private internal struct */
	free(p->y);
	free(p->z);
	free(p);

	/* destroy the root node */
	free(tree->root);

	/* destroy the tree */
	free(tree);
}

/* ARGSUSED */
static void
size_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	/* dummy proc for monitor */
}

extern int
rb_size(Rb_tree *tree)
{
	Private *p;
	if (tree==NULL) return(0);
	p = (Private *) tree->private;
	if (tree != NULL) {
		doit(tree, size_callback);
		return(p->size);
	}
	else return(0);
}

/* ARGSUSED */
static void
insert_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	Tree_node *x=NULL, *gg=NULL, *g=NULL, *f=NULL;
	_DtCmsComparisonResult c = _DtCmsIsGreater;

	f = root;
	x = f->rlink;
	for (;;) {
		if (x->llink->color == red && x->rlink->color == red) {
			if (x == z) {
				if (c == _DtCmsIsEqual) {
					private->status = rb_duplicate;
					root->rlink->color = black;
					return;
				}
	      			x = private->i;
	      			x->llink = z;
	      			x->rlink = z;
	      			if (c == _DtCmsIsLess) f->llink = x; else f->rlink = x;
	      			c = _DtCmsIsEqual;
	      			private->size++;
	      		}
	    		x->llink->color = black;
	    		x->rlink->color = black;
	    		x->color = red;
	    		if (f->color == red) {
	      			g = balance (gg, g, f, x);
	      			x = g;
	      		}
		}
	  	if (c == _DtCmsIsEqual) break;
	  	gg = g; g = f; f = x;
	  	c = private->compare (private->key, x->data);
	  	if (c==_DtCmsIsEqual) {
			private->status=rb_duplicate;
			root->rlink->color=black;
			return;
		}
	  	x = (c == _DtCmsIsLess) ? x->llink : x-> rlink;
	} 
	root->rlink->color = black;
}

extern Rb_Status
rb_insert_node(Rb_tree *tree, Tree_node *node, caddr_t key)
{
	Private *private;

	if (tree==NULL) return(rb_notable);
	private = (Private *) tree->private;
	private->status = rb_ok;
	private->i = node;
	private->key = key;
	doit (tree, insert_callback);
	return (private->status);
}

extern Rb_Status
rb_insert(Rb_tree *tree, caddr_t data, caddr_t key)
{
	Tree_node *node;

	if (tree==NULL) return(rb_notable);
	node = (Tree_node *)calloc(1, sizeof(Tree_node));
	node->data = data;
	return(rb_insert_node(tree, node, key));
}

static void
delete_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	Tree_node *f, *result, *parent;
	Tree_node *x, *g, *b;
	_DtCmsComparisonResult c;

	f = root;
	x = f->rlink;
	result = NULL;

	if (x == z) return;
	y->color = black;
	if (x->llink->color == black && x->rlink->color == black)
		x->color = red;
	c = private->compare(private->key, x->data);
	if (c == _DtCmsIsEqual) {
		result = x;
		parent = f;
	}
	for (;;) {
		g = f;
	  	f = x;
	  	if (c == _DtCmsIsLess) {
	    		b = x->rlink;
	    		x = x->llink;
	    	}
	  	else {
	    		b = x->llink;
	    		x = x->rlink;
	    	}
	  	if (x != z) {
	    		c = private->compare(private->key, x->data);
	    		if (c == _DtCmsIsEqual) {
	      			result = x; 
	      			parent = f;
	      		}
	    	}
		if (x->color == red || x->llink->color == red ||
	     		x->rlink->color == red) continue;
		if (b->color == red) {
	  		if (b == f->llink) {
	    			f->llink = b->rlink;
	    			b->rlink = f;
	    		}
	  		else {
	    			f->rlink = b->llink;
	    			b->llink = f;
	    		}
	  		f->color = red;
	  		b->color = black;
	  		if (f == g->llink) g->llink = b;
	  		else g->rlink = b;
	  		x = b;
	  		c = private->compare(private->key, x->data);
	  		continue;
		}
		if (x == z) break;
		x->color = red;
		if (b->llink->color == red) {
	  		b->llink->color = black;
	  		x = balance (g, f, b, b->llink);
	  		c = private->compare(private->key, x->data);
	  		continue;
	  	}
		if (b->rlink->color == red) {
	  		b->rlink->color = black;
	  		x = balance(g, f, b, b->rlink);
	  		c = private->compare(private->key, x->data);
	  		continue;
		}
		f->color = black;
		b->color = red;
	} /* end for-loop */ 
	root->rlink->color = black;
	z->color = black;
	y->color = red;
	if (result != NULL) {
		if (g->llink == f) g->llink = z;
	  	else g->rlink = z;
	  	if (f != result) {
	    		if (parent->llink == result) parent->llink = f;
	    		else parent->rlink = f;
	    		f->llink = result->llink;
	    		f->rlink = result->rlink;
	    		f->color = result->color;
	    	}
	  	private->size--;
	} 
	private->d = result;
}

extern Tree_node *
rb_delete(Rb_tree *tree, caddr_t key)
{
	Private *p;
	if (tree==NULL) return((Tree_node *)NULL);
	p = (Private *) tree->private;
	p->key = key;
	p->d = NULL;	/* in case the key is not found */
	doit (tree, delete_callback);
	return(p->d);
}

/* ARGSUSED */
static void
lookup_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	_DtCmsComparisonResult c;
	Tree_node *eq = root->rlink;
	for (;;) {
		if (eq == z) return;
		c = private->compare(private->key, eq->data);
	  	switch(c) {
	    	case _DtCmsIsEqual:
	      		goto bye;
	    	case _DtCmsIsLess:
	      		eq = eq->llink;
	      		break;
	    	case _DtCmsIsGreater:
	      		eq = eq->rlink;
	      		break;
	    	default:
	      		break;
	    	}
	} 
	bye: private->data = eq->data;
}

extern caddr_t
rb_lookup(Rb_tree *tree, caddr_t key)
{
	Private *private;
	if (tree==NULL) return((caddr_t)NULL);
	private = (Private *)tree->private;
	private->key = key;
	private->data = NULL; /* might have been previously used */
	doit (tree, lookup_callback);
	return (private->data);
}

/* ARGSUSED */
static void
lookup_smallest_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	Tree_node *smallest = root->rlink;
	if (smallest == z) return;
	while (smallest->llink != z) {
		  smallest = smallest->llink;
	}
	private->data = smallest->data;
}

extern caddr_t
rb_lookup_smallest(Rb_tree *tree)
{
	Private *private;
	if (tree==NULL) return((caddr_t)NULL);
	private = (Private *)tree->private;
	private->data = NULL; /* might have been previously used */
	doit (tree, lookup_smallest_callback);
	return (private->data);
}

/* ARGSUSED */
static void
next_larger_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	Tree_node *larger = NULL;
	Tree_node *x = root->rlink;
	while (x != z) {
		if (private->compare (private->key, x->data) == _DtCmsIsLess) {
	    		larger = x;
	    		x = x->llink;
		}
		else x= x->rlink;
	}
	if (larger != NULL) private->data = larger->data;
}

extern caddr_t
rb_lookup_next_larger(Rb_tree *tree, caddr_t key)
{
	Private *private;
	if (tree==NULL) return((caddr_t)NULL);
	private = (Private *) tree->private;
	private->key = key;
	private->data = NULL; /* might have been previously used */
	doit (tree, next_larger_callback);
	return(private->data);
}

/* ARGSUSED */
static void
lookup_largest_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	Tree_node *largest = root->rlink;
	if (largest == z) return;
	while (largest->rlink != z) {
		largest = largest->rlink;
	}
	private->data = largest->data;
}

extern caddr_t
rb_lookup_largest(Rb_tree *tree)
{
	Private *private;

	if (tree==NULL) return((caddr_t)NULL);
	private = (Private *) tree->private;
	private->data = NULL; /* might have been previously used */
	doit (tree, lookup_largest_callback);
	return (private->data);
}

/* ARGSUSED */
static void
next_smaller_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{

	Tree_node *smaller = NULL;
	Tree_node *x = root->rlink;
	while (x != z) {
		if (private->compare(private->key, x->data) == _DtCmsIsGreater) {
			smaller = x;
			x = x->rlink;
		}
		else x = x->llink;
	}
	if (smaller != NULL) private->data = smaller->data;
}

extern caddr_t
rb_lookup_next_smaller(Rb_tree *tree, caddr_t key)
{
	Private *private;
	if (tree==NULL) return((caddr_t)NULL);
	private = (Private *) tree->private;
	private->key = key;
	private->data = NULL; /* might have been previously used */
	doit (tree, next_smaller_callback);
	return(private->data);
}

typedef enum {up, down} Direction;

static boolean_t
visit_subtree(Tree_node *node, Private *p, Tree_node *z, Direction dir)
{
	Tree_node *link;
	link = (dir == up) ? node->llink : node->rlink;
	if (link != z && visit_subtree(link, p, z, dir)) return(B_TRUE);
	if (p->enumerate((caddr_t)node, node->data)) return(B_TRUE);
	link = (dir == up) ? node->rlink : node->llink;
	if (link != z) return(visit_subtree(link, p, z, dir));
	else return(B_FALSE);
}

/* ARGSUSED */
static boolean_t
enumerate_up_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	if (root == NULL || root->rlink == z) return (B_FALSE);
	return (visit_subtree(root->rlink, private, z, up));
}

extern Rb_Status
rb_enumerate_up(Rb_tree *tree, _DtCmsEnumerateProc proc)
{
	Private *private;
	if (tree==NULL) return (rb_badtable);
	private = (Private *) tree->private;
	private->enumerate = proc;
	if (tree->root == NULL || tree->root->llink != NULL)
		return rb_badtable;

	if (enumerate_up_callback(private, private->y, private->z, tree->root))
		return (rb_failed);
	else
		return (rb_ok);
}

/* ARGSUSED */
static void
enumerate_down_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	if (root == NULL || root->rlink == z) return;
	(void) visit_subtree(root->rlink, private, z, down);
}

extern void
rb_enumerate_down(Rb_tree *tree, _DtCmsEnumerateProc proc)
{
	Private *private;
	if (tree==NULL) return;
	private = (Private *) tree->private;
	private->enumerate = proc;
	doit (tree, enumerate_down_callback);
}

/* --------------------DEBUGGING-------------------------*/

static int 
assert(int p)
{
	return(p);
}

typedef struct {caddr_t max; int i; boolean_t bool;} Rec;

static void
check1(Tree_node *x, caddr_t max, Tree_node *z, Rec *rec, Private *private)
{
	int dl, dr;
	boolean_t redchild;
	Rec localrec; Rec *localp = &localrec;
	if (x == z) {
		rec->max = max;
	  	rec->i = 0;
	  	rec->bool = B_FALSE;
	  	return;
	}
	check1(x->llink, max, z, localp, private);
	if (private->status == rb_badtable) return;
	max = localp->max;
	dl = localp->i;
	redchild = localp->bool;
	if (!assert (!(redchild && (x->color == red)))) {
		private->status = rb_badtable;
	  	return;
	} 
	if (!assert (private->compare(max, x->data) ==
		     (private->count == 0 ? _DtCmsIsEqual : _DtCmsIsLess))) {
		private->status = rb_badtable;
	  	return;
	}
	private->count++;
	check1(x->rlink, private->get(x->data), z, localp, private);
	if (private->status == rb_badtable) return;
	max = localp->max;
	dr = localp->i;
	redchild = localp->bool;
	if (!assert (!(redchild && (x->color == red)))) {
		private->status = rb_badtable;
	  	return;
	}
	if (!assert (dl == dr)) {
	  	private->status = rb_badtable;
	  	return;
	}
	rec->max = max;
	rec->i = dl + ((x->color == black) ? 1 : 0);
	rec->bool = ((x->color == red) ? B_TRUE : B_FALSE);
}

static void
check_tree_callback(Private *private, Tree_node *y, Tree_node *z, Tree_node *root)
{
	if (!assert (z->llink == y)) {
		private->status = rb_badtable;
		return;
	}
	if (!assert (z->rlink == y)) {
		private->status = rb_badtable;
		return;
	}
	if (root->rlink != z) {
		Rec localrec;
		Rec *localp = &localrec;
		Tree_node *smallest = root->rlink;
		while (smallest->llink != z) {
			smallest = smallest->llink;
		}
		check1(root->rlink, private->get(smallest->data), z, localp, private);
		if (private->status == rb_badtable) return;
	}
}

extern Rb_Status
rb_check_tree(Rb_tree *tree)
{
	Private *p;
	if (tree==NULL) return(rb_notable);
	p = (Private *) tree->private;
	p->status = rb_ok;
	p->count = 0;
	doit (tree, check_tree_callback);
	return(p->status);
}
