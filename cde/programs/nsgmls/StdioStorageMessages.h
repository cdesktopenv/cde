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
/* $XConsortium: StdioStorageMessages.h /main/1 1996/07/29 17:04:53 cde-hp $ */
// This file was automatically generated from StdioStorageMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct StdioStorageMessages {
  // 2400
  static const MessageType2 openFailed;
  // 2401
  static const MessageType2 readFailed;
  // 2402
  static const MessageType2 seekFailed;
};
const MessageType2 StdioStorageMessages::openFailed(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2400
#ifndef SP_NO_MESSAGE_TEXT
,"cannot open %1 (%2)"
#endif
);
const MessageType2 StdioStorageMessages::readFailed(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2401
#ifndef SP_NO_MESSAGE_TEXT
,"\"error reading %1 (%2)"
#endif
);
const MessageType2 StdioStorageMessages::seekFailed(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2402
#ifndef SP_NO_MESSAGE_TEXT
,"error seeking %1 (%2)"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
