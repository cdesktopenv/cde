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
 *	$TOG: RFCImpl.hh /main/22 1998/10/01 17:28:08 mgreess $
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

#ifndef _RFCIMPL_HH
#define _RFCIMPL_HH

#include <DtMail/Buffer.hh>
#include <DtMail/DtMail.hh>
#include <DtMail/DtMailServer.hh>
#include <DtMail/FileShare.hh>
#include <DtMail/Threads.hh>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#if defined(NEED_MMAP_WRAPPER)
extern "C" {
#endif
  #include <sys/mman.h>
#if defined(NEED_MMAP_WRAPPER)
}
#endif


// Structs needed to exec sendmail
typedef struct waitentry {
        struct waitentry * next;
        int pid;
        void * data;
        SubProcessFinishedProc * proc;
} waitentry_t;
 
typedef struct pipedata {
        int pid;
        int status;
} pipedata_t;

typedef enum { 
  DTMBX_LONGLOCK_FAILED_CANCEL,	   // failed to obtain lock, cancel operation
  DTMBX_LONGLOCK_FAILED_READONLY,  // failed to obtain lock, open read only
  DTMBX_LONGLOCK_FAILED_READWRITE, // failed to obtain lock,
				   // open read write [user locking override]
  DTMBX_LONGLOCK_SUCCESSFUL	   // obtained the lock
} DTMBX_LONGLOCK;

class RFCMailBox;
class RFCBodyPart;


class RFCMessage : public DtMail::Message {
  public:
    RFCMessage(DtMailEnv & error,
	       DtMail::MailBox * parent,
	       const char ** start, // Also returns the end of the message.
	       const char * end_of_file);

    RFCMessage(DtMailEnv & error,
	       const char * alt_start,
	       const char * alt_end);
  
    RFCMessage(DtMailEnv & error,
	       DtMail::Session * session,
	       DtMailObjectSpace space,
	       void * arg,
	       DtMailCallback cb,
	       void * client_data);

    virtual ~RFCMessage(void);
    
    virtual DtMail::Envelope * getEnvelope(DtMailEnv &);
    
    // This should only be used when you REALLY need to know
    // how many body parts are present before actually traversing
    // the body parts (i.e. like when writing a MIME format message).
    virtual int getBodyCount(DtMailEnv &);
    
    virtual DtMail::BodyPart * getFirstBodyPart(DtMailEnv &);
    virtual DtMail::BodyPart * getNextBodyPart(DtMailEnv &,
					       DtMail::BodyPart *);

    virtual DtMail::BodyPart * newBodyPart(DtMailEnv &,
					   DtMail::BodyPart *);

#ifdef DEAD_WOOD
    virtual void newBodyPartOrder(DtMailEnv &,
				  DtMail::BodyPart *,
				  const int) { };
#endif /* DEAD_WOOD */

    virtual void setFlag(DtMailEnv &,
			 const DtMailMessageState);
    
    virtual void resetFlag(DtMailEnv &,
			   const DtMailMessageState);
    
    virtual DtMailBoolean flagIsSet(DtMailEnv &,
				    const DtMailMessageState);

    virtual time_t getDeleteTime(DtMailEnv &);
    
    virtual void toBuffer(DtMailEnv & error, DtMailBuffer &);

    virtual const char * impl(DtMailEnv &);

    // Methods following this point are specific to RFCMessage.
    //
    void markDirty(const int delta);

    void fixMessageLocation(char ** msgHeaderStart, long & msgHeaderLen,
			    char ** msgBodyStart, long &msgBodyLen,
			    int & msgTemporary, long & msgBodyOffset);

    void unfixMessageLocation(char *msgStart, int msgTemporary);

    void adjustMessageLocation(
			char * oldStart,
			char * newStart,
			long newLength,
			int msgTemporary,
			long newBodyOffset);

    void pinMessageDown(char ** msgHeaderStart, long & msgHeaderLen,
			char ** msgBodyStart, long &msgBodyLen);
  
