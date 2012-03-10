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

    try{
      extern int yylex();
      
      yylex();

      ret = 0;
    }
    catch(Unexpected&, u)
      {
	try {
	  Dispatch::tok->reportError(Token::User, Token::Fatal,
				     "markup error: %s", u.msg());
	}
	catch(ErrorReported&, e)
	  {
	    if ( e.f_severity == Token::Fatal ) {
	      exit(1);
	    }
	  }end_try;
      }
    catch(PosixError&, pe)
      {
	fprintf(stderr, "(ERROR) %s\n", pe.msg() );
	exit(1);
      }
    catch(ErrorReported&, e)
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
