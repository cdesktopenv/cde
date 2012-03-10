/* $XConsortium: AttributeStore.C /main/3 1996/08/21 15:46:08 drk $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>

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


  