    RFCMailBox * parent(void) { return (RFCMailBox *)_parent; }

  protected:
    unsigned long	_object_signature;
    const char *	_msg_start;
    const char *	_msg_end;
    const char *	_body_start;
    SafeScalar<int>	_dirty;
    DtMailBuffer	*_msg_buf;

    struct BodyPartCache : public DtCPlusPlusAllocator {
	RFCBodyPart	*body;
	const char *	body_start;
    };

    DtVirtArray<BodyPartCache *>	_bp_cache;

    // _alternativeMultipart -- does message contain alternative messages?
    // If DTM_FALSE: this message does not contain alternative messages.
    //   .. can ignore: _alt_msg_cache, _alternativeValid
    // If DTM_TRUE: this message does contain alternative messages.
    //   The body part cache is allocated; however, the pointers within
    //   the cache are copied from the latest valid alternative message
    //   within the alternative message cache, and so should not be
    //   deleted when a message is destroyed and this is true.
    //   The alternative message cache (_alt_msg_cache) contains one or
    //   more messages that map 1 to 1 with the corresponding alternative
    //   messages contained within the entire message.
    //   
    DtMailBoolean	_alternativeMultipart;

    // _alternativeMessage -- is this message an alternative message?
    // If DTM_FALSE: this is not an alternative message but rather a real
    //   message -- _msg_start, _msg_end, _body_start, etc. are valid.
    // If DTM_TRUE: this message is an alternative message; _msg_start,
    //   _msg_end, _body_start, etc. demark the alternative message within
    //   the entire real message. The body part cache is fully allocated and
    //   represents all body parts contained within this alternative message.
    //
    DtMailBoolean	_alternativeMessage;

    // _alternativeValid -- is this alternative message part valid?
    // If DTM_FALSE: this alternative cannot be displayed or otherwise
    //   acted upon on this system
    // If DTM_TRUE: this alternative can be displayed or otherwise
    //   acted upon on this system
    //
    DtMailBoolean	_alternativeValid;
    
    // _alt_msg_cache is the "alternative message cache", an array of
    //   MIMEBodyParts an RFCMessages, which have _alternativeMessage
    //   set, and contain only those body parts that are associated with a
    //   single alternative message. There is one entry in the array for
    //   each alternative message within the entire message. Each alternative
    //   message will have _alternativeMessage == DTM_TRUE.
    // This cache is present only if _alternativeMultipart == DTM_TRUE.
    //
    struct AlternativeMessageCache : public DtCPlusPlusAllocator {
      RFCMessage *	amc_msg;     	// -> RFCMessage for alternative
      RFCBodyPart *	amc_body;	// -> MIMEBodyPart for alternative
      const char *	amc_body_start;	// -> first byte of body
      const char *	amc_body_end;	// -> last byte of body
    };
  
    DtVirtArray<AlternativeMessageCache *>	_alt_msg_cache;

    DtMail::BodyPart * bodyPart(DtMailEnv & error, const int slot);

    int lookupByBody(DtMail::BodyPart *);

    const char * parseMsg(DtMailEnv &, const char * end_of_file);
    const char * findMsgEnd(DtMailEnv &, const char * end_of_file);
    void parseBodies(DtMailEnv &);
    void parseMIMEBodies(DtMailEnv &);
    void parseMIMETextPlain(DtMailEnv &);
    void parseMIMEMultipartSubtype(DtMailEnv &, const char * subtype);
    void parseMIMEMessageSubtype(DtMailEnv &, const char * subtype);
    void parseMIMEMessageExternalBody(DtMailEnv &);
    void parseMIMEMultipartMixed(DtMailEnv &, const char * boundary);
    void parseMIMEMultipartAlternative(DtMailEnv &, const char * boundary);
    void parseV3Bodies(DtMailEnv &);

    void writeMsg(char * buffer);

    int sizeMIMEBodies(DtMailEnv &);
    int sizeV3Bodies(DtMailEnv &);

