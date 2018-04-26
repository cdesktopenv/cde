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
// $XConsortium: Destructable.C /main/8 1996/10/04 12:48:45 cde-hal $
#ifndef NATIVE_EXCEPTIONS
#include "Exceptions.hh"


void *Destructable::g_stack_start;
unsigned short Destructable::g_size;

#ifdef C_API
NEW_AND_DELETE_BODIES_SIMPLE(Destructable)
#endif

#ifdef DEBUG
#ifdef NOPE
struct Init
{
  Init() {}
};

static Init I;
#endif
#endif


// /////////////////////////////////////////////////////////////////
// in_stack - return true if ptr is in stack memory
// /////////////////////////////////////////////////////////////////

int
Destructable::in_stack()
{
  char stack_end;
  PRINTF (("In_stack : %p -- %p  |  %p  ",
	   g_stack_start, &stack_end, this));
  
  if (((void *) this > &stack_end && (void *) this < g_stack_start) ||
      ((void *) this < &stack_end && (void *) this > g_stack_start))
    {
      PRINTF (("(stack)\n"));
      return (1);
    }
  else
    {
      PRINTF (("(heap)\n"));
      return (0);
    }
}


// /////////////////////////////////////////////////////////////////
// in_stack_set_size - return 1 if in the stack, set size as well
// /////////////////////////////////////////////////////////////////

int
Destructable::in_stack_set_size()
{
  char stack_end;
  PRINTF (("In_stack_size : %p -- %p  |  %p  ",
	   g_stack_start, &stack_end, this));

  // Handle stack grows down case.
  if ((void *) this > (void *) &stack_end &&
      (void *) this < (void *) g_stack_start)
    {
      PRINTF (("(stack)\n"));
      // Size unknown, also used to indicate stack grows down.
      Destructable::g_size = 0;
      return (1);
    }
  // Handle stack grows up case. 
  else if ((void *) this > (void *) g_stack_start &&
	   (void *) this < (void *) &stack_end)
    {
      // stack_end is just past this, so size is space between them. 
      Destructable::g_size = (unsigned long) &stack_end - (unsigned long) this;
#ifdef foobar
      g_size -=
#endif
      PRINTF (("(stack)\n"));
      PRINTF (("  obj start = %p, end = %p, size = %d\n",
	       this, &stack_end, g_size));

      return (1);
    }
  else
    {
      PRINTF (("(heap)\n"));
      return (0);
    }
}

#endif /* NATIVE_EXCEPTIONS */
