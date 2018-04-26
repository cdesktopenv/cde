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
/* $XConsortium: MessageReporter.h /main/1 1996/07/29 16:57:36 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef MessageReporter_INCLUDED
#define MessageReporter_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "MessageBuilder.h"
#include "Boolean.h"
#include "OutputCharStream.h"
#include "Message.h"
#include "Location.h"
#include "StringC.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API MessageReporter : private MessageBuilder, public Messenger {
public:
  enum Option {
    openElements = 01,
    openEntities = 02,
    messageNumbers = 04
    };
  // The OutputCharStream will be deleted by the MessageReporter
  MessageReporter(OutputCharStream *);
  ~MessageReporter();
  void setMessageStream(OutputCharStream *);
  OutputCharStream *releaseMessageStream();
  void dispatchMessage(const Message &);
  void dispatchMessage(Message &);
  virtual Boolean getMessageText(const MessageFragment &, StringC &);
  void addOption(Option);
  void setProgramName(const StringC &);
  void formatMessage(const MessageFragment &,
		     const Vector<CopyOwner<MessageArg> > &args);
private:
  MessageReporter(const MessageReporter &); // undefined
  void operator=(const MessageReporter &);  // undefined
  
  void appendNumber(unsigned long);
  void appendOrdinal(unsigned long);
  void appendChars(const Char *, size_t);
  void appendOther(const OtherMessageArg *);
  void appendFragment(const MessageFragment &);
  const ExternalInfo *locationHeader(const Location &loc, Offset &off);
  void printLocation(const ExternalInfo *info, Offset off);
  OutputCharStream &os();

  OutputCharStream *os_;
  unsigned options_;
  StringC programName_;
};

inline
OutputCharStream &MessageReporter::os()
{
  return *os_;
}

inline
void MessageReporter::setProgramName(const StringC &programName)
{
  programName_ = programName;
}

inline
OutputCharStream *MessageReporter::releaseMessageStream()
{
  OutputCharStream *tem = os_;
  os_ = 0;
  return tem;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not MessageReporter_INCLUDED */
