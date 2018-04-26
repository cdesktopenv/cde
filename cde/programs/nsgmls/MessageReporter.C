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
/* $XConsortium: MessageReporter.C /main/1 1996/07/29 16:57:32 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "MessageReporter.h"
#include "ExtendEntityManager.h"
#include "StorageManager.h"
#include "macros.h"
#include "rtti.h"
#include "MessageArg.h"
#include "ErrnoMessageArg.h"
#include "SearchResultMessageArg.h"
#include "MessageReporterMessages.h"

#include <string.h>
#include <errno.h>

#ifdef DECLARE_STRERROR
extern "C" {
  char *strerror(int);
}
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

const OutputCharStream::Newline nl = OutputCharStream::newline;

MessageReporter::MessageReporter(OutputCharStream *os)
: os_(os), options_(0)
{
}

MessageReporter::~MessageReporter()
{
  delete os_;
}

void MessageReporter::setMessageStream(OutputCharStream *os)
{
  if (os != os_) {
    delete os_;
    os_ = os;
  }
}

void MessageReporter::addOption(Option option)
{
  options_ |= option;
}

void MessageReporter::dispatchMessage(Message &message)
{
  dispatchMessage((const Message &) message);
}

void MessageReporter::dispatchMessage(const Message &message)
{
  Offset off;
  const ExternalInfo *externalInfo = locationHeader(message.loc, off);
  if (programName_.size())
    os() << programName_ << ':';
  if (externalInfo) {
    printLocation(externalInfo, off);
    os() << ':';
  }
  if (options_ & messageNumbers)
    os() << (unsigned long)message.type->module() << "." 
      << (unsigned long)message.type->number() << ":";
  switch (message.type->severity()) {
  case MessageType::info:
    appendFragment(MessageReporterMessages::infoTag);
    break;
  case MessageType::warning:
    appendFragment(MessageReporterMessages::warningTag);
    break;
  case MessageType::quantityError:
    appendFragment(MessageReporterMessages::quantityErrorTag);
    break;
  case MessageType::idrefError:
    appendFragment(MessageReporterMessages::idrefErrorTag);
    break;
  case MessageType::error:
    appendFragment(MessageReporterMessages::errorTag);
    break;
  default:
    CANNOT_HAPPEN();
  }
  os() << ": ";
  formatMessage(*message.type, message.args);
  os() << nl;
  if (!message.auxLoc.origin().isNull()) {
    Offset off;
    const ExternalInfo *externalInfo = locationHeader(message.auxLoc, off);
    if (programName_.size())
      os() << programName_ << ':';
    if (externalInfo) {
      printLocation(externalInfo, off);
      os() << ": ";
    }
    formatMessage(message.type->auxFragment(), message.args);
    os() << nl;
  }
  if ((options_ & openElements) && message.openElementInfo.size() > 0) {
    if (programName_.size())
      os() << programName_ << ':';
    if (externalInfo) {
      printLocation(externalInfo, off);
      os() << ": ";
    }
    appendFragment(MessageReporterMessages::openElements);
    os() << ':';
    unsigned nOpenElements = message.openElementInfo.size();
    for (unsigned i = 0;; i++) {
      if (i > 0
	  && (i == nOpenElements || message.openElementInfo[i].included)) {
	// describe last match in previous open element
	const OpenElementInfo &prevInfo = message.openElementInfo[i - 1];
	if (prevInfo.matchType.size() != 0) {
	  os() << " (" << prevInfo.matchType;
	  if (prevInfo.matchIndex != 0)
	    os() << '[' << (unsigned long)prevInfo.matchIndex << ']';
	  os() << ')';
	}
      }
      if (i == nOpenElements)
	break;
      const OpenElementInfo &e = message.openElementInfo[i];
      os() << ' ' << e.gi;
      if (i > 0 && !e.included) {
	unsigned long n = message.openElementInfo[i - 1].matchIndex;
	if (n != 0)
	  os() << '[' << n << ']';
      }
    }
    os() << nl;
  }
  os().flush();
}

void MessageReporter::formatMessage(const MessageFragment &frag,
				    const Vector<CopyOwner<MessageArg> > &args)
{
  StringC text;
  if (!getMessageText(frag, text)) {
    appendFragment(MessageReporterMessages::invalidMessage);
    return;
  }
  size_t i = 0;
  while (i < text.size()) {
    if (text[i] == '%') {
      i++;
      if (i >= text.size())
	break;
      if (text[i] >= '1' && text[i] <= '9') {
	if (unsigned(text[i] - '1') < args.size())
	  args[text[i] - '1']->append(*this);
      }
      else
	os().put(text[i]);
      i++;
    }
    else {
      os().put(text[i]);
      i++;
    }
  }
}

const ExternalInfo *MessageReporter::locationHeader(const Location &loc,
						    Offset &off)
{
  const Origin *origin = loc.origin().pointer();
  Index index = loc.index();
  if (!(options_ & openEntities)) {
    while (origin) {
      const InputSourceOrigin *inputSourceOrigin = origin->asInputSourceOrigin();
      if (inputSourceOrigin) {
	const ExternalInfo *externalInfo = inputSourceOrigin->externalInfo();
	if (externalInfo) {
	  off = inputSourceOrigin->startOffset(index);
	  return externalInfo;
	}
      }
      const Location &loc = origin->parent();
      index = loc.index() + origin->refLength();
      origin = loc.origin().pointer();
    }
  }
  else {
    Boolean doneHeader = 0;
    while (origin) {
      const InputSourceOrigin *inputSourceOrigin = origin->asInputSourceOrigin();
      if (inputSourceOrigin) {
	if (!doneHeader) {
	  Offset parentOff;
	  Location parentLoc = inputSourceOrigin->parent();
	  parentLoc += inputSourceOrigin->refLength();
	  const ExternalInfo *parentInfo = locationHeader(parentLoc, parentOff);
	  if (parentInfo) {
	    StringC text;
	    if (getMessageText(inputSourceOrigin->entityName()
			       ? MessageReporterMessages::inNamedEntity
			       : MessageReporterMessages::inUnnamedEntity,
				text)) {
	      for (size_t i = 0; i < text.size(); i++) {
		if (text[i] == '%') {
		  if (i + 1 < text.size()) {
		    i++;
		    if (text[i] == '1')
		      os() << *inputSourceOrigin->entityName();
		    else if (text[i] == '2')
		      printLocation(parentInfo, parentOff);
		    else if (text[i] >= '3' && text[i] <= '9')
		      ;
		    else
		      os().put(text[i]);
		  }
		}
		else
		  os().put(text[i]);
	      }
	      os() << nl;
	    }
	  }
	  doneHeader = 1;
	}
	off = inputSourceOrigin->startOffset(index);
	const ExternalInfo *externalInfo = inputSourceOrigin->externalInfo();
	if (externalInfo)
	  return externalInfo;
	Location loc;
	if (!inputSourceOrigin->defLocation(off, loc))
	  break;
	index = loc.index();
	origin = loc.origin().pointer();
      }
      else {
	const Location &loc = origin->parent();
	index = loc.index() + origin->refLength();
	origin = loc.origin().pointer();
      }
    }
  }
  return 0;
}

void MessageReporter::printLocation(const ExternalInfo *externalInfo,
				    Offset off)
{
  if (!externalInfo) {
    appendFragment(MessageReporterMessages::invalidLocation);
    return;
  }
  StorageObjectLocation soLoc;
  if (!ExtendEntityManager::externalize(externalInfo, off, soLoc)) {
    appendFragment(MessageReporterMessages::invalidLocation);
    return;
  }
  if (strcmp(soLoc.storageObjectSpec->storageManager->type(), "OSFILE") != 0)
    os() << '<' << soLoc.storageObjectSpec->storageManager->type() << '>';
  os() << soLoc.storageObjectSpec->id;
  if (soLoc.lineNumber == (unsigned long)-1) {
    os() << ": ";
    appendFragment(MessageReporterMessages::offset);
    os() << soLoc.storageObjectOffset;
  }
  else {
    os() << ':' << soLoc.lineNumber;
    if (soLoc.columnNumber != 0 && soLoc.columnNumber != (unsigned long)-1)
      os() << ':' << soLoc.columnNumber - 1;
  }
#if 0
  if (soLoc.byteIndex != (unsigned long)-1)
    os() << ':' << soLoc.byteIndex;
#endif
}

void MessageReporter::appendNumber(unsigned long n)
{
  os() << n;
}

void MessageReporter::appendOrdinal(unsigned long n)
{
  os() << n;
  switch (n % 10) {
  case 1:
    appendFragment(MessageReporterMessages::ordinal1);
    break;
  case 2:
    appendFragment(MessageReporterMessages::ordinal2);
    break;
  case 3:
    appendFragment(MessageReporterMessages::ordinal3);
    break;
  default:
    appendFragment(MessageReporterMessages::ordinaln);
    break;
  }
}

void MessageReporter::appendChars(const Char *p, size_t n)
{
  os().put('"').write(p, n).put('"');
}

void MessageReporter::appendOther(const OtherMessageArg *p)
{
  const ErrnoMessageArg *ea = DYNAMIC_CAST_CONST_PTR(ErrnoMessageArg, p);
  
  if (ea) {
    os() << strerror(ea->errnum());
    return;
  }

  const SearchResultMessageArg *sr
    = DYNAMIC_CAST_CONST_PTR(SearchResultMessageArg, p);
  if (sr) {
    for (size_t i = 0; i < sr->nTried(); i++) {
      if (i > 0)
	os() << ", ";
      const StringC &f = sr->filename(i);
      appendChars(f.data(), f.size());
      switch (sr->errnum(i)) {
      default:
	os() << " (";
	os() << strerror(sr->errnum(i));
	os() << ")";
#ifdef ENOENT
      case ENOENT:
#endif
	break;
      }
    }
    return;
  }
  appendFragment(MessageReporterMessages::invalidArgumentType);
}

void MessageReporter::appendFragment(const MessageFragment &frag)
{
  StringC text;
  if (getMessageText(frag, text))
    os() << text;
}

Boolean MessageReporter::getMessageText(const MessageFragment &frag,
					StringC &str)
{
  const char *p = frag.text();
  if (!p)
    return 0;
  str.resize(0);
  for (; *p; p++)
    str += Char((unsigned char)*p);
  return 1;
}

#ifdef SP_NAMESPACE
}
#endif
