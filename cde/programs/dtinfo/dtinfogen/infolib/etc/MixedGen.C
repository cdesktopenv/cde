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
/* $XConsortium: MixedGen.C /main/10 1996/11/26 12:29:15 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(__uxp__) && !defined(USL)
#include <strings.h>
#endif

#include "Exceptions.hh"
#include "FlexBuffer.h"
#include "oliasdb/mmdb.h"
#include "oliasdb/asciiIn_filters.h"
#include "DataBase.h"
#include "BookCaseDB.h"
#include "StringList.h"
#include "GraphicsTask.h"
#include "Handler.h"

#include "BTCollectable.h"

#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
// #include "BT_StrHashDict.h"

#define SKIP_CODE -1
#define NULL_OID "0.0"
#define NULL_LOCATOR "0"

#ifdef FISH_DEBUG
#include "dbug.h" /* Fred Fish's dbug.h */
#endif

extern void insert_remotelink( hashTable<CC_String,BTCollectable> *, char *,
			       size_t, FlexBuffer *);
static unsigned hash_func(const CC_String &str ) 
{
  return( str.hash() );
}

//------------------------------------------------------------
static
void
checkTocLink( const char *toc_file_name, 
	      hashTable<CC_String,int> &linkTab, 
	      hashTable<CC_String,BTCollectable> &nodeTab )
{
  int flag = 0;

  char *ToCFileName = strdup ( toc_file_name );
  
  hashTableIterator<CC_String,BTCollectable> nodeit( nodeTab );
  while ( ++nodeit ) {

    CC_String *key = (CC_String *)nodeit.key();
    BTCollectable       *value = (BTCollectable *)nodeit.value();

    if ( !linkTab.contains( key ) ) {
      flag = 1;

      const char *file_name = value->filename();
      int line_num = value->linenum();

      cerr << "(ERROR) Section ID = " << (const char *)*key << endl
	   << "        of file    = " << file_name << endl
	   << "        at line    = " << line_num << endl
	   << "        does not have a corresponding link with the value  = " << (const char *)*key << endl
           << "        from the ToC file = " << ToCFileName << "\n\n";

    }
  }

  hashTableIterator<CC_String,int> linkit( linkTab );

  while ( ++linkit ) {
    CC_String *link = linkit.key();
    int *line_num = linkit.value();

    if ( !nodeTab.contains( link ) ) {
      flag = 1;
      cerr << "(ERROR) IDREF       = " << (const char *)*link << endl
	   << "        of ToC file = " << ToCFileName << endl
	   << "        at line     = " << *line_num << endl
           << "        is pointing to an unavailable node\n\n";

    }
  }
  
  delete ToCFileName;

  if ( flag ) {
    throw(Unexpected("TOC validation failed\n"));
  }
}

