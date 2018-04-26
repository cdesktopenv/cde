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
/* $XConsortium: ExtendEntityManager.C /main/1 1996/07/29 16:51:42 cde-hp $ */
// Copyright (c) 1994, 1995, 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "ExtendEntityManager.h"
#include "Message.h"
#include "MessageArg.h"
#include "OffsetOrderedList.h"
#include "rtti.h"
#include "StorageManager.h"
#include "Vector.h"
#include "NCVector.h"
#include "Owner.h"
#include "RegisteredCodingSystem.h"
#include "constant.h"
#include "EntityManagerMessages.h"
#include "StorageObjectPosition.h"
#include "Owner.h"
#include "CodingSystem.h"
#include "InputSource.h"
#include "macros.h"
#include "EntityCatalog.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#ifdef DECLARE_MEMMOVE
extern "C" {
  void *memmove(void *, const void *, size_t);
}
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

const char EOFCHAR = '\032';	// Control-Z

class ExternalInputSource;

class EntityManagerImpl : public ExtendEntityManager {
public:
  EntityManagerImpl(StorageManager *defaultStorageManager,
		    const InputCodingSystem *defaultCodingSystem);
  void setCatalogManager(CatalogManager *catalogManager);
  void registerStorageManager(StorageManager *);
  void registerCodingSystem(const char *, const InputCodingSystem *);
  InputSource *open(const StringC &sysid,
		    const CharsetInfo &,
		    InputSourceOrigin *,
		    Boolean mayRewind,
		    Messenger &);
  InputSource *openIfExists(const StringC &sysid,
			    const CharsetInfo &,
			    InputSourceOrigin *,
			    Boolean mayRewind,
			    Messenger &);
  ConstPtr<EntityCatalog> makeCatalog(StringC &systemId,
						  const CharsetInfo &charset,
						  Messenger &mgr);
  Boolean expandSystemId(const StringC &,
			 const Location &,
			 Boolean isNdata,
			 const CharsetInfo &,
			 const StringC *,
			 Messenger &,
			 StringC &);
  Boolean mergeSystemIds(const Vector<StringC> &,
			 Boolean mapCatalogDocument,
			 const CharsetInfo &,
			 Messenger &mgr,
			 StringC &) const;
  StorageManager *lookupStorageType(const StringC &, const CharsetInfo &) const;
  StorageManager *lookupStorageType(const char *) const;
  StorageManager *guessStorageType(const StringC &, const CharsetInfo &) const;
  const InputCodingSystem *lookupCodingSystem(const StringC &,
					      const CharsetInfo &,
					      const char *&) const;
  Boolean resolveSystemId(const StringC &str,
			  const CharsetInfo &idCharset,
			  Messenger &mgr,
			  const Location &defLocation,
			  Boolean isNdata,
			  ParsedSystemId &parsedSysid) const;
  Boolean parseSystemId(const StringC &str,
			const CharsetInfo &idCharset,
			Boolean isNdata,
			const StorageObjectSpec *defSpec,
			Messenger &mgr,
			ParsedSystemId &parsedSysid) const;
private:
  EntityManagerImpl(const EntityManagerImpl &); // undefined
  void operator=(const EntityManagerImpl &); // undefined
  static const StorageObjectSpec *defStorageObject(const Location &);
  static Boolean matchKey(const StringC &type, const char *s,
			  const CharsetInfo &docCharset);
  NCVector<Owner<StorageManager> > storageManagers_;
  Vector<RegisteredCodingSystem> codingSystems_;
  Owner<StorageManager> defaultStorageManager_;
  const InputCodingSystem *defaultCodingSystem_;
  Owner<CatalogManager> catalogManager_;
  friend class FSIParser;
};

class ExternalInfoImpl : public ExternalInfo {
  RTTI_CLASS
public:
  ExternalInfoImpl(ParsedSystemId &parsedSysid);
  const StorageObjectSpec &spec(size_t i) const;
  size_t nSpecs() const;
  const ParsedSystemId &parsedSystemId() const;
  void noteRS(Offset);
  void noteStorageObjectEnd(Offset);
  void noteInsertedRSs();
  void setDecoder(size_t i, Decoder *);
  StringC &id(size_t i);
  Boolean convertOffset(Offset, StorageObjectLocation &) const;
private:
  ParsedSystemId parsedSysid_;
  NCVector<StorageObjectPosition> position_;
  size_t currentIndex_;
  // list of inserted RSs
  OffsetOrderedList rsList_;
  Boolean notrack_;
};

class ExternalInputSource : public InputSource {
public:
  ExternalInputSource(ParsedSystemId &parsedSysid,
		      InputSourceOrigin *origin,
		      Boolean mayRewind,
		      Boolean mayNotExist = 0);
  void pushCharRef(Char, const NamedCharRef &);
  ~ExternalInputSource();
private:
  Xchar fill(Messenger &);
  Boolean rewind(Messenger &);
  void willNotRewind();

  void init();
  void noteRS();
  void noteRSAt(const Char *);
  void reallocateBuffer(size_t size);
  void insertChar(Char);
  static const Char *findNextCr(const Char *start, const Char *end);
  static const Char *findNextLf(const Char *start, const Char *end);
  static const Char *findNextCrOrLf(const Char *start, const Char *end);

  ExternalInfoImpl *info_;
  Char *buf_;
  const Char *bufLim_;
  Offset bufLimOffset_;
  size_t bufSize_;
  size_t readSize_;
  NCVector<Owner<StorageObject> > sov_;
  StorageObject *so_;
  size_t soIndex_;
  Boolean insertRS_;
  Decoder *decoder_;
  const char *leftOver_;
  size_t nLeftOver_;
  Boolean mayRewind_;
  Boolean mayNotExist_;
  enum RecordType {
    unknown,
    crUnknown,
    crlf,
    lf,
    cr,
    asis
    };
  RecordType recordType_;
  Boolean zapEof_;
};

class FSIParser {
public:
  FSIParser(const StringC &, const CharsetInfo &idCharset,
	    Boolean isNdata,
	    const StorageObjectSpec *defSpec,
	    const EntityManagerImpl *em,
	    Messenger &mgr);
  Boolean parse(ParsedSystemId &parsedSysid);
  static const char *recordsName(StorageObjectSpec::Records records);
  struct RecordType {
    const char *name;
    StorageObjectSpec::Records value;
  };
private:
  Boolean handleInformal(size_t startIndex, ParsedSystemId &parsedSysid);
  Boolean convertId(StringC &, Xchar smcrd, const StorageManager *);
  Xchar get();
  void unget();
  StorageManager *lookupStorageType(const StringC &key, Boolean &neutral);
  Boolean matchKey(const StringC &, const char *);
  Boolean matchChar(Xchar, char);
  Boolean isS(Xchar);
  Boolean convertDigit(Xchar c, int &weight);
  void uncharref(StringC &);
  Boolean setAttributes(StorageObjectSpec &sos, Boolean neutral,
			Xchar &smcrd, Boolean &fold);
  Boolean setCatalogAttributes(ParsedSystemId &parsedSysid);
  void setDefaults(StorageObjectSpec &sos);
  Boolean parseAttribute(StringC &token, Boolean &gotValue, StringC &value);
  Boolean lookupRecords(const StringC &token, StorageObjectSpec::Records &);
  void convertMinimumLiteral(const StringC &from, StringC &to);

