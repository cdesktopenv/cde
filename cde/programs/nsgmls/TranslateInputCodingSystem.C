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