//------------------------------------------------------------
static
void
create_node_dict( hashTable<CC_String,BTCollectable> &dict,
		  BookCaseDB& db )
{
  DBTable *nodeMeta = db.table(BookCaseDB::NodeMeta, DB::READ);
  DBCursor cursor(*nodeMeta);

  const char *bookLocator;
  const char *nodeLocator;
  const char *filename;
  int line_num;
  const char *title;
  const char *stitle;
  const char *style;

  int throw_exception = 0;
  while(cursor.next(STRING_CODE, &bookLocator,
		    STRING_CODE, &nodeLocator,
		    STRING_CODE, &filename,
		    INTEGER_CODE, &line_num,
		    SKIP_CODE, /* TOC num */
		    STRING_CODE, &title,
		    STRING_CODE, &stitle,
		    STRING_CODE, &style,
		    NULL)){
    CC_String *key = new CC_String ( nodeLocator );
    BTCollectable       *value = new BTCollectable ( filename,line_num,bookLocator );

    BTCollectable    *val = (BTCollectable *)dict.findValue( key );
    if ( !val ) { 
      dict.insertKeyAndValue( key, value );
    }
    else {
      
      delete key;
      delete value;

      throw_exception = 1;
      cerr << "(ERROR) Duplicate section ID = " << (const char *)*key << endl
	   << "            found in file    = " << filename << endl
	   << "                  at line    = " << line_num << endl
	   << "          is in conflict with  " << endl
	   << "                  section ID = " << (const char *)*key << endl
	   << "            found in file    = " << val->filename() << endl
	   << "                  at line    = " << val->linenum() << "\n\n";

    }
  }

  if ( throw_exception ) {
    throw(Unexpected("Duplicate section IDs are found"));
  }



}
  
  
//------------------------------------------------------------
static
void
node_table( DBCursor *node_meta_cursor,
	    const char *this_book,
	    hashTable<CC_String, BTCollectable> &this_node_table )
{
  assert( node_meta_cursor != NULL );
  assert( this_book != NULL );
 
  const char *bookLocator;
  const char *nodeLocator;
  const char *file_name;
  int line_num;
  const char *title;
  const char *stitle;
  const char *style;

  while ( node_meta_cursor->next(STRING_CODE, &bookLocator,
				 STRING_CODE, &nodeLocator,
				 STRING_CODE, &file_name,
				 INTEGER_CODE, &line_num,
				 SKIP_CODE, /* TOC num */
				 STRING_CODE, &title,
				 STRING_CODE, &stitle,
				 STRING_CODE, &style,
				 NULL)){
    /* this book has changed */
    if ( strcmp( bookLocator, this_book ) != 0 ) {
      node_meta_cursor->undoNext();
      break;
    }

    CC_String *key = new CC_String( nodeLocator );
    BTCollectable *value = new BTCollectable( file_name,line_num,bookLocator );
    this_node_table.insertKeyAndValue( key, value );

  }

}

//------------------------------------------------------------
/* MMDB interfaces use char*; I like to use const char *
 * whenever possible
 * Plus, in TOCTask, we write "" to indicate "no locator",
 * but the MMDB code expected "0".
 * so we do the conversion here.
 *
 * Hmmm.... get_oid_2(... "0") doesn't seem to work. I'm going
 * to bypass it and return "0.0" directly.
 */
static const char *
to_oid(info_lib *mmdb, const char *bcname, const char *str)
{
  const char *ret = NULL_OID;
  
  if(*str){
    ret = get_oid_2(mmdb, (char*)bcname, (char*)str);
  }

  return ret;
}

