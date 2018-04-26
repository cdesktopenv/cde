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
/* $XConsortium: StyleTask.h /main/3 1996/08/21 15:47:29 drk $ */
/* $XConsortium: StyleTask.h /main/3 1996/08/21 15:47:29 drk $ */

#ifndef __StyleTasks_h
#define __StyleTasks_h

#include "Task.h"
#include "FlexBuffer.h"
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
#include "dti_cc/CC_Stack.h"

class BookCaseTask;
class DataTask;
class DB;
class DBTable;
class FlexBuffer;
class OL_Data;

class imp_die;

class StyleTask : public ComplexTask{

public:

  StyleTask();
  ~StyleTask();

  /*
   * USE:
   *   StyleTask t;
   *      t.markup(...); ... t.data(...); ...
   *      const char *l = t.locator();
   */
  const char * locator() /* throw(Unexpected) */;

  void reset(void);

  void markup(const Token&);

  void data(const char *, size_t );

  int exist( const char * view ); /*
				   * Given a view determine if it exists? 
				   */

  /*
   * USE:
   *   StyleTask st(...);
   *       st.markup(...); st.data(...); ... // feed stylesheet to the task
   *       const char *p = st.print();
   *       const char *o = st.online();
   */
  const char *print();
  int         print_data_size();

  const char *online();
  int         online_data_size();

protected:
  void write_record( void );

      /* comments below to avoid compiler warnings... */
  virtual void done(const char * /*name*/,
		    const char * /*online*/, int /*online_len*/,
		    const char * /*print*/, int /*print_len*/)  {};

private:
  int
     f_base;                  /* tag nesting level of <STYLESHEET> elt */
  int f_select;

  enum { inPath, startContent, inContent } f_dataMode;

  FlexBuffer
     *f_buffer,
     *f_pathbuf,
     *onlineSS,
     *printSS;		       /*
				* The style sheet data proper.
				* Need to determine if it gets transformed
				* or not.
			        */
  OL_Data
     *f_locator;

  hashTable<CC_String,int>
     *viewset;          	       /*
			        * List of views,
			        * Must be a unique set of names.
			        */
  Stack<int> *feature_depth;

};


class StyleTaskDB : public StyleTask{

public:

  StyleTaskDB(BookCaseTask *);

  BookCaseTask *bookcase() { return f_bookcase; };

protected:
  virtual void done(const char * name,
		    const char * online, int online_len,
		    const char * print, int print_len);

private:
  BookCaseTask
     *f_bookcase;    /* 'parent' bookcase object */

};


#endif /* __StyleTasks_h */

