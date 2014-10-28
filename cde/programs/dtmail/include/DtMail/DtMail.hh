/*
 *+SNOTICE
 *
 *
 *	$TOG: DtMail.hh /main/18 1998/11/10 17:02:07 mgreess $
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

#ifndef _DTMAIL_HH
#define _DTMAIL_HH

#include <stdio.h>
#include <stdarg.h>
#if defined(sun)
#include <iconv.h>
#endif
#include <sys/stat.h>
/*#include <nl_types.h>*/
#include <DtMail/DtMailError.hh>
#include <DtMail/DtMailProps.h>
#include <DtMail/DtMailTypes.h>
#include <DtMail/DtVirtArray.hh>
#include <DtMail/DtLanguages.hh>
#include <Tt/tttk.h>
#include <fcntl.h>

// Define a type for the handler used in RFCTransport::launchSendmail
typedef void SubProcessFinishedProc (int pid, int status, void *data);

//
// Gorp to get mailrc to work
//
#define HSHSIZE 40
#define MAILRC_NOFILE          20      /* this define is here for      */
                                /* compatibility purposes only  */
                                /* and will be removed in a     */
                                /* later release                */

#ifdef __osf__
    struct hash;
    struct var;
#endif

// enums cannot be declared inside of classes because the enum scope rules
// changed from V2 to V3 compilers causing incompatibilities
//
enum DtmFileLocality {
  Dtm_FL_UNKNOWN,		// cannot determine file locality
  Dtm_FL_LOCAL,			// file is local to this system (eg ufs)
  Dtm_FL_LOCAL_AND_REMOTE,	// file is remote with local copy (eg cachefs)
  Dtm_FL_REMOTE			// file is remote (e.g. nfs)
};

class DtMail {
  public:
    
    // Returns the minor code set by a DtMail routine. This method
    // should only be called if error._major is not NO_EXCEPTION.
    //
    //  Parameters:
    //	  error - The environment returned from any DtMail method.
    //
    //  Returns - The error code as defined in DtMailError.h
    //
    static DTMailError_t getMinorCode(DtMailEnv & error);
    
    // Returns the internationalized error text for an error. This
    // method should only be called if error._major is not NO_EXCEPTION.
    //
    // Parameters:
    //	error - The environment returned from any DtMail method.
    //
    // Returns - The error text string. This string is valid until
    //    error.clear() is called on the error.
    //
    static const char * getErrorString(DtMailEnv & error);

    // Determine locality of referenced path object
    // This is not part of the public interface, do not call
    // as a client of this class.
    static enum DtmFileLocality DetermineFileLocality(const char * path);

    class MailBox;
    class Message;
    class Transport;

    class MailRc;

    class Session : public DtCPlusPlusAllocator {
      public:
	    Session(DtMailEnv &, const char * app_name);
	    ~Session(void);
	    
	    const char ** enumerateImpls(DtMailEnv & error);
	    void setDefaultImpl(DtMailEnv & error, const char * impl);
	    const char * getDefaultImpl(DtMailEnv & error);
	    void queryImpl(DtMailEnv & error,
			   const char * impl,
			   const char * capability,
			   ...);
	    void queryImplV(DtMailEnv & error,
			    const char * impl,
			    const char * capability,
			    va_list args);

	    // Create a mail box object, after selecting an implementation.
	    MailBox * mailBoxConstruct(DtMailEnv & error,
				       DtMailObjectSpace space,
				       void * arg,
				       DtMailCallback open_callback,
				       void * client_data,
				       const char * impl = NULL);

	    Message * messageConstruct(DtMailEnv & error,
				       DtMailObjectSpace space,
				       void * arg,
				       DtMailCallback open_callback,
				       void * client_data,
				       const char * impl = NULL);

	    // Create a transport object after selecting an implementation.
	    Transport * transportConstruct(DtMailEnv & error,
					   const char * impl,
					   DtMailStatusCallback call_back,
					   void * client_data);