//------------------------------------------------------------
static
void
locator_table( BookCaseDB& db,
	       hashTable<CC_String, BTCollectable> &hd)
{

  
  DBTable *locTable = db.table(BookCaseDB::Locator, DB::READ);
  
  DBCursor cursor(*locTable);

  const char *locator;
  const char *nodeloc;
  const char *reflabel;
  const char *filename;
  int line_num;

  int dup_count = 0;
  
  while(cursor.next(STRING_CODE, &locator,
		    STRING_CODE, &nodeloc,
		    STRING_CODE, &reflabel,
		    STRING_CODE, &filename,
		    INTEGER_CODE, &line_num,
		    NULL)){

  char *buf = new char[strlen(nodeloc) + strlen(reflabel) + 2];
  sprintf(buf, "%s\t%s", nodeloc, reflabel);
    
    CC_String *loc_collect = new CC_String( locator );
    BTCollectable *node_collect = new BTCollectable( filename, 
						     line_num, 
						     buf );

  delete[] buf;

    BTCollectable *val = hd.findValue( loc_collect );
    if ( !val ) {
      hd.insertKeyAndValue( loc_collect, node_collect );
    }
    else {

      delete loc_collect;
      delete node_collect;

      dup_count++;

      
      cerr << "(ERROR)   Duplicate ID  = " << locator << endl
           << "          found in file = " << filename << endl
	   << "                at line = " << line_num << endl
	   << "    is in conflict with   " << endl
	   << "                    ID  = " << locator << endl
	   << "          found in file = " << val->filename() << endl
	   << "                at line = " << val->linenum()  << "\n\n";
      

    }
      
  }

  if ( dup_count ) {
    throw(Unexpected(form("Number of duplicate IDs found = %d\n", 
			  dup_count)));
  }
}  
//------------------------------------------------------------
static void
writeCCF(BookCaseDB& db,
	 info_lib *mmdb,
	 const char *bcname )
{

  DBTable *bookMeta = db.table(BookCaseDB::BookMeta, DB::READ);
  DBCursor cursor(*bookMeta);
  DBTable *ccf = db.DB::table(DATABASE_STDIO,
			      DOC_CODE, BT_NUM_DOC_FIELDS,
			      DB::CREATE);

  const char *bookLocator;
  const char *stitle;
  const char *title;
  int seq_no;
  int tabQty;
  const char **tabLines;
  const char *access;

  /*
   * First put the global node table into hash dictionary
   */

  hashTable<CC_String,BTCollectable> global_node_tab(hash_func);
  create_node_dict( global_node_tab , db);  /* throw exception if duplicate
					   node locator is found */
  
  int exception_flag = 0;
  while(cursor.next(STRING_CODE, &bookLocator,
		    STRING_CODE, &stitle,
		    STRING_CODE, &title,
		    INTEGER_CODE, &seq_no,
		    SHORT_LIST_CODE, &tabQty, STRING_CODE, &tabLines,
		    STRING_CODE, &access,
		    NULL)){

    StringList heap;

    /* convert tab to oids in forst tabQty items in heap */
    for(int i = 0; i < tabQty; i++){

      char *t = (char *)tabLines[i];                   

      const char *name = strtok( t, "\t" );
      const char *loc  = strtok( NULL, "\t");
      const char *line = strtok( NULL, "\t");
      const char *file_name = strtok( NULL, "\t");

      int nameLen = strlen( name );

      /*
       * First check if the tab link is resolved
       */

      CC_String key ( loc );
      
      BTCollectable *tab_link = (BTCollectable *)global_node_tab.
	                                            findValue( &key );


      if ( !tab_link ) {
	cerr << "(ERROR)         Tab ID = " << loc << endl
	     << "               of book = " << title << endl
	     << "     specified in file = " << file_name << endl
	     << "               at line = " << line << endl
	     << "     is not pointing to any section ID found in the book\n\n";	
	exception_flag = 1;
      }
      else {
	/*
	 * see if it is a node locator within the same book
	 */
	if ( strcmp( tab_link->get_value(), bookLocator ))  {
	  cerr << "(ERROR)         Tab ID = " << loc << endl
	       << "               of book = " << title << endl
	       << "     specified in file = " << file_name << endl
	       << "               at line = " << line << endl
	       << "     is not pointing to any section ID found in the book\n\n";

	  exception_flag = 1;
	}
      }
	
      // if exception_flag is set, calling to_oid will throw exception
      // It will just loop through all the tabs, and report all the bad ones
      if ( !exception_flag ) {
	const char *tabOID = to_oid(mmdb, bcname, loc);
	char *result = new char[nameLen + 1 + strlen(tabOID) + 1];
	sprintf(result, "%s\t%s", name, tabOID );
	heap.add(result);
      }
    }

    if ( !exception_flag ) {
  
      const char *bookOID = heap.append(to_oid(mmdb, bcname, bookLocator));
      
#ifdef FISH_DEBUG
      DBUG_PRINT("CCF", ("Load Book: O:%s `%s'\n", bookOID, title));
#endif
    
      ccf->insert(OID_CODE, bookOID,
		  STRING_CODE, stitle,
		  STRING_CODE, title,
		  INTEGER_CODE, seq_no,
		  SHORT_LIST_CODE, tabQty, STRING_CODE, heap.array(),
		  STRING_CODE, access,
		  NULL);
    }
  }

  if ( exception_flag ) {
    throw(Unexpected("Tab validation failed"));
  }

  delete ccf;
  global_node_tab.clearAndDestroy();
}


