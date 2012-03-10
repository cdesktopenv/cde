/*
 * $XConsortium: long_pstring.h /main/5 1996/07/18 14:42:33 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#ifndef _long_pstring_h
#define _long_pstring_h 1

#include "object/pstring.h"
#include "utility/ostring.h"
#include "storage/page_storage.h"
#include "storage/chunks_index.h"


/***************************************
* long string class.
****************************************/

class long_pstring: public pstring
{

public:
   long_pstring(long_pstring&);
   long_pstring(c_code_t = LONG_STRING_CODE);
   long_pstring(char* string, int sz, c_code_t = LONG_STRING_CODE);
   virtual ~long_pstring();

   MMDB_SIGNATURES(long_pstring);

// export function
   Boolean extract(int left, int right, char* sink);

/*
// comparison functions
   Boolean value_LS(root& x, Boolean safe) const;
   Boolean value_EQ(root& x, Boolean safe) const;
*/

// print function
   virtual io_status asciiIn(istream&) ;

protected:
   chunks_index *v_str_index;
};
   
HANDLER_SIGNATURES(long_pstring)
   
#endif
