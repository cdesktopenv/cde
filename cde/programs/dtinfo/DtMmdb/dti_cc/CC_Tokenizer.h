/* $XConsortium: CC_Tokenizer.h /main/4 1996/11/04 13:35:14 drk $ */
#ifndef __CC_Token_h
#define __CC_Token_h

#include "CC_String.h"

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

class CC_Tokenizer {

private:
  char      *current_ptr;
  char      *str_;
  CC_Boolean touched;
  _Xstrtokparams	strtok_buf;

public:
  CC_Tokenizer(const CC_String & );
  ~CC_Tokenizer() { delete str_; }


  /* Here is an example to use the code 
   *     CC_String b("This is a string");
   *     CC_Tokenizer next( b );
   *     while ( next() ) {
   *         cout << next.data() << endl;
   *     }
   */

  CC_Boolean operator()();  /* returns TRUE if next token exists, 
			     * FALSE if otherwise
			     */
  const char *data() { return(current_ptr); } /* returns the current token */
};
  
#endif

