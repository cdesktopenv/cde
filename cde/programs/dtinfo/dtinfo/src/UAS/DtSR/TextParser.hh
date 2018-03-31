// $XConsortium: TextParser.hh /main/3 1996/06/11 17:41:47 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __STRING_PARSER_HH__
#define __STRING_PARSER_HH__

class StringParser {

public:

  static const char *
	hilite(const char* text, int n, const char* pats);

  static char *
	brute_force(const char* text, int, const char* pats, int sensitive = 0);

  static const char *
	project_textrun(const char *);

};

#endif
