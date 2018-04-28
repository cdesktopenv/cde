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
/* $XConsortium: GenericEventHandler.h /main/1 1996/07/29 16:52:21 cde-hp $ */
// Copyright (c) 1995, 1996 James Clark
// See the file COPYING for copying permission.

#ifndef GenericEventHandler_INCLUDED
#define GenericEventHandler_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

// Must include Boolean.h before SGMLApplication.h.

#include "Boolean.h"
#include "SGMLApplication.h"
#include "Event.h"
#include "MessageReporter.h"
#include "ErrorCountEventHandler.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API GenericEventHandler : public ErrorCountEventHandler {
public:
  GenericEventHandler(SGMLApplication &, bool generalEntities);
  ~GenericEventHandler();
  void message(MessageEvent *);
  void appinfo(AppinfoEvent *);
  void startDtd(StartDtdEvent *);
  void endDtd(EndDtdEvent *);
  void endProlog(EndPrologEvent *);
  void entityDefaulted(EntityDefaultedEvent *);
  void startElement(StartElementEvent *);
  void endElement(EndElementEvent *);
  void data(DataEvent *);
  void pi(PiEvent *);
  void sdataEntity(SdataEntityEvent *);
  void externalDataEntity(ExternalDataEntityEvent *);
  void subdocEntity(SubdocEntityEvent *);
  void commentDecl(CommentDeclEvent *);
  void ignoredChars(IgnoredCharsEvent *);
  void markedSectionStart(MarkedSectionStartEvent *);
  void markedSectionEnd(MarkedSectionEndEvent *);
  void *allocate(size_t);
  void freeAll();
  void freeAll1();

  struct Block {
    Block *next;
    char *mem;
    size_t size;
  };
  static void setString(SGMLApplication::CharString &, const StringC &);
  static void clearString(SGMLApplication::CharString &);
  virtual void reportMessage(const Message &msg, StringC &) = 0;
private:
  GenericEventHandler(const GenericEventHandler &); // undefined
  void operator=(const GenericEventHandler &);  // undefined
  void setLocation(SGMLApplication::Position &, const Location &);
  void setLocation1(SGMLApplication::Position &, const Location &);
  void setAttributes(const SGMLApplication::Attribute *&attributes,
		     const AttributeList &attributeList);
  void setExternalId(SGMLApplication::ExternalId &to,
		     const ExternalId &from);
  void setEntity(SGMLApplication::Entity &to, const Entity &from);
  void setNotation(SGMLApplication::Notation &, const Notation &notation);
  static void clearNotation(SGMLApplication::Notation &);
  static void clearExternalId(SGMLApplication::ExternalId &);
  ConstPtr<Origin> lastOrigin_;
  SGMLApplication::OpenEntityPtr openEntityPtr_;
  size_t firstBlockUsed_;
  size_t firstBlockSpare_;
  Block *freeBlocks_;
  Block *allocBlocks_;
  bool generalEntities_;
  SGMLApplication *app_;
};

class SP_API MsgGenericEventHandler : public GenericEventHandler {
public:
  MsgGenericEventHandler(SGMLApplication &, 
			 bool generalEntities,
			 MessageReporter &reporter,
			 const bool *messagesInhibitedPtr);
  void reportMessage(const Message &msg, StringC &);
private:
  MsgGenericEventHandler(const MsgGenericEventHandler &); // undefined
  void operator=(const MsgGenericEventHandler &);  // undefined
  struct WrapReporter {
    WrapReporter(MessageReporter *r) : reporter(r), origStream(0) {
      origStream = reporter->releaseMessageStream();
      reporter->setMessageStream(&strStream);
    }
    ~WrapReporter() {
      if (origStream) {
	reporter->releaseMessageStream();
	reporter->setMessageStream(origStream);
      }
    }
    MessageReporter *reporter;
    OutputCharStream *origStream;
    StrOutputCharStream strStream;
  };
  const bool *messagesInhibitedPtr_;
  MessageReporter *reporter_;
};

inline void
GenericEventHandler::setString(SGMLApplication::CharString &to,
			   const StringC &from)
{
  to.ptr = from.data();
  to.len = from.size();
}

inline
void GenericEventHandler::clearString(SGMLApplication::CharString &to)
{
  to.len = 0;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not GenericEventHandler_INCLUDED */
