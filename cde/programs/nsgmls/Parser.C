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
/* $XConsortium: Parser.C /main/1 1996/07/29 17:00:24 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "Parser.h"
#include "ParserMessages.h"
#include "constant.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Parser::Parser(const SgmlParser::Params &params)
: ParserState(params.parent
	      ? params.parent->parser_->entityManagerPtr()
	      : params.entityManager,
	      params.options
	      ? *params.options
	      : params.parent->parser_->options(),
	      paramsSubdocLevel(params),
	      params.entityType == SgmlParser::Params::dtd
	      ? declSubsetPhase
	      : contentPhase)
{
  Parser *parent = 0;
  if (params.parent)
    parent = params.parent->parser_;
  if (params.entityType == SgmlParser::Params::document) {
    Sd *sd = new Sd();
    const ParserOptions &opt = *params.options;
    sd->setDocCharsetDesc(*params.initialCharset);
    sd->setBooleanFeature(Sd::fDATATAG, opt.datatag);
    sd->setBooleanFeature(Sd::fOMITTAG, opt.omittag);
    sd->setBooleanFeature(Sd::fRANK, opt.rank);
    sd->setBooleanFeature(Sd::fSHORTTAG, opt.shorttag);
    sd->setNumberFeature(Sd::fSIMPLE, opt.linkSimple);
    sd->setBooleanFeature(Sd::fIMPLICIT, opt.linkImplicit);
    sd->setNumberFeature(Sd::fEXPLICIT, opt.linkExplicit);
    sd->setNumberFeature(Sd::fCONCUR, opt.concur);
    sd->setNumberFeature(Sd::fSUBDOC, opt.subdoc);
    sd->setBooleanFeature(Sd::fFORMAL, opt.formal);
    PublicId publicId;
    CharsetDecl docCharsetDecl;
    docCharsetDecl.addSection(publicId);
    docCharsetDecl.addRange(0, charMax > 99999999 ? 99999999 : charMax + 1, 0);
    sd->setDocCharsetDecl(docCharsetDecl);
    setSd(sd);
  }
  else if (params.sd.isNull()) {
    setSd(parent->sdPointer());
    setSyntaxes(parent->prologSyntaxPointer(),
		parent->instanceSyntaxPointer());
  }
  else {
    setSd(params.sd);
    setSyntaxes(params.prologSyntax, params.instanceSyntax);
  }

  // Make catalog
  StringC sysid(params.sysid);
  ConstPtr<EntityCatalog> catalog
    = entityManager().makeCatalog(sysid,
				  sd().docCharset(),
				  messenger());
  if (!catalog.isNull())
    setEntityCatalog(catalog);
  else if (parent)
    setEntityCatalog(parent->entityCatalogPtr());
  else {
    allDone();
    return;
  }
  
  // Set up the input stack.
  if (sysid.size() == 0) {
    allDone();
    return;
  }
  Ptr<InputSourceOrigin> origin;
  if (params.origin.isNull())
    origin = new InputSourceOrigin;
  else
    origin = params.origin;
  pushInput(entityManager().open(sysid,
				 sd().docCharset(),
				 origin.pointer(),
				 1,
				 messenger()));
  if (inputLevel() == 0) {
    allDone();
    return;
  }
  switch (params.entityType) {
  case SgmlParser::Params::document:
    setPhase(initPhase);
    break;
  case SgmlParser::Params::subdoc:
    if (params.subdocInheritActiveLinkTypes && parent)
      inheritActiveLinkTypes(*parent);
    if (subdocLevel() == sd().subdoc() + 1)
      message(ParserMessages::subdocLevel, NumberMessageArg(sd().subdoc()));
    setPhase(prologPhase);
    compilePrologModes();
    break;
  case SgmlParser::Params::dtd:
    compilePrologModes();
    startDtd(params.doctypeName);
    setPhase(declSubsetPhase);
    break;
  }
}

void Parser::giveUp()
{
  if (subdocLevel() > 0)	// FIXME might be subdoc if level == 0
    message(ParserMessages::subdocGiveUp);
  else
    message(ParserMessages::giveUp);
  allDone();
}

unsigned Parser::paramsSubdocLevel(const SgmlParser::Params &params)
{
  if (!params.parent)
    return 0;
  unsigned n = params.parent->parser_->subdocLevel();
  if (params.subdocReferenced)
    return n + 1;
  else
    return n;
}

Event *Parser::nextEvent()
{
  while (eventQueueEmpty()) {
    switch (phase()) {
    case noPhase:
      return 0;
    case initPhase:
      doInit();
      break;
    case prologPhase:
      doProlog();
      break;
    case declSubsetPhase:
      doDeclSubset();
      break;
    case instanceStartPhase:
      doInstanceStart();
      break;
    case contentPhase:
      doContent();
      break;
    }
  }
  return eventQueueGet();
}

void Parser::parseAll(EventHandler &handler,
		      SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr)
{
  while (!eventQueueEmpty())
    eventQueueGet()->handle(handler);
  // FIXME catch exceptions and reset handler.
  setHandler(&handler, cancelPtr);
  for (;;) {
    switch (phase()) {
    case noPhase:
      unsetHandler();
      return;
    case initPhase:
      doInit();
      break;
    case prologPhase:
      doProlog();
      break;
    case declSubsetPhase:
      doDeclSubset();
      break;
    case instanceStartPhase:
      doInstanceStart();
      break;
    case contentPhase:
      doContent();
      break;
    }
  }
}

#ifdef SP_NAMESPACE
}
#endif
