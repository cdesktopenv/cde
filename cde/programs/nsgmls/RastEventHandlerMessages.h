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
/* $XConsortium: RastEventHandlerMessages.h /main/1 1996/07/29 17:02:35 cde-hp $ */
// This file was automatically generated from RastEventHandlerMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct RastEventHandlerMessages {
  // 100
  static const MessageType0 invalidRastPiError;
  // 101
  static const MessageType1 invalidActiveLinkType;
  // 102
  static const MessageType1 duplicateActiveLinkType;
  // 103
  static const MessageType0 multipleLinkRuleMatch;
  // 104
  static const MessageType0 noLinkRuleMatch;
  // 105
  static const MessageType0 multipleLinkRules;
};
const MessageType0 RastEventHandlerMessages::invalidRastPiError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
100
#ifndef SP_NO_MESSAGE_TEXT
,"invalid RAST processing instruction"
#endif
);
const MessageType1 RastEventHandlerMessages::invalidActiveLinkType(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
101
#ifndef SP_NO_MESSAGE_TEXT
,"invalid link type %1 in rast-active-lpd processing instruction"
#endif
);
const MessageType1 RastEventHandlerMessages::duplicateActiveLinkType(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
102
#ifndef SP_NO_MESSAGE_TEXT
,"duplicate link type %1 in rast-active-lpd processing instruction"
#endif
);
const MessageType0 RastEventHandlerMessages::multipleLinkRuleMatch(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
103
#ifndef SP_NO_MESSAGE_TEXT
,"rast-link-rule: processing instruction matches more than one link rule"
#endif
);
const MessageType0 RastEventHandlerMessages::noLinkRuleMatch(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
104
#ifndef SP_NO_MESSAGE_TEXT
,"rast-link-rule: processing instruction matches does not match any link rules"
#endif
);
const MessageType0 RastEventHandlerMessages::multipleLinkRules(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
105
#ifndef SP_NO_MESSAGE_TEXT
,"multiple applicable link rules without disambiguating rast-link-rule: processing instruction"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
