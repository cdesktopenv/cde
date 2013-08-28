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
// $XConsortium: UAS_SearchScope.C /main/6 1996/09/14 13:10:00 cde-hal $
#include "UAS_Exceptions.hh"
#include "UAS_SearchScope.hh"
#include "UAS_Collection.hh"

#include <string.h>
#include <iostream>
#include <sstream>
using namespace std;

#define CLASS UAS_SearchScope
STATIC_SENDER_CC (ScopeCreated);
STATIC_SENDER_CC (ScopeDeleted);
STATIC_SENDER_CC (ScopeRenamed);

UAS_SearchScope::UAS_SearchScope (const char *name,
                                  UAS_PtrList<UAS_BookcaseEntry> &bcases,
                                  u_int component_mask, bool ro)
: f_name (NULL),
  f_deleted (0),
  f_read_only (ro),
  f_infolib(NULL)
{
  int len = strlen(name);
  f_name = new char[len + 1];
  *((char *) memcpy(f_name, name, len) + len) = '\0';

  f_bcases = bcases;

  f_search_zones.zones(component_mask);

  static ScopeCreated create_msg;
  create_msg.f_search_scope = this;
  send_message (create_msg, 0);
}

UAS_SearchScope::~UAS_SearchScope()
{
  static ScopeDeleted delete_msg;
  delete_msg.f_search_scope = this;
  send_message (delete_msg, 0);
  delete f_name;
  for (int i = 0; i < f_bcases.numItems(); i++)
    delete f_bcases[i]; 

  f_infolib = NULL;
}

void
UAS_SearchScope::bookcases(UAS_PtrList<UAS_BookcaseEntry> &bcases)
{
  // delete existing bookcases before inserting new set.
  for (int i = 0; i < f_bcases.numItems(); i++)
    delete f_bcases[i]; 

  f_bcases = bcases;
}

const char *
UAS_SearchScope::name(const char *newname)
{
  delete f_name ;
  u_int len = strlen(newname);
  f_name = new char[len + 1] ;
  *((char *) memcpy(f_name, newname, len) + len) = '\0';
  return name();
}

void
UAS_SearchScope::set_name (const char *name)
{
  delete f_name;
  int len = strlen(name);
  f_name = new char[len + 1];
  *((char *) memcpy(f_name, name, len) + len) = '\0';
  static ScopeRenamed rename_msg;
  rename_msg.f_search_scope = this;
  send_message (rename_msg, 0);
}

#ifdef DEBUG
void
UAS_SearchScope::dump()
{
  UAS_BookcaseEntry *bce;

  cout << "Scope: " << f_name << endl;
  for (int i = 0; i < f_bcases.numItems(); i++) {
      bce = f_bcases[i];
      cout << "  Bookcase:     " << bce->name() << endl;
      if (bce->book_list().numItems()) {
          cout << "  Book Numbers: ";
          for (int j = 0; j < bce->book_list().numItems(); j++)
              cout << (bce->book_list())[j] << " ";
          cout << "\n";
      }
  }
  cout << "  Search Zones: ";

  if (f_search_zones.all())
      cout << "All Components\n";
  else {
      if (f_search_zones.titles())
          cout << "Titles ";
      if (f_search_zones.bodies())
          cout << "Body ";
      if (f_search_zones.examples())
          cout << "Examples ";
      if (f_search_zones.indexes())
          cout << "Index ";
      if (f_search_zones.tables())
          cout << "Tables ";
      if (f_search_zones.graphics())
          cout << "Graphics";
      cout << "\n";
  }
}
#endif
