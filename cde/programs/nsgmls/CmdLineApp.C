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
/* $XConsortium: CmdLineApp.C /main/2 1996/08/12 12:36:14 mgreess $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

// Need option registration method that allows derived class to change
// option names.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "CmdLineApp.h"
#include "CmdLineAppMessages.h"
#include "MessageArg.h"
#include "ErrnoMessageArg.h"
#include "Options.h"
#include "version.h"
#include "xnew.h"
#include "macros.h"
#include "sptchar.h"
#include "MessageTable.h"

#ifdef SP_MULTI_BYTE
#include "UTF8CodingSystem.h"
#include "Fixed2CodingSystem.h"
#include "UnicodeCodingSystem.h"
#include "EUCJPCodingSystem.h"
#include "SJISCodingSystem.h"
#include "ISO8859InputCodingSystem.h"
#ifdef WIN32
#include "Win32CodingSystem.h"
#endif
#endif /* SP_MULTI_BYTE */
#include "IdentityCodingSystem.h"

#include "ConsoleOutput.h"

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <iostream>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <fstream.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef SP_HAVE_LOCALE
#include <locale.h>
#endif
#ifdef SP_HAVE_SETMODE
#include <fcntl.h>
#include <io.h>
#endif
#ifdef SP_HAVE_SETMODE
#define IOS_BINARY ios::binary
#else
#define IOS_BINARY 0
#endif
#ifdef SP_WIDE_SYSTEM

#include <stdio.h>

#else /* not SP_WIDE_SYSTEM */

#include <sys/types.h>
#ifdef SP_INCLUDE_UNISTD_H
#include <unistd.h>
#endif
#ifdef SP_INCLUDE_IO_H
#include <io.h>
#endif

#endif /* not SP_WIDE_SYSTEM */

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef SP_MULTI_BYTE
static UTF8CodingSystem utf8CodingSystem;
static Fixed2CodingSystem fixed2CodingSystem;
static UnicodeCodingSystem unicodeCodingSystem;
static EUCJPCodingSystem eucjpCodingSystem;
static SJISCodingSystem sjisCodingSystem;
#ifdef WIN32
static Win32CodingSystem ansiCodingSystem(Win32CodingSystem::codePageAnsi);
static Win32CodingSystem oemCodingSystem(Win32CodingSystem::codePageOEM);
static UnicodeCodingSystem maybeUnicodeCodingSystem(&ansiCodingSystem);
#endif
#endif /* SP_MULTI_BYTE */
static IdentityCodingSystem identityCodingSystem;

static struct {
  const char *name;
  const CodingSystem *cs;
} codingSystems[] = {
#ifdef SP_MULTI_BYTE
  { "UTF-8", &utf8CodingSystem },
  { "FIXED-2", &fixed2CodingSystem },
  { "UNICODE", &unicodeCodingSystem },
  { "EUC-JP", &eucjpCodingSystem },
  { "SJIS", &sjisCodingSystem },
#ifdef WIN32
  { "WINDOWS", &ansiCodingSystem },
  { "MS-DOS", &oemCodingSystem },
  { "WUNICODE", &maybeUnicodeCodingSystem },
#endif
#endif /* SP_MULTI_BYTE */
  { "IS8859-1", &identityCodingSystem },
  { "IDENTITY", &identityCodingSystem },
};

const CodingSystem *CmdLineApp::codingSystem_ = 0;

static const SP_TCHAR *progName = 0;

static const SP_TCHAR versionString[] = SP_VERSION;

CmdLineApp::CmdLineApp()
: errorFile_(0),
  outputCodingSystem_(0),
  // Colon at beginning is Posix.2ism that says to return : rather than ? for
  // missing option argument.
  optstr_(SP_T(":"), 1),
  MessageReporter(makeStdErr())
{
  registerOption('b', SP_T("bctf"));
  registerOption('f', SP_T("error_file"));
  registerOption('v');
}

void CmdLineApp::registerOption(AppChar c, const AppChar *argName)
{
  optstr_ += c;
  if (argName) {
    optstr_ += SP_T(':');
    optArgNames_.push_back(argName);
  }
}

