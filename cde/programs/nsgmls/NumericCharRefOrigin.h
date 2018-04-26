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
/* $XConsortium: NumericCharRefOrigin.h /main/1 1996/07/29 16:59:01 cde-hp $ */
#ifndef NumericCharRefOrigin_INCLUDED
#define NumericCharRefOrigin_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Location.h"
#include "Markup.h"
#include "Owner.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class NumericCharRefOrigin : public Origin {
public:
  NumericCharRefOrigin(const Location &start, Index endIndex,
		       Owner<Markup> &markup);
  const Location &parent() const;
  Index refLength() const;
  Boolean isNumericCharRef(const Markup *&) const;
private:
  NumericCharRefOrigin(const NumericCharRefOrigin &); // undefined
  void operator=(const NumericCharRefOrigin &);	      // undefined
  Location start_;
  Index refLength_;
  Owner<Markup> markup_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not NumericCharRefOrigin_INCLUDED */
