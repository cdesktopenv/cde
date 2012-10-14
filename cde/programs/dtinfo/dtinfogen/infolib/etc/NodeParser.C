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
/* $XConsortium: NodeParser.C /main/6 1996/08/21 15:47:06 drk $ */

#include <stdio.h>

#include "dti_cc/CC_Stack.h"
#include "Exceptions.hh"
#include "DataBase.h"

#include "Dispatch.h"
#include "SearchPath.h"

#include "Task.h"
#include "BookTasks.h"
#include "OLAF.h"

#ifdef FISH_DEBUG
#include "dbug.h" /* Fred Fish's dbug.h */
#endif
#include "Handler.h"

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

  if(argc == 4){

    const char *toc_option = argv[1];
    const char *infolib = argv[2];
    const char *srcdir  = argv[3];
    

    Dispatch::tmpdir = infolib;
    Dispatch::srcdir = srcdir;

    if ( !strcmp(toc_option, "toc") ) {
      Dispatch::tocgen_only = 1;
    }
    else if ( !strcmp(toc_option, "all") ) {
      Dispatch::tocgen_only = 0;
    }
    else { 
      fprintf(stderr, "usage: NodeParse [ tocgen_only | all ] <database-dir> <source-dir>\n");

      exit(1);
    }

      
    
    /*
     * Add the . directory as a default if the file is not found in scrdir
     */

    SearchPath *sptable = new SearchPath( srcdir, ".", 0 );
    Dispatch::search_path_table = sptable;

    Task *t = new BookCaseTask( infolib );
    Stack<int> *istack = new Stack<int>;

    Dispatch::setRoot(t, istack);

    mtry{
      extern int yylex();
      
      yylex();

      ret = 0;
    }
    mcatch(Unexpected&, u)
      {
	mtry {
	  Dispatch::tok->reportError(Token::User, Token::Fatal,
				     "markup error: %s", u.msg());
	}
	mcatch(ErrorReported&, e)
	  {
	    if ( e.f_severity == Token::Fatal ) {
	      exit(1);
	    }
	  }end_try;
      }
    mcatch(PosixError&, pe)
      {
	fprintf(stderr, "(ERROR) %s\n", pe.msg() );
	exit(1);
      }
    mcatch(ErrorReported&, e)
      {
	if ( e.f_severity == Token::Fatal ) {
	  exit(1);
	}
      }end_try;

  }else{
    fprintf(stderr, "usage: NodeParse [ tocgen_only | all ] <database-dir> <source-dir>\n");
  }

  return ret;
}
