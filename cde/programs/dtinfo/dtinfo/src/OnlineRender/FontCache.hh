// $XConsortium: FontCache.hh /main/6 1996/08/30 11:52:57 rcs $
#ifndef _FontCache_hh
#define _FontCache_hh

//#include <StyleSheet/cde_next.h>
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"

class FontEntry ;
class FontList ;
class FontCache
{
public:
  FontCache();
  ~FontCache();

  // returns an xlfd
  // scale factor is current font scale from preferences dialog 
  const char *lookup(const char *family,
		     const char *weight,
		     const char *slant,
		     int	 ptsize,
		     const char *charset,
		     int	 scale_factor,
		     const char *fallback = NULL);
		     

private:
  const char *getxlfd(const char *family, int bold,
		      int italic, const char* charset, int size, 
		      const char *fallback = NULL);

private:
  
  hashTable<FontEntry, char> 	f_cache;
  hashTable<CC_String, FontList>	f_fontlist;

};

class FontEntry
{
public:
  FontEntry(const char *family,
	    const char *weight,
	    const char *slant,
	    const char *charset,
	    int		ptsize);

  FontEntry(const FontEntry &);
  ~FontEntry();

  bool operator ==(const FontEntry &) const;

  unsigned hash() const;

private:
  CC_String	f_family ;
  CC_String	f_weight ;
  CC_String	f_slant ;
  CC_String	f_charset ;
  int		f_ptsize ;

};

class FontList
{
public:
  FontList()			// for scalable fonts 
  : f_names(0), f_count(0)
    {}

  FontList(int count, const char **names) ;
  ~FontList();
  const char **names() { return f_names ; }
  int	       count() { return f_count ; }

  int	       scaleable() { return f_names == 0 ; }
private:
  const char 	**f_names ;
  int		  f_count ;
};



#endif /* _FontCache_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
