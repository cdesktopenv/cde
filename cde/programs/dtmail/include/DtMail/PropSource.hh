/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
