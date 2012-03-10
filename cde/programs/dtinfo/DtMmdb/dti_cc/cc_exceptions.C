// $XConsortium: cc_exceptions.C /main/4 1996/08/21 15:48:54 drk $

#include "dti_cc/cc_exceptions.h"

ostream& ccException::asciiOut(ostream& out)
{
   out << "ccException::asciiOut() called\n";
   return out;
}

ostream& ccStringException::asciiOut(ostream& out)
{
   out << msg << "\n";
   return out;
}

//////////////////////////////////////////
//////////////////////////////////////////

ostream& ccBoundaryException::asciiOut(ostream& out)
{
   cerr << low << "\t";
   cerr << high << "\t";
   cerr << index << "\n";
   return out;
}