  const StringC &str_;
  size_t strIndex_;
  Messenger &mgr_;
  const EntityManagerImpl *em_;
  const StorageObjectSpec *defSpec_;
  const CharsetInfo &idCharset_;
  Boolean isNdata_;
  static RecordType recordTypeTable[];
};

const Char RS = '\n';
const Char RE = '\r';
const char lineEnd = '\n';

ExtendEntityManager::CatalogManager::~CatalogManager()
{
}

ExtendEntityManager *ExtendEntityManager::make(StorageManager *sm,
					       const InputCodingSystem *cs)
{
  return new EntityManagerImpl(sm, cs);
}

Boolean ExtendEntityManager::externalize(const ExternalInfo *info,
					 Offset off,
					 StorageObjectLocation &loc)
{
  if (!info)
    return false;
  const ExternalInfoImpl *p = DYNAMIC_CAST_CONST_PTR(ExternalInfoImpl, info);
  if (!p)
    return false;
  return p->convertOffset(off, loc);
}

const ParsedSystemId *
ExtendEntityManager::externalInfoParsedSystemId(const ExternalInfo *info)
{
  if (!info)
    return 0;
  const ExternalInfoImpl *p = DYNAMIC_CAST_CONST_PTR(ExternalInfoImpl, info);
  if (!p)
    return 0;
  return &p->parsedSystemId();
}


EntityManagerImpl::EntityManagerImpl(StorageManager *defaultStorageManager,
				     const InputCodingSystem *defaultCodingSystem)
: defaultStorageManager_(defaultStorageManager),
  defaultCodingSystem_(defaultCodingSystem)
{
}

InputSource *EntityManagerImpl::open(const StringC &sysid,
				     const CharsetInfo &docCharset,
				     InputSourceOrigin *origin,
				     Boolean mayRewind,
				     Messenger &mgr)
{
  ParsedSystemId parsedSysid;
  if (!parseSystemId(sysid, docCharset, 0, 0, mgr, parsedSysid)
      || !catalogManager_->mapCatalog(parsedSysid, this, mgr))
    return 0;
  return new ExternalInputSource(parsedSysid, origin, mayRewind, 0);
}

InputSource *EntityManagerImpl::openIfExists(const StringC &sysid,
					     const CharsetInfo &docCharset,
					     InputSourceOrigin *origin,
					     Boolean mayRewind,
					     Messenger &mgr)
{
  ParsedSystemId parsedSysid;
  if (!parseSystemId(sysid, docCharset, 0, 0, mgr, parsedSysid)
      || !catalogManager_->mapCatalog(parsedSysid, this, mgr))
    return 0;
  return new ExternalInputSource(parsedSysid, origin, mayRewind, 1);
}

ConstPtr<EntityCatalog>
EntityManagerImpl::makeCatalog(StringC &systemId,
			       const CharsetInfo &charset,
			       Messenger &mgr)
{
  return catalogManager_->makeCatalog(systemId, charset, this, mgr);
}

Boolean
EntityManagerImpl::mergeSystemIds(const Vector<StringC> &sysids,
				  Boolean mapCatalogDocument,
				  const CharsetInfo &charset,
				  Messenger &mgr,
				  StringC &result) const
{
  ParsedSystemId parsedSysid;
  if (mapCatalogDocument) {
    parsedSysid.maps.resize(parsedSysid.maps.size() + 1);
    parsedSysid.maps.back().type = ParsedSystemIdMap::catalogDocument;
  }
  for (size_t i = 0; i < sysids.size(); i++)
    if (!parseSystemId(sysids[i],
		       charset,
		       0,
		       0,
		       mgr,
		       parsedSysid))
      return 0;
  parsedSysid.unparse(charset, result);
  return 1;
}

Boolean
EntityManagerImpl::expandSystemId(const StringC &str,
				  const Location &defLocation,
				  Boolean isNdata,
				  const CharsetInfo &charset,
				  const StringC *mapCatalogPublic,
				  Messenger &mgr,
				  StringC &result)
{
  ParsedSystemId parsedSysid;
  const StorageObjectSpec *defSpec = defStorageObject(defLocation);
  if (!parseSystemId(str, charset, isNdata, defSpec, mgr, parsedSysid))
    return 0;
  if (mapCatalogPublic) {
    ParsedSystemIdMap map;
    map.type = ParsedSystemIdMap::catalogPublic;
    map.publicId = *mapCatalogPublic;
    parsedSysid.maps.insert(parsedSysid.maps.begin(), 1, map);
  }
  parsedSysid.unparse(charset, result);
  return 1;
}

Boolean EntityManagerImpl::parseSystemId(const StringC &str,
					 const CharsetInfo &idCharset,
					 Boolean isNdata,
					 const StorageObjectSpec *defSpec,
					 Messenger &mgr,
					 ParsedSystemId &parsedSysid) const
{
  FSIParser fsiParser(str, idCharset, isNdata, defSpec, this, mgr);
  return fsiParser.parse(parsedSysid);
}

StorageManager *
EntityManagerImpl::guessStorageType(const StringC &type,
				    const CharsetInfo &docCharset) const
{
  for (size_t i = 0; i < storageManagers_.size(); i++)
    if (storageManagers_[i]->guessIsId(type, docCharset))
      return storageManagers_[i].pointer();
  if (defaultStorageManager_->guessIsId(type, docCharset))
    return defaultStorageManager_.pointer();
  return 0;
}

StorageManager *
EntityManagerImpl::lookupStorageType(const StringC &type,
				     const CharsetInfo &docCharset) const
{
  if (type.size() == 0)
    return 0;
  if (matchKey(type, defaultStorageManager_->type(), docCharset))
    return defaultStorageManager_.pointer();
  for (size_t i = 0; i < storageManagers_.size(); i++)
    if (matchKey(type, storageManagers_[i]->type(), docCharset))
      return storageManagers_[i].pointer();
  return 0;
}

StorageManager *
EntityManagerImpl::lookupStorageType(const char *type) const
{
  if (type == defaultStorageManager_->type())
    return defaultStorageManager_.pointer();
  for (size_t i = 0; i < storageManagers_.size(); i++)
    if (type == storageManagers_[i]->type())
      return storageManagers_[i].pointer();
  return 0;
}

const InputCodingSystem *
EntityManagerImpl::lookupCodingSystem(const StringC &type,
				      const CharsetInfo &docCharset,
				      const char *&name) const
{
  for (size_t i = 0; i < codingSystems_.size(); i++)
    if (matchKey(type, codingSystems_[i].name, docCharset)) {
      name = codingSystems_[i].name;
      return codingSystems_[i].ics;
    }
  return 0;
}

Boolean
EntityManagerImpl::matchKey(const StringC &type,
			    const char *s,
			    const CharsetInfo &docCharset)
{
  if (strlen(s) != type.size())
    return false;
  for (size_t i = 0; i < type.size(); i++)
    if (docCharset.execToDesc(toupper(s[i])) != type[i]
	&& docCharset.execToDesc(tolower(s[i])) != type[i])
      return false;
  return true;
}

void EntityManagerImpl::registerStorageManager(StorageManager *sm)
{
  storageManagers_.resize(storageManagers_.size() + 1);
  storageManagers_.back() = sm;
}

void EntityManagerImpl::registerCodingSystem(const char *name,
					     const InputCodingSystem *ics)
{
  codingSystems_.resize(codingSystems_.size() + 1);
  RegisteredCodingSystem &rcs = codingSystems_.back();
  rcs.name = name;
  rcs.ics = ics;
}

