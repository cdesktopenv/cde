/* $XConsortium: StyleTaskDB.cc /main/2 1996/07/18 15:20:49 drk $ */
/* export... */
#include "StyleTask.h"

/* import... */
#include <assert.h>

#include "BookCaseDB.h"
#include "BookTasks.h"
#include "DataBase.h"

//---------------------------------------------------------------------

StyleTaskDB::StyleTaskDB(BookCaseTask *bc)
: StyleTask()
{
   f_bookcase = bc;
}


void StyleTaskDB::done(const char * name,
		  const char * online, int online_len,
		  const char * print, int print_len)
{
  /*
   * Use -STRING_CODE instead of STRING_CODE to handle 8-bit clean
   * data
   */
  
  DBTable *tbl = f_bookcase->table(BookCaseDB::StyleSheet);
  
  tbl->insert(STRING_CODE, name,
	      -STRING_CODE, online, online_len,
	      -STRING_CODE, print,  print_len,
	      NULL);

  reset();
}
