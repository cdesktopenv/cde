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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *+SNOTICE
 *
 *	$TOG: DtMailRc.C /main/9 1998/07/23 18:02:08 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993, 1995, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

/*
 * Format of the command description table.
 * The actual table is declared and initialized
 * in lex.c
 */

#include <stdlib.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pwd.h>
#include <DtMail/DtMail.hh>
#include <DtMail/DtMailError.hh>
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>
#include <EUSCompat.h>
#include "str_utils.h"

struct cmd {
    char *c_name;   /* Name of command */
    int (*c_func)(char **, DtMail::MailRc *);     /* Implementor of the command */
    void (*c_write)(const char * verbatim, char ** args, FILE * outf);
    short c_argtype;     /* Type of arglist (see below) */
    short c_msgflag;     /* Required flags of messages */
    short c_msgmask;     /* Relevant flags of messages */
};

struct var *DtMail::MailRc::variables[HSHSIZE]; /* Pointer to active var list */
#ifdef __osf__
typedef DtMail::MailRc::globals DtMailRc_globals;
DtMailRc_globals  DtMail::MailRc::glob;
#else
struct DtMail::MailRc::globals DtMail::MailRc::glob;
#endif
char *DtMail::MailRc::nullfield;
Boolean DtMail::MailRc::clearAliases;

#ifdef HAS_VFORK
  #define DTMAIL_FORK	vfork
#else
  #define DTMAIL_FORK	fork
#endif

#define MAXIMUM_PATH_LENGTH        2048

/* can't initialize unions */

#define c_minargs c_msgflag             /* Minimum argcount for RAWLIST */
#define c_maxargs c_msgmask             /* Max argcount for RAWLIST */
#define HASHSIZE 120


// defines needed by mail-parse
#define LINESIZE 10240        /* max readable line width */
#define MAXARGC  1024    /* Maximum list of raw strings */
#define equal(a, b)   (strcmp(a,b)==0)/* A nice function to string compare */
#define NONE  ((struct cmd *) 0) /* The nil pointer to command tab */

#define	CANY		0		/* Execute in send or receive mode */
#define	CRCV		1		/* Execute in receive mode only */
#define	CSEND		2		/* Execute in send mode only */
#define	CTTY		3		/* Execute if attached to a tty only */
#define	CNOTTY		4		/* Execute if not attached to a tty */

#define STRLIST  1       /* A pure string */
#define RAWLIST  2       /* Shell string list */
#define F 01000  /* Is a conditional command */
#define NOLIST   3       /* Just plain 0 */
#define CANY            0               /* Execute in send or receive mode */
#define NOSTR           ((char *) 0)    /* Null string pointer */
// #define NOFILE_DTMAIL          20      /* this define is here for      */
//                                 /* compatibility purposes only  */
//                                 /* and will be removed in a     */
//                                 /* later release                */




struct var {
    struct  var *v_link;            /* Forward link to next variable */
    char    *v_name;                /* The variable's name */
    char    *v_value;               /* And it's current value */
    int	    v_written;		    /* It has been written on this pass. */
};	

struct hash {
    struct hash *h_next;
    char *h_key;
    void *h_value;
    int h_written;
};

struct cmd cmdtab[] = {
{ "unset",      DtMail::MailRc::unset, DtMail::MailRc::wunset,     RAWLIST,        1,      1000, },
{ "alias",      DtMail::MailRc::group, DtMail::MailRc::wgroup, RAWLIST,	2,	1000, },
{ "group",      DtMail::MailRc::group, DtMail::MailRc::wgroup, RAWLIST,	2,	1000, },
{ "set",        DtMail::MailRc::set,   DtMail::MailRc::wset, RAWLIST,	1,	1000,},
{ "ignore",     DtMail::MailRc::igfield, DtMail::MailRc::wigfield, RAWLIST,        1,      1000, },
{ "source",     DtMail::MailRc::source, DtMail::MailRc::wsource, RAWLIST,        1,      1000, },
{ "if",		DtMail::MailRc::ifcmd, DtMail::MailRc::wifcmd, F|RAWLIST, 1, 1, },
{ "else",      	DtMail::MailRc::elsecmd, DtMail::MailRc::welsecmd, F|RAWLIST, 0, 0, },
{ "endif",     	DtMail::MailRc::endifcmd, DtMail::MailRc::wendifcmd, F|RAWLIST, 0, 0, },
{ "alternates", DtMail::MailRc::alternates, DtMail::MailRc::walternates, RAWLIST,        1,      1000, },
{ "clearaliases", DtMail::MailRc::clearaliases, DtMail::MailRc::wclearaliases, RAWLIST, 0, 0, },
{ 0, 0,		0,		0,	0,	0 }
};

#if defined(sun)
#define SYSTEM_MAILRC	"/etc/mail/mail.rc"
#elif defined(_AIX) || defined(__osf__) || defined(linux)
#define SYSTEM_MAILRC	"/usr/share/lib/Mail.rc"
#elif defined(USL) || defined(__hpux)
#define SYSTEM_MAILRC	"/usr/share/lib/mailx.rc"
#elif defined(__uxp__)
#define SYSTEM_MAILRC	"/etc/mail/mailx.rc"
#elif defined(CSRG_BASED)
#define SYSTEM_MAILRC	"/etc/mail.rc"
#endif

// constructor
DtMail::MailRc::MailRc(DtMailEnv &env, DtMail::Session *session)
{
  //char *line = NULL;
   
    session = session;


    env.clear();
    _parseError = DTME_NoError;

    input = NULL;

    init_globals();
    
    char * mail_rc = getenv("MAILRC");
    if (mail_rc) {
	_mailrc_name = strdup(mail_rc);
    }
    else {
	passwd pw;
	GetPasswordEntry(pw);
	
	_mailrc_name = (char *)malloc(MAXIMUM_PATH_LENGTH);
	
	strcpy(_mailrc_name, pw.pw_dir);
	strcat(_mailrc_name, "/.mailrc");
    }
    
    cond = CANY;
  
    char line[LINESIZE];
    int rval = 0;
    line[0] = '\0';
    
    if ((rval = this->load(_mailrc_name, line)) != 0) {
    	char *msg = new char[MAXIMUM_PATH_LENGTH + LINESIZE];
	if (rval == -1) {
      	    sprintf(msg, "error while reading \'%s\' in %s\n", line,
		        _mailrc_name);
	}
	else if (rval == -2) {
      	    sprintf(msg, "%s\n", line);
	}
	else if (rval == -3) {
      	    sprintf(msg, "error: no endif for conditional if statement while reading %s\n", _mailrc_name);
	    env.setError(DTME_ResourceParsingNoEndif, NULL);
	    _parseError = DTME_ResourceParsingNoEndif;
	}
		
      	env.logError(DTM_FALSE, "%s", msg);
	delete [] msg;
    }

    rval = 0;
    line[0] = '\0';

    if ((rval = this->load(SYSTEM_MAILRC, line)) != 0) {
        char *msg = new char[MAXIMUM_PATH_LENGTH + LINESIZE];

        if (rval == -1) {
      	    sprintf(msg, "error while reading \'%s\' in %s\n",
		    line, SYSTEM_MAILRC);
    	    env.logError(DTM_FALSE, "%s", msg);
        } else if (rval == -2) {
      	    /* Ignore error if the system logfile does not exist. */
        }
	delete [] msg;
    }
    
}

DtMail::MailRc::~MailRc(void)
{
    free(_mailrc_name);
}

