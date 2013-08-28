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
/* $XConsortium: mark_test.cc /main/5 1996/07/18 16:03:09 drk $ */

#ifdef REGRESSION_TEST

#include "mark_test.h"

void print_mark_list(mark_base* bptr, char* locator)
{
   oid_list_handler* x = bptr -> get_mark_list(locator);

   if ( x == 0 ) {
     MESSAGE(cerr, "empty oid_list");
     return ;
   }

   int ind = (*x) -> first();

   while (ind) {

      oid_t id = (*x) -> operator()(ind);

      if ( id.eq(ground) == false ) {
         mark_smart_ptr y(bptr, id);
         debug(cerr, y);
      }

      (*x) -> next(ind);
   }
}

mark_base* get_mark_base(char* path, char* name)
{
   mark_base* ub = 0;

   try {
     ub = new mark_base(
			     path,
                             name,
                             "this marks",
                             user_base::WRITE
                        );
   }

   catch (MMDBeXCEPtion &,e)
   {
       return 0;
   } end_try;

            
   if ( ub -> open_status() != user_base::SUCC ) {
       return 0;
   }

   return ub;
}

            
void _get_id(istream& in, oid_t& x)
{
   static char buf[LBUFSIZ];
   in.getline(buf, LBUFSIZ);

   if ( buf[0] != 'k' || buf[1] != '\t' ) {
       debug(cerr, buf);
       throw(stringException("_get_id(): missing k\t"));
   }

    char* key = buf+2;

    oid_t id(key, true, false);

    x = id;
}

void _get_key(istream& in, char*& key)
{
   static char buf[LBUFSIZ];
   in.getline(buf, LBUFSIZ);

   if ( buf[0] != 'k' || buf[1] != '\t' ) {
       debug(cerr, buf);
     throw(stringException("_get_key(): missing k\t"));
   }

   key = buf + 2;
}

void _get_data(istream& in, char*& data)
{
   static char buf[LBUFSIZ];
   in.getline(buf, LBUFSIZ);

   if ( buf[0] != 'd' || buf[1] != '\t' ) {
       debug(cerr, buf);
     throw(stringException("_get_data(): missing d\t"));
   }

   data = buf + 2;
}

void _get_key_pos(istream& in, char*& key, int& n)
{
   static char buf[LBUFSIZ];
   in.getline(buf, LBUFSIZ);

   if ( buf[0] != 'k' || buf[1] != '\t' ) {
       debug(cerr, buf);
     throw(stringException("_get_key_pos(): missing k\t"));
   }

   key = buf + 2;

   char* nextToken = strchr(key, '\t');

   if ( nextToken == 0 )
     throw(stringException("can't find position (integer)."));

   *nextToken = 0;

   n = atoi(nextToken+1);
}


oid_t get_nth_mark_id(mark_base* ub, char* key, int n)
{
   oid_list_handler* handle = ub -> get_mark_list(key);
   
   if ( handle == 0 ) {
     throw(stringException("empty mark list"));
   }

   int ind = (*handle) -> first();

   int i = 0;

   while (ind) {

      if ( n == i ) {
        oid_t id = (*handle) -> operator()(ind);
        delete handle;
        return id;
      }

      (*handle) -> next(ind);
      i++;
   }
   
   int count = (*handle)->count();
   delete handle;
   throw(boundaryException(0, count, i));
}


