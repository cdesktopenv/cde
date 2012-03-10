/* $XConsortium: SearchPath.h /main/3 1996/08/21 15:47:25 drk $ */
// SearchPath.h

#ifndef SRH_PATH_HDR
#define SRH_PATH_HDR

#include <stdarg.h>

#include "dti_cc/CC_Slist.h"
#include "dti_cc/CC_String.h"

class SearchPath {
private:
  CC_TPtrSlist<CC_String> *search_path_table;
  CC_String *new_path;

public:

  /* the search path order is significant because they are searched in that
   * order
   */
  /*  
   * Usage :: SearchPath *ptr = new SearchPath ( "dir1", "dir2", "dir3" );
   * and later on when I invoked ptr->get_real_path( "file_name" );
   * dir1, dir2 and dir3 will be searched in order to determine if file_name
   * exists in any one of the directories. 
   * 
   * The first directory which contains "file_name" will be returned by 
   * get_real_path( "file_name" );
   *
   */
  SearchPath(const char *path, ... );

  /*
   * Usage: replace_file_scope checks if we are in a file that is outside the
   *        the build directory and the source directory, if so , it will
   *        get rid of that directory, and prepend the new one at the front
   *        of the search path table list. This is primarily tied to the 
   *        the ESIS format for the display of current file name and line 
   *        number.
   *        
   */

  void replace_file_scope( const char *path );
  char *get_real_path( const char *file_name );
  ~SearchPath() { if ( search_path_table ) delete search_path_table; }

};
   
#endif