void EntityManagerImpl::setCatalogManager(CatalogManager *catalogManager)
{
  catalogManager_ = catalogManager;
}

const StorageObjectSpec *
EntityManagerImpl::defStorageObject(const Location &defLocation)
{
  Offset off;
  const ExternalInfo *info;
  Location loc(defLocation);
  for (;;) {
    if (loc.origin().isNull())
      return 0;
    const InputSourceOrigin *inputSourceOrigin = loc.origin()->asInputSourceOrigin();
    if (inputSourceOrigin) {
      off = inputSourceOrigin->startOffset(loc.index());
      info = inputSourceOrigin->externalInfo();
      if (info)
	break;
      if (!inputSourceOrigin->defLocation(off, loc))
	return 0;
    }
    else
      loc = loc.origin()->parent();
  }
  StorageObjectLocation soLoc;
  if (!ExtendEntityManager::externalize(info, off, soLoc))
    return 0;
  return soLoc.storageObjectSpec;
}

ExternalInputSource::ExternalInputSource(ParsedSystemId &parsedSysid,
					 InputSourceOrigin *origin,
					 Boolean mayRewind,
					 Boolean mayNotExist)
: InputSource(origin, 0, 0),
  mayRewind_(mayRewind),
  mayNotExist_(mayNotExist),
  sov_(parsedSysid.size()),
  readSize_(0),
  decoder_(NULL),
  recordType_(unknown),
  zapEof_(false)
{
  init();
  info_ = new ExternalInfoImpl(parsedSysid);
  origin->setExternalInfo(info_);
}

void ExternalInputSource::init()
{
  so_ = 0;
  buf_ = 0;
  bufSize_ = 0;
  bufLim_ = 0;
  bufLimOffset_ = 0;
  insertRS_ = true;
  soIndex_ = 0;
  leftOver_ = 0;
  nLeftOver_ = 0;  
}

ExternalInputSource::~ExternalInputSource()
{
  if (buf_)
    delete [] buf_;
}

Boolean ExternalInputSource::rewind(Messenger &mgr)
{
  reset(0, 0);
  if (buf_)
    delete [] buf_;
  // reset makes a new EntityOrigin
  ParsedSystemId parsedSysid(info_->parsedSystemId());
  info_ = new ExternalInfoImpl(parsedSysid);
  inputSourceOrigin()->setExternalInfo(info_);
  so_ = 0;
  for (size_t i = 0; i < soIndex_; i++) {
    if (sov_[i] && !sov_[i]->rewind(mgr))
      return 0;
  }
  init();
  return 1;
}

void ExternalInputSource::willNotRewind()
{
  for (size_t i = 0; i < soIndex_; i++)
    if (sov_[i])
      sov_[i]->willNotRewind();
  mayRewind_ = 0;
}

// Round up N so that it is a power of TO.
// TO must be a power of 2.

inline
size_t roundUp(size_t n, size_t to)
{
  return (n + (to - 1)) & ~(to - 1);
}

inline
void ExternalInputSource::noteRSAt(const Char *p)
{
  info_->noteRS(bufLimOffset_ - (bufLim_ - p));
}

inline
void ExternalInputSource::noteRS()
{
  noteRSAt(cur());
}

Xchar ExternalInputSource::fill(Messenger &mgr)
{
  ASSERT(cur() == end());
  while (end() >= bufLim_) {
    // need more data
    while (so_ == 0) {
      if (soIndex_ >= sov_.size())
	return eE;
      if (soIndex_ > 0)
	info_->noteStorageObjectEnd(bufLimOffset_ - (bufLim_ - end()));
      const StorageObjectSpec &spec = info_->spec(soIndex_);
      if (mayNotExist_) {
	NullMessenger nullMgr;
	sov_[soIndex_]
	  = spec.storageManager->makeStorageObject(spec.specId, spec.baseId,
						   spec.search,
						   mayRewind_, nullMgr,
						   info_->id(soIndex_));
      }
      else
	sov_[soIndex_]
	  = spec.storageManager->makeStorageObject(spec.specId, spec.baseId,
						   spec.search,
						   mayRewind_, mgr,
						   info_->id(soIndex_));
      so_ = sov_[soIndex_].pointer();
      if (so_) {
	decoder_ = spec.codingSystem->makeDecoder();
	info_->setDecoder(soIndex_, decoder_);
	zapEof_ = spec.zapEof;
	switch (spec.records) {
	case StorageObjectSpec::asis:
	  recordType_ = asis;
	  insertRS_ = false;
	  break;
	case StorageObjectSpec::cr:
	  recordType_ = cr;
	  break;
	case StorageObjectSpec::lf:
	  recordType_ = lf;
	  break;
	case StorageObjectSpec::crlf:
	  recordType_ = crlf;
	  break;
	case StorageObjectSpec::find:
	  recordType_ = unknown;
	  break;
	default:
	  CANNOT_HAPPEN();
	}
	soIndex_++;
	readSize_ = so_->getBlockSize();
	nLeftOver_ = 0;
	break;
      }
      else
	setAccessError();
      soIndex_++;
    }

    size_t keepSize = end() - start();
    const size_t align = sizeof(int)/sizeof(Char);
    size_t readSizeChars = (readSize_ + (sizeof(Char) - 1))/sizeof(Char);
    readSizeChars = roundUp(readSizeChars, align);
    size_t neededSize;		// in Chars
    size_t startOffset;
    // compute neededSize and readSize
    unsigned minBytesPerChar = decoder_->minBytesPerChar();
    if (nLeftOver_ == 0 && minBytesPerChar >= sizeof(Char)) {
      // In this case we want to do decoding in place.
      // FIXME It might be a win on some systems (Irix?) to arrange that the
      // read buffer is on a page boundary.

      if (keepSize >= size_t(-1)/sizeof(Char) - (align - 1) - insertRS_)
	abort();			// FIXME throw an exception
      
      // Now size_t(-1)/sizeof(Char) - (align - 1) - insertRS_ - keepSize > 0
      if (readSizeChars
	  > size_t(-1)/sizeof(Char) - (align - 1) - insertRS_ - keepSize)
	abort();
      neededSize = roundUp(readSizeChars + keepSize + insertRS_, align);
      startOffset = ((neededSize > bufSize_ ? neededSize : bufSize_)
		     - readSizeChars - insertRS_ - keepSize);
    }
    else {
      // Needs to be room for everything before decoding.
      neededSize = (keepSize + insertRS_ + readSizeChars
		    + (nLeftOver_ + sizeof(Char) - 1)/sizeof(Char));
      // Also must be room for everything after decoding.
      size_t neededSize2
	= (keepSize + insertRS_
	   // all the converted characters
	   + (nLeftOver_ + readSize_)/minBytesPerChar
	   // enough Chars to contain left over bytes
	   + ((readSize_ % minBytesPerChar + sizeof(Char) - 1)
	      / sizeof(Char)));
      if (neededSize2 > neededSize)
	neededSize = neededSize2;
      neededSize = roundUp(neededSize, align);
      if (neededSize > size_t(-1)/sizeof(Char))
	abort();
      startOffset = 0;
    }
    if (bufSize_ < neededSize)
      reallocateBuffer(neededSize);
    Char *newStart = buf_ + startOffset;
    if (newStart != start() && keepSize > 0)
      memmove(newStart, start(), keepSize*sizeof(Char));
    char *bytesStart = (char *)(buf_ + bufSize_ - readSizeChars) - nLeftOver_;
    if (nLeftOver_ > 0 && leftOver_ != bytesStart)
      memmove(bytesStart, leftOver_, nLeftOver_);
    moveStart(newStart);
    bufLim_ = end();

    size_t nread;
    if (so_->read((char *)(buf_ + bufSize_ - readSizeChars), readSize_,
		  mgr, nread)) {
      if (nread > 0) {
	const char *bytesEnd = bytesStart + nLeftOver_ + nread;
	size_t nChars = decoder_->decode((Char *)end() + insertRS_,
					 bytesStart,
					 nLeftOver_ + nread
					 - (zapEof_ && bytesEnd[-1] == EOFCHAR),
					 &leftOver_);
	nLeftOver_ = bytesEnd - leftOver_;
	if (nChars > 0) {
	  if (insertRS_) {
	    noteRS();
	    *(Char *)end() = RS;
	    advanceEnd(end() + 1);
	    insertRS_ = false;
	    bufLim_ += 1;
	    bufLimOffset_ += 1;
	  }
	  bufLim_ += nChars;
	  bufLimOffset_ += nChars;
	  break;
	}
      }
    }
    else
      so_ = 0;
  }
  ASSERT(end() < bufLim_);
  if (insertRS_) {
    noteRS();
    insertChar(RS);
    insertRS_ = false;
    bufLimOffset_ += 1;
  }
  switch (recordType_) {
  case unknown:
    {
      const Char *e = findNextCrOrLf(end(), bufLim_);
      if (e) {
	if (*e == '\n') {
	  recordType_ = lf;
	  info_->noteInsertedRSs();
	  *(Char *)e = RE;
	  advanceEnd(e + 1);
	  insertRS_ = true;
	}
	else {
	  if (e + 1 < bufLim_) {
	    if (e[1] == '\n') {
	      recordType_ = crlf;
	      advanceEnd(e + 1);
	      if (e + 2 == bufLim_) {
		bufLim_--;
		bufLimOffset_--;
		insertRS_ = true;
	      }
	    }
	    else {
	      advanceEnd(e + 1);
	      recordType_ = cr;
	      info_->noteInsertedRSs();
	      insertRS_ = true;
	    }
	  }
	  else {
	    recordType_ = crUnknown;
	    advanceEnd(e + 1);
	  }
	}
      }
      else
	advanceEnd(bufLim_);
    }
    break;
  case crUnknown:
    {
      if (*cur() == '\n') {
	noteRS();
	advanceEnd(cur() + 1);
	recordType_ = crlf;
      }
      else {
	advanceEnd(cur() + 1);
	insertRS_ = true;
	recordType_ = cr;
	info_->noteInsertedRSs();
      }
    }
    break;
  case lf:
    {
      Char *e = (Char *)findNextLf(end(), bufLim_);
      if (e) {
	advanceEnd(e + 1);
	*e = RE;
	insertRS_ = true;
      }
      else
	advanceEnd(bufLim_);
    }
    break;
  case cr:
    {
      const Char *e = findNextCr(end(), bufLim_);
      if (e) {
	advanceEnd(e + 1);
	insertRS_ = true;
      }
      else
	advanceEnd(bufLim_);
    }
    break;
  case crlf:
    {
      const Char *e = end();
      for (;;) {
	e = findNextLf(e, bufLim_);
	if (!e) {
	  advanceEnd(bufLim_);
	  break;
	}
	// Need to delete final RS if not followed by anything.
	if (e + 1 == bufLim_) {
	  bufLim_--;
	  bufLimOffset_--;
	  advanceEnd(e);
	  insertRS_ = true;
	  break;
	}
	noteRSAt(e);
	e++;
      }
    }
    break;
  case asis:
    advanceEnd(bufLim_);
    break;
  default:
    CANNOT_HAPPEN();
  }

  return nextChar();
}