	    // Retrieve a copy of the MailRc handle.
	    MailRc * mailRc(DtMailEnv & error);

	    // The following methods set up the polling requirements for
	    // the DtMail library.
	    //
	    DtMailBoolean pollRequired(DtMailEnv & error);

	    int eventFileDesc(DtMailEnv & error);

	    void poll(DtMailEnv & error);

	    // The expandPath method will turn a relative path, with
	    // variable's into an absolute path, relative to the current
	    // network node.
	    //
	    char * expandPath(DtMailEnv & error, const char * path);
	    char * getRelativePath(DtMailEnv & error, const char * path);

	    // Accessors to session data.
	    const char * appName(void) const { return _app_name; }
	    char * ttChannel(void) const { return _tt_channel; }
	    int ttFile(void) const { return _tt_fd; }

	    // Meta factory type. This method gives access to the
	    // factory and the implementations specific query
	    // mechanisms.
	    typedef void * (*MetaImplFactory)(const char * operation);

	    // The setError method needs to be here so it can be accessed
	    // by the drivers.
	    //
	    void setError(DtMailEnv & error, const DTMailError_t);

	    // Add an event routine.
	    void addEventRoutine(DtMailEnv &,
				 DtMailEventFunc,
				 void * client_data,
				 time_t interval);

	    // Remove an event routine.
	    void removeEventRoutine(DtMailEnv &,
				    DtMailEventFunc,
				    void * client_data);

	    // Push some data into the event queue.
	    //
	    void writeEventData(DtMailEnv & error,
				const void * buf,
				const unsigned long size);

	    // These routines check the list to make sure the created
	    // object is still valid. This is used by the event handlers
	    // to make sure events don't appear for deleted objects.
	    //
	    DtMailBoolean validObjectKey(DtMailObjectKey);
    	    DtMailObjectKey newObjectKey(void);
	    void removeObjectKey(DtMailObjectKey);

	    // This method allows the client to register a busy callback.
	    // Any time the library is going to be busy for a while, it will
	    // call this handler to set the app state to busy, and unbusy
	    // the app when it is done. This is an optional interface.
	    //
	    typedef void (*BusyApplicationCallback)(DtMailEnv &error,
						    DtMailBusyState busy_state,
						    void * client_data);
	    void registerBusyCallback(DtMailEnv &,
				      BusyApplicationCallback,
				      void * client_data);

#ifdef DEAD_WOOD
	    void unregisterBusyCallback(DtMailEnv &);
#endif /* DEAD_WOOD */

	    void setBusyState(DtMailEnv &, DtMailBusyState busy_state);

	    void setAutoSaveFlag(DtMailBoolean flag);
	    DtMailBoolean getAutoSaveFlag();

	    // These methods allow disabling and enabling of group privileges
	    //
            typedef void (*DisableGroupPrivilegesCallback)(void * client_data);
            typedef void (*EnableGroupPrivilegesCallback)(void * client_data);
      
            void registerDisableGroupPrivilegesCallback(
					DisableGroupPrivilegesCallback,
					void * client_data);
            void registerEnableGroupPrivilegesCallback(
					EnableGroupPrivilegesCallback,
					void * client_data);
      
            void disableGroupPrivileges(void);
            void enableGroupPrivileges(void);
	
	    // This method allows the client to register a callback that
	    // can be called to return the last time() an interactive X
	    // event was processed.
	    // Any time the library is about to be busy for a while, it
	    // may call this handler to get the time() that the last
	    // interactive X event was processed. It was very recent, the
	    // library may wish to delay the busy event for a while.
            //
            typedef long (*LastInteractiveEventTimeCallback)(void * client_data);
      
            void registerLastInteractiveEventTimeCallback(
					LastInteractiveEventTimeCallback,
					void * client_data);
      
            long lastInteractiveEventTime(void);
      
