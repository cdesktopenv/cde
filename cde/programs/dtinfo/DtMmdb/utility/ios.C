// $XConsortium: ios.C /main/4 1996/08/21 15:54:46 drk $

#include "utility/c_ios.h"
#include <ctype.h>
#include <stdio.h>

ios::ios(streambuf* sb) : sbuf(sb), f_state(OK)
{
} 

ios::~ios() 
{
   delete sbuf;
}

int ios::fail() 
{
    if ( bad() ) return 1;
    if ( BIT_TEST(f_state, FAIL) )
      return 1;
    else
      return 0;
}

int ios::bad() 
{
    if ( BIT_TEST(f_state, BAD) )
       return 1;
    else
       return 0;
}
