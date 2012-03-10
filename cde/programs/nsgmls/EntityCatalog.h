/* $XConsortium: EntityCatalog.h /main/1 1996/07/29 16:50:18 cde-hp $ */
#ifndef EntityCatalog_INCLUDED
#define EntityCatalog_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "StringC.h"
#include "Resource.h"
#include "SubstTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;
class CharsetInfo;
class EntityDecl;

class SP_API EntityCatalog : public Resource {
public:
  class SP_API Syntax {
  public:
    virtual Boolean namecaseGeneral() const = 0;
    virtual Boolean namecaseEntity() const = 0;
    virtual const SubstTable<Char> &upperSubstTable() const = 0;
    virtual const StringC &peroDelim() const = 0;
  };
  virtual ~EntityCatalog();
  virtual Boolean sgmlDecl(const CharsetInfo &,
			   Messenger &,
			   StringC &) const;
  virtual Boolean lookup(const EntityDecl &,
			 const Syntax &,
			 const CharsetInfo &,
			 Messenger &,
			 StringC &) const;
  virtual Boolean lookupPublic(const StringC &,
			       const CharsetInfo &,
			       Messenger &,
			       StringC &) const;
  virtual Boolean defaultDoctype(const CharsetInfo &,
				 Messenger &,
				 StringC &,
				 StringC &) const;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EntityCatalog_INCLUDED */
