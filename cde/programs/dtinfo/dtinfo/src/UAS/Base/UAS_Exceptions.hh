// $TOG: UAS_Exceptions.hh /main/4 1998/04/17 11:41:17 mgreess $
#ifndef _UAS_Exceptions_hh_
#define _UAS_Exceptions_hh_

#include "Exceptions.hh"
#include "UAS_String.hh"

#define CASTUASEXCEPT

class UAS_Exception: public Exception {
    protected:
	UAS_String fMessage;
    public:
	DECLARE_EXCEPTION(UAS_Exception, Exception)
	UAS_Exception(const UAS_String&m): fMessage(m) {}
	virtual ~UAS_Exception() {}
	const UAS_String & message () const { return fMessage; }
};

#endif