		// For CHARSET
        int OpenLcxDb(void);
        void DtXlateStdToOpLocale(char *op, char *std, char *dflt,
                     char **ret);
        void DtXlateOpToStdLocale(char *op, char *opLoc, char **retLoc,
                     char **ret_retLang, char **retSet);
        void DtXlateStdToOpCodeset(char *op, char *std, char *dflt,
                     char **ret);
        void DtXlateMimeToIconv(const char *, const char *, const char *,
                     char **, char **);
        void DtXlateLocaleToMime(const char *, const char *,
                     const char *, char **);

        char *csToConvName(char *);
        char *locToConvName();
        char *targetConvName();
        char *targetTagName();
        char *targetTagName(char *);
		int csConvert(char **, unsigned long &, int, char *, char *);
        // End of For CHARSET

	  private:
	    unsigned long	_object_signature;
	    void		*_obj_mutex;
	    char *		_tt_channel;
	    int			_tt_fd;
	    int			_event_fd[2];
	    char		*_app_name;
	    MailRc *		_mail_rc;
	    DtMailBoolean	_canAutoSave;
	    
	    /* Implementation structure. */
	    struct Impls {
	      char		*impl_name;
	      MetaImplFactory	impl_meta_factory;
	      void		*impl_lib;
	    };
	    
	    Impls		*_impls;
	    const char	**_impl_names; // Used to return names from enumerate.
	    int		_num_impls;
	    int		_default_impl;
	    
	    struct EventRoutine : public DtCPlusPlusAllocator {
	      DtMailEventFunc	routine;
	      void *		client_data;
	      time_t		interval;
	      time_t		last_ran;
	    };

	    DtVirtArray<EventRoutine *>		_events;
	    DtVirtArray<DtMailObjectKey>	_valid_keys;
	    DtMailObjectKey			_cur_key;

	    void buildImplTable(DtMailEnv & error);
	    void buildTypeTable(DtMailEnv & error);
	    int lookupImpl(const char * impl);

	    BusyApplicationCallback	_busy_cb;
	    void *			_busy_cb_data;

	    DisableGroupPrivilegesCallback	_disableGroupPrivileges_cb;
	    void *				_disableGroupPrivileges_cb_data;

	    EnableGroupPrivilegesCallback	_enableGroupPrivileges_cb;
	    void *				_enableGroupPrivileges_cb_data;

	    LastInteractiveEventTimeCallback	_interactive_time_cb;
	    void *				_interactive_time_cb_data;
	};

//==================NEW MAILRC CLASS
    class MailRc {
	
      public:
	    
	    MailRc(DtMailEnv &, Session *);
	    
	    ~MailRc();
	    
	    int  encryptedLength(int length);
	    void encryptValue(char *to, char *from, int length);
	    int  decryptValue(char *to, char *from, int length);

	    void getValue(
			DtMailEnv &,
			const char * var, const char ** value,
			DtMailBoolean decrypt = DTM_FALSE);
	    void setValue(
			DtMailEnv &,
			const char * var, const char * value,
			DtMailBoolean encrypt = DTM_FALSE);
	    void removeValue(DtMailEnv &, const char * var);

	    const char * getAlias(DtMailEnv &, const char * name);
	    typedef void (*hm_callback)(char * key, void * value,
					void * client_data);
	    void getAliasList(hm_callback stuffing_func, void *client_data);
	    void setAlias(DtMailEnv &, const char * name, const char * value);
	    void removeAlias(DtMailEnv &, const char * name);
	    DtVirtArray<char *> *getAliasList();

	    DtMailBoolean ignore(DtMailEnv &, const char *name);
	    void addIgnore(DtMailEnv &, const char * name);
	    void removeIgnore(DtMailEnv &, const char * name);
	    DtVirtArray<char *> *getIgnoreList();

	    const char * getAlternates(DtMailEnv &);
	    void setAlternate(DtMailEnv &, const char * alt);
	    void removeAlternate(DtMailEnv &, const char * alt);

