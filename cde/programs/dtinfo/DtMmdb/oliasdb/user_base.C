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
/*
 * $XConsortium: user_base.cc /main/8 1996/08/15 14:13:12 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */



#include "oliasdb/user_base.h"
#include "oliasdb/mark.h"
#include "utility/debug.h"
#include "misc/unique_id.h"

#define LOCK_DIR "lock"
#define AT_LOCK "at_lock"
#define W_LOCK "w_lock"
#define ACCESS_INFO "access_info"

extern void_ptr_array g_store_array;
extern Boolean g_transaction_on;

extern int g_mode_8_3;

user_base::user_base( const char* spec_path, rw_flag_t rw) : 
   base(0), backup_file(0), rw_flag(rw), first_desc_ptr(0),
   spec_name(spec_path)
{
   g_mode_8_3 = 1;
   int len;

   f_obj_dict = new object_dict;

   char* path = getenv("DTINFO_USER_DB") ;

   if ( path == 0 ) {
       char* home = getenv("HOME");
       char* lang = getenv("LANG");

       if ( home == 0 )
          throw(stringException("HOME unspecified"));
       if ( lang == 0 )
          throw(stringException("LANG unknown"));

       path = form("%s/.dt/dtinfo/%s", home, lang);
   }

   char* name = getenv("USER");

   if ( name == 0 )
      name = (char*)"";

   len = MIN(strlen(path), PATHSIZ - 1);
   *((char *) memcpy(base_path, path, len) + len) = '\0';
   len = MIN(strlen(name), PATHSIZ - 1);
   *((char *) memcpy(base_name, name, len) + len) = '\0';
   len = MIN(strlen(""), PATHSIZ - 1);
   *((char *) memcpy(base_desc, "", len) + len) = '\0';

   _init();

}


user_base::user_base( const char* base_dir, 
                      const char* base_nm,
                      const char* base_ds,
                      const char* spec_path,
		      rw_flag_t rw 
                    ) : 
	base(0, 0, 0, base_dir, base_nm, base_ds, ""), backup_file(0),
	rw_flag(rw), checking_status(SUCC), first_desc_ptr(0),
	spec_name(spec_path)
{
   g_mode_8_3 = 1;

   f_obj_dict = new object_dict;

   _init();
}

user_base::checking_status_t user_base::check_mode()
{
   mtry {

     switch ( rw_flag ) {
         case user_base::READ:
        {
           if ( exist_dir(base_path) == false ) {
              return user_base::FAIL;
           }
           break;
        }
         case user_base::WRITE:
        {
           if ( check_and_create_dir(base_path) == false ) {
              return user_base::FAIL;
           }
           break;
        }
     }
   }

   mcatch (systemException&, e)
      {
         return user_base::FAIL;
      }
   end_try;

   if ( exist_file(SCHEMA_FILE, base_path) == true ) 
      return user_base::SUCC;

   if ( exist_file(form("%s.%s", base_name, SCHEMA_FILE_SUFFIX), base_path) == true ) 
      return user_base::SUCC;

   return user_base::NO_BASE;
}

user_base::checking_status_t user_base::check_lock()
{
   char lock_dir[PATHSIZ];
   int len;

   snprintf(lock_dir, sizeof(lock_dir), "%s/%s", base_path, LOCK_DIR);

   if ( check_and_create_dir(lock_dir) == false ) {
      MESSAGE(cerr, form("no write permission to %s", lock_dir));
      return user_base::CREATE_LOCKFILE_FAIL;
   }

   len = MIN(strlen(atomic_lock_path), strlen(lock_dir) + strlen(AT_LOCK) + 1);
   *((char *) memcpy(atomic_lock_path,
		     form("%s/%s", lock_dir, AT_LOCK), len) + len) = '\0';
   len = MIN(strlen(write_lock_path), strlen(lock_dir) + strlen(W_LOCK) + 1);
   *((char *) memcpy(write_lock_path,
		     form("%s/%s", lock_dir, W_LOCK), len) + len) = '\0';
   len = MIN(strlen(ai_path), strlen(lock_dir) + strlen(ACCESS_INFO) + 1);
   *((char *) memcpy(ai_path,
		     form("%s/%s", lock_dir, ACCESS_INFO), len) + len) = '\0';

   char* ai_info = 0;

   switch (rw_flag) {
     case user_base::READ:

        if (
         read_lock(atomic_lock_path, write_lock_path, 
                   ai_path, access_info((char*)"read"), offset, ai_info
                  ) == false
           ) {
            if ( ai_info ) {
              debug(cerr, ai_info); 
              delete ai_info;
            }
            return user_base::RLOCK_FAIL;
        } else
           return user_base::SUCC;
        break;
 
     case user_base::WRITE:

         if (
          write_lock(atomic_lock_path, write_lock_path, 
                    ai_path, access_info((char*)"write"), ai_info
                   ) == false
         ) {
             if ( ai_info ) {
                debug(cerr, ai_info); 
             
//
//                int x = strlen(ai_info);
//                if ( x >= 5 && strncmp(ai_info + x - 5, "read", 4) == 0 )
//                {
//                   rw_flag = user_base::READ;
//                   MESSAGE(cerr, "write locking failed, try read mode");
//                   int ok = check_lock();
//                   delete ai_info;
//                   return ok;
//                }

                delete ai_info;
             }
             return user_base::WLOCK_FAIL;
         } else {
            return user_base::SUCC;
         }
        break;

     default:
         abort();
   }
}


