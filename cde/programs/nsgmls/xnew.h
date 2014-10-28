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
/* $XConsortium: xnew.h /main/1 1996/07/29 17:10:33 cde-hp $ */
#ifndef xnew_INCLUDED
#define xnew_INCLUDED 1

#ifdef SP_NEW_H_MISSING

typedef void (*VFP)();

#ifdef SP_SET_NEW_HANDLER_EXTERN_C
extern "C" 
#endif
void set_new_handler(VFP);

#ifndef SP_DECLARE_PLACEMENT_OPERATOR_NEW
#define SP_DECLARE_PLACEMENT_OPERATOR_NEW
#endif

#else /* not SP_NEW_H_MISSING */

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <new>
#else
#include <new.h>
#endif

#endif /* not SP_NEW_H_MISSING */

#ifdef SP_DECLARE_PLACEMENT_OPERATOR_NEW

inline
void *operator new(size_t, void *p)
{
    return p;
}

#endif /* SP_DECLARE_PLACEMENT_OPERATOR_NEW */

#endif /* not xnew_INCLUDED */
