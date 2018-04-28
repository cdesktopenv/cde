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
/* $XConsortium: Syntax.C /main/2 1996/08/12 13:22:19 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "Syntax.h"
#include "Sd.h"
#include "CharsetInfo.h"
#include "ISetIter.h"
#include "macros.h"
#include "MarkupScan.h"
#include "constant.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

const int Syntax::referenceQuantity_[] = {
  40,
  960,
  960,
  16,
  16,
  16,
  32,
  96,
  16,
  240,
  8,
  2,
  240,
  960,
  24
};

Syntax::Syntax(const Sd &sd)
: generalSubst_(0),
  entitySubst_(0),
  categoryTable_(otherCategory),
  shuncharControls_(0),
  multicode_(0),
  markupScanTable_(MarkupScan::normal),
  namecaseGeneral_(false),
  namecaseEntity_(false)
{
  static const char lcletter[] = "abcdefghijklmnopqrstuvwxyz";
  static const char ucletter[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int i;
  for (i = 0; i < 26; i++) {
    Char lc = sd.execToDoc(lcletter[i]);
    Char uc = sd.execToDoc(ucletter[i]); 
    set_[nameStart] += lc;
    set_[nameStart] += uc;
    set_[minimumData] += lc;
    set_[minimumData] += uc;
    set_[significant] += lc;
    set_[significant] += uc;
    categoryTable_.setChar(lc, nameStartCategory);
    categoryTable_.setChar(uc, nameStartCategory);
    subst(lc, uc);
  }
  static const char digits[] = "0123456789";
  for (i = 0; i < 10; i++) {
    Char c = sd.execToDoc(digits[i]);
    set_[digit] += c;
    set_[minimumData] += c;
    set_[significant] += c;
    categoryTable_.setChar(c, digitCategory);
  }
  static const char special[] = "'()+,-./:=?";
  for (i = 0; special[i] != '\0'; i++) {
    Char c = sd.execToDoc(special[i]);
    set_[minimumData] += c;
    set_[significant] += c;
  }
  for (i = 0; i < nQuantity; i++)
    quantity_[i] = referenceQuantity_[i];
  for (i = 0; i < 3; i++)
    standardFunctionValid_[i] = 0;
}


Syntax::Syntax(const Syntax & syn)
	: generalSubst_(syn.generalSubst_),
	  entitySubst_(syn.entitySubst_),
	  shunchar_(syn.shunchar_),
	  shuncharControls_(syn.shuncharControls_),
	  namecaseGeneral_(syn.namecaseGeneral_),
	  namecaseEntity_(syn.namecaseEntity_),
	  delimShortrefComplex_(syn.delimShortrefComplex_),
	  delimShortrefSimple_(syn.delimShortrefSimple_),
	  nameTable_(syn.nameTable_),
	  functionTable_(syn.functionTable_),
	  upperSubst_(syn.upperSubst_),
	  identitySubst_(syn.identitySubst_),
	  categoryTable_(syn.categoryTable_),
	  multicode_(syn.multicode_),
	  markupScanTable_(syn.markupScanTable_)
{
  int i;

  for (i = 0; i < nSet; i++)
    set_[i] = syn.set_[i];

  for (i = 0; i < 3; i++) {
    standardFunction_[i] = syn.standardFunction_[i];
    standardFunctionValid_[i] = syn.standardFunctionValid_[i];
  }

  for (i = 0; i < nDelimGeneral; i++)
    delimGeneral_[i] = syn.delimGeneral_[i];

  for (i = 0; i < nNames; i++)
    names_[i] = syn.names_[i];

  for (i = 0; i < nQuantity; i++)
    quantity_[i] = syn.quantity_[i];
}

void Syntax::addNameCharacters(const ISet<Char> &set)
{
  ISetIter<Char> iter(set);
  Char min, max;
  while (iter.next(min, max)) {
    set_[nmchar].addRange(min, max);
    set_[significant].addRange(min, max);
    categoryTable_.setRange(min, max, otherNameCategory);
  }
}

void Syntax::addNameStartCharacters(const ISet<Char> &set)
{
  ISetIter<Char> iter(set);
  Char min, max;
  while (iter.next(min, max)) {
    set_[nameStart].addRange(min, max);
    set_[significant].addRange(min, max);
    categoryTable_.setRange(min, max, nameStartCategory);
  }
}

void Syntax::addSubst(Char lc, Char uc)
{
  subst(lc, uc);
}

void Syntax::setStandardFunction(StandardFunction f, Char c)
{
  standardFunction_[f] = c;
  standardFunctionValid_[f] = 1;
  set_[minimumData] += c;
  set_[s] += c;
  categoryTable_.setChar(c, sCategory);
  set_[functionChar] += c;
  set_[significant] += c;
  switch (f) {
  case fSPACE:
    set_[blank] += c;
    break;
  case fRE:
  case fRS:
    break;
  }
}

void Syntax::enterStandardFunctionNames()
{
  static ReservedName name[3] = {
    rRE, rRS, rSPACE
  };
  for (int i = 0; i < 3; i++)
    if (standardFunctionValid_[i])
      functionTable_.insert(reservedName(name[i]), standardFunction_[i]);
}

void Syntax::setDelimGeneral(int i, const StringC &str)
{
  delimGeneral_[i] = str;
  for (size_t j = 0; j < str.size(); j++)
    set_[significant] += str[j];
}

void Syntax::addDelimShortref(const StringC &str, const CharsetInfo &charset)
{
  if (str.size() == 1 && str[0] != charset.execToDesc('B') && !isB(str[0]))
    delimShortrefSimple_.add(str[0]);
  else
    delimShortrefComplex_.push_back(str);
  for (size_t i = 0; i < str.size(); i++)
    set_[significant] += str[i];
}

void Syntax::addDelimShortrefs(const ISet<Char> &shortrefChars,
			       const CharsetInfo &charset)
{
  ISetIter<Char> blankIter(set_[blank]);
  Char min, max;
  StringC specialChars;
  while (blankIter.next(min, max)) {
    do {
      specialChars += min;
    } while (min++ != max);
  }
  specialChars += charset.execToDesc('B');
  const ISet<Char> *simpleCharsPtr = &shortrefChars;
  ISet<Char> simpleChars;
  for (size_t i = 0; i < specialChars.size(); i++)
    if (shortrefChars.contains(specialChars[i])) {
      if (simpleCharsPtr != &simpleChars) {
	simpleChars = shortrefChars;
	simpleCharsPtr = &simpleChars;
      }
      simpleChars.remove(specialChars[i]);
    }
  ISetIter<Char> iter(*simpleCharsPtr);
  while (iter.next(min, max)) {
    delimShortrefSimple_.addRange(min, max);
    set_[significant].addRange(min, max);
  }
}

void Syntax::addFunctionChar(const StringC &str, FunctionClass fun, Char c)
{
  switch (fun) {
  case cFUNCHAR:
    break;
  case cSEPCHAR:
    set_[s] += c;
    categoryTable_.setChar(c, sCategory);
    set_[blank] += c;
    set_[sepchar] += c;
    break;
  case cMSOCHAR:
    multicode_ = 1;
    markupScanTable_.setChar(c, MarkupScan::out);
    break;
  case cMSICHAR:
    // don't need to do anything special if we just have MSICHARs
    markupScanTable_.setChar(c, MarkupScan::in);
    break;
  case cMSSCHAR:
    multicode_ = 1;
    markupScanTable_.setChar(c, MarkupScan::suppress);
    break;
  }
  set_[functionChar] += c;
  set_[significant] += c;
  functionTable_.insert(str, c);
}

void Syntax::setName(int i, const StringC &str)
{
  names_[i] = str;
  nameTable_.insert(str, i);
}

void Syntax::setNamecaseGeneral(Boolean b)
{
  namecaseGeneral_ = b;
  generalSubst_ = b ? &upperSubst_ : &identitySubst_;
}

void Syntax::setNamecaseEntity(Boolean b)
{
  namecaseEntity_ = b;
  entitySubst_ = b ? &upperSubst_ : &identitySubst_;
}

void Syntax::subst(Char from, Char to)
{
  upperSubst_.addSubst(from, to);
}

void Syntax::addShunchar(Char c)
{
  shunchar_.add(c);
}

Boolean Syntax::lookupReservedName(const StringC &str,
				   ReservedName *result) const
{
  const int *tem = nameTable_.lookup(str);
  if (tem) {
    *result = ReservedName(*tem);
    return 1;
  }
  else
    return 0;
}

Boolean Syntax::lookupFunctionChar(const StringC &name, Char *result) const
{
  const Char *p = functionTable_.lookup(name);
  if (p) {
    *result = *p;
    return 1;
  }
  else
    return 0;
}

#ifdef __GNUG__
typedef HashTableIter<StringC,Char> Dummy_HashTableIter_StringC_Char;
#endif

Boolean Syntax::charFunctionName(Char c, const StringC *&name) const
{
  HashTableIter<StringC,Char> iter(functionTable_);
  const Char *cp;
  while (iter.next(name, cp))
    if (*cp == c)
      return 1;
  return 0;
}

Boolean Syntax::isValidShortref(const StringC &str) const
{
  if (str.size() == 1 && delimShortrefSimple_.contains(str[0]))
    return 1;
  for (size_t i = 0; i < delimShortrefComplex_.size(); i++)
    if (str == delimShortrefComplex_[i])
      return 1;
  return 0;
}

void Syntax::implySgmlChar(const CharsetInfo &docCharset)
{
  docCharset.getDescSet(set_[sgmlChar]);
  ISet<WideChar> invalid;
  checkSgmlChar(docCharset, 0, invalid);
  ISetIter<WideChar> iter(invalid);
  WideChar min, max;
  while (iter.next(min, max)) {
    do {
      if (min <= charMax)
	set_[sgmlChar].remove(Char(min));
    } while (min++ != max);
  }
}

void Syntax::checkSgmlChar(const CharsetInfo &docCharset,
			   const ::SP_NAMESPACE_SCOPE Syntax *otherSyntax,
			   ISet<WideChar> &invalid) const
{
  ISetIter<Char> iter(shunchar_);
  Char min, max;
  while (iter.next(min, max)) {
    if (min <= max) {
      do {
	if (!set_[significant].contains(min)
	    && (!otherSyntax || !otherSyntax->set_[significant].contains(min))
	    && set_[sgmlChar].contains(min))
	  invalid += min;
      } while (min++ != max);
    }
  }
  if (shuncharControls_) {
    UnivChar i;
    for (i = 0; i < 32; i++)
      checkUnivControlChar(i, docCharset, otherSyntax, invalid);
    for (i = 127; i < 160; i++)
      checkUnivControlChar(i, docCharset, otherSyntax, invalid);
  }
}

void Syntax::checkUnivControlChar(UnivChar univChar,
				  const CharsetInfo &docCharset,
				  const ::SP_NAMESPACE_SCOPE Syntax *otherSyntax,
				  ISet<WideChar> &invalid) const
{
  WideChar c;
  ISet<WideChar> set;
  switch (docCharset.univToDesc(univChar, c, set)) {
  case 0:
    break;
  case 1:
    set += c;
    // fall through
  default:
    {
      ISetIter<WideChar> iter(set);
      WideChar min, max;
      while (iter.next(min, max)) {
	do {
	  if (min > charMax)
	    break;
	  Char ch = Char(min);
	  if (!set_[significant].contains(ch)
	      && (!otherSyntax
		  || !otherSyntax->set_[significant].contains(ch))
	      && set_[sgmlChar].contains(ch))
	    invalid += ch;
	} while (min++ != max);
      }
    }
  }
}

StringC Syntax::rniReservedName(ReservedName i) const
{
  StringC result = delimGeneral(dRNI);
  result += reservedName(i);
  return result;
}

const SubstTable<Char> &Syntax::upperSubstTable() const
{
  return upperSubst_;
}

const StringC &Syntax::peroDelim() const
{
  return delimGeneral(dPERO);
}


#ifdef SP_NAMESPACE
}
#endif