//------------------------------------------------------------
static void
writeTOC(BookCaseDB &db,
	 info_lib *mmdb,
	 const char *bcname,
	 const char *thisBook,
	 DBCursor &toc_cursor )
{
  DBTable *out = db.DB::table(DATABASE_STDIO,
			      TOC_CODE, NUM_TOC_FIELDS,
			      DB::CREATE);
  const char *aBook;
  const char *nodeLoc;
  const char *parent;
  int childQty;
  char **children;
  int treeSize;
  
  while(toc_cursor.next(STRING_CODE, &aBook,
			STRING_CODE, &nodeLoc,
			STRING_CODE, &parent,
			SHORT_LIST_CODE, &childQty, STRING_CODE, &children,
			INTEGER_CODE, &treeSize,
			NULL)){
    StringList heap;
    
    if(strcmp(aBook, thisBook) != 0){ /* book id has changed! We're done... */
      toc_cursor.undoNext();
      break;
    }

    for(int i = 0; i < childQty; i++){
      heap.append(to_oid(mmdb, bcname, children[i]));
    }
    
    const char *nodeOID = heap.append(to_oid(mmdb, bcname, nodeLoc));
    const char *parentOID = heap.append(to_oid(mmdb, bcname, parent));
    
#ifdef FISH_DEBUG
    DBUG_PRINT("TOC", ("TOC Entry: O:%s treesize: %d\n", nodeOID, treeSize));
#endif
    
    out->insert(OID_CODE, nodeOID,
		OID_CODE, parentOID,
		INTEGER_CODE, treeSize,
		/* first childQty strings in heap are oids for children */
		OID_LIST_CODE, childQty, heap.array(),
		NULL);
  }

  delete out;
}


//------------------------------------------------------------
static void
writeDLP(BookCaseDB &db, info_lib *mmdb,
	 const char *bcname,
	 const char *thisBook,
	 DBCursor &dlp_cursor,
	 hashTable<CC_String,BTCollectable> &node_tab)
{
  DBTable *out = db.DB::table(DATABASE_STDIO,
			      DLP_CODE, 1,
			      DB::CREATE);

  const char *aBook;
  const char *nodeLoc;
  int   line_num;
  const char *toc_file_name;

  hashTable<CC_String, int> link_table(hash_func);
  
  int record_pos = dlp_cursor.tell();
  
  // create the link table  
  while(dlp_cursor.next(STRING_CODE, &aBook,
			STRING_CODE, &nodeLoc,
			INTEGER_CODE, &line_num,
			STRING_CODE,  &toc_file_name,
			NULL)){
    
    if(strcmp(aBook, thisBook) != 0){ /* book id has changed! We're done... */
      dlp_cursor.undoNext();
      break;
    }

    CC_String *key = new CC_String ( nodeLoc );
    link_table.insertKeyAndValue( key, new int(line_num) );
      
#ifdef FISH_DEBUG
    // DBUG_PRINT("DLP", ("DLP Entry: O:%s\n", nodeOID));
#endif
  }

  checkTocLink( toc_file_name, link_table, node_tab ); /* throw exception if
							  unresolved TOC links
							  are found */
  link_table.clearAndDestroy();
  
  dlp_cursor.seekToRec( record_pos );
  out->start_list();
  while ( dlp_cursor.next( STRING_CODE, &aBook,
			   STRING_CODE, &nodeLoc,
			   INTEGER_CODE, &line_num,
			   STRING_CODE, &toc_file_name,
			   NULL)) {

    if(strcmp(aBook, thisBook) != 0){ /* book id has changed! We're done... */
      dlp_cursor.undoNext();
      break;
    }
    const char *nodeOID = to_oid(mmdb, bcname, nodeLoc);
    out->insert_untagged(OID_CODE, nodeOID,
			   NULL);
#ifdef FISH_DEBUG
    DBUG_PRINT("DLP", ("DLP Entry: O:%s\n", nodeOID));
#endif
  }

  out->end_list();

  /* exception thrown in this if failed */
  delete out;
  
}

//------------------------------------------------------------
// Parsing functions for LAST indicator determination
//------------------------------------------------------------
char *
getGI( char * bufptr )
{
  char* cptr;
  for (cptr = bufptr; *cptr != '>'; cptr++) {
  }
  *cptr = '\0';
  char * retval = strdup(bufptr);
  *cptr = '>';
  return(retval);
}

