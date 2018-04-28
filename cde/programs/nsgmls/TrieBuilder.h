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
/* $XConsortium: TrieBuilder.h /main/1 1996/07/29 17:06:48 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef TrieBuilder_INCLUDED
#define TrieBuilder_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "StringOf.h"
#include "Owner.h"
#include "Trie.h"
#include "Vector.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class TrieBuilder {
public:
  typedef Vector<Token> TokenVector;
  TrieBuilder(int nCodes);
  void recognize(const String<EquivCode> &chars,
		 Token t,
		 Priority::Type pri,
		 TokenVector &ambiguities);
  void recognize(const String<EquivCode> &chars,
		 const String<EquivCode> &set,
		 Token t,
		 Priority::Type pri,
		 TokenVector &ambiguities);
  // recognize a delimiter with a blank sequence
  void recognizeB(const String<EquivCode> &chars,
		  int bSequenceLength, // >= 1
		  size_t maxBlankSequenceLength,
		  const String<EquivCode> &blankCodes,
		  const String<EquivCode> &chars2,
		  Token t,
		  TokenVector &ambiguities);
  void recognizeEE(EquivCode code, Token t);
  Trie *extractTrie() { return root_.extract(); }
private:
  TrieBuilder(const TrieBuilder &); // undefined
  void operator=(const TrieBuilder &); // undefined
  void doB(Trie *trie,
	   int tokenLength,
	   int minBLength,
	   size_t maxLength,
	   const String<EquivCode> &blankCodes,
	   const String<EquivCode> &chars2,
	   Token token,
	   Priority::Type pri,
	   TokenVector &ambiguities);
  Trie *extendTrie(Trie *, const String<EquivCode> &);
  void setToken(Trie *trie, int tokenLength, Token token, Priority::Type pri,
		TokenVector &ambiguities);

  Trie *forceNext(Trie *trie, EquivCode);
  void copyInto(Trie *, const Trie *, int);

  int nCodes_;
  Owner<Trie> root_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not TrieBuilder_INCLUDED */