const Char *ExternalInputSource::findNextCr(const Char *start,
					    const Char *end)
{
  for (; start < end; start++)
    if (*start == '\r')
      return start;
  return 0;
}

const Char *ExternalInputSource::findNextLf(const Char *start,
					    const Char *end)
{
  for (; start < end; start++)
    if (*start == '\n')
      return start;
  return 0;
}

const Char *ExternalInputSource::findNextCrOrLf(const Char *start,
						const Char *end)
{
  for (; start < end; start++)
    if (*start == '\n' || *start == '\r')
      return start;
  return 0;
}

void ExternalInputSource::pushCharRef(Char ch, const NamedCharRef &ref)
{
  ASSERT(cur() == start());
  noteCharRef(startIndex() + (cur() - start()), ref);
  insertChar(ch);
}

void ExternalInputSource::insertChar(Char ch)
{
  if (start() > buf_) {
    if (cur() > start())
      memmove((Char *)start() - 1, start(), (cur() - start())*sizeof(Char));
    moveLeft();
    *(Char *)cur() = ch;
  }
  else {
    // must have start == buf
    if (buf_ + (bufSize_ - (nLeftOver_ + sizeof(Char) - 1)/sizeof(Char))
	== bufLim_) {
      if (bufSize_ == size_t(-1))
	abort();		// FIXME throw an exception
      reallocateBuffer(bufSize_ + 1);
    }
    else if (nLeftOver_ > 0 && ((char *)(bufLim_ + 1) > leftOver_)) {
      char *s = (char *)(buf_ + bufSize_) - nLeftOver_;
      memmove(s, leftOver_, nLeftOver_);
      leftOver_ = s;
    }
    if (cur() < bufLim_)
      memmove((Char *)cur() + 1, cur(), (bufLim_ - cur())*sizeof(Char));
    *(Char *)cur() = ch;
    advanceEnd(end() + 1);
    bufLim_ += 1;
  }
}

void ExternalInputSource::reallocateBuffer(size_t newSize)
{
  Char *newBuf = new Char[newSize];
  
  memcpy(newBuf, buf_, bufSize_*sizeof(Char));
  bufSize_ = newSize;
  changeBuffer(newBuf, buf_);
  bufLim_ = newBuf + (bufLim_ - buf_);
  if (nLeftOver_ > 0) {
    char *s = (char *)(newBuf + bufSize_) - nLeftOver_;
    memmove(s,
	    newBuf + (leftOver_ - (char *)buf_),
	    nLeftOver_);
    leftOver_ = s;
  }
  delete [] buf_;
  buf_ = newBuf;
}

RTTI_DEF1(ExternalInfoImpl, ExternalInfo)

ExternalInfoImpl::ExternalInfoImpl(ParsedSystemId &parsedSysid)
: currentIndex_(0), position_(parsedSysid.size())
{
  parsedSysid.swap(parsedSysid_);
  if (parsedSysid_.size() > 0) {
    notrack_ = parsedSysid_[0].notrack;
  } else {
    notrack_ = false;
  }
}

StringC &ExternalInfoImpl::id(size_t i)
{
  return parsedSysid_[i].id;
}

void ExternalInfoImpl::setDecoder(size_t i, Decoder *decoder)
{
  position_[i].decoder = decoder;
}

void ExternalInfoImpl::noteInsertedRSs()
{
  position_[currentIndex_].insertedRSs = 1;
}

