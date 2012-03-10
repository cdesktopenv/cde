// $XConsortium: CC_Tokenizer.C /main/5 1996/11/21 19:47:27 drk $
#include <string.h>
#include "CC_Tokenizer.h"

//--------------------------------------------------------------
CC_Tokenizer::CC_Tokenizer( const CC_String &s )
{
  str_ = new char [s.length()+1];
  strcpy(str_, s.data() );
  current_ptr = str_;
  touched = FALSE;
}


//--------------------------------------------------------------
CC_Boolean
CC_Tokenizer::operator()()
{

  if ( !touched ) {
    current_ptr = _XStrtok(str_, " \t\n", strtok_buf);
    touched = TRUE;
  }
  else {
    current_ptr = _XStrtok(NULL, " \t\n", strtok_buf);
  }

  return ( current_ptr != NULL );
}
    
  
  
  