StringC CmdLineApp::usageString()
{
  String<AppChar> result;
  size_t i;

  if (progName)
    result.assign(progName, tcslen(progName));
  PackedBoolean hadOption[128];
  for (i = 0; i < 128; i++)
    hadOption[i] = 0;
  Boolean hadNoArgOption = 0;
  for (i = 1; i < optstr_.size(); i++) {
    if (optstr_[i] == 0)
      break;
    if (i + 1 < optstr_.size() && optstr_[i + 1] == ':')
      i++;
    else if (!hadOption[optstr_[i]]) {
      hadOption[optstr_[i]] = 1;
      if (!hadNoArgOption) {
	hadNoArgOption = 1;
	result.append(SP_T(" [-"), 3);
      }
      result += optstr_[i];
    }
  }
  if (hadNoArgOption)
    result += SP_T(']');
  size_t j = 0;
  for (i = 1; i < optstr_.size(); i++) {
    if (i + 1 < optstr_.size() && optstr_[i + 1] == ':') {
      if (!hadOption[optstr_[i]]) {
	hadOption[optstr_[i]] = 1;
	result += SP_T(' ');
	result += SP_T('[');
	result += SP_T('-');
	result += optstr_[i];
	result += SP_T(' ');
	result.append(optArgNames_[j], tcslen(optArgNames_[j]));
	result += SP_T(']');
      }
      i++;
      j++;
    }
  }
  result.append(SP_T(" sysid..."), tcslen(SP_T(" sysid...")));
  result += 0;
  return convertInput(result.data());
}

static
void ewrite(const AppChar *s)
{
#ifdef SP_WIDE_SYSTEM
  fputts(s, stderr);
#else
  int n = (int)strlen(s);
  while (n > 0) {
    int nw = write(2, s, n);
    if (nw < 0)
      break;
    n -= nw;
    s += nw;
  }
#endif
}

static
#ifdef SP_FANCY_NEW_HANDLER
int outOfMemory(size_t)
#else
void outOfMemory()
#endif
{
  if (progName) {
    ewrite(progName);
    ewrite(SP_T(": "));
  }
  ewrite(SP_T(": out of memory\n"));
  exit(1);
#ifdef SP_FANCY_NEW_HANDLER
  return 0;
#endif  
}

int CmdLineApp::init(int, AppChar **argv)
{
  set_new_handler(outOfMemory);
#ifdef SP_HAVE_LOCALE
  setlocale(LC_ALL, "");
#endif
#ifdef SP_HAVE_SETMODE
  _setmode(1, _O_BINARY);
  _setmode(2, _O_BINARY);
#endif
  progName = argv[0];
  if (progName)
    setProgramName(convertInput(progName));
#ifdef __GNUG__
  // cout is a performance disaster in libg++ unless we do this.
  ios::sync_with_stdio(0);
#endif
  return 0;
}

int CmdLineApp::run(int argc, AppChar **argv)
{
  int ret = init(argc, argv);
  if (ret)
    return ret;
  int firstArg;
  ret = processOptions(argc, argv, firstArg);
  if (ret)
    return ret;
  ret = processArguments(argc - firstArg, argv + firstArg);
  progName = 0;
  return ret;
}    

Boolean CmdLineApp::openFilebufWrite(filebuf &file,
				     const AppChar *filename)
{
#ifdef SP_WIDE_SYSTEM
  int fd = _wopen(filename, _O_CREAT|_O_WRONLY|_O_TRUNC|_O_BINARY);
  if (fd < 0)
    return 0;
  return file.attach(fd) != 0;
#else
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
  return file.open(filename, ios::out|ios::trunc) != 0;
#else
  return file.open(filename, ios::out|ios::trunc|IOS_BINARY) != 0;
  #endif
#endif
}

int CmdLineApp::processOptions(int argc, AppChar **argv, int &nextArg)
{
  AppChar ostr[2];
  optstr_ += SP_T('\0');
  Options<AppChar> options(argc, argv, optstr_.data());
  AppChar opt;
  while (options.get(opt)) {
    switch (opt) {
    case ':':
      ostr[0] = options.opt();
      ostr[1] = SP_T('\0');
      message(CmdLineAppMessages::missingOptionArgError,
	      StringMessageArg(convertInput(ostr)));
      message(CmdLineAppMessages::usage,
	      StringMessageArg(usageString()));
      return 1;
    case '?':
      ostr[0] = options.opt();
      ostr[1] = SP_T('\0');
      message(CmdLineAppMessages::invalidOptionError,
	      StringMessageArg(convertInput(ostr)));
      message(CmdLineAppMessages::usage,
	      StringMessageArg(usageString()));
      return 1;
    default:
      processOption(opt, options.arg());
      break;
    }
  }
  nextArg = options.ind();
  if (errorFile_) {
    static filebuf file;
    if (!openFilebufWrite(file, errorFile_)) {
      message(CmdLineAppMessages::cannotOpenOutputError,
	      StringMessageArg(convertInput(errorFile_)),
	      ErrnoMessageArg(errno));
      return 1;
    }
    setMessageStream(new IosOutputCharStream(&file, codingSystem()));
  }
  if (!outputCodingSystem_)
    outputCodingSystem_ = codingSystem();
  return 0;
}