void ExternalInfoImpl::noteRS(Offset offset)
{
  if (!notrack_)
    rsList_.append(offset);
  if (offset
      == (currentIndex_ == 0 ? 0 : position_[currentIndex_- 1].endOffset))
    position_[currentIndex_].startsWithRS = 1;
}

void ExternalInfoImpl::noteStorageObjectEnd(Offset offset)
{
  ASSERT(currentIndex_ < position_.size());
  // The last endOffset_ must be -1.
  if (currentIndex_ < position_.size() - 1) {
    position_[currentIndex_++].endOffset = offset;
    position_[currentIndex_].line1RS = rsList_.size();
    notrack_ = parsedSysid_[currentIndex_].notrack;
  }
}

Boolean ExternalInfoImpl::convertOffset(Offset off,
					StorageObjectLocation &ret) const
{
  ret.storageObjectSpec = 0;
  if (off == Offset(-1) || position_.size() == 0)
    return false;
  // the last endOffset_ is Offset(-1), so this will
  // terminate
  int i;
  for (i = 0; off >= position_[i].endOffset; i++)
    ;
  for (; parsedSysid_[i].id.size() == 0; i--)
    if (i == 0)
      return false;
  ret.storageObjectSpec = &parsedSysid_[i];
  Offset startOffset = i == 0 ? 0 : position_[i - 1].endOffset;
  ret.storageObjectOffset = off - startOffset;
  ret.byteIndex = ret.storageObjectOffset;
  if (parsedSysid_[i].notrack
      || parsedSysid_[i].records == StorageObjectSpec::asis) {
    ret.lineNumber = (unsigned long)-1;
    if (parsedSysid_[i].records != StorageObjectSpec::asis) {
      if (position_[i].insertedRSs)
	ret.byteIndex = (unsigned long)-1;
      else if (ret.byteIndex > 0 && position_[i].startsWithRS)
	ret.byteIndex--;	// first RS is inserted
    }
    ret.columnNumber = (unsigned long)-1;
    return true;
  }
  else {
    size_t line1RS = position_[i].line1RS;
    // line1RS is now the number of RSs that are before or on the current line.
    size_t j;
    Offset colStart;
    if (rsList_.findPreceding(off, j, colStart)) {
      if (position_[i].insertedRSs)
	ret.byteIndex -= j + 1 - line1RS;
      else if (ret.byteIndex > 0 && position_[i].startsWithRS)
	ret.byteIndex--;	// first RS is inserted
      j++;
      colStart++;
    }
    else {
      j = 0;
      colStart = 0;
    }
    // j is now the number of RSs that are before or on the current line
    // colStart is the offset of the first column
    ret.lineNumber = j - line1RS + 1 - position_[i].startsWithRS;
    // the offset of the first column
    if (colStart < startOffset)
      colStart = startOffset;
    // the RS that starts a line will be in column 0;
    // the first real character of a line will be column 1
    ret.columnNumber = 1 + off - colStart;
  }
  if (!position_[i].decoder
      || !position_[i].decoder->convertOffset(ret.byteIndex))
    ret.byteIndex = (unsigned long)-1;
  return true;
}

const StorageObjectSpec &ExternalInfoImpl::spec(size_t i) const
{
  return parsedSysid_[i];
}

size_t ExternalInfoImpl::nSpecs() const
{
  return parsedSysid_.size();
}

const ParsedSystemId &ExternalInfoImpl::parsedSystemId() const
{
  return parsedSysid_;
}

StorageObjectSpec::StorageObjectSpec()
: storageManager(0), codingSystem(0), codingSystemName(0), notrack(0),
  records(find), zapEof(1), search(1)
{
}

StorageObjectPosition::StorageObjectPosition()
: endOffset(Offset(-1)), line1RS(0), startsWithRS(0), insertedRSs(0)
{
}

FSIParser::FSIParser(const StringC &str,
		     const CharsetInfo &idCharset,
		     Boolean isNdata,
		     const StorageObjectSpec *defSpec,
		     const EntityManagerImpl *em,
		     Messenger &mgr)
: str_(str),
  strIndex_(0),
  idCharset_(idCharset),
  isNdata_(isNdata),
  defSpec_(defSpec),
  em_(em),
  mgr_(mgr)
{
}

Xchar FSIParser::get()
{
  if (strIndex_ < str_.size())
    return str_[strIndex_++];
  else
    return -1;
}

void FSIParser::unget()
{
  if (strIndex_ > 0)
    strIndex_ -= 1;
}

Boolean FSIParser::matchKey(const StringC &str, const char *s)
{
  if (strlen(s) != str.size())
    return false;
  for (size_t i = 0; i < str.size(); i++)
    if (idCharset_.execToDesc(toupper(s[i])) != str[i]
	&& idCharset_.execToDesc(tolower(s[i])) != str[i])
      return false;
  return true;
}

Boolean FSIParser::matchChar(Xchar ch, char execC)
{
  return ch == idCharset_.execToDesc(execC);
}

Boolean FSIParser::isS(Xchar c)
{
  return (matchChar(c, ' ')
	  || matchChar(c, '\r')
	  || matchChar(c, '\n')
	  || matchChar(c, ' '));
}

Boolean FSIParser::convertDigit(Xchar c, int &weight)
{
  static const char digits[] = "0123456789";
  for (int i = 0; digits[i] != '\0'; i++)
    if (matchChar(c, digits[i])) {
      weight = i;
      return 1;
    }
  return 0;
}

Boolean FSIParser::parse(ParsedSystemId &parsedSysid)
{
  size_t startIndex = strIndex_;
  if (!matchChar(get(), '<'))
    return handleInformal(startIndex, parsedSysid);
  StringC key;
  for (;;) {
    Xchar c = get();
    if (c == -1)
      return handleInformal(startIndex, parsedSysid);
    if (isS(c) || matchChar(c, '>'))
      break;
    key += Char(c);
  }
  unget();
  if (matchKey(key, "CATALOG")) {
    if (!setCatalogAttributes(parsedSysid))
      return 0;
    return parse(parsedSysid);
  }
  Boolean neutral;
  StorageManager *sm = lookupStorageType(key, neutral);
  if (!sm)
    return handleInformal(startIndex, parsedSysid);
  for (;;) {
    parsedSysid.resize(parsedSysid.size() + 1);
    StorageObjectSpec &sos = parsedSysid.back();
    sos.storageManager = sm;
    Xchar smcrd;
    Boolean fold;
    if (!setAttributes(sos, neutral, smcrd, fold))
      return 0;
    sm = 0;
    StringC id;
    Boolean hadData = 0;
    for (;;) {
      Xchar c = get();
      if (c == -1)
	break;
      if (matchChar(c, '<')) {
	hadData = 1;
	Char stago = c;
	key.resize(0);
	for (;;) {
	  c = get();
	  if (c == -1) {
	    id += stago;
	    id += key;
	    break;
	  }
	  if (isS(c) || matchChar(c, '>')) {
	    unget();
	    sm = lookupStorageType(key, neutral);
	    if (!sm) {
	      id += stago;
	      id += key;
	    }
	    break;
	  }
	  key += c;
	}
	if (sm)
	  break;
      }
      else if (!((!hadData && matchChar(c, '\r')) // ignored RE
		 || matchChar(c, '\n') )) {	  // ignored RS
	hadData = 1;
	id += c;
      }
    }
    if (id.size() > 0 && matchChar(id[id.size() - 1], '\r'))
      id.resize(id.size() - 1);
    uncharref(id);
    id.swap(sos.specId);
    if (!convertId(sos.specId, smcrd, sos.storageManager))
      return 0;
    if (neutral) {
      if (!sos.storageManager->transformNeutral(sos.specId, fold, mgr_))
	return 0;
    }
    if (sos.storageManager->resolveRelative(sos.baseId, sos.specId,
					    sos.search))
      sos.baseId.resize(0);
    if (!sm)
      break;
  }
  return 1;
}

