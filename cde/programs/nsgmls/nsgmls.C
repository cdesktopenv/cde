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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: nsgmls.C /main/1 1996/07/29 17:08:59 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#include "config.h"
#include "Event.h"
#include "MessageEventHandler.h"
#include "SgmlsEventHandler.h"
#include "RastEventHandler.h"
#include "OutputCharStream.h"
#include "Boolean.h"
#include "NsgmlsMessages.h"
#include "MessageArg.h"
#include "ErrnoMessageArg.h"
#include "ParserApp.h"
#include "sptchar.h"
#include "macros.h"

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <iostream>
#include <fstream>
#else
#include <iostream.h>
#include <fstream.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#ifdef SP_NAMESPACE
using namespace SP_NAMESPACE;
#endif

class NsgmlsApp : public ParserApp {
public:
  NsgmlsApp();
  int processArguments(int argc, AppChar **argv);
  ErrorCountEventHandler *makeEventHandler();
  void processOption(AppChar opt, const AppChar *arg);
  void allLinkTypesActivated();
private:
  Boolean suppressOutput_;
  Boolean prologOnly_;
  unsigned outputFlags_;
  String<AppChar> rastFile_;
  const AppChar *rastOption_;
  Boolean batchMode_;
};

SP_DEFINE_APP(NsgmlsApp)

class PrologMessageEventHandler : public MessageEventHandler {
public:
  PrologMessageEventHandler(Messenger *messenger);
  void endProlog(EndPrologEvent *);
};

class XRastEventHandler : public RastEventHandler {
public:
  XRastEventHandler(SgmlParser *,
		    const AppChar *filename,
		    const StringC &filenameStr,
		    const OutputCodingSystem *,
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
		    ::Messenger *messenger);
#else
		    Messenger *messenger);
#endif
  ~XRastEventHandler();
  void message(MessageEvent *);
  void truncateOutput();
  void allLinkTypesActivated();
private:
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
  ::Messenger *messenger_;
#else
  Messenger *messenger_;
#endif
  // file_ must come before os_ so it gets inited first
  filebuf file_;
  IosOutputCharStream os_;
  const AppChar *filename_;
  const StringC filenameStr_;
};

NsgmlsApp::NsgmlsApp()
: suppressOutput_(0),
  batchMode_(0),
  prologOnly_(0),
  outputFlags_(0),
  rastOption_(0)
{
  registerOption('B');
  registerOption('d');
  registerOption('l');
  registerOption('m', SP_T("catalog_sysid"));
  registerOption('o', SP_T("output_option"));
  registerOption('p');
  registerOption('r');
  registerOption('s');
  registerOption('t', SP_T("rast_file"));
  registerOption('u');
}

void NsgmlsApp::processOption(AppChar opt, const AppChar *arg)
{
  switch (opt) {
  case 'B':
    batchMode_ = 1;
    break;
  case 'd':
    // warn about duplicate entity declarations
    options_.warnDuplicateEntity = 1;
    break;
  case 'l':
    // output L commands
    outputFlags_ |= SgmlsEventHandler::outputLine;
    break;
  case 'm':
    processOption(SP_T('c'), arg);
    break;
  case 'o':
    {
      static struct {
	const AppChar *name;
	unsigned flag;
      } outputOptions[] = {
	{ SP_T("line"), SgmlsEventHandler::outputLine },
	{ SP_T("entity"), SgmlsEventHandler::outputEntity },
	{ SP_T("id"), SgmlsEventHandler::outputId },
	{ SP_T("included"), SgmlsEventHandler::outputIncluded },
	{ SP_T("notation-sysid"), SgmlsEventHandler::outputNotationSysid },
      };
      Boolean found = 0;
      for (size_t i = 0; i < SIZEOF(outputOptions); i++)
	if (tcscmp(arg, outputOptions[i].name) == 0) {
	  outputFlags_ |= outputOptions[i].flag;
	  found = 1;
	  break;
	}
      if (!found)
	message(NsgmlsMessages::unknownOutputOption,
		StringMessageArg(convertInput(arg)));
    }
    break;
  case 'p':
    prologOnly_ = 1;
    break;
  case 'r':
    // warn about defaulted entity reference
    options_.warnDefaultEntityReference = 1;
    break;
  case 's':
    suppressOutput_ = 1;
    break;
  case 't':
    rastOption_ = arg;
    break;
  case 'u':
    // warn about undefined elements
    options_.warnUndefinedElement = 1;
    break;
  default:
    ParserApp::processOption(opt, arg);
    break;
  }
}

int NsgmlsApp::processArguments(int argc, AppChar **argv)
{
  if (batchMode_) {
    int ret = 0;
    for (int i = 0; i < argc; i++) {
      if (rastOption_) {
	rastFile_.assign(rastOption_, tcslen(rastOption_));
	rastFile_.append(argv[i], tcslen(argv[i]));
	rastFile_ += SP_T('\0');
      }
      int tem = ParserApp::processArguments(1, argv + i);
      if (tem > ret)
	ret = tem;
    }
    return ret;
  }
  else
    return ParserApp::processArguments(argc, argv);
}

void NsgmlsApp::allLinkTypesActivated()
{
  if (!rastOption_)
    ParserApp::allLinkTypesActivated();
}

ErrorCountEventHandler *NsgmlsApp::makeEventHandler()
{
  if (prologOnly_)
    return new PrologMessageEventHandler(this);
  else if (rastOption_) {
    const AppChar *s = batchMode_ ? rastFile_.data() : rastOption_;
    return new XRastEventHandler(&parser_, s, convertInput(s),
				 outputCodingSystem_, this);
  }
  else if (suppressOutput_)
    return new MessageEventHandler(this, &parser_);
  else
    return new SgmlsEventHandler(&parser_,
				 makeStdOut(),
				 this,
				 outputFlags_);
}

PrologMessageEventHandler::PrologMessageEventHandler(Messenger *messenger)
: MessageEventHandler(messenger)
{
}

void PrologMessageEventHandler::endProlog(EndPrologEvent *event)
{
  cancel();
  delete event;
}

XRastEventHandler::XRastEventHandler(SgmlParser *parser,
				     const AppChar *filename,
				     const StringC &filenameStr,
				     const OutputCodingSystem *codingSystem,
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
				     ::Messenger *messenger)
#else
				     ::Messenger *messenger)
#endif
: RastEventHandler(parser, messenger),
  messenger_(messenger),
  filename_(filename),
  filenameStr_(filenameStr)
{
  errno = 0;
  if (!CmdLineApp::openFilebufWrite(file_, filename)) {
    messenger->message(NsgmlsMessages::cannotOpenOutputError,
		      StringMessageArg(filenameStr),
		      ErrnoMessageArg(errno));
    exit(1);
  }
  os_.open(&file_, codingSystem);
  setOutputStream(&os_);
}

XRastEventHandler::~XRastEventHandler()
{
  end();
}

void XRastEventHandler::truncateOutput()
{
  os_.flush();
  errno = 0;
  if (!file_.close())
    messenger_->message(NsgmlsMessages::closeOutputError,
			StringMessageArg(filenameStr_),
			ErrnoMessageArg(errno));
  errno = 0;
  if (!CmdLineApp::openFilebufWrite(file_, filename_)) {
    messenger_->message(NsgmlsMessages::cannotOpenOutputError,
			StringMessageArg(filenameStr_),
			ErrnoMessageArg(errno));
    exit(1);
  }
}

void XRastEventHandler::message(MessageEvent *event)
{
  messenger_->dispatchMessage(event->message());
  ErrorCountEventHandler::message(event);
}
