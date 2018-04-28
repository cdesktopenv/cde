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
/* $XConsortium: Sd.h /main/1 1996/07/29 17:03:44 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Sd_INCLUDED
#define Sd_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "Boolean.h"
#include "Resource.h"
#include "CharsetInfo.h"
#include "ExternalId.h"
#include "ISet.h"
#include "Syntax.h"
#include "CharsetDecl.h"
#include "HashTable.h"

// Information about the SGML declaration that is not syntax specific.

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API Sd : public Resource {
public:
  // These must be in the same order as in the SGML declaration.
  enum BooleanFeature {
    fDATATAG,
    fOMITTAG,
    fRANK,
    fSHORTTAG,
    fIMPLICIT,
    fFORMAL
    };
  enum { nBooleanFeature = fFORMAL + 1 };
  // These must be in the same order as in the SGML declaration.
  enum NumberFeature {
    fSIMPLE,
    fEXPLICIT,
    fCONCUR,
    fSUBDOC
  };
  enum { nNumberFeature = fSUBDOC + 1 };
  // These are names used only in the SGML declaration.
  enum ReservedName {
    rAPPINFO,
    rBASESET,
    rCAPACITY,
    rCHARSET,
    rCONCUR,
    rCONTROLS,
    rDATATAG,
    rDELIM,
    rDESCSET,
    rDOCUMENT,
    rENTITY,
    rEXPLICIT,
    rFEATURES,
    rFORMAL,
    rFUNCHAR,
    rFUNCTION,
    rGENERAL,
    rIMPLICIT,
    rINSTANCE,
    rLCNMCHAR,
    rLCNMSTRT,
    rLINK,
    rMINIMIZE,
    rMSICHAR,
    rMSOCHAR,
    rMSSCHAR,
    rNAMECASE,
    rNAMES,
    rNAMING,
    rNO,
    rNONE,
    rOMITTAG,
    rOTHER,
    rPUBLIC,
    rQUANTITY,
    rRANK,
    rRE,
    rRS,
    rSCOPE,
    rSEPCHAR,
    rSGML,
    rSGMLREF,
    rSHORTREF,
    rSHORTTAG,
    rSHUNCHAR,
    rSIMPLE,
    rSPACE,
    rSUBDOC,
    rSWITCHES,
    rSYNTAX,
    rUCNMCHAR,
    rUCNMSTRT,
    rUNUSED,
    rYES
  };
  enum Capacity {
    TOTALCAP,
    ENTCAP,
    ENTCHCAP,
    ELEMCAP,
    GRPCAP,
    EXGRPCAP,
    EXNMCAP,
    ATTCAP,
    ATTCHCAP,
    AVGRPCAP,
    NOTCAP,
    NOTCHCAP,
    IDCAP,
    IDREFCAP,
    MAPCAP,
    LKSETCAP,
    LKNMCAP
  };
  enum { nCapacity = LKNMCAP + 1 };
  Sd();
  void setDocCharsetDesc(const UnivCharsetDesc &);
  void setSgmlChar(const ISet<Char> &);
  Boolean matchesReservedName(const StringC &, ReservedName) const;
  int digitWeight(Char) const;
  Boolean link() const;
  Number simpleLink() const;
  Boolean implicitLink() const;
  Number explicitLink() const;
  Boolean shorttag() const;
  Number concur() const;
  Boolean omittag() const;
  Boolean rank() const;
  Boolean datatag() const;
  Boolean formal() const;
  Number subdoc() const;
  StringC reservedName(int) const;
  Boolean lookupQuantityName(const StringC &, Syntax::Quantity &) const;
  Boolean lookupGeneralDelimiterName(const StringC &, Syntax::DelimGeneral &)
       const;
  Boolean lookupCapacityName(const StringC &, Sd::Capacity &) const;
  StringC quantityName(Syntax::Quantity) const;
  const CharsetInfo &docCharset() const;
  Char execToDoc(char) const;
  StringC execToDoc(const char *) const;
  Number capacity(int) const;
  void setCapacity(int, Number);
  StringC capacityName(int) const;
  Boolean scopeInstance() const;
  void setScopeInstance();
  void setDocCharsetDecl(CharsetDecl &);
  const CharsetDecl &docCharsetDecl() const;
  void setBooleanFeature(BooleanFeature, Boolean);
  void setNumberFeature(NumberFeature, Number);
  StringC generalDelimiterName(Syntax::DelimGeneral) const;
  UnivChar nameToUniv(const StringC &);
private:
  enum { nFeature = fFORMAL + 1 };
  PackedBoolean booleanFeature_[nBooleanFeature];
  Number numberFeature_[nNumberFeature];
  Number capacity_[nCapacity];
  CharsetInfo docCharset_;
  CharsetDecl docCharsetDecl_;
  Boolean scopeInstance_;
  HashTable<StringC,int> namedCharTable_;
  static const char *const reservedName_[];
  static const char *const generalDelimiterName_[];
  static const char *const capacityName_[];
  static const char *const quantityName_[];
};

inline
Boolean Sd::link() const
{
  return (numberFeature_[fSIMPLE]
	  || booleanFeature_[fIMPLICIT]
	  || numberFeature_[fEXPLICIT]);
}

inline
Number Sd::explicitLink() const
{
  return numberFeature_[fEXPLICIT];
}

inline
Boolean Sd::implicitLink() const
{
  return booleanFeature_[fIMPLICIT];
}

inline
Number Sd::simpleLink() const
{
  return numberFeature_[fSIMPLE];
}

inline
Boolean Sd::shorttag() const
{
  return booleanFeature_[fSHORTTAG];
}

inline
Number Sd::concur() const
{
  return numberFeature_[fCONCUR];
}

inline
Number Sd::subdoc() const
{
  return numberFeature_[fSUBDOC];
}


inline
Boolean Sd::omittag() const
{
  return booleanFeature_[fOMITTAG];
}

inline
Boolean Sd::rank() const
{
  return booleanFeature_[fRANK];
}

inline
Boolean Sd::datatag() const
{
  return booleanFeature_[fDATATAG];
}

inline
Boolean Sd::formal() const
{
  return booleanFeature_[fFORMAL];
}

inline
Char Sd::execToDoc(char c) const
{
  return docCharset_.execToDesc(c);
}

inline
StringC Sd::execToDoc(const char *s) const
{
  return docCharset_.execToDesc(s);
}

inline
StringC Sd::reservedName(int i) const
{
  return execToDoc(reservedName_[i]);
}

inline
const CharsetInfo &Sd::docCharset() const
{
  return docCharset_;
}

inline
int Sd::digitWeight(Char c) const
{
  return docCharset_.digitWeight(c);
}

inline
Number Sd::capacity(int i) const
{
  return capacity_[i];
}

inline
void Sd::setCapacity(int i, Number n)
{
  capacity_[i] = n;
}

inline
StringC Sd::capacityName(int i) const
{
  return execToDoc(capacityName_[i]);
}

inline
Boolean Sd::scopeInstance() const
{
  return scopeInstance_;
}

inline
void Sd::setScopeInstance()
{
  scopeInstance_ = 1;
}

inline
void Sd::setDocCharsetDecl(CharsetDecl &decl)
{
  decl.swap(docCharsetDecl_);
}

inline
const CharsetDecl &Sd::docCharsetDecl() const
{
  return docCharsetDecl_;
}

inline
void Sd::setBooleanFeature(BooleanFeature i, Boolean b)
{
  booleanFeature_[i] = b;
}

inline
void Sd::setNumberFeature(NumberFeature i, Number n)
{
  numberFeature_[i] = n;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* Sd_INCLUDED */