Boolean FSIParser::handleInformal(size_t index, ParsedSystemId &parsedSysid)
{
  parsedSysid.resize(parsedSysid.size() + 1);
  StorageObjectSpec &sos = parsedSysid.back();
  sos.specId.assign(str_.data() + index,
		    str_.size() - index);
  sos.storageManager = em_->guessStorageType(sos.specId, idCharset_);
  if (!sos.storageManager) {
    if (defSpec_ && defSpec_->storageManager->inheritable())
      sos.storageManager = defSpec_->storageManager;
    else
      sos.storageManager = em_->defaultStorageManager_.pointer();
  }
  setDefaults(sos);
  if (!convertId(sos.specId, -1, sos.storageManager))
    return 0;
  if (sos.storageManager->resolveRelative(sos.baseId, sos.specId, sos.search))
    sos.baseId.resize(0);
  return 1;
}

StorageManager *FSIParser::lookupStorageType(const StringC &key,
					     Boolean &neutral)
{
  if (matchKey(key, "NEUTRAL")) {
    neutral = 1;
    if (defSpec_ && defSpec_->storageManager->inheritable())
      return defSpec_->storageManager;
    else
      return em_->defaultStorageManager_.pointer();
  }
  else {
    StorageManager *sm = em_->lookupStorageType(key, idCharset_);
    if (sm)
      neutral = 0;
    return sm;
  }
}

Boolean FSIParser::setCatalogAttributes(ParsedSystemId &parsedSysid)
{
  Boolean hadPublic = 0;
  parsedSysid.maps.resize(parsedSysid.maps.size() + 1);
  parsedSysid.maps.back().type = ParsedSystemIdMap::catalogDocument;
  for (;;) {
    StringC token, value;
    Boolean gotValue;
    if (!parseAttribute(token, gotValue, value)) {
      mgr_.message(EntityManagerMessages::fsiSyntax, StringMessageArg(str_));
      return 0;
    }
    if (token.size() == 0)
      break;
    if (matchKey(token, "PUBLIC")) {
      if (hadPublic)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("PUBLIC")));
      else if (gotValue) {
	convertMinimumLiteral(value, parsedSysid.maps.back().publicId);
	parsedSysid.maps.back().type = ParsedSystemIdMap::catalogPublic;
      }
      else
	mgr_.message(EntityManagerMessages::fsiMissingValue,
		     StringMessageArg(token));
      hadPublic = 1;
    }
    else
      mgr_.message(gotValue
		   ? EntityManagerMessages::fsiUnsupportedAttribute
		   : EntityManagerMessages::fsiUnsupportedAttributeToken,
		   StringMessageArg(token));
  }
  return 1;
}

void FSIParser::convertMinimumLiteral(const StringC &from, StringC &to)
{
  // Do just enough to ensure it can be reparsed.
  to.resize(0);
  for (size_t i = 0; i < from.size(); i++) {
    Char c = from[i];
    if (matchChar(c, '"') || matchChar(c, '#'))
      mgr_.message(EntityManagerMessages::fsiLookupChar, NumberMessageArg(c));
    else if (matchChar(c, ' ')) {
      if (to.size() && to[to.size() - 1] != c)
	to += c;
    }
    else
      to += c;
  }
  if (to.size() && matchChar(to[to.size() - 1], ' '))
    to.resize(to.size() - 1);
}

// FIXME This should be table driven.

