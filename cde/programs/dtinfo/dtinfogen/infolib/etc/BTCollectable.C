/* $XConsortium: BTCollectable.cc /main/2 1996/07/18 16:09:37 drk $ */
#include <string.h>

// exported interfaces 
#include "BTCollectable.h"

//--------------------------------------------------------------------
BTCollectable::BTCollectable():f_name(0), line_num(0), value(0)
{
}

//--------------------------------------------------------------------
BTCollectable::BTCollectable( 
  const char *filename, int line_no, const char *val
  )
{

  f_name = strdup( filename );
  line_num = line_no;
  value = strdup( val );
}

//--------------------------------------------------------------------
BTCollectable::~BTCollectable()
{
  delete f_name;
  delete value;
}
