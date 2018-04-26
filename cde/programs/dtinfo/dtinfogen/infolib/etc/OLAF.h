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
/* $XConsortium: OLAF.h /main/4 1996/09/24 16:55:57 cde-hal $
 *
 * OLAF -- OLIAS Architectural Forms
 */

#ifndef __OLAF_h
#define __OLAF_h

#include "SGMLName.h"

class OLAF{
 public:
  /* you can call this whenever you like. Only the first call has
   * any effect. 
   */
  static int init();

  enum {
    /* Architectural form namespaces... */
    OLIAS = SGMLName::qty,
    OL_data,
    OL_id,
    OL_idref,
    OL_scope,
    OL_style,
    OL_Choice,
    OL_ToC, 
    OL_ToCEntry,
    OL_TOClevel,
    OL_Section,
    OL_Title,
    OL_ShortTitle,
    OL_Ignore,
    OL_Graphic,
    OL_Table,
    OL_XRefLabel,
    OL_XRef,

    Example,
    Graphic,
    Index,
    Table,
    Title, 
    BcDesc, 
    BcName,
    BkSTitle, 
    BkTitle, 
    Book, 
    Bookcase, 

    Style, 
    Stylesheet, 
    Tab, 
    Feature, 
    Online,
    Print,
    Path,
    Select,
    FeatureSet,
    FeatureText,
    AutoNumber,
    AutoRef,
    BookAccess,

    /* 
     * The following names are for entities , shouldn't be inside OLAF
     * namespace. But ... 
     */
    lnfeed,
    nbsp,
    amp,
    lt,

    /*
     * The following are attribute names used in the access control element
     */

    VenCode,
    Version,
    Grouping,
    DemoTerms,
    DefaultSection
    
  };
};

#endif /* __OLAF_h */