Boolean FSIParser::setAttributes(StorageObjectSpec &sos,
				 Boolean neutral,
				 Xchar &smcrd,
				 Boolean &fold)
{
  Boolean hadBctf = 0;
  Boolean hadTracking = 0;
  Boolean hadSmcrd = 0;
  smcrd = -1;
  fold = 1;
  Boolean hadRecords = 0;
  Boolean hadBase = 0;
  Boolean hadZapeof = 0;
  Boolean hadSearch = 0;
  Boolean hadFold = 0;
  StorageObjectSpec::Records records;
  setDefaults(sos);
  for (;;) {
    StringC token, value;
    Boolean gotValue;
    if (!parseAttribute(token, gotValue, value)) {
      mgr_.message(EntityManagerMessages::fsiSyntax, StringMessageArg(str_));
      return 0;
    }
    if (token.size() == 0)
      break;
    if (matchKey(token, "BCTF")) {
      if (sos.storageManager->requiredCodingSystem())
	mgr_.message(EntityManagerMessages::fsiBctfNotApplicable);
      else if (hadBctf)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	const char *codingSystemName;
	const InputCodingSystem *codingSystem
	  = em_->lookupCodingSystem(value, idCharset_, codingSystemName);
	if (codingSystem) {
	  sos.codingSystem = codingSystem;
	  sos.codingSystemName = codingSystemName;
	}
	else if (matchKey(value, "SAME")) {
	  if (isNdata_) {
	    if (defSpec_) {
	      sos.codingSystem = defSpec_->codingSystem;
	      sos.codingSystemName = defSpec_->codingSystemName;
	    }
	    else {
	      sos.codingSystem = em_->defaultCodingSystem_;
	      sos.codingSystemName = 0;
	    }
	  }
	}
	else
	  mgr_.message(EntityManagerMessages::fsiUnknownBctf,
		       StringMessageArg(value));
      }
      else
	mgr_.message(EntityManagerMessages::fsiMissingValue,
		     StringMessageArg(token));
      hadBctf = 1;
    }
    else if (matchKey(token, "TRACKING")) {
      if (hadTracking)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	if (matchKey(value, "NOTRACK"))
	  sos.notrack = 1;
	else if (!matchKey(value, "TRACK"))
	  mgr_.message(EntityManagerMessages::fsiBadTracking,
		       StringMessageArg(value));
      }
      else
	mgr_.message(EntityManagerMessages::fsiMissingValue,
		     StringMessageArg(token));
      hadTracking = 1;
    }
    else if (matchKey(token, "ZAPEOF")) {
      if (sos.storageManager->requiredCodingSystem())
	mgr_.message(EntityManagerMessages::fsiZapeofNotApplicable);
      else if (hadZapeof)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	if (matchKey(value, "ZAPEOF"))
	  sos.zapEof = 1;
	else if (matchKey(value, "NOZAPEOF"))
	  sos.zapEof = 0;
	else
	  mgr_.message(EntityManagerMessages::fsiBadZapeof,
		       StringMessageArg(value));
      }
      else
	sos.zapEof = 1;
      hadZapeof = 1;
    }
    else if (matchKey(token, "NOZAPEOF")) {
      if (sos.storageManager->requiredCodingSystem())
	mgr_.message(EntityManagerMessages::fsiZapeofNotApplicable);
      else if (hadZapeof)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("ZAPEOF")));
      else if (gotValue)
	mgr_.message(EntityManagerMessages::fsiValueAsName,
		     StringMessageArg(token));
      else
	sos.zapEof = 0;
      hadZapeof = 1;
    }
    else if (matchKey(token, "SEARCH")) {
      if (hadSearch)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	if (matchKey(value, "SEARCH"))
	  sos.search = 1;
	else if (matchKey(value, "NOSEARCH"))
	  sos.search = 0;
	else
	  mgr_.message(EntityManagerMessages::fsiBadSearch,
		       StringMessageArg(value));
      }
      else
	sos.search = 1;
      hadSearch = 1;
    }
    else if (matchKey(token, "NOSEARCH")) {
      if (hadSearch)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("SEARCH")));
      else if (gotValue)
	mgr_.message(EntityManagerMessages::fsiValueAsName,
		     StringMessageArg(token));
      else
	sos.search = 0;
      hadSearch = 1;
    }
    else if (matchKey(token, "FOLD")) {
      if (!neutral)
	mgr_.message(EntityManagerMessages::fsiFoldNotNeutral);
      else if (hadFold)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	if (matchKey(value, "FOLD"))
	  fold = 1;
	else if (matchKey(value, "NOFOLD"))
	  fold = 0;
	else
	  mgr_.message(EntityManagerMessages::fsiBadFold,
		       StringMessageArg(value));
      }
      else
	fold = 1;
      hadFold = 1;
    }
    else if (matchKey(token, "NOFOLD")) {
      if (!neutral)
	mgr_.message(EntityManagerMessages::fsiFoldNotNeutral);
      else if (hadFold)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("FOLD")));
      else if (gotValue)
	mgr_.message(EntityManagerMessages::fsiValueAsName,
		     StringMessageArg(token));
      else
	fold = 0;
      hadFold = 1;
    }
    else if (matchKey(token, "SMCRD")) {
      if (hadSmcrd)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	if (value.size() == 0)
	  smcrd = -1;
	else if (value.size() == 1)
	  smcrd = value[0];
	else
	  mgr_.message(EntityManagerMessages::fsiBadSmcrd,
		       StringMessageArg(value));
      }
      else
	mgr_.message(EntityManagerMessages::fsiMissingValue,
		     StringMessageArg(token));
      hadSmcrd = 1;
    }
    else if (matchKey(token, "RECORDS")) {
      if (sos.storageManager->requiresCr())
	mgr_.message(EntityManagerMessages::fsiRecordsNotApplicable);
      else if (hadRecords)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue) {
	if (!lookupRecords(value, sos.records))
	  mgr_.message(EntityManagerMessages::fsiUnsupportedRecords,
		       StringMessageArg(value));
      }
      else
	mgr_.message(EntityManagerMessages::fsiMissingValue,
		     StringMessageArg(token));
      hadRecords = 1;
    }
    else if (matchKey(token, "SOIBASE")) {
      if (hadBase)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(token));
      else if (gotValue)
	value.swap(sos.baseId);
      else {
	mgr_.message(EntityManagerMessages::fsiMissingValue,
		     StringMessageArg(token));
	sos.baseId.resize(0);
      }
      hadBase = 1;
    }
    else if (lookupRecords(token, records)) {
      if (sos.storageManager->requiresCr())
	mgr_.message(EntityManagerMessages::fsiRecordsNotApplicable);
      else if (hadRecords)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("RECORDS")));
      else if (!gotValue)
	sos.records = records;
      else
	mgr_.message(EntityManagerMessages::fsiValueAsName,
		     StringMessageArg(token));
      hadRecords = 1;
    }
    else if (matchKey(token, "NOTRACK")) {
      if (hadTracking)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("TRACKING")));
      else if (!gotValue)
	sos.notrack = 1;
      else
	mgr_.message(EntityManagerMessages::fsiValueAsName,
		     StringMessageArg(token));
      hadTracking = 1;
    }
    else if (matchKey(token, "TRACK")) {
      if (hadTracking)
	mgr_.message(EntityManagerMessages::fsiDuplicateAttribute,
		     StringMessageArg(idCharset_.execToDesc("TRACKING")));
      else if (gotValue)
	mgr_.message(EntityManagerMessages::fsiValueAsName,
		     StringMessageArg(token));
      hadTracking = 1;
    }
    else
      mgr_.message(gotValue
		   ? EntityManagerMessages::fsiUnsupportedAttribute
		   : EntityManagerMessages::fsiUnsupportedAttributeToken,
		   StringMessageArg(token));
  }
  if (hadBase && sos.baseId.size() > 0) {
    convertId(sos.baseId, smcrd, sos.storageManager);
    if (neutral) {
      if (!sos.storageManager->transformNeutral(sos.baseId, fold, mgr_))
	sos.baseId.resize(0);
    }
  }
  if (!hadZapeof && hadRecords && sos.records == StorageObjectSpec::asis)
    sos.zapEof = 0;
  return 1;
}

FSIParser::RecordType FSIParser::recordTypeTable[] = {
  { "FIND", StorageObjectSpec::find },
  { "ASIS", StorageObjectSpec::asis },
  { "CR", StorageObjectSpec::cr },
  { "LF", StorageObjectSpec::lf },
  { "CRLF", StorageObjectSpec::crlf }
};

const char *FSIParser::recordsName(StorageObjectSpec::Records records)
{
  for (size_t i = 0; i < SIZEOF(recordTypeTable); i++)
    if (records == recordTypeTable[i].value)
      return recordTypeTable[i].name;
  return 0;
}

Boolean FSIParser::lookupRecords(const StringC &token,
				 StorageObjectSpec::Records &result)
{
  for (size_t i = 0; i < SIZEOF(recordTypeTable); i++)
    if (matchKey(token, recordTypeTable[i].name)) {
      result = recordTypeTable[i].value;
      return 1;
    }
  return 0;
}

void FSIParser::setDefaults(StorageObjectSpec &sos)
{
  if (sos.storageManager->requiresCr())
    sos.records = StorageObjectSpec::cr;
  else if (isNdata_
	   || (defSpec_ && defSpec_->records == StorageObjectSpec::asis))
    sos.records = StorageObjectSpec::asis;
  if (isNdata_ || (defSpec_ && !defSpec_->zapEof))
    sos.zapEof = 0;
  if (defSpec_ && defSpec_->storageManager == sos.storageManager) {
    if (defSpec_->id.size())
      sos.baseId = defSpec_->id;
    else {
      sos.baseId = defSpec_->specId;
      sos.storageManager->resolveRelative(defSpec_->baseId,
					  sos.baseId,
					  0);
    }
  }
  sos.codingSystem = sos.storageManager->requiredCodingSystem();
  if (sos.codingSystem)
    sos.zapEof = 0;		// hack
  else {
    sos.codingSystem = em_->defaultCodingSystem_;
    if (isNdata_) {
      const InputCodingSystem *id = 0;
      size_t j;
      for (j = 0; j < em_->codingSystems_.size(); j++)
	if (em_->codingSystems_[j].ics->isIdentity()) {
	  id = em_->codingSystems_[j].ics;
	  break;
	}
      if (id && id != em_->defaultCodingSystem_) {
	sos.codingSystem = id;
	sos.codingSystemName = em_->codingSystems_[j].name;
      }
    }
    else if (defSpec_) {
      sos.codingSystem = defSpec_->codingSystem;
      sos.codingSystemName = defSpec_->codingSystemName;
    }
  }
}

