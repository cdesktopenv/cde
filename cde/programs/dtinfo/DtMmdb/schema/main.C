// $XConsortium: main.cc /main/3 1996/06/11 17:32:36 cde-hal $

#include "utility/funcs.h"
#include "schema/object_dict.h"

int main( int argc, char** argv )
{
   INIT_EXCEPTIONS();

    try
   {
    if ( argc != 4 ) {
	MESSAGE(cerr, "usage: def_name db_path base_nm\n");
	exit(1);
    }

    object_dict od(argv[1], argv[2], argv[3]);
   }

   catch (mmdbException &,e)
   {
      cerr << "Exception msg: " << e << "\n";
#ifdef DEBUG
      abort();
#else
      ok = -1;
#endif

   }
   end_try;

    exit( 0 );
}

