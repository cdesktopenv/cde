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
/* $XConsortium: PosixStorageMessages.h /main/1 1996/07/29 17:01:59 cde-hp $ */
// This file was automatically generated from PosixStorageMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct PosixStorageMessages {
  // 2200
  static const MessageType2 readSystemCall;
  // 2201
  static const MessageType2 openSystemCall;
  // 2202
  static const MessageType2 closeSystemCall;
  // 2203
  static const MessageType2 lseekSystemCall;
  // 2204
  static const MessageType1 invalidFilename;
  // 2205
  static const MessageType2 fdRead;
  // 2206
  static const MessageType2 fdLseek;
  // 2207
  static const MessageType1 invalidNumber;
  // 2208
  static const MessageType2 cannotFind;
};
const MessageType2 PosixStorageMessages::readSystemCall(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2200
#ifndef SP_NO_MESSAGE_TEXT
,"error reading %1 (%2)"
#endif
);
const MessageType2 PosixStorageMessages::openSystemCall(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2201
#ifndef SP_NO_MESSAGE_TEXT
,"cannot open %1 (%2)"
#endif
);
const MessageType2 PosixStorageMessages::closeSystemCall(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2202
#ifndef SP_NO_MESSAGE_TEXT
,"error closing %1 (%2)"
#endif
);
const MessageType2 PosixStorageMessages::lseekSystemCall(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2203
#ifndef SP_NO_MESSAGE_TEXT
,"error seeking on %1 (%2)"
#endif
);
const MessageType1 PosixStorageMessages::invalidFilename(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2204
#ifndef SP_NO_MESSAGE_TEXT
,"invalid filename %1"
#endif
);
const MessageType2 PosixStorageMessages::fdRead(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2205
#ifndef SP_NO_MESSAGE_TEXT
,"error reading file descriptor %1 (%2)"
#endif
);
const MessageType2 PosixStorageMessages::fdLseek(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2206
#ifndef SP_NO_MESSAGE_TEXT
,"error seeking on file descriptor %1 (%2)"
#endif
);
const MessageType1 PosixStorageMessages::invalidNumber(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2207
#ifndef SP_NO_MESSAGE_TEXT
,"%1 is not a valid file descriptor number"
#endif
);
const MessageType2 PosixStorageMessages::cannotFind(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2208
#ifndef SP_NO_MESSAGE_TEXT
,"cannot find %1; tried %2"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
