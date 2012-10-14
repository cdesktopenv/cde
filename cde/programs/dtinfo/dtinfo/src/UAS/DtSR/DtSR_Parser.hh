// $XConsortium: DtSR_Parser.hh /main/4 1996/10/16 14:24:46 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __DtSR_Parser_HH__
#define __DtSR_Parser_HH__

#include "Registration.hh"

#include "UAS_String.hh"
#include "UAS_OQLParser.hh"

class DtSR_Parser : public UAS_OQLParser
{
public:
  DtSR_Parser();

  int stemming_suggested() { return f_completion_specified; }

private:
  UAS_String se_construct (OQL_Token, UAS_String, UAS_String, UAS_String);

  UAS_String validate(UAS_String &seql);

private:

  // flag to see if completion has been specified in a query
  int f_completion_specified;
  int f_completion_specified_transient;
};

#endif