// return true if in list
DtMailBoolean DtMail::MailRc::ignore(DtMailEnv &env, const char *name)
{
    
    char *ignore_list = (char *)MailRc::hm_test((struct hash **)glob.g_ignore, (char *)name);
    
    env.clear();
    
    if(ignore_list != NULL)  // we have a valid ignore list
      {
	  return DTM_TRUE;
      }
    else
      {
	  env.setError(DTME_NoObjectValue, NULL);
	  return DTM_FALSE;
      }
    
}

void
DtMail::MailRc::addIgnore(DtMailEnv & error, const char * name)
{
    error.clear();
    add_ignore((char *)name);
}

void
DtMail::MailRc::removeIgnore(DtMailEnv & error, const char * name)
{
    error.clear();
    if(MailRc::hm_test((struct hash **)glob.g_ignore, (char *)name)) {
	MailRc::hm_delete((struct hash **)glob.g_ignore, (char *)name);
    }
}

const char *DtMail::MailRc::getAlias(DtMailEnv &env, const char * name)
{
    
    char *return_value = (char *)hm_test((struct hash **)glob.g_alias, (char *)name);
    
    env.clear();
    
    if(return_value == NULL)
      {
	  env.setError(DTME_NoObjectValue, NULL);
	  return NULL;
      }
    else
      {
	  return return_value;
      }
    
}

void
DtMail::MailRc::setAlias(DtMailEnv & error,
			 const char * name,
			 const char * value)
{
    error.clear();

    if (hm_test((struct hash **)glob.g_alias, (char *)name)) {
	hm_delete((struct hash **)glob.g_alias, (char *)name);
    }

    add_alias((char *)name, (char *)value);
}

void
DtMail::MailRc::removeAlias(DtMailEnv & error,
			    const char * name)
{
    error.clear();

    if (hm_test((struct hash **)glob.g_alias, (char *)name)) {
	hm_delete((struct hash **)glob.g_alias, (char *)name);
    }
}

// 
// Adapted from uuencode and uudecode written by Ian Lance Taylor.
//
#define ENCRYPT(c) ((c) ? (((c)&077) + ' ' + 2) : ('`' + 2))
#define DECRYPT(c) ((((c) - ' ') & 077) - 2)

int 
DtMail::MailRc::encryptedLength(int length)
{
    // One for the length, one for the '\0', expansion factor of 4/3.
    int	encrypted_length = (2 + (4 * ((length+2)/3)));
    return encrypted_length;
}

void 
DtMail::MailRc::encryptValue(char *to, char *from, int buflen)
{
    int		ch, length;
    char	*p;
    int		i=0;
  
    memset(to, 0, buflen);

    length = strlen(from);
    to[i++] = ENCRYPT(length);

    for (p=from; length>0; length-=3, p+=3)
    {
        ch = *p >> 2;
        ch = ENCRYPT(ch);
        to[i++] = (char) ch;

        ch = ((*p << 4) & 060) | ((p[1] >> 4) & 017);
        ch = ENCRYPT(ch);
        to[i++] = (char) ch;

        ch = ((p[1] << 2) & 074) | ((p[2] >> 6) & 03);
        ch = ENCRYPT(ch);
        to[i++] = (char) ch;

        ch = p[2] & 077;
        ch = ENCRYPT(ch);   
	to[i++] = (char) ch;
    }
}

int
DtMail::MailRc::decryptValue(char *to, char *from, int buflen)
{
    int		ch, length;
    int		i=0;
    char	*p;
   
    memset(to, 0, buflen);
    p = from;

    length = DECRYPT(*p);
    if (length<=0) return 1;

    for (++p; length>0; p+=4, length-=3)
    {
        if (length>=3)
        {
            ch = DECRYPT(p[0]) << 2 | DECRYPT(p[1]) >> 4;
            to[i++] = (char) ch;
            ch = DECRYPT(p[1]) << 4 | DECRYPT(p[2]) >> 2;
            to[i++] = (char) ch;
            ch = DECRYPT(p[2]) << 6 | DECRYPT(p[3]);
            to[i++] = (char) ch;
        }
	else
	{
            if (length>=1)
	    {
	        ch = DECRYPT(p[0]) << 2 | DECRYPT(p[1]) >> 4;
	        to[i++] = (char) ch;
            }
            if (length>=2)
	    {
	        ch = DECRYPT(p[1]) << 4 | DECRYPT(p[2]) >> 2;
	        to[i++] = (char) ch;
            }
        }
    }
    return 0;
}

// if set with value return no error in env and set value
// if set with no value return no error in env and return empty string
// if not set, return error in env and leave value alone

void DtMail::MailRc::getValue(DtMailEnv &env, 
			      const char * var, 
			      const char ** value,
			      DtMailBoolean decrypt)
{
    char *get_result = mt_value((char *)var);
    
    *value = NULL;
    env.clear();
    
    if (get_result != NULL)
    {
	if (decrypt)
	{
	    int	length = encryptedLength(strlen(get_result));
            *value = (char*) malloc(length);
	    if (decryptValue((char*) *value, get_result, length))
	      strcpy((char*) *value, get_result);
	}
	else
          *value = strdup((char *)get_result);
    }
    else
      env.setError(DTME_NoObjectValue, NULL);
}

void
DtMail::MailRc::setValue(DtMailEnv & error,
			 const char * var,
			 const char * value,
			 DtMailBoolean encrypt)
{
    error.clear();

    if (encrypt && strlen(value))
    {
	int	length = encryptedLength(strlen(value));
        char	*encrypted = (char*) malloc(length);
	encryptValue(encrypted, (char*) value, length);
        mt_assign((char*) var, (char*) encrypted);
    }
    else
      mt_assign((char*) var, (char*) value);
}

void
DtMail::MailRc::removeValue(DtMailEnv & error,
			    const char * var)
{
    error.clear();

    mt_deassign((char *)var);
}

// return alternates list
const char * DtMail::MailRc::getAlternates(DtMailEnv &env)
{
    register int    i;
    register struct hash *h;
    struct hash **table = (struct hash **)glob.g_alternates;
    int len;
    
    env.clear();
    
    // we don't free this memory...
    alternate_list = NULL;
    
    if(table == NULL)
      {
	  env.setError(DTME_NoObjectValue, NULL);
	  return NULL;
      }
    
    i = HASHSIZE;
    while (--i >= 0) {
	
	h = *table++;
	while (h) {
	    len = strlen((const char*)h->h_key);
	    
	    if(alternate_list == NULL)
	      {
		  alternate_list = (char *)malloc(len + 1); // plus terminator
		  strcpy(alternate_list, (const char*)h->h_key);
	      }
	    else
	      {
		  len += strlen(alternate_list);
		  alternate_list = (char *)realloc(alternate_list, 
						   len + 2); // plus terminator
		  // and space
		  strcat(alternate_list, " "); // add space
		  strcat(alternate_list, (const char *)h->h_key);
		  
	      }
	    
	    h = h->h_next;
	}
	
    }
    
    return (alternate_list);
    
}

void
DtMail::MailRc::setAlternate(DtMailEnv & error, const char * alt)
{
    error.clear();

    add_alternates((char *)alt);
}

void
DtMail::MailRc::removeAlternate(DtMailEnv & error, const char * name)
{
    error.clear();

    if(MailRc::hm_test((struct hash**)glob.g_alternates, (char *)name)) {
	MailRc::hm_delete((struct hash **)glob.g_alternates, (char *)name);
    }
}

