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
/*
 * $XConsortium: MarkBase.cc /main/3 1996/06/11 16:29:11 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#define C_xList
#define L_Support

#define C_Mark
#define C_MarkBase
#define L_Marks

#include <Prelude.h>

#include <string.h>

static xList<MarkBase::open_func_t> *g_open_func_list;
static xList<MarkBase::mb_create_func_t> *g_create_func_list;
static xList<const char *> *g_create_format_list;

MarkBase::~MarkBase()
{
}


// /////////////////////////////////////////////////////////////////
// register a MarkBase creation function
// /////////////////////////////////////////////////////////////////

int
MarkBase::register_open_func (open_func_t open_func)
{
  ON_DEBUG (printf ("Registered MarkBase open func\n"));
  static xList<open_func_t> open_func_list;
  if (g_open_func_list == NULL)
    g_open_func_list = &open_func_list;

  open_func_list.append (open_func);
  return (0);
}

int
MarkBase::register_create_func (mb_create_func_t create_func, const char *format)
{
  ON_DEBUG (printf ("Registered `%s' creation func\n", format));
  static xList<mb_create_func_t> create_func_list;
  static xList<const char *> create_format_list;

  if (g_create_func_list == NULL)
    {
      g_create_func_list = &create_func_list;
      g_create_format_list = &create_format_list;
    }

  create_func_list.append (create_func);
  create_format_list.append (format);
  return (0);
}


// /////////////////////////////////////////////////////////////////
// create a new mark base by calling the registered functions
// /////////////////////////////////////////////////////////////////

MarkBase *
MarkBase::open (const char *filename, bool read_only)
{
  MarkBase *base = NULL;
  List_Iterator<open_func_t> open_func (g_open_func_list);

  while (open_func)
    {
      base = open_func.item() (filename, read_only);
      if (base != NULL)
	break;
      open_func++;
    }
  return (base);
}


MarkBase *
MarkBase::create (const char *filename, bool read_only, const char *format)
{
  MarkBase *base = NULL;
  List_Iterator<const char *> fmt (g_create_format_list);
  List_Iterator<mb_create_func_t> create_func (g_create_func_list);

  // Find a matching format creation function.
  while (fmt)
    {
      if (strcmp (fmt.item(), format) == 0)
	break;
      fmt++;
      create_func++;
    }

  // Create the base if a matching format was found. 
  if (fmt)
    base = create_func.item() (filename, read_only);

  return (base);
}
