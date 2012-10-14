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
/* $XConsortium: StyleUpdate.C /main/7 1996/08/21 15:47:33 drk $ */

#include <stdio.h>
#include <iostream>
using namespace std;


#include "Exceptions.hh"
#include "dti_cc/CC_Stack.h"

#include "DataBase.h"
#include "SearchPath.h"

#include "Dispatch.h"
#include "Task.h"
#include "StyleTask.h"
#include "OLAF.h"
#include "Handler.h"
#include "StyleValidate.h"

#include "oliasdb/mmdb.h"
#include "oliasdb/stylesheet_hd.h"
#include "utility/mmdb_exception.h"

#ifdef FISH_DEBUG
#include "dbug.h" /* Fred Fish's dbug.h */
#endif

//---------------------------------------------------------------------
int main(int argc, char **argv)
{
  INIT_EXCEPTIONS();

  /* can't seem to get C++ initialization stuff to do this... */
  OLAF::init();
  
  set_new_handler( FreeStoreException );
  
  int ret = 1;
  
#ifdef FISH_DEBUG
  DBUG_PROCESS(argv[0]);
  if(getenv("FISH_DBUG")) DBUG_PUSH(getenv("FISH_DBUG"));
#endif

  if(argc == 3){
    char *infolibDir = argv[1];
    char *bcname = argv[2];
    OLIAS_DB mmdb_handle;
    info_lib *mmdb = mmdb_handle.openInfoLib(infolibDir, bcname);
    info_base* ibase = mmdb->get_info_base(bcname);

    SearchPath *sptable = new SearchPath( ".", NULL );
    Dispatch::search_path_table = sptable;

    StyleTask *styleTask = new StyleTask( );
    Stack<int> *istack = new Stack<int>;

    Dispatch::setRoot(styleTask, istack);

    mtry{
      mtry{
	extern int yylex();
      
	yylex();
	
	const char *styleName = styleTask->locator();
	
	stylesheet_smart_ptr sty(ibase, styleName);
	
	const char *ps = styleTask->print();
	int ps_size    = styleTask->print_data_size();
	
	const char *os = styleTask->online();
	int os_size    = styleTask->online_data_size();
	
	if(!ps) ps = "";
	if( validate_stylesheet( ps, ps_size, PRINT ) ){
	  Token::signalError(Token::User, Token::Fatal, 0, 0,
			     "Print style sheet is invalid; update aborted.");
	}
	
	if(!os) os = "";
	if( validate_stylesheet( os, os_size, ONLINE ) ){
	  Token::signalError(Token::User, Token::Fatal, 0, 0,
			     "Online style sheet is invalid; update aborted.");
	}
	   
	sty.update_hardcopy_data(ps, ps_size );
	sty.update_online_data(os, os_size );

	ret = 0;

      }mcatch(Unexpected&, u){
	Dispatch::tok->reportError(Token::User, Token::Fatal,
				   "markup error: %s", u.msg());
      }
      mcatch(PosixError&, p){
	Token::signalError(Token::Internal, Token::Fatal, 0, 0,
			   "%s", p.msg() );
      }end_try;
	
    }mcatch(ErrorReported&, e){
      /* error is already reported. */
    }mcatch(mmdbException&, e){
      cerr << e;
    }
    mcatch_any() {
      fprintf(stderr, "*** Internal Error ***: unexpected exception\n");
      abort();
    }end_try;

  }else{
    fprintf(stderr,
	    "usage: sgmls styleheet | StyleUpdate <infolib> <bookcase>\n");
  }

  return ret;
}