	    void update(DtMailEnv &);
	    
	    
	    // Methods below this line are not part of the public interface.
	    // They must be declared public due to implementation restrictions.
	    // DO NOT CALL THESE AS A CLIENT OF THIS CLASS.
	    //
	    int load(char *filename, char* line);
	    void init_globals();
	    static void add_alias(char *name, char *value);
	    static void mt_assign(char *name,char * val);
	    static int mt_deassign(char *s);
	    static void mt_puthash(
				char *name,
				char * val,
				struct var **hasharray);
	    static void mt_scan(FILE * outf);
	    static char *vcopy(char *str);
	    static int group(char **argv, DtMail::MailRc *);
	    static void wgroup(const char *, char **, FILE *);
	    static void ngroup(char * key, void * value, void * client_data);
	    static void nalias(char * key, void * data, void * client_data);
	    static void nignorelist(char *key, void *data, void *client_data);
	    static int unset(char **arglist, DtMail::MailRc *);
	    static void wunset(
				const char * verbatim,
				char ** arglist,
				FILE * outf);
	    static int set(char **arglist, DtMail::MailRc *);
	    static void wset(const char *, char **, FILE *);
            static int source(char **arglist, DtMail::MailRc *);
            static void wsource(const char *, char **, FILE *);
            static int ifcmd(char **arglist, DtMail::MailRc *);
            static void wifcmd(const char *, char **, FILE *);
            static int elsecmd(char **arglist, DtMail::MailRc *);
            static void welsecmd(const char *, char **, FILE *);
            static int endifcmd(char **arglist, DtMail::MailRc *);
            static void wendifcmd(const char *, char **, FILE *);
	    static int igfield(char **list, DtMail::MailRc *);
	    static void wigfield(const char *, char **, FILE *);
	    static void nigfield(char * key, void * value, void * client_data);
	    static int clearaliases(char **list, DtMail::MailRc *);
	    static void wclearaliases(const char *, char **, FILE *);

	    static void *hm_alloc();
	    static void *hm_test(struct hash **table, char *key);
	    static void hm_delete(struct hash **table, char *key);
	    static void hm_add(struct hash **table, 
			       char *key, 
			       void *value, 
			       int size);
	    static void hm_mark(struct hash **table, char * key);
	    static int hm_ismarked(struct hash **table, char * key);
	    static void hm_scan(
				struct hash **table,
				hm_callback,
				void * client_data);
	    static void free_hash(struct hash *h);
	    static int hash_index(char *key);
	    static void add_ignore(char *name);
	    static void add_alternates(char *name);
	    static int alternates(char **namelist, DtMail::MailRc *);
	    static void walternates(const char *, char **, FILE *);
	    static void nalternates(
				char * key,
				void * value,
				void * client_data);
	    
	    static char *nullfield;
	    static Boolean clearAliases;
	    
	    struct globals {
	    char *g_myname;
	    void *g_ignore;		/* hash list of ignored fields */
	    void *g_retain;		/* hash list of retained fields */
	    void *g_alias;		/* hash list of alias names */
	    void *g_alternates;	/* hash list of alternate names */
	    int g_nretained;	/* the number of retained fields */
	    };	
	    
	    static struct globals glob;
	    
	    /* Pointer to active var list */
	    static struct   var *variables[HSHSIZE]; 

            DTMailError_t getParseError(void) { return _parseError; }

	  protected:
	    void updateByLine(FILE * in, FILE * out);
	    void outputLine(
			const char * verbatim,
			const char * parseable,
			FILE * out);
	    int commands(char* line);
	    int execute(char linebuf[]);
	    int readline(FILE *ibuf, char *linebuf);
	    void unstack();
	    int isprefix(char *as1, char *as2);
	    void *lex(char word[]);
	    int getrawlist(char line[], char ** argv, int argc);
	    void freerawlist(char **argv);
	    char *mt_value(char name[]);
            char *expand(char *);
            int getfolderdir(char *);
	    static int hash(char *name);
	    static void vfree(char *cp);
	    