void
DtMail::MailRc::update(DtMailEnv & error)
{
    error.clear();
    
    // Generate a temporary file.
    char *tmp_mailrc = new char[MAXIMUM_PATH_LENGTH];
    strcpy(tmp_mailrc, _mailrc_name);
    strcat(tmp_mailrc, ".tmp");
    
    FILE * outf = fopen(tmp_mailrc, "w+");
    if (outf == NULL) {
	error.setError(DTME_ObjectCreationFailed);
	delete [] tmp_mailrc;
	return;
    }
    
    // Now open the mailrc for input.
    FILE * inf = fopen(_mailrc_name, "r");
    if (inf != NULL) {
	// Now we will read the input file, and copy it to the output,
	// based on type and changes to the mailrc.
	//
	updateByLine(inf, outf);
	
	fclose(inf);
    }
    
    // Now we need to make a final scan. This will cause new values
    // to be written and the written flag to be reset for the next
    // update.
    //
    mt_scan(outf);

    hm_scan((struct hash **)glob.g_alias, ngroup, outf);

    // Alternates and groups we will add to a list, and then
    // put them out in one batch.
    //
    DtVirtArray<char *>	value_list(32);

    hm_scan((struct hash **)glob.g_ignore, nigfield, &value_list);

    if (value_list.length()) {
	fwrite("ignore ", 1, 7, outf);
	while (value_list.length()) {
	    char * val = value_list[0];
	    fwrite(val, 1, strlen(val), outf);
	    fwrite(" ", 1, 1, outf);
	    value_list.remove(0);
	}
	fwrite("\n", 1, 1, outf);
    }

    hm_scan((struct hash **)glob.g_alternates, nalternates, &value_list);

    if (value_list.length()) {
	fwrite("alternates ", 1, 11, outf);
	while (value_list.length()) {
	    char * val = value_list[0];
	    fwrite(val, 1, strlen(val), outf);
	    fwrite(" ", 1, 1, outf);
	    value_list.remove(0);
	}
	fwrite("\n", 1, 1, outf);
    }

    fclose(outf);
    if (rename(tmp_mailrc, _mailrc_name)) {
	error.setError(DTME_ObjectAccessFailed);
        delete [] tmp_mailrc;
	return;
    }
    delete [] tmp_mailrc;
}

void DtMail::MailRc::getAliasList(hm_callback stuffing_func, void *client_data)
{
  hm_scan((struct hash **)glob.g_alias, stuffing_func, client_data);
}


DtVirtArray<char *> *DtMail::MailRc::getAliasList()
{	

  DtVirtArray<char *>	*value_list = NULL;

  value_list = new DtVirtArray<char *>(10);

  hm_scan((struct hash **)glob.g_alias, nalias, value_list);

  return value_list;
}

DtVirtArray<char *> *DtMail::MailRc::getIgnoreList()
{	

  DtVirtArray<char *>	*value_list = NULL;

  value_list = new DtVirtArray<char *>(10);

  hm_scan((struct hash **)glob.g_ignore, nignorelist, value_list);

  return value_list;
}

void
DtMail::MailRc::updateByLine(FILE * inf, FILE * outf)
{
    // We are going to keep two line buffers, unlike the first
    // pass parser. One is the line, with continuations, verbatim.
    // If nothing has changed, then we put this into the output
    // unmodified. The other is the line with continuations stripped
    // so we can finish parsing and analyzing the line.
    //
    char *verbatim = new char[LINESIZE];
    char *for_parser = new char[LINESIZE];
    char *linebuf = new char[LINESIZE];
    int  at_eof = 0;

    while(!at_eof) {
	verbatim[0] = 0;
	for_parser[0] = 0;

	while(1) {
	    if (readline(inf, linebuf) <= 0) {
		at_eof = 1;
		break;
	    }

	    int len = strlen(linebuf);
	    if (len == 0 || linebuf[len - 1] != '\\') {
		break;
	    }

	    strcat(verbatim, linebuf);
	    strcat(verbatim, "\n");

	    linebuf[len - 1] = ' ';
	    strcat(for_parser, linebuf);
	}
	if (at_eof) {
	    break;
	}

	strcat(verbatim, linebuf);
	strcat(verbatim, "\n");
	strcat(for_parser, linebuf);
	outputLine(verbatim, for_parser, outf);
    }

    delete [] verbatim;
    delete [] for_parser;
    delete [] linebuf;
}

void
DtMail::MailRc::outputLine(const char * verbatim,
			   const char * for_parser,
			   FILE * outf)
{
    char *arglist[MAXARGC];
    const char * start = for_parser;
    while(*start && isspace(*start)) {
	start++;
    }

    // If we have a comment, write it out and move on.
    //
    if (*start == '#') {
	fwrite(verbatim, 1, strlen(verbatim), outf);
	return;
    }

    char word[LINESIZE];

    // Pull off the command word.
    //
    char * cp2 = word;
    while (*start && !strchr(" \t0123456789$^.:/-+*'\"", *start))
	*cp2++ = *start++;
    *cp2 = '\0';

    if (equal(word, "")) {
	fwrite(verbatim, 1, strlen(verbatim), outf);
	return;
    }

    struct cmd * com = (struct cmd *)lex(word);
    if (com == NONE) {
	// We dont know the command, so just copy the line.
	//
	fwrite(verbatim, 1, strlen(verbatim), outf);
	return;
    }

    // We will simply rewrite conditionals.
    if ((com->c_argtype & F)) {
	fwrite(verbatim, 1, strlen(verbatim), outf);
	return;
    }

    int c;
    switch (com->c_argtype & ~(F)) {
      case STRLIST:
	/*
	 * Just the straight string, with
	 * leading blanks removed.
	 */
	while (strchr(" \t", *start)) {
	    start++;
	}
	com->c_write(verbatim, (char **)&start, outf);
	break;
	
      case RAWLIST:
	/*
	 * A vector of strings, in shell style.
	 */
	if ((c = getrawlist((char *)start, arglist,
			    sizeof arglist / sizeof *arglist)) < 0)
	    break;
	if (c < com->c_minargs) {
	    fprintf(stderr,"%s requires at least %d arg(s)\n",
		    com->c_name, com->c_minargs);
	    break;
	}
	if (c > com->c_maxargs) {
	    fprintf(stderr,"%s takes no more than %d arg(s)\n",
		    com->c_name, com->c_maxargs);
	    break;
	}
	com->c_write(verbatim, arglist, outf);
	freerawlist(arglist);
	break;
	
      case NOLIST:
	/*
	 * Just the constant zero, for exiting,
	 * eg.
	 */
	com->c_write(verbatim, NULL, outf);
	break;
	
      default:
	fprintf(stderr,"Unknown argtype %#x", com->c_argtype);
	fprintf(stderr, "NEED TO FIX THIS FOR DTMAIL!\n");
/* 		mt_done(1); */
    }

}

// init the global hash structure
void DtMail::MailRc::init_globals()
{
    glob.g_myname = NULL; 
    
    glob.g_ignore = this->hm_alloc(); 
    glob.g_retain = this->hm_alloc(); 
    glob.g_alias = this->hm_alloc(); 
    glob.g_alternates = this->hm_alloc(); 
    ssp = -1;  
    
    nullfield = new char[3];
    
    strcpy(nullfield, "");
    
    alternate_list = NULL;
    
}

