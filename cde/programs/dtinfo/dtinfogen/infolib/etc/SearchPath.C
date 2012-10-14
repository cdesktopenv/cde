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
/* $XConsortium: SearchPath.C /main/7 1996/08/21 15:47:21 drk $ */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if !defined(__uxp__) && !defined(USL)
#include <strings.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
using namespace std;

#include "Exceptions.hh"
#include "Task.h"
#include "api/utility.h"

/* exported interface */
#include "SearchPath.h"

#ifdef NEED_STRCASECMP
#include <ctype.h>
/*
 * In case strcasecmp and strncasecmp are not provided by the system
 * here are ones which do the trick.
 */

int
strcasecmp(register const char *s1,
	   register const char *s2)
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}


int
strncasecmp(register const char *s1,
	    register const char *s2,
	    register size_t count)
{
    register int c1, c2;

    if (!count)
      return 0;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if ((c1 != c2) || (! --count))
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}
#endif


//--------------------------------------------------------------------
static int isdir(char* filename)
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

  
//-------------------------------------------------------------------
SearchPath::SearchPath( const char *path, ... )
{
  
  search_path_table = new CC_TPtrSlist<CC_String>;
  new_path = 0;

  va_list ap;
  va_start ( ap , path);

  const char *spath = path;
  while ( spath ) {
    CC_String *key = new CC_String(spath);
    search_path_table->append( key );
    spath = va_arg ( ap, const char * );
  }
  
  va_end ( ap );
}

//-------------------------------------------------------------------
char *
SearchPath::get_real_path( const char *file_name )
{
  CC_TPtrSlistIterator<CC_String> path_it( *search_path_table );
  FILE *fp;

  if (file_name == NULL || *file_name == '\0')
    return NULL;

  // remove storage object specifier
  if (strncasecmp(file_name, "<OSFILE", 7) == 0) {
    if (file_name = strchr(file_name, '>'))
      file_name++;
    else
      return NULL;
  }

  while ( path_it() ) {
    const char *path = (const char *)*path_it.key();

    char *full_path_name = form( "%s/%s", path, file_name );
    if (( fp = fopen( full_path_name , "r" )) && !isdir(full_path_name) ) {
      fclose( fp );
      return ( full_path_name );
    }
  }

  return NULL;
}


//-------------------------------------------------------------------    
void
SearchPath::replace_file_scope( const char *f_path )
{
  if ( new_path ) {
    if ( !search_path_table->remove( new_path ) ) {
      throw(Unexpected("Cannot replace file scope\n"));
    }
    delete new_path; new_path = 0;
  }

  new_path = new CC_String(f_path);

  search_path_table->prepend( new_path );

}
  