    char * extractBoundary(const char * content_type);
    DtMailBoolean hasHeaders(const char * buf, const unsigned long size);

    // This friend declaration is used temporarily by copyMsg to copy
    // messages from RFC->RFC. A more general solution is needed for
    // FCS.
friend class RFCMailBox;
};

class RFCEnvelope : public DtMail::Envelope {
  public:
    RFCEnvelope(DtMailEnv & error,
		DtMail::Message * parent,
		const char * start,
		const int len);
    
    virtual ~RFCEnvelope(void);

    virtual DtMailHeaderHandle getFirstHeader(DtMailEnv &,
					      char ** name,
					      DtMailValueSeq & value);
    
    virtual DtMailHeaderHandle getNextHeader(DtMailEnv &,
					     DtMailHeaderHandle last,
					     char ** name,
					     DtMailValueSeq & value);
    
    virtual void getHeader(DtMailEnv &,
			   const char * name,
			   const DtMailBoolean abstract,
			   DtMailValueSeq & value);
    
    virtual void setHeaderSeq(DtMailEnv &, 
			      const char *,
			      const DtMailValueSeq &) {} ;
    
    // The last parameter is left to the client to provide
    // because it can not be done in a type safe manner.
    //
    virtual void setHeader(DtMailEnv &, 
			   const char *, 
			   const DtMailBoolean,
			   const char *);
    
    virtual void removeHeader(DtMailEnv &,
			      const char *);

    // Methods below this point are specific to RFCEnvelope.
    //
    int dirty(void) { return _dirty; }

    void adjustHeaderLocation(char * headerStart, int headerLength);
  
    char *writeHeaders(char *buf);

    const char * headerLocation(void) { return _header_text; }
  
    long headerLength(void) { return _header_len; }
    
    const char * unixFrom(DtMailEnv &, int & length);
    //
    // fix for the defect 177527
    // when a reply-to field is in a message headers, the reply-to is
    // going to be displayed as the send in RMW's msg list scrolled window
    // instead of the real sender. The following three public methods are
    // going to access _use_reply_to.
    // 
    void setUseReplyTo(void) { _use_reply_to = DTM_TRUE; }
    void unsetUseReplyTo(void) {  _use_reply_to = DTM_FALSE; }
    DtMailBoolean getUseReplyTo(void) { return _use_reply_to; }


  protected:

    DtMailBoolean       _use_reply_to;
    unsigned long	_object_signature;
    SafeScalar<int>	_dirty;

    struct ParsedHeader : public DtCPlusPlusAllocator {
	    ParsedHeader(void);
	    ~ParsedHeader(void);
	const char	*name_start;
	int		name_len;
	const char	*value_start;
	int		value_len;
	int		alloc_mask; // Tracks updates to value.
	void *		mutex;
    };

    const char *	_header_text;
    int			_header_len;
    DtVirtArray<ParsedHeader *>	_parsed_headers;
    void *		_header_lock;

    void getTransportHeader(DtMailEnv & error,
			    const char * name,
			    DtMailValueSeq & value);

    DtMailBoolean matchName(const ParsedHeader &, const char *);

    void parseUnixFrom(DtMailEnv &,
		       const ParsedHeader &,
		       DtMailValueSeq & value);

    void parseUnixDate(DtMailEnv &,
		       const ParsedHeader &,
		       DtMailValueSeq & value);

    void makeValue(DtMailEnv &,
		   const ParsedHeader &,
		   DtMailValueSeq & value);

    void makeReply(DtMailEnv &,
		   const char * name,
		   DtMailValueSeq & value);

    DtMailBoolean metooAddr(DtMailValueAddress & addr,
			    DtMailAddressSeq & alts,
			    DtMailBoolean allnet);

    void parseHeaders(void);

    int lookupHeader(const char * name, DtMailBoolean real_only = DTM_FALSE);
    const char * mapName(const char * name);
};

