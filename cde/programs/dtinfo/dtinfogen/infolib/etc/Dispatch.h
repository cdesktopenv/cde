/* $XConsortium: Dispatch.h /main/3 1996/08/21 15:46:29 drk $ -*- c++ -*- */
/* $XConsortium: Dispatch.h /main/3 1996/08/21 15:46:29 drk $ -*- c++ -*- */

//---------------------------------------------------------------------
// Dispatch Functions
//---------------------------------------------------------------------

#ifndef DISPATCH_HEADER
#define DISPATCH_HEADER

#include "Token.h"
#include "Task.h"
#include "FlexBuffer.h"
#include "dti_cc/CC_Stack.h"

class SGMLDefn;
class SearchPath;

class Dispatch {

friend int yylex();
friend int main(int argc, char **argv);

protected:
  static Token *tok;
  static const char *tmpdir;
  static const char *srcdir;
  static SearchPath *search_path_table;
  static int tocgen_only;

  static void token ( TOKEN_TYPE, unsigned char * );
  static void subdoc_start();
  static void subdoc_end();
  static void data  ( FlexBuffer * );
  static void setRoot( Task *t, Stack<int> *istack);
  static void setTempDir ( const char *tmpdir );
  static void setSrcDir  ( const char *srcdir );
  static void entity_decl( SGMLDefn *);

  static void file(const char *);
  static void line(int l) { f_line = l; };
  static void newline() { f_line++; };
  
private:
  static Task *TaskObject;
  static int level;
  static Stack<int> *IgnoreStack;

  static char *f_file;
  static int   f_line;
  
public:  
  static SGMLDefn *entity_ref ( const char *ename );
  static int OutsideIgnoreScope() { return IgnoreStack->empty(); }
  static int       RunTocGenOnly() { return(tocgen_only); }
#ifdef SC3
  static const char *GetTmpDir()       { return( tmpdir ); }
  static const char *GetSrcDir()       { return( srcdir ); } 
  static SearchPath *GetSearchPath()       { return ( search_path_table ); }
#else
  static const char *GetTmpDir() { return( tmpdir ); }
  static const char *GetSrcDir() { return( srcdir ); } 
  static SearchPath *GetSearchPath() { return ( search_path_table ); }
#endif

};

inline
void
Dispatch::data( FlexBuffer *buf )
{
  TaskObject->data( (const char *)( buf->GetBuffer() ),
		    buf->GetSize() );
}

#endif
