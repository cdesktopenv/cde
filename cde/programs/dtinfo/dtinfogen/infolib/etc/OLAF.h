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
