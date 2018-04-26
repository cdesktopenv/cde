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
/* $XConsortium: EntityManagerMessages.h /main/1 1996/07/29 16:50:47 cde-hp $ */
// This file was automatically generated from EntityManagerMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct EntityManagerMessages {
  // 2000
  static const MessageType1 fsiSyntax;
  // 2001
  static const MessageType1 fsiMissingValue;
  // 2002
  static const MessageType1 fsiValueAsName;
  // 2003
  static const MessageType1 fsiBadSmcrd;
  // 2004
  static const MessageType1 fsiUnknownBctf;
  // 2005
  static const MessageType1 fsiUnsupportedRecords;
  // 2006
  static const MessageType1 fsiUnsupportedAttribute;
  // 2007
  static const MessageType1 fsiUnsupportedAttributeToken;
  // 2008
  static const MessageType1 fsiBadTracking;
  // 2009
  static const MessageType1 fsiDuplicateAttribute;
  // 2010
  static const MessageType1 fsiBadZapeof;
  // 2011
  static const MessageType1 fsiBadSearch;
  // 2012
  static const MessageType1 fsiBadFold;
  // 2013
  static const MessageType0 fsiFoldNotNeutral;
  // 2014
  static const MessageType0 fsiBctfNotApplicable;
  // 2015
  static const MessageType0 fsiZapeofNotApplicable;
  // 2016
  static const MessageType0 fsiRecordsNotApplicable;
  // 2017
  static const MessageType1 fsiBadIndirect;
  // 2018
  static const MessageType1 fsiLookupChar;
};
const MessageType1 EntityManagerMessages::fsiSyntax(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2000
#ifndef SP_NO_MESSAGE_TEXT
,"bad formal system identifier syntax in %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiMissingValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2001
#ifndef SP_NO_MESSAGE_TEXT
,"value for attribute %1 missing in formal system identifier"
#endif
);
const MessageType1 EntityManagerMessages::fsiValueAsName(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2002
#ifndef SP_NO_MESSAGE_TEXT
,"%1 is a formal system identifier attribute value not an attribute name"
#endif
);
const MessageType1 EntityManagerMessages::fsiBadSmcrd(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2003
#ifndef SP_NO_MESSAGE_TEXT
,"value of SMCRD attribute must be a single character not %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiUnknownBctf(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2004
#ifndef SP_NO_MESSAGE_TEXT
,"unknown BCTF %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiUnsupportedRecords(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2005
#ifndef SP_NO_MESSAGE_TEXT
,"unsupported record boundary indicator %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiUnsupportedAttribute(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2006
#ifndef SP_NO_MESSAGE_TEXT
,"unsupported formal system identifier attribute %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiUnsupportedAttributeToken(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2007
#ifndef SP_NO_MESSAGE_TEXT
,"unsupported formal system identifier attribute value %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiBadTracking(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2008
#ifndef SP_NO_MESSAGE_TEXT
,"bad value %1 for formal system identifier tracking attribute"
#endif
);
const MessageType1 EntityManagerMessages::fsiDuplicateAttribute(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2009
#ifndef SP_NO_MESSAGE_TEXT
,"duplicate specification for formal system identifier attribute %1"
#endif
);
const MessageType1 EntityManagerMessages::fsiBadZapeof(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2010
#ifndef SP_NO_MESSAGE_TEXT
,"bad value %1 for formal system identifier zapeof attribute"
#endif
);
const MessageType1 EntityManagerMessages::fsiBadSearch(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2011
#ifndef SP_NO_MESSAGE_TEXT
,"bad value %1 for formal system identifier search attribute"
#endif
);
const MessageType1 EntityManagerMessages::fsiBadFold(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2012
#ifndef SP_NO_MESSAGE_TEXT
,"bad value %1 for formal system identifier fold attribute"
#endif
);
const MessageType0 EntityManagerMessages::fsiFoldNotNeutral(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2013
#ifndef SP_NO_MESSAGE_TEXT
,"fold attribute allowed only for neutral storage manager"
#endif
);
const MessageType0 EntityManagerMessages::fsiBctfNotApplicable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2014
#ifndef SP_NO_MESSAGE_TEXT
,"BCTF attribute not applicable to this storage manager"
#endif
);
const MessageType0 EntityManagerMessages::fsiZapeofNotApplicable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2015
#ifndef SP_NO_MESSAGE_TEXT
,"ZAPEOF attribute not applicable to this storage manager"
#endif
);
const MessageType0 EntityManagerMessages::fsiRecordsNotApplicable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2016
#ifndef SP_NO_MESSAGE_TEXT
,"RECORDS attribute not applicable to this storage manager"
#endif
);
const MessageType1 EntityManagerMessages::fsiBadIndirect(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2017
#ifndef SP_NO_MESSAGE_TEXT
,"bad value %1 for formal system identifier indirect attribute"
#endif
);
const MessageType1 EntityManagerMessages::fsiLookupChar(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2018
#ifndef SP_NO_MESSAGE_TEXT
,"non-minimum data character (number %1) in value of formal system identifier lookup attribute"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
