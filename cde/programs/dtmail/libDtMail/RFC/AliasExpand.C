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
/*
 *+SNOTICE
 *
 *
 *	$XConsortium: AliasExpand.C /main/4 1996/04/21 19:49:19 drk $
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

#include <assert.h>

#include "AliasExpand.hh"
#include "RFCImpl.hh"

AliasKey::AliasKey(const char * str)
: ObjectKey("AliasKey")
{
    _str = strdup(str);
}

AliasKey::~AliasKey(void)
{
    free(_str);
}

int
AliasKey::operator==(ObjectKey & other)
{
    AliasKey * ok = (AliasKey *)&other;

    return(strcmp(_str, ok->_str) == 0);
}

int
AliasKey::operator!=(ObjectKey & other)
{
    AliasKey * ok = (AliasKey *)&other;

    return(strcmp(_str, ok->_str) != 0);
}

int
AliasKey::operator<(ObjectKey & other)
{
    AliasKey * ok = (AliasKey *)&other;

    return(strcmp(_str, ok->_str) < 0);
}

int
AliasKey::operator<=(ObjectKey & other)
{
    AliasKey * ok = (AliasKey *)&other;

    return(strcmp(_str, ok->_str) <= 0);
}

int
AliasKey::operator>(ObjectKey & other)
{
    AliasKey * ok = (AliasKey *)&other;

    return(strcmp(_str, ok->_str) > 0);
}

int
AliasKey::operator>=(ObjectKey & other)
{
    AliasKey * ok = (AliasKey *)&other;

    return(strcmp(_str, ok->_str) >= 0);
}

HashVal
AliasKey::hashValue(void)
{
    return(genericHashValue(_str, strlen(_str)));
}

// deleteAllocatedKey: used to iterate through a hash table to remove
// all key values that are allocated during the duration of rfcAliasExpand
//
static int
deleteAllocatedKey(ObjectKey & object, AliasKey *, void *) {
  assert(&object != NULL);
  delete &object;
  return(1);
}

		   
void
rfcAliasExpand(DtMailEnv & error,
	       DtMail::MailRc & mailrc,
	       DtMailAddressSeq & addrs)
{
  HashTable<DtMailBoolean>	been_there(32);
  
  error.clear();
  
  // We will go through each item in the address table.
  // For each address, we will look it up in the alias
  // table. If it exists there, we will check to see if
  // we have expanded before. If not, then we expand and
  // note our visit.
  //
  DtMailEnv lerror;
  
  for (int naddr = 0; naddr < addrs.length(); naddr++) {
    DtMailValueAddress * caddr = addrs[naddr];
    
    const char * alias = mailrc.getAlias(lerror, caddr->dtm_address);
    if (alias) {
      AliasKey * key = new AliasKey(caddr->dtm_address);
      if (been_there.lookup(*key)) {	// already visited address?
	delete key;			// yes: cleanup unneeded key
      }
      else {				// no: expand the alias
	DtMailAddressSeq exp_addrs(32);
	RFCTransport::arpaPhrase(alias, exp_addrs);
	
	for (int cp = 0; cp < exp_addrs.length(); cp++) {
	  DtMailValueAddress * new_addr = new DtMailValueAddress(*exp_addrs[cp]);
	  addrs.append(new_addr);
	}
	
	been_there.set(*key, DTM_TRUE);
      }
      
      addrs.remove(naddr);		// no longer need this address
      naddr -= 1;			// ...
      delete caddr;			// or what it pointed to.
    }
  }
  
  // Finally, as a courtesy to the recipients, we will throw away
  // duplicates that have resulted from our expansion.
  //
  HashTable<DtMailBoolean>	done_that(32);
  for (int cur = 0; cur < addrs.length(); cur++) {
    DtMailValueAddress * caddr = addrs[cur];
    AliasKey * dup_key = new AliasKey(caddr->dtm_address);
    
    if (done_that.lookup(*dup_key)) {
      addrs.remove(cur);
      delete caddr;
      delete dup_key;
      cur -= 1;
    }
    else {
      done_that.set(*dup_key, DTM_TRUE);
    }
  }
}
