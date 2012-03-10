// $XConsortium: tst_strstream.C /main/4 1996/08/21 15:55:44 drk $


#include <stdio.h>
#include "utility/c_strstream.h"

tst_ostrstream()
{
   char buf[1024];
   ostrstream os(buf, 1024);
   os << "istream : virtual public ios.";
   fprintf(stderr, "buf=%s\n", buf);
}

tst_istrstream()
{
   char* buf = "303.0";
   istrstream is(buf, strlen(buf));

   int c = is.get() ;
   fprintf(stderr, "c=%c\n", c);
   is.putback(c) ;

   int x; char ch;
   is >> x; fprintf(stderr, "x=%d\n", x);
   is >> ch; fprintf(stderr, "ch=%c\n", ch);
   is >> x; fprintf(stderr, "x=%d\n", x);
}

main()
{
   tst_ostrstream();
   tst_istrstream();
}
