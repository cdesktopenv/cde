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
 *	$XConsortium: RFCFormat.hh /main/4 1996/04/02 16:20:29 mgreess $
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

#ifndef _RFCFORMAT_HH
#define _RFCFORMAT_HH

#include <DtMail/DtMail.hh>
#include <DtMail/Buffer.hh>

class RFCFormat : public DtCPlusPlusAllocator {
  public:
    RFCFormat(DtMail::Session * session);
    ~RFCFormat(void);

    virtual void msgToBuffer(DtMailEnv & error,
			     DtMail::Message & msg,
			     DtMailBoolean include_content_length,
			     DtMailBoolean include_unix_from,
			     DtMailBoolean process_ignores,
			     Buffer & headers,
			     Buffer & body);
           //call setIsWriteBcc when log message to files so that the BCC
           // field will log to those files. 
    void setIsWriteBcc() { _is_write_bcc = DTM_TRUE;}
    void unsetIsWriteBcc() { _is_write_bcc = DTM_FALSE;}
    DtMailBoolean getIsWriteBcc() { return _is_write_bcc;}
  
  protected:
    DtMail::Session	*_session;
    DtMailBoolean	_use_cr;
    DtMailBoolean	_is_write_bcc;  // if log to file: Yes
                                        //     send out: No (RFC 822)


    virtual void formatBodies(DtMailEnv & error,
			      DtMail::Message & msg,
			      DtMailBoolean include_content_length,
			      char ** extra_headers,
			      Buffer & buf) = 0;

    virtual void formatHeaders(DtMailEnv & error,
			       DtMail::Message & msg,
			       DtMailBoolean include_unix_from,
			       const char * extra_headers,
			       Buffer & buf) = 0;

    void writeHeaders(DtMailEnv & error,
		      DtMail::Message & msg,
		      DtMailBoolean include_unix_from,
		      const char * extra_headers,
		      const char ** suppress_headers,
		      Buffer & buf);

    virtual void rfc1522cpy(Buffer & buf, const char * value);
    void getCharSet(char * charset);
    void getCharSet(char * charset, char *special);
	int OpenLcxDb(void);
	void DtXlateStdToOpLocale(char *op, char *std, char *dflt, char **ret);
	void DtXlateOpToStdLocale(char *op, char *opLoc, char **retLoc, char **ret_retLang, char **retSet);
	char *targetTagName();

    void crlf(Buffer & buf)
  {
      const char * term = (_use_cr ? "\r\n" : "\n");
      int len = (_use_cr ? 2 : 1);
      buf.appendData(term, len);
  }

};

inline void
appendString(Buffer & buf, const char * str)
{
    buf.appendData(str, strlen(str));
}

#endif
