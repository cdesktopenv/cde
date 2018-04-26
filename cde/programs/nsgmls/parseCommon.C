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
/* $XConsortium: parseCommon.C /main/1 1996/07/29 17:09:12 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "Parser.h"
#include "token.h"
#include "MessageArg.h"
#include "ParserMessages.h"
#include "constant.h"
#include "NumericCharRefOrigin.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Boolean Parser::parseProcessingInstruction()
{
  currentInput()->startToken();
  Location location(currentLocation());
  StringC buf;
  for (;;) {
    Token token = getToken(piMode);
    if (token == tokenPic)
      break;
    switch (token) {
    case tokenEe:
      message(ParserMessages::processingInstructionEntityEnd);
      return 0;
    case tokenUnrecognized:
      reportNonSgmlCharacter();
      // fall through
    case tokenChar:
      buf += *currentInput()->currentTokenStart();
      if (buf.size()/2 > syntax().pilen()) {
	message(ParserMessages::processingInstructionLength,
		NumberMessageArg(syntax().pilen()));
	message(ParserMessages::processingInstructionClose);
	return 0;
      }
      break;
    }
  }
  if (buf.size() > syntax().pilen())
    message(ParserMessages::processingInstructionLength,
	    NumberMessageArg(syntax().pilen()));
  noteMarkup();
  eventHandler().pi(new (eventAllocator()) ImmediatePiEvent(buf, location));
  return 1;
}

Boolean Parser::parseLiteral(Mode litMode,
			     Mode liteMode,
			     size_t maxLength,
			     const MessageType1 &tooLongMessage,
			     unsigned flags,
			     Text &text)
{
  unsigned startLevel = inputLevel();
  Mode currentMode = litMode;
  // If the literal gets to be longer than this, then we assume
  // that the closing delimiter has been omitted if we're at the end
  // of a line and at the starting input level.
  size_t reallyMaxLength = (maxLength > size_t(-1)/2
			    ? size_t(-1)
			    : maxLength * 2);
  text.clear();
  Location startLoc(currentLocation());
  if (flags & literalDelimInfo)
    text.addStartDelim(currentLocation());
  for (;;) {
    int done = 0;
    Token token = getToken(currentMode);
    switch (token) {
    case tokenEe:
      if (inputLevel() == startLevel) {
	message(ParserMessages::literalLevel);
	return 0;
      }
      text.addEntityEnd(currentLocation());
      popInputStack();
      if (inputLevel() == startLevel)
	currentMode = litMode;
      break;
    case tokenUnrecognized:
      if (reportNonSgmlCharacter())
	break;
      message(ParserMessages::literalMinimumData,
	      StringMessageArg(currentToken()));
      break;
    case tokenRs:
      text.ignoreChar(currentChar(), currentLocation());
      break;
    case tokenRe:
      if (text.size() > reallyMaxLength && inputLevel() == startLevel) {
#if 0
	message(tooLongMessage, NumberMessageArg(maxLength));
#endif
	// guess that the closing delimiter has been omitted
	Messenger::setNextLocation(startLoc);
	message(ParserMessages::literalClosingDelimiter);
	return 0;
      }
      // fall through
    case tokenSepchar:
      if ((flags & literalSingleSpace)
	  && (text.size() == 0 || text.lastChar() == syntax().space()))
	text.ignoreChar(currentChar(), currentLocation());
      else
	text.addChar(syntax().space(),
		     Location(new ReplacementOrigin(currentLocation(),
						    currentChar()),
			      0));
      break;
    case tokenSpace:
      if ((flags & literalSingleSpace)
	  && (text.size() == 0 || text.lastChar() == syntax().space()))
	text.ignoreChar(currentChar(), currentLocation());
      else
	text.addChar(currentChar(), currentLocation());
      break;
    case tokenCroDigit:
      {
	Char c;
	Location loc;
	if (!parseNumericCharRef(c, loc))
	  return 0;
	if (flags & literalDataTag) {
	  if (!syntax().isSgmlChar(c))
	    message(ParserMessages::dataTagPatternNonSgml);
	  else if (syntax().charSet(Syntax::functionChar)->contains(c))
	    message(ParserMessages::dataTagPatternFunction);
	}
	if ((flags & literalSingleSpace)
	    && c == syntax().space()
	    && (text.size() == 0 || text.lastChar() == syntax().space()))
	  text.ignoreChar(c, loc);
	else
	  text.addChar(c, loc);
      }
      break;
    case tokenCroNameStart:
      if (!parseNamedCharRef())
	return 0;
      break;
    case tokenEroGrpo:
      message(inInstance() ? ParserMessages::eroGrpoStartTag : ParserMessages::eroGrpoProlog);
      break;
    case tokenLit:
    case tokenLita:
      if (flags & literalDelimInfo)
	text.addEndDelim(currentLocation(), token == tokenLita);
      done = 1;
      break;
    case tokenEroNameStart:
    case tokenPeroNameStart:
      {
	ConstPtr<Entity> entity;
	Ptr<EntityOrigin> origin;
	if (!parseEntityReference(token == tokenPeroNameStart,
				  (flags & literalNoProcess) ? 2 : 0,
				  entity, origin))
	  return 0;
	if (!entity.isNull())
	  entity->litReference(text, *this, origin,
			       (flags & literalSingleSpace) != 0);
	if (inputLevel() > startLevel)
	  currentMode = liteMode;
      }
      break;
    case tokenPeroGrpo:
      message(ParserMessages::peroGrpoProlog);
      break;
    case tokenChar:
      if (text.size() > reallyMaxLength && inputLevel() == startLevel
	  && currentChar() == syntax().standardFunction(Syntax::fRE)) {
#if 0
	message(tooLongMessage, NumberMessageArg(maxLength));
#endif
	// guess that the closing delimiter has been omitted
	Messenger::setNextLocation(startLoc);
	message(ParserMessages::literalClosingDelimiter);
	return 0;
      }
      text.addChar(currentChar(), currentLocation());
      break;
    }
    if (done) break;
  }
  if ((flags & literalSingleSpace)
      && text.size() > 0
      && text.lastChar() == syntax().space())
    text.ignoreLastChar();
  if (text.size() > maxLength) {
    switch (litMode) {
    case alitMode:
    case alitaMode:
    case talitMode:
    case talitaMode:
      if (AttributeValue::handleAsUnterminated(text, *this))
	return 0;
    default:
      break;
    }
    message(tooLongMessage, NumberMessageArg(maxLength));
  }
  return 1;
}

Boolean Parser::parseNamedCharRef()
{
  InputSource *in = currentInput();
  Index startIndex = currentLocation().index();
  in->discardInitial();
  extendNameToken(syntax().namelen(), ParserMessages::nameLength);
  Char c;
  Boolean valid;
  StringC name;
  getCurrentToken(syntax().generalSubstTable(), name);
  if (!syntax().lookupFunctionChar(name, &c)) {
    message(ParserMessages::functionName, StringMessageArg(name));
    valid = 0;
  }
  else {
    valid = 1;
    if (wantMarkup())
      getCurrentToken(name);	// the original name
  }
  NamedCharRef::RefEndType refEndType;
  switch (getToken(refMode)) {
  case tokenRefc:
    refEndType = NamedCharRef::endRefc;
    break;
  case tokenRe:
    refEndType = NamedCharRef::endRE;
    break;
  default:
    refEndType = NamedCharRef::endOmitted;
    break;
  }
  in->startToken();
  if (valid)
    in->pushCharRef(c, NamedCharRef(startIndex, refEndType, name));
  return 1;
}

Boolean Parser::parseNumericCharRef(Char &ch, Location &loc)
{
  InputSource *in = currentInput();
  Location startLocation = currentLocation();
  in->discardInitial();
  extendNumber(syntax().namelen(), ParserMessages::numberLength);
  Boolean valid = 1;
  Char c = 0;
  const Char *lim = in->currentTokenEnd();
  for (const Char *p = in->currentTokenStart(); p < lim; p++) {
    int val = sd().digitWeight(*p);
    if (c <= charMax/10 && (c *= 10) <= charMax - val)
      c += val;
    else {
      message(ParserMessages::characterNumber, StringMessageArg(currentToken()));
      valid = 0;
      break;
    }
  }
  if (valid && !sd().docCharsetDecl().charDeclared(c)) {
    valid = 0;
    message(ParserMessages::characterNumber, StringMessageArg(currentToken()));
  }
  Owner<Markup> markupPtr;
  if (wantMarkup()) {
    markupPtr = new Markup;
    markupPtr->addDelim(Syntax::dCRO);
    markupPtr->addNumber(in);
    switch (getToken(refMode)) {
    case tokenRefc:
      markupPtr->addDelim(Syntax::dREFC);
      break;
    case tokenRe:
      markupPtr->addRefEndRe();
      break;
    default:
      break;
    }
  }
  else
    (void)getToken(refMode);
  if (valid) {
    ch = c;
    loc = Location(new NumericCharRefOrigin(startLocation,
					    currentLocation().index()
					    + currentInput()->currentTokenLength()
					    - startLocation.index(),
					    markupPtr),
		   0);
  }
  return valid;
}

// ignoreLevel: 0 means don't ignore;
// 1 means parse name group and ignore if inactive
// 2 means ignore

Boolean Parser::parseEntityReference(Boolean isParameter,
				     int ignoreLevel,
				     ConstPtr<Entity> &entity,
				     Ptr<EntityOrigin> &origin)
{
  InputSource *in = currentInput();
  Location startLocation(in->currentLocation());
  Owner<Markup> markupPtr;
  if (wantMarkup()) {
    markupPtr = new Markup;
    markupPtr->addDelim(isParameter ? Syntax::dPERO : Syntax::dERO);
  }
  if (ignoreLevel == 1) {
    Markup savedMarkup;
    Markup *savedCurrentMarkup = currentMarkup();
    if (savedCurrentMarkup)
      savedCurrentMarkup->swap(savedMarkup);
    Location savedMarkupLocation(markupLocation());
    startMarkup(markupPtr != 0, startLocation);
    if (markupPtr) {
      markupPtr->addDelim(Syntax::dGRPO);
      markupPtr->swap(*currentMarkup());
    }
    Boolean ignore;
    if (!parseEntityReferenceNameGroup(ignore))
      return 0;
    if (markupPtr)
      currentMarkup()->swap(*markupPtr);
    startMarkup(savedCurrentMarkup != 0, savedMarkupLocation);
    if (savedCurrentMarkup)
      savedMarkup.swap(*currentMarkup());
    if (!ignore)
      ignoreLevel = 0;
    in->startToken();
    Xchar c = in->tokenChar(messenger());
    if (!syntax().isNameStartCharacter(c)) {
      message(ParserMessages::entityReferenceMissingName);
      return 0;
    }
  }
  in->discardInitial();
  if (isParameter)
    extendNameToken(syntax().penamelen(), ParserMessages::parameterEntityNameLength);
  else
    extendNameToken(syntax().namelen(), ParserMessages::nameLength);
  StringC &name = nameBuffer();
  getCurrentToken(syntax().entitySubstTable(), name);
  if (ignoreLevel)
    entity = new IgnoredEntity(name,
			       isParameter
			       ? Entity::parameterEntity
			       : Entity::generalEntity);
  else {
    entity = lookupEntity(isParameter, name, startLocation, 1);
    if (entity.isNull()) {
      if (haveApplicableDtd())
	message(isParameter
		? ParserMessages::parameterEntityUndefined
		: ParserMessages::entityUndefined,
		StringMessageArg(name));
      else
	message(ParserMessages::entityApplicableDtd);
    }
    else if (entity->defaulted() && options().warnDefaultEntityReference)
      message(ParserMessages::defaultEntityReference, StringMessageArg(name));
  }
  if (markupPtr) {
    markupPtr->addName(in);
    switch (getToken(refMode)) {
    case tokenRefc:
      markupPtr->addDelim(Syntax::dREFC);
      break;
    case tokenRe:
      markupPtr->addRefEndRe();
      break;
    default:
      break;
    }
  }
  else
    (void)getToken(refMode);
  if (!entity.isNull())
    origin = new (internalAllocator())
               EntityOrigin(entity, startLocation,
			    currentLocation().index()
			    + currentInput()->currentTokenLength()
			    - startLocation.index(),
			    markupPtr);
  else
    origin = (EntityOrigin *)0;
  return 1;
}

Boolean Parser::parseComment(Mode mode)
{
  Location startLoc(currentLocation());
  Markup *markup = currentMarkup();
  if (markup)
    markup->addCommentStart();
  Token token;
  while ((token = getToken(mode)) != tokenCom)
    switch (token) {
    case tokenUnrecognized:
      if (!reportNonSgmlCharacter())
	message(ParserMessages::sdCommentSignificant,
		StringMessageArg(currentToken()));
      break;
    case tokenEe:
      message(ParserMessages::commentEntityEnd, startLoc);
      return 0;
    default:
      if (markup)
	markup->addCommentChar(currentChar());
      break;
    }
  return 1;
}

void Parser::extendNameToken(size_t maxLength,
			     const MessageType1 &tooLongMessage)
{
  InputSource *in = currentInput();
  size_t length = in->currentTokenLength();
  const Syntax &syn = syntax();
  while (syn.isNameCharacter(in->tokenChar(messenger())))
    length++;
  if (length > maxLength)
    message(tooLongMessage, NumberMessageArg(maxLength));
  in->endToken(length);
}


void Parser::extendNumber(size_t maxLength, const MessageType1 &tooLongMessage)
{
  InputSource *in = currentInput();
  size_t length = in->currentTokenLength();
  while (syntax().isDigit(in->tokenChar(messenger())))
    length++;
  if (length > maxLength)
    message(tooLongMessage, NumberMessageArg(maxLength));
  in->endToken(length);
}

Boolean Parser::reportNonSgmlCharacter()
{
  Char c = getChar();
  if (!syntax().isSgmlChar(c)) {
    message(ParserMessages::nonSgmlCharacter, NumberMessageArg(c));
    return 1;
  }
  return 0;
}

void Parser::extendS()
{
  InputSource *in = currentInput();
  size_t length = in->currentTokenLength();
  while (syntax().isS(in->tokenChar(messenger())))
    length++;
  in->endToken(length);
}

#ifdef SP_NAMESPACE
}
#endif
