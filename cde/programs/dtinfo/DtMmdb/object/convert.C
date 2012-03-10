// $XConsortium: convert.cc /main/3 1996/06/11 17:23:53 cde-hal $

#include <iostream.h>

/*****************************************/
//Note: convert node.dat file to new format
/*****************************************/

#define BUFSIZ 100

main()
{
   int sz;
   int buf_sz = BUFSIZ;
   char *buf = new char[BUFSIZ];

   while ( cin.getline(buf, BUFSIZ) ) { // read in '100L'
       cout << "102	0\n";
       cout << "4\n";

       for ( int i=0; i<4; i++ ) {
          cin >> sz;
/*
cout << "***";
cout << sz;
cout << "***\n";
*/
          cin.get();
   
          if ( buf_sz < sz ) {
             buf_sz = sz;
             buf = new char[buf_sz];
          }
   
          cin.read(buf, sz); 

          cin.get(); // skip '\n'
          cout << "5\t0" << "\n";
          cout << sz << "\t";
          cout.write(buf, sz); 

          cout << "\n";
       }
   }

   delete buf;
}
