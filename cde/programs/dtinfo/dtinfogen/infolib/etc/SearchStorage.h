/* $XConsortium: SearchStorage.h /main/3 1996/07/18 16:49:38 drk $ */

#ifndef SRCH_STOR_HDR
#define SCRH_STOR_HDR

#include <stdio.h>

class SearchStorage {
private:
  FILE *catalogfile;
  char *filteredPath;

public:

  SearchStorage( const char *BookCasePath, const char *BookCaseName ); 
  /*
   * insert ( "This Book Case Name", 2,
   *           "XmyLcfhalklkoop",
   * 	      "This is the text that the indexing machine will see",
   *	      51 );
   *
   */
  
  void insert( const char *BookCaseName,
	       const int   BookNum,
    	       const char *BookShortTitle,
	       const char *SectionID,
	       const char *SectionTitle,
	       const char *buffer,
	       int         size
    );
  ~SearchStorage();
  
};

#endif

  
