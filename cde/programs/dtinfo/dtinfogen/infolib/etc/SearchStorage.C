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
/* $XConsortium: SearchStorage.cc /main/4 1996/07/23 18:10:39 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <sstream>
using namespace std;

/* imported interfaces */
#include <misc/unique_id.h>
#include "FlexBuffer.h"
#include "Task.h"
#include "DataBase.h"
#include "api/utility.h"

/* exported interfaces */
#include "SearchStorage.h"

//-----------------------------------------------------------------------
static int isdir(const char* filename)
{
  int ret = 0;
  struct stat sb;

  if(stat(filename, &sb) == 0){
    if(S_ISDIR(sb.st_mode)){
      ret = 1;
    }
  }

  return ret;
}

//-----------------------------------------------------------------------
static void makedir(const char *path) /* throw(PosixError) */
{
  if(mkdir((char*)path, 0775) != 0){
    throw(PosixError(errno, path));
  }
}

//-----------------------------------------------------------------------
static const char *FilteredString( FlexBuffer &f_buf, const char *str )
{
  for ( const char *ptr = str; *ptr != 0; ptr++ ) {
    if ( *ptr == '\"' ) {
      f_buf.writeStr ( "\\\"" );
    }
    else if ( *ptr == '\\' ) {
      f_buf.writeStr ( "\\\\" );
    }
    else {
      f_buf.put ( *ptr );
    }
  }

  return ( f_buf.GetBuffer() );
}

//-----------------------------------------------------------------------
SearchStorage::SearchStorage( const char *path, const char *name )
{
  if ( !isdir(path) ) {
    makedir(path);
  }

  filteredPath = new char [ strlen(path) + 1 + strlen("filtered") + 1 ];
  /*
   * throw(ResourceExhausted)
   *
   */
  assert ( filteredPath != NULL );
  
  sprintf( filteredPath, "%s/filtered", path );
  
  if ( !isdir(filteredPath) ) {
    makedir(filteredPath);
  }

  /* construct the fulcrum path */
  char *fulcrumpath = form("%s/fulcrum", path );

  if ( !isdir(fulcrumpath) ) {
    makedir(fulcrumpath);
  }

  char *catalogname = form("%s/%s.cin", fulcrumpath, name );

  catalogfile = fopen ( catalogname, "w" );
  if ( !catalogfile ) {
    throw(PosixError(errno, form("unable to open catalog %s\n", catalogname) ) );
  }
}

//-----------------------------------------------------------------------
void
SearchStorage::insert( const char *BookCaseName,
		       const int   BookNum,
		       const char *BookShortTitle,
		       const char *SectionID,
		       const char *SectionTitle,
		       const char *buffer,
		       int         size
		       )
{

  // write the search buffer to a file with name "NodeLocator" under
  // fulcrumpath

  // we use a unique_id() as the file name, in order to avoid spaces, /
  // and stuffs like that.....

  const char *filtered_file_name = unique_id();

  char *filename = form("%s/%s", filteredPath, filtered_file_name );
  FILE *fp = fopen( filename, "w" );
  if ( !fp ) {
    throw(PosixError(errno, form("Unable to open %s", filename) ) );
  }

  if ( !fwrite ( buffer, size, 1, fp ) ) {
    throw(PosixError(errno, "unable to write to buffer\n" ) );
  }

  fclose(fp);

  FlexBuffer new_section_id;
  FlexBuffer new_book_short_title;
  FlexBuffer new_section_title;
  FlexBuffer new_book_case_name;

  // append to the catalog file
  if ( catalogfile ) {
    fprintf( catalogfile, "{\ns %s s 32 1;\n", filtered_file_name );

    fprintf( catalogfile, "f 100 \"%s\" ;\n",  
	     FilteredString( new_section_id, SectionID ) );

    fprintf( catalogfile, "f 127 \"%s\" ;\n" , 
	     FilteredString( new_book_short_title, BookShortTitle ) );

    fprintf( catalogfile, "f 128 \"%s\" ;\n",  
	     FilteredString( new_section_title, SectionTitle ) );

    fprintf( catalogfile, "f 130 \"%d\";\n",   BookNum     );

    fprintf( catalogfile, "f 131 \"%s\";\n",   
	     FilteredString( new_book_case_name, BookCaseName) );

    fprintf( catalogfile, "}\n");
  }
  
}

//-----------------------------------------------------------------------
SearchStorage::~SearchStorage()
{
  if ( catalogfile ) { fclose(catalogfile); }
  if ( filteredPath ) { delete [] filteredPath; }
}

  
