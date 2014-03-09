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
/* $XConsortium: Parser.h /main/1 1996/07/29 17:00:29 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Parser_INCLUDED
#define Parser_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "Attribute.h"
#include "Attributed.h"
#include "Boolean.h"
#include "StringC.h"
#include "ElementType.h"
#include "Entity.h"
#include "Event.h"
#include "IList.h"
#include "ISet.h"
#include "Location.h"
#include "Owner.h"
#include "ParserState.h"
#include "Ptr.h"
#include "SgmlParser.h"
#include "StringOf.h"
#include "Undo.h"
#include "Vector.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class AllowedParams;
class Param;
class ExternalId;
class PublicId;
class GroupToken;
class AllowedGroupTokens;
struct GroupConnector;
class AllowedGroupConnectors;
class AllowedSdParams;
class Text;
class AttributeList;
class AttributeDefinition;
class AttributeDefinitionList;
class UnivCharsetDesc;
class CharsetInfo;
class CharsetDecl;
class DeclaredValue;
struct SdBuilder;
struct SdParam;
class Syntax;
class ElementDefinition;
class CharSwitcher;
struct StandardSyntaxSpec;
class Undo;
class Decl;

class Parser : private ParserState { 
public:
  Parser(const SgmlParser::Params &);
  Event *nextEvent();
  void parseAll(EventHandler &, SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr);
  using ParserState::sdPointer;
  using ParserState::instanceSyntaxPointer;
  using ParserState::prologSyntaxPointer;
  using ParserState::activateLinkType;
  using ParserState::allLinkTypesActivated;
  using ParserState::entityManager;
  using ParserState::entityCatalog;
  using ParserState::baseDtd;
  using ParserState::options;
private:
  Parser(const Parser &);	// undefined
  void operator=(const Parser &); // undefined
  Boolean setStandardSyntax(Syntax &syn, const StandardSyntaxSpec &,
			    const CharsetInfo &docCharset,
			    CharSwitcher &);
  Boolean addRefDelimShortref(Syntax &syntax,
			      const CharsetInfo &syntaxCharset,
			      const CharsetInfo &docCharset,
			      CharSwitcher &switcher);
  Boolean setRefDelimGeneral(Syntax &syntax,
			     const CharsetInfo &syntaxCharset,
			     const CharsetInfo &docCharset,
			     CharSwitcher &switcher);
  void setRefNames(Syntax &syntax, const CharsetInfo &docCharset);

  void giveUp();
  void compileSdModes();
  void compilePrologModes();
  void compileInstanceModes();
  void addNeededShortrefs(Dtd &, const Syntax &);
  Boolean shortrefCanPreemptDelim(const StringC &sr,
				  const StringC &d,
				  Boolean dIsSr,
				  const Syntax &);
  void compileModes(const Mode *modes, int n, const Dtd *);
  void compileNormalMap();

  void doInit();
  void doProlog();
  void doDeclSubset();
  void doInstanceStart();
  void doContent();
  void extendNameToken(size_t, const MessageType1 &);
  void extendNumber(size_t, const MessageType1 &);
  void extendData();
  void extendS();
  void extendContentS();
  void declSubsetRecover(unsigned startLevel);
  void prologRecover();
  void skipDeclaration(unsigned startLevel);
  Boolean parseElementDecl();
  Boolean parseAttlistDecl();
  Boolean parseNotationDecl();
  Boolean parseEntityDecl();
  Boolean parseShortrefDecl();
  Boolean parseUsemapDecl();
  Boolean parseUselinkDecl();
  Boolean parseDoctypeDeclStart();
  Boolean parseDoctypeDeclEnd(Boolean fake = 0);
  Boolean parseMarkedSectionDeclStart();
  void handleMarkedSectionEnd();
  Boolean parseCommentDecl();
  void emptyCommentDecl();
  Boolean parseExternalId(const AllowedParams &,
			  const AllowedParams &,
			  unsigned,
			  Param &,
			  ExternalId &);
  Boolean parseParam(const AllowedParams &, unsigned, Param &);
  Boolean parseMinimumLiteral(Boolean, Text &);
  Boolean parseAttributeValueLiteral(Boolean, Text &);
  Boolean parseTokenizedAttributeValueLiteral(Boolean, Text &);
  Boolean parseSystemIdentifier(Boolean, Text &);
  Boolean parseParameterLiteral(Boolean, Text &);
  Boolean parseDataTagParameterLiteral(Boolean, Text &);
  // flags for parseLiteral()
  enum {
    literalSingleSpace = 01,
    literalDataTag = 02,
    literalMinimumData = 04,
    // Keep info about delimiters
    literalDelimInfo = 010,
    // Ignore references in the literal
    literalNoProcess = 020
    };
  Boolean parseLiteral(Mode litMode, Mode liteMode, size_t maxLength,
		       const MessageType1 &tooLongMessage,
		       unsigned flags, Text &text);

  Boolean parseGroupToken(const AllowedGroupTokens &allow,
			  unsigned nestingLevel,
			  unsigned declInputLevel,
			  unsigned groupInputLevel,
			  GroupToken &gt);
  Boolean parseGroupConnector(const AllowedGroupConnectors &allow,
			      unsigned declInputLevel,
			      unsigned groupInputLevel,
			      GroupConnector &gc);
  Boolean parseGroup(const AllowedGroupTokens &allowToken,
		     unsigned declInputLevel,
		     Param &parm);
  Boolean parseModelGroup(unsigned nestingLevel, unsigned declInputLevel,
			  ModelGroup *&, Mode);
  Boolean parseNameGroup(unsigned declInputLevel, Param &);
  Boolean parseNameTokenGroup(unsigned declInputLevel, Param &);
  Boolean parseDataTagGroup(unsigned nestingLevel, unsigned declInputLevel,
			    GroupToken &);
  Boolean parseDataTagTemplateGroup(unsigned nestingLevel,
				    unsigned declInputLevel, GroupToken &);

  Boolean parseElementNameGroup(unsigned declInputLevel, Param &);
  Boolean parseReservedName(const AllowedParams &allow, Param &parm);
  Boolean parseIndicatedReservedName(const AllowedParams &allow, Param &parm);
  Boolean getReservedName(Syntax::ReservedName *);
  Boolean getIndicatedReservedName(Syntax::ReservedName *);
  Boolean parseAttributeValueParam(Param &parm);
  Boolean parseEntityReference(Boolean isParameter,
			       int ignoreLevel,
			       ConstPtr<Entity> &entity,
			       Ptr<EntityOrigin> &origin);
  ContentToken::OccurrenceIndicator getOccurrenceIndicator(Mode);
  Boolean parseComment(Mode);
  Boolean parseNamedCharRef();
  Boolean parseNumericCharRef(Char &, Location &);
  Boolean parseDeclarationName(Syntax::ReservedName *, Boolean allowAfdr = 0);
  void paramInvalidToken(Token, const AllowedParams &);
  void groupTokenInvalidToken(Token, const AllowedGroupTokens &);
  void groupConnectorInvalidToken(Token, const AllowedGroupConnectors &);
  ElementType *lookupCreateElement(const StringC &);
  RankStem *lookupCreateRankStem(const StringC &);
  Boolean parseExceptions(unsigned declInputLevel,
			  Ptr<ElementDefinition> &def);
  void parsePcdata();
  void parseStartTag();
  const ElementType *completeRankStem(const StringC &);
  void handleRankedElement(const ElementType *);
  void parseEmptyStartTag();
  void acceptPcdata(const Location &);
  void acceptStartTag(const ElementType *, StartElementEvent *,
		      Boolean netEnabling);
  void handleBadStartTag(const ElementType *, StartElementEvent *,
			 Boolean netEnabling);
  void undo(IList<Undo> &);
  Boolean tryStartTag(const ElementType *, StartElementEvent *,
		      Boolean netEnabling, IList<Event> &);
  void checkExclusion(const ElementType *e);
  Boolean tryImplyTag(const Location &, unsigned &, unsigned &,
		      IList<Undo> &, IList<Event> &);
  void pushElementCheck(const ElementType *, StartElementEvent *,
			Boolean netEnabling);
  void pushElementCheck(const ElementType *, StartElementEvent *,
			IList<Undo> &, IList<Event> &);
  void queueElementEvents(IList<Event> &);
  Boolean parseAttributeSpec(Boolean inDeclaration,
			     AttributeList &,
			     Boolean &netEnabling);
  Boolean handleAttributeNameToken(Text &text,
				   AttributeList &,
				   unsigned &specLength);
  struct AttributeParameter {
    enum Type {
      end,
      name,
      nameToken,
      vi,
      recoverUnquoted
      };
  };

  Boolean parseAttributeParameter(Boolean inDecl,
				  Boolean allowVi,
				  AttributeParameter::Type &result,
				  Boolean &netEnabling);
  void extendUnquotedAttributeValue();

  Boolean parseAttributeValueSpec(Boolean inDecl,
				  const StringC &name,
				  AttributeList &atts,
				  unsigned &specLength);

  void parseEndTag();
  void parseEndTagClose();
  void parseEmptyEndTag();
  void parseNullEndTag();
  void endAllElements();
  void acceptEndTag(const ElementType *, EndElementEvent *);
  void implyCurrentElementEnd(const Location &);
  void maybeDefineEntity(const Ptr<Entity> &entity);
  Notation *lookupCreateNotation(const StringC &name);
  Boolean parseExternalEntity(StringC &name,
			      Entity::DeclType declType,
			      unsigned declInputLevel,
			      Param &parm);
  ShortReferenceMap *lookupCreateMap(const StringC &);
  StringC prettifyDelim(const StringC &delim);
  void handleShortref(int index);
  Boolean parseProcessingInstruction();
  Boolean parseAttributed(unsigned declInputLevel, Param &parm,
			  Vector<Attributed *> &attributed,
			  Boolean &isNotation);
  Boolean parseDeclaredValue(unsigned declInputLevel, Boolean isNotation,
			     Param &parm, Owner<DeclaredValue> &value);
  Boolean parseDefaultValue(unsigned declInputLevel, Boolean isNotation,
			    Param &parm, const StringC &attributeName,
			    Owner<DeclaredValue> &declaredValue,
			    Owner<AttributeDefinition> &def,
			    Boolean &anyCurrent);
  Boolean reportNonSgmlCharacter();
  void endInstance();
  Boolean implySgmlDecl();
  Boolean scanForSgmlDecl(const CharsetInfo &initCharset);
  void findMissingMinimum(const CharsetInfo &charset, ISet<WideChar> &);
  Boolean parseSgmlDecl();
  Boolean sdParseDocumentCharset(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseCapacity(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseScope(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseSyntax(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseExplicitSyntax(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseSyntaxCharset(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseShunchar(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseFunction(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseNaming(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseDelim(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseNames(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseQuantity(SdBuilder &sdBuilder, SdParam &parm);
  Boolean sdParseFeatures(SdBuilder &sd, SdParam &parm);
  Boolean sdParseAppinfo(SdBuilder &sd, SdParam &parm);
  Boolean parseSdParam(const AllowedSdParams &allow, SdParam &);
  Boolean parseSdParamLiteral(Boolean lita, String<SyntaxChar> &str);
  Boolean stringToNumber(const Char *s, size_t length, unsigned long &);
  void sdParamConvertToLiteral(SdParam &parm);
  void sdParamInvalidToken(Token token, const AllowedSdParams &);
  Boolean sdParseCharset(SdBuilder &sdBuilder, SdParam &parm,
			 Boolean isDocument,
			 CharsetDecl &, UnivCharsetDesc &);
  Boolean sdParseExternalCharset(Sd &, UnivCharsetDesc &desc);
  Boolean translateSyntax(CharSwitcher &switcher,
			  const CharsetInfo &syntaxCharset,
			  const CharsetInfo &docCharset,
			  WideChar syntaxChar,
			  Char &docChar);
  Boolean translateSyntax(SdBuilder &sdBuilder,
			  WideChar syntaxChar, Char &docChar);
  Boolean translateSyntax(SdBuilder &sdBuilder,
			  const String<SyntaxChar> &syntaxString,
			  StringC &docString);
  Boolean translateSyntaxNoSwitch(SdBuilder &sdBuilder,
				  WideChar syntaxChar, Char &docChar,
				  Number &count);
  Boolean translateName(SdBuilder &sdBuilder,
			const StringC &name,
			StringC &str);
  void translateRange(SdBuilder &sdBuilder, SyntaxChar start,
		      SyntaxChar end, ISet<Char> &chars);
  UnivChar translateUniv(UnivChar univChar,
			 CharSwitcher &switcher,
			 const CharsetInfo &syntaxCharset);
  Boolean univToDescCheck(const CharsetInfo &charset, UnivChar from,
			  Char &to);
  Boolean univToDescCheck(const CharsetInfo &charset, UnivChar from,
			  Char &to, WideChar &count);
  Boolean checkNotFunction(const Syntax &syn, Char c);
  Boolean checkGeneralDelim(const Syntax &syn, const StringC &delim);
  Boolean checkShortrefDelim(const Syntax &syn,
			     const CharsetInfo &charset,
			     const StringC &delim);
  Boolean checkNmchars(const ISet<Char> &set, const Syntax &syntax);
  void intersectCharSets(const ISet<Char> &s1, const ISet<Char> &s2,
			 ISet<WideChar> &inter);
  Boolean checkSwitches(CharSwitcher &switcher,
			const CharsetInfo &syntaxCharset);
  Boolean checkSwitchesMarkup(CharSwitcher &switcher);

  const StandardSyntaxSpec *lookupSyntax(const PublicId &id);
  Boolean referencePublic(const PublicId &id, PublicId::TextClass,
			  Boolean &givenError);
  void checkIdrefs();
  void checkTaglen(Index tagStartIndex);
  void checkSyntaxNamelen(const Syntax &syn);
  void checkElementAttribute(const ElementType *e, size_t checkFrom = 0);
  void checkDtd(Dtd &dtd);
  Boolean maybeStatusKeyword(const Entity &entity);
  void reportAmbiguity(const LeafContentToken *from,
		       const LeafContentToken *to1,
		       const LeafContentToken *to2,
		       unsigned ambigAndDepth);
  Boolean parseLinktypeDeclStart();
  Boolean parseLinktypeDeclEnd();
  Boolean parseLinkDecl();
  Boolean parseIdlinkDecl();
  Boolean parseLinkSet(Boolean idlink);
  void addIdLinkRule(const StringC &id, IdLinkRule &rule);
  void addLinkRule(LinkSet *linkSet,
		   const ElementType *sourceElement,
		   const ConstPtr<SourceLinkRuleResource> &linkRule);
  Boolean parseResultElementSpec(unsigned declInputLevel,
				 Param &parm,
				 Boolean idlink,
				 Boolean &implied,
				 const ElementType *&resultType,
				 AttributeList &attributes);
  LinkSet *lookupCreateLinkSet(const StringC &name);
  const ElementType *lookupResultElementType(const StringC &name);
  void endProlog();
  Boolean parseEntityReferenceNameGroup(Boolean &ignore);
  Boolean parseTagNameGroup(Boolean &active);
  void parseGroupStartTag();
  void parseGroupEndTag();
  Boolean skipAttributeSpec();
  Boolean lookingAtStartTag(StringC &gi);
  Boolean implyDtd(const StringC &gi);
  void findMissingTag(const ElementType *e, Vector<const ElementType *> &);
  unsigned paramsSubdocLevel(const SgmlParser::Params &);
  void addCommonAttributes(Dtd &dtd);
  Boolean parseAfdrDecl();
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Parser_INCLUDED */
