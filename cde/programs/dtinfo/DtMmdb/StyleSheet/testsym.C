// $XConsortium: testsym.cc /main/3 1996/06/11 17:10:33 cde-hal $
#include "SymTab.h"


SymbolTable *gSymTab = new SymbolTable;
SymbolTable *gElemSymTab = 0;

main()
{


  Symbol a = gSymTab->intern("a");
  Symbol b = gSymTab->intern("b");
  Symbol a2 = gSymTab->intern("a");

  cout << (a == a) << endl;
  cout << (a == b) << endl;
  cout << (a == a2) << endl;

  cout << "ids = " << endl;
  cout << a.name() << "\t" << a.id() << endl;
  cout << b.name() << "\t" << b.id() << endl;
  cout << a2.name() << "\t" << a2.id() << endl;


}