class RFCBodyPart : public DtMail::BodyPart {
  public:
    RFCBodyPart(DtMailEnv &,
		DtMail::Message * parent,
		const char * start,
		const int len,
		RFCEnvelope * body_env);

    virtual void lockContents(DtMailEnv &, const DtMailLock);
    virtual void unlockContents(DtMailEnv &);

    virtual void getContents(DtMailEnv &,
			     const void ** contents,
			     unsigned long * length,
			     char ** type,
			     char ** name,
			     int * mode,
			     char ** description);
    
    virtual void setContents(DtMailEnv &,
			     const void * contents,
			     const unsigned long length,
			     const char * type,
			     const char * name,
			     const int mode,
			     const char * description);

    virtual void setFlag(DtMailEnv &,
			 DtMailBodyPartState);
    
    virtual void resetFlag(DtMailEnv &,
			   DtMailBodyPartState);
    
    virtual DtMailBoolean flagIsSet(DtMailEnv &,
				    DtMailBodyPartState);
    
    virtual time_t getDeleteTime(DtMailEnv &);

    virtual void getHeader(DtMailEnv &,
        const char * name,
        const DtMailBoolean abstract,
        DtMailValueSeq & value);

	// For CHARSET
    //---------------------------------------------------
    // These methods are duplicated in class BodyPart (also being implemented
    // in class Session) because RFCFormat and RFCBodyPart need to access
    // them.  Duplicating routines like what is being done here is a workaround
    // for an implementation bug/hole because there is no class where global
    // routines can be defined (and be accessed by any class).
    // class Session is not a proper place to put (global) methods because
    // not every class can get at Session.
    // RFCFormat accesses these routines through its private Session handle.
    // RFCBodyPart does not have a Session handle.  Hence the need to duplicate
    // the following routines.

    virtual int OpenLcxDb(void);
    virtual void DtXlateStdToOpLocale(char *op, char *std, char *dflt,
                 char **ret);
    virtual void DtXlateOpToStdLocale(char *op, char *opLoc, char **retLoc,
                 char **ret_retLang, char **retSet);
    virtual void DtXlateStdToOpCodeset(char *op, char *std, char *dflt,
                 char **ret);
    virtual void DtXlateMimeToIconv(const char *, const char *, const char *,
                 char **, char **);
    virtual void DtXlateLocaleToMime(const char *, const char *,
                  const char *, char **);
 
    virtual char *csToConvName(char *);
    virtual char *locToConvName();
    virtual char *targetConvName();
    virtual char *targetTagName();
    virtual int csConvert(char **, unsigned long &, int, char *, char *);
    //---------------------------------------------------
   
	// Implement in both MIMEBodyPart and V3BodyPart
    virtual char *csFromContentType(DtMailValueSeq & value) = 0;
	// End of For CHARSET

#ifdef DEAD_WOOD
    virtual DtMailChecksumState checksum(DtMailEnv &) = 0;
#endif /* DEAD_WOOD */

    // Methods below this point are specific to RFCBodyPart.
    //
    virtual char *writeBodyParts(char * buf) = 0;

    void adjustBodyPartsLocation(char * start);
  
    virtual int rfcSize(const char * boundary, DtMailBoolean &) = 0;

  protected:
    ~RFCBodyPart(void);
    DtMailBoolean isTerm(const char *); // Determines if we have an eol.

    void *		_body_lock;

    // _body_start includes any boundaries.
    const char *	_body_start;

    // _body_text is where the text of the body begins, after
    // boundaries, headers, and other uninteresting stuff.
    //
    const char *	_body_text;
    int			_body_len;
    RFCEnvelope *	_body_env;
    DtMailBoolean	_my_env; // True if we made the envelope.
    SafeScalar<int>	_dirty;

friend class RFCMessage;

    // The following entries are a computed cache. They
    // are built in a lazy fashion, based on the caller's
    // requests. These entries can be very expensive to
    // compute so lazy is important.
    //
    char *		_body;
    DtMailBoolean	_must_free_body;
    int			_body_decoded_len;
    char *		_body_type;

