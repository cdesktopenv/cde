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
/* $XConsortium: ErrorCountEventHandler.h /main/1 1996/07/29 16:51:11 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef ErrorCountEventHandler_INCLUDED
#define ErrorCountEventHandler_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include <signal.h>
#include "Event.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ErrorCountEventHandler : public EventHandler {
public:
  ErrorCountEventHandler(unsigned errorLimit = 0);
  void setErrorLimit(unsigned maxErrors);
  const sig_atomic_t *cancelPtr() const;
  void cancel();
  Boolean cancelled() const;
  unsigned errorCount() const;
  void message(MessageEvent *);
  void noteMessage(const Message &);
private:
  unsigned maxErrors_;
  unsigned errorCount_;
  sig_atomic_t cancel_;
};

inline
unsigned ErrorCountEventHandler::errorCount() const
{
  return errorCount_;
}

inline
const sig_atomic_t *ErrorCountEventHandler::cancelPtr() const
{
  return &cancel_;
}

inline
void ErrorCountEventHandler::cancel()
{
  cancel_ = 1;
}

inline
void ErrorCountEventHandler::setErrorLimit(unsigned maxErrors)
{
  maxErrors_ = maxErrors;
}

inline
Boolean ErrorCountEventHandler::cancelled() const
{
  return cancel_ != 0;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ErrorCountEventHandler_INCLUDED */
