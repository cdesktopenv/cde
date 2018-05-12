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
// $XConsortium: MMDB_EmbeddedObject.hh /main/4 1996/06/11 16:43:43 cde-hal $
# ifndef _MMDB_EmbeddedObject_hh_
# define _MMDB_EmbeddedObject_hh_

# include "UAS_EmbeddedObject.hh"
# include "UAS_Collection.hh"
# include "UAS_Exceptions.hh"
# include "MMDB_Common.hh"

class MMDB_EmbeddedObject: public UAS_EmbeddedObject, public MMDB_Common {
    public:
	MMDB_EmbeddedObject (MMDB &, info_base *, const UAS_String &);
	~MMDB_EmbeddedObject ();

    public:
	
	UAS_String locator();
	unsigned int width ();
	unsigned int height ();
	UAS_String data ();
	unsigned int data_length ();
	UAS_String content_type();
	UAS_String title (UAS_TitleType tt = UAS_LONG_TITLE);

	unsigned int llx ();
	unsigned int lly ();
	unsigned int urx ();
	unsigned int ury ();

    protected:
	graphic_smart_ptr fEmbeddedObject;
};

# endif