    // The sub classes implement these because they are the major difference
    // in the get/set contents methods.
    //
    virtual void getContentType(DtMailEnv &, char**) = 0;
    virtual void getDtType(DtMailEnv &) = 0;
    virtual void loadBody(DtMailEnv &) = 0;
    virtual char * getDescription(DtMailEnv &) = 0;
    virtual char * getName(DtMailEnv &) = 0;
    virtual void setName(DtMailEnv &, const char *) = 0;
    virtual unsigned long getLength(DtMailEnv &) = 0;
    virtual const void * getBody(DtMailEnv &);
};

class MIMEBodyPart : public RFCBodyPart {
  public:
    MIMEBodyPart(DtMailEnv &,
		 DtMail::Message * parent,
		 const char * start,
		 const int len,
		 RFCEnvelope * body_env);
    
    MIMEBodyPart(DtMailEnv &,
		 DtMail::Message * parent,
		 const char * start,
		 const char ** end,
		 const char * boundary);

#ifdef DEAD_WOOD
    virtual DtMailChecksumState checksum(DtMailEnv &);
#endif /* DEAD_WOOD */

    char *writeBodyParts(char * buf);

    int rfcSize(const char * boundary, DtMailBoolean &);

    // For CHARSET
    virtual char *csFromContentType(DtMailValueSeq & value);

    char * parameterValue(
			DtMailValueSeq & value,
			const char* parameter,
			DtMailBoolean isCaseSensitive);

  protected:
friend RFCMessage::~RFCMessage(void);
    ~MIMEBodyPart(void);

    void getContentType(DtMailEnv &, char **);
    void getDtType(DtMailEnv &);
    void loadBody(DtMailEnv &);
    char * getDescription(DtMailEnv &);
    char * getName(DtMailEnv &);
    void setName(DtMailEnv &, const char *);
    char * getNameHeaderVal(DtMailEnv &);
    unsigned long getLength(DtMailEnv &);
    const void * getBody(DtMailEnv &);
};

class V3BodyPart : public RFCBodyPart {
  public:
    V3BodyPart(DtMailEnv &,
	       DtMail::Message * parent,
	       const char * start,
	       const int len,
	       RFCEnvelope * body_env);

    V3BodyPart(DtMailEnv &,
	       DtMail::Message * parent,
	       const char * start,
	       const char ** end);

#ifdef DEAD_WOOD
    virtual DtMailChecksumState checksum(DtMailEnv &);
#endif /* DEAD_WOOD */

    char *writeBodyParts(char * buf);

    int rfcSize(const char * boundary, DtMailBoolean &);

	// For CHARSET
	virtual char *csFromContentType(DtMailValueSeq & value);

  protected:
friend RFCMessage::~RFCMessage(void);
    ~V3BodyPart(void);

    void getContentType(DtMailEnv &, char **);
    void getDtType(DtMailEnv &);
    void loadBody(DtMailEnv &);
    char * getDescription(DtMailEnv &);
    char * getName(DtMailEnv &);
    void setName(DtMailEnv &, const char *);
    unsigned long getLength(DtMailEnv &);
};

class RFCMailBox : public DtMail::MailBox
{
  public:

    RFCMailBox(DtMailEnv &,
	       DtMail::Session * session,
	       DtMailObjectSpace space,
	       void * arg,
	       DtMailCallback cb,
	       void * clientData,
	       const char * impl_name);
    
    virtual ~RFCMailBox(void);

    static void	 appendCB(DtMailEnv&, char *buf, int len, void *clientData);
    virtual void append(DtMailEnv &error, char *buf, int len);
    virtual void create(DtMailEnv & error, mode_t = DTMAIL_DEFAULT_CREATE_MODE);
    virtual void open(DtMailEnv &,
		      DtMailBoolean auto_create = DTM_TRUE,
		      int open_mode = DTMAIL_DEFAULT_OPEN_MODE,
		      mode_t create_mode = DTMAIL_DEFAULT_CREATE_MODE,
		      DtMailBoolean lock_flag = DTM_TRUE,
		      DtMailBoolean auto_parse = DTM_TRUE);
    virtual void lock();
    virtual void unlock();
    virtual void save();

