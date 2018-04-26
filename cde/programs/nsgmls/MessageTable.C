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
/* $XConsortium: MessageTable.C /main/1 1996/07/29 16:57:45 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "MessageTable.h"

#ifdef SP_NO_MESSAGE_TEXT

// Windows only

#define STRICT
#include "windows.h"
static HINSTANCE dllInstanceHandle = NULL;

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class WinMessageTable : public MessageTable {
public:
  Boolean getText(const MessageFragment &,
		  String<SP_TCHAR> &) const;
  
};

Boolean WinMessageTable::getText(const MessageFragment &frag,
				 String<SP_TCHAR> &str) const
{
  static const int bufSize = 4096;
  SP_TCHAR buf[bufSize];
  int len = LoadString(frag.module() == MessageFragment::libModule
		       ? dllInstanceHandle
		       : 0,
		       frag.number(),
		       buf,
		       bufSize);
  if (len == 0) {
    // The resource might be empty.
    if (GetLastError() != 0)
      return 0;
    else {
      str.resize(0);
      return 1;
    }
  }
  str.assign(buf, len);
  return 1;
}

const MessageTable *MessageTable::instance()
{
  if (!instance_)
    instance_ = new WinMessageTable;
  return instance_;
}

#ifdef SP_NAMESPACE
}
#endif

#ifdef SP_USE_DLL
extern "C"
BOOL WINAPI DllMain(HINSTANCE inst, ULONG reason, LPVOID)
{
  if (reason == DLL_PROCESS_ATTACH)
    dllInstanceHandle = inst;
  return TRUE;
}
#endif

#else /* not SP_NO_MESSAGE_TEXT */

#ifdef SP_HAVE_GETTEXT
extern "C" {
extern char *dgettext(const char *, const char *);
extern char *gettext(const char *);
extern char *textdomain(const char *);
extern char *bindtextdomain(const char *, const char *);
}

#include <stdlib.h>

#ifndef MESSAGE_DOMAIN
#define MESSAGE_DOMAIN "sp"
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class GettextMessageTable : public MessageTable {
public:
  GettextMessageTable();
  Boolean getText(const MessageFragment &, String<SP_TCHAR> &) const;
  
};

GettextMessageTable::GettextMessageTable()
{
  const char *dir = getenv("TEXTDOMAINDIR");
  if (dir)
    bindtextdomain(MESSAGE_DOMAIN, dir);
}

Boolean GettextMessageTable::getText(const MessageFragment &frag,
				     String<SP_TCHAR> &str) const
{
  const char *s = frag.text();
  if (!s)
    return 0;
  s = dgettext(MESSAGE_DOMAIN, s);
  if (!s)
    return 0;
  str.assign(s, strlen(s));
  return 1;
}

const MessageTable *MessageTable::instance()
{
  if (!instance_)
    instance_ = new GettextMessageTable;
  return instance_;
}

#ifdef SP_NAMESPACE
}
#endif

#else /* not SP_HAVE_GETTEXT */

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class DefaultMessageTable : public MessageTable {
public:
  Boolean getText(const MessageFragment &, String<SP_TCHAR> &) const;
};

Boolean DefaultMessageTable::getText(const MessageFragment &frag,
				     String<SP_TCHAR> &str) const
{
  if (!frag.text())
    return 0;
  str.resize(0);
  for (const char *s = frag.text(); *s; s++)
    str += SP_TCHAR((unsigned char)*s);
  return 1;
}

const MessageTable *MessageTable::instance()
{
  if (!instance_)
    instance_ = new DefaultMessageTable;
  return instance_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SP_HAVE_GETTEXT */

#endif /* not SP_NO_MESSAGE_TEXT */

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

MessageTable *MessageTable::instance_ = 0;

#ifdef SP_NAMESPACE
}
#endif
