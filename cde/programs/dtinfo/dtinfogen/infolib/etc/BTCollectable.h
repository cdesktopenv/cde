/* $XConsortium: BTCollectable.h /main/2 1996/07/18 16:39:46 drk $ */

#ifndef BT_COLLECT
#define BT_COLLECT

class BTCollectable
{

public:
  
  BTCollectable();
  BTCollectable( const char *filename, int line_no, const char *val=NULL );
  ~BTCollectable();

  char *filename() { return(f_name); }
  int linenum()          { return(line_num); }
  char *get_value() { return( value ); }

private:
  char *f_name;
  int line_num;
  char *value;

};

#endif
