/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: main.cc /main/3 1996/06/11 16:57:24 cde-hal $
#include <iostream>
using namespace std;
#include "Exceptions.hh"
#include "cc_exceptions.h"
#include "CC_Slist.h"
#include "CC_Dlist.h"
#include "CC_String.h"
#include "CC_Tokenizer.h"
#include "CC_Stack.h"

CC_Boolean mytestfunc(int *a, void *b)
{
  return(*a == 20);
}

#define TEST(tcond)                                    \
   cout << "Testing " << "(" << #tcond << ")\t";       \
   if (tcond) {                                        \
      cout << "Test #" << TestNum << " passed\n";      \
      TestPassed++;                                    \
   }                                                   \
   else {                                              \
      cout << "Test #" << TestNum << " failed\n";      \
      TestFailed++;                                    \
   }                                                   \
   TestNum++;                                         


main()
{

  INIT_EXCEPTIONS();

  try {
    int TestNum = 1;
    int TestPassed = 0;
    int TestFailed = 0;
    
    CC_TPtrSlist<int> slist;
    int a =10;
    int b =20;
    int c =30;

    // inserting 10, 20 , 30 
    slist.insert(&a);
    slist.insert(&b);
    slist.insert(&c);

    // testing first
    TEST(*slist.first() == 10);
    TEST(*slist.at(0) == 10);
    TEST(*slist.at(1) == 20);
    TEST(*slist.at(2) == 30);
    TEST(slist.entries() == 3);

    int d =40;

    slist.prepend(&d);
    slist.append(&d);
  
    TEST(*slist.first() == 40);
    TEST(*slist.at(4) == 40);
  
    TEST(*slist.removeAt(2) == 20);
    TEST(*slist.at(2) == 30);
    TEST(slist.find(&a) != NULL);
    TEST(slist.find(&b) == NULL);
    TEST(slist.contains(&a) == 1);
    TEST(slist.contains(&b) == 0);
    TEST(slist.remove(&b) == NULL);
    TEST(*slist.remove(&a) == 10);
    TEST(slist.remove(&a) == NULL);
    TEST(*slist.remove(&d) == 40);
    TEST(*slist.remove(&d) == 40);
    TEST(slist.remove(&d) == NULL);
    
    // Test slist copy constructor
    CC_TPtrSlistIterator<int> slist_iter( slist );
    CC_TPtrSlist<int> new_slist( slist );
    CC_TPtrSlistIterator<int> new_slist_iter(new_slist);

    TEST(slist.entries() == new_slist.entries());
    for ( int w = 0; w < slist.entries(); w++ ) {
      slist_iter();
      new_slist_iter();
      TEST(*slist_iter.key() == *new_slist_iter.key());
    }
    
    // empty list
    CC_TPtrSlist<int> empty_list;
    TEST(empty_list.remove(&d) == NULL) ;
    TEST(empty_list.find(&d) == NULL);
    TEST(empty_list.removeFirst() == NULL);
    TEST(empty_list.removeLast() == NULL);
    
    empty_list.clearAndDestroy();
    
    CC_TPtrDlist<int> dlist;
    dlist.append(&a);
    dlist.append(&b);
    dlist.append(&c);
    
    TEST(*dlist.first() == 10 );
    TEST(*dlist.at(1) == 20 );
    TEST(*dlist.at(2) == 30);
    
    TEST(*dlist.last() == 30);
    
    dlist.prepend(&c);
    TEST(*dlist.first() == 30);
    
    dlist.insert(&b);
    TEST(*dlist.last() == 20);
    
    TEST(*dlist.removeFirst() == 30);
    TEST(*dlist.removeLast() == 20);
    TEST(*dlist.remove(&b) == 20);
    TEST(*dlist.remove(&c) == 30);
    dlist.insert(&b);
    dlist.insert(&c);

    
    CC_TPtrDlistIterator<int> dlist_iter(dlist);
    CC_TPtrDlist<int> new_dlist(dlist);
    CC_TPtrDlistIterator<int> new_dlist_iter(new_dlist);

    // Compare the elements in the 2 lists 
    for ( int i = 0; i < dlist.entries(); i++ ) {
      dlist_iter();
      new_dlist_iter();
      TEST(*dlist_iter.key() == *new_dlist_iter.key());
    }

    /* Traverse backward */
    for ( int j = 0; j < dlist.entries(); j++ ) {
      cout << *dlist_iter.key() << ", ";
      --dlist_iter;
    }

    cout << endl;
    
    dlist_iter += 2;
    TEST(*dlist_iter.key() == 20);

    TEST(*dlist.find(mytestfunc, NULL) == 20 );

    // dlist.clearAndDestroy();

    CC_String cstr("abc");
    TEST(cstr.isNull() == FALSE);
    TEST(cstr.length() == 3);
    TEST(cstr.compareTo("abc") == 0);
    TEST(cstr.compareTo("def") < 0 );
  
    TEST(cstr.compareTo("ABC") != 0);
    TEST(cstr.compareTo("ABC", CC_String::ignoreCase) == 0);
    TEST(cstr.compareTo("ABC", CC_String::exact) != 0);
  
    CC_String dstr("abc");
    TEST(cstr.compareTo(dstr) == 0);
    TEST(cstr.compareTo(dstr, CC_String::ignoreCase) == 0);
    TEST(cstr.compareTo(dstr.data()) == 0);
    TEST(cstr.compareTo("abcde") != 0 );
    TEST(cstr.compareTo("abcde", CC_String::ignoreCase) != 0 );

    CC_TValSlist<int> vlist;
    vlist.append(a);
    vlist.append(b);
    vlist.append(c);
    
    CC_TValSlistIterator<int> vlist_iter(vlist);
    CC_TValSlist<int> new_vlist(vlist);
    TEST(vlist.entries() == new_vlist.entries());
    CC_TValSlistIterator<int> new_vlist_iter(new_vlist);

    for ( int k = 0; k < vlist.entries(); k++ ) {
      ++vlist_iter;
      ++new_vlist_iter;
      TEST(vlist_iter.key() == new_vlist_iter.key());
    }

    vlist_iter.reset();
    
    cout << endl;
    
    
    

    CC_String cc_str1("This is a pretty long string for testing");
    CC_Tokenizer next1( cc_str1 );
    
    while ( next1() ) {
      cout << next1.data() << endl;
    }


    char str[128];
    snprintf(str, sizeof(str), "\t\nthis is \t    another string\t\n");
    CC_String cc_str2( (const char *)str);
    
    CC_Tokenizer next2( cc_str2 );
    while ( next2() ) {
      cout << next2.data() << endl;
    }

    int *empty;
    CC_TPtrDlist<int> tdlist;
    // tdlist.insert(empty);
    TEST(tdlist.removeFirst() == NULL);
    TEST(tdlist.removeLast() == NULL);
    tdlist.prepend(empty);
    tdlist.insert(empty);
    tdlist.removeFirst();
    tdlist.removeLast();
    TEST(tdlist.removeFirst() == NULL);
    // TEST(0);


    Stack<int> stack;
    stack.push(2);
    stack.push(3);
    stack.push(4);

    TEST(stack.top() == 4 );
    TEST(stack.pop() == 4 );
    TEST(stack.pop() == 3 );
    TEST(stack.pop() == 2 );
    stack.push(20);
    TEST(stack.top() == 20);
    TEST(stack.pop() == 20);
    TEST(stack.empty());

    cout << "test passed = " << TestPassed << endl;
    cout << "test failed = " << TestFailed << endl;
    
    try {
      TEST(stack.top());  // This will throw exception
    }
    catch( Exception&, u)
      {
	cerr << "Exception generated by top() were caught\n";
      }end_try;

    try {
      TEST(stack.pop());
    }
    catch( Exception&, u )
      {
	cerr << "Exception generated by pop() were caught\n";
	exit(0);
      }end_try;

  }
  catch(ccException&, u)
    {
      cerr << "ccException caught\n";
    }
  catch(Exception&, u)
    {
      cout << "Exception generated by pop() caught\n";
      exit(1);
    }end_try;
}
  

  

  
