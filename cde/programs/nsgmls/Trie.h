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
/* $XConsortium: Trie.h /main/1 1996/07/29 17:06:38 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Trie_INCLUDED
#define Trie_INCLUDED 1

#include <limits.h>
#include "types.h"
#include "Boolean.h"
#include "Vector.h"
#include "CopyOwner.h"
#include "Priority.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class BlankTrie;

class Trie {
public:
  Trie() : next_(0), nCodes_(0), token_(0), tokenLength_(0), priority_(Priority::data) { }
  Trie(const Trie &);
  ~Trie();
  Trie &operator=(const Trie &);
  const Trie *next(int i) const { return &next_[i]; }
  Boolean hasNext() const { return next_ != 0; }
  Token token() const { return token_; }
  int tokenLength() const { return tokenLength_; }
  const BlankTrie *blank() const;
  Boolean includeBlanks() const {
    return Priority::isBlank(priority_);
  }
  friend class TrieBuilder;
private:
  Trie *next_;
  int nCodes_;
  unsigned short token_;
  unsigned char tokenLength_;
  Priority::Type priority_;
  CopyOwner<BlankTrie> blank_;
};

class BlankTrie : public Trie {
public:
  BlankTrie() : additionalLength_(0), maxBlanksToScan_(0) { }
  Boolean codeIsBlank(EquivCode c) const { return codeIsBlank_[c]; }
  // maximum number of blanks to scan (minimum is 0)
  size_t maxBlanksToScan() const { return maxBlanksToScan_; }
  // length to add to tokenLengths in this trie (for those > 0).
  int additionalLength() const { return additionalLength_; }
  BlankTrie *copy() const { return new BlankTrie(*this); }
private:
  unsigned char additionalLength_;
  size_t maxBlanksToScan_;
  Vector<PackedBoolean> codeIsBlank_;
  friend class TrieBuilder;
};

inline
const BlankTrie *Trie::blank() const
{
  return blank_.pointer();
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Trie_INCLUDED */
