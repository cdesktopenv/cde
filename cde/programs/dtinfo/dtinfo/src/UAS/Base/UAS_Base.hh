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
// $TOG: UAS_Base.hh /main/9 1998/04/17 11:40:28 mgreess $
#ifndef _UAS_Base_h_
#define _UAS_Base_h_

# include "Exceptions.hh"
# include "UAS_Pointer.hh"

class UAS_Base: public Destructable {
    public:
	UAS_Base ();
	virtual ~UAS_Base ();
	int operator == (const UAS_Base &);
#if (defined(sparc) && defined(SC3)) || defined(__linux__)
	/* SC++ 4.0.1 does not like these being protected  */
#else
    protected:
    template <class T> friend class UAS_Pointer;
#endif
	virtual void reference ();
	virtual void unreference ();
    private:
	unsigned int fReferenceCount;
};

#endif
