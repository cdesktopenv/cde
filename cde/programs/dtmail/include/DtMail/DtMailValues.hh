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
 *	$TOG: DtMailValues.hh /main/7 1997/09/03 17:27:15 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreementbertween
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

#ifndef _DTMAILVALUE_HH
#define _DTMAILVALUE_HH

#include <DtMail/DtVirtArray.hh>

// Dates consist of a date, and an offset from GMT.
//
#include <time.h>

struct DtMailValueDate : public DtCPlusPlusAllocator {
    time_t	dtm_date;
    time_t	dtm_tz_offset_secs;
};

// An address consists of 3 components. The addressee is the string that
// is used by the transport to deliver the mail to the user. The person
// is the string that represents this person in the real world (e.g. Bob Patterson).
// The namespace is used to identify which address space the name belongs to.
// This can be set to DtMailAddressDefault to specify the library should figure
// out the appropriate name space.
//

#define DtMailAddressDefault	"Dt:Mail:Address:Default"

struct DtMailValueAddress : public DtCPlusPlusAllocator {
    char	*dtm_address;
    char	*dtm_person;
    char	*dtm_namespace;

    DtMailValueAddress(void);
    DtMailValueAddress(const DtMailValueAddress &);
    DtMailValueAddress(const char * address,
		       const char * person,
		       const char * nameSpace);

    DtMailValueAddress(const char * address, const int addr_len,
		       const char * person, const int per_len,
		       const char * nameSpace);
    ~DtMailValueAddress(void);
};

class DtMailAddressSeq : public DtVirtArray<DtMailValueAddress *> {
  public:
    DtMailAddressSeq(int size = 8);
    ~DtMailAddressSeq(void);
};

class DtMailValue : public DtCPlusPlusAllocator {
  public:
    DtMailValue(const char * value);
    virtual ~DtMailValue(void);

    virtual operator const char *(void);

    virtual const char * operator= (const char *);

    virtual DtMailValueDate toDate(void);
#ifdef DEAD_WOOD
    virtual void fromDate(const DtMailValueDate &);
#endif /* DEAD_WOOD */

    virtual DtMailAddressSeq * toAddress(void);
#ifdef DEAD_WOOD
    virtual void fromAddress(const DtMailAddressSeq & list);
#endif /* DEAD_WOOD */

    // This method gives the raw format used to store the value.
    //
    virtual const char * raw(void);

  protected:
    char *	_value;
};

class DtMailValueSeq : public DtVirtArray<DtMailValue *> {
  public:
    DtMailValueSeq(int size = 8);
    ~DtMailValueSeq(void);

    void clear(void);
};

#endif
