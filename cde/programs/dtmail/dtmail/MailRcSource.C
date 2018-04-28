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
/* $TOG: MailRcSource.C /main/13 1999/03/25 13:41:38 mgreess $ */
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

#include <EUSCompat.h>
#include <DtMail/PropSource.hh>
#include <DtMail/DtMailTypes.h>
#include <DtMail/DtVirtArray.hh>
#include "str_utils.h"

#include "DmxPrintOptions.h"

static PropDefaults static_defaults[] ={
{ "retrieveinterval",		"300" },
{ "bell",			"1" },
{ "flash",			"0" },
{ "headerlines",		"15" },
{ "showto",			"f"  },
{ "showmsgnum",			"f"  },
{ "popuplines",			"24" },
{ "toolcols",			"80" },
{ "indentprefix",		">" },
{ "hideattachments",		"f" },
{ "folder",			"~" },
{ "filefolder",			"~" },
{ "dontunifyselection",		"f" },
{ "dontunifyfileselection",	"f" },
{ "confirmattachments",		"f" },
{ "confirmattachmentthreshold",	"64" },
{ "cachedfilemenusize",		"10" },
{ "dontdisplaycachedfiles",	"f" },
{ "record",			"" },
{ "dontlogmessages",		"f" },
{ "keepdeleted",		"f"  },
{ "quietdelete",		"f"  },
{ "expert",			"f"   },
{ "strictmime",                 "f"  },
{ "cdetooltalklock",		"f"  },
{ "allnet",			"f"  },
{ "metoo",			"f"  },
{ "usealternates",              "f"  },
{ "deaddir",                	"~/dead_letter"  },
{ "saveinterval",             	"30" },
{ DMX_PROPKEY_HEADER_LEFT,	DMX_PROPVAL_SUBJECT_HEADER },
{ DMX_PROPKEY_HEADER_RIGHT,	DMX_PROPVAL_EMPTY },
{ DMX_PROPKEY_FOOTER_LEFT,	DMX_PROPVAL_USER_NAME },
{ DMX_PROPKEY_FOOTER_RIGHT,	DMX_PROPVAL_PAGE_NUMBER },
{ DMX_PROPKEY_MARGIN_TOP,	DMX_PROPVAL_DFLT_MARGIN },
{ DMX_PROPKEY_MARGIN_RIGHT,	DMX_PROPVAL_DFLT_MARGIN },
{ DMX_PROPKEY_MARGIN_BOTTOM,	DMX_PROPVAL_DFLT_MARGIN },
{ DMX_PROPKEY_MARGIN_LEFT,	DMX_PROPVAL_DFLT_MARGIN },
{ DMX_PROPKEY_PRINT_HEADERS,	DMX_PROPVAL_STANDARD },
{ DMX_PROPKEY_MESSAGE_SEPARATOR,DMX_PROPVAL_PAGE_BREAK },
{ DMX_PROPKEY_SEPARATOR_STRING,	"-" },
{ NULL,				NULL }
};

DtVirtArray<PropDefaults*> *PropSource::_dynamic_defaults =
	new DtVirtArray<PropDefaults*> (20);

PropSource::PropSource(DtMail::MailRc *m_rc,  char *key)
{
    _mail_rc = m_rc;
    _key = key;
}

PropSource::~PropSource() {;}

void
PropSource::setDefaultValue(char *key, char *value)
{
    PropDefaults *df;

    for (int i=0; i<_dynamic_defaults->length(); i++)
    {
	df = (PropDefaults*) (*_dynamic_defaults)[i];
	if (0 == strcmp(key, df->key))
	{
	    if (NULL != df->value) free((void*) df->value);
	    df->value = strdup(value);
	    return;
	}
    }

    df = (PropDefaults*) malloc(sizeof(PropDefaults));
    df->key = strdup(key);
    df->value = strdup(value);
    _dynamic_defaults->append(df);
}

const char *
PropSource::getDefaultValue(void)
{
    PropDefaults *df;

    for (int i=0; i<_dynamic_defaults->length(); i++)
    {
	df = (PropDefaults*) (*_dynamic_defaults)[i];
	if (0 == strcmp(_key, df->key)) return strdup(df->value);
    }

    for (df=static_defaults; df->key; df++)
      if (strcasecmp(_key, df->key) == 0) return strdup(df->value);

    return strdup(PropSourceDEFAULTVALUE);
}

// MailRcSource::getValue
// gets the value from the mailrc hash table and returns it
/////////////////////////////////////////////////////////////////
const char *MailRcSource::getValue(DtMailBoolean decrypt)
{
  const char *value = NULL;
  DtMailEnv error;

  _mail_rc->getValue(error, _key , &value, decrypt);

  if (value == NULL || error.isSet())
  {
      value = getDefaultValue();
      return value;
  }
  
  return value;
}

// MailRcSource::setValue
// Sets the passed value into the mailrc hash tables
/////////////////////////////////////////////////////////////////
void MailRcSource::setValue(char *value, DtMailBoolean encrypt)
{
  DtMailEnv error;

  const char * d_value = getDefaultValue();

  if (strcmp(d_value, value))
    _mail_rc->setValue(error, _key , value, encrypt);
  else
    _mail_rc->removeValue(error, _key);
}
