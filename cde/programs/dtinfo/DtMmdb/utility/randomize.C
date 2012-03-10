// $XConsortium: randomize.cc /main/3 1996/06/11 17:38:40 cde-hal $

#include "utility/randomize.h"

randomize::randomize(int sd) : rdn(sd)
{
}

void randomize::scramble(buffer& original)
{
   int bytes = original.content_sz();
   char* buf = original.get_base();

   for ( int i=0; i<bytes-1; i++ ) 
      char_swap(buf[i], buf[rdn.rand() % ( bytes - i ) + i]);
}   

void randomize::restore(buffer& scrambled)
{
   int bytes = scrambled.content_sz();
   char* buf = scrambled.get_base();

   int *pos = new int[bytes-1];

   for ( int i=0; i<bytes-1; i++ ) {
      pos[i] = rdn.rand() % ( bytes - i ) + i;
   }

   for ( i=bytes-2; i>=0; i-- ) {
      char_swap(buf[i], buf[pos[i]]);
   }

   delete pos;
}
