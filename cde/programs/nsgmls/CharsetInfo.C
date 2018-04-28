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
/* $XConsortium: CharsetInfo.C /main/1 1996/07/29 16:47:17 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "CharsetInfo.h"
#include "ISet.h"
#include "constant.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

CharsetInfo::CharsetInfo(const UnivCharsetDesc &desc)
: desc_(desc)
{
  // FIXME remove mappings from desc for characters greater charMax
  init();
}

CharsetInfo::CharsetInfo()
{
  for (size_t i = 0; i < nSmall; i++) {
    smallUnivValid_[i] = 0;
    smallDescValid_[i] = 0;
  }
}

void CharsetInfo::set(const UnivCharsetDesc &desc)
{
  desc_ = desc;
  init();
}

void CharsetInfo::init()
{
  size_t i;
  for (i = 0; i < nSmall; i++) {
    smallUnivValid_[i] = 0;
    smallDescValid_[i] = 0;
  }

  UnivCharsetDescIter iter(desc_);
  
  WideChar descMin, descMax;
  UnivChar univMin;
  while (iter.next(descMin, descMax, univMin)) {
    WideChar j = descMin;
    do {
      UnivChar k = univMin + (j - descMin);
      if (k >= nSmall)
	break;
      if (!smallUnivValid_[k]) {
	smallUnivValid_[k] = 1;
	smallUnivToDesc_[k] = j;
      }
      else
	smallUnivValid_[k] = 2;
    } while (j++ != descMax);
    j = descMin;
    do {
      if (j >= nSmall)
	break;
      if (!smallDescValid_[j]) {
	smallDescValid_[j] = 1;
	smallDescToUniv_[j] = univMin + (j - descMin);
      }
    } while (j++ != descMax);
  }
  // These are the characters that the ANSI C
  // standard guarantees will be in the basic execution
  // character set.
  static char execChars[] =
    "\t\n\r "
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!\"#%&'()*+,-./:"
    ";<=>?[\\]^_{|}~";
  // These are the corresponding ISO 646 codes.
  static char univCodes[] = {
    9, 10, 13, 32,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
    78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
    33, 34, 35, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 58,
    59, 60, 61, 62, 63, 91, 92, 93, 94, 95, 123, 124, 125, 126,
  };
  for (i = 0; execChars[i] != '\0'; i++) {
    WideChar c;
    ISet<WideChar> set;
    if (univToDesc(univCodes[i], c, set) > 0 && c <= Char(-1))
      execToDesc_[(unsigned char)execChars[i]] = Char(c);
  }
}

void CharsetInfo::getDescSet(ISet<Char> &set) const
{
  UnivCharsetDescIter iter(desc_);
  WideChar descMin, descMax;
  UnivChar univMin;
  while (iter.next(descMin, descMax, univMin)) {
    if (descMin > charMax)
      break;
    if (descMax > charMax)
      descMax = charMax;
    set.addRange(Char(descMin), Char(descMax));
  }
}

int CharsetInfo::digitWeight(Char c) const
{
  for (int i = 0; i < 10; i++)
    if (c == execToDesc('0' + i))
      return i;
  return -1;
}

StringC CharsetInfo::execToDesc(const char *s) const
{
  StringC result;
  while (*s != '\0')
    result += execToDesc(*s++);
  return result;
}

#ifdef SP_NAMESPACE
}
#endif
