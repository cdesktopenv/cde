/* $XConsortium: CC_String.h /main/5 1996/08/21 15:48:50 drk $ */
#ifndef _CC_STRING_H_
#define _CC_STRING_H_

#include <string.h>
#include "dti_cc/types.h"

class CC_String {
public: // functions

  CC_String (const CC_String& x);

  CC_String (const char *string)
  {
    f_string = new char[strlen(string) + 1];
    strcpy (f_string, string);
  }

  CC_String() 
  {
    f_string = NULL;
  }

  virtual ~CC_String () { delete f_string; }

  CC_Boolean isNull() const { 
    return( f_string[0] == '\0' );
  }

  unsigned int hash() const; /* This returns a hash value 
			      * used by the hash funcion
			      */

  size_t length() const {    /* Used strlen, so expect string terminated by
			      * \0. The library in RW takes care of non-null
			      * terminated string. I assume this is not 
			      * the case here.
			      */
    return(strlen(f_string));
  }

  enum caseCompare { exact, ignoreCase };
  int  compareTo(const char *, caseCompare = exact) const;
  int  compareTo(const CC_String &cs, caseCompare CaseSensitive = exact) const
  { 
    return(compareTo((const char *)cs, CaseSensitive));
  }
 
  CC_Boolean operator ==(const CC_String &k)  const
  { 
    return (compareTo(k) == 0) ? TRUE : FALSE; 
  }

  operator const char *() const { return f_string; }
  const char *data() const { return f_string; }

private: 
  char *f_string;
};

#endif
