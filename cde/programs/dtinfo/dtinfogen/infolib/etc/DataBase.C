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
/* $TOG: DataBase.C /main/5 1998/04/17 11:43:17 mgreess $ 
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/* imported interfaces... */
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "StringList.h"
#include "Token.h"

/* exported interfaces... */
#include "DataBase.h"


#define FRIENDLY_ASSERT(e) \
  if(!(e)) Token::signalError(Token::Internal, Token::Fatal, \
			      __FILE__, __LINE__, \
			      "assertion failed: " #e);

DB::DB(const char *name)
{
  int len = strlen(name);
  f_name = new char[len + 1];
  *((char *) memcpy(f_name, name, len) + len) = '\0';
}


static int isdir(const char* filename)
{
  int ret = 0;
  struct stat sb;

  if(stat(filename, &sb) == 0){
    if(S_ISDIR(sb.st_mode)){
      ret = 1;
    }
  }

  return ret;
}


static void makedir(const char *path) /* throw(PosixError) */
{
  if(mkdir((char*)path, DATABASE_DIRECTORY_MODE) != 0){
    throw(PosixError(errno, path));
  }
}


DBTable *DB::table(const char *tname, int scode, int cols,
		   int access)
     /* throw(PosixError); */
{
  DBTable *ret = 0; /* keep compiler happy */

  switch(access){
  case CREATE:
    if(!isdir(f_name)){
      makedir(f_name);
    }

    ret = new DBTable(this, scode, cols, tname);
    break;

  case READ:
    ret = new DBTable(this, scode, cols, tname);
    ret->file(DB::READ);
    break;

  default:
    abort();
  }

  return ret;
}



DBTable::DBTable(DB *database, int schema_code, int cols, const char *name)
{
  f_database = database;
  f_schema_code = schema_code;
  f_cols = cols;

  int len = strlen(name);
  f_name = new char[len + 1];
  *((char *) memcpy(f_name, name, len) + len) = '\0';

  f_file = NULL;
  f_start = 0;
}


DBTable::~DBTable()
{
  if(f_file && strcmp(f_name, DATABASE_STDIO) != 0) fclose(f_file);
  delete f_name; 
}


FILE *
DBTable::file(DB::Access access)
{
  if(!f_file){
    if(strcmp(f_name, DATABASE_STDIO) == 0){

      f_file = access == DB::CREATE ? stdout : stdin;

    }else{

      const char *p = f_database->path();
      int pathlen = strlen(p) + 1 + strlen(f_name) + 1;
      char *path = new char[pathlen];
      snprintf(path, pathlen, "%s/%s", p, f_name);
      
      f_file = fopen(path, access == DB::CREATE ? "w" : "r");
      
      if(!f_file){
	throw(PosixError(errno, path));
      }

      delete path;
    }
  }

  return f_file;
}


//----------------------------------------------------------
void DBTable::insert(int typecode, ...)
{
  FILE *out = file(DB::CREATE);
  va_list ap;

  va_start(ap, typecode);

  fprintf(out, "%d\n%d\n", f_schema_code, f_cols);

  int cols_found = 0;
  
  while(typecode != 0){
    switch(typecode){
    case STRING_CODE:
      {
	const char *str = va_arg(ap, const char*);
	fprintf(out, "%d\n%ld\t%s\n", STRING_CODE, (long)strlen(str), str);
      }
      break;

    case -STRING_CODE:
      {
	const char *str = va_arg(ap, const char*);
	size_t len = va_arg(ap, size_t);
	fprintf(out, "%d\n%ld\t", STRING_CODE, (long)len );
	fwrite ( str, len, 1, out );
	fputc( '\n', out);
      }
      break;

    case COMPRESSED_STRING_CODE:
      {
	const char *comp_agent  = va_arg(ap, const char* );
	const char *str = va_arg(ap, const char* );

	fprintf(out, "%d\n%s\n%ld\t%s\n", COMPRESSED_STRING_CODE,comp_agent,
		(long)strlen(str), str );
      }
      break;

   case -COMPRESSED_STRING_CODE:
      {
	const char *comp_agent  = va_arg(ap, const char* );
	const char *str = va_arg(ap, const char* );

	size_t len = va_arg(ap, size_t );
	fprintf(out, "%d\n%s\n%ld\t", COMPRESSED_STRING_CODE,
		                     comp_agent,
		                     (long)len
		);

	fwrite( str, len, 1, out );
	fputc('\n', out);
      }
      break;
	

    case OID_CODE:
      {
	const char *oid = va_arg(ap, const char*);
	fprintf(out, "%d\n%s\n", OID_CODE, oid);
      }
      break;
      
    case INTEGER_CODE:
      {
	int x = va_arg(ap, int);
	fprintf(out, "%d\n%d\n", INTEGER_CODE, x);
      }
      break;

    case SHORT_LIST_CODE:
      {
	int qty = va_arg(ap, int);
	int code = va_arg(ap, int);

	fprintf(out, "%d\n#\n", SHORT_LIST_CODE);
	
	switch(code){
	case INTEGER_CODE:
	  {
	    int *items = va_arg(ap, int*);

	    for(int i = 0; i < qty; i++){
	      fprintf(out, "%d\n%d\n", code, items[i]);
	    }
	  }
	  break;

	case STRING_CODE:
	  {
	    const char **items = va_arg(ap, const char**);

	    for(int i = 0; i < qty; i++){
	      fprintf(out, "%d\n%ld\t%s\n",
		      code, (long)strlen(items[i]), items[i]);
	    }
	  }
	  break;
	  
	case OID_CODE:
	  {
	    const char **items = va_arg(ap, const char**);

	    for(int i = 0; i < qty; i++){
	      fprintf(out, "%d\n%s\n",
		      code, items[i]);
	    }
	  }
	  break;
	  
	default:
	  fprintf(stderr, "Internal error: unknown database type code: %d\n",
		  code);
	  abort();
	}

	fprintf(out, "#\n");

	break;
      }
      
    case OID_LIST_CODE:
      {
	int qty = va_arg(ap, int);

	fprintf(out, "%d\n#\n", OID_LIST_CODE);
	
	const char **items = va_arg(ap, const char**);

	for(int i = 0; i < qty; i++){
	  fprintf(out, "%s\n", items[i]);
	}

	fprintf(out, "#\n");

	break;
      }
      
    default:
      fprintf(stderr, "Internal error: unknown database type code: %d\n",
	      typecode);
      abort();
    }

    cols_found++;
    
    typecode = va_arg(ap, int);
  }

  va_end(ap);
  
  fflush(out); /* @# some routines are sloppy and don't
		* close their tables!
		*/
  
  assert(cols_found == f_cols);
}



//----------------------------------------------------------
void DBTable::insert_untagged(int typecode, ...)
{
  FILE *out = file(DB::CREATE);
  va_list ap;

  va_start(ap, typecode);

  fprintf(out, "%d\n", f_schema_code);

  if(f_start){
    fprintf(out, "+");
    f_start = 0;
  }

  int cols_found = 0;
  
  while(typecode != 0){
    switch(typecode){
    case STRING_CODE:
      {
	const char *str = va_arg(ap, const char*);
	fprintf(out, "%ld\t%s\n", (long)strlen(str), str);
      }
      break;

    case -STRING_CODE:
      {
	const char *str = va_arg(ap, const char*);
	size_t len = va_arg(ap, size_t);
	fprintf(out, "%ld\t", (long)len );
	fwrite ( str, len, 1, out );
	fputc( '\n', out);
      }
      break;

    case OID_CODE:
      {
	const char *oid = va_arg(ap, const char*);
	fprintf(out, "%s\n", oid);
      }
      break;
      
    case INTEGER_CODE:
      {
	int x = va_arg(ap, int);
	fprintf(out, "%d\n", x);
      }
      break;

    default:
      fprintf(stderr, "Internal error: unknown database type code: %d\n",
	      typecode);
      abort();
    }

    cols_found++;
    
    typecode = va_arg(ap, int);
  }

  va_end(ap);
  
  fflush(out); /* @# some routines are sloppy and don't
		* close their tables!
		*/
  
  assert(cols_found == f_cols);
}



//----------------------------------------------------------
void DBTable::start_list()
{
  this->f_start = 1;
}


//----------------------------------------------------------
void DBTable::end_list()
{
  fprintf(file(DB::CREATE), "-\n");
  f_start = 0;
}


//----------------------------------------------------------
DBCursor::DBCursor(DBTable &t)
{
  f_table = &t;
  f_start = -1;
  
  f_fields = new StringList();
  f_list = NULL;
}


//----------------------------------------------------------
DBCursor::~DBCursor()
{

  // this is for the last record
  delete f_fields;
  if ( f_list ) delete f_list;
}


//----------------------------------------------------------

void DBCursor::string_field(FILE *fp, char **out, int *lenOut)
{
  int len = 0;
  int io;

  /* fscanf is wierd, so we do it ourselves... */
  while(isdigit(io = fgetc(fp))){
    len = len * 10 + (io - '0');
  }
  FRIENDLY_ASSERT(io == '\t');
  
  char *str = new char[len + 1];
  io = fread(str, sizeof(str[0]), len+1, fp); /* read \n also */

  FRIENDLY_ASSERT(io == len+1);

  str[len] = 0; /* replace \n with 0 (just in case...) */

  if(out){
    f_fields->add(str);
    *out = str;

    if(lenOut) *lenOut = len;
  }else{
    delete str;
  }
}



void DBCursor::int_field(FILE *fp, int *out)
{
  int an_int;
  int io;
  
  io = fscanf(fp, "%d\n", &an_int);

  FRIENDLY_ASSERT(io == 1);
	  
  if(out) *out = an_int;
}


void DBCursor::short_list(FILE *fp, int *qout, int ltype, void *out)
{
  int c;
  int ret;
      
  c = fgetc(fp);
  FRIENDLY_ASSERT(c == '#');

  c = fgetc(fp);
  FRIENDLY_ASSERT(c == '\n');
      
  switch(ltype){
  case STRING_CODE:
    {
      typedef const char** ccpp;
      ccpp *cpout = (ccpp*)out;
	  
      assert(f_list == NULL); /* only one SHORT_LIST per record supported */
      f_list = new StringList;

      while((c = fgetc(fp)) != '#'){
	char *item;
	int ftype;
	
	ungetc(c, fp);
	ret = fscanf(fp, "%d\n", &ftype);
	if(ret == 0) throw(PosixError(errno, "Unable to fscanf\n"));
	FRIENDLY_ASSERT(ftype == STRING_CODE);
	
	string_field(fp, &item, NULL);
	f_list->append(item);
      }

      *cpout = f_list->array();
      *qout = f_list->qty();
    }
    break;
	
  default:
    abort(); /* only strings supported */
  }

  c = fgetc(fp);

  FRIENDLY_ASSERT(c == '\n');
}


int DBCursor::next(int typeCode, ...)
{
  int ret = 1;
  FILE *fp = f_table->file(DB::READ);
  int io;
  int recordClass;
    
  if(f_start < 0){ /* on first call to next(), reset the file */
    rewind(fp);
  }
  
  f_start = ftell(fp);
  io = fscanf(fp, "%d\n", &recordClass); /* get record code */

  if(io != EOF){ /* got any data? */
    
    FRIENDLY_ASSERT(io == 1);

    // clean up previous fields first if they exist
    f_fields->reset();
    delete f_list; f_list = NULL;

    va_list ap;
    va_start(ap, typeCode);

    int fieldQty;

    io = fscanf(fp, "%d\n", &fieldQty); /* get field count */
    FRIENDLY_ASSERT(io == 1);

    /* iterate over fields in the input stream... */
    while(fieldQty--){
      int fieldCode;
  
      io = fscanf(fp, "%d\n", &fieldCode); /* get field type */
      FRIENDLY_ASSERT(io == 1);
      
      assert(typeCode); /* make sure caller didn't give too few args */
      
      switch(fieldCode){
      case STRING_CODE:
	{
	  char **data = NULL;
	  int *len = NULL;

	  if (fieldCode == typeCode || (fieldCode + typeCode) == 0) {
	    data = va_arg(ap, char**);

	    if (fieldCode != typeCode) {
	      len = va_arg(ap, int*);
	    }
	  }
	  
	  string_field(fp, data, len);
	}
	break;

      case INTEGER_CODE:
	int_field(fp, fieldCode == typeCode ? va_arg(ap, int*) : 0);
	break;

      case SHORT_LIST_CODE:
	{
	  int *qout = va_arg(ap, int *);
	  int ltype = va_arg(ap, int);
	  void *out = va_arg(ap, void*);
	  short_list(fp, qout, ltype, out);
	}
	break;
    
      default:
	abort();
      }

      typeCode = va_arg(ap, int);
    }

    assert(typeCode == 0); /* check for end marker */
    
    va_end(ap);
    
  }else{
    ret = 0; /* EOF found... no record */
  }

  return ret;
}


void DBCursor::undoNext()
{
  FILE *fp = f_table->file(DB::READ);

  if(f_start >= 0){
    if(fseek(fp, f_start, 0) < 0){
      throw(PosixError(errno, f_table->name()));
    }
  }else{
    abort(); /* @# throw("no next to undo!") */
  }
}

//----------------------------------------------------------------
void DBCursor::local_rewind()
{
  f_start = -1;
}

//----------------------------------------------------------------
int DBCursor::tell()
{
  if ( f_start == -1 ) {
    return(0);
  }
  else {
    return f_start;
  }
}

//----------------------------------------------------------------
void DBCursor::seekToRec( int pos )
{
  FILE *fp = f_table->file(DB::READ);
  if ( pos >= 0 ){
    if (fseek(fp, pos, 0) < 0 ) {
      throw(PosixError(errno, f_table->name()));
    }
  }
  else{
    abort();
  }
}  
  
  
  
