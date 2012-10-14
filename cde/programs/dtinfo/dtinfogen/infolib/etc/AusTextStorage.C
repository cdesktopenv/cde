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
/* $XConsortium: AusTextStorage.cc /main/5 1996/07/23 18:08:29 cde-hal $
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
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <sstream>
using namespace std;

/* imported interfaces */
#include <misc/unique_id.h>
#include "FlexBuffer.h"
#include "Task.h"
#include "DataBase.h"
#include "DataRepository.h"
#include "api/utility.h"

/* exported interfaces */
#include "AusTextStorage.h"

#ifdef DTSR_USE_CNTR_L
const char CNTR_L = '\014';  /*  This is for ascii system only */
#endif

// If NodeParser ever gets setup to run on all bookcases at one time, we
// will need a reset() function for this member.
unsigned long AusTextStore::f_recordcount = 0;

#ifdef DTSR_LIKES_FGETS
const int LINE_SIZE = 80;    /* this is the line size allowed for data in 
			      * the *.fzk file
			      */

/* Most of the non-alphanumeric character in ascii code set */
const char *DELIMITER_SET = "\t\n !@#$%^&*()_-=+\\|~[]{};:,.<>/?";

enum EucCodeSet {
	CodeSetInv = -1,
	CodeSet0   =  0,
	CodeSet1   =  1,
	CodeSet2   =  2,
	CodeSet3   =  3
};

/* 
 * charcspn determines if ch is found in the set
 * returns 1 if so, 0 if otherwise
 */
/*
 * @@ charset is expensive, alternative approach is to use a 
 * static array 
 * static char char_tab[] = { 0, 0, 0, 1,...... }
 * where 1 indicates the character is in the delimiter character set
 * however, this might not be portable for character set other than
 * ascii , so this has to be done carefully 
 * If the format of the fzk is changed, all this will no longer be
 * required. So, I am not going to do anything at this point 
 */

//-----------------------------------------------------------------
static int charset ( const char ch, const char *set)
{
  for ( const char *ptr = set;
       *ptr != '\0';
	ptr++ ) {
    if ( ch == *ptr ) return 1;
  }
  
  return 0;
}

/*
 * getline returns the no. of bytes that should be read as a line.
 * Normally it should read line_size, but if there is a token that 
 * spans 2 lines, getline need to determine the line size such that
 * at the end of the line, no token should be spanning the next line.
 */
/*
 * start_ptr is the start of the buffer and end_ptr is the end of the buffer
 * it is similar to fread except that end_ptr is supplied as the bounding
 * condition as opposed to the EOF in fread. Besides, no actual character
 * is read , only the number of characters that should be read as a line.
 */
//--------------------------------------------------------------------------

static unsigned int DefaultGetLine ( const char *start_ptr,
			      const char *end_ptr,
			      int line_size )
{
  if ( start_ptr > end_ptr ) { return 0; }
  
  if ( start_ptr + line_size - 1 <= end_ptr ) {  // not @ the end yet
    /* 
     * FIrst see if there is a token that spans multiple lines
     */
    const char *ptr = start_ptr + line_size - 1;
    if ( ptr == end_ptr ) { return line_size; }

    if ( charset( *(ptr+1), DELIMITER_SET ) || charset ( *ptr, DELIMITER_SET ) ) {
      return ( line_size );
    }

    /* That means found a token that spans 2 lines */
    /* So now loop back until *ptr is not in DELIMITER_SET */
    const char *new_end_ptr;
    for ( new_end_ptr = ptr;
	 new_end_ptr > start_ptr && !charset( *new_end_ptr , DELIMITER_SET );
	 new_end_ptr-- );
    
    return( new_end_ptr - start_ptr + 1 );

  }
  else { 
    // last chunk of line
    return ( end_ptr - start_ptr + 1 );
  }
}

inline EucCodeSet JpEucCodeSet(const unsigned char* text)
{
    EucCodeSet codeset;

    if (text == NULL)
	codeset = CodeSetInv;
    else if (*text < 0x80)
	codeset = CodeSet0;
    else if (*text == 0x8E)
	codeset = CodeSet2;
    else if (*text == 0x8F)
	codeset = CodeSet3;
    else {
	assert( *text > 0xA0 && *text < 0xFF);
	codeset = CodeSet1;
    }

    return codeset;
}

static unsigned int JpGetLine ( const char *start_ptr,
			      const char *end_ptr,
			      int line_size )
{
    if (start_ptr > end_ptr)
	return 0;

    if (end_ptr - start_ptr + 1 <= line_size)
	return (end_ptr - start_ptr + 1);

    // reference limit
    const char* limit = start_ptr + line_size;

    EucCodeSet codeset = JpEucCodeSet((const unsigned char*)start_ptr);

    int len;
    const char* p;

    for (p = start_ptr; p < limit; p += len) {

	if (JpEucCodeSet((const unsigned char*)p) != codeset)
	    break;

	if (codeset == CodeSet0)
		len = 1;
	else if ((codeset == CodeSet1) || (codeset == CodeSet2))
		len = 2;
	else if (codeset == CodeSet3)
		len = 3;
	else
		len = 0;

	if ((len == 0) || (p + len - 1 > end_ptr))
	    break;
    }

    return (p - start_ptr);
}
#endif // DTSR_LIKES_FGETS