// load a "user definintion" file
int
DtMail::MailRc::load(char *name, char* line)
{
    register FILE *in, *oldin;
    int ret=0;
    
    if ((in = fopen(name, "r")) == NULL) {
        sprintf(line, "can not open file %s\n", name);
	return(-2);
    }
    line[0] = '\0';
    oldin = input;
    input = in;
    sourcing = 0;
    if ((ret = commands(line)) != 0) {
    	input = oldin;
    	fclose(in);
	return ret;
    }
    input = oldin;
    fclose(in);
    return ret;
}

/*
 * Interpret user commands one by one.  If standard input is not a tty,
 * print no prompt.
 */

int
DtMail::MailRc::commands(char* iline)
{
    register int n;
    char *linebuf = new char[LINESIZE];
    char *line = new char[LINESIZE];
    
    for (;;) {
	/*
	 * Read a line of commands from the current input
	 * and handle end of file specially.
	 */
	
	n = 0;
	linebuf[0] = '\0';
	for (;;) {
	    if (readline(input, line) <= 0) {
		if (n != 0)
		    break;
		if (sourcing) {
		    unstack();
		    goto more;
		}
    		// Conditional if statement with no corresponding endif 
    		if (cond != CANY)
		{
		    delete [] linebuf;
		    delete [] line;
	   	    return(-3);
		}
		delete [] linebuf;
		delete [] line;
		return 0;
	    }
	    if ((n = strlen(line)) == 0)
		break;
	    n--;
	    if (line[n] != '\\')
		break;
	    line[n++] = ' ';
	    if (n > LINESIZE - strlen(linebuf) - 1)
		break;
	    strcat(linebuf, line);
	}
	n = LINESIZE - strlen(linebuf) - 1;
	if (strlen(line) > n) {
	    fprintf(stderr,
		    "Line plus continuation line too long:\n\t%s\n\nplus\n\t%s\n",
		    linebuf, line);
	    if (sourcing) {
		unstack();
		goto more;
	    }
            // Conditional if statement with no corresponding endif 
            if (cond != CANY)
	    {
	        delete [] linebuf;
	        delete [] line;
		return(-3);
	    }
	    delete [] linebuf;
	    delete [] line;
	    return 0;
	}
	strncat(linebuf, line, n);
	if (execute(linebuf)) {
		strncpy(iline, linebuf, LINESIZE);
		iline[LINESIZE-1] = '\0';
	    delete [] linebuf;
	    delete [] line;
	    return(-1);
	}
      more:		;
    }
    delete [] linebuf;
    delete [] line;
}

/*
 * Execute a single command.  If the command executed
 * is "quit," then return non-zero so that the caller
 * will know to return back to main, if he cares.
 * Contxt is non-zero if called while composing mail.
 */

int DtMail::MailRc::execute(char linebuf[])
{
    char word[LINESIZE];
    char *arglist[MAXARGC];
    struct cmd *com;
    char *cp, *cp2;
    int c;
//	int muvec[2];
    int e;
    int newcmd = 0;
    
    /*
     * Strip the white space away from the beginning
     * of the command, then scan out a word, which
     * consists of anything except digits and white space.
     *
     * Handle ! escapes differently to get the correct
     * lexical conventions.
     */
    
    cp = linebuf;
    while (*cp && strchr(" \t", *cp))
	cp++;
    /*
     * Throw away comment lines here.
     */
    if (*cp == '#') {
	if (*++cp != '-') {
	    return(0);
	}
	/* the special newcmd header -- "#-" */
	newcmd = 1;
	
	/* strip whitespace again */
	while (*++cp && strchr(" \t", *cp));
    }
    cp2 = word;
    while (*cp && !strchr(" \t0123456789$^.:/-+*'\"", *cp))
	*cp2++ = *cp++;
    *cp2 = '\0';
    
    /*
     * Look up the command; if not found, complain.
     * We ignore blank lines to eliminate confusion.
     */
    
    if (equal(word, ""))
	return(0);
    
    com = (struct cmd *)lex(word);
    if (com == NONE) {
	if (newcmd) {
	    /* this command is OK not to be found; that way
	     * we can extend the .mailrc file with new
	     * commands and not kill old mail and mailtool
	     * programs.
	     */
	    return(0);
	}
	fprintf(stderr,"Unknown command: \"%s\"\n", word);
	if (sourcing)
	    unstack();
	return(1);
    }
    
    /*
     * See if we should execute the command -- if a conditional
     * we always execute it, otherwise, check the state of cond.  
     */
    
    if ((com->c_argtype & F) == 0) {
	if (cond == CSEND || cond == CTTY )
	  {
	      return(0);
	  }
	
#ifdef undef
	if (cond == CRCV && !rcvmode || cond == CSEND && rcvmode ||
	    cond == CTTY && !intty || cond == CNOTTY && intty)
	  {
	      return(0);
	  }
#endif 
    }
    
    /*
     * Process the arguments to the command, depending
     * on the type he expects.  Default to an error.
     * If we are sourcing an interactive command, it's
     * an error.
     */
    
    e = 1;
    switch (com->c_argtype & ~(F)) {
      case STRLIST:
	/*
	 * Just the straight string, with
	 * leading blanks removed.
	 */
	while (strchr(" \t", *cp))
	    cp++;
	e = (*com->c_func)(&cp, this);
	break;
	
      case RAWLIST:
	/*
	 * A vector of strings, in shell style.
	 */
	if ((c = getrawlist(cp, arglist,
			    sizeof arglist / sizeof *arglist)) < 0)
	    break;
	if (c < com->c_minargs) {
	    fprintf(stderr,"%s requires at least %d arg(s)\n",
		    com->c_name, com->c_minargs);
	    break;
	}
	if (c > com->c_maxargs) {
	    fprintf(stderr,"%s takes no more than %d arg(s)\n",
		    com->c_name, com->c_maxargs);
	    break;
	}
	e = (*com->c_func)(arglist, this);
	freerawlist(arglist);
	break;
	
      case NOLIST:
	/*
	 * Just the constant zero, for exiting,
	 * eg.
	 */
	e = (*com->c_func)(0, this);
	break;
	
      default:
	fprintf(stderr,"Unknown argtype %#x", com->c_argtype);
	fprintf(stderr, "NEED TO FIX THIS FOR DTMAIL!\n");
/* 		mt_done(1); */
    }
    
    /*
     * Exit the current source file on
     * error.
     */
    
    if (e && sourcing)
	unstack();
    
    /* ZZZ:katin: should we return an error here? */
    return(e);
}

/*
 * Read up a line from the specified input into the line
 * buffer.  Return the number of characters read.  Do not
 * include the newline at the end.
 */

int
DtMail::MailRc::readline(FILE *ibuf, char *linebuf)
{
    register char *cp;
    register int c;
    int seennulls = 0;
    
    clearerr(ibuf);
    c = getc(ibuf);
    for (cp = linebuf; c != '\n' && c != EOF; c = getc(ibuf)) {
	if (c == 0) {
	    if (!seennulls) {
		fprintf(stderr,
			"Mail: ignoring NULL characters in mail\n");
		seennulls++;
	    }
	    continue;
	}
	if (cp - linebuf < LINESIZE-2)
	    *cp++ = c;
    }
    *cp = 0;
    if (c == EOF && cp == linebuf)
	return(0);
    return(cp - linebuf + 1);
}


/*
 * Pop the current input back to the previous level.
 * Update the "sourcing" flag as appropriate.
 */

void DtMail::MailRc::unstack()
{
    if (ssp < 0) {
	fprintf(stderr,"\"Source\" stack over-pop.\n");
	sourcing = 0;
	return;
    }
    fclose(input);
    if (cond != CANY)
	fprintf(stderr,"Unmatched \"if\"\n");
    cond = sstack[ssp].s_cond;
    input = sstack[ssp--].s_file;
    if (ssp < 0)
	sourcing = 0;
}
/*
 * Find the correct command in the command table corresponding
 * to the passed command "word"
 */

