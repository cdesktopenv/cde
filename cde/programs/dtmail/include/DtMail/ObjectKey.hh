/*
 *+SNOTICE
 *
 *
 *	$XConsortium: ObjectKey.hh /main/4 1996/04/21 19:45:25 drk $
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

#ifndef _OBJECTKEY_HH
#define _OBJECTKEY_HH

#include <DtMail/DtLanguages.hh>

typedef char * ObjectKeyType;
typedef short HashVal;

class ObjectKey : public DtCPlusPlusAllocator {
  public:
    ObjectKey(ObjectKeyType type);
    ObjectKey(ObjectKey &);
    virtual ~ObjectKey(void);

    virtual int operator==(ObjectKey &) = 0;
    virtual int operator!=(ObjectKey &) = 0;
    virtual int operator<(ObjectKey &) = 0;
    virtual int operator>(ObjectKey &) = 0;
    virtual int operator<=(ObjectKey &) = 0;
    virtual int operator>=(ObjectKey &) = 0;

    virtual HashVal hashValue(void) = 0;

  protected:
    ObjectKeyType	_type;
    HashVal genericHashValue(void * buf, int len);
};

#endif
