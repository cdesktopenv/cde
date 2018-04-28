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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: TableTask.C /main/3 1996/08/21 15:47:46 drk $ */

/* exported interfaces... */
#include "TableTask.h"

/* imported interfaces... */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "FlexBuffer.h"
#include "Token.h"
#include "SGMLName.h"

#include "dbug.h"

#define NAMECASE 1


int TableTask::TGroup;
int TableTask::ColSpec;
int TableTask::Row;
int TableTask::TBody;
int TableTask::Entry;


TableTask::TableTask(const Token& t)
{
  static int inited = 0;

  if(!inited){
    TGroup    = SGMLName::intern("TGroup",   NAMECASE);
    ColSpec   = SGMLName::intern("ColSpec",  NAMECASE);
    Row       = SGMLName::intern("Row",      NAMECASE);
    TBody     = SGMLName::intern("TBody",    NAMECASE);
    Entry     = SGMLName::intern("Entry",    NAMECASE);
    inited = 1;
  }
  
  f_base = t.level();
  f_buf = new FlexBuffer();

  state = Start;
  markup(t);


}



TableTask::~TableTask()
{
  delete f_buf;
}


void
TableTask::markup(const Token& t)
{
  int again = 0;

  if(t.type() == END && t.level() == f_base){
    write_record();

    delete this; /* this object deletes itself when it's done. */
    
    return;
  }
  
  do{
    switch(state){
    case Start:
      if(t.type() == START && t.Gi() == TGroup){
	// @@ attributes?

	/*
	 * _@Tab
	 *    @Fmta  {_
	 */
	puts("@Tab\n"
	     " @Fmta {");
	f_cols = 0;
	state = Fmt_;
      }
      else{
	expected("TGROUP", t);
      }


    case Fmt_:
      if(t.Gi() == ColSpec){
	// @@ attributes?

	if(f_cols < 26){
	  /*
	   * @Tab
	   *   @Fmta_  { @Col A_
	   */
	  if(f_cols > 0) puts(" ! ");
	  puts(" @Col ");
	  putC('A' + f_cols);
	  f_cols++;
	}else{
	  error("Too many columns. Maximum is 26.");
	}
      }

      else if(t.Gi() == Row){
	/*
	 * @Tab
	 *   @Fmta  { @Col A ! @Col B ! ..._ }
	 * {
	 * _
	 */
	puts(" }\n"
	     "{\n");
	state = _Row;
      }
      
      else{
	error("Expected <COLSPEC> or <ROW>; found <%s%s>",
	      t.type() == END ? "/" : "", t.giName());
      }
      break;

    case _Row:
      if(t.type() == START && t.Gi() == Row){
	// @@ attributes?

	/*
	 * @Tab
	 *   @Fmta  { @Col A ! @Col B ! ..._ }
	 * {
	 * _ @Rowa _
	 */
	puts("  @Rowa ");
	f_col = 0;
	state = _Entry;
      }
      
      else if(t.type() == END && t.Gi() == TBody){
	/*
	 * @Tab
	 *   @Fmta  { @Col A ! @Col B ! ... }
	 * {
	 *   @Rowa A { abc } B { def } 
	 * _}
	 * _
	 */
	puts("}\n");
	state = End;
      }

      else{
	expected("ROW", t); /*@@ or </TBODY> */
      }
      break;

      
    case _Entry:
      if(t.type() == START && t.Gi() == Entry){
	// @@ attributes?

	if(f_col < f_cols){
	  /*
	   * @Tab
	   *   @Fmta  { @Col A ! @Col B ! ... }
	   * {
	   *   @Rowa _A { _
	   */
	  putC('A' + f_col);
	  f_col ++;
	  puts(" { ");
	  f_entryBase = t.level();
	  state = Object;
	}else{
	  error("Too many entries in row: only %s columns specified",
		f_cols);
	}
      }
      
      if(t.type() == END && t.Gi() == Row){
	/*
	 * @Tab
	 *   @Fmta  { @Col A ! @Col B ! ... }
	 * {
	 *   @Rowa A { abc } B { def } _
	 * _
	 */
	puts("\n");
	f_col = 0;
	state = _Row;
      }
      
      else{
	expected("ENTRY", t);
      }
      break;

    case Object:
      if(t.type() == END && t.level() == f_entryBase){
	/*
	 * @Tab
	 *   @Fmta  { @Col A ! @Col B ! ... }
	 * {
	 *   @Rowa A { object } _
	 */
	puts(" } ");
	state = _Entry;
      }

      else{
	putC(' '); /* @# put whitespace in for markup to make
		    * "a<tag>b" come out as "a b" in stead of "ab"
		    */
	
	/* @@ markup within an entry ignored ?!!? */
      }
      break;

    case End:
      break;
      
    default:
      abort();
    }
  }while(again);
}


void
TableTask::expected(const char *right, const Token& wrong)
{
  error("Expected <%s>; found <%s%s>",
	right, wrong.type() == END ? "/" : "", wrong.giName());
}


void
TableTask::error(const char *fmt, ...)
{
  va_list ap;
  
  va_start(ap, fmt);

  /* @# more info here? */
  vfprintf(stderr, fmt, ap);
  
  va_end(ap);
}


void
TableTask::putC(char c)
{
  f_buf->put(c);
}


void
TableTask::puts(const char *s)
{
  f_buf->writeStr(s);
}


void
TableTask::write(const char *buf, size_t len)
{
  f_buf->write(buf, len);
}


void
TableTask::data(const char *chars, size_t len)
{
  while(len--){
    char c = *chars++;
    switch(c){
    case '/':
    case '|':
    case '&':
    case '{':
    case '}':
    case '#':
    case '@':
    case '^':
    case '"':
    case '\\':
      putC('\\');
      putC(c);
      break;
      
    default:
      putC(c);
    }
  }
}



SearchableTableTask::SearchableTableTask(SearchEngine *s, const Token& t)
: TableTask(t)
{
  f_search = s;
}


void
SearchableTableTask::write_record()
{
  DBUG_PRINT("Table", ("Lout table: `%s'", f_buf->GetBuffer()));
  /*@@ write to BookCaseDB? Convert to PS? */
}


