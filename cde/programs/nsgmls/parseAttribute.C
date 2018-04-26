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
/* $XConsortium: parseAttribute.C /main/1 1996/07/29 17:09:07 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "Parser.h"
#include "MessageArg.h"
#include "token.h"
#include "macros.h"
#include "ParserMessages.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Boolean Parser::parseAttributeSpec(Boolean inDecl,
				   AttributeList &atts,
				   Boolean &netEnabling)

{
  unsigned specLength = 0;
  AttributeParameter::Type curParm;

  if (!parseAttributeParameter(inDecl, 0, curParm, netEnabling))
    return 0;
  while (curParm != AttributeParameter::end) {
    switch (curParm) {
    case AttributeParameter::name:
      {
	Text text;
	text.addChars(currentInput()->currentTokenStart(),
		      currentInput()->currentTokenLength(),
		      currentLocation());
	size_t nameMarkupIndex;
	if (currentMarkup())
	  nameMarkupIndex = currentMarkup()->size() - 1;
	text.subst(*syntax().generalSubstTable(), syntax().space());
	if (!parseAttributeParameter(inDecl, 1, curParm, netEnabling))
	  return 0;
	if (curParm == AttributeParameter::vi) {
	  specLength += text.size() + syntax().normsep();
	  if (!parseAttributeValueSpec(inDecl, text.string(), atts,	
				       specLength))
	    return 0;
	  // setup for next attribute
	  if (!parseAttributeParameter(inDecl, 0, curParm, netEnabling))
	    return 0;
	}
	else {
	  if (currentMarkup())
	    currentMarkup()->changeToAttributeValue(nameMarkupIndex);
	  if (!handleAttributeNameToken(text, atts, specLength))
	    return 0;
	}
      }
      break;
    case AttributeParameter::nameToken:
      {
	Text text;
	text.addChars(currentInput()->currentTokenStart(),
		      currentInput()->currentTokenLength(),
		      currentLocation());
	text.subst(*syntax().generalSubstTable(), syntax().space());
	if (!handleAttributeNameToken(text, atts, specLength))
	  return 0;
	if (!parseAttributeParameter(inDecl, 0, curParm, netEnabling))
	  return 0;
      }
      break;
    case AttributeParameter::recoverUnquoted:
      {
	if (!atts.recoverUnquoted(currentToken(), currentLocation(), *this)) {
	  // Don't treat it as an unquoted attribute value.
	  currentInput()->endToken(1);
	  if (!atts.handleAsUnterminated(*this))
	    message(ParserMessages::attributeSpecCharacter,
		    StringMessageArg(currentToken()));
	  return 0;
	}
	if (!parseAttributeParameter(inDecl, 0, curParm, netEnabling))
	  return 0;
      }
      break;
    default:
      CANNOT_HAPPEN();
    }
  }
  atts.finish(*this);
  if (specLength > syntax().attsplen())
    message(ParserMessages::attsplen,
	    NumberMessageArg(syntax().attsplen()),
	    NumberMessageArg(specLength));
  return 1;
}

Boolean Parser::handleAttributeNameToken(Text &text,
					 AttributeList &atts,
					 unsigned &specLength)
{
  unsigned index;
  if (!atts.tokenIndex(text.string(), index)) {
    if (atts.handleAsUnterminated(*this))
      return 0;
    atts.noteInvalidSpec();
    message(ParserMessages::noSuchAttributeToken,
	    StringMessageArg(text.string()));
  }
  else {
    if (!sd().shorttag())
      message(ParserMessages::attributeNameShorttag);
    atts.setSpec(index, *this);
    atts.setValueToken(index, text, *this, specLength);
  }
  return 1;
}

Boolean Parser::parseAttributeValueSpec(Boolean inDecl,
					const StringC &name,
					AttributeList &atts,
					unsigned &specLength)
{
  Mode mode = inDecl ? asMode : tagMode;
  Markup *markup = currentMarkup();
  Token token = getToken(mode);
  if (token == tokenS) {
    if (markup) {
      do {
	markup->addS(currentChar());
	token = getToken(mode);
      } while (token == tokenS);
    }
    else {
      do {
	token = getToken(mode);
      } while (token == tokenS);
    }
  }
  unsigned index;
  Boolean valid = atts.attributeIndex(name, index);
  if (!valid) {
    message(ParserMessages::noSuchAttribute, StringMessageArg(name));
    atts.noteInvalidSpec();
  }
  else
    atts.setSpec(index, *this);
  Text text;
  switch (token) {
  case tokenUnrecognized:
    if (reportNonSgmlCharacter())
      return 0;
    // fall through
  case tokenEtago:
  case tokenStago:
  case tokenNet:
    message(ParserMessages::unquotedAttributeValue);
    extendUnquotedAttributeValue();
    if (markup)
      markup->addAttributeValue(currentInput());
    text.addChars(currentInput()->currentTokenStart(),
		  currentInput()->currentTokenLength(),
		  currentLocation());
    break;
  case tokenEe:
    message(ParserMessages::attributeSpecEntityEnd);
    return 0;
  case tokenTagc:
  case tokenDsc:
  case tokenVi:
    message(ParserMessages::attributeValueExpected);
    return 0;
  case tokenNameStart:
  case tokenDigit:
  case tokenLcUcNmchar:
    if (!sd().shorttag())
      message(ParserMessages::attributeValueShorttag);
    extendNameToken(syntax().litlen() >= syntax().normsep()
		    ? syntax().litlen() - syntax().normsep()
		    : 0,
		    ParserMessages::attributeValueLength);
    if (markup)
      markup->addAttributeValue(currentInput());
    text.addChars(currentInput()->currentTokenStart(),
		  currentInput()->currentTokenLength(),
		  currentLocation());
    break;
  case tokenLit:
  case tokenLita:
    Boolean lita;
    lita = (token == tokenLita);
    if (!((valid ? atts.tokenized(index) : 1)
	  ? parseTokenizedAttributeValueLiteral(lita, text)
	  : parseAttributeValueLiteral(lita, text)))
      return 0;
    if (markup)
      markup->addLiteral(text);
    break;
  default:
      CANNOT_HAPPEN();
  }
  if (valid)
    return atts.setValue(index, text, *this, specLength);
  else
    return !AttributeValue::handleAsUnterminated(text, *this);
}


Boolean Parser::parseAttributeParameter(Boolean inDecl,
					Boolean allowVi,
					AttributeParameter::Type &result,
					Boolean &netEnabling)
{
  Mode mode = inDecl ? asMode : tagMode;
  Token token = getToken(mode);
  Markup *markup = currentMarkup();
  if (markup) {
    while (token == tokenS) {
      markup->addS(currentChar());
      token = getToken(mode);
    }
  }
  else {
    while (token == tokenS)
      token = getToken(mode);
  }
  switch (token) {
  case tokenUnrecognized:
    if (reportNonSgmlCharacter())
      return 0;
    extendUnquotedAttributeValue();
    result = AttributeParameter::recoverUnquoted;
    break;
  case tokenEe:
    message(ParserMessages::attributeSpecEntityEnd);
    return 0;
  case tokenEtago:
  case tokenStago:
    if (!sd().shorttag())
      message(ParserMessages::minimizedStartTag);
    else if (options().warnUnclosedTag)
      message(ParserMessages::unclosedStartTag);
    result = AttributeParameter::end;
    currentInput()->ungetToken();
    netEnabling = 0;
    break;
  case tokenNet:
    if (markup)
      markup->addDelim(Syntax::dNET);
    if (!sd().shorttag())
      message(ParserMessages::minimizedStartTag);
    else if (options().warnNet)
      message(ParserMessages::netStartTag);
    netEnabling = 1;
    result = AttributeParameter::end;
    break;
  case tokenTagc:
    if (markup)
      markup->addDelim(Syntax::dTAGC);
    netEnabling = 0;
    result = AttributeParameter::end;
    break;
  case tokenDsc:
    if (markup)
      markup->addDelim(Syntax::dDSC);
    result = AttributeParameter::end;
    break;
  case tokenNameStart:
    extendNameToken(syntax().namelen(), ParserMessages::nameTokenLength);
    if (markup)
      markup->addName(currentInput());
    result = AttributeParameter::name;
    break;
  case tokenDigit:
  case tokenLcUcNmchar:
    extendNameToken(syntax().namelen(), ParserMessages::nameTokenLength);
    if (markup)
      markup->addName(currentInput());
    result = AttributeParameter::nameToken;
    break;
  case tokenLit:
  case tokenLita:
    message(allowVi
	    ? ParserMessages::attributeSpecLiteral
	    : ParserMessages::attributeSpecNameTokenExpected);
    return 0;
  case tokenVi:
    if (!allowVi) {
      message(ParserMessages::attributeSpecNameTokenExpected);
      return 0;
    }
    if (markup)
      markup->addDelim(Syntax::dVI);
    result = AttributeParameter::vi;
    break;
  default:
    CANNOT_HAPPEN();
  }
  return 1;
}

void Parser::extendUnquotedAttributeValue()
{
  InputSource *in = currentInput();
  size_t length = in->currentTokenLength();
  const Syntax &syn = syntax();
  for (;;) {
    Xchar c = in->tokenChar(messenger());
    if (syn.isS(c)
	|| !syn.isSgmlChar(c)
	|| c == InputSource::eE
	|| c == syn.delimGeneral(Syntax::dTAGC)[0])
      break;
    length++;
  }
  in->endToken(length);
}

Boolean Parser::parseAttributeValueLiteral(Boolean lita, Text &text)
{
  size_t maxLength = (syntax().litlen() > syntax().normsep()
		      ? syntax().litlen() - syntax().normsep()
		      : 0);
  if (parseLiteral(lita ? alitaMode : alitMode, aliteMode,
		   maxLength,
		   ParserMessages::attributeValueLength,
 		   (wantMarkup() ? unsigned(literalDelimInfo) : 0),
		   text)) {
    if (text.size() == 0
	&& syntax().normsep() > syntax().litlen())
      message(ParserMessages::attributeValueLengthNeg,
	      NumberMessageArg(syntax().normsep() - syntax().litlen()));
    return 1;
  }
  else
    return 0;
}

Boolean Parser::parseTokenizedAttributeValueLiteral(Boolean lita, Text &text)
{
  size_t maxLength = (syntax().litlen() > syntax().normsep()
		      ? syntax().litlen() - syntax().normsep()
		      : 0);
  if (parseLiteral(lita ? talitaMode : talitMode, taliteMode,
		   maxLength,
		   ParserMessages::tokenizedAttributeValueLength,
		   literalSingleSpace
		   | (wantMarkup() ? unsigned(literalDelimInfo) : 0),
		   text)) {
    if (text.size() == 0
	&& syntax().normsep() > syntax().litlen())
      message(ParserMessages::tokenizedAttributeValueLengthNeg,
	      NumberMessageArg(syntax().normsep() - syntax().litlen()));
    return 1;
  }
  else
    return 0;
}


Boolean Parser::skipAttributeSpec()
{
  AttributeParameter::Type parm;
  Boolean netEnabling;
  if (!parseAttributeParameter(0, 0, parm, netEnabling))
    return 0;
  while (parm != AttributeParameter::end) {
    if (parm == AttributeParameter::name) {
      size_t nameMarkupIndex = 0;
      if (currentMarkup())
	nameMarkupIndex = currentMarkup()->size() - 1;
      if (!parseAttributeParameter(0, 1, parm, netEnabling))
	return 0;
      if (parm == AttributeParameter::vi) {
	Token token = getToken(tagMode);
	while (token == tokenS) {
	  if (currentMarkup())
	    currentMarkup()->addS(currentChar());
	  token = getToken(tagMode);
	}
	switch (token) {
	case tokenUnrecognized:
	  if (!reportNonSgmlCharacter())
	    message(ParserMessages::attributeSpecCharacter,
		    StringMessageArg(currentToken()));
	  return 0;
	case tokenEe:
	  message(ParserMessages::attributeSpecEntityEnd);
	  return 0;
	case tokenEtago:
	case tokenStago:
	case tokenNet:
	case tokenTagc:
	case tokenDsc:
	case tokenVi:
	  message(ParserMessages::attributeValueExpected);
	  return 0;
	case tokenNameStart:
	case tokenDigit:
	case tokenLcUcNmchar:
	  if (!sd().shorttag())
	    message(ParserMessages::attributeValueShorttag);
	  extendNameToken(syntax().litlen() >= syntax().normsep()
			  ? syntax().litlen() - syntax().normsep()
			  : 0,
			  ParserMessages::attributeValueLength);
	  if (currentMarkup())
	    currentMarkup()->addAttributeValue(currentInput());
	  break;
	case tokenLit:
	case tokenLita:
	  {
	    Text text;
	    if (!parseLiteral(token == tokenLita ? talitaMode : talitMode,
			      taliteMode,
			      syntax().litlen(),
			      ParserMessages::tokenizedAttributeValueLength,
			      (currentMarkup() ? literalDelimInfo : 0)
			      | literalNoProcess,
			      text))
	      return 0;
	    if (currentMarkup())
	      currentMarkup()->addLiteral(text);
	  }
	  break;
	default:
	  CANNOT_HAPPEN();
	}
	if (!parseAttributeParameter(0, 0, parm, netEnabling))
	  return 0;
      }
      else {
	if (currentMarkup())
	  currentMarkup()->changeToAttributeValue(nameMarkupIndex);
	if (!sd().shorttag())
	  message(ParserMessages::attributeNameShorttag);
      }
    }
    else {
      // It's a name token.
      if (!parseAttributeParameter(0, 0, parm, netEnabling))
	return 0;
      if (!sd().shorttag())
	message(ParserMessages::attributeNameShorttag);
    }
  }
  if (netEnabling)
    message(ParserMessages::startTagGroupNet);
  return 1;
}

#ifdef SP_NAMESPACE
}
#endif
