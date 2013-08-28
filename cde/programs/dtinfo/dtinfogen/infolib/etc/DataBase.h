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
/* $XConsortium: DataBase.h /main/6 1996/10/26 18:17:26 cde-hal $ -*- c++ -*- */

#ifndef __DataBase_h
#define __DataBase_h

#include "Exceptions.hh"
#include "object/c_codes.h" /* mmdb codes */
#include "oliasdb/olias_consts.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

class PosixError : public Exception{

public:

  enum { MAXMSG = 200 };
  
  PosixError(int error_no, const char *msg)
  { f_errno = error_no;
    *((char *) memcpy(f_msg, msg, MAXMSG+1) + MAXMSG+1) = '\0'; };

  const char *msg(void) const { return f_msg; };
  int error_no(void) const { return f_errno; };
  
  DECLARE_EXCEPTION(PosixError, Exception);
  
private:
  char f_msg[MAXMSG+1];
  int f_errno;
};


#define DATABASE_DIRECTORY_MODE 0775

#define DATABASE_STDIO "-"

class DBTable;

class DB{
public:
  /*
   * USE: const char *dir = "/some/dir/for/all/the/tables";
   *      DB *db = DB(dir);
   *      assert(strcmp(db->path(), dir) == 0);
   */
  DB(const char *name); /* a directory, for now */
  ~DB() { if ( f_name ) delete f_name; }
  const char *path(void) { return f_name; };


  typedef enum { READ, CREATE
#if DB_UPDATE
		  , UPDATE
#endif
	       } Access;

  /*
   * USE: DBTable *t = db->table("NodeMeta", NODE_CODE, 4, CREATE);
   *  where NODE_CODE is an MMDB object code, and 4
   *  is the number of "columns" in the table, i.e. the
   *  number of items you're going to pass to each call to
   *  insert()
   *
   * CREATE creates a file named "NodeMeta" in db's directory.
   * READ   opens a file named "NodeMeta" in db's directory.
   */
  DBTable *table(const char *name,
		 int schema_code, int cols,
		 int a = READ); /* throw PosixError */


private:
  char *f_name;
  
};

#define BT_NUM_DOC_FIELDS 6
#define BT_NUM_LOCATOR_FIELDS 3
#define BT_NUM_GRAPHIC_FIELDS 6
#define BT_NUM_STYLESHEET_FIELDS 3
#define BT_NUM_OLIAS_NODE_FIELDS 7

class DBTable{
friend class DB;
friend class DBCursor;

public:

  ~DBTable();
  
  /*
   * USE: int intlist = 4;
   *      table->insert(INTEGER_CODE, 4,
   *                    STRING_CODE, "xyz",
   *                    -STRING_CODE, "abcd", 4,
   *                    SHORT_LIST_CODE, 1, INTEGER_CODE, &intlist,
   *                    NULL);
   */
  void insert(int typecode, ...);

  void insert_untagged(int typecode, ...);

  const char *name() { return f_name; };

  void start_list();
  void end_list();
  
protected:
  DBTable(DB* database, int schema_code, int cols, const char *name);

  FILE *file(DB::Access);
  
private:
  FILE *f_file;
  DB *f_database;
  int f_schema_code;
  int f_cols;
  char *f_name;
  int f_start; /* at start of linked list */
};


class DBCursor{
 public:
  DBCursor(DBTable &t);
  ~DBCursor();

  /*
   * USE: DBTable t(...);
   *      DBCurcor c(t);
   *      const char *f1;
   *      int f2
   *
   *      while(c.next(STRING_CODE, &f1,
   *                   INTEGER_CODE, &f2,
   *                   NULL)){
   *        ...use f1, f2...
   *      }
   */
  int next(int code, ...);

  void undoNext(); /* seek back to the beginning of the last record read
		    * throw(PosixError) on unseekable device
		    */
  void local_rewind(); /* restart at the beginning of the table file */
  int  tell();         /* current position of the file */
  void seekToRec( int pos ); /* seek directly to pos */

 protected:
  void string_field(FILE *, char **, int *);
  void int_field(FILE *, int *);
  void short_list(FILE *, int *, int, void*);

 private:
  DBTable *f_table;
  class StringList *f_fields;
  class StringList *f_list;
  long f_start;
};


#endif /* __DataBase_h */