//////////////
// create
// k \t locator
// d \t data
//////////////
int _create_subtest(char* buf, istream& in, mark_base* ub) 
{
    char* key, *data;
    mark_smart_ptr* x = 0;

    if ( strcmp(buf, "create") == 0 ) {
                   
       cout << ">>> create:" << endl;

       _get_key(in, key);
       _get_data(in, data);

       cout << "---- new mark smtart_ptr ..." << endl;
       x = new mark_smart_ptr(ub, key);

       cout << "---- updating ..." << endl;
       x -> update_usermark(data, strlen(data));
       delete x;
       cout << "<<< create:" << endl;
   } else
      return 1;
}
      
  
//////////////
// get_by_key 
// k \t locator \t n
//////////////
_get_by_key_subtest(char* buf, istream& in, mark_base* ub)
{
   char* key;
   int n;
   mark_smart_ptr* x = 0;

   if ( strcmp(buf, "get_by_key") == 0 ) {
      cout << ">>> get_by_key:" << endl;
                   
      _get_key_pos(in, key, n);

      oid_t id = get_nth_mark_id(ub, key, n);

      x = new mark_smart_ptr(ub, id);
                      
      cout << x->node_locator();
      cout << "\n";
      cout << *(x->mark_value());
      cout << "\n";
                      
      delete x;
               
      cout << "<<< get_by_key:" << endl;
   } else return 1;
}

//////////////
// get_by_oid
// k \t oidInAscii
//////////////
_get_by_oid_subtest(char* buf, istream& in, mark_base* ub)
{
   char* key;
   mark_smart_ptr* x = 0;
   if ( strcmp(buf, "get_by_oid") == 0 ) {
                      
      _get_key(in, key);

      oid_t id(key, true, false);
                      
      x = new mark_smart_ptr(ub, id);
                      
      cout << x->node_locator();
      cout << "\n";
      cout << *(x->mark_value());
      cout << "\n";
                      
      delete x;
               
   } else 
      return 1;
}

//////////////
// show_for_a_locator
// k \t locator
//////////////
_show_for_a_locator_subtest(char* buf, istream& in, mark_base* ub)
{
   char* key;
   if ( strcmp(buf, "show_for_a_locator") == 0 ) {
                      
      cout << ">>> show_for_a_locator:" << endl;

      _get_key(in, key);

      oid_list_handler* z = ub-> get_mark_list(key);

      if ( z == 0 ) 
         throw(stringException("empty oid_list"));
                   
                      
      int ind = (*z) -> first();
                   
      while (ind) {
                   
         oid_t id = (*z) -> operator()(ind);
                   
                         
         if ( id.eq(ground) == false ) {
                            
            mark_smart_ptr y(ub, id);
            cout << y.its_oid();
            cout << "\n";
            cout << y.node_locator();
            cout << "\n";
            cout << *(y.mark_value());
            cout << "\n";
                         
         }
                   
         (*z) -> next(ind);
      }

      delete z;

      cout << "<<< show_for_a_locator" << endl;
		
   } else return 1;
}

//////////////
//show_all_marks_subtest 
//////////////
_show_all_marks_subtest(char* buf, istream& in, mark_base* ub)
{
   if ( strcmp(buf, "show_all_marks") == 0 ) {
      cout << ">>> show_all_marks" << endl;

      mmdb_pos_t ind = ub -> first();
      while (ind) {
         oid_t id = ub -> get_mark_oid(ind);

         if ( id.eq(ground) == false ) {
                            
            mark_smart_ptr y(ub, id);
            cout << y.its_oid();
            cout << "\n";
            cout << y.node_locator();
            cout << "\n";
            cout << *(y.mark_value());
            cout << "\n";
                         
         }

         ub -> next(ind);
      }
      cout << "<<< show_all_marks." << endl;
   } else
      return 1;
}

//////////////////////////////////////
//////////////////////////////////////
int _update_by_id_subtest(istream& in, oid_t id, mark_base* ub)
{
   char* data;
   mark_smart_ptr* x = 0;
   _get_data(in, data);

    x = new mark_smart_ptr(ub, id);
                      
    cout << x -> its_oid() << "\n";

    x -> update_usermark(data, strlen(data));
    delete x;

    x = new mark_smart_ptr(ub, id);
                      
    pstring* y = x->mark_value();
                      
    if ( y -> size() != strlen(data) || strcmp(y -> get(), data) != 0 ) 
    {
      debug(cerr, y->size());
      debug(cerr, (long)strlen(data));
      debug(cerr, y -> get());
      debug(cerr, data);
      throw(stringException("improperly updated mark"));
                      
    }
                      
    delete x;

    return 0;
}