    virtual void		 callCallback(DtMailCallbackOp, void *);
    virtual void		 disableMailRetrieval()
				     { _mr_allowed = DTM_FALSE; }
    virtual void		 enableMailRetrieval()
				     { _mr_allowed = DTM_TRUE; }
    virtual void		 checkForMail(
					DtMailEnv &, 
					const DtMailBoolean
						already_locked = DTM_FALSE); 
    virtual void		 clearMessageSummary(DtMailHeaderLine &);
    virtual void		 copyMessage(DtMailEnv &, DtMail::Message *);
    virtual void		 copyMailBox(DtMailEnv &, DtMail::MailBox *);
    virtual void		 createMailRetrievalAgent(char *passwd=NULL);
    virtual void		 deleteMailRetrievalAgent();
    virtual void		 expunge(DtMailEnv &);
    virtual DtMail::Message	*getFirstMessage(DtMailEnv &);
    virtual DtMailMessageHandle	 getFirstMessageSummary(
					DtMailEnv &,
					const DtMailHeaderRequest &,
					DtMailHeaderLine &);
    virtual DtMail::Message	*getNextMessage(DtMailEnv &, DtMail::Message *);
    virtual DtMailMessageHandle	 getNextMessageSummary(
   				 	DtMailEnv &,
				 	DtMailMessageHandle,
				 	const DtMailHeaderRequest &,
				 	DtMailHeaderLine &);
    virtual DtMail::Message	*getMessage(DtMailEnv &, DtMailMessageHandle);
    virtual void		 getMessageSummary(
					DtMailEnv &,
					DtMailMessageHandle,
					const DtMailHeaderRequest &,
					DtMailHeaderLine &);
    virtual const char		*impl(DtMailEnv & error);
#ifdef DEAD_WOOD
    virtual int			 messageCount(DtMailEnv &);
#endif /* DEAD_WOOD */
    virtual DtMail::Message	*newMessage(DtMailEnv &);
    virtual DtMailCallbackOp	 retrieveNewMail(DtMailEnv&);
    virtual void		 updateMailRetrievalPassword(char *passwd=NULL);

    // This is not used by any clients!
    struct MapRegion : public DtCPlusPlusAllocator
    {
	char *		map_region;
	unsigned long	map_size;
	char *		file_region; // Where the parsing begins.
	unsigned long	file_size;
	long		offset;
    };

    struct MessageCache : public DtCPlusPlusAllocator {
	RFCMessage	*message;
	DtMailBoolean	delete_pending;
    };

    // Methods below this point are specific to RFCMailBox.
    void	markDirty(const int delta);
    void	startAutoSave(DtMailEnv & error, DtMailBoolean start=DTM_TRUE);

  protected:

    struct NewMailData : public DtCPlusPlusAllocator
    {
        RFCMailBox *	self;
        Condition *	object_valid;
    };

    // Content-Type: message/partial support.
    struct _partialData
    {
        const char	* id;
        unsigned int	  number;
        unsigned int	  total;	// 0 == I do not know.
        RFCMessage     	* msg;
    };

