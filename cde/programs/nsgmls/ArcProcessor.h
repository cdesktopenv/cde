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
/* $XConsortium: ArcProcessor.h /main/1 1996/07/29 16:46:28 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef ArcProcessor_INCLUDED
#define ArcProcessor_INCLUDED 1

#include "Event.h"
#include "ContentState.h"
#include "Id.h"
#include "NamedTable.h"
#include "Vector.h"
#include "ArcEngine.h"
#include "SgmlParser.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Allocator;

// Processor for a single architecture

class ArcProcessor : private ContentState, private AttributeContext {
public:
  struct MetaMap {
    MetaMap();
    void clear();
    const Attributed *attributed;
    unsigned suppressFlags;
    // #ARCCONT and #CONTENT are handled with a special index
    // list of indexes into element's attlist of architectural attributes
    Vector<unsigned> attMapFrom;
    // corresponding list of indexes in form's attlist
    Vector<unsigned> attMapTo;
  };
  struct MetaMapCache {
    MetaMapCache();
    void clear();
    MetaMap map;
    enum { nNoSpec = 4 };
    // Prerequisites for this cached entry to be valid.
    // The cache is only valid if for each member of noSpec != -1
    // the attribute with that index was not specified (or current)
    unsigned noSpec[nNoSpec];
    unsigned suppressFlags;
    const AttributeList *linkAtts;
  };
  ArcProcessor();
  void setName(const StringC &);
  void init(const EndPrologEvent &,
	    const ConstPtr<Sd> &,
	    const ConstPtr<Syntax> &,
	    const SgmlParser *parser,
	    Messenger *,
	    const Vector<StringC> &superName,
	    ArcDirector &director,
	    SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr);
  // Return 0 if the content is needed, but wasn't supplied
  Boolean processStartElement(const StartElementEvent &,
			      const AttributeList *linkAttributes,
			      const Text *content,
			      Allocator &);
  void processEndElement(const EndElementEvent &,
			 Allocator &);
  // Return true if its architectural.
  Boolean processData();
  const ConstPtr<Dtd> &dtdPointer() const { return metaDtd_; }
  Boolean valid() const { return valid_; }
  void checkIdrefs();
  const StringC &name() const { return name_; }
  EventHandler &docHandler() const { return *docHandler_; }
private:
  ArcProcessor(const ArcProcessor &); // undefined
  void operator=(const ArcProcessor &);	// undefined
  const Syntax &attributeSyntax() const;
  ConstPtr<Notation> getAttributeNotation(const StringC &,
					  const Location &);
  ConstPtr<Entity> getAttributeEntity(const StringC &,
				      const Location &);
  void noteCurrentAttribute(size_t, AttributeValue *);
  ConstPtr<AttributeValue> getCurrentAttribute(size_t) const;
  Boolean defineId(const StringC &, const Location &, Location &);
  void noteIdref(const StringC &, const Location &);
  Id *lookupCreateId(const StringC &);
  void dispatchMessage(const Message &);
  void dispatchMessage(Message &);
  void initMessage(Message &);
  const MetaMap &buildMetaMap(const ElementType *,
			      const Notation *,
			      const AttributeList &,
			      const AttributeList *linkAtts,
			      unsigned suppressFlags);
  void considerSupr(const AttributeList &atts,
		    const AttributeList *linkAtts,
		    unsigned &thisSuppressFlags,
		    unsigned &newSuppressFlags,
		    Boolean &inhibitCache,
		    unsigned &arcSuprIndex);
  void considerIgnD(const AttributeList &atts,
		    const AttributeList *linkAtts,
		    unsigned thisSuppressFlags,
		    unsigned &newSuppressFlags,
		    Boolean &inhibitCache,
		    unsigned &arcSuprIndex);
  const Attributed *considerForm(const AttributeList &atts,
				 const AttributeList *linkAtts,
				 const StringC &name,
				 Boolean isNotation,
				 unsigned thisSuppressFlags,
				 unsigned &newSuppressFlags,
				 Boolean &inhibitCache,
				 unsigned &arcFormIndex);
  const Attributed *autoForm(const AttributeList &atts,
			     const StringC &name,
			     Boolean isNotation,
			     unsigned thisSuppressFlags,
			     unsigned &newSuppressFlags,
			     Boolean &inhibitCache,
			     unsigned &idIndex);
  const Text *considerNamer(const AttributeList &atts,
			    Boolean &inhibitCache,
			    unsigned &arcNamerIndex);
  void buildAttributeMapRename(MetaMap &map,
			       const Text &rename,
			       const AttributeList &atts,
			       const AttributeList *linkAtts,
			       Vector<PackedBoolean> &attRenamed);
  void buildAttributeMapRest(MetaMap &map,
			     const AttributeList &atts,
			     const AttributeList *linkAtts,
			     const Vector<PackedBoolean> &attRenamed);
  Boolean matchName(const StringC &name, const char *key);
  void split(const Text &text,
	     Char space,
	     Vector<StringC> &tokens,
	     Vector<size_t> &tokenPos);
  Boolean mapAttributes(const AttributeList &from,
			const AttributeList *fromLink,
			const Text *content,
			AttributeList &to,
			ConstPtr<AttributeValue> &arcContent,
			const MetaMap &map);
  void initNotationSet(const Location &loc);
  const Attributed *considerNotation(const AttributeList &atts,
				     unsigned thisSuppressFlags,
				     Boolean &inhibitCache,
				     unsigned &notAttIndex);
  void supportAttributes(const AttributeList &);
  void processArcOpts(const AttributeList &atts);
  void processArcQuant(const Text &);
  ConstPtr<Entity> makeDtdEntity(const Notation *);
  void mungeMetaDtd(Dtd &metaDtd, const Dtd &docDtd);
  Boolean mungeDataEntity(ExternalDataEntity &entity);
  void emitArcContent(const Text &text,
		      EventHandler &handler,
		      Allocator &allocator);

  Boolean valid_;
  StringC name_;
  Messenger *mgr_;
  ConstPtr<Dtd> docDtd_;
  ConstPtr<Dtd> metaDtd_;
  ConstPtr<Syntax> docSyntax_;
  ConstPtr<Syntax> metaSyntax_;
  ConstPtr<Sd> docSd_;
  enum ReservedName {
    rArcFormA,
    rArcNamrA,
    rArcSuprA,
    rArcIgnDA,
    rArcDocF,
    rArcSuprF,
    rArcBridF,
    rArcDataF,
    rArcAuto,
    rArcIndr,
    rArcDTD,
    rArcQuant
  };
  enum { nReserve = rArcQuant + 1 };
  StringC supportAtts_[nReserve];
  Boolean arcDtdIsParam_;
  Boolean arcAuto_;
  Vector<StringC> arcOpts_;
  StringC rniContent_;
  StringC rniArcCont_;
  StringC rniDefault_;
  enum {
    isArc = 01,
    suppressForm = 02,
    suppressSupr = 04,
    ignoreData = 010,
    condIgnoreData = 020,
    // recovering from invalid data
    recoverData = 040
    };
  Vector<unsigned> openElementFlags_;
  AttributeList attributeList_;
  NCVector<Owner<struct MetaMapCache> > metaMapCache_;
  MetaMap noCacheMetaMap_;
  NamedTable<Id> idTable_;
  Vector<ConstPtr<AttributeValue> > currentAttributes_;
  ConstPtr<Notation> defaultNotation_;
  Boolean errorIdref_;
  Boolean notationSetArch_;
  ArcDirector *director_;
  EventHandler *docHandler_;
  Owner<EventHandler> ownEventHandler_;
  size_t docIndex_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ArcProcessor_INCLUDED */
