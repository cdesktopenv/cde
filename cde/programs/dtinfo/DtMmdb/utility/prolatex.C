// $XConsortium: prolatex.C /main/3 1996/06/11 17:38:26 cde-hal $

#include <stdlib.h> 
#include <string.h> 
#include <iostream.h> 

#define BUFSIZ 1000

main()
{
   char buf[BUFSIZ];
   while ( cin.getline(buf, BUFSIZ) ) {
       int loc = strlen("\\epsffile{") ; 
       char c = buf[loc];
       buf[loc] = '\0';
       if ( strcmp(buf, "\\epsffile{") == 0 ) {
         cout << buf;
         cout << getenv("PWD");
         cout << "/";
         buf[loc] = c;
         cout << buf + loc << "\n";
       } else {
         buf[loc] = c;
         cout << buf << "\n";
       }
   }
   return 0;
}