	  private:
	    
	    DTMailError_t			_parseError;
	    FILE *input;
	    int sourcing;
	    int cond;
	    int  ssp;               /* Top of file stack */
	    char *alternate_list;
	    char *_mailrc_name;
	    
	    static struct var *lookup(char *name, struct var **hasharray);
	    
	    
	    struct sstack {
	    FILE    *s_file;                /* File we were in. */
	    int     s_cond;                 /* Saved state of conditionals */
	    };
	    
	    struct sstack sstack[MAILRC_NOFILE];
	
	
    };

    
    class Envelope;
    class BodyPart;

    class Message : public DtCPlusPlusAllocator {
      public:
	    virtual ~Message(void);
	    
	    virtual Envelope * getEnvelope(DtMailEnv &) = 0;
	    
	    // This should only be used when you REALLY need to know
	    // how many body parts are present before actually traversing
	    // the body parts (i.e. like when writing a MIME format message).
	    virtual int getBodyCount(DtMailEnv &) = 0;
	    
	    virtual BodyPart * getFirstBodyPart(DtMailEnv &) = 0;
	    virtual BodyPart * getNextBodyPart(DtMailEnv &,
					       BodyPart * last) = 0;
	    
	    virtual BodyPart * newBodyPart(DtMailEnv &,
					   BodyPart * after) = 0;
	    
#ifdef DEAD_WOOD
	    virtual void newBodyPartOrder(DtMailEnv &,
					  BodyPart * new_order,
					  const int bodypart_count) = 0;
#endif /* DEAD_WOOD */

	    virtual void setFlag(DtMailEnv &,
				 const DtMailMessageState) = 0;

	    virtual void resetFlag(DtMailEnv &,
				   const DtMailMessageState) = 0;

	    virtual DtMailBoolean flagIsSet(DtMailEnv &,
					    const DtMailMessageState) = 0;

	    virtual time_t getDeleteTime(DtMailEnv &) = 0;

	    virtual void toBuffer(DtMailEnv & error, DtMailBuffer &) = 0;

	    virtual const char * impl(DtMailEnv & error) = 0;

	    // The mail box method is used by the components of
	    // the message object to determine which mail box the
	    // belong to, and in turn which session.
	    //
	    MailBox * mailBox(void);
	    Session * session(void);

	    
	  protected:
	    Message(DtMailEnv &,
		    MailBox * parent);
	    
	    MailBox *		_parent;
	    Session *		_session;
	    Envelope *		_envelope;
	    void *		_obj_mutex;
	};

    class Envelope : public DtCPlusPlusAllocator {
      public:
	    virtual DtMailHeaderHandle getFirstHeader(
						DtMailEnv &,
						char ** name,
						DtMailValueSeq & value) = 0;

	    virtual DtMailHeaderHandle getNextHeader(
	   					 DtMailEnv &,
						 DtMailHeaderHandle last,
						 char ** name,
						 DtMailValueSeq & value) = 0;

	    virtual void getHeader(DtMailEnv &,
				   const char * name,
				   const DtMailBoolean abstract,
				   DtMailValueSeq & value) = 0;

	    virtual void setHeaderSeq(DtMailEnv &, 
				      const char * header_name, 
				      const DtMailValueSeq & val) = 0;

	    // The last parameter is left to the client to provide
	    // because it can not be done in a type safe manner.
	    //
	    virtual void setHeader(DtMailEnv &, 
				   const char * header_name, 
				   const DtMailBoolean replace,
				   const char *) = 0;

	    virtual void removeHeader(DtMailEnv &,
				      const char * header_name) = 0;
    //
    // fix for the defect 177527
    // when a reply-to field is in a message headers, the reply-to is
    // going to be displayed as the send in RMW's msg list scrolled window
    // instead of the real sender. The following three public methods are
    // going to access _use_reply_to.
    //
    virtual void setUseReplyTo(void)  = 0;
    virtual void unsetUseReplyTo(void) = 0;
    virtual DtMailBoolean getUseReplyTo(void) = 0;


