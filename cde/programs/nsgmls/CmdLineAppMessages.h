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
/* $XConsortium: CmdLineAppMessages.h /main/1 1996/07/29 16:47:51 cde-hp $ */
// This file was automatically generated from CmdLineAppMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct CmdLineAppMessages {
  // 4000
  static const MessageType1 invalidOptionError;
  // 4001
  static const MessageType1 missingOptionArgError;
  // 4002
  static const MessageType1 usage;
  // 4003
  static const MessageType1 versionInfo;
  // 4004
  static const MessageType1 unknownBctf;
  // 4005
  static const MessageType2 cannotOpenOutputError;
};
const MessageType1 CmdLineAppMessages::invalidOptionError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4000
#ifndef SP_NO_MESSAGE_TEXT
,"invalid option %1"
#endif
);
const MessageType1 CmdLineAppMessages::missingOptionArgError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4001
#ifndef SP_NO_MESSAGE_TEXT
,"missing argument for option %1"
#endif
);
const MessageType1 CmdLineAppMessages::usage(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4002
#ifndef SP_NO_MESSAGE_TEXT
,"usage is %1"
#endif
);
const MessageType1 CmdLineAppMessages::versionInfo(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4003
#ifndef SP_NO_MESSAGE_TEXT
,"version %1"
#endif
);
const MessageType1 CmdLineAppMessages::unknownBctf(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4004
#ifndef SP_NO_MESSAGE_TEXT
,"unknown BCTF %1"
#endif
);
const MessageType2 CmdLineAppMessages::cannotOpenOutputError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4005
#ifndef SP_NO_MESSAGE_TEXT
,"cannot open output file %1 (%2)"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
