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
// $XConsortium: MMDB_Library.C /main/9 1996/09/14 13:10:52 cde-hal $

# include "MMDB_Library.hh"
# include "MMDB_BookCase.hh"
# include "MMDB_Factory.hh"

#ifdef DTSEARCH
# include "../DtSR/DtSR_SearchEngine.hh"
#else
# include "../Fulcrum/Fulcrum_SearchEngine.hh"
#endif

#include "Support/UtilityFunctions.hh"

#include <dirent.h>

#include "Managers/CatMgr.hh"
#include "Registration.hh"

// fSearchEngine is shared among all MMDB_Library instances
UAS_Pointer<UAS_SearchEngine> MMDB_Library::fSearchEngine = NULL;
int MMDB_Library::f_mmdbs = 0;

MMDB_Library::MMDB_Library (MMDB &theMMDB): MMDB_Common (theMMDB)
{
    ++f_mmdbs;
    f_name = UAS_String();
}

MMDB_Library::~MMDB_Library()
{
    if (--f_mmdbs == 0)
	fSearchEngine = NULL;
}

UAS_String
MMDB_Library::title (UAS_TitleType) {
    info_lib *il = mmdb().infolib();
    UAS_String rval = il->get_info_lib_name();
    if (rval == "")
    {
      rval = CATGETS(Set_UAS_MMDB, 5, "File a Bug");
      rval = rval + mmdb().infoLibPath();
    }
    return rval;
}

UAS_String
MMDB_Library::locator () {
    MMDB_URL rval;
    rval.fType = "collection";
    rval.fInfoLib = mmdb().infoLibPath();
    return rval.locator();
}

UAS_String
MMDB_Library::id () {
    return mmdb().infoLibPath();
}

UAS_String
MMDB_Library::lid () {
    info_lib &il = *(mmdb().infolib());
    return UAS_String(il.get_info_lib_uid());
}
 
UAS_String
MMDB_Library::bid () {
    return UAS_String();
}

UAS_String
MMDB_Library::name () {
  if (f_name == "")
  {
    DIR *dir;
    struct dirent *sdir;
    dir = opendir((const char*)id()); 
    while((sdir = readdir(dir)) != NULL)
    {
       char *p;
       p = strstr(sdir->d_name, ".oli");
       if (p != NULL)
       {
         *p = '\0';
         f_name = UAS_String(sdir->d_name);
         break;
       }
    }
    closedir(dir);

    // check to see if found .oli file. if not,
    // use the name of the first bookcase as the
    // name of the infolib.
    if (f_name == "")
    {
      UAS_List<UAS_Common> kids = children();
      unsigned int i;
      for (i = 0; i < kids.length(); i++)
      {
        if (kids[i]->type() == UAS_BOOKCASE)
          break;
      }
      if (i < kids.length())
      { 
        char scratch[128];
        snprintf(scratch, sizeof(scratch),
                 CATGETS(Set_UAS_MMDB, 6, "File a Bug"), (char *)kids[i]->id());
        f_name = UAS_String(scratch);
      }
    }
  }

  return f_name;
}

UAS_ObjectType
MMDB_Library::type () {
    return UAS_LIBRARY;
}

UAS_List<UAS_Common>
MMDB_Library::children () {
    UAS_List<UAS_Common> theList;

    info_lib &il = *(mmdb().infolib());
    for (long key = il.first (); key != 0; il.next(key)) {
        info_base *curBase = il(key);
	MMDB_BookCase *newEl = new MMDB_BookCase (mmdb (), curBase);
        theList.insert_item (newEl);
    }
    return theList;
}

UAS_Pointer<UAS_SearchEngine>
MMDB_Library::search_engine() {

  UAS_List<UAS_Common> bookcases = bookcaseList();
  UAS_PtrList<const char> cList;

  unsigned int i;
#ifdef DEBUG
  for (i = 0; i < bookcases.length(); i++)
      fprintf(stderr, "(DEBUG) bookcases %d=%s\n",
					i, (char*)bookcases[i]->id());
#endif

  UAS_List<UAS_String> strings;
  for (i = 0; i < bookcases.length(); i++) {
      UAS_Pointer<UAS_String> string = new UAS_String;
      strings.insert_item(string);
  }

  for (i = 0; i < bookcases.length(); i++)
  {
      UAS_Pointer<UAS_Common> infolib = bookcases[i]->parent();

      UAS_String str = infolib->id();
      str = str + "/" + bookcases[i]->id();

      *(UAS_String *)strings[i] = str;
      cList.append((char*)(*(UAS_String *)strings[i]));
  }

#ifdef DTSEARCH
  fSearchEngine = &(DtSR_SearchEngine::search_engine (&cList));
#else
  fSearchEngine = &(Fulcrum_SearchEngine::search_engine (&cList));
#endif

  return fSearchEngine;
}
