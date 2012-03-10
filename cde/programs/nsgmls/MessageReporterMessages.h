/* $XConsortium: MessageReporterMessages.h /main/1 1996/07/29 16:57:40 cde-hp $ */
// This file was automatically generated from MessageReporterMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct MessageReporterMessages {
  // 5000
  static const MessageFragment infoTag;
  // 5001
  static const MessageFragment warningTag;
  // 5002
  static const MessageFragment quantityErrorTag;
  // 5003
  static const MessageFragment idrefErrorTag;
  // 5004
  static const MessageFragment errorTag;
  // 5005
  static const MessageFragment openElements;
  // 5006
  static const MessageFragment inNamedEntity;
  // 5007
  static const MessageFragment inUnnamedEntity;
  // 5008
  static const MessageFragment invalidLocation;
  // 5009
  static const MessageFragment offset;
  // 5010
  static const MessageFragment ordinal1;
  // 5011
  static const MessageFragment ordinal2;
  // 5012
  static const MessageFragment ordinal3;
  // 5013
  static const MessageFragment ordinaln;
  // 5014
  static const MessageFragment invalidArgumentType;
  // 5015
  static const MessageFragment invalidMessage;
};
const MessageFragment MessageReporterMessages::infoTag(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5000
#ifndef SP_NO_MESSAGE_TEXT
,"I"
#endif
);
const MessageFragment MessageReporterMessages::warningTag(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5001
#ifndef SP_NO_MESSAGE_TEXT
,"W"
#endif
);
const MessageFragment MessageReporterMessages::quantityErrorTag(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5002
#ifndef SP_NO_MESSAGE_TEXT
,"Q"
#endif
);
const MessageFragment MessageReporterMessages::idrefErrorTag(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5003
#ifndef SP_NO_MESSAGE_TEXT
,"X"
#endif
);
const MessageFragment MessageReporterMessages::errorTag(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5004
#ifndef SP_NO_MESSAGE_TEXT
,"E"
#endif
);
const MessageFragment MessageReporterMessages::openElements(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5005
#ifndef SP_NO_MESSAGE_TEXT
,"open elements"
#endif
);
const MessageFragment MessageReporterMessages::inNamedEntity(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5006
#ifndef SP_NO_MESSAGE_TEXT
,"In entity %1 included from %2"
#endif
);
const MessageFragment MessageReporterMessages::inUnnamedEntity(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5007
#ifndef SP_NO_MESSAGE_TEXT
,"In entity included from %2"
#endif
);
const MessageFragment MessageReporterMessages::invalidLocation(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5008
#ifndef SP_NO_MESSAGE_TEXT
,"(invalid location)"
#endif
);
const MessageFragment MessageReporterMessages::offset(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5009
#ifndef SP_NO_MESSAGE_TEXT
,"offset "
#endif
);
const MessageFragment MessageReporterMessages::ordinal1(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5010
#ifndef SP_NO_MESSAGE_TEXT
,"st"
#endif
);
const MessageFragment MessageReporterMessages::ordinal2(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5011
#ifndef SP_NO_MESSAGE_TEXT
,"nd"
#endif
);
const MessageFragment MessageReporterMessages::ordinal3(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5012
#ifndef SP_NO_MESSAGE_TEXT
,"rd"
#endif
);
const MessageFragment MessageReporterMessages::ordinaln(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5013
#ifndef SP_NO_MESSAGE_TEXT
,"th"
#endif
);
const MessageFragment MessageReporterMessages::invalidArgumentType(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5014
#ifndef SP_NO_MESSAGE_TEXT
,"(invalid argument type)"
#endif
);
const MessageFragment MessageReporterMessages::invalidMessage(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5015
#ifndef SP_NO_MESSAGE_TEXT
,"(invalid message)"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
