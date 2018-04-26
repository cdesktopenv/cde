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
/* $XConsortium: SgmlParser.C /main/1 1996/07/29 17:04:11 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "SgmlParser.h"
#include "Parser.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

SgmlParser::SgmlParser()
: parser_(0)
{
}

SgmlParser::SgmlParser(const Params &params)
: parser_(new Parser(params))
{
}

void SgmlParser::init(const Params &params)
{
  delete parser_;
  parser_ = new Parser(params);
}

SgmlParser::~SgmlParser()
{
  delete parser_;
}

Event *SgmlParser::nextEvent()
{
  return parser_->nextEvent();
}

void SgmlParser::parseAll(EventHandler &handler,
			  SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr)
{
  parser_->parseAll(handler, cancelPtr);
}

ConstPtr<Sd> SgmlParser::sd() const
{
  return parser_->sdPointer();
}

ConstPtr<Syntax> SgmlParser::instanceSyntax() const
{
  return parser_->instanceSyntaxPointer();
}

ConstPtr<Syntax> SgmlParser::prologSyntax() const
{
  return parser_->prologSyntaxPointer();
}

const EntityManager &SgmlParser::entityManager() const
{
  return parser_->entityManager();
}

const EntityCatalog &SgmlParser::entityCatalog() const
{
  return parser_->entityCatalog();
}

void SgmlParser::activateLinkType(const StringC &name)
{
  parser_->activateLinkType(name);
}

void SgmlParser::allLinkTypesActivated()
{
  parser_->allLinkTypesActivated();
}

void SgmlParser::swap(SgmlParser &s)
{
  Parser *tem = parser_;
  parser_ = s.parser_;
  s.parser_ = tem;
}

Ptr<Dtd> SgmlParser::baseDtd()
{
  return parser_->baseDtd();
}

const ParserOptions &SgmlParser::options() const
{
  return parser_->options();
}

SgmlParser::Params::Params()
: entityType(document),
  initialCharset(0),
  parent(0),
  options(0),
  subdocInheritActiveLinkTypes(0),
  subdocReferenced(0),
  subdocLevel(0)
{
}

#ifdef SP_NAMESPACE
}
#endif
