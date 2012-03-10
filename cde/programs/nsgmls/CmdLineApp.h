/* $XConsortium: CmdLineApp.h /main/1 1996/07/29 16:47:46 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef CmdLineApp_INCLUDED
#define CmdLineApp_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MessageReporter.h"
#include "Vector.h"
#include "StringOf.h"
#include "Boolean.h"
#include "CodingSystem.h"
#include "OutputCharStream.h"

#ifdef SP_WIDE_SYSTEM
// for wchar_t
#include <stddef.h>
#endif

class filebuf;

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API CmdLineApp  : public MessageReporter {
public:
#ifdef SP_WIDE_SYSTEM
#define AppChar wchar_t
#else
#define AppChar char
#endif
  CmdLineApp();
  int run(int argc, AppChar **argv);
  virtual int processOptions(int argc, AppChar **argv, int &nextArg);
  virtual void processOption(AppChar opt, const AppChar *arg);
  virtual int processArguments(int argc, AppChar **files) = 0;
  static Boolean openFilebufWrite(filebuf &file, const AppChar *filename);
  StringC usageString();
  static const CodingSystem *codingSystem();
  static const CodingSystem *lookupCodingSystem(const AppChar *);
  static const CodingSystem *codingSystem(size_t, const char *&);
  static StringC convertInput(const AppChar *s);
  OutputCharStream *makeStdOut();
  static OutputCharStream *makeStdErr();
protected:
  virtual void registerOption(AppChar c, const AppChar *argName = 0);
  virtual int init(int argc, AppChar **argv);
  const AppChar *errorFile_;
  const CodingSystem *outputCodingSystem_;
  String<AppChar> optstr_;
  Vector<const AppChar *> optArgNames_;
private:
  Boolean getMessageText(const MessageFragment &, StringC &);
  static const CodingSystem *codingSystem_;
};

#ifdef SP_WIDE_SYSTEM
#define SP_DEFINE_APP(CLASS) \
  extern "C" \
  wmain(int argc, wchar_t **argv) { CLASS app; return app.run(argc, argv); }
#else
#define SP_DEFINE_APP(CLASS) \
 int main(int argc, char **argv) { CLASS app; return app.run(argc, argv); }
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* not CmdLineApp_INCLUDED */
