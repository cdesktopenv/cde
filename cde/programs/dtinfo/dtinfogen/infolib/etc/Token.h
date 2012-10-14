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
/* $XConsortium: Token.h /main/3 1996/08/21 15:47:54 drk $    -*- c++ -*-  */


#ifndef TOKEN_HEADER
#define TOKEN_HEADER

#include <sstream>
using namespace std;

#include "Exceptions.hh"

#include "SGMLDefn.h"
#include "AttributeList.h"

class FlexBuffer;
class AttributeRec;
class SGMLDefn;

enum TOKEN_TYPE {
  TK_INVALID = 0,
  START,
  END,
  EXTERNAL_ENTITY,
  INTERNAL_ENTITY,
  SUBDOC_START,
  SUBDOC_END
  /* eventually... ENTITY, COMMENT */
};

// Token is the data structure for repository storage of the token
class Token {

public:
  Token();

  TOKEN_TYPE type() const       { return tokType; }
  const char *giName() const;
  int Gi() const                { return GiValue; }

  /* find value of OLIAS attribute, or -1 if none */
  int olaf() const              { return f_olaf;  }
  
  const AttributeRec *GetFirstAttr() const;
  const AttributeRec *GetNextAttr( const AttributeRec *) const;
  int                 AttributeMatch( int , int ) const;
  const AttributeRec *LookupAttr( int ) const;

  int         getEntityName()     const   { return entity_name; }
  const char *getEntityFileName() const;
  
  int        level() const                { return f_level; };
  const char *file() const                { return f_file; };
  int         line() const                { return f_line; };

  typedef enum { User, Internal } ErrorSource;
  typedef enum { Fatal, Continuable } ErrorSeverity;

  /* USE:
   *   t.reportError(Token::User, Token::Continuable,
   *                 "this is wrong: %s", some_string);
   * or
   *   Token::reportError(Token::User, Token::Continuable,
   *                      "this is wrong: %s", some_string);
   *
   * (src and sev would be declared using the above enumerations,
   *  but the compiler does strange things with them, so we use int.)
   */
  void  reportError(int src, int sev, const char *fmt, ...);
  /* throw ErrorReported */

  static void signalError(int src, int sev,
			  const char *file, /* can be NULL */
			  int line,
			  const char *fmt, ...);

private:
friend int yylex();
friend class Dispatch;
  
  OL_AttributeList aList;
  int GiValue;
  int entity_name;
  SGMLDefn *entity_defn;
  TOKEN_TYPE tokType;

  int f_level;
  const char *f_file;
  int f_line;
  int f_olaf;
  
protected:
  void StoreAttribute( const char *name, const char *value,
		       int declared_value );
  void StoreStartTag ( unsigned char *, int);
  void StoreEndTag   ( unsigned char *, int);
  void StoreEntity   ( unsigned char *, TOKEN_TYPE entity_type);
  void StoreSubDoc   ( TOKEN_TYPE tokType ); 
  void SetEntityValue( SGMLDefn *defn );

  void setFileLine(char *file /* we keep a pointer to file: don't delete it!*/,
		   int line) { f_file = file; f_line = line; };
  
};  

inline
const char *
Token::getEntityFileName() const
{
  if ( entity_defn ) {
    return( entity_defn->getFileName() );
  }
  else return(NULL);
}

class ErrorReported : public Exception{
public:
  ErrorReported(int source, int severity) /* see ReportError above */
  { f_source = source;
    f_severity = severity;
  };
  
  int f_source;
  int f_severity;

  DECLARE_EXCEPTION(ErrorReported, Exception);
};


#endif
