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
 * $XConsortium: FolioObject.C /main/5 1996/10/04 11:23:44 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_FolioObject
#define L_Basic
#include "Prelude.h"

INIT_CLASS (FolioObject);

// private class to hold dependent data
// NOTE: To avoid overhead there should be a class below FolioObject
// to hold the dependency type stuff called "??"
// Need to fix this up later.
// BaseObject -> Object -> Hasbable -> etc.
// This may not be a problem once we have templates, because then we can
// stick anything in our list. 

class Dependent
{
public:
  Dependent (Dependent **pre_next, Dependent *next,
	     FolioObject *dependent, FolioObject *folio_object,
	     notify_handler_t handler,
	     u_int notify_type, void *dependent_data)
  : f_pre_next (pre_next), f_next (next),
    f_dependent (dependent), f_folio_object (folio_object),
    f_handler (handler),
    f_notify_type (notify_type), f_dependent_data (dependent_data)
    { if (next) next->f_pre_next = &f_next; }
  Dependent        **f_pre_next;
  Dependent         *f_next;
  FolioObject       *f_dependent;
  FolioObject       *f_folio_object;
  notify_handler_t   f_handler;
  u_int              f_notify_type;
  void              *f_dependent_data;
  void release();
  void call (FolioObject *obj, void *notify_data);
};

inline void
Dependent::release()
{
  ON_DEBUG(printf ("@** assigning: f_dependent = %p\n", f_next));
  *f_pre_next = f_next;
  if (f_next) f_next->f_pre_next = f_pre_next;
  delete this;
}

inline void
Dependent::call (FolioObject *obj, void *notify_data)
{
  (f_dependent->*f_handler)(obj, f_notify_type, notify_data, f_dependent_data);
}


class DependOnList
{
public:
  DependOnList (DependOnList *next, Dependent *d)
    : f_next (next), f_dependent_obj (d)
  { }
  void delete_dependent();
  DependOnList *f_next;
  Dependent *f_dependent_obj;
};

inline void
DependOnList::delete_dependent()
{
  f_dependent_obj->release();
}


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