    // Keep track of how many messages, and whether we've hit the
    // EOF. The number of messages is a condition variable because
    // we might have multiple threads trying to read the state
    // while the parsing thread is reading the state.
    //
    Condition			_at_eof;
    char			*_buffer;
    SafeScalar<int>		 _dirty;	// Write access/dirty state.
    DtMailBoolean		 _dot_lock_active;
    DtMailBoolean		 _errorLogging;	// Extra logging done??
    int				 _fd;
    SafeScalar<unsigned long>	 _file_size;
    const char			*_impl_name;	// Might be V3 or MIME.
    int				 _last_check;	// For polling only.
    time_t			 _last_poll;	// For polling only.
    int				 _links;
    DtMailBoolean		 _lock_flag;
    FileShare			*_lock_obj;
    DtMailBoolean		 _lockf_active;	// lockf() is being held
    char *			 _lockFileName;	// lock file name for mailbox
    DtMailBoolean		 _long_lock_active;
    Thread			 _mbox_daemon;
    void *			 _map_lock;
    DtVirtArray<MapRegion *>	 _mappings;
    unsigned long		 _object_signature;
    Condition			*_object_valid;
    DtMailBoolean		 _parsed;
    char			*_real_path;
    DtMailBoolean		 _mr_allowed;
    char			*_mra_command;
    DtMailServer		*_mra_server;
    char			*_mra_serverpw;
    DtVirtArray<MessageCache *>	 _msg_list;
    _partialData		**_partialList;
    unsigned int		 _partialListCount;
    struct stat			 _stinfo;
    NewMailData			*_thread_info;
    DtMailBoolean		 _tt_lock;
    char *			 _uniqueLockId;	// unique id for .lock files
    int				 _uniqueLockIdLength;
    DtMailBoolean		 _use_dot_lock;

//  void	CheckPointEvent(DtMailEnv & error);
    void	CheckPointEvent();
    void	ExpungeEvent(DtMailBoolean closing = DTM_FALSE);
    void	NewMailEvent(const DtMailBoolean already_locked = DTM_FALSE);
    static DtMailBoolean
		PollEntry(void *);
    static void	*ThreadNewMailEntry(void *);
    static void	*ThreadParseEntry(void *);

    RFCMessage	*_assemblePartial(DtMailEnv &error,RFCMessage *Message);
    DtMailBoolean
		_isPartial(DtMailEnv &error, RFCMessage	*message);
    DtMailBoolean
		addressIsMapped(void *addressToCheck);
#if defined(__linux__)
    void	alterPageMappingAdvice(MapRegion *map, int advice = 0);
#else
    void	alterPageMappingAdvice(
			MapRegion	*map,
			int		advice = MADV_SEQUENTIAL);
#endif
    int		createTemporaryMailboxFile(DtMailEnv &, char *tmp_name);
    void	checkLockFileOwnership(DtMailEnv &);
    void	dotDtmailLockFile(char *, int);
    void	dotDtmailLock(DtMailEnv &);
    void	dotDtmailUnlock(DtMailEnv &);
    void	dumpMaps(const char *);
    char	*generateLockFileName(void);
    char	*generateUniqueLockId(void);
    void	incorporate(
			DtMailEnv &,
			const DtMailBoolean already_locked = DTM_FALSE);
    time_t	linkLockFile(DtMailEnv &, char *tempLockFileName);
    void	lockFile(DtMailEnv &);
    void	lockNewMailboxFile(int new_fd);
    DTMBX_LONGLOCK
		longLock(DtMailEnv &);
    void	longUnlock(DtMailEnv &);
    int		lookupByMsg(RFCMessage * msg);
    void	mailboxAccessHide(char *prefix);
    void	mailboxAccessShow(time_t mtime, char *prefix);
    void	makeHeaderLine(DtMailEnv & error, 
			int slot,
			const DtMailHeaderRequest & request,
			DtMailHeaderLine &);
    int		mapFile(DtMailEnv & error,
			const DtMailBoolean already_locked = DTM_FALSE,
			mode_t create_mode = DTMAIL_DEFAULT_CREATE_MODE);
    MapRegion	*mapNewRegion(DtMailEnv &, int fd, unsigned long bytesWritten);
    int		nextNotDel(const int cur);
    void	openRealFile(DtMailEnv &error, int mode, mode_t create_mode);
    int		prevNotDel(const int cur);
    void	parseFile(DtMailEnv & error, int slot);
    off_t	realFileSize(DtMailEnv &error, struct stat *stat_buffer = NULL);
    void	transferLock(int old_fd, int new_fd);
    void	unlockOldMailboxFile(int old_fd);
    void	unlockFile(DtMailEnv &, int fd);
    void	waitForMsgs(int needed);
    void	writeMailBox(DtMailEnv&, DtMailBoolean);
    void	writeToDumpFile(const char* format, ...);
};