char *
getIndicator( char * bufptr )
{
  char *iptr = bufptr;
  // Find the end of attributes, assuming that every start tag now has
  // attributes because of the last indicator at least.
  while ((*iptr != '<') ||
	 (*(iptr+1) != '/') ||
	 (*(iptr+2) != '#') ||
	 (*(iptr+3) != '>'))  {
    iptr++;
  }
  // Work back to the indicator value (subtract "</#LAST>" and 1 for the
  // digit)
  iptr = iptr - 9;
  return(iptr);
}

void
lookahead( char * bufptr, char * currentgi, char * indpos)
{
  char * laptr = bufptr;
  while (*laptr != '<') {
    laptr++;
  }
  laptr++;
  if (*laptr == '/') {
    // end tag ahead so the current indicator must be changed to say last
    // on level.
    *indpos = '2'; // LASTONLEVEL
  } else {
    // get the startag gi and compare it against the one passed in
    char * nextgi = getGI(laptr);
    if (strcmp(currentgi, nextgi) != 0) {
      // next start tag is different, so the current GI is the last sibling
      *indpos = '1';  // LASTSIBLING
    }
    // otherwise the next start tag is the same as the current one, so no
    // change to the indicator is needed.
    delete nextgi;
  }
  return;
}

char *
parse4last( char * bufptr )
{
  char
    *startname,
    *indicator;

  while (*bufptr != 0) {
    if (*bufptr == '<') {
      // TAG
      bufptr++;
      if (*bufptr != '/') {
	// START TAG
	startname = getGI(bufptr);
	bufptr = bufptr + strlen(startname);
	indicator = getIndicator(bufptr);
	// move buffer pointer past the end of the attributes.  Assuming
	// that indicator is pointing at the character position where the
	// value is stored we need to move past it and the characters
	// "</#LAST></#>"
	bufptr = indicator + 13;
	// parse the content of this tag past its end tag and when it
	// returns get to the end of where that parse got to.
	bufptr = parse4last( bufptr );
	// Handle the end tag
	if (*bufptr == 0) {
	  // end of the buffer, so it is automatically last...
	  *indicator = '2';  // LASTONLEVEL
	} else {
	  lookahead(bufptr, startname, indicator);
	}
	delete startname;
      } else {
	// END TAG
	for ( ; *bufptr != '>'; bufptr++) {
	  // Just move the pointer past the tag name.
	}
	bufptr++; // Get past the '>'
	return(bufptr);
      }
    } else {
      // Data
      bufptr++;
    }
  }
  return(bufptr);
}

//------------------------------------------------------------
//------------------------------------------------------------
static void
writeSGML(BookCaseDB &db,
	  const char *thisBook,
	  DBCursor &sgml_cursor,
	  hashTable<CC_String,BTCollectable> &hd)
{
  DBTable *out = db.DB::table(DATABASE_STDIO,
			      SGML_CONTENT_CODE, 2,
			      DB::CREATE);

  const char *aBook;
  const char *nodeLoc;
  size_t dataLen;
  const char *data;
  char *bufptr;		// Walk through SGML data stream and update LAST fields
  char *nevermind;	// We don't care about the return value from the parse

  while(sgml_cursor.next(STRING_CODE, &aBook,
		    STRING_CODE, &nodeLoc,
		    -STRING_CODE, &data, &dataLen,
		    NULL)){

    if(strcmp(aBook, thisBook) != 0){ /* book id has changed! We're done... */
      sgml_cursor.undoNext();
      break;
    }

#ifdef FISH_DEBUG
    DBUG_PRINT("SGML", ("SGML data for: ::%s `%.40s...' \n",
			nodeLoc, data));
#endif

    FlexBuffer new_node_buffer;
    
    bufptr = (char *) data;
    nevermind = parse4last( bufptr );

    insert_remotelink( &hd, (char *)data, dataLen, &new_node_buffer);
    
    out->insert_untagged(STRING_CODE, nodeLoc,
			 -STRING_CODE,
			 new_node_buffer.GetBuffer(),
			 new_node_buffer.GetSize(),
			 NULL);
  }

  delete out;
}


