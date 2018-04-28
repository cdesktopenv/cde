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
/* $XConsortium: Sd.C /main/1 1996/07/29 17:03:39 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "Sd.h"
#include "macros.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Sd::Sd()
: scopeInstance_(0)
{
  int i;
  for (i = 0; i < nBooleanFeature; i++)
    booleanFeature_[i] = 0;
  for (i = 0; i < nNumberFeature; i++)
    numberFeature_[i] = 0;
  for (i = 0; i < nCapacity; i++)
    capacity_[i] = 35000;
}

void Sd::setDocCharsetDesc(const UnivCharsetDesc &desc)
{
  docCharset_.set(desc);
}

const char *const Sd::reservedName_[] = {
  "APPINFO",
  "BASESET",
  "CAPACITY",
  "CHARSET",
  "CONCUR",
  "CONTROLS",
  "DATATAG",
  "DELIM",
  "DESCSET",
  "DOCUMENT",
  "ENTITY",
  "EXPLICIT",
  "FEATURES",
  "FORMAL",
  "FUNCHAR",
  "FUNCTION",
  "GENERAL",
  "IMPLICIT",
  "INSTANCE",
  "LCNMCHAR",
  "LCNMSTRT",
  "LINK",
  "MINIMIZE",
  "MSICHAR",
  "MSOCHAR",
  "MSSCHAR",
  "NAMECASE",
  "NAMES",
  "NAMING",
  "NO",
  "NONE",
  "OMITTAG",
  "OTHER",
  "PUBLIC",
  "QUANTITY",
  "RANK",
  "RE",
  "RS",
  "SCOPE",
  "SEPCHAR",
  "SGML",
  "SGMLREF",
  "SHORTREF",
  "SHORTTAG",
  "SHUNCHAR",
  "SIMPLE",
  "SPACE",
  "SUBDOC",
  "SWITCHES",
  "SYNTAX",
  "UCNMCHAR",
  "UCNMSTRT",
  "UNUSED",
  "YES"
};

const char *const Sd::capacityName_[] = {
  "TOTALCAP",
  "ENTCAP",
  "ENTCHCAP",
  "ELEMCAP",
  "GRPCAP",
  "EXGRPCAP",
  "EXNMCAP",
  "ATTCAP",
  "ATTCHCAP",
  "AVGRPCAP",
  "NOTCAP",
  "NOTCHCAP",
  "IDCAP",
  "IDREFCAP",
  "MAPCAP",
  "LKSETCAP",
  "LKNMCAP"
};


const char *const Sd::quantityName_[] = {
  "ATTCNT",
  "ATTSPLEN",
  "BSEQLEN",
  "DTAGLEN",
  "DTEMPLEN",
  "ENTLVL",
  "GRPCNT",
  "GRPGTCNT",
  "GRPLVL",
  "LITLEN",
  "NAMELEN",
  "NORMSEP",
  "PILEN",
  "TAGLEN",
  "TAGLVL"
};

const char *const Sd::generalDelimiterName_[] = {
  "AND",
  "COM",
  "CRO",
  "DSC",
  "DSO",
  "DTGC",
  "DTGO",
  "ERO",
  "ETAGO",
  "GRPC",
  "GRPO",
  "LIT",
  "LITA",
  "MDC",
  "MDO",
  "MINUS",
  "MSC",
  "NET",
  "OPT",
  "OR",
  "PERO",
  "PIC",
  "PIO",
  "PLUS",
  "REFC",
  "REP",
  "RNI",
  "SEQ",
  "STAGO",
  "TAGC",
  "VI"
};

Boolean Sd::lookupQuantityName(const StringC &name, Syntax::Quantity &quantity)
     const
{
  for (size_t i = 0; i < SIZEOF(quantityName_); i++)
    if (execToDoc(quantityName_[i]) == name) {
      quantity = Syntax::Quantity(i);
      return 1;
    }
  return 0;
}

Boolean Sd::lookupCapacityName(const StringC &name, Sd::Capacity &capacity)
     const
{
  for (size_t i = 0; i < SIZEOF(capacityName_); i++)
    if (execToDoc(capacityName_[i]) == name) {
      capacity = Sd::Capacity(i);
      return 1;
    }
  return 0;
}

Boolean Sd::lookupGeneralDelimiterName(const StringC &name,
				       Syntax::DelimGeneral &delimGeneral)
     const
{
  for (size_t i = 0; i < SIZEOF(generalDelimiterName_); i++)
    if (execToDoc(generalDelimiterName_[i]) == name) {
      delimGeneral = Syntax::DelimGeneral(i);
      return 1;
    }
  return 0;
}

StringC Sd::quantityName(Syntax::Quantity q) const
{
  return execToDoc(quantityName_[q]);
}

StringC Sd::generalDelimiterName(Syntax::DelimGeneral d) const
{
  return execToDoc(generalDelimiterName_[d]);
}

UnivChar Sd::nameToUniv(const StringC &name)
{
  const int *p = namedCharTable_.lookup(name);
  int n;
  if (p)
    n = *p;
  else {
    n = int(namedCharTable_.count());
    namedCharTable_.insert(name, n);
  }
  return n + 0x60000000;	// 10646 private use group
}

#ifdef SP_NAMESPACE
}
#endif