//////////////
// update_by_key 
// k \t locator \t n
// d \t data
//////////////
_update_by_key_subtest(char* buf, istream& in, mark_base* ub)
{
   char* key;
   int n;

   if ( strcmp(buf, "update_by_key") == 0 ) {

      cout << ">>> update_by_key" << endl;

      _get_key_pos(in, key, n);

      oid_t id = get_nth_mark_id(ub, key, n);

      _update_by_id_subtest(in, id, ub);

      cout << "<<< update_by_key" << endl;
      return 0;

   } else 
      return 1;
}
  
////////////////////
// update_by_id 
// k \t oid_tInAscii
// d \t data
////////////////////

_update_by_id_subtest(char* buf, istream& in, mark_base* ub)
{
    if ( strcmp(buf, "update_by_id") == 0 ) {

       cout << ">>> _update_by_id_subtest" << endl;

       oid_t id;

       _get_id(in, id);

       _update_by_id_subtest(in, id, ub);

       cout << "<<< _update_by_id_subtest" << endl;
       return 0;

     } else
       return 1; 
}


//////////////
// delete_by_key
// k \t locator \t n
//////////////
_delete_by_key_subtest(char* buf, istream& in, mark_base* ub)
{
   mark_smart_ptr* x = 0;
   char* key; int n;
   if ( strcmp(buf, "delete_by_key") == 0 ) {

      cout << ">>> _delete_by_key_subtest" << endl;

      _get_key_pos(in, key, n);

      oid_t id = get_nth_mark_id(ub, key, n);

      x = new mark_smart_ptr(ub, id);

      x -> remove_from_db();
                   
      delete x;

      cout << "<<< _delete_by_key_subtest" << endl;
	
    } else return 1;
}

//////////////
// delete_by_oid
// k \t oidInAscii
//////////////
_delete_by_oid_subtest(char* buf, istream& in, mark_base* ub)
{
   mark_smart_ptr* x = 0;
   if ( strcmp(buf, "delete_by_oid") == 0 ) {
        oid_t id;
        _get_id(in, id);

        x = new mark_smart_ptr(ub, id);
                      
        x -> remove_from_db();
                      
        delete x;
               
    } else return 1;
}

int mark_test(int argc, char** argv)
{
    int ok = 2;

    if ( strcmp(argv[1], "create_mark_base") == 0 ) {
      if ( argc != 4 ) {
         MESSAGE(cerr, "args: create_mark_base path name");
         ok = -1;
      } else {

         mark_base* ub = get_mark_base(argv[2], argv[3]);

         if ( ub == 0 || ub -> open_status() != user_base::SUCC ) {
            ok = -1;
         } else {
            delete ub;
            ok = 0;
         }
      }
   } else

   if ( strcmp(argv[1], "mark_comprehensive") == 0 ) {
      if ( argc != 5 ) {
         MESSAGE(cerr,
           "mark_comprehensive args: mark_comprehensive test_file path name");
         ok = -1;
      } else {

         mark_base* ub = get_mark_base(argv[3], argv[4]);

         if ( ub == 0 || ub -> open_status() != user_base::SUCC ) {
            ok = -1;
         } else {


            fstream in(argv[2], ios::in);

            int i=1;
            char buf[LBUFSIZ];

            while ( in.getline(buf, LBUFSIZ) ) {

               if ( buf[0] == '#' )
		   continue;

	       if ( _create_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _get_by_key_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _get_by_oid_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _show_for_a_locator_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _show_all_marks_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _update_by_key_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _update_by_id_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _delete_by_key_subtest(buf, in, ub) != 1 )
		   continue;

               if ( _delete_by_oid_subtest(buf, in, ub) != 1 ) {
                   debug(cerr, buf);
		   throw(stringException("bad subtest command"));
               }

               if ( i % 50 == 0 )
                  MESSAGE(cerr, form("%d processed", i));
 
               i++;
            }
         }

         delete ub;
         ok = 0;
      }
   }

   return ok;
}
#endif