void CmdLineApp::processOption(AppChar opt, const AppChar *arg)
{
  switch (opt) {
  case 'b':
    outputCodingSystem_ = lookupCodingSystem(arg);
    if (!outputCodingSystem_)
      message(CmdLineAppMessages::unknownBctf,
	      StringMessageArg(convertInput(arg)));
    break;
  case 'f':
    errorFile_ = arg;
    break;
  case 'v':
    // print the version number
    message(CmdLineAppMessages::versionInfo,
	    StringMessageArg(convertInput(versionString)));
    break;
  default:
    CANNOT_HAPPEN();
  }
}

Boolean CmdLineApp::getMessageText(const MessageFragment &frag,
				   StringC &text)
{
  String<SP_TCHAR> str;
  if (!MessageTable::instance()->getText(frag, str))
    return 0;
#ifdef SP_WIDE_SYSTEM
  text.assign((const Char *)str.data(), str.size());
#else
  str += 0;
  text = codingSystem()->convertIn(str.data());
#endif
  return 1;
}

const CodingSystem *CmdLineApp::codingSystem()
{
  if (!codingSystem_) {
    const SP_TCHAR *codingName = tgetenv(SP_T("SP_BCTF"));
    if (codingName)
      codingSystem_ = lookupCodingSystem(codingName);
    if (!codingSystem_
#ifndef SP_WIDE_SYSTEM
	|| codingSystem_->fixedBytesPerChar() > 1
#endif
	)
      codingSystem_ = &identityCodingSystem;
  }
  return codingSystem_;
}

const CodingSystem *
CmdLineApp::lookupCodingSystem(const SP_TCHAR *codingName)
{
#define MAX_CS_NAME 50
  if (tcslen(codingName) < MAX_CS_NAME) {
    char buf[MAX_CS_NAME];
    int i;
    for (i = 0; codingName[i] != SP_T('\0'); i++) {
      SP_TUCHAR c = totupper((SP_TUCHAR)(codingName[i]));
#ifdef SP_WIDE_SYSTEM
      if (c > (unsigned char)-1)
	return 0;
#endif
      buf[i] = char(c);
    }
    buf[i] = SP_T('\0');
    {
    for (size_t i = 0; i < SIZEOF(codingSystems); i++)
      if (strcmp(buf, codingSystems[i].name) == 0)
	return codingSystems[i].cs;
    }
  }
  return 0;
}

const CodingSystem *
CmdLineApp::codingSystem(size_t i, const char *&name)
{
  if (i < SIZEOF(codingSystems)) {
    name = codingSystems[i].name;
    return codingSystems[i].cs;
  }
  return 0;
}

StringC CmdLineApp::convertInput(const SP_TCHAR *s)
{
#ifdef SP_WIDE_SYSTEM
  StringC str(s, wcslen(s));
#else
  StringC str(codingSystem()->convertIn(s));
#endif
  for (size_t i = 0; i < str.size(); i++)
    if (str[i] == '\n')
      str[i] = '\r';
  return str;
}

OutputCharStream *CmdLineApp::makeStdErr()
{
  OutputCharStream *os = ConsoleOutput::makeOutputCharStream(2);
  if (os)
    return os;
  return new IosOutputCharStream(cerr.rdbuf(), codingSystem());
}

OutputCharStream *CmdLineApp::makeStdOut()
{
  OutputCharStream *os = ConsoleOutput::makeOutputCharStream(1);
  if (os)
    return os;
  return new IosOutputCharStream(cout.rdbuf(), outputCodingSystem_);
}

#ifdef SP_NAMESPACE
}
#endif
