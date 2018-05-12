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
 *	$XConsortium: SunV3.hh /main/4 1996/04/21 19:50:07 drk $
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

#ifndef _SUNV3_HH
#define _SUNV3_HH

#include "RFCFormat.hh"

class SunV3 : public RFCFormat {
  public:
    SunV3(DtMail::Session *);
    ~SunV3(void);


    static void decode(const char * enc_info, 
		       char ** outputBp, 
		       int & outputLen, 
		       const char * inputBp, 
		       const unsigned long inputLen);
    static void encode(Buffer & buf,
		       const char * path,
		       const char * bp, 
		       const unsigned long len);

  protected:
    virtual void formatBodies(DtMailEnv & error,
			      DtMail::Message & msg,
			      DtMailBoolean include_content_length,
			      char ** extra_headers,
			      Buffer & buf);

    virtual void formatHeaders(DtMailEnv & error,
			       DtMail::Message & msg,
			       DtMailBoolean include_unix_from,
			       const char * extra_headers,
			       Buffer & buf);

  private:

    static int uudecode(char ** outputBp, 
			 int & outputLen, 
			 const char * inputBp, 
			 const unsigned long inputLen);

    static int uncompress(char ** outputBp, 
			 int & outputLen, 
			 const char * inputBp, 
			 const unsigned long inputLen);

    void encode_uue_line(Buffer & buf,
		     const unsigned char * unencodedBp,
		     const unsigned long unencodedLen);

    void uuencode(Buffer & buf,
		  const char * path,
		  const char * bp,
		  const unsigned long len);
    
    void getV3Type(DtMail::BodyPart *, char *);
};

#endif
