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
/* $XConsortium: replace.h /main/3 1996/06/19 17:17:05 drk $ */
/* replace.h
   Interface to replacement file parser. */

enum replacement_type {
  DATA_REPL,
  ATTR_REPL
  };
  
struct replacement_item {
  union {
    char *attr;
    struct {
      char *s;
      unsigned n;
    } data;
  } u;
  enum replacement_type type;
  struct replacement_item *next;
};

#define NEWLINE_BEGIN 01
#define NEWLINE_END 02

struct replacement {
  unsigned flags;
  struct replacement_item *items;
};

enum event_type { START_ELEMENT, END_ELEMENT };

struct replacement_table *make_replacement_table P((void));
void load_replacement_file P((struct replacement_table *, char *));
  
struct replacement *
lookup_replacement P((struct replacement_table *, enum event_type, char *));
