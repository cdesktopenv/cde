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
// $XConsortium: MMDB_EmbeddedObject.cc /main/5 1996/06/11 16:43:38 cde-hal $
# include "MMDB_EmbeddedObject.hh"

MMDB_EmbeddedObject::MMDB_EmbeddedObject (MMDB &theMMDB,
					  info_base *ib,
					  const UAS_String &locator):
				      MMDB_Common (theMMDB, ib),
				      fEmbeddedObject (ib, locator) {
}

MMDB_EmbeddedObject::~MMDB_EmbeddedObject () {
}

UAS_String
MMDB_EmbeddedObject::locator()
{
  return fEmbeddedObject.locator();
}

UAS_String
MMDB_EmbeddedObject::data () {
    return UAS_String (fEmbeddedObject.data(),
			   fEmbeddedObject.data_size(),
			   UAS_NOT_OWNER);
}

unsigned int
MMDB_EmbeddedObject::data_length () {
    return fEmbeddedObject.data_size ();
}

unsigned int
MMDB_EmbeddedObject::width () {
    return fEmbeddedObject.width ();
}

unsigned int
MMDB_EmbeddedObject::height () {
    return fEmbeddedObject.height ();
}

UAS_String
MMDB_EmbeddedObject::content_type() {
    switch (fEmbeddedObject.type()) {
    case GR_TYPE_CGM:
      return UAS_String ("application/CGM", -1, UAS_NOT_OWNER);
    case GR_TYPE_GIF:
      return UAS_String ("image/GIF", -1, UAS_NOT_OWNER);
    case GR_TYPE_JPEG:
      return UAS_String ("image/JPEG", -1, UAS_NOT_OWNER);
    case GR_TYPE_PBM:
      return UAS_String ("image/PBM", -1, UAS_NOT_OWNER);
    case GR_TYPE_PGM:
      return UAS_String ("image/PGM", -1, UAS_NOT_OWNER);
    case GR_TYPE_POSTSCRIPT:
      return UAS_String ("application/POSTSCRIPT", -1, UAS_NOT_OWNER);
    case GR_TYPE_PPM:
      return UAS_String ("image/PPM", -1, UAS_NOT_OWNER);
    case GR_TYPE_TIFF:
      return UAS_String ("image/TIFF", -1, UAS_NOT_OWNER);
    case GR_TYPE_XBM:
      return UAS_String ("image/XBM", -1, UAS_NOT_OWNER);
    case GR_TYPE_XPM:
      return UAS_String ("image/XPM", -1, UAS_NOT_OWNER);
    case GR_TYPE_XWD:
      return UAS_String ("image/XWD", -1, UAS_NOT_OWNER);
    case GR_TYPE_UNKNOWN:
    default:
      return UAS_Common::content_type();
    }
}

UAS_String
MMDB_EmbeddedObject::title (UAS_TitleType) {
    return UAS_String (fEmbeddedObject.title(), -1, UAS_OWNER);
}

unsigned int
MMDB_EmbeddedObject::llx () {
    return fEmbeddedObject.llx ();
}

unsigned int
MMDB_EmbeddedObject::lly () {
    return fEmbeddedObject.lly ();
}

unsigned int
MMDB_EmbeddedObject::urx () {
    return fEmbeddedObject.urx ();
}

unsigned int
MMDB_EmbeddedObject::ury () {
    return fEmbeddedObject.ury ();
}
