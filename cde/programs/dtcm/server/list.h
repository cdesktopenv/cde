/* $XConsortium: list.h /main/4 1995/11/09 12:45:45 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _LIST_H
#define _LIST_H

#include "ansi_c.h"
#include "data.h"
#include "rerule.h"

typedef struct lnode {
	struct lnode	*llink;
	struct lnode	*rlink;
	caddr_t		data;
	time_t		lasttick;
	int		duration;
	RepeatEvent	*re;
} List_node;

typedef struct {
	List_node	*root;
	caddr_t	private;	/* for internal tool state */
} Hc_list;

typedef int(*Destroy_proc)P((caddr_t));

#define	hc_lookup_next(p_node)		(p_node)->rlink
#define	hc_lookup_previous(p_node)	(p_node)->llink

extern Rb_Status	hc_check_list	P((Hc_list*));
extern Hc_list*		hc_create	P((_DtCmsGetKeyProc, _DtCmsCompareProc));
extern List_node	*hc_delete	P((Hc_list*, caddr_t key));
extern List_node	*hc_delete_node	P((Hc_list*, List_node*));
extern void		hc_destroy	P((Hc_list*, Destroy_proc));
extern void		hc_enumerate_down P((Hc_list*, _DtCmsEnumerateProc));
extern Rb_Status	hc_enumerate_up	P((Hc_list*, _DtCmsEnumerateProc));
extern Rb_Status	hc_insert	P((Hc_list*, caddr_t data, caddr_t key,
					RepeatEvent *re, List_node **node_r));
extern caddr_t		hc_lookup	P((Hc_list*, caddr_t key));
extern caddr_t		hc_lookup_largest	P((Hc_list*));
extern caddr_t		hc_lookup_next_larger	P((Hc_list*, caddr_t key));
extern caddr_t		hc_lookup_next_smaller	P((Hc_list*, caddr_t key));
extern caddr_t		hc_lookup_smallest	P((Hc_list*));
extern int		hc_size		P((Hc_list*));
extern List_node	*hc_lookup_node	P((Hc_list*, caddr_t key));
extern Hc_list*		hc_create	P((_DtCmsGetKeyProc, _DtCmsCompareProc));
extern Rb_Status	hc_insert_node	P((Hc_list *, List_node *, caddr_t key));

#endif