void *DtMail::MailRc::lex(char word[])
{
    register struct cmd *cp;

    for (cp = &cmdtab[0]; cp->c_name != NOSTR; cp++)
	if (DtMail::MailRc::isprefix(word, cp->c_name))
	    return(cp);
    return(NONE);
}

/*
 * Determine if as1 is a valid prefix of as2.
 * Return true if yep.
 */

int DtMail::MailRc::isprefix(char *as1, char *as2)
{
    register char *s1, *s2;
    
    s1 = as1;
    s2 = as2;
    while (*s1++ == *s2)
	if (*s2++ == '\0')
	    return(1);
    return(*--s1 == '\0');
}

/*
 * Scan out the list of string arguments, shell style
 * for a RAWLIST.
 */

int DtMail::MailRc::getrawlist(char line[], char **argv, int argc)
{
    register char **ap, *cp, *cp2;
    char linebuf[LINESIZE], quotec;
    register char **last;
    
    ap = argv;
    cp = line;
    last = argv + argc - 1;
    while (*cp != '\0') {
	while (*cp && strchr(" \t", *cp))
	    cp++;
	cp2 = linebuf;
	quotec = 0;
	while (*cp != '\0') {
	    if (quotec) {
		if (*cp == quotec) {
		    quotec=0;
		    cp++;
		} else
		    *cp2++ = *cp++;
	    } else {
		if (*cp && strchr(" \t", *cp))
		    break;
		if (*cp && strchr("'\"", *cp))
		    quotec = *cp++;
		else
		    *cp2++ = *cp++;
	    }
	}
	*cp2 = '\0';
	if (cp2 == linebuf)
	    break;
	if (ap >= last) {
	    fprintf(stderr,
		    "Too many elements in the list; excess discarded\n");
	    break;
	}
	*ap++ = strdup((char*)linebuf);
    }
    *ap = NOSTR;
    return(ap-argv);
}

void DtMail::MailRc::freerawlist(char **argv)
{
    
    while(*argv) {
	free((char*)*argv);
	argv++;
    }
}

/*
 * Get the value of a variable and return it.
 * Look in the environment if its not available locally.
 */

char *DtMail::MailRc::mt_value(char name[])
{
    register struct var *vp;
    register char *cp;
//        extern char *getenv();
    
    if ((vp = lookup(name, (struct var **)this->variables)) == (struct var *)NULL)
	cp = getenv(name);
    else
	cp = vp->v_value;
    return (cp);
}

/*
 * Locate a variable and return its variable
 * node.
 */
struct var *DtMail::MailRc::lookup(char *name, struct var **hasharray)
{
    register struct var *vp;
    register int h;
    
    h = hash(name);
    for (vp = hasharray[h]; vp != (struct var *)NULL; vp = vp->v_link)
	if (strcmp(vp->v_name, name) == 0)
	    return (vp);
    return ((struct var *)NULL);
}


/*
 * Put add users to a group.
 */

int DtMail::MailRc::group(char **argv, DtMail::MailRc *)
{
    int size;
    char *buf;
    char *s;
    char **ap;
    
    /*
     * Insert names from the command list into the group.
     * Who cares if there are duplicates?  They get tossed
     * later anyway.
     */
    
    /* compute the size of the buffer */
    size = 0;
    for (ap = argv+1; *ap != NOSTR; ap++) {
	size += strlen(*ap) +1;
    }
    buf = (char *)malloc(size);
    s = buf;
    for (ap = argv+1; *ap != NOSTR; ap++) {
	strcpy(s, *ap);
	s += strlen(s);
	*s++ = ' ';
    }
    *--s = '\0';
    
    MailRc::add_alias((char *)argv[0], (char *)buf);
    free(buf);
    return(0);
}

void
DtMail::MailRc::wgroup(const char * verbatim, char ** argv, FILE * outf)
{
    int size = 0;
    char * buf;
    char * s;
    char ** ap;

    for (ap = argv+1; *ap != NOSTR; ap++) {
	size += strlen(*ap) +1;
    }
    buf = (char *)malloc(size);
    s = buf;
    for (ap = argv+1; *ap != NOSTR; ap++) {
	strcpy(s, *ap);
	s += strlen(s);
	*s++ = ' ';
    }
    *--s = '\0';

    char * cur = (char *)hm_test((struct hash **)glob.g_alias, argv[0]);
    if (!cur || (!clearAliases && hm_ismarked((struct hash **)glob.g_alias, 
			argv[0]))) {
	// Its been removed or written. Dont write it to the file.
	free(buf);
	return;
    }

    if (strcmp(cur, buf) == 0) {
	// It hasnt changed. Write the original to preserve spacing.
	//
	fwrite(verbatim, 1, strlen(verbatim), outf);
    }
    else {
	// It has changed. Create a new alias.
	//
	fwrite("alias ", 1, 6, outf);
	fwrite(argv[0], 1, strlen(argv[0]), outf);
	fwrite(" ", 1, 1, outf);
	fwrite(cur, 1, strlen(cur), outf);
	fwrite("\n", 1, 1, outf);
    }

    hm_mark((struct hash **)glob.g_alias, argv[0]);
}

void
DtMail::MailRc::ngroup(char * key, void * data, void * client_data)
{
    char * value = (char *)data;
    FILE * outf = (FILE *)client_data;

    fwrite("alias ", 1, 6, outf);
    fwrite(key, 1, strlen(key), outf);
    fwrite(" ", 1, 1, outf);
    fwrite(value, 1, strlen(value), outf);
    fwrite("\n", 1, 1, outf);
}

void
DtMail::MailRc::nalias(char * key, void * data, void * client_data)
{
    DtVirtArray<char *> *value_list = (DtVirtArray<char *> *)client_data;
    char *new_alias = NULL;
    char *white_space = NULL;
    int m_size = 0;
    int  i, num_spaces = 0;
    int key_len = strlen(key);
    // figure out whitespace for formatting
    // assume 13 for normal sized alias name

    if(key_len < 13)  // need to add spaces
      {
	num_spaces = 13 - key_len;

	white_space = (char *)malloc(num_spaces + 1);
	white_space[0] = '\0';

	for(i = 0; i < num_spaces; i++)
	  white_space[i] = ' ';

	white_space[num_spaces] = '\0';

//	  strcat(white_space, " ");

	/* make an alias string */
	m_size = key_len + strlen((char *)white_space)
		 + strlen((char *)data) + strlen(" = ") + 1;
	new_alias = (char *)malloc(m_size);
    
	sprintf(new_alias, "%s%s = %s",key, white_space, data);

      }
    else
      {
		/* make an alias string */
	m_size = key_len + strlen((char *)data) + strlen(" = ") + 1;
	new_alias = (char *)malloc(m_size);
    
	sprintf(new_alias, "%s = %s",key, data);

      }
    
    value_list->append(new_alias);

}

void
DtMail::MailRc::nignorelist(char * key, void * data, void * client_data)
{
    data = data;
    DtVirtArray<char *> *value_list = (DtVirtArray<char *> *)client_data;
    char *new_ignore = NULL;

    new_ignore = (char *)malloc(strlen((char *)key) + 2);

    sprintf(new_ignore, "%s", key);

    value_list->append(new_ignore);

}


/*
 * Set or display a variable value.  Syntax is similar to that
 * of csh.
 */

