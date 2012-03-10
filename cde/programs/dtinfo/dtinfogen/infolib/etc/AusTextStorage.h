/* $XConsortium: AusTextStorage.h /main/3 1996/07/18 16:39:21 drk $ */

#ifndef AUSTEXT_STOR_HDR
#define AUSTEXT_STOR_HDR

class DataRepository;

class AusTextStore {
private:
  FILE *afp;
  char *austext_path;
  static unsigned long f_recordcount;
  
public:

  AusTextStore( const char *BookCasePath, const char *BookCaseName ); 
  /*
   * insert ( "This Book Case Name", 2,
   *           "XmyLcfhalklkoop",
   * 	      "This is the text that the indexing machine will see",
   *	      51 );
   *
   */
  
  void insert( const char *BookShortTitle,
	       const char *BookID,
	       const char *SectionID,
	       const char *SectionTitle,
	       DataRepository *store
    );
  ~AusTextStore();
  
};

#endif

  
