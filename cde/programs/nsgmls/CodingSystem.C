/* $XConsortium: CodingSystem.C /main/2 1996/08/08 12:10:51 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "CodingSystem.h"
#ifdef SP_SHORT_HEADERS
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <string.h>
#include <sys/param.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

InputCodingSystem::~InputCodingSystem()
{
}

StringC InputCodingSystem::convertIn(const char *s) const
{
  Decoder *decoder = makeDecoder();
  StringC str;
  str.resize(strlen(s));
  str.resize(decoder->decode(&str[0], s, strlen(s), &s));
  delete decoder;
  return str;
}

Boolean InputCodingSystem::isIdentity() const
{
  return 0;
}

OutputCodingSystem::~OutputCodingSystem()
{
}

unsigned OutputCodingSystem::fixedBytesPerChar() const
{
  return 0;
}

String<char> OutputCodingSystem::convertOut(const StringC &str) const
{
  Encoder *encoder = makeEncoder();
  strstreambuf stream(MAXPATHLEN);
  StringC copy(str);
  encoder->output(copy.data(), copy.size(), &stream);
  delete encoder;
  char *s = stream.str();
  String<char> result(s, stream.out_waiting());
  result += '\0';
  stream.freeze(0);
#ifdef __lucid
  // Workaround lcc bug (3.1p2 with -O -XF).
  String<char> temp(result);
  return temp;
#else
  return result;
#endif
}

Decoder::Decoder(unsigned minBytesPerChar)
: minBytesPerChar_(minBytesPerChar)
{
}

Decoder::~Decoder()
{
}

Boolean Decoder::convertOffset(unsigned long &) const
{
  return false;
}

Encoder::Encoder()
: unencodableHandler_(0)
{
}

Encoder::~Encoder()
{
}

void Encoder::output(Char *s, size_t n, streambuf *sp)
{
  output((const Char *)s, n, sp);
}

void Encoder::startFile(streambuf *)
{
}

#ifdef SP_NAMESPACE
}
#endif
