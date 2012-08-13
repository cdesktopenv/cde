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
/* $XConsortium: OutputCharStream.h /main/1 1996/07/29 16:59:39 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef OutputCharStream_INCLUDED
#define OutputCharStream_INCLUDED 1

#include "types.h"
#include <stddef.h>
#include "StringC.h"
#include "Owner.h"
#include "CodingSystem.h"

#if defined(linux) || defined(CSRG_BASED)
#include <streambuf>
using namespace std;
#else
class streambuf;
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API OutputCharStream {
public:
  enum Newline { newline };
  typedef void (*Escaper)(OutputCharStream &, Char);
  OutputCharStream();
  virtual ~OutputCharStream();
  OutputCharStream &put(Char);
  OutputCharStream &write(const Char *, size_t);
  virtual void flush() = 0;
  void setEscaper(Escaper);

  OutputCharStream &operator<<(char);
  OutputCharStream &operator<<(const char *);
  OutputCharStream &operator<<(const StringC &);
  OutputCharStream &operator<<(unsigned long);
  OutputCharStream &operator<<(int);
  OutputCharStream &operator<<(Newline);
private:
  OutputCharStream(const OutputCharStream &);	// undefined
  void operator=(const OutputCharStream &);	// undefined

  virtual void flushBuf(Char) = 0;
  Escaper escaper_;
protected:
  void escape(OutputCharStream &, Char c);
  Char *ptr_;
  Char *end_;
};

class SP_API IosOutputCharStream : public OutputCharStream,
                            private Encoder::Handler {
public:
  IosOutputCharStream();
  // the streambuf will not be deleted
  IosOutputCharStream(streambuf *, const OutputCodingSystem *);
  ~IosOutputCharStream();
  void open(streambuf *, const OutputCodingSystem *);
  void flush();
private:
  IosOutputCharStream(const IosOutputCharStream &); // undefined
  void operator=(const IosOutputCharStream &);	    // undefined
  IosOutputCharStream(streambuf *, Encoder *);
  void allocBuf(int bytesPerChar);
  void flushBuf(Char);
  void handleUnencodable(Char c, streambuf *);
  Char *buf_;
  streambuf *byteStream_;
  Encoder *encoder_;
  Owner<Encoder> ownedEncoder_;
};

class SP_API StrOutputCharStream : public OutputCharStream {
public:
  StrOutputCharStream();
  ~StrOutputCharStream();
  void extractString(StringC &);
  void flush();
private:
  void flushBuf(Char);
  void sync(size_t);
  StrOutputCharStream(const StrOutputCharStream &); // undefined
  void operator=(const StrOutputCharStream &);	    // undefined
  Char *buf_;
  size_t bufSize_;
};

class SP_API RecordOutputCharStream : public OutputCharStream {
public:
  RecordOutputCharStream(OutputCharStream *);
  ~RecordOutputCharStream();
  void flush();
private:
  RecordOutputCharStream(const RecordOutputCharStream &); // undefined
  void operator=(const RecordOutputCharStream &);	  // undefined
  void flushBuf(Char);
  void outputBuf();

  OutputCharStream *os_;
  enum { bufSize_ = 1024 };
  Char buf_[bufSize_];
};

inline
OutputCharStream &OutputCharStream::put(Char c)
{
  if (ptr_ < end_)
    *ptr_++ = c;
  else
    flushBuf(c);
  return *this;
}

inline
OutputCharStream &OutputCharStream::operator<<(char c)
{
  return put(Char(c));
}

inline
OutputCharStream &OutputCharStream::operator<<(Newline)
{
#ifdef SP_HAVE_SETMODE
  put(Char('\r'));
#endif
  return put(Char('\n'));
}

inline
OutputCharStream &OutputCharStream::operator<<(const StringC &str)
{
  return write(str.data(), str.size());
}

inline
void OutputCharStream::setEscaper(Escaper f)
{
  escaper_ = f;
}

inline
void OutputCharStream::escape(OutputCharStream &s, Char c)
{
  if (escaper_)
    (*escaper_)(s, c);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not OutputCharStream_INCLUDED */
