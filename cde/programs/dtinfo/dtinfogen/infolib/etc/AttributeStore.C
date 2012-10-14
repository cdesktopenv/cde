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
/* $XConsortium: AttributeStore.C /main/3 1996/08/21 15:46:08 drk $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#include "List.h"
#include "AttributeRec.h"
#include "AttributeStore.h"
#include "HashBucketEntry.h"

// Debugging macro
#ifdef DEBUG
#define DBG(level) if ( dbgLevel >= level)
#else
#define DBG(level) if (0)
#endif

//---------------------------------------------------------
// AttributeStore : Constructor

AttributeStore::AttributeStore( int sz ):NumBuckets(sz)
{

  char *dbgStr;
  dbgStr = getenv("OL_DEBUG");
  dbgLevel = ( dbgStr ? atoi(dbgStr):0);

  HashTable = new HashBucketEntry * [ NumBuckets ];
  if ( !HashTable ) {
    cerr << "(ERROR) Unable to allocate memory for HashTable\n";
    exit(1);
  }

  for ( int i=0; i < sz; i++) {
    HashTable[i] = NULL;
  }
  
}

//---------------------------------------------------------
// AttributeStore : Destructor

AttributeStore::~AttributeStore()
{
  for ( int i=0; i < NumBuckets; i++ ) {
    delete HashTable[i];
  }
}

//---------------------------------------------------------
// AttributeStore::hash()
int AttributeStore::hash( char *HashKeyStr )
{
  DBG(10) cerr << "(DEBUG) Invoking hash function with HashKeyStr = "
               << HashKeyStr << endl;
  
  int sum=0;

  // This is a simple hash function
  for ( char *ptr=HashKeyStr; *ptr; ptr++ ) {
    sum += *ptr;
  }

  sum = sum % NumBuckets;

  DBG(10) cerr << "(DEBUG) returning value for hash function = "
	       << sum << endl;
  return ( sum );
}
  
    
//---------------------------------------------------------
// AttributeStore::lookup
char *AttributeStore::lookup( Token *ThisToken, char *aName )
{
  int HashValue;
  AttributeRec *aRecPtr;
  
  HashValue=hash( aName );
  aRecPtr = HashTable[ HashValue ]->lookup( aName );

  return ( aRecPtr->GetAttributeValue( ThisToken ) );
}

//---------------------------------------------------------
// AttributeStore::add

int AttributeStore::insert ( AttributeRec *att )
{

  int HashValue;
  HashBucketEntry *BucketEntry;

  HashValue = hash ( att->name );
  
  BucketEntry = HashTable [ HashValue ];
  
  if ( BucketEntry->lookup ( att->name ) ) {
    return(0);
  }
  else {
    BucketEntry->insert( att );
    return(1);
  }

}


  
