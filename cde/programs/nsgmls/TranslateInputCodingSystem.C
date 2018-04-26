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
/* $XConsortium: TranslateInputCodingSystem.C /main/1 1996/07/29 17:06:28 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "TranslateInputCodingSystem.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class TranslateDecoder : public Decoder {
public:
  TranslateDecoder(const Char *table);
  size_t decode(Char *to, const char *from, size_t fromLen,
		const char **rest);
  Boolean convertOffset(unsigned long &offset) const;
private:
  const Char *table_;
};

TranslateInputCodingSystem::TranslateInputCodingSystem(const Char *table)
: table_(table)
{
}

Decoder *TranslateInputCodingSystem::makeDecoder() const
{
  return new TranslateDecoder(table_);
}

TranslateDecoder::TranslateDecoder(const Char *table)
: table_(table)
{
}

size_t TranslateDecoder::decode(Char *to, const char *from, size_t fromLen,
				const char **rest)
{
  for (size_t n = fromLen; n > 0; n--)
    *to++ = table_[(unsigned char)*from++]; // zero extend
  *rest = from;
  return fromLen;
}

Boolean TranslateDecoder::convertOffset(unsigned long &) const
{
  return true;
}


#ifdef SP_NAMESPACE
}
#endif
