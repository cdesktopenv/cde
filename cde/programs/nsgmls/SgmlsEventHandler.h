/* $XConsortium: SgmlsEventHandler.h /main/1 1996/07/29 17:04:25 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef SgmlsEventHandler_INCLUDED
#define SgmlsEventHandler_INCLUDED 1

#include <stddef.h>
#include "Event.h"
#include "Boolean.h"
#include "StringC.h"
#include "OutputCharStream.h"
#include "StringSet.h"
#include "Location.h"
#include "Syntax.h"
#include "Sd.h"
#include "Ptr.h"
#include "LinkProcess.h"
#include "Message.h"
#include "ErrorCountEventHandler.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SgmlParser;
struct StorageObjectSpec;

class SgmlsSubdocState {
public:
  SgmlsSubdocState();
  SgmlsSubdocState(const SgmlParser *);
  void init(const SgmlParser *);
  void swap(SgmlsSubdocState &);
protected:
  const SgmlParser *parser_;
  StringSet definedEntities_;
  StringSet definedNotations_;
  Boolean haveLinkProcess_;
  LinkProcess linkProcess_;
private:
  SgmlsSubdocState(const SgmlsSubdocState &); // undefined
  void operator=(const SgmlsSubdocState &);   // undefined
};

class SgmlsEventHandler : public ErrorCountEventHandler,
  private SgmlsSubdocState, private Messenger {
public:
  enum {
    outputLine = 01,
    outputEntity = 02,
    outputId = 04,
    outputIncluded = 010,
    outputNotationSysid = 020
    };
      
  SgmlsEventHandler(const SgmlParser *,
		    OutputCharStream *,
		    Messenger *,
		    unsigned outputFlags);
  ~SgmlsEventHandler();
  void data(DataEvent *);
  void startElement(StartElementEvent *);
  void endElement(EndElementEvent *);
  void pi(PiEvent *);
  void sdataEntity(SdataEntityEvent *);
  void externalDataEntity(ExternalDataEntityEvent *);
  void subdocEntity(SubdocEntityEvent *);
  void appinfo(AppinfoEvent *);
  void uselink(UselinkEvent *);
  void sgmlDecl(SgmlDeclEvent *);
  void endProlog(EndPrologEvent *);
  void message(MessageEvent *);
  void entityDefaulted(EntityDefaultedEvent *event);
protected:
  void dispatchMessage(const Message &);
  void dispatchMessage(Message &);
  void initMessage(Message &);
private:
  SgmlsEventHandler(const SgmlsEventHandler &); // undefined
  void operator=(const SgmlsEventHandler &);	// undefined

  void attributes(const AttributeList &attributes, char code,
		  const StringC *ownerName);
  void startAttribute(const StringC &name, char code,
		      const StringC *ownerName);
  void defineEntity(const Entity *entity);
  void defineExternalDataEntity(const ExternalDataEntity *entity);
  void defineSubdocEntity(const SubdocEntity *entity);
  void defineExternalTextEntity(const ExternalEntity *entity);
  void defineInternalEntity(const InternalEntity *entity);
  void defineNotation(const Notation *notation);
  void externalId(const ExternalId &id, Boolean outputFile = 1);
  Boolean markEntity(const Entity *);
  Boolean markNotation(const Notation *);
  void startData();
  void flushData();

  void outputLocation(const Location &loc);
  void outputLocation1(const Location &loc);
  void outputString(const StringC &str);
  void outputString(const Char *, size_t);
  static void escape(OutputCharStream &, Char);

  OutputCharStream &os();

  Messenger *messenger_;
  Location currentLocation_;
  OutputCharStream *os_;
  Boolean haveData_;
  ConstPtr<Sd> sd_;
  ConstPtr<Syntax> syntax_;
  const StorageObjectSpec *lastSos_;
  unsigned long lastLineno_;
  Location lastLoc_;
  PackedBoolean outputLine_;
  PackedBoolean outputIncluded_;
  PackedBoolean outputEntity_;
  PackedBoolean outputId_;
  PackedBoolean outputNotationSysid_;
};

inline
void SgmlsEventHandler::outputString(const StringC &str)
{
  outputString(str.data(), str.size());
}

inline
OutputCharStream &SgmlsEventHandler::os()
{
  return *os_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SgmlsEventHandler_INCLUDED */
