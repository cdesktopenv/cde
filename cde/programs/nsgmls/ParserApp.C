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
/* $XConsortium: ParserApp.C /main/1 1996/07/29 17:00:34 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "ParserApp.h"
#include "ParserAppMessages.h"
#include "MessageArg.h"
#include "Location.h"
#include "macros.h"
#include "sptchar.h"
#include "ArcEngine.h"

#include <limits.h>
#include <errno.h>
#include <stdlib.h>

#ifndef DEFAULT_ERROR_LIMIT
#define DEFAULT_ERROR_LIMIT 200
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

ParserApp::ParserApp()
: errorLimit_(DEFAULT_ERROR_LIMIT)
{
  registerOption('a', SP_T("link_type"));
  registerOption('A', SP_T("arch"));
  registerOption('e');
  registerOption('E', SP_T("max_errors"));
  registerOption('g');
  registerOption('i', SP_T("entity"));
  registerOption('w', SP_T("warning_type"));
}

void ParserApp::initParser(const StringC &sysid)
{
  SgmlParser::Params params;
  params.sysid = sysid;
  params.entityManager = entityManager().pointer();
  params.initialCharset = &systemCharset_.desc();
  params.options = &options_;
  parser_.init(params);
  if (arcNames_.size() > 0)
    parser_.activateLinkType(arcNames_[0]);
  for (size_t i = 0; i < activeLinkTypes_.size(); i++) 
    parser_.activateLinkType(convertInput(activeLinkTypes_[i]));
  allLinkTypesActivated();
}

void ParserApp::allLinkTypesActivated()
{
  parser_.allLinkTypesActivated();
}

int ParserApp::processSysid(const StringC &sysid)
{
  initParser(sysid);
  ErrorCountEventHandler *eceh = makeEventHandler();
  if (errorLimit_)
    eceh->setErrorLimit(errorLimit_);	
  return generateEvents(eceh);
}

int ParserApp::generateEvents(ErrorCountEventHandler *eceh)
{
  Owner<EventHandler> eh(eceh);
  parseAll(parser_, *eh, (SP_CONST sig_atomic_t*) eceh->cancelPtr());
  unsigned errorCount = eceh->errorCount();
  if (errorLimit_ != 0 && errorCount >= errorLimit_)
    message(ParserAppMessages::errorLimitExceeded,
	    NumberMessageArg(errorLimit_));
  return errorCount > 0;
}

void ParserApp::parseAll(SgmlParser &parser,
			 EventHandler &eh,
			 SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr)
{
  if (arcNames_.size() > 0) {
    SelectOneArcDirector director(arcNames_, eh);
    ArcEngine::parseAll(parser, director, director, cancelPtr);
  }
  else
    parser.parseAll(eh, cancelPtr);
}

void ParserApp::processOption(AppChar opt, const AppChar *arg)
{
  switch (opt) {
  case 'a':
    // activate link
    activeLinkTypes_.push_back(arg);
    break;
  case 'A':
    arcNames_.push_back(convertInput(arg));
    break;
  case 'E':
    {
      AppChar *end;
      unsigned long n = tcstoul((AppChar *)arg, &end, 10);
      if ((n == 0 && end == arg)
	  || *end != SP_T('\0')
	  || (n == ULONG_MAX && errno == ERANGE)
	  || n > UINT_MAX)
	message(ParserAppMessages::badErrorLimit);
      else
	errorLimit_ = unsigned(n);
    }
    break;
  case 'e':
    // describe open entities in error messages
    addOption(MessageReporter::openEntities);
    break;
  case 'g':
    // show gis of open elements in error messages
    addOption(MessageReporter::openElements);
    break;
  case 'i':
    // pretend that arg is defined as INCLUDE
    options_.includes.push_back(convertInput(arg));
    break;
  case 'w':
    if (!enableWarning(arg))
      message(ParserAppMessages::unknownWarning,
	      StringMessageArg(convertInput(arg)));
    break;
  default:
    EntityApp::processOption(opt, arg);
    break;
  }
}

Boolean ParserApp::enableWarning(const AppChar *s)
{
  struct {
    const AppChar *name;
    PackedBoolean ParserOptions::*ptr;
    PackedBoolean partOfAll;
  } table[] = {
    { SP_T("mixed"), &ParserOptions::warnMixedContent, 1 },
    { SP_T("should"), &ParserOptions::warnShould, 1 },
    { SP_T("duplicate"), &ParserOptions::warnDuplicateEntity, 0 },
    { SP_T("default"), &ParserOptions::warnDefaultEntityReference, 1 },
    { SP_T("undefined"), &ParserOptions::warnUndefinedElement, 1 },
    { SP_T("sgmldecl"), &ParserOptions::warnSgmlDecl, 1 },
    { SP_T("unclosed"), &ParserOptions::warnUnclosedTag, 1 },
    { SP_T("empty"), &ParserOptions::warnEmptyTag, 1 },
    { SP_T("net"), &ParserOptions::warnNet, 0 },
    { SP_T("unused-map"), &ParserOptions::warnUnusedMap, 1 },
    { SP_T("unused-param"), &ParserOptions::warnUnusedParam, 1 },
    { SP_T("notation-sysid"), &ParserOptions::warnNotationSystemId, 0 },
    { SP_T("idref"), &ParserOptions::errorIdref, 0 },
    { SP_T("significant"), &ParserOptions::errorSignificant, 0 },
    { SP_T("afdr"), &ParserOptions::errorAfdr, 0 },
    { SP_T("lpd-notation"), &ParserOptions::errorLpdNotation, 0 },
  };
  PackedBoolean val = 1;
  if (tcsncmp(s, SP_T("no-"), 3) == 0) {
    s += 3;
    val = 0;
  }
  if (tcscmp(s, SP_T("all")) == 0) {
    for (size_t i = 0; i < SIZEOF(table); i++)
      if (table[i].partOfAll) {
	// Use parentheses to work around Watcom 10.0a bug.
	(options_.*(table[i].ptr)) = val;
      }
    return 1;
  }
  for (size_t i = 0; i < SIZEOF(table); i++)
    if (tcscmp(s, table[i].name) == 0) {
      // Use parentheses to work around Watcom 10.0a bug.
      (options_.*(table[i].ptr)) = val;
      return 1;
    }
  if (tcscmp(s, SP_T("min-tag")) == 0) {
    options_.warnUnclosedTag = val;
    options_.warnEmptyTag = val;
    options_.warnNet = val;
    return 1;
  }
  return 0;
}

#ifdef SP_NAMESPACE
}
#endif
