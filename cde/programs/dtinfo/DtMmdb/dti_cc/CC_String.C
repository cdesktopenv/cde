// $XConsortium: CC_String.cc /main/3 1996/06/11 16:55:54 cde-hal $
#include "CC_String.h"
#include <ctype.h>

#include "utility/atoi_fast.h"

unsigned int 
CC_String::hash() const
{
  static atoi_fast converter(65535, 256);
  return converter.atoi(f_string);
}

CC_String::CC_String (const CC_String& x)
{
   f_string = strdup(x.f_string);
}

//-----------------------------------------------------
int
CC_String::compareTo(const char *str, caseCompare CaseSensitive ) const
{
  if ( CaseSensitive == exact ) {
    return (strcmp(f_string, str));
  }
  else {
    const char *ptr1 = f_string;
    const char *ptr2 = str;
    for ( ; tolower(*ptr1) == tolower(*ptr2) ; ptr1++, ptr2++ ) {
      if ( *ptr1 == '\0' ) {
	return (0);
      }
    }

    return ( tolower(*ptr1) - tolower(*ptr2) );
  }
}

	
      
      
    
