// $XConsortium: MMDB.hh /main/6 1996/09/26 16:48:45 cde-hal $
# ifndef _MMDB_hh_
# define _MMDB_hh_

# include "UAS_String.hh"

# include "oliasdb/olias_consts.h"
# include "oliasdb/mmdb.h"
# include "oliasdb/collectionIterator.h"

class MMDB
{
public:
  MMDB(const UAS_String &infolibPath);
  ~MMDB();
  OLIAS_DB *database();
  info_lib *infolib () { return f_infoLib; }
  info_base *infobase (const char *locator);
  MMDB &mmdb()
    { return (*this); }

  UAS_String infoLibPath ();
  UAS_String infoLibUid ();

private:
  int          f_initialized;
  OLIAS_DB	*f_oliasDB;
  info_lib	*f_infoLib;
  UAS_String	fInfoLibPath;
};

#endif