Boolean FSIParser::parseAttribute(StringC &token, Boolean &gotValue,
				  StringC &value)
{
  Xchar c = get();
  while (isS(c))
    c = get();
  if (c == -1) {
    return 0;
  }
  token.resize(0);
  if (matchChar(c, '>'))
    return 1;
  if (matchChar(c, '"') || matchChar(c, '\'') || matchChar(c, '='))
    return 0;
  for (;;) {
    token += c;
    c = get();
    if (c == -1)
      return 0;
    if (isS(c))
      break;
    if (matchChar(c, '>') || matchChar(c, '='))
      break;
  }
  while (isS(c))
    c = get();
  if (c == -1)
    return 0;
  if (!matchChar(c, '=')) {
    unget();
    gotValue = 0;
    return 1;
  }
  gotValue = 1;
  value.resize(0);

  c = get();
  while (isS(c))
    c = get();
  if (matchChar(c, '>') || matchChar(c, '='))
    return 0;
  if (matchChar(c, '"') || matchChar(c, '\'')) {
    Char lit = c;
    for (;;) {
      Xchar c = get();
      if (c == lit)
	break;
      if (c == -1)
	return 0;
      if (matchChar(c, '\n'))
	;
      else if (matchChar(c, '\r') || matchChar(c, '\t'))
	value += idCharset_.execToDesc(' ');
      else
	value += c;
    }
    uncharref(value);
  }
  else {
    for (;;) {
      value += c;
      c = get();
      if (c == -1)
	return 0;
      if (isS(c))
	break;
      if (matchChar(c, '>') || matchChar(c, '=')) {
	unget();
	break;
      }
    }
  }
  return 1;
}

void FSIParser::uncharref(StringC &str)
{
  size_t j = 0;
  size_t i = 0;
  while (i < str.size()) {
    int digit;
    if (matchChar(str[i], '&')
	&& i + 2 < str.size()
	&& matchChar(str[i + 1], '#')
	&& convertDigit(str[i + 2], digit)) {
      unsigned long val = digit;
      i += 3;
      while (i < str.size() && convertDigit(str[i], digit)) {
	val = val*10 + digit;
	i++;
      }
      str[j++] = val;
      if (i < str.size() && matchChar(str[i], ';'))
	i++;
    }
    else
      str[j++] = str[i++];
  }
  str.resize(j);
}

Boolean FSIParser::convertId(StringC &id, Xchar smcrd,
			     const StorageManager *sm)
{
  const CharsetInfo *smCharset = sm->idCharset();
  StringC newId;
  size_t i = 0;
  while (i < id.size()) {
    UnivChar univ;
    WideChar wide;
    ISet<WideChar> wideSet;
    int digit;
    if (Xchar(id[i]) == smcrd
	&& i + 1 < id.size()
	&& convertDigit(id[i + 1], digit)) {
      i += 2;
      Char val = digit;
      while (i < id.size() && convertDigit(id[i], digit)) {
	val = val*10 + digit;
	i++;
      }
      newId += val;
      if (i < id.size() && matchChar(id[i], ';'))
	i++;
    }
    else if (smCharset) {
      if (!idCharset_.descToUniv(id[i++], univ))
	return 0;
      if (univ == UnivCharsetDesc::rs)
	;
      else if (univ == UnivCharsetDesc::re && sm->reString())
	newId += *sm->reString();
      else if (smCharset->univToDesc(univ, wide, wideSet) != 1
	       || wide > charMax)
	return 0;			// FIXME give error
      else
	newId += Char(wide);
    }
    else
      newId += id[i++];
  }
  newId.swap(id);
  return 1;
}

ParsedSystemId:: ParsedSystemId()
{
}

static
void unparseSoi(const StringC &soi,
		const CharsetInfo *idCharset,
		const CharsetInfo &resultCharset,
		StringC &result,
		Boolean &needSmcrd);

void ParsedSystemId::unparse(const CharsetInfo &resultCharset,
			     StringC &result) const
{
  size_t len = size();
  result.resize(0);
  size_t i;
  for (i = 0; i < maps.size(); i++) {
    if (maps[i].type == ParsedSystemIdMap::catalogDocument)
      result += resultCharset.execToDesc("<CATALOG>");
    else if (maps[i].type == ParsedSystemIdMap::catalogPublic) {
      result += resultCharset.execToDesc("<CATALOG PUBLIC=\"");
      result += maps[i].publicId;
      result += resultCharset.execToDesc("\">");
    }
  }
  for (i = 0; i < len; i++) {
    const StorageObjectSpec &sos = (*this)[i];
    result += resultCharset.execToDesc('<');
    result += resultCharset.execToDesc(sos.storageManager->type());
    if (sos.notrack)
      result += resultCharset.execToDesc(" NOTRACK");
    if (!sos.search)
      result += resultCharset.execToDesc(" NOSEARCH");
    if (!sos.storageManager->requiresCr()
	&& sos.records != StorageObjectSpec::find) {
      result += resultCharset.execToDesc(' ');
      result += resultCharset.execToDesc(FSIParser::recordsName(sos.records));
    }
    if (sos.codingSystemName) {
      if (!sos.zapEof)
	result += resultCharset.execToDesc(" NOZAPEOF");
      result += resultCharset.execToDesc(" BCTF=");
      result += resultCharset.execToDesc(sos.codingSystemName);
    }
    Boolean needSmcrd = 0;
    if (sos.baseId.size() != 0) {
      result += resultCharset.execToDesc(" SOIBASE='");
      unparseSoi(sos.baseId,
		 sos.storageManager->idCharset(),
		 resultCharset,
		 result,
		 needSmcrd);
      result += resultCharset.execToDesc('\'');
    }
    StringC tem;
    unparseSoi(sos.specId,
	       sos.storageManager->idCharset(),
	       resultCharset,
	       tem,
	       needSmcrd);
    if (needSmcrd)
      result += resultCharset.execToDesc(" SMCRD='^'");
    result += resultCharset.execToDesc('>');
    result += tem;
  }
}

void unparseSoi(const StringC &soi,
		const CharsetInfo *idCharset,
		const CharsetInfo &resultCharset,
		StringC &result,
		Boolean &needSmcrd)
{
  if (!idCharset) {
    for (size_t i = 0; i < soi.size(); i++) {
      char buf[32];
      sprintf(buf, "&#%lu;", (unsigned long)soi[i]);
      result += resultCharset.execToDesc(buf);
    }
    return;
  }
  for (size_t i = 0; i < soi.size(); i++) {
    UnivChar univ;
    WideChar to;
    ISet<WideChar> toSet;
    if (!idCharset->descToUniv(soi[i], univ)
	|| univ >= 127
	|| univ < 32
	|| univ == 36		// $
	|| univ == 96		// `
#ifndef MSDOS_FILENAMES
	|| univ == 92		// backslash
#endif
	|| univ == 94		// ^
	|| resultCharset.univToDesc(univ, to, toSet) != 1) {
      needSmcrd = 1;
      char buf[32];
      sprintf(buf, "^%lu;", (unsigned long)soi[i]);
      result += resultCharset.execToDesc(buf);
    }
    else {
      switch (univ) {
      case 34:		// double quote
      case 35:		// #
      case 39:		// apostrophe
      case 60:		// <
	{
	  char buf[32];
	  sprintf(buf, "&#%lu;", (unsigned long)to);
	  result += resultCharset.execToDesc(buf);
	}
	break;
      default:
	result += Char(to);
	break;
      }
    }
  }
}

#ifdef SP_NAMESPACE
}
#endif
