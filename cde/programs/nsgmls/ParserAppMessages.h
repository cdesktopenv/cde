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
/* $XConsortium: ParserAppMessages.h /main/1 1996/07/29 17:00:43 cde-hp $ */
// This file was automatically generated from ParserAppMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct ParserAppMessages {
  // 4200
  static const MessageType1 unknownWarning;
  // 4201
  static const MessageType0 badErrorLimit;
  // 4202
  static const MessageType1 errorLimitExceeded;
};
const MessageType1 ParserAppMessages::unknownWarning(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4200
#ifndef SP_NO_MESSAGE_TEXT
,"unknown warning type %1"
#endif
);
const MessageType0 ParserAppMessages::badErrorLimit(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4201
#ifndef SP_NO_MESSAGE_TEXT
,"invalid error limit"
#endif
);
const MessageType1 ParserAppMessages::errorLimitExceeded(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4202
#ifndef SP_NO_MESSAGE_TEXT
,"maximum number of errors (%1) reached; change with -E option"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
