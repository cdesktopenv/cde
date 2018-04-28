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
/* $XConsortium: ParserMessages.h /main/1 1996/07/29 17:01:05 cde-hp $ */
// This file was automatically generated from ParserMessages.msg by msggen.pl.
#ifndef ParserMessages_INCLUDED
#define ParserMessages_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct ParserMessages {
  // 0
  static const MessageType1 nameLength;
  // 1
  static const MessageType1 parameterEntityNameLength;
  // 2
  static const MessageType1 numberLength;
  // 3
  static const MessageType1 attributeValueLength;
  // 4
  static const MessageType0 peroGrpoProlog;
  // 5
  static const MessageType0 groupLevel;
  // 6
  static const MessageType2 groupCharacter;
  // 7
  static const MessageType0 psRequired;
  // 8
  static const MessageType2 markupDeclarationCharacter;
  // 9
  static const MessageType0 declarationLevel;
  // 10
  static const MessageType0 groupEntityEnd;
  // 11
  static const MessageType1 invalidToken;
  // 12
  static const MessageType0 groupEntityReference;
  // 13
  static const MessageType1 duplicateGroupToken;
  // 14
  static const MessageType1 groupCount;
  // 15
  static const MessageType0 literalLevel;
  // 16
  static const MessageType1 literalMinimumData;
  // 17
  static const MessageType0 dataTagPatternNonSgml;
  // 18
  static const MessageType0 dataTagPatternFunction;
  // 19
  static const MessageType0 eroGrpoStartTag;
  // 20
  static const MessageType0 eroGrpoProlog;
  // 21
  static const MessageType1 functionName;
  // 22
  static const MessageType1 characterNumber;
  // 23
  static const MessageType1 parameterEntityUndefined;
  // 24
  static const MessageType1 entityUndefined;
  // 25
  static const MessageType0 rniNameStart;
  // 26
  static const MessageType0L commentEntityEnd;
  // 28
  static const MessageType0 mixedConnectors;
  // 29
  static const MessageType1 noSuchReservedName;
  // 30
  static const MessageType1 invalidReservedName;
  // 31
  static const MessageType1 minimumLiteralLength;
  // 32
  static const MessageType1 tokenizedAttributeValueLength;
  // 33
  static const MessageType1 systemIdentifierLength;
  // 34
  static const MessageType1 parameterLiteralLength;
  // 35
  static const MessageType1 dataTagPatternLiteralLength;
  // 36
  static const MessageType0 literalClosingDelimiter;
  // 37
  static const MessageType2 paramInvalidToken;
  // 38
  static const MessageType2 groupTokenInvalidToken;
  // 39
  static const MessageType2 connectorInvalidToken;
  // 40
  static const MessageType1 noSuchDeclarationType;
  // 41
  static const MessageType1 dtdSubsetDeclaration;
  // 42
  static const MessageType1 declSubsetCharacter;
  // 43
  static const MessageType0 documentEndDtdSubset;
  // 44
  static const MessageType1 prologCharacter;
  // 45
  static const MessageType0 documentEndProlog;
  // 46
  static const MessageType1 prologDeclaration;
  // 47
  static const MessageType1 rankStemGenericIdentifier;
  // 48
  static const MessageType0 missingTagMinimization;
  // 49
  static const MessageType1 duplicateElementDefinition;
  // 50
  static const MessageType0 entityApplicableDtd;
  // 51
  static const MessageType1L commentDeclInvalidToken;
  // 53
  static const MessageType1 instanceDeclaration;
  // 54
  static const MessageType0 contentNonSgml;
  // 55
  static const MessageType1 noCurrentRank;
  // 56
  static const MessageType1 duplicateAttlistNotation;
  // 57
  static const MessageType1 duplicateAttlistElement;
  // 58
  static const MessageType0 endTagEntityEnd;
  // 59
  static const MessageType1 endTagCharacter;
  // 60
  static const MessageType1 endTagInvalidToken;
  // 61
  static const MessageType0 pcdataNotAllowed;
  // 62
  static const MessageType1 elementNotAllowed;
  // 63
  static const MessageType2 missingElementMultiple;
  // 64
  static const MessageType2 missingElementInferred;
  // 65
  static const MessageType1 startTagEmptyElement;
  // 66
  static const MessageType1L omitEndTagDeclare;
  // 68
  static const MessageType1L omitEndTagOmittag;
  // 70
  static const MessageType1 omitStartTagDeclaredContent;
  // 71
  static const MessageType1 elementEndTagNotFinished;
  // 72
  static const MessageType1 omitStartTagDeclare;
  // 73
  static const MessageType1 taglvlOpenElements;
  // 74
  static const MessageType1 undefinedElement;
  // 75
  static const MessageType0 emptyEndTagNoOpenElements;
  // 76
  static const MessageType1 elementNotFinished;
  // 77
  static const MessageType1 elementNotOpen;
  // 78
  static const MessageType1 internalParameterDataEntity;
  // 79
  static const MessageType1 attributeSpecCharacter;
  // 80
  static const MessageType0 unquotedAttributeValue;
  // 81
  static const MessageType0 attributeSpecEntityEnd;
  // 82
  static const MessageType1 externalParameterDataSubdocEntity;
  // 83
  static const MessageType1 duplicateEntityDeclaration;
  // 84
  static const MessageType1 duplicateParameterEntityDeclaration;
  // 85
  static const MessageType1 noDtdSubset;
  // 86
  static const MessageType0 piEntityReference;
  // 87
  static const MessageType0 internalDataEntityReference;
  // 88
  static const MessageType0 externalNonTextEntityReference;
  // 89
  static const MessageType0 externalNonTextEntityRcdata;
  // 90
  static const MessageType0 entlvl;
  // 91
  static const MessageType0 piEntityRcdata;
  // 92
  static const MessageType1 recursiveEntityReference;
  // 93
  static const MessageType1 undefinedShortrefMapInstance;
  // 94
  static const MessageType0 usemapAssociatedElementTypeDtd;
  // 95
  static const MessageType0 usemapAssociatedElementTypeInstance;
  // 96
  static const MessageType2 undefinedShortrefMapDtd;
  // 97
  static const MessageType1 unknownShortrefDelim;
  // 98
  static const MessageType1 delimDuplicateMap;
  // 99
  static const MessageType0 noDocumentElement;
  // 100
  static const MessageType0 processingInstructionEntityEnd;
  // 101
  static const MessageType1 processingInstructionLength;
  // 102
  static const MessageType0 processingInstructionClose;
  // 103
  static const MessageType0 attributeSpecNameTokenExpected;
  // 104
  static const MessageType1 noSuchAttributeToken;
  // 105
  static const MessageType0 attributeNameShorttag;
  // 106
  static const MessageType1 noSuchAttribute;
  // 107
  static const MessageType0 attributeValueExpected;
  // 108
  static const MessageType1 nameTokenLength;
  // 109
  static const MessageType0 attributeSpecLiteral;
  // 110
  static const MessageType1 duplicateAttributeSpec;
  // 111
  static const MessageType1 duplicateAttributeDef;
  // 112
  static const MessageType0 emptyDataAttributeSpec;
  // 113
  static const MessageType0 markedSectionEnd;
  // 114
  static const MessageType1 markedSectionLevel;
  // 115
  static const MessageType0L unclosedMarkedSection;
  // 117
  static const MessageType0 specialParseEntityEnd;
  // 118
  static const MessageType2 normalizedAttributeValueLength;
  // 119
  static const MessageType0 attributeValueSyntax;
  // 120
  static const MessageType2 attributeValueChar;
  // 121
  static const MessageType1 attributeValueMultiple;
  // 122
  static const MessageType2 attributeValueNumberToken;
  // 123
  static const MessageType2 attributeValueName;
  // 124
  static const MessageType1 attributeMissing;
  // 125
  static const MessageType1 requiredAttributeMissing;
  // 126
  static const MessageType1 currentAttributeMissing;
  // 127
  static const MessageType1 invalidNotationAttribute;
  // 128
  static const MessageType1 invalidEntityAttribute;
  // 129
  static const MessageType3 attributeValueNotInGroup;
  // 130
  static const MessageType1 notDataOrSubdocEntity;
  // 131
  static const MessageType3 ambiguousModelInitial;
  // 132
  static const MessageType5 ambiguousModel;
  // 133
  static const MessageType5 ambiguousModelSingleAnd;
  // 134
  static const MessageType6 ambiguousModelMultipleAnd;
  // 135
  static const MessageType1L commentDeclarationCharacter;
  // 137
  static const MessageType1 nonSgmlCharacter;
  // 138
  static const MessageType0 dataMarkedSectionDeclSubset;
  // 139
  static const MessageType1L duplicateId;
  // 141
  static const MessageType1 notFixedValue;
  // 142
  static const MessageType1 sdCommentSignificant;
  // 143
  static const MessageType1 standardVersion;
  // 144
  static const MessageType1 namingBeforeLcnmstrt;
  // 145
  static const MessageType1 sdEntityEnd;
  // 146
  static const MessageType2 sdInvalidNameToken;
  // 147
  static const MessageType1 numberTooBig;
  // 148
  static const MessageType1 sdLiteralSignificant;
  // 149
  static const MessageType1 syntaxCharacterNumber;
  // 150
  static const MessageType0 sdParameterEntity;
  // 151
  static const MessageType2 sdParamInvalidToken;
  // 152
  static const MessageType0 giveUp;
  // 153
  static const MessageType1 sdMissingCharacters;
  // 154
  static const MessageType1 missingMinimumChars;
  // 155
  static const MessageType1 duplicateCharNumbers;
  // 156
  static const MessageType1 codeSetHoles;
  // 157
  static const MessageType1 basesetCharsMissing;
  // 158
  static const MessageType1 documentCharMax;
  // 159
  static const MessageType1 fpiMissingField;
  // 160
  static const MessageType1 fpiMissingTextClassSpace;
  // 161
  static const MessageType1 fpiInvalidTextClass;
  // 162
  static const MessageType1 fpiInvalidLanguage;
  // 163
  static const MessageType1 fpiIllegalDisplayVersion;
  // 164
  static const MessageType1 fpiExtraField;
  // 165
  static const MessageType0 notationIdentifierTextClass;
  // 166
  static const MessageType1 unknownBaseset;
  // 167
  static const MessageType2 lexicalAmbiguity;
  // 168
  static const MessageType1 translateSyntaxChar;
  // 169
  static const MessageType1 missingSignificant;
  // 170
  static const MessageType1 missingSyntaxChar;
  // 171
  static const MessageType1 unknownCapacitySet;
  // 172
  static const MessageType1 duplicateCapacity;
  // 173
  static const MessageType1 capacityExceedsTotalcap;
  // 174
  static const MessageType1 unknownPublicSyntax;
  // 175
  static const MessageType0 nmstrtLength;
  // 176
  static const MessageType0 nmcharLength;
  // 177
  static const MessageType1 subdocLevel;
  // 178
  static const MessageType1 subdocEntity;
  // 179
  static const MessageType0 parameterEntityNotEnded;
  // 180
  static const MessageType1 missingId;
  // 181
  static const MessageType1 dtdUndefinedElement;
  // 182
  static const MessageType1 elementNotFinishedDocumentEnd;
  // 183
  static const MessageType0 subdocGiveUp;
  // 184
  static const MessageType0 noDtd;
  // 185
  static const MessageType1 taglen;
  // 186
  static const MessageType0 groupParameterEntityNotEnded;
  // 187
  static const MessageType1 invalidSgmlChar;
  // 188
  static const MessageType1 translateDocChar;
  // 189
  static const MessageType1 attributeValueLengthNeg;
  // 190
  static const MessageType1 tokenizedAttributeValueLengthNeg;
  // 191
  static const MessageType1 scopeInstanceQuantity;
  // 192
  static const MessageType1 basesetTextClass;
  // 193
  static const MessageType1 capacityTextClass;
  // 194
  static const MessageType1 syntaxTextClass;
  // 195
  static const MessageType0 msocharRequiresMsichar;
  // 196
  static const MessageType1 switchNotMarkup;
  // 197
  static const MessageType1 switchNotInCharset;
  // 198
  static const MessageType1 ambiguousDocCharacter;
  // 199
  static const MessageType1 oneFunction;
  // 200
  static const MessageType1 duplicateFunctionName;
  // 201
  static const MessageType1 missingSignificant646;
  // 202
  static const MessageType1 generalDelimAllFunction;
  // 203
  static const MessageType1 nmcharLetter;
  // 204
  static const MessageType1 nmcharDigit;
  // 205
  static const MessageType1 nmcharRe;
  // 206
  static const MessageType1 nmcharRs;
  // 207
  static const MessageType1 nmcharSpace;
  // 208
  static const MessageType1 nmcharSepchar;
  // 209
  static const MessageType1 switchLetterDigit;
  // 210
  static const MessageType0 zeroNumberOfCharacters;
  // 211
  static const MessageType1 nameReferenceReservedName;
  // 212
  static const MessageType1 ambiguousReservedName;
  // 213
  static const MessageType1 duplicateReservedName;
  // 214
  static const MessageType1 reservedNameSyntax;
  // 215
  static const MessageType1 multipleBSequence;
  // 216
  static const MessageType1 blankAdjacentBSequence;
  // 217
  static const MessageType2 delimiterLength;
  // 218
  static const MessageType2 reservedNameLength;
  // 219
  static const MessageType1 nmcharNmstrt;
  // 220
  static const MessageType0 scopeInstanceSyntaxCharset;
  // 221
  static const MessageType0 emptyOmitEndTag;
  // 222
  static const MessageType1 conrefOmitEndTag;
  // 223
  static const MessageType1 conrefEmpty;
  // 224
  static const MessageType1 notationEmpty;
  // 225
  static const MessageType0 dataAttributeDeclaredValue;
  // 226
  static const MessageType0 dataAttributeDefaultValue;
  // 227
  static const MessageType2 attcnt;
  // 228
  static const MessageType0 idDeclaredValue;
  // 229
  static const MessageType1 multipleIdAttributes;
  // 230
  static const MessageType1 multipleNotationAttributes;
  // 231
  static const MessageType1 duplicateAttributeToken;
  // 232
  static const MessageType1 notationNoAttributes;
  // 233
  static const MessageType2 entityNotationUndefined;
  // 234
  static const MessageType2 mapEntityUndefined;
  // 235
  static const MessageType1 attlistNotationUndefined;
  // 236
  static const MessageType1 bracketedLitlen;
  // 237
  static const MessageType1 genericIdentifierLength;
  // 238
  static const MessageType0 instanceStartOmittag;
  // 239
  static const MessageType1 grplvl;
  // 240
  static const MessageType1 grpgtcnt;
  // 241
  static const MessageType0 minimizedStartTag;
  // 242
  static const MessageType0 minimizedEndTag;
  // 243
  static const MessageType0 multipleDtds;
  // 244
  static const MessageType0 afterDocumentElementEntityEnd;
  // 245
  static const MessageType1 declarationAfterDocumentElement;
  // 246
  static const MessageType0 characterReferenceAfterDocumentElement;
  // 247
  static const MessageType0 entityReferenceAfterDocumentElement;
  // 248
  static const MessageType0 markedSectionAfterDocumentElement;
  // 249
  static const MessageType3 requiredElementExcluded;
  // 250
  static const MessageType3 invalidExclusion;
  // 251
  static const MessageType0 attributeValueShorttag;
  // 252
  static const MessageType0 conrefNotation;
  // 253
  static const MessageType1 duplicateNotationDeclaration;
  // 254
  static const MessageType1L duplicateShortrefDeclaration;
  // 256
  static const MessageType1 duplicateDelimGeneral;
  // 257
  static const MessageType1 idrefGrpcnt;
  // 258
  static const MessageType1 entityNameGrpcnt;
  // 259
  static const MessageType2 attsplen;
  // 260
  static const MessageType1 duplicateDelimShortref;
  // 261
  static const MessageType1 duplicateDelimShortrefSet;
  // 262
  static const MessageType1 defaultEntityInAttribute;
  // 263
  static const MessageType1 defaultEntityReference;
  // 264
  static const MessageType2 mapDefaultEntity;
  // 265
  static const MessageType1 noSuchDtd;
  // 266
  static const MessageType1 noLpdSubset;
  // 267
  static const MessageType0 assocElementDifferentAtts;
  // 268
  static const MessageType1 duplicateLinkSet;
  // 269
  static const MessageType0 emptyResultAttributeSpec;
  // 270
  static const MessageType1 noSuchSourceElement;
  // 271
  static const MessageType1 noSuchResultElement;
  // 272
  static const MessageType0 documentEndLpdSubset;
  // 273
  static const MessageType1 lpdSubsetDeclaration;
  // 274
  static const MessageType0 idlinkDeclSimple;
  // 275
  static const MessageType0 linkDeclSimple;
  // 276
  static const MessageType1 simpleLinkAttlistElement;
  // 277
  static const MessageType0 shortrefOnlyInBaseDtd;
  // 278
  static const MessageType0 usemapOnlyInBaseDtd;
  // 279
  static const MessageType0 linkAttributeDefaultValue;
  // 280
  static const MessageType0 linkAttributeDeclaredValue;
  // 281
  static const MessageType0 simpleLinkFixedAttribute;
  // 282
  static const MessageType0 duplicateIdLinkSet;
  // 283
  static const MessageType1 noInitialLinkSet;
  // 284
  static const MessageType1 notationUndefinedSourceDtd;
  // 285
  static const MessageType0 simpleLinkResultNotImplied;
  // 286
  static const MessageType0 simpleLinkFeature;
  // 287
  static const MessageType0 implicitLinkFeature;
  // 288
  static const MessageType0 explicitLinkFeature;
  // 289
  static const MessageType0 lpdBeforeBaseDtd;
  // 290
  static const MessageType0 dtdAfterLpd;
  // 291
  static const MessageType1 unstableLpdGeneralEntity;
  // 292
  static const MessageType1 unstableLpdParameterEntity;
  // 293
  static const MessageType1 multipleIdLinkRuleAttribute;
  // 294
  static const MessageType1 multipleLinkRuleAttribute;
  // 295
  static const MessageType2 uselinkBadLinkSet;
  // 296
  static const MessageType1 uselinkSimpleLpd;
  // 297
  static const MessageType1 uselinkBadLinkType;
  // 298
  static const MessageType1 duplicateDtdLpd;
  // 299
  static const MessageType1 duplicateLpd;
  // 300
  static const MessageType1 duplicateDtd;
  // 301
  static const MessageType1 undefinedLinkSet;
  // 302
  static const MessageType1 duplicateImpliedResult;
  // 303
  static const MessageType1 simpleLinkCount;
  // 304
  static const MessageType0 duplicateExplicitChain;
  // 305
  static const MessageType1 explicit1RequiresSourceTypeBase;
  // 306
  static const MessageType0 oneImplicitLink;
  // 307
  static const MessageType1 sorryLink;
  // 308
  static const MessageType0 entityReferenceMissingName;
  // 309
  static const MessageType1 explicitNoRequiresSourceTypeBase;
  // 310
  static const MessageType0 linkActivateTooLate;
  // 311
  static const MessageType0 pass2Ee;
  // 312
  static const MessageType2 idlinkElementType;
  // 313
  static const MessageType0 datatagNotImplemented;
  // 314
  static const MessageType0 startTagMissingName;
  // 315
  static const MessageType0 endTagMissingName;
  // 316
  static const MessageType0 startTagGroupNet;
  // 317
  static const MessageType0 documentElementUndefined;
  // 318
  static const MessageType0 badDefaultSgmlDecl;
  // 319
  static const MessageType1L nonExistentEntityRef;
  // 321
  static const MessageType0 pcdataUnreachable;
  // 322
  static const MessageType0 sdInvalidEllipsis;
  // 323
  static const MessageType0 sdInvalidRange;
  // 324
  static const MessageType0 sdEmptyDelimiter;
  // 325
  static const MessageType0 tooManyCharsMinimumLiteral;
  // 326
  static const MessageType1 defaultedEntityDefined;
  // 327
  static const MessageType0 unclosedStartTag;
  // 328
  static const MessageType0 unclosedEndTag;
  // 329
  static const MessageType0 emptyStartTag;
  // 330
  static const MessageType0 emptyEndTag;
  // 331
  static const MessageType0 netStartTag;
  // 332
  static const MessageType0 nullEndTag;
  // 333
  static const MessageType1 unusedMap;
  // 334
  static const MessageType1 unusedParamEntity;
  // 335
  static const MessageType1 cannotGenerateSystemIdPublic;
  // 336
  static const MessageType1 cannotGenerateSystemIdGeneral;
  // 337
  static const MessageType1 cannotGenerateSystemIdParameter;
  // 338
  static const MessageType1 cannotGenerateSystemIdDoctype;
  // 339
  static const MessageType1 cannotGenerateSystemIdLinktype;
  // 340
  static const MessageType1 cannotGenerateSystemIdNotation;
  // 341
  static const MessageType1 excludeIncludeSame;
  // 342
  static const MessageType1 implyingDtd;
  // 343
  static const MessageType1 afdrVersion;
  // 344
  static const MessageType0 missingAfdrDecl;
  // 1000
  static const MessageFragment delimStart;
  // 1001
  static const MessageFragment delimEnd;
  // 1002
  static const MessageFragment digit;
  // 1003
  static const MessageFragment nameStartCharacter;
  // 1004
  static const MessageFragment sepchar;
  // 1005
  static const MessageFragment separator;
  // 1006
  static const MessageFragment nameCharacter;
  // 1007
  static const MessageFragment dataCharacter;
  // 1008
  static const MessageFragment minimumDataCharacter;
  // 1009
  static const MessageFragment significantCharacter;
  // 1010
  static const MessageFragment recordEnd;
  // 1011
  static const MessageFragment recordStart;
  // 1012
  static const MessageFragment space;
  // 1013
  static const MessageFragment listSep;
  // 1014
  static const MessageFragment rangeSep;
  // 1015
  static const MessageFragment parameterLiteral;
  // 1016
  static const MessageFragment dataTagGroup;
  // 1017
  static const MessageFragment modelGroup;
  // 1018
  static const MessageFragment dataTagTemplateGroup;
  // 1019
  static const MessageFragment name;
  // 1020
  static const MessageFragment nameToken;
  // 1021
  static const MessageFragment elementToken;
  // 1022
  static const MessageFragment inclusions;
  // 1023
  static const MessageFragment exclusions;
  // 1024
  static const MessageFragment minimumLiteral;
  // 1025
  static const MessageFragment attributeValueLiteral;
  // 1026
  static const MessageFragment systemIdentifier;
  // 1027
  static const MessageFragment number;
  // 1028
  static const MessageFragment attributeValue;
  // 1029
  static const MessageFragment capacityName;
  // 1030
  static const MessageFragment generalDelimiteRoleName;
  // 1031
  static const MessageFragment referenceReservedName;
  // 1032
  static const MessageFragment quantityName;
  // 1033
  static const MessageFragment entityEnd;
  // 1034
  static const MessageFragment shortrefDelim;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ParserMessages_INCLUDED */