//-----------------------------------------------------------------------
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

//-----------------------------------------------------------------------
static void makedir(const char *path) /* throw(PosixError) */
{
  if(mkdir((char*)path, 0775) != 0){
    throw(PosixError(errno, path));
  }
}

//-----------------------------------------------------------------------
AusTextStore::AusTextStore( const char *path, const char *name )
{
  if ( !isdir(path) ) {
    makedir(path);
  }

  austext_path = new char [ strlen(path) + 1 + strlen("dtsearch") + 1 ];
  /*
   * throw(ResourceExhausted)
   *
   */
  assert ( austext_path != NULL );
  
  sprintf( austext_path, "%s/dtsearch", path );
  
  if ( !isdir(austext_path) ) {
    makedir(austext_path);
  }

  char *fzk = form("%s/%s.fzk", austext_path, name );

  /* Use append instead because this fzk file is going to be appended 
   * all the time
   */

  afp = fopen ( fzk, "a" );
  if ( !afp ) {
    throw(PosixError(errno, form("unable to open fzk file %s\n", fzk) ) );
  }
}

//-----------------------------------------------------------------------
void
AusTextStore::insert( const char *BookShortTitle,
		      const char *BookID,
		      const char *SectionID,
		      const char *SectionTitle,
		      DataRepository *store
                    )
{

  /* write the abstract and record stuff in the fzk file */
  if ( afp ) {
    
    f_recordcount++;
    /* Record type ie for all the zone content */
    FlexBuffer **table = store->tabbuf();
    for ( int pos=store->Default; 
	  pos < store->Total;
	  pos++ ) {

      if (  table[pos] ) {

	if ( table[pos]->GetSize() > 0 ) {

	  fprintf(afp, " 0,2\n");
	  
	  /* abstract includes SectionID\tBookShortTitle\tSectionTitle */
	  fprintf(afp, "ABSTRACT: %s\t%s\t%s\n", SectionID, 
		  BookShortTitle, 
		  SectionTitle );

	  // first the record type 
	  // The following was unique, but there is a limit to the size of
	  // the key, so let's just use a simple counter.
//	  fprintf(afp, "%s%s%s\n", store->get_zone_name(pos), BookID, SectionID);
	  fprintf(afp, "%s%d\n", store->get_zone_name(pos), (int)f_recordcount);

	  fprintf(afp, "0/0/0~0:0\n"); // null date

	  // Now the actual buffer
	  const char *start_ptr = table[pos]->GetBuffer();
	  const char *end_ptr = start_ptr + table[pos]->GetSize() - 1;

#ifdef DTSR_LIKES_FGETS
	  unsigned int (*getline)(const char *, const char *, int);

	  const char* lang = getenv("LANG");
	  if (lang && !strncmp(lang, "ja", strlen("ja")))
	    getline = JpGetLine;
	  else
	    getline = DefaultGetLine;
	
	  int num_byte;
	  while ( num_byte = getline(start_ptr, end_ptr, LINE_SIZE) ) {
	    if ( !fwrite(start_ptr, num_byte, 1, afp ) ) 
	      {
		throw(PosixError(errno, "unable to write to fzk file\n" ) );
	      }
	    fputc('\n', afp );

	    start_ptr += num_byte;
	  }

	  // for current section and book level scopes, place the book and
	  //  section ids into the indexed data.
	  fprintf(afp, "\n%s\n%s\n", BookID, SectionID);
#else
	  char *ptr = (char*)start_ptr;
	  for (; ptr <= end_ptr; ptr++) {
	    if (*ptr == '\n')
	      *ptr = ' ';
	  }

	  if (fwrite(start_ptr, table[pos]->GetSize(), 1, afp) == 0)
	    throw(PosixError(errno, "unable to write to fzk file\n"));

	  // for current section and book level scopes, place the book and
	  //  section ids into the indexed data.
	  fprintf(afp, "\t%s\t%s", BookID, SectionID);
#endif
	
#ifdef DTSR_USE_CNTR_L
	  // Then the ^L character at the end
	  fprintf(afp, "\n%c\n", CNTR_L );
#else
	  fprintf(afp, "\n");
#endif
	}
      }
    }
  }
}
	
//-----------------------------------------------------------------------
AusTextStore::~AusTextStore()
{
  if ( afp ) { fclose(afp); }
  if ( austext_path ) { delete [] austext_path; }
}

  
