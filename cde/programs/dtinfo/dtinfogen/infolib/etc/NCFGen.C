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
/* $XConsortium: NCFGen.C /main/9 1996/08/21 15:47:02 drk $ */

#include <stdio.h>
#include <stdlib.h>
#if !defined(__uxp__) && !defined(USL)
#include <strings.h>
#endif
#include <sstream>
using namespace std;

#include "Exceptions.hh"
#include "DataBase.h"
#include "BookCaseDB.h"
#include "Task.h"
#include "Handler.h"
#include "StyleValidate.h"
#include "Token.h"

/* MMDB interfaces */
#include "oliasdb/mmdb.h"
#include "oliasdb/asciiIn_filters.h"
#include "oliasdb/olias_consts.h"
#include "oliasdb/stylesheet_hd.h"


/* Hash table interfaces */
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
#include "BTCollectable.h"

#ifdef FISH_DEBUG
#include "dbug.h"
#endif

#define SKIP_CODE -1
static unsigned hash_func(const CC_String &str)
{
  return str.hash();
}

//-------------------------------------------------------------------------
static void
writeStyleSheets(BookCaseDB& db)
{
  DBTable *out = db.DB::table(DATABASE_STDIO,
			      STYLESHEET_CODE, BT_NUM_STYLESHEET_FIELDS,
			      DB::CREATE);

  DBTable *in = db.table(BookCaseDB::StyleSheet, DB::READ);
  DBCursor cursor(*in);

  const char *name;
  const char *online;
  int len_o;
  const char *print;
  int len_p;

  int statusO = 0;
  int statusP = 0;

  
  while(cursor.next(STRING_CODE, &name,
		    -STRING_CODE, &online, &len_o,
		    -STRING_CODE, &print, &len_p,
		    NULL)){
    if( statusO=validate_stylesheet( online, len_o, ONLINE )){
      Token::signalError(Token::User, Token::Continuable, 0, 0,
			 "Online style sheet for `%s' is invalid.", name);
    }
    
    if( statusP=validate_stylesheet( print, len_p, PRINT )){
      Token::signalError(Token::User, Token::Continuable, 0, 0,
			 "Print style sheet for `%s' is invalid.", name);
    }

    if ( statusO || statusP ) {
      throw(Unexpected("Style sheet validation failed\n"));
    }
    
    out->insert(STRING_CODE, name,
		-STRING_CODE, online, len_o,
		-STRING_CODE, print, len_p,
		NULL);
  }

  delete out;
}