//------------------------------------------------------------
static void
writeGraphics(BookCaseDB &db, const char *thisBook, DBCursor &gr_cursor,
	      int compressed, char *comp_agent)
{
  DBTable *out = db.DB::table(DATABASE_STDIO,
			      GRAPHIC_CODE, BT_NUM_GRAPHIC_FIELDS,
			      DB::CREATE);

  const char *aBook;
  const char *gid;
  const char *name;
  const char *version;
  const char *typeInfo;
  const char *data;
  int len;
  const char *title;
  
  while(gr_cursor.next(STRING_CODE, &aBook,
		       STRING_CODE, &gid,
		       STRING_CODE, &name,
		       STRING_CODE, &version,
		       STRING_CODE, &typeInfo,
		       -STRING_CODE, &data, &len,
		       STRING_CODE, &title,
		       NULL)){


    if(strcmp(aBook, thisBook) != 0){ /* book id has changed! We're done... */
      gr_cursor.undoNext();
      break;
    }

#ifdef FISH_DEBUG
    DBUG_PRINT("Graphics", ("Graphics data for: ::%s `%.40s...' \n",
			    gid, data));
#endif

    if ( typeInfo[0] - '0'  == GR_TYPE_POSTSCRIPT &&
	 compressed ) {

      out->insert(STRING_CODE, gid,
		  STRING_CODE, name,
		  STRING_CODE, version,
		  STRING_CODE, typeInfo,
		  -COMPRESSED_STRING_CODE, comp_agent, data, len,
		  STRING_CODE, title,
		  NULL);
    }
    else {
      out->insert(STRING_CODE, gid,
		  STRING_CODE, name,
		  STRING_CODE, version,
		  STRING_CODE, typeInfo,
		  -STRING_CODE, data, len,
		  STRING_CODE, title,
		  NULL);
    }
  }

  delete out;

}


//------------------------------------------------------------
static void
writeBooks(BookCaseDB& db,
	   info_lib *mmdb,
	   const char *bcname,
	   DBCursor *node_meta_cursor,
	   int compressed,
	   char *comp_agent,
	   hashTable<CC_String,BTCollectable> &hd)
{
  DBTable *bookMeta = db.table(BookCaseDB::BookMeta, DB::READ);
  DBCursor BookCursor(*bookMeta);
  
  DBTable *toc = db.table(BookCaseDB::TOCTree, DB::READ);
  DBCursor TocCursor(*toc);
  
  DBTable *dlp = db.table(BookCaseDB::TOCPath, DB::READ);
  DBCursor DlpCursor(*dlp);
  
  DBTable *sgml = db.table(BookCaseDB::NodeSGML, DB::READ);
  DBCursor SgmlCursor(*sgml);

  DBCursor *GraphicsCursorPtr;
  
  int process_graphics = 1;
  mtry {
    DBTable *graphics = db.table(BookCaseDB::Graphics, DB::READ);
    GraphicsCursorPtr = new DBCursor( *graphics );
  }
  mcatch (PosixError&, p){
    /* error opening graphics stuff... skip graphics */

    process_graphics = 0;
  }end_try

  const char *bookLocator;
  int tabQty;
  char **tabLines;
  
  while(BookCursor.next(STRING_CODE, &bookLocator,
			SKIP_CODE,
			SKIP_CODE,
			SKIP_CODE,
			/* @# skipping shortlists not suported yet... */
			SHORT_LIST_CODE, &tabQty, STRING_CODE, &tabLines,
			SKIP_CODE, /* skip access data */
			NULL)){

    hashTable<CC_String,BTCollectable> node_tab(hash_func);
    node_table( node_meta_cursor, bookLocator, node_tab);

    writeDLP(db, mmdb, bcname, bookLocator, DlpCursor, node_tab);
    writeTOC(db, mmdb, bcname, bookLocator, TocCursor);    
    writeSGML(db, bookLocator, SgmlCursor, hd);

    if ( process_graphics ) {
      writeGraphics(db, bookLocator, *GraphicsCursorPtr,
		    compressed, comp_agent );
    }

    node_tab.clearAndDestroy();
      
  }

  if ( process_graphics ) { delete GraphicsCursorPtr; }
  
}


