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
/* $XConsortium: NsgmlsMessages.h /main/1 1996/07/29 16:58:51 cde-hp $ */
// This file was automatically generated from NsgmlsMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct NsgmlsMessages {
  // 0
  static const MessageType2 cannotOpenOutputError;
  // 1
  static const MessageType2 closeOutputError;
  // 2
  static const MessageType1 unknownOutputOption;
};
const MessageType2 NsgmlsMessages::cannotOpenOutputError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
0
#ifndef SP_NO_MESSAGE_TEXT
,"cannot open output file %1 (%2)"
#endif
);
const MessageType2 NsgmlsMessages::closeOutputError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
1
#ifndef SP_NO_MESSAGE_TEXT
,"error closing output file %1 (%2)"
#endif
);
const MessageType1 NsgmlsMessages::unknownOutputOption(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2
#ifndef SP_NO_MESSAGE_TEXT
,"unknown output option %1"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