FolioObject::FolioObject()
: f_dependents (NULL), f_depend_on_list (NULL)
{
  // Initialize status to reasonable value.
  // Subclasses only need to change on creation if an error occurs.
  setStatus (eSuccess);
  f_initialized = FALSE;
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

FolioObject::~FolioObject()
{
  notify (DESTROYED);
  release_dependents();
  release_depend_on_list();
}


// /////////////////////////////////////////////////////////////////
// notify - this object changed
// /////////////////////////////////////////////////////////////////

void
FolioObject::notify (u_int notify_type, void *notify_data)
{
  Dependent *d, *next;

  // NOTE: This function is temporarily hacked up to get the next
  // entry in the list before calling the function because it
  // might be a destroy message which causes the object notified
  // to destroy itself and delete the current Dependent object,
  // leaving d pointing into freed memory.  next could also point
  // into freed memory, so for now we need to make sure that any
  // notification that deletes the target object is at the end
  // of the list of dependents.  This is done by adding first, since
  // Dependent records are added to the start of the list.
  // The Right fix for this is probably to pull items from the "to check"
  // list and move them to a "called" list as they are called.  After
  // each call, d should start at the beginning of the "to check" list
  // since it will be the only memory guaranteed to be around.
  // DJB 11/10/92 
  for (d = f_dependents; d != NULL;)
    {
      next = d->f_next;
      if (d->f_notify_type == notify_type)
	d->call (this, notify_data);
      d = next;
    }
}


// /////////////////////////////////////////////////////////////////
// add_dependent - add a new dependent
// /////////////////////////////////////////////////////////////////

void
#if defined(SC3) || defined(__osf__) || defined(USL)
FolioObject::add_dependent (FolioObject *dependent, notify_handler_t handler,
#else
FolioObject::add_dependent (void *dependent, notify_handler_t handler,
#endif
			    u_int notify_type, void *dependent_data)
{
  // Adds to begining because that's easiest.
#if defined(SC3) || defined(__osf__) || defined(USL)
  f_dependents = new Dependent (&f_dependents, f_dependents,
				dependent, NULL, handler,
				notify_type, dependent_data);
#else
  f_dependents = new Dependent (&f_dependents, f_dependents,
				(FolioObject*)dependent, NULL, handler,
				notify_type, dependent_data);
#endif
  ON_DEBUG(printf ("@@@2 adding dependent %p to object %p\n",
		   f_dependents, this));
  // Also remember it in the dependent for easy removal if it is deleted.
  // The code below failed in a big way because the "dependent" variable
  // was the real this pointer of the original object, not the offset
  // this pointer for a FolioObject (in a multiply inherited object).
  // The net result was the the code mucked up the object because dependent
  // really wasn't a FolioObject.
  // NOTE: need to change all code in which we're doing semi-bogus stuff
  // with a FolioObject to be a void * so that functions can't be called
  // by mistake like this.  We'll just have to cast it to a FolioObject
  // when the call through it is made and hope the compiler doesn't
  // expect it to point to an actual FolioObject (or whatever, eg: WWL). 
#if defined(SC3) || defined(__osf__) || defined(USL)
  ON_DEBUG(printf ("@@@ adding to depend_on_list of %p\n", dependent));
  dependent->f_depend_on_list =
    new DependOnList (dependent->f_depend_on_list, f_dependents);
#endif
}


// /////////////////////////////////////////////////////////////////
// remove_depend_on
// /////////////////////////////////////////////////////////////////

void
FolioObject::remove_depend_on (FolioObject *target, Dependent *d)
{
  ON_DEBUG(printf ("Calling remove_depend_on for object %p, entry %p\n",
		   target, d));
  DependOnList **l;
  for (l = &(target->f_depend_on_list); *l != NULL; l = &((*l)->f_next))
    {
      ON_DEBUG(printf ("   checking against entry %p\n", *l));
      if ((*l)->f_dependent_obj == d)
	{
	  ON_DEBUG(printf ("removing DependOnList %p\n", *l));
	  DependOnList *dead_meat = *l;
	  *l = (*l)->f_next;
	  delete dead_meat;
	  break;
	}
    }
}


// /////////////////////////////////////////////////////////////////
// remove_dependent - remove first matching dependent
// /////////////////////////////////////////////////////////////////

void
#if defined(SC3) || defined(__osf__) || defined(USL)
FolioObject::remove_dependent (FolioObject *dependent,
#else
FolioObject::remove_dependent (void *dependent,
#endif
			       notify_handler_t handler,
			       u_int notify_type, void *dependent_data)
{
  Dependent **d;

  // This only removes the first matching handler!
  // That means that each handler added must be removed!
  for (d = &f_dependents; *d != NULL; d = &((*d)->f_next))
    {
#if defined(SC3) || defined(__osf__) || defined(USL)
      if ((*d)->f_dependent      == dependent   &&
#else
      if ((*d)->f_dependent      == (FolioObject*)dependent   &&
#endif
	  (*d)->f_handler        == handler     &&
	  (*d)->f_notify_type    == notify_type &&
	  (*d)->f_dependent_data == dependent_data)
	{
	  // Following line is BOGUS because dependent is "real this"
	  // not necessarily a FolioObject!
	  // f_folio_object will be NULL if the Dependent object was
	  // created in add_dependent!  DJB 11/10/92 
	  assert ((*d)->f_folio_object != NULL);
#if defined(SC3) || defined(__osf__) || defined(USL)
	  remove_depend_on (dependent, *d);
#else
	  remove_depend_on ((FolioObject*)dependent, *d);
#endif
	  Dependent *dead_meat = *d;
	  *d = (*d)->f_next;
	  delete dead_meat;
	  break;
	}
    }
}


// /////////////////////////////////////////////////////////////////
// release_dependents - free all dependents
// /////////////////////////////////////////////////////////////////

void
FolioObject::release_dependents()
{
  Dependent *d = f_dependents, *next;

  while (d != NULL)
    {
      // NOTE: Quick fix below.  Improve with doubly-linked lists and
      // cross pointers in Dependent and DependOnList objects.
      // f_folio_object member is part of the quick fix.  DJB 11/10/92 
      if (d->f_folio_object != NULL)
	remove_depend_on (d->f_folio_object, d);
      else
	ON_DEBUG(printf ("WARNING: NULL f_folio_object member in release_dependents\n"));
      next = d->f_next;
      delete d;
      d = next;
    }
  f_dependents = NULL;
}


// /////////////////////////////////////////////////////////////////
// release_depend_on_list
// /////////////////////////////////////////////////////////////////

void
FolioObject::release_depend_on_list()
{
  DependOnList *l = f_depend_on_list, *next;

  while (l != NULL)
    {
      next = l->f_next;
      l->delete_dependent();
      delete l;
      l = next;
    }

  f_depend_on_list = NULL;
}


// /////////////////////////////////////////////////////////////////
// select_on - alternative add_dependent interface
// /////////////////////////////////////////////////////////////////

// NOTE: it would be useful to have this call return a handle to
// the thing we're observing so that we can stop observing just
// by deleting that handle!!! 

void
#if defined(SC3) || defined(__osf__) || defined(USL)
FolioObject::observe (FolioObject *real_this, FolioObject *target,
#else
FolioObject::observe (void *real_this, FolioObject *target,
#endif
		      notify_handler_t handler, u_int notify_type,
		      void *dependent_data)
{
  // This routine will not work, because if target multiply inherits
  // from FolioObject (like Agent) and FolioObject is not first inherited
  // class, the this pointer in this routine will be offset from the
  // real object this pointer, which will cause big problems when
  // we try to call the callback which expects the real this pointer.
  // ---
  // OK, instead we will have to pass in the "real" this pointer which
  // may be different from the this pointer visible in this routine. 
#if defined(SC3) || defined(__osf__) || defined(USL)
  target->f_dependents =
    new Dependent (&target->f_dependents, target->f_dependents,
		   real_this, this,
		   handler, notify_type, dependent_data);
  assert( real_this == this );
#else
  target->f_dependents =
    new Dependent (&target->f_dependents, target->f_dependents,
		   (FolioObject *) real_this, this,
		   handler, notify_type, dependent_data);
#endif
  ON_DEBUG(printf ("@@@ adding dependent %p to object %p for object %p\n",
		   target->f_dependents, target, this));
  f_depend_on_list =
    new DependOnList (f_depend_on_list, target->f_dependents);
}

const char *
FolioObject::display_as()
{
  return "Generic Folio Object"; 
}