//-------------------------------------------------------------------------
static void
buildNCF(BookCaseDB& db, const char *base_name, int compressed)
{
  DBTable *ncf = db.DB::table(DATABASE_STDIO,
			      OLIAS_NODE_CODE, BT_NUM_OLIAS_NODE_FIELDS,
			      DB::CREATE);

  DBTable *nodeMeta = db.table(BookCaseDB::NodeMeta, DB::READ);
  DBCursor cursor(*nodeMeta);


  const char *bookLocator;
  const char *nodeLocator;
  const char *filename;
  int         line_num;
  const char *title;
  const char *stitle;
  const char *style;
  int dupID = 0;

  OLIAS_DB mmdb_handle;
  info_lib *mmdb = 
    mmdb_handle.openInfoLib(getenv("MMDB_PATH"), (char*)base_name);
  info_base *base_ptr = mmdb->get_info_base(base_name);

  const int BUFSIZE=30;

  hashTable<CC_String,BTCollectable> node_dict(hash_func);    // Hash table...
  
  if ( compressed ) {

    // 30 will be enough for now.    
    const int COMPRESSED_AGENT_SIZE=30;
    
    char comp_agent[COMPRESSED_AGENT_SIZE];

    // was bzero before, but unable to find bzero on solaris
    for ( int i = 0; i < COMPRESSED_AGENT_SIZE; i++ ) {
        comp_agent[i] = 0;
    }

    ostringstream str_buf( comp_agent );
    handler *x = (base_ptr->get_obj_dict()).get_handler(
      form("%s.%s", base_name, "sgml.dict"));
    x->its_oid().asciiOut(str_buf);
    

    while(cursor.next(STRING_CODE, &bookLocator,
		      STRING_CODE, &nodeLocator,
		      STRING_CODE, &filename,
		      INTEGER_CODE, &line_num,
		      SKIP_CODE, /* TOC num */
		      STRING_CODE, &title,
		      STRING_CODE, &stitle,
		      STRING_CODE, &style,
		      NULL)){


      CC_String *key = new CC_String(nodeLocator);

      // check for duplicate node locator 
      BTCollectable *val = node_dict.findValue( key );
      if ( val ) {
	delete key;
	dupID++;
	
	cerr << "(ERROR) Duplicate section ID    = " << nodeLocator << endl
	     << "               found in file    = " << filename << endl
	     << "                     at line    = " << line_num << endl
	     << "         is in conflict with    " << endl
	     << "                  section ID    = " << nodeLocator << endl
	     << "                     in file    = " << val->filename() << endl
	     << "                     at line    = " << val->linenum() << "\n\n";


      }
      else {
	
	BTCollectable *val = new BTCollectable( filename, 
						line_num,
						bookLocator);
	node_dict.insertKeyAndValue( key, val );

      }

      stylesheet_smart_ptr sheet(base_ptr, style);
      ostringstream strout;
      sheet.its_oid().asciiOut(strout);

      ncf->insert(STRING_CODE, nodeLocator,
		  STRING_CODE, title,
		  STRING_CODE, stitle,
		  COMPRESSED_STRING_CODE, comp_agent, "",
		  STRING_CODE, bookLocator,
		  OID_CODE, "0.0", /* pointer to Book/CCF/DOC object */
		  OID_CODE, (char *)strout.str().c_str(),
		  NULL);
    }

  }
  else {
    
    while(cursor.next(STRING_CODE, &bookLocator,
		      STRING_CODE, &nodeLocator,
		      STRING_CODE, &filename,
		      INTEGER_CODE, &line_num,
		      SKIP_CODE, /* TOC num */
		      STRING_CODE, &title,
		      STRING_CODE, &stitle,
		      STRING_CODE, &style,
		      NULL)){

      CC_String *key = new CC_String(nodeLocator);

      // check for duplicate node locator 
      BTCollectable *val = node_dict.findValue( key );
      if ( val ) {
	delete key;
	dupID++;

	
	cerr << "(ERROR) Duplicate section ID    = " << nodeLocator << endl
	     << "               found in file    = " << filename << endl
	     << "                     at line    = " << line_num << endl
	     << "         is in conflict with    " << endl
	     << "                  section ID    = " << nodeLocator << endl
	     << "                     in file    = " << val->filename() << endl
	     << "                     at line    = " << val->linenum() << "\n\n";

      }
      else {
	
	BTCollectable *val = new BTCollectable( filename, 
						line_num, 
						bookLocator );
	node_dict.insertKeyAndValue( key , val );

      }
      
      
      stylesheet_smart_ptr sheet(base_ptr, style);
      ostringstream strout;
      sheet.its_oid().asciiOut(strout);
      
      ncf->insert(STRING_CODE, nodeLocator,
		  STRING_CODE, title,
		  STRING_CODE, stitle,
		  STRING_CODE, "",
		  STRING_CODE, bookLocator,
		  OID_CODE, "0.0", /* pointer to Book/CCF/DOC object */
		  OID_CODE, (char *)strout.str().c_str(),
		  NULL);
    }
    
  }

  if ( dupID ) {
    throw(Unexpected(
      form("Number of duplicated section ID found = %d", dupID)
      ));
  }

  delete ncf;
  node_dict.clearAndDestroy();
}

//-------------------------------------------------------------------------
static void
usage(const char *progname)
{
  fprintf(stderr, "usage: %s [-compressed] [-load-style] <bookcasename> <bookcasedir>\n", progname);
  exit(1);
}

//-------------------------------------------------------------------------
main(int argc, char **argv)
{
  INIT_EXCEPTIONS();

  set_new_handler( FreeStoreException );
  
  int ret = 1;
  const char *progname = argv[0];
  int compressed = 0;
  int load_style_only = 0;

#ifdef FISH_DEBUG
  DBUG_PROCESS(argv[0]);
  if(getenv("FISH_DBUG")) DBUG_PUSH(getenv("FISH_DBUG"));
#endif
  
  argv++;
  argc--;
  
  while(argc > 0 && argv[0][0] == '-'){
    const char *opt = argv[0];
    argv++;
    argc--;
    
    if(strcmp(opt, "-compressed") == 0){
      compressed = 1;
    }
    else if ( strcmp(opt, "-load-style") == 0 ) {
      load_style_only = 1;
    }
    else {
      usage(progname);
    }
  }
  
  if(argc == 2){
    const char *base_name = argv[0];
    const char *bookcaseDir = argv[1];

    mtry{
      BookCaseDB db(bookcaseDir);

      if ( load_style_only ) {
	writeStyleSheets(db);
      }
      else {
	buildNCF(db, base_name, compressed);
      }
      ret = 0;
    }

    mcatch(PosixError&, pe){
      fprintf(stderr, "%s: error on %s: %s\n",
	      progname, bookcaseDir, pe.msg());
    }

    mcatch(Unexpected&, pe) {
      fprintf(stderr, "(ERROR) %s\n\n", pe.msg() );
    }
    
    mcatch(mmdbException&, e) {
      cerr << e;
    }
    
    mcatch_any() {
      fprintf(stderr, "*** Internal Error ***: unexpected exception\n");
      abort();
    }end_try;

  }else{
    usage(progname);
  }

  return ret;
}

