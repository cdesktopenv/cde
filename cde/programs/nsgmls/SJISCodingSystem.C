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
/* $XConsortium: SJISCodingSystem.C /main/1 1996/07/29 17:03:20 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"

#ifdef SP_MULTI_BYTE

#include "SJISCodingSystem.h"

#if defined(linux) || defined(CSRG_BASED)
#include <iostream>
#else
#include <iostream.h>
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SJISDecoder : public Decoder {
public:
  SJISDecoder();
  size_t decode(Char *, const char *, size_t, const char **);
private:
};

class SJISEncoder : public Encoder {
public:
  SJISEncoder();
  void output(const Char *, size_t, streambuf *);
};

Decoder *SJISCodingSystem::makeDecoder() const
{
  return new SJISDecoder;
}

Encoder *SJISCodingSystem::makeEncoder() const
{
  return new SJISEncoder;
}


SJISDecoder::SJISDecoder()
{
}

size_t SJISDecoder::decode(Char *to, const char *s,
			   size_t slen, const char **rest)
{
  Char *start = to;
  while (slen > 0) {
    unsigned char c = *(unsigned char *)s;
    if (!(c & 0x80)) {
      *to++ = c;
      s++;
      slen--;
    }
    else if (129 <= c && c <= 159) {
      if (slen < 2)
	break;
      s++;
      slen -= 2;
      unsigned char c2 = *(unsigned char *)s++;
      unsigned short n = ((c - 112) << 9) | c2;
      if (64 <= c2 && c2 <= 127)
	n -= 31 + (1 << 8);
      else if (c2 <= 158)
	n -= 32 + (1 << 8);
      else if (c2 <= 252)
	n -= 126;
      else
	continue;
      n |= 0x8080;
      *to++ = n;
    }
    else if (224 <= c && c <= 239) {
      if (slen < 2)
	break;
      s++;
      slen -= 2;
      unsigned char c2 = *(unsigned char *)s++;
      unsigned short n = ((c - 176) << 9) | c2;
      if (64 <= c2 && c2 <= 127)
	n -= 31 + (1 << 8);
      else if (c2 <= 158)
	n -= 32 + (1 << 8);
      else if (c2 <= 252)
	n -= 126;
      else
	continue;
      n |= 0x8080;
      *to++ = n;
    }
    else if (161 <= c && c <= 223) {
      slen--;
      s++;
      *to++ = c;
    }
    else {
      // 128, 160, 240-255
      slen--;
      s++;
    }
  }
  *rest = s;
  return to - start;
}

SJISEncoder::SJISEncoder()
{
}

// FIXME handle errors from streambuf::sputc

void SJISEncoder::output(const Char *s, size_t n, streambuf *sb)
{
  for (; n > 0; s++, n--) {
    Char c = *s;
    unsigned short mask = (unsigned short)(c & 0x8080);
    if (mask == 0)
      sb->sputc(char(c & 0xff));
    else if (mask == 0x8080) {
      unsigned char c1 = (c >> 8) & 0x7f;
      unsigned char c2 = c & 0x7f;
      char out1;
      if (c1 < 33)
	out1 = 0;
      else if (c1 < 95)
	out1 = ((c1 + 1) >> 1) + 112;
      else if (c1 < 127)
	out1 = ((c1 + 1) >> 1) + 176;
      else
	out1 = 0;
      if (out1) {
	char out2;
	if (c1 & 1) {
	  if (c2 < 33)
	    out2 = 0;
	  else if (c2 <= 95)
	    out2 = c2 + 31;
	  else if (c2 <= 126)
	    out2 = c2 + 32;
	  else
	    out2 = 0;
	}
	else {
	  if (33 <= c2 && c2 <= 126)
	    out2 = c2 + 126;
	  else
	    out2 = 0;
	}
	if (out2) {
	  sb->sputc(out1);
	  sb->sputc(out2);
	}
	else
	  handleUnencodable(c, sb);
      }
      else
	handleUnencodable(c, sb);
    }
    else if (mask == 0x0080) {
      if (161 <= c && c <= 223)
	sb->sputc(char(c & 0xff));
      else
	handleUnencodable(c, sb);
    }
    else
      handleUnencodable(c, sb);
  }
}

#ifdef SP_NAMESPACE
}
#endif

#else /* not SP_MULTI_BYTE */

#ifndef __GNUG__
static char non_empty_translation_unit;	// sigh
#endif

#endif /* not SP_MULTI_BYTE */