int DtMail::MailRc::set(char **arglist, DtMail::MailRc *)
{
    register char *cp, *cp2;
    char varbuf[LINESIZE], **ap;
    int errs;
    
    errs = 0;
    for (ap = arglist; *ap != NOSTR; ap++) {
	cp = *ap;
	cp2 = varbuf;
	while (*cp != '=' && *cp != '\0')
	    *cp2++ = *cp++;
	*cp2 = '\0';
	if (*cp == '\0')
	    cp = "";
	else
	    cp++;
	if (equal(varbuf, "")) {
	    fprintf(stderr,"Non-null variable name required\n");
	    errs++;
	    continue;
	}
	MailRc::mt_assign(varbuf, cp);
    }
    return(errs);
}

void
DtMail::MailRc::wset(const char * verbatim,
		     char ** arglist,
		     FILE * outf)
{
    char varbuf[LINESIZE];
    char *cp, *cp2, **ap;

    for (ap = arglist; *ap != NOSTR; ap++) {
	cp = *ap;
	cp2 = varbuf;
	while (*cp != '=' && *cp != '\0')
	    *cp2++ = *cp++;
	*cp2 = '\0';
	if (*cp == '\0')
	    cp = "";
	else
	    cp++;
	if (equal(varbuf, "")) {
	    continue;
	}

	// Lookup the current value, and see if we should rewrite this
	// variable.
	//
	struct var * vp = lookup(varbuf, (struct var **)variables);
	if (vp == NULL || vp->v_written) {
	    // If the original input line was set novar then just write
	    // it out again. We can not easily track duplicates here.
	    //
	    if (varbuf[0] == 'n' && varbuf[1] == 'o') {
		fwrite(verbatim, 1, strlen(verbatim), outf);
	    }

	    // This variable has been unassigned or previously written.
	    // Remove it from the file, by not writing it.
	    //
	    continue;
	}

	// Compare the values. If equal, then write the line verbatim to
	// preserve the users original spacing and quoting.
	//
	if (strcmp(cp, vp->v_value) == 0) {
	    fwrite(verbatim, 1, strlen(verbatim), outf);
	    vp->v_written = 1;
	}
	else {
	    // Write the variable, with a new value.
	    //
	    fwrite("set ", 1, 4, outf);
	    fwrite(varbuf, 1, strlen(varbuf), outf);
	    if (strlen(vp->v_value) > 0) {
		fwrite("='", 1, 2, outf);
		fwrite(vp->v_value, 1, strlen(vp->v_value), outf);
		fwrite("'", 1, 1, outf);
	    }
	    fwrite("\n", 1, 1, outf);
	    vp->v_written = 1;
	}
    }
}

/*
 * Unset a bunch of variable values.
 */

int DtMail::MailRc::unset(char **arglist, DtMail::MailRc *)
{
        register char **ap;

        for (ap = arglist; *ap != NOSTR; ap++)
                (void) MailRc::mt_deassign(*ap);
        return(0);
}

void
DtMail::MailRc::wunset(const char * verbatim,
		     char ** arglist,
		     FILE * outf)
{
  arglist = arglist;
  if(verbatim != NULL && outf != NULL)
    fwrite(verbatim, 1, strlen(verbatim), outf);
}

/*
 * Hash the passed string and return an index into
 * the variable or group hash table.
 */
int DtMail::MailRc::hash(char *name)
{
    register unsigned h;
    register char *cp;
    
    for (cp = name, h = 0; *cp; h = (h << 2) + *cp++)
	;
    return (h % HSHSIZE);
}

/* manage the alias list */
void DtMail::MailRc::add_alias(char *name, char *value)
{
    char *old;
    char *new_ptr;
    
    /* aliases to the same name get appended to the list */
    old = (char*)MailRc::hm_test((struct hash **)glob.g_alias, name);
    if (old) {
	
	int size;
	
	size = strlen(value) + strlen(old) + 2;
	new_ptr = (char *)malloc(size);
	sprintf(new_ptr, "%s %s", value, old);
	
	/* delete any old bindings for the name */
	MailRc::hm_delete((struct hash**)glob.g_alias, name);
	
	/* add the new binding */
	MailRc::hm_add((struct hash**)glob.g_alias, name, new_ptr, size);
	
	/* free up the temporary space */
	free(new_ptr);
    } else {
	/* add the new binding */
	MailRc::hm_add((struct hash**)glob.g_alias, name, value, strlen(value) +1);
    }
}

/*
 * Assign a value to a mail variable.
 */
void DtMail::MailRc::mt_assign(char *name,char * val)
{
    
    if (name[0]=='-')
	(void) mt_deassign(name+1);
    else if (name[0]=='n' && name[1]=='o')
	(void) mt_deassign(name+2);
    else if ((val[0] == 'n' || val[0] == 'N') &&
	     (val[1] == 'o' || val[1] == 'O') && val[2] == '\0') {
	(void) mt_deassign(name);
    } else mt_puthash(name, vcopy(val), MailRc::variables);
}

int DtMail::MailRc::mt_deassign(char *s)
{
    register struct var *vp, *vp2;
    register int h;
    
    if ((vp2 = lookup(s, MailRc::variables)) == (struct var *)NULL) {
	return (1);
    }
    h = hash(s);
    if (vp2 == MailRc::variables[h]) {
	MailRc::variables[h] = MailRc::variables[h]->v_link;
	vfree(vp2->v_name);
	vfree(vp2->v_value);
	free((char *)vp2);
	return (0);
    }
    for (vp = MailRc::variables[h]; vp->v_link != vp2; vp = vp->v_link)
	;
    vp->v_link = vp2->v_link;
    vfree(vp2->v_name);
    vfree(vp2->v_value);
    free((char *)vp2);
    return (0);
}

/*
 * associate val with name in hasharray
 */
void DtMail::MailRc::mt_puthash(char *name, char *val, struct var **hasharray)
{
    register struct var *vp;
    register int h;
    
    vp = lookup(name, hasharray);
    if (vp == (struct var *)NULL) {
	h = hash(name);
	vp = (struct var *) (calloc(sizeof *vp, 1));
	vp->v_name = vcopy(name);
	vp->v_link = hasharray[h];
	vp->v_written = 0;
	hasharray[h] = vp;
    } else
	vfree(vp->v_value);
    vp->v_value = val;
}

void
DtMail::MailRc::mt_scan(FILE * outf)
{
    for (int slot = 0; slot < HSHSIZE; slot++) {
	for (var * vp = MailRc::variables[slot]; vp != (struct var *)NULL; vp = vp->v_link) {
	    if (!vp->v_written) {
		fwrite("set ", 1, 4, outf);
		fwrite(vp->v_name, 1, strlen(vp->v_name), outf);
		if (strlen(vp->v_value)) {
		    fwrite("='", 1, 2, outf);
		    fwrite(vp->v_value, 1, strlen(vp->v_value), outf);
		    fwrite("'", 1, 1, outf);
		}
		fwrite("\n", 1, 1, outf);
	    }
	    vp->v_written = 0;
	}
    }
}

/*
 * Copy a variable value into permanent space.
 * Do not bother to alloc space for "".
 */
char *DtMail::MailRc::vcopy(char *str)
{
    
    if (strcmp(str, "") == 0)
	return ("");
    return (strdup(str));
}

/*
 * Free up a variable string.  We do not bother to allocate
 * strings whose value is "" since they are expected to be frequent.
 * Thus, we cannot free same!
 */
