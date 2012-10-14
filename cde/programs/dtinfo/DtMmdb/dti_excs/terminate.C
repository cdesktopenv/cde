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
// $TOG: terminate.C /main/4 1997/09/05 11:30:17 samborn $
#if !defined(NATIVE_EXCEPTIONS) && !defined(HAS_TERMINATE)
#include "Exceptions.hh"

typedef void (*PFV)();

static PFV _terminate_handler = NULL;

PFV
set_terminate (PFV handler)
{
  PFV old_handler = _terminate_handler;

  _terminate_handler = handler != NULL ? handler : NULL;

  return (old_handler);
}

void
terminate()
{
  if (_terminate_handler != NULL)
    {
      // Reset terminate handler to NULL before handling to
      // avoid potential recursive calls due to exceptions
      // abuse in the terminate handler. 
      PFV handler = _terminate_handler;
      _terminate_handler = NULL;
      mtry
	{
	  (*handler)();
	}
      mcatch_any()
	{
	  Exceptions::error (Exceptions::f_msg_throw_from_terminate,
			     Exceptions::APPLICATION_ERROR);
	}
      end_try;
    }

  abort();
}
#else

#ifdef hpux
// HPUX doesn't define the set_terminate function from the ARM. 
typedef void (*PFV)();
PFV set_terminate (PFV) { return (0); }
#endif

#endif /* NATIVE_EXCEPTIONS */