	  protected:
	    Envelope(DtMailEnv & error, Message * parent);

	friend Message::~Message(void);
	    virtual ~Envelope(void); // Only called from Message destructor.

	    void *	_obj_mutex;
	    Message *	_parent;
	};
    
    class BodyPart : public DtCPlusPlusAllocator {
      public:

	    virtual void lockContents(DtMailEnv &, const DtMailLock) = 0;
	    virtual void unlockContents(DtMailEnv &) = 0;

	    virtual void getContents(DtMailEnv &,
				     const void ** contents,
				     unsigned long * length,
				     char ** type,
				     char ** name,
				     int * mode,
				     char ** description) = 0;
	    
	    virtual void getContentType(DtMailEnv &,
				     char ** content_type) = 0;
	    
	    virtual void setContents(DtMailEnv &,
				     const void * contents,
				     const unsigned long length,
				     const char * type,
				     const char * name,
				     const int mode,
				     const char * description) = 0;

	    virtual void setFlag(DtMailEnv &,
				 DtMailBodyPartState) = 0;

	    virtual void resetFlag(DtMailEnv &,
				   DtMailBodyPartState) = 0;
	    
	    virtual DtMailBoolean flagIsSet(DtMailEnv &,
					    DtMailBodyPartState) = 0;

	    virtual time_t getDeleteTime(DtMailEnv &) = 0;

		virtual void getHeader(DtMailEnv &,
     		const char * name,
	    	const DtMailBoolean abstract,
	    	DtMailValueSeq & value) = 0;

        // For CHARSET
        //-------------------------------------------
        // These methods are duplicated in class BodyPart
	// (also being implemented in class Session) because
	// RFCFormat and RFCBodyPart need to access them. 
	// Duplicating routines like what is being done here is a workaround
	// for an implementation bug/hole because there is no class where global
	// routines can be defined (and be accessed by any class).
	// class Session is not a proper place to put (global) methods because
	// not every class can get at Session.
	// RFCFormat accesses these routines through its private Session handle.
	// RFCBodyPart does not have a Session handle. 
	// Hence the need to duplicate the following routines.

        virtual int OpenLcxDb(void) = 0;
        virtual void DtXlateStdToOpLocale(char *op, char *std, char *dflt,
                     char **ret) = 0;
        virtual void DtXlateOpToStdLocale(char *op, char *opLoc, char **retLoc,
                     char **ret_retLang, char **retSet) = 0;
        virtual void DtXlateMimeToIconv(const char *, const char *,
		     const char *, char **, char **) = 0;
        virtual void DtXlateLocaleToMime(const char *, const char *,
                     const char *, char **) = 0;
 
        virtual char *csToConvName(char *) = 0;
        virtual char *locToConvName() = 0;
        virtual char *targetConvName() = 0;
        virtual char *targetTagName() = 0;
        virtual int csConvert(char **, unsigned long &, int,
		    char *, char *) = 0;
        //-------------------------------------------

		virtual char *csFromContentType(DtMailValueSeq & value) = 0;
        // End of For CHARSET

#ifdef DEAD_WOOD
	    virtual DtMailChecksumState checksum(DtMailEnv &) = 0;
#endif /* DEAD_WOOD */

	  protected:
	    BodyPart(DtMailEnv &, Message * parent);

	friend Message::~Message(void);
	    virtual ~BodyPart(void); // Only called from Message destructor.

	    void *	_obj_mutex;
	    Message *	_parent;
	};

    
    // The MailBox encapsulates the concept of a mail box as a container.
    // This class should be used for manipulating mail containers.
    
    class MailBox : public DtCPlusPlusAllocator {
      public:
	    
