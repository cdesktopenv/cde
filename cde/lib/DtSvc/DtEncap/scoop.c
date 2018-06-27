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
 * File:         scoop.c $XConsortium: scoop.c /main/3 1995/10/26 15:38:19 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h>  /* This must be the first file included */
#include <bms/scoop.h>

static void system_class_init (object_clasp to_do, object_clasp on_behalf_of);
static void system_object_init (object_clasp c, object *p);
static void root_class_init (object_clasp c);
static void root_object_init (object *p);
static object *root_clone (object *this_ptr, object *clone);
static object *root_new (object_clasp c);
static void root_free (object *p);
     
/*--------------------------------------------------------------------------+*/
void object_destroy (object *p)
/*--------------------------------------------------------------------------+*/
{
  memf (p->, free_obj, (p)) ;
}

/*--------------------------------------------------------------------------+*/
object *object_create (object_clasp c)
/*--------------------------------------------------------------------------+*/
{
  object *p ;
  
  if (! c->init) { system_class_init (c, c) ; }
  p = (*(c->new_obj))(c) ;
  if (p)
     object_init (c, p) ;
  return p ;
}
     
/*--------------------------------------------------------------------------+*/
void object_init (object_clasp c,  object *p)
/*--------------------------------------------------------------------------+*/
{
  if (! c->init) { system_class_init (c, c) ; }
  p->class_ptr = c ;
  system_object_init (c, p) ;
  }
  
/*--------------------------------------------------------------------------+*/
static void system_class_init (object_clasp to_do, object_clasp on_behalf_of)
/*--------------------------------------------------------------------------+*/
{
  object_clasp base ;

  if ( on_behalf_of && to_do && ! on_behalf_of->init )
    { }
  else
    return ;

  if (to_do == on_behalf_of && to_do != root_class)
    { on_behalf_of->base =  *((object_clasp *) (on_behalf_of->base)) ;
    } ;
  base = to_do->base ;
  system_class_init (base, base) ;
  system_class_init (base, on_behalf_of) ;

  if (to_do == on_behalf_of)
    { to_do->object_init = root_object_init ;
      to_do->new_obj = root_new ;
      to_do->free_obj = root_free ;
    } ;

#ifdef GLSDEBUG
  printf ("-> '%s' class for '%s'\n",
		to_do->name, on_behalf_of->name) ;
#endif /* GLSDEBUG */

  (*(to_do->class_init))(on_behalf_of);
  if (to_do == on_behalf_of) on_behalf_of->init = TRUE ;
  }
  
/*--------------------------------------------------------------------------+*/
static void system_object_init (object_clasp c, object *p)
/*--------------------------------------------------------------------------+*/
{ 
  if (!c) return ;
  
#ifdef GLSDEBUG
  printf ("   '%s' object for '%s'\n", c->name, p->class_ptr->name) ;
#endif /* GLSDEBUG */
  
  system_object_init (c->base, p) ;
  
#ifdef GLSDEBUG
  printf ("-> '%s' object for '%s'\n", c->name, p->class_ptr->name) ;
#endif /* GLSDEBUG */
  
  (*(c->object_init))(p) ;
}

/*--------------------------------------------------------------------------+*/
static void root_class_init (object_clasp c)
/*--------------------------------------------------------------------------+*/
{
  c->clone = root_clone ;
}

/*--------------------------------------------------------------------------+*/
static void root_object_init (object * UNUSED_PARM(p))
/*--------------------------------------------------------------------------+*/
{
}

/*
  The clone (second) argument to the clone method may be either NULL or an
  object pointer.  If NULL, a space of the size necessary for an object of
  the same class as the primary object (this) will be allocated.  If the
  input argument is not null, it is assumed that the caller has verified
  there is sufficient space for a copy of the original object.
     If the clone procedure does allocate the space for the clone, it only
  allocates the space.  It does not do object initialization.
  */
/*--------------------------------------------------------------------------+*/
static object *root_clone (object *this_ptr, object *clone)
/*--------------------------------------------------------------------------+*/
{
  XeString orig = (XeString) this_ptr ;
  XeString copy ;
  
  object_clasp o_class =	this_ptr->class_ptr ;
  OSizeType	size =		o_class->object_size ;

  if ( ! clone )
    clone = (*(o_class->new_obj))(o_class) ;
  if (clone) {
     copy = (XeString) clone ;
     while (size--)
	*copy++ = *orig++ ;
  }
  return clone ;
}

/*--------------------------------------------------------------------------+*/
static object *root_new (object_clasp c)
/*--------------------------------------------------------------------------+*/
{  object *p = (object *)malloc((unsigned) c->object_size) ;
   
   /* Don't use XeMalloc here, we want to be able to grab our Edit */
   /* widget without the rest of the world being pulled in.        */
   if (!p) 
   {
       fprintf(stderr, "scoop: malloc in root_new failed, out of memory!\n");
       exit(1);
   }
   
   return p ;
}

/*--------------------------------------------------------------------------+*/
static void root_free (object *p)
/*--------------------------------------------------------------------------+*/
{
  if (p) free ((char *)p) ;
}

struct root_class root_class_struct = {
	(object_clasp) NULL,	/* root has no base class */
	"root",			/* class name */
	root_class_init,
	sizeof (object),
	0,
} ;

object_clasp root_class = & root_class_struct ;
