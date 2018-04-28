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
/* $XConsortium: ConsoleOutput.C /main/1 1996/07/29 16:48:10 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "ConsoleOutput.h"

#ifdef SP_WIDE_SYSTEM
#include <windows.h>
#include <io.h>
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef SP_WIDE_SYSTEM

class ConsoleOutputCharStream : public OutputCharStream {
public:
  ConsoleOutputCharStream(HANDLE h);
  void flush();
  void flushBuf(Char);
private:
  HANDLE h_;
};

OutputCharStream *ConsoleOutput::makeOutputCharStream(int fd)
{
  HANDLE h = (HANDLE)_get_osfhandle(fd);
  DWORD flags;
  if (GetConsoleMode(h, &flags))
    return new ConsoleOutputCharStream(h);
  else
    return 0;
}

ConsoleOutputCharStream::ConsoleOutputCharStream(HANDLE h)
: h_(h)
{
}

void ConsoleOutputCharStream::flush()
{
}

void ConsoleOutputCharStream::flushBuf(Char c)
{
  DWORD nWritten;
  unsigned short ch = c;
  WriteConsoleW(h_, &ch, 1, &nWritten, 0);
}

#else /* not SP_WIDE_SYSTEM */

OutputCharStream *ConsoleOutput::makeOutputCharStream(int)
{
  return 0;
}

#endif  /* not SP_WIDE_SYSTEM */

#ifdef SP_NAMESPACE
}
#endif