	    // Create a MailBox. The constructor simply initializes the
	    // instance and sets the appropriate meta handlers for mail
	    // containers.
	    //
	    // Errors:
	    //   None.
	    //
	    MailBox(DtMailEnv &,
		    Session * session,
		    DtMailObjectSpace space,
		    void * arg,
		    DtMailCallback cb,
		    void * clientData);
	    
	    // Returns DTM_TRUE if the is mail box is writable and DTM_FALSE
	    // if the mail box is read only.
	    //
	    DtMailBoolean	mailBoxWritable(DtMailEnv &);

	    // Destroy a MailBox. The destructor will close the open mail
	    // container, and any embedded containers.
	    //
	    virtual ~MailBox(void);
	    
	    static void	appendCB(DtMailEnv&, char*, int, void *clientData);
            virtual void append(DtMailEnv &error, char *buf, int len) = 0;

	    // Create a named mail container in the file system. The
	    // container will be truncated if it already exists, otherwise
	    // it will be created. The mode in this case will be set to
	    // DTMAIL_DEFAULT_CREATE_MODE, the default for mail.
	    //
	    // Errors:
	    //  DTME_ObjectInvalid - The instance was not proprerly initialized.
	    //  DTME_BadArg - The file_name parameter was invalid.
	    //  DTME_ObjectInUse - This object already has an open container.
	    //  DTME_ObjectCreationFailed - A required object could not
	    //       be created.
	    //
	    virtual void create(DtMailEnv &,
				mode_t mode = DTMAIL_DEFAULT_CREATE_MODE) = 0;
	    
	    // Open a named mail container in the file system. The container
	    // must exist, and be readable (at least) by the user.
	    // If the container is writable by the user it will be opened
	    // for update.
	    //
	    // Parameters:
	    //  auto_create - Create the container if it doesn't exist, using
	    //        MailBox::create.
	    //
	    // Errors:
	    //  DTME_ObjectInvalid - The instance was not proprerly initialized.
	    //  DTME_BadArg - The file_name parameter was invalid.
	    //  DTME_ObjectInUse - This object already has an open container.
	    //  DTME_ObjectCreationFailed - A required object could not
	    //       be created.
	    //  DTME_NoSuchFile - The file doesn't exist & auto_create is false.
	    //  DTME_NotMailBox - The object is a container, but not a mail box.
	    //  DTME_NoMemory - A memory allocation failed.
	    //
	    virtual void open(DtMailEnv & error,
			      DtMailBoolean auto_create = DTM_TRUE,
			      int open_mode = DTMAIL_DEFAULT_OPEN_MODE,
			      mode_t create_mode = DTMAIL_DEFAULT_CREATE_MODE,
			      DtMailBoolean lock_flag = DTM_TRUE,
			      DtMailBoolean auto_parse = DTM_TRUE) = 0;

	    // (Un)locks an open mailbox.
	    virtual void lock() = 0;
	    virtual void unlock() = 0;
	    virtual void save() = 0;

#ifdef DEAD_WOOD
	    virtual int messageCount(DtMailEnv & error) = 0;
#endif /* DEAD_WOOD */

	    virtual DtMailMessageHandle getFirstMessageSummary(
					DtMailEnv & error,
					const DtMailHeaderRequest & header_list,
					DtMailHeaderLine & header_line) = 0;
	    
	    virtual DtMailMessageHandle getNextMessageSummary(
					DtMailEnv & error,
					DtMailMessageHandle last,
					const DtMailHeaderRequest & header_list,
					DtMailHeaderLine & header_line) = 0;

	    virtual void getMessageSummary(
					DtMailEnv & error,
					DtMailMessageHandle handle,
					const DtMailHeaderRequest &,
					DtMailHeaderLine &) = 0;

	    virtual void clearMessageSummary(
					DtMailHeaderLine &) = 0;

	    virtual Message * getMessage(
					DtMailEnv & error,
					DtMailMessageHandle message_handle) = 0;
	    
