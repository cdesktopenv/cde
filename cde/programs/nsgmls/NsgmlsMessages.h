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
