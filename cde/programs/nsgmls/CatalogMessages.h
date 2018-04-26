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
/* $XConsortium: CatalogMessages.h /main/1 1996/07/29 16:47:02 cde-hp $ */
// This file was automatically generated from CatalogMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct CatalogMessages {
  // 2100
  static const MessageType0 nameExpected;
  // 2101
  static const MessageType0 literalExpected;
  // 2102
  static const MessageType0 nameOrLiteralExpected;
  // 2103
  static const MessageType0 nulChar;
  // 2104
  static const MessageType0 minimumData;
  // 2105
  static const MessageType0 eofInComment;
  // 2106
  static const MessageType0 eofInLiteral;
  // 2107
  static const MessageType0 overrideYesOrNo;
  // 2108
  static const MessageType0 inLoop;
  // 2109
  static const MessageType0 systemShouldQuote;
  // 2110
  static const MessageType1 noDocumentEntry;
  // 2111
  static const MessageType2 noPublicEntry;
};
const MessageType0 CatalogMessages::nameExpected(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2100
#ifndef SP_NO_MESSAGE_TEXT
,"name expected"
#endif
);
const MessageType0 CatalogMessages::literalExpected(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2101
#ifndef SP_NO_MESSAGE_TEXT
,"literal expected"
#endif
);
const MessageType0 CatalogMessages::nameOrLiteralExpected(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2102
#ifndef SP_NO_MESSAGE_TEXT
,"name or literal expected"
#endif
);
const MessageType0 CatalogMessages::nulChar(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2103
#ifndef SP_NO_MESSAGE_TEXT
,"nul character"
#endif
);
const MessageType0 CatalogMessages::minimumData(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2104
#ifndef SP_NO_MESSAGE_TEXT
,"not a minimum data character"
#endif
);
const MessageType0 CatalogMessages::eofInComment(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2105
#ifndef SP_NO_MESSAGE_TEXT
,"end of entity in comment"
#endif
);
const MessageType0 CatalogMessages::eofInLiteral(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2106
#ifndef SP_NO_MESSAGE_TEXT
,"end of entity in literal"
#endif
);
const MessageType0 CatalogMessages::overrideYesOrNo(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2107
#ifndef SP_NO_MESSAGE_TEXT
,"OVERRIDE requires argument of YES or NO"
#endif
);
const MessageType0 CatalogMessages::inLoop(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2108
#ifndef SP_NO_MESSAGE_TEXT
,"CATALOG entries cause loop"
#endif
);
const MessageType0 CatalogMessages::systemShouldQuote(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2109
#ifndef SP_NO_MESSAGE_TEXT
,"second argument for SYSTEM entry should be quoted to avoid ambiguity"
#endif
);
const MessageType1 CatalogMessages::noDocumentEntry(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2110
#ifndef SP_NO_MESSAGE_TEXT
,"no DOCUMENT entry in catalog %1"
#endif
);
const MessageType2 CatalogMessages::noPublicEntry(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2111
#ifndef SP_NO_MESSAGE_TEXT
,"no entry for public identifier %1 in catalog %2"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