	    virtual Message * getFirstMessage(DtMailEnv &) = 0;
	    
	    virtual Message * getNextMessage(DtMailEnv &, Message * last) = 0;
	    
	    virtual Message * newMessage(DtMailEnv &) = 0;

	    virtual void copyMessage(DtMailEnv &, Message *) = 0;

	    virtual void copyMailBox(DtMailEnv &, MailBox *) = 0;

	    virtual void disableMailRetrieval() = 0;
	    virtual void enableMailRetrieval() = 0;
	    virtual void checkForMail(
			DtMailEnv &,
			const DtMailBoolean already_locked = DTM_FALSE) = 0;

    	    virtual void createMailRetrievalAgent(char *password = NULL) = 0;
    	    virtual void deleteMailRetrievalAgent() = 0;
	    virtual void updateMailRetrievalPassword(char *password = NULL) = 0;

	    virtual void expunge(DtMailEnv &) = 0;

	    virtual const char * impl(DtMailEnv & error) = 0;

	    // This method is typically used to propagate the session
	    // to embedded objects, such as Messages which may need to
	    // acquire the session to which it belongs.
	    //
	    virtual Session * session(void);
	    
	    virtual void callCallback(DtMailCallbackOp, void *) = 0;
	    virtual void startAutoSave(
				DtMailEnv & error,
				DtMailBoolean start=DTM_TRUE) = 0;
            typedef void (*err_func)(char *,Boolean,void *);
            void registerErrMsgFunc(err_func,void * client_data);
            void unregisterErrMsgFunc(DtMailEnv &);
            void showError(char *);

	    void hideAccessEvents(DtMailBoolean onoff)
			{_hide_access_events = onoff;}
             
    	    DtMailObjectKey getObjectKey(void)
			{return _key;}

          private:
            err_func                    _errmsg_func;
            void *			_err_data;


	  protected:
	    DtMailBoolean       _hide_access_events;

	    DtMailObjectKey	_key;
	    Session		*_session;
	    void		*_arg;
	    DtMailObjectSpace	_space;
	    DtMailCallback	_callback;
	    void		*_cb_data;
	    void		*_obj_mutex;
	    DtMailBoolean	_mail_box_writable;
	    mode_t		_default_mode;	// Default mode for file.
	};
    
    class Transport : public DtCPlusPlusAllocator {
      public:
	    virtual DtMailOperationId submit(
					DtMailEnv &,
					Message * msg,
					DtMailBoolean log_msg = DTM_FALSE) = 0;

	    // SendMsgDialog initiates a send.  It needs to set information
	    // needed to exec sendmail by calling these init functions.
	    // getSendmailReturnProc returns a handler so that SendMsgDialog
	    // can use it in XtAppAddInput.
	    virtual void initTransportData(int fds[2],
		SubProcessFinishedProc proc, void *ptr) = 0;
	    virtual void *getSendmailReturnProc(void) = 0;


	    virtual void callCallback(DtMailOperationId, void * arg);

	  protected:
	    Transport(DtMailEnv &, Session *, DtMailStatusCallback, void *);
	    virtual ~Transport(void);

	    DtMailObjectKey		_key;
	    Session *			_session;
	    DtMailStatusCallback	_callback;
	    void *			_cb_data;
	    void *			_obj_mutex;
	};

  private:
    
friend class MailBox;
friend class Message;
friend class Envelope;
friend class BodyPart;
friend class Transport;
friend class EmbeddedMessage;
friend class FileMessage;
friend class FileShare;
friend class Session;
    static void setError(
			Session &,
			DtMailEnv & error,
			DTMailError_t minor_code);
};

#if defined(sun)
template <typename T>
size_t iconv (iconv_t i, const T inbuf, size_t* inleft,
	       char** outbuf, size_t* outleft)
{
    return iconv(i, const_cast<T>(inbuf), inleft, outbuf, outleft);
};
#endif

#endif