//------------------------------------------------------------
static void
writeLCF(BookCaseDB& db, info_lib *mmdb,
	 const char *bcname,
	 hashTable<CC_String,BTCollectable> &hd)
{

  DBTable *out = db.DB::table(DATABASE_STDIO,
			      LOCATOR_CODE, BT_NUM_LOCATOR_FIELDS,
			      DB::CREATE);
  
  hashTableIterator<CC_String,BTCollectable> loc_it( hd );

  while ( ++loc_it ) {
    CC_String *locator_str = ( CC_String *)loc_it.key();
    const char *locator = (const char *)*locator_str;

    BTCollectable *loc_val = ( BTCollectable *)loc_it.value();
    const char *opaque, *nodeloc;
    char* reflabel;
    nodeloc = opaque = (const char *)loc_val->get_value();
    reflabel = strchr((char*)opaque, '\t');
    *reflabel++ = 0;
    
    const char *nodeOID = to_oid(mmdb, bcname, nodeloc);
    
#ifdef FISH_DEBUG
    DBUG_PRINT("LCF", ("LCF entry: L:%s->O:%s\n",
		       locator, nodeOID));
#endif
    out->insert(STRING_CODE, locator,
		OID_CODE, nodeOID,
		STRING_CODE, reflabel,
		NULL);
    
  }

  delete out;
  
}



//------------------------------------------------------------
static void
split_path(char * path, const char*& dir, const char*& name)
{
  char *p = strrchr(path, '/');

  if(p){
    *p = 0;
    dir = path;
    name = p+1;
  }else{
    dir = name = path;
  }
}


//------------------------------------------------------------
static void
usage(const char *progname)
{
  fprintf(stderr,
	  "usage: %s [-compressed] <tmpBCdir> <infobasedir>\n", progname);
  exit(1);
}


main(int argc, char **argv)
{
  INIT_EXCEPTIONS();
  
  set_new_handler ( FreeStoreException );

  int ret = 1;
  const char *progname = argv[0];
  int compressed = 0;
  
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
    else{
      usage(progname);
    }
  }
  
  if(argc == 2){
    char *bookcaseDir = argv[0];
    char *infobaseDir = argv[1];
    
    mtry{
      BookCaseDB db(bookcaseDir);

      const char *infolibDir;
      const char *bcname;
      split_path(infobaseDir, infolibDir, bcname);
      
      OLIAS_DB mmdb_handle;
      info_lib *mmdb = mmdb_handle.openInfoLib(infolibDir, bcname);
      
// 30 will be enough for now
#define COMPRESSED_AGENT_SIZE 30

      char comp_agent[ COMPRESSED_AGENT_SIZE ];
      
      if ( compressed ) {
        for ( int i = 0; i < COMPRESSED_AGENT_SIZE; i++ ) {
            comp_agent[i] = 0;
        }

	ostringstream str_buf( comp_agent );

	info_base *bcptr = mmdb->get_info_base(bcname);
	handler *x = 
	  (bcptr->get_obj_dict()).get_handler(
	    form("%s.%s", bcname, "ps.dict"));

	x->its_oid().asciiOut(str_buf);
      }

      hashTable<CC_String, BTCollectable> hd(hash_func);
      locator_table( db, hd); /* throw exception if duplicate
					       locator is found */
      
      DBTable *nodeMeta = db.table(BookCaseDB::NodeMeta, DB::READ);
      DBCursor node_cursor( *nodeMeta );
      
      writeCCF(db, mmdb, bcname);
      writeBooks(db, mmdb, bcname, &node_cursor, compressed, comp_agent, hd);
      writeLCF(db, mmdb, bcname, hd);

      hd.clearAndDestroy();
      ret = 0;

    }
    mcatch(PosixError&, pe){
      fprintf(stderr, "%s: error on %s: %s\n",
	      progname, bookcaseDir, pe.msg());
    }
    mcatch(Unexpected&, pe) {
      fprintf(stderr, "(ERROR) %s\n\n", pe.msg() );
      exit(1);
    }end_try;

  }else{
    usage(progname);
  }

  return ret;
}