void DtMail::MailRc::vfree(char *cp)
{
    
    if (strcmp(cp, "") != 0)
	free(cp);
}

void *DtMail::MailRc::hm_alloc(void)
{
    struct hash **table;
    
    table = (struct hash**)malloc(sizeof (struct hash) * HASHSIZE);
    memset(table, '\0', sizeof (struct hash) * HASHSIZE);
    return (table);
}
void DtMail::MailRc::hm_add(struct hash **table, 
			    char *key, 
			    void *value, 
			    int size)
{
    int index;
    register struct hash *h;
    
    if (!table)
	return;
    
    index = hash_index(key);
    h = (struct hash *)malloc(sizeof (struct hash));
    h->h_next = table[index];
    h->h_written = 0;
    table[index] = h;
    h->h_key = strdup(key);
    if (size && value != NULL) {
	h->h_value = malloc(size);
	memcpy(h->h_value, value, size);
    } else {
	h->h_value = nullfield;
    }
}


void DtMail::MailRc::hm_delete(struct hash **table, char *key)
{
    register int index;
    register struct hash *h;
    register struct hash *old;
    
    if (!table)
	return;
    
    index = hash_index(key);
    old = NULL;
    h = table[index];
    while (h) {
	if (strcasecmp(h->h_key, key) == 0) {
	    /* found the match */
	    if (old == NULL)
		table[index] = h->h_next;
	    else
		old->h_next = h->h_next;
	    
	    free_hash(h);
	    break;
	}
	
	old = h;
	h = h->h_next;
    }
}


void *DtMail::MailRc::hm_test(struct hash **table, char *key)
{
    register struct hash *h;
    
    if (!table)
	return (NULL);
    
    h = table[hash_index(key)];
    
    while (h) {
	if (strcasecmp(h->h_key, key) == 0) {
	    /* found a match */
	    return (h->h_value);
	}
	
	h = h->h_next;
    }
    
    return (NULL);
}

void DtMail::MailRc::hm_mark(struct hash **table, char *key)
{
    register struct hash *h;
    
    if (!table)
	return;
    
    h = table[hash_index(key)];
    
    while (h) {
	if (strcasecmp(h->h_key, key) == 0) {
	    h->h_written = 1;
	    return;
	}
	
	h = h->h_next;
    }
}

int DtMail::MailRc::hm_ismarked(struct hash **table, char *key)
{
    register struct hash *h;
    
    if (!table) return 0;
    h = table[hash_index(key)];
    while (h)
    {
	if (strcasecmp(h->h_key, key) == 0) return(h->h_written);
	h = h->h_next;
    }

    return 0;
}

void
DtMail::MailRc::hm_scan(struct hash **table, hm_callback callback, void * client_data)
{
    for (int slot = 0; slot < HASHSIZE; slot++) {
	for (struct hash * h = table[slot]; h; h = h->h_next) {
	    if (!h->h_written) {
		callback(h->h_key, h->h_value, client_data);
	    }
	    h->h_written = 0;
	}
    }
}

int DtMail::MailRc::hash_index(char *key)
{
    register unsigned h;
    register char *s;
    register int c;
    
    s = key;
    h = 0;
    while (*s) {
	c = *s++;
	if (isupper(c)) {
	    c = tolower(c);
	}
	h = (h << 2) + c;
    }
    
    return (h % HASHSIZE);
}

void DtMail::MailRc::free_hash(struct hash *h)
{
    free(h->h_key);
    if (h->h_value != nullfield) {
	free(h->h_value);
    }
    free(h);
}

/*
 * Add the given header fields to the ignored list.
 * If no arguments, print the current list of ignored fields.
 */
int DtMail::MailRc::igfield(char *list[], DtMail::MailRc *)
{
    char **ap;
    
    for (ap = list; *ap != 0; ap++) {
//                DP(("igfield: adding %s\n", *ap));
	MailRc::add_ignore(*ap);
    }
    return(0);
}

void
DtMail::MailRc::wigfield(const char * verbatim, char ** list, FILE * outf)
{
    char ** ap;
    Boolean ignore_written = FALSE;

    // We need to see that every name in this line still exists. If not,
    // then we will make a second pass, rewriting the line with only the
    // valid ignores.
    //
    int good = 1;
    for (ap = list; *ap && good; ap++) {
	if (!hm_test((struct hash **)glob.g_ignore, *ap)) {
	    good = 0;
	    break;
	}
    }

    if (good) {
	fwrite(verbatim, 1, strlen(verbatim), outf);
	for (ap = list; *ap; ap++) {
	    hm_mark((struct hash **)glob.g_ignore, *ap);
	}
	return;
    }

    // Create a new ignore line, leaving out the ignores that have
    // been removed. Also, dont write any ignores that have been
    // previously written.
    //
    for (ap = list; *ap; ap++) {
	if (hm_test((struct hash **)glob.g_ignore, *ap) &&
	!hm_ismarked((struct hash **)glob.g_ignore, *ap)) {
		// Only write 'ignore' if there is something in the list
		if (!ignore_written) {
    			fwrite("ignore ", 1, 7, outf);
			ignore_written = TRUE;
		}
		fwrite(*ap, 1, strlen(*ap), outf);
		fwrite(" ", 1, 1, outf);
		hm_mark((struct hash **)glob.g_ignore, *ap);
	}
     }
     if (ignore_written)
     	fwrite("\n", 1, 1, outf);
}

void
DtMail::MailRc::nigfield(char * key, void *, void * client_data)
{
    DtVirtArray<char *> *value_list = (DtVirtArray<char *> *)client_data;

    value_list->append(key);
}

/* manage the retain/ignore list */
void DtMail::MailRc::add_ignore(char *name)
{
    if(! MailRc::hm_test((struct hash **)glob.g_ignore, name)) {
	/* name is not already there... */
	MailRc::hm_add((struct hash **)glob.g_ignore, name, NULL, 0);
    }
}


/*
 * Set the list of alternate names.
 */
int DtMail::MailRc::alternates(char **namelist, DtMail::MailRc *)
{
    while (*namelist != NULL)
	MailRc::add_alternates(*namelist++);
    return(0);
}

void
DtMail::MailRc::walternates(const char * verbatim, char ** list, FILE * outf)
{
    // This is like ignores. We need to make sure all of the alternates
    // on this command are still in the database.
    //
    char ** ap;
    int good = 1;
    for (ap = list; *ap && good; ap++) {
	if (!hm_test((struct hash **)glob.g_alternates, *ap)) {
	    good = 0;
	    break;
	}
    }

    if (good) {
	fwrite(verbatim, 1, strlen(verbatim), outf);
	for (ap = list; *ap; ap++) {
	    hm_mark((struct hash **)glob.g_alternates, *ap);
	}
	return;
    }

    // Write out the alternates that still exist and have not been
    // previously written.
    //
    Boolean written = FALSE;
    for (ap = list; *ap; ap++) {
	if (hm_test((struct hash **)glob.g_alternates, *ap) &&
	    !hm_ismarked((struct hash **)glob.g_alternates, *ap)) {
	    if (!written) {
	    	fwrite("alternates ", 1, 11, outf);
	  	written=TRUE;
	    }
	    fwrite(*ap, 1, strlen(*ap), outf);
	    fwrite(" ", 1, 1, outf);
	    hm_mark((struct hash **)glob.g_alternates, *ap);
	}
    }
}

void
DtMail::MailRc::nalternates(char * key, void *, void * client_data)
{
    DtVirtArray<char *> *value_list = (DtVirtArray<char *> *)client_data;

    value_list->append(key);
}

