/* $TOG: PropSource.hh /main/6 1997/11/13 13:26:08 mgreess $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include "DtMail.hh"
#include <DtMail/DtMailTypes.h>
#include <DtMail/DtVirtArray.hh>

#ifndef _PROPSOURCE_HH
#define _PROPSOURCE_HH

struct PropDefaults {
    const char *	key;
    const char *	value;
};

// Virtual class is base class for props data accessing funcs
////////////////////////////////////////////////////////////////
class PropSource
{
public:

#define PropSourceDEFAULTVALUE	" "

  PropSource(DtMail::MailRc *m_rc,  char *key);
  virtual ~PropSource();

  virtual const char	*getValue(DtMailBoolean decrypt = DTM_FALSE) = 0;
  virtual void		 setValue(char*, DtMailBoolean encrypt=DTM_FALSE) = 0;

  static void		 setDefaultValue(char *key, char *value);
  virtual const char	*getDefaultValue(void);
  virtual DtMail::MailRc *getMailRc(void) { return _mail_rc; };

protected:
  
  char			*_key; // just a pointer NOT our memory
  DtMail::MailRc	*_mail_rc;
  static DtVirtArray<PropDefaults*>
			*_dynamic_defaults;

};


// Accesses the mailrc hash tables...
/////////////////////////////////////////////////////////////////

class MailRcSource : public PropSource {

public:
  
  MailRcSource(DtMail::MailRc *m_rc, char *key) : PropSource(m_rc, key) {;};
  virtual ~MailRcSource() {;}

  virtual const char *getValue(DtMailBoolean decrypt = DTM_FALSE);
  virtual void setValue(char *value, DtMailBoolean encrypt = DTM_TRUE);

private:
};

#endif