class RFCTransport : public DtMail::Transport {
  public:
    RFCTransport(DtMailEnv &, DtMail::Session *, DtMailStatusCallback, void *,
		 const char * impl);
    virtual ~RFCTransport(void);

    virtual DtMailOperationId submit(DtMailEnv &,
				     DtMail::Message *,
				     DtMailBoolean log_msg);
    
    static void arpaPhrase(const char * list, DtMailAddressSeq & tokens);

    // SendMsgDialog initiates a send.  It needs to set information
    // needed to exec sendmail by calling these initialization functions.
    // getSendmailReturnProc returns a handler so that SendMsgDialog
    // can use it in XtAppAddInput.
    virtual void initTransportData(int fds[2],
	SubProcessFinishedProc proc, void *ptr);
    virtual void *getSendmailReturnProc(void);


  protected:
    Condition *		_object_valid;
    char *		_impl;

    struct ThreadSimpleData : public DtCPlusPlusAllocator {
	RFCTransport *		self;
	const char *		to;
	const char *		cc;
	const char *		bcc;
	const char *		subject;
	const char *		text;
    };
    static void * ThreadSimpleEntry(void * arg);

    void format(DtMailEnv &, DtMail::Message * msg, DtMailBoolean log_msg);
    void formatSimple(DtMailEnv &,
		      const char * to,
		      const char * cc,
		      const char * bcc,
		      const char * subject,
		      const char * text);

    void deliver(DtMailEnv &,
		 DtMailAddressSeq &,
		 Buffer & headers,
		 Buffer & bodies,
		 DtMailBoolean log_msg,
		 int **log_files,
		 int & log_count);

    void launchSendmail(DtMailEnv & error,
			Buffer & headers, 
			Buffer & boides,
			char ** argv);

    char * concatValue(DtMailValueSeq &);
    void appendAddrs(DtMailAddressSeq & to, DtMailAddressSeq & from);
    void skinFiles(DtMailAddressSeq &);
    char * addrToString(DtMailAddressSeq &);
    int openLogFile(const char * path);
    void closeLogFiles(int * files, int file_cnt);

    // These functions are needed to exec sendmail
    static void childHandler(void);
    void signalRegister(void);
    int startSubprocess(DtMailEnv &error, char * cmd, 
	Buffer & headers, Buffer & bodies, char **argv);
    static void sendmailReturnProc(void);

    // Store info from SendMsgDialog
    // ptr to SendMsgDialog::SendmaliErrorProc
    SubProcessFinishedProc *_error_proc; 
    // a pointer to SendMsgDialog
    void *_smd; 			
    // get the status from the low and hi bytes returned from wait()
    inline static int lowByte  (int status) { return (status & 0377); }
    inline static int highByte (int status) { return (lowByte(status >> 8)); }
};

extern const char * RFCDeleteHeader;

class RFCValue : public DtMailValue {
  public:
    RFCValue(const char * str, int size, DtMail::Session *s);
    RFCValue(const char * str, int size);
    ~RFCValue(void);

    virtual operator const char *(void);

    virtual const char * operator= (const char *);

    virtual DtMailValueDate toDate(void);
#ifdef DEAD_WOOD
    virtual void fromDate(const DtMailValueDate &) { }
#endif /* DEAD_WOOD */

    virtual DtMailAddressSeq * toAddress(void);
#ifdef DEAD_WOOD
    virtual void fromAddress(const DtMailAddressSeq &) { }
#endif /* DEAD_WOOD */
    virtual const char * raw(void);

  protected:
	DtMail::Session *_session;
    char *	_decoded;

    void decodeValue(void);
};

#endif