void user_base::_init()
{
  set_mode(HEALTH, true);
  
  checking_status = check_mode();

  MESSAGE(cerr, "Initial mode");
  debug(cerr, checking_status);

  first_desc_ptr = 0;

  switch ( checking_status ) {

    case user_base::NO_BASE:

       if ( disk_space(base_path) < 10*KB ) {
           checking_status = user_base::FAIL;
           break;
       }

       mtry
         {
           ubase_trans.begin();

           define();

           ubase_trans.sync();
           ubase_trans.end();
   
         }

       mcatch (beginTransException &,e)
         {
// cases: can't open log or write size info to log
           checking_status = user_base::FAIL;
           clean_up();
           break;
         }
       mcatch (commitTransException &,e)
         {
// cases: bad log file, can't write to store, etc.
           checking_status = user_base::FAIL;
           clean_up();
           break;
         }
// cases: can't do define()
       mcatch (mmdbException &,e)
         {
           checking_status = user_base::FAIL;
           ubase_trans.rollback(); 
	   ubase_trans.abort();
           clean_up();
           break;

         }
       end_try;
       
       checking_status = check_mode();
       break;

    case user_base::SUCC:

       mtry {
          ubase_trans.begin();

          first_desc_ptr = f_obj_dict -> init_a_base(base_path, base_name);

          ubase_trans.end();
       }

       mcatch (mmdbException &,e) {
#ifdef DEBUG
	  fprintf(stderr, "mmdbException caught @ %s line:%d.\n",
						__FILE__, __LINE__);
#endif
          ubase_trans.abort();

          set_mode(HEALTH, false);
          checking_status = user_base::FAIL;
          break;
       } end_try;

       break;

    default:
       set_mode(HEALTH, false);
       break;
  }
  
#ifndef NO_DB_LOCK
  if ( checking_status == user_base::SUCC ) {
     checking_status = check_lock();
     debug(cerr, checking_status);
     if ( checking_status != user_base::SUCC ) {
       set_mode(HEALTH, false);
       return;
     }
  }
#endif
  

  MESSAGE(cerr, "Final mode"); debug(cerr, checking_status);

  switch ( checking_status ) {

    case user_base::SUCC:
      set_mode(HEALTH, true);
      MESSAGE(cerr, form("userbase \"%s\" available", base_name));
      break;

    default:
      set_mode(HEALTH, false);
      break;
  }

  return;
}

user_base::~user_base()
{
   if ( checking_status == user_base::SUCC &&
        exist_dir(form("%s/%s", base_path, LOCK_DIR)) == true ) 
   {

#ifndef NO_DB_LOCK
   switch (rw_flag) {
     case user_base::READ:
        read_unlock(atomic_lock_path, ai_path, offset);
        break;
     case user_base::WRITE:
        write_unlock(atomic_lock_path, write_lock_path, ai_path);
             );
        break;
   }
#endif
   }

  delete f_obj_dict;
}

void user_base::clean_up()
{
   abs_storage* store_ptr = 0;
   desc* desc_ptr = first_desc_ptr;

   while ( desc_ptr ) {
        store_ptr = desc_ptr -> get_store();

        if ( store_ptr ) {
          mtry {
/*
MESSAGE(cerr, "removing: ");
MESSAGE(cerr, store_ptr -> my_name());
*/
               store_ptr->remove();
          }
          mcatch (mmdbException &,e)
          {
          } end_try;
        }
        desc_ptr = desc_ptr -> get_next_desc();
   }

   if ( exist_file(SCHEMA_FILE, base_path) == true )
      del_file(SCHEMA_FILE, base_path);
}

Boolean user_base::define()
{
   int len;

   if ( check_and_create_dir(base_path) == false ) {
      throw(stringException(form("can't create %s", base_path))); 
   }

   char spec_file_path[PATHSIZ];

   //char* x = getenv("DTINFO_LIB");
   char* x = getenv("DTINFO_MARKSPECPATH");

   if ( x == 0 ) {
       len = MIN(strlen(spec_name), PATHSIZ - 1);
       *((char *) memcpy(spec_file_path, spec_name, len) + len) = '\0';
   } 
   else {
       len = MIN(strlen(x) + strlen(spec_name) + 1, PATHSIZ - 1);
       *((char *) memcpy(spec_file_path,
			 form("%s/%s", x, spec_name), len) + len) = '\0';
   }
   
   if (exist_file(spec_file_path) == false) {
       
      debug(cerr, spec_file_path);

      throw(stringException(
          form("missing %s. can't define user_base", spec_file_path)
                           )
           );
   }
 
/*
   char unique_nm[PATHSIZ];
   const char* uid;
   uid = unique_id();
   len = MIN(strlen(base_name) + strlen(uid) + 1, PATHSIZ - 1);
   *((char *) memcpy(unique_nm,
		     form("%s.%s", base_name, uid), len) + len) = '\0';
*/

   first_desc_ptr = 
     f_obj_dict -> init_a_base(spec_file_path, base_path, base_name);

   return true;
}

