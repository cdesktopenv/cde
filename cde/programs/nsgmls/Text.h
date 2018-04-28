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
/* $XConsortium: Text.h /main/1 1996/07/29 17:06:13 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Text_INCLUDED
#define Text_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "StringC.h"
#include "Vector.h"
#include "Location.h"
#include "SubstTable.h"
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class InternalEntity;

struct SP_API TextItem {
  TextItem();
  enum Type {
    data,
    cdata,
    sdata,
    entityStart,
    entityEnd,
    startDelim,
    endDelim,
    endDelimA,
    ignore
    };
  Type type;
  // char that was ignored
  Char c;
  // location of this item
  // data - location of first char
  // (c/sdata)entityStart - location of first char of entity
  // (c/sdata)entityEnd - location of entity end in entity
  // ignore - location of ignored character
  // startDelim - location of first char of delimiter
  // endDelim(A) - location of first char of delimiter
  Location loc;
  // index of character in chars_ to which this applies
  size_t index;
};

// This is used to represent literals and attribute values.

class SP_API Text {
public:
  Text();
  void clear();
  void swap(Text &to);
  void addChar(Char c, const Location &);
  void addChars(const StringC &, const Location &);
  void addChars(const Char *, size_t, const Location &);
  void insertChars(const StringC &, const Location &);
  void ignoreChar(Char, const Location &);
  void ignoreLastChar();
  void addEntityStart(const Location &);
  void addEntityEnd(const Location &);
  void addCdata(const InternalEntity *, const ConstPtr<Origin> &);
  void addSdata(const InternalEntity *, const ConstPtr<Origin> &);
  void addStartDelim(const Location &loc);
  void addEndDelim(const Location &loc, Boolean lita);
  void subst(const SubstTable<Char> &, Char space);
  void addCharsTokenize(const Char *, size_t, const Location &loc, Char space);
  void addCharsTokenize(const StringC &, const Location &loc, Char space);
  void tokenize(Char space, Text &text) const;
  Location charLocation(size_t i) const;
  size_t size() const;
  Char lastChar() const;
  const StringC &string() const;
  size_t nDataEntities() const;
  Boolean fixedEqual(const Text &) const;
  // Location of first char of start delimiter.
  Boolean startDelimLocation(Location &) const;
  // Location of first char of end delimiter
  Boolean endDelimLocation(Location &) const;
  // Is delimiter a lit or lita?
  Boolean delimType(Boolean &lita) const;
private:
  void addSimple(TextItem::Type, const Location &);
  StringC chars_;
  Vector<TextItem> items_;
  friend class TextIter;
};

class SP_API TextIter {
public:
  TextIter(const Text &);
  void rewind();
  Boolean next(TextItem::Type &, const Char *&, size_t &,
	       const Location *&);
  // Alternative interface to next()
  Boolean valid() const;
  void advance();
  TextItem::Type type() const;
  const Location &location() const;
  const Char *chars(size_t &length) const;
private:
  const TextItem *ptr_;
  const Text *text_;
};

inline
size_t Text::size() const
{
  return chars_.size();
}

inline
Char Text::lastChar() const
{
  return chars_[chars_.size() - 1];
}

inline
const StringC &Text::string() const
{
  return chars_;
}

inline
void Text::addEntityStart(const Location &loc)
{
  addSimple(TextItem::entityStart, loc);
}

inline
void Text::addEntityEnd(const Location &loc)
{
  addSimple(TextItem::entityEnd, loc);
}

inline
void Text::addChars(const StringC &s, const Location &loc)
{
  addChars(s.data(), s.size(), loc);
}

inline
void Text::addStartDelim(const Location &loc)
{
  addSimple(TextItem::startDelim, loc);
}

inline
void Text::addEndDelim(const Location &loc, Boolean lita)
{
  addSimple(lita ? TextItem::endDelimA : TextItem::endDelim,
	    loc);
}

inline
void Text::addCharsTokenize(const StringC &str, const Location &loc,
			    Char space)
{
  addCharsTokenize(str.data(), str.size(), loc, space);
}

inline
void TextIter::rewind()
{
  ptr_ = text_->items_.begin();
}

inline
void TextIter::advance()
{
  ptr_++;
}

inline
Boolean TextIter::valid() const
{
  return ptr_ != (text_->items_.begin() + text_->items_.size());
}

inline
const Location &TextIter::location() const
{
  return ptr_->loc;
}

inline
TextItem::Type TextIter::type() const
{
  return ptr_->type;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Text_INCLUDED */