/* manage the alternate name list */
void DtMail::MailRc::add_alternates(char *name)
{
    if(! MailRc::hm_test((struct hash**)glob.g_alternates, name)) {
	/* name is not already there... */
	MailRc::hm_add((struct hash **)glob.g_alternates, name, NULL, 0);
    }
}

/*
 * Determine the current folder directory name.
 */
int
DtMail::MailRc::getfolderdir(char *name)
{
	char *folder;

	if ((folder = mt_value("folder")) == NOSTR)
		return(-1);
	if (*folder == '/')
		strcpy(name, folder);
	else
		sprintf(name, "%s/%s", mt_value("HOME"), folder);
	return(0);
}

/*
 * Take a file name, possibly with shell meta characters
 * in it and expand it by using "sh -c echo filename"
 * Return the file name as a dynamic string.
 */

char *
DtMail::MailRc::expand(char *name)
{
	char *xname = new char[LINESIZE];
	char *cmdbuf = new char[LINESIZE];
	char *str;
	register int pid, l;
	register char *cp, *Shell;
	int s, pivec[2];
	struct stat sbuf;

	if (name[0] == '+' && getfolderdir(cmdbuf) >= 0) {
		sprintf(xname, "%s/%s", cmdbuf, name + 1);
		str = expand(xname);
		delete [] xname;
		delete [] cmdbuf;
		return str;
	}
	if (!strpbrk(name, "~{[*?$`'\"\\")) {
		return(strdup(name));
	}
	if (pipe(pivec) < 0) {
		perror("pipe");
		delete [] xname;
		delete [] cmdbuf;
		return(strdup(name));
	}
	sprintf(cmdbuf, "echo %s", name);
	if ((pid = DTMAIL_FORK()) == 0) {
		Shell = mt_value("SHELL");
		if (Shell == NOSTR || *Shell=='\0')
			Shell = "/bin/sh";
		close(pivec[0]);
		close(1);
		dup(pivec[1]);
		close(pivec[1]);
		close(2);
		execlp(Shell, Shell, "-c", cmdbuf, (char *)0);
		_exit(1);
	}
	if (pid == -1) {
		perror("fork");
		close(pivec[0]);
		close(pivec[1]);
		delete [] xname;
		delete [] cmdbuf;
		return(NOSTR);
	}
	close(pivec[1]);
	l = read(pivec[0], xname, LINESIZE);
	close(pivec[0]);
	while (wait(&s) != pid);
		;
	s &= 0377;
	if (s != 0 && s != SIGPIPE) {
		fprintf(stderr, "Echo failed\n");
		goto err;
	}
	if (l < 0) {
		perror("read");
		goto err;
	}
	if (l == 0) {
		fprintf(stderr, "%s: No match\n", name);
		goto err;
	}
	if (l == LINESIZE) {
		fprintf(stderr, "Buffer overflow expanding %s\n", name);
		goto err;
	}
	xname[l] = 0;
	for (cp = &xname[l-1]; *cp == '\n' && cp > xname; cp--)
		;
	*++cp = '\0';
	if (strchr(xname, ' ') && stat(xname, &sbuf) < 0) {
		fprintf(stderr, "%s: Ambiguous\n", name);
		goto err;
	}

	delete [] xname;
	delete [] cmdbuf;
	return(strdup(xname));

err:
	fflush(stderr);
	delete [] xname;
	delete [] cmdbuf;
	return(NOSTR);
}

int
DtMail::MailRc::source(char **arglist, DtMail::MailRc *self)
{
  char *fname = arglist[0];
  FILE *fi;
  char *cp;

  /* if any of the three if test failed, return 0,
   * since we have not updated the input and stack pointer yet
   */
  if ((cp = self->expand(fname)) == NOSTR)
    return(0);

  if ((fi = fopen(cp, "r")) == NULL) {
    free(cp);
    return(0);
  }
  free(cp);

  if (self->ssp >= NOFILE - 2) {
    fclose(fi);
    return(0);
  }
  self->sstack[++(self->ssp)].s_file = self->input;
  self->sstack[self->ssp].s_cond = self->cond;
  self->cond = CANY;
  self->input = fi;
  self->sourcing++;

  return(0);
}

void
DtMail::MailRc::wsource(const char * verbatim, char ** arglist, FILE * outf)
{
  arglist = arglist;
  if(verbatim != NULL && outf != NULL)
    fwrite(verbatim, 1, strlen(verbatim), outf);
}

int
DtMail::MailRc::ifcmd(char **arglist, DtMail::MailRc *self)
{
	register char *cp;

	if (self->cond != CANY) {
		fprintf(stderr,"Illegal nested \"if\"\n");
		return(1);
	}
	self->cond = CANY;
	cp = arglist[0];
	switch (*cp) {
	case 'r': case 'R':
		self->cond = CRCV;
		break;

	case 's': case 'S':
		self->cond = CSEND;
		break;

	case 't': case 'T':
		self->cond = CTTY;
		break;

	default:
		fprintf(stderr,"Unrecognized if-keyword: \"%s\"\n",
			cp);
		return(1);
	}
	return(0);
}

void
DtMail::MailRc::wifcmd(const char * verbatim, char ** arglist, FILE * outf)
{
  arglist = arglist;
  if(verbatim != NULL && outf != NULL)
    fwrite(verbatim, 1, strlen(verbatim), outf);
}


int
DtMail::MailRc::elsecmd(char **, DtMail::MailRc *self)
{

	switch (self->cond) {
	case CANY:
		fprintf(stderr, "\"Else\" without matching \"if\"\n");
		return(1);

	case CSEND:
		self->cond = CRCV;
		break;

	case CRCV:
		self->cond = CSEND;
		break;

	case CTTY:
		self->cond = CNOTTY;
		break;

	case CNOTTY:
		self->cond = CTTY;
		break;

	default:
		fprintf(stderr,"invalid condition encountered\n");
		self->cond = CANY;
		break;
	}
	return(0);
}

void
DtMail::MailRc::welsecmd(const char * verbatim, char ** arglist, FILE * outf)
{
  arglist = arglist;
  if(verbatim != NULL && outf != NULL)
    fwrite(verbatim, 1, strlen(verbatim), outf);
}


int
DtMail::MailRc::endifcmd(char **, DtMail::MailRc *self)
{

	if (self->cond == CANY) {
		fprintf(stderr,"\"Endif\" without matching \"if\"\n");
		return(1);
	}
	self->cond = CANY;
	return(0);
}

void
DtMail::MailRc::wendifcmd(const char * verbatim, char ** arglist, FILE * outf)
{
  arglist = arglist;
  if(verbatim != NULL && outf != NULL)
    fwrite(verbatim, 1, strlen(verbatim), outf);
}

int
DtMail::MailRc::clearaliases(char **, DtMail::MailRc *)
{
	DtVirtArray<char *>   *value_list = NULL;
	DtMailEnv error;
	
  	value_list = new DtVirtArray<char *>(10);

  	hm_scan((struct hash **)glob.g_alias, nalias, value_list);

	while (value_list->length()) {
		char *buf, *val = (*value_list)[0];
		if ((buf = strchr(val, ' ')) != NULL)
			*buf = '\0';
		if (hm_test((struct hash **)glob.g_alias, val))
        		hm_delete((struct hash **)glob.g_alias, val);
		value_list->remove(0);
	}
	delete value_list;
	clearAliases = 1;
	return 0;
}
void
DtMail::MailRc::wclearaliases(const char *, char **, FILE *)
{
}
