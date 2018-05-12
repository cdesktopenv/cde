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
 *	$TOG: RFCMIME.hh /main/6 1997/03/28 12:10:13 mgreess $
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

#ifndef I_HAVE_NO_IDENT
#endif

#ifndef _RFCMIME_HH
#define _RFCMIME_HH

#include "RFCFormat.hh"

class RFCMIME : public RFCFormat {
  public:
    RFCMIME(DtMail::Session *);
    ~RFCMIME(void);

    static void readBase64(char * dest, int & size, const char * text,
			   const unsigned long text_len);

    static void readQPrint(char * dest, int & size, const char * text,
			   const unsigned long text_len);

    static void readTextEnriched(char * dest, int & size, const char * text,
			   const unsigned long text_len);

    void writeBase64(Buffer & buf, const char * bp, 
		     const unsigned long len);
  
    void writeQPrint(Buffer & buf, const char * bp, 
		     const unsigned long len);

    // digest is:
    // unsigned char digest[16];
    //
    static void md5PlainText(const char * bp,
			     const unsigned long len,
			     unsigned char * digest);

    enum Encoding {
	MIME_7BIT,
	MIME_8BIT,
	MIME_QPRINT,
	MIME_BASE64
	};

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

    virtual void rfc1522cpy(Buffer & buf, const char * value);

  private:

    void getMIMEType(
			DtMail::BodyPart * bp,
			char * mime_type,
			DtMailBoolean & is_text);
    Encoding getHdrEncodingType(
			const char *,
			const unsigned int,
			DtMailBoolean,
			const char *);
    Encoding getEncodingType(
			const char *,
			const unsigned int,
			DtMailBoolean,
			int *);
    Encoding getClearEncoding(
			const char *,
			const unsigned int,
			int *);
    void writeContentHeaders(
			Buffer & buf,
			const char * type,
			const char * name,
			const Encoding,
			const char * digest,
			DtMailBoolean show_as_attachment,
			int is2022ASCII );
    
    void writePlainText(
			Buffer & buf,
			const char * bp,
			const unsigned long len);
    void owCompat(
		Buffer & buf, char * type, char * name,
		char *contents, unsigned long len);
};

#endif
