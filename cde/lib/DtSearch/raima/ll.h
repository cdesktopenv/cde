/* $XConsortium: ll.h /main/2 1996/05/09 04:11:29 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: LLIST_INIT
 *		LL_ELEM_INIT
 *		ll_access
 *		ll_append
 *		ll_deaccess
 *		ll_first
 *		ll_free
 *		ll_next
 *		ll_prepend
 *
 *   ORIGINS: 157
 *
 */
/* The functions in this file are defined in alloc.c */

typedef struct {struct ll_elem FAR *ptr; LOCK_DESC} LL_P;

typedef struct ll_elem {
   LL_P next;
   CHAR_P data;
} ll_elem;

#define LL_ELEM_INIT()	{ POINTER_INIT(), POINTER_INIT() }

typedef struct {
   LL_P head;
   LL_P tail;
   LL_P FAR *curr;
} llist;

#define LLIST_INIT()    { POINTER_INIT(), POINTER_INIT(), NULL }

BOOLEAN ll_access(P1(llist FAR *));
int ll_append(P1(llist FAR *) Pi(CHAR_P FAR *));
#define ll_deaccess(ll)	    /**/
CHAR_P FAR *ll_first(P1(llist FAR *));
void ll_free(P1(llist FAR *));
CHAR_P FAR *ll_next(P1(llist FAR *));
int ll_prepend(P1(llist FAR *) Pi(CHAR_P FAR *));
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ll.h */
